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
 * @file common.h contains prototypes for common functions
 * @author Slava Semushin <php-coder at altlinux.ru>
 * @since 25.06.2006
 * @date  10.07.2006
 **/

#ifndef COMMON_H
#define COMMON_H


/* for textdomain()
 *     gettext()
 **/
#include <libintl.h>


/** Redefine gettext() functions to be more short. */
#define _(str) gettext(str)


/**
 * Test string for confirm to format.
 *
 * Functions do 11 tests under giving string. Right string looks like
 * this
 *
 * \c "sign|dd.mm.yyyy|amount|comment"
 *
 * and consist of four fields:
 *
 * -# sign -- should be '+' or '-' only. It is says to program: costs
 *    or profit.
 * -# date in format \c "dd.mm.yyyy"
 * -# amount of profit/costs (depends on first field)
 * -# comment
 *
 * @param str string which would be checked
 * @param lineno number of string, which will be printed if error
 *
 * @retval 0 successfully result
 * @retval 1 too small string
 * @retval 2 wrong first field of string
 * @retval 3 wrong separators for whole string
 * @retval 4 date consist of non digital symbols
 * @retval 5 wrong separators for date field
 * @retval 6 day of month is out of range
 * @retval 7 month is out of range
 * @retval 8 year equals to 0
 * @retval 9 day of month for leap year is out of range
 * @retval 10 day of month is out of range for that month
 * @retval 11 date in future
 *
 * @return 0 or digit more then 0 if one of test was failed
 **/
int is_string_confirm_to_format(const char *str, unsigned long lineno);


/**
 * Examinate file: he should exist and be regular.
 *
 * Function tries to get statistics about file and determine that it is a
 * regular file. Used for verify user-defined filename.
 *
 * @param filename name of file
 * @param verbose level of verbose
 *
 * @retval 0 file exists and is regular
 * @retval 1 stat(2) returns error. The file may not exist
 * @retval 2 file is not regular
 *
 * @return 0 or digit more then 0 if file doesn't exist or is not regular
 **/
int is_file_exist_and_regular(const char *filename, unsigned int verbose);


/**
 * Open file and add record.
 *
 * Function open file and append string to him. If file does not
 * exists then he will be created with permissions 0600. For locking
 * uses fcntl() function.
 *
 * @param filename name of file
 * @param record string for writing
 * @param verbose level of verbose
 **/
void add_record_to_file(const char *filename, const char *record, unsigned int verbose);

#endif /* COMMON_H */

