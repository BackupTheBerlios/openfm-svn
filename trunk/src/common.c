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
 * @file   common.c contains common functions
 * @author Slava Semushin <php-coder at altlinux.ru>
 * @since  25.06.2006
 **/

/* for stat()
 *     open()
 **/
#include <sys/types.h>
#include <sys/stat.h>

/* for stat()
 *     fcntl()
 *     write()
 *     fsync()
 *     close()
 **/
#include <unistd.h>

#if 0
/* for add_record_to_file() */
/* for open()
 *     fcntl()
 **/
#include <fcntl.h>
#endif /* 0 */

/* for isdigit() */
#include <ctype.h>

/* for fprintf()
 *     perror()
 *     NULL constant
 **/
#include <stdio.h>

/* for exit() */
#include <stdlib.h>

/* for strlen()
 *     strchr()
 * */
#include <string.h>

/* for time()
 *     localtime()
 *     __isleap macros
 **/
#include <time.h>

/**
 * @todo:
 * - Obviate from using __isleap macros
 **/
/**
 * Nonzero if YEAR is a leap year (every 4 years, except every 100th
 * isn't, and every 400th is).
 *
 * Redefine macros from time.h for systems which not have him and
 * rename macros for system which have him.
 **/
#ifdef __isleap
   #define ISLEAP __isleap
#else
   #define ISLEAP(year) \
        ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif

/* for gettext&co stuff */
#include "common.h"


/**
 * Print error message with number of line when error was found.
 *
 * @warning Dont forget add ';' at end of macros. Else you will have
 * compiler error.
 * */
#define PRINTLN(msg) \
    fprintf(stderr, "%lu: %s\n", lineno, _(msg))

/** Likewise \ref PRINTLN but have one digital argument */
#define PRINTLN1(msg, arg) \
    fprintf(stderr, "%lu: %s: %d\n", lineno, _(msg), arg)

/** Likewise \ref PRINTLN but have two digital arguments */
#define PRINTLN2(msg, arg1, arg2) \
    fprintf(stderr, "%lu: %s: %d.%d\n", lineno, _(msg), arg1, arg2)

/** Likewise \ref PRINTLN but have three digital arguments */
#define PRINTLN3(msg, arg1, arg2, arg3) \
    fprintf(stderr, "%lu: %s: %d.%d.%d\n", lineno, _(msg), arg1, arg2, arg3)


/**
 * Test string for confirm to format.
 *
 * Functions do 15 tests under giving string. Right string looks like
 * this
 *
 * \c "sign|dd.mm.yyyy|category|amount|comment"
 *
 * and consist of four fields:
 *
 * -# sign -- should be '+' or '-' only. It is says to program: costs
 *    or profit.
 * -# date in format \c "dd.mm.yyyy"
 * -# category should be in numerical format
 * -# amount of profit/costs (depends on first field)
 * -# comment
 *
 * @param str string which would be checked
 * @param lineno number of string, which will be printed if error
 *
 * @retval 0 one of tests failed
 * @retval 1 all tests successful
 *
 **/
int
is_string_confirm_to_format(const char *str, unsigned long lineno)
{
  /**
   * @todo
   * - incorporate tests 6, 10 and perhaps 9
   **/
  int day;   /* day gets from string */
  int month; /* month gets from string */
  int year;  /* year gets from string */

  time_t unix_time;      /* current time in unix format (seconds since 01.01.1970) */
  struct tm *local_time; /* current time in local-time format */

  char *sep_cat;    /* point to separator after 3rd field */
  char *sep_amount; /* point to separator after 4th field */
  const char *i;

  /* check lenght of string */
  if (strlen(str) < 18) {
      PRINTLN("String is too small");
      return 0;
  }

  /* check first field */
  if (str[0] != '-' && str[0] != '+') {
      PRINTLN("First field of string should be sign '+' or '-'!");
      return 0;
  }

  /* check separators for fields */
  if (str[1] != '|' || str[12] != '|') {
      PRINTLN("Separator for fields should be sign '|'!");
      return 0;
  }

  sep_cat = strchr(str+13, '|');
  if (sep_cat == NULL) {
      PRINTLN("Separator after third field not found!");
      return 0;
  }

  sep_amount = strchr(sep_cat+1, '|');
  if (sep_amount == NULL) {
      PRINTLN("Separator after fourth field not found!");
      return 0;
  }

  /* check category: should consist of digitals only */
  for (i = str+13; i < sep_cat; i++) {
    if (!isdigit(*i)) {
        PRINTLN("Third field should consist of digitals only!");
        return 0;
    }
  }

  /* check amount: should consist of digitals or point/comma only */
  for (i = sep_cat+1; i < sep_amount; i++) {
    if ((!isdigit(*i)) && *i != '.' && *i != ',') {
        PRINTLN("Fourth field should consist of digitals and point or comma only!");
        return 0;
    }
  }

  /* check date: should consist of digitals only */
  if (!(isdigit(str[2] ) && isdigit(str[3] ) && /* check day */
        isdigit(str[5] ) && isdigit(str[6] ) && /* check month */
        isdigit(str[8] ) && isdigit(str[9] ) && /* check year */
        isdigit(str[10]) && isdigit(str[11]))) {
      PRINTLN("Date should consist of digitals only!");
      return 0;
  }

  /* check separators for date */
  if (str[4] != '.' || str[7] != '.') {
      PRINTLN("Separator for date should be sign '.'!");
      return 0;
  }

  /* check day */
  day = (str[2] - '0') * 10 + (str[3] - '0');
  if (day > 31 || day == 0) {
      PRINTLN1("Invalid number of day", day);
      return 0;
  }

  /* check month */
  month = (str[5] - '0') * 10 + (str[6] - '0');
  if (month > 12 || month == 0) {
      PRINTLN1("Invalid number of month", month);
      return 0;
  }

  /* check year */
  year = (str[8]  - '0') * 1000 +
         (str[9]  - '0') * 100  +
         (str[10] - '0') * 10   +
         (str[11] - '0');

  if (year == 0) {
      PRINTLN("Invalid number of year! Year should be more then 0");
      return 0;
   }

  /* check day of months */
  if (month == 2) {
      /* if it is leap year */
      if (ISLEAP(year) && day > 29) {
          PRINTLN2("Invalid day of month in leap year", day, month);
          return 0;
      }

      /* other months */
      if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
          PRINTLN2("Invalid day of month", day, month);
          return 0;
      }
  }

  /* if date is in the future */
  unix_time = time(NULL);
  if (unix_time == (time_t)-1) {
      perror("time");
  } else {
    local_time = localtime(&unix_time);
    if (local_time == NULL) {
        fprintf(stderr, "localtime: %s\n", _("error occurs"));
    } else {
      local_time->tm_year += 1900; /* because years since 1900 */
      if (year  > local_time->tm_year || /* year more */
         (year == local_time->tm_year && /* month more */
          month > local_time->tm_mon + 1) ||
         (year == local_time->tm_year && /* day more */
          month == local_time->tm_mon + 1 &&
          day > local_time->tm_mday)) {
          PRINTLN3("Date in future", day, month, year);
          return 0;
      }
    } /* localtime */
  } /* time */

  /**
   * @todo
   * - change symbol '#' in comment to '\\0' for ignore comments (?)
   **/

  return 1;
}


