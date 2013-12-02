/*
 * Notbit - A Bitmessage client
 * Copyright (C) 2013  Neil Roberts
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

#ifndef NTB_PROTO_H
#define NTB_PROTO_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "ntb-error.h"
#include "ntb-buffer.h"
#include "ntb-netaddress.h"

extern struct ntb_error_domain
ntb_proto_error;

enum ntb_proto_error {
        NTB_PROTO_ERROR_PROTOCOL
};

enum ntb_proto_argument {
        NTB_PROTO_ARGUMENT_8,
        NTB_PROTO_ARGUMENT_16,
        NTB_PROTO_ARGUMENT_32,
        NTB_PROTO_ARGUMENT_64,
        NTB_PROTO_ARGUMENT_BOOL,
        NTB_PROTO_ARGUMENT_VAR_INT,
        NTB_PROTO_ARGUMENT_TIMESTAMP,
        NTB_PROTO_ARGUMENT_NETADDRESS,
        NTB_PROTO_ARGUMENT_VAR_STR,
        NTB_PROTO_ARGUMENT_END
};

#define NTB_PROTO_HEADER_SIZE (4 + 12 + 4 + 4)

#define NTB_PROTO_VERSION UINT32_C(3)

#define NTB_PROTO_NETWORK_NODE UINT64_C(1)
#define NTB_PROTO_SERVICES (NTB_PROTO_NETWORK_NODE)

void
ntb_proto_double_hash(const void *data,
                      int length,
                      uint8_t *hash);

void
ntb_proto_address_hash(const void *data,
                       int length,
                       uint8_t *hash);

uint16_t
ntb_proto_get_16(const uint8_t *p);

uint32_t
ntb_proto_get_32(const uint8_t *p);

uint64_t
ntb_proto_get_64(const uint8_t *p);

bool
ntb_proto_get_var_int(const uint8_t **p_ptr,
                      int *length_ptr,
                      uint64_t *result,
                      struct ntb_error **error);

static inline void
ntb_proto_add_8(struct ntb_buffer *buf,
                uint8_t value)
{
        ntb_buffer_append_c(buf, value);
}

static inline void
ntb_proto_add_16(struct ntb_buffer *buf,
                 uint16_t value)
{
        value = NTB_UINT16_TO_BE(value);
        ntb_buffer_append(buf, (uint8_t *) &value, sizeof value);
}

static inline void
ntb_proto_add_32(struct ntb_buffer *buf,
                 uint32_t value)
{
        value = NTB_UINT32_TO_BE(value);
        ntb_buffer_append(buf, (uint8_t *) &value, sizeof value);
}

static inline void
ntb_proto_add_64(struct ntb_buffer *buf,
                 uint64_t value)
{
        value = NTB_UINT64_TO_BE(value);
        ntb_buffer_append(buf, (uint8_t *) &value, sizeof value);
}

static inline void
ntb_proto_add_bool(struct ntb_buffer *buf,
                   int value)
{
        ntb_proto_add_8(buf, !!value);
}

void
ntb_proto_add_var_int(struct ntb_buffer *buf,
                      uint64_t value);

void
ntb_proto_add_timestamp(struct ntb_buffer *buf);

void
ntb_proto_add_netaddress(struct ntb_buffer *buf,
                         const struct ntb_netaddress *address);

void
ntb_proto_add_var_str(struct ntb_buffer *buf,
                      const char *str);

void
ntb_proto_add_command(struct ntb_buffer *buf,
                      const char *command,
                      ...);

void
ntb_proto_add_command_va_list(struct ntb_buffer *buf,
                              const char *command,
                              va_list ap);

#endif /* NTB_PROTO_H */
