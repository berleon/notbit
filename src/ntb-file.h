#ifndef NTB_file_H
#define NTB_file_H

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


#include "ntb-buffer.h"

struct ntb_file_env;

struct ntb_file;

struct ntb_file_env *
ntb_file_env_new(uint8_t *key,
                 size_t key_length);

struct ntb_file *
ntb_file_open(struct ntb_file_env *env,
                    const char *filename,
                    const char *mode);

struct ntb_file *
ntb_file_create(struct ntb_file_env *config,
                       const char *filename);

int
ntb_file_read(struct ntb_file *cf,
                     struct ntb_buffer *plaintext);

bool
ntb_file_write(struct ntb_file *cf,
                      struct ntb_buffer *plaintext);

int
ntb_file_size(struct ntb_file *f);

void
ntb_file_close(struct ntb_file *cf);

void
ntb_file_env_free(struct ntb_file_env * env);

#endif
