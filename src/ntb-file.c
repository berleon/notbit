/*
 * Notbit - A Bitmessage client
 * Copyright (C) 2014  Leon Sixt
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "config.h"

#include <assert.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ntb-file.h"
#include "ntb-buffer.h"
#include "ntb-error.h"
#include "ntb-log.h"
#include "ntb-util.h"
#include "ntb-slice.h"

#define KEY_LENGH 256
#define IV_N_BITS 128
#define IV_LENGTH (IV_N_BITS / 8)
#define CIPHER (EVP_aes_256_cbc())
#define CIPHER_BLOCK_SIZE 256
#define FILE_BUFFER_SIZE 8096

struct ntb_file_env {
        struct ntb_slice_allocator file_allocator;
        const EVP_CIPHER *cipher;
        EVP_CIPHER_CTX *ctx;
        struct ntb_buffer key_buf;
        pthread_mutex_t mutex;
};
/*
 * An encrypted file is stored as:
 *   0          127 | 128
 *   |     iv       | encrypted data
 *
 */
struct ntb_file {
        FILE * file;
        char *filename;
        struct ntb_file_env *env;
        bool iv_is_valid;
        /* initialization vector */
        unsigned char iv[IV_LENGTH];
};

struct ntb_file_env *
ntb_file_env_new(uint8_t *key,
                 size_t key_length)
{
        struct ntb_file_env *env = ntb_alloc(sizeof *env);
        env->ctx = EVP_CIPHER_CTX_new();

        ntb_buffer_init(&env->key_buf);
        ntb_buffer_append(&env->key_buf, key, key_length);

        pthread_mutex_init(&env->mutex, NULL);
        ntb_slice_allocator_init(&env->file_allocator,
                                 sizeof (struct ntb_file),
                                 NTB_ALIGNOF(struct ntb_file));
        return env;;
}

static struct ntb_file *
ntb_file_new(struct ntb_file_env *env,
             const char *filename)
{
        struct ntb_file *cf = ntb_slice_alloc(&env->file_allocator);
        cf->filename = strdup(filename);
        cf->iv_is_valid = false;
        cf->env = env;
        return cf;
}
static void
set_random_iv(struct ntb_file *f)
{
        RAND_pseudo_bytes(f->iv, IV_N_BITS);
        f->iv_is_valid = true;
}
static bool
seek_behind_iv(struct ntb_file *f)
{
        // seek behind the initialization vector
        if (fseek(f->file, IV_LENGTH, SEEK_SET)) {
                ntb_log("Error file is to small %s: %s",
                        f->filename,
                        strerror(errno));
                fclose(f->file);
                unlink(f->filename);
                return false;
        }
        return true;
}
struct ntb_file *
ntb_file_open(struct ntb_file_env *env,
                    const char *filename,
                    const char *mode)
{
        struct ntb_file *cf;
        FILE *file;
        file = fopen(filename, mode);
        if (file == NULL) {
                ntb_log("Error opening %s: %s",
                        filename,
                        strerror(errno));
                return NULL;
        }

        cf = ntb_file_new(env, filename);
        cf->file = file;
        return cf;
}

struct ntb_file *
ntb_file_create(struct ntb_file_env *env,
                       const char *filename)
{
        struct ntb_file *f = ntb_file_new(env, filename);
        set_random_iv(f);

        f->file = fopen(filename, "wb");
        if (f->file == NULL) {
                ntb_log("Error opening %s: %s",
                        filename, strerror(errno));
                return NULL;
        }

        if (fwrite(&f->iv,sizeof(unsigned char), IV_LENGTH, f->file) != IV_LENGTH) {
                ntb_log("Error writing %s: %s",
                        filename, strerror(errno));
                fclose(f->file);
                unlink(filename);
                return NULL;
        }
        return f;
}