/**
 * Examinate file: he should exist and be regular.
 *
 * Function tries to get statistics about file and determine is that a
 * regular file. Used for verify user-defined filename.
 *
 * @param filename name of file
 * @param verbose level of verbose
 *
 * @retval 0 stat(2) returns error or file is not regular
 * @retval 1 file exists and is regular
 *
 **/
int
is_file_exist_and_regular(const char *filename, unsigned int verbose)
{
  struct stat file_info;
  int ret;

  if (verbose >= 1)
      printf(_("-> Trying to get statistics for %s file\n"), filename);

  /* get statistics about file */
  ret = stat(filename, &file_info);
  if (ret == -1) {
      perror("stat");
      return 0;
   }

  /* check if file is regular */
  if (!S_ISREG(file_info.st_mode)) {
      fprintf(stderr, _("File %s isn't regular file!\n"), filename);
      return 0;
  }

  return 1;
}


#if 0
/* This function not used now, but will be used later */
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
void
add_record_to_file(const char *filename, const char *record, unsigned int verbose)
{
  int fd;       /* file descriptor retured by open() */
  int ret;      /* for storage close(), fcntl() and fsync() return values */
  ssize_t wret; /* for storage write() return value */
  struct flock lock; /* need for fcntl() function */

  if (verbose >= 1) {
      printf("-> %s (%s)\n", _("Open data file"), filename);
  }

  /* open file for append data */
  fd = open(filename, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
  if (fd == -1) {
      perror("open");
      exit(EXIT_FAILURE);
  }

  if (verbose >= 2) {
      printf("--> %s\n", _("Lock file"));
  }

  lock.l_type   = F_WRLCK;  /* lock for write */
  lock.l_whence = SEEK_END; /* begin lock from end of file */
  lock.l_start  = 0;        /* start lock from 0 byte */
  lock.l_len    = 0;        /* lock from end of file and to the end %) */

  ret = fcntl(fd, F_SETLK, &lock);
  if (ret == -1 ) {
      fprintf(stderr, "fcntl: %s\n", _("cannot lock file for writing"));
      perror("fcntl");
      exit(EXIT_FAILURE);
  }

  if (verbose >= 2) {
      printf("--> %s\n", _("Writing data"));
  }

  /* write data */
  wret = write(fd, record, strlen(record)*sizeof(char));
  if (wret == -1) {
      perror("write");
      exit(EXIT_FAILURE);
  }

  if (verbose >= 2) {
      printf("--> %s\n", _("Unlock file"));
  }

  /* reset lock */
  lock.l_type = F_UNLCK;
  ret = fcntl(fd, F_SETLK, &lock);
  if (ret == -1) {
      fprintf(stderr, "fcntl: %s\n", _("cannot unlock file"));
      perror("fcntl");
      exit(EXIT_FAILURE);
  }

  if (verbose >= 2) {
      printf("--> %s\n", _("Flushing data to disk"));
  }

  /* flush data */
  ret = fsync(fd);
  if (ret == -1) {
      perror("fsync");
  }

  if (verbose >= 2) {
      printf("--> %s\n", _("Close file"));
  }

  /* close file */
  ret = close(fd);
  if (ret == -1) {
      perror("close");
      exit(EXIT_FAILURE);
  }

}
#endif /* 0 */

