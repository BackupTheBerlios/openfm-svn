/*
 * OpenFM - Open Financial Manager
 * Copyright (C) 2006 Slava Semushin <php-coder at altlinux.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * $Id$
 *
 **/

/**
 * @file   common.h contains prototypes for common functions
 * @author Slava Semushin <php-coder at altlinux.ru>
 * @since  25.06.2006
 **/

#ifndef COMMON_H
#define COMMON_H

#include "config.h"

#ifdef NLS
   /* for textdomain()
    *     gettext()
    **/
   #include <libintl.h>

   /** Redefine gettext() functions to be more short. */
   #define _(str) gettext(str)
#else /* no NLS */
   #define _(str) str
#endif /* NLS */


int  is_string_confirm_to_format(const char *str, unsigned long lineno);
int  is_file_exist_and_regular(const char *filename, unsigned int verbose);
void add_record_to_file(const char *filename, const char *record, unsigned int verbose);


#endif /* COMMON_H */