static bool
read_iv(struct ntb_file *f)
{
        if (fread(f->iv, sizeof(uint8_t), IV_LENGTH, f->file) != IV_LENGTH) {
            if (errno == 0)
                ntb_log("The object file %s is too short", f->filename);
            else
                ntb_log("Error reading %s: %s", f->filename, strerror(errno));
            return false;
        }
        f->iv_is_valid = true;
        return true;
}
static unsigned char *
get_iv(struct ntb_file *f) {
        if (! f->iv_is_valid) {
            read_iv(f);
        }
        return f->iv;
}
int
ntb_file_read(struct ntb_file *cf,
                     struct ntb_buffer *p)
{
        // p = plaintext
        // c = ciphertext

        int c_length;
        int n_bytes_decrypted;
        unsigned char c[FILE_BUFFER_SIZE];

        if(!EVP_DecryptInit_ex(cf->env->ctx, CIPHER, NULL,
                               cf->env->key_buf.data, get_iv(cf)))
            goto decrypt_error;

        if(!seek_behind_iv(cf)) goto decrypt_error;

        while((c_length = fread(c, sizeof(char), FILE_BUFFER_SIZE, cf->file)) != 0)
        {
                int max_new_size = p->length + c_length + CIPHER_BLOCK_SIZE;
                ntb_buffer_ensure_size(p, max_new_size);

                if(!EVP_DecryptUpdate(cf->env->ctx,
                                 p->data + p->length,
                                 &n_bytes_decrypted,
                                 c, c_length)) {
                    goto decrypt_error;
                }
                p->length += n_bytes_decrypted;
        }
        if(!EVP_DecryptFinal_ex(cf->env->ctx,
                                p->data + p->length,
                                &n_bytes_decrypted))
                goto decrypt_error;

        p->length += n_bytes_decrypted;

        EVP_CIPHER_CTX_cleanup(cf->env->ctx);

        return true;

decrypt_error:
        EVP_CIPHER_CTX_cleanup(cf->env->ctx);
        return false;
}
int
ntb_file_size(struct ntb_file *f)
{
        int pos, size;
        pos = ftell(f->file);
        fseek(f->file, 0L, SEEK_END);
        size = ftell(f->file);
        fseek(f->file, pos, SEEK_SET);
        return size;
}

bool
ntb_file_write(struct ntb_file *cf,
                      struct ntb_buffer *plaintext)
{
        struct ntb_buffer ciphertext;
        int tmplen = 0;
        assert(cf->iv_is_valid);
        ntb_buffer_init(&ciphertext);
        ntb_buffer_ensure_size(&ciphertext, plaintext->length + CIPHER_BLOCK_SIZE);

        if(!seek_behind_iv(cf)) goto encrypt_error;


        if(!EVP_EncryptInit_ex(cf->env->ctx, CIPHER, NULL,
                               cf->env->key_buf.data, cf->iv))
                goto encrypt_error;


        if(!EVP_EncryptUpdate(cf->env->ctx,
                              ciphertext.data, &tmplen,
                              plaintext->data, (int) plaintext->length))
                goto encrypt_error;
        ciphertext.length += tmplen;

        if(!EVP_EncryptFinal_ex(cf->env->ctx,
                                ciphertext.data + ciphertext.length,
                                &tmplen))
                goto encrypt_error;

        ciphertext.length += tmplen;

        if (fwrite(ciphertext.data, sizeof (char),
                   ciphertext.length, cf->file) != ciphertext.length) {
                ntb_log("Error writing %s: %s",
                        cf->filename,
                        strerror(errno));
                fclose(cf->file);
                unlink(cf->filename);
                goto encrypt_error;
        }

        EVP_CIPHER_CTX_cleanup(cf->env->ctx);
        ntb_buffer_destroy(&ciphertext);

        return true;

encrypt_error:
        ntb_buffer_destroy(&ciphertext);
        EVP_CIPHER_CTX_cleanup(cf->env->ctx);
        return false;
}

void
ntb_file_close(struct ntb_file *cf)
{
        fclose(cf->file);
        ntb_free(cf->filename);
        ntb_slice_free(&cf->env->file_allocator, cf);
}

void
ntb_file_env_free(struct ntb_file_env * env)
{
        ntb_slice_allocator_destroy(&env->file_allocator);
        EVP_CIPHER_CTX_free(env->ctx);
        ntb_buffer_destroy(&env->key_buf);
        pthread_mutex_destroy(&env->mutex);
        ntb_free(env);
}

