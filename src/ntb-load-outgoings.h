/*
 * Notbit - A Bitmessage client
 * Copyright (C) 2014  Neil Roberts
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

#ifndef NTB_LOAD_OUTGOINGS_H
#define NTB_LOAD_OUTGOINGS_H

#include <stdio.h>

#include "ntb-store.h"

/* This is a helper function for ntb_store. It's in its own file just
 * because it's a bit big */

typedef void
(* ntb_load_outgoings_func)(const struct ntb_store_outgoing *outgoing,
                            void *user_data);

void
ntb_load_outgoings(FILE *file,
                   ntb_load_outgoings_func func,
                   void *user_data);

#endif /* NTB_LOAD_OUTGOINGS_H */
