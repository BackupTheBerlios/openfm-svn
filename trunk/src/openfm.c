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
 * @file   openfm.c contains main() function and related functions
 * @author Slava Semushin <php-coder at altlinux.ru>
 * @since  23.04.2006
 **/

/* for getpwuid()
 *     getuid()
 **/
#include <sys/types.h>

/* for assert() */
#include <assert.h>

/* for getpwuid() */
#include <pwd.h>

/* for getuid()
 *     getopt()
 **/
#include <unistd.h>

/* for errno variable */
#include <errno.h>

/* for printf()
 *     fprintf()
 *     snprintf()
 *     sscanf()
 *     fopen()
 *     fgets()
 *     fclose()
 *     perror()
 *     FILE and NULL constants
 **/
#include <stdio.h>

/* for exit()
 *     malloc()
 *     calloc()
 *     free()
 *     getenv()
 *     EXIT_* constants
 **/
#include <stdlib.h>

/* for strlen()
 *     strdup()
 *     strchr()
 **/
#include <string.h>

/* for LINE_MAX constant */
#include <limits.h>

/** Use self-defined value if POSIX2 is not supported */
#ifndef LINE_MAX
   #define LINE_MAX 2048
#endif

/* Also includes config.h and other headers which needs for gettext
 * support */
#include "common.h"

#ifdef NLS
   /* for setlocale() */
   #include <locale.h>
#endif /* NLS */


/** Name of data file */
#define DATA_FILE "finance.db"

/** Maximal count of wrong lines.\ If more then exit from program */
#define MAX_WRONG_LINES 5


/* struct and enumerations with program settings */
/** Possible actions */
typedef enum {NONE, ADD, SHOW} actions;

/** Arguments for \ref actions */
typedef enum {COST, PROFIT, CATEGORY, BALANCE, FULLSTAT} arguments;

/** Program settings.
 *
 * Contains program settings which will initial in \ref
 * analyze_arguments() function.
 **/
struct settings {
  actions      act;     /**< see description for \ref actions */
  arguments    arg;     /**< see description for \ref arguments */
  char        *dbfile;  /**< full path to data file */
  unsigned int verbose; /**< level of verbose */
};


/* Prototypes */
static  int parse_cmd_line(int argc, char **argv, unsigned int *verbose);
static void analyze_arguments(struct settings *ofm, int argc, char **argv, int start);
static char *get_path_to_datafile(unsigned int verbose);
static void read_and_parse_datafile(const struct settings *ofm);

#ifdef NLS
static void turn_on_localization(void);
#endif /* NLS */


/**
 * Prepare to work.
 *
 * Function do all preparations for work with data. This includes
 * setup localization, set default values for base variables and parse
 * command line arguments and options.
 *
 * @param ofm struct with program settings
 * @param argc program arguments counter
 * @param argv list of program arguments
 **/
static void
prepare(struct settings *ofm, int argc, char **argv)
{
  /* temporary variable. Used only as routine between parse_cmd_line()
   * and analyze_arguments()
   **/
  int opt_num;

  assert(ofm != NULL);
  assert(argc > 0);
  assert(argv != NULL);

#ifdef NLS
  turn_on_localization();
#endif /* NLS */

  /* look at command line options */
  opt_num = parse_cmd_line(argc, argv, &ofm->verbose);

  assert(opt_num > 0);

  /* parse another arguments if they exists */
  if (opt_num < argc) {
      analyze_arguments(ofm, argc, argv, opt_num);
  }

  /* if user does not give data file */
  if (ofm->dbfile == NULL) {
      ofm->dbfile = get_path_to_datafile(ofm->verbose);
  }

}


/**
 * Main function.
 *
 * Function is a mother of all another functions. Program starts from
 * that function.
 *
 * @param argc program arguments counter
 * @param argv list of program arguments
 *
 * @retval EXIT_SUCCESS all right
 * @retval EXIT_FAILURE error occurs
 **/
int
main(int argc, char **argv)
{
 /* program settings which will get from command line */
 struct settings ofm;

 ofm.act     = NONE; /* no actions should be perform by default */
 ofm.verbose = 0;    /* no verbose by default */
 ofm.dbfile  = NULL;

 prepare(&ofm, argc, argv);

 switch (ofm.act) {
     case NONE:
         /* read datafile, parse him and print statistics */
         read_and_parse_datafile(&ofm);
         break;
     /**
      * @todo
      * - implement action "add"
      **/
     case ADD:
         fprintf(stderr, "Action \"add\" not implemented yet!\n");
         break;
     /**
      * @todo
      * - implement action "show"
      **/
     case SHOW:
         fprintf(stderr, "Action \"show\" not implemented yet!\n");
         break;
     default:
         fprintf(stderr, "Unknown action!\n");
         break;
 }


 return EXIT_SUCCESS;
}


/**
 * Print helps.
 *
 * Function prints helpful information about usage and keys of
 * program. Get name of program via one argument. Usually is argv[0].
 *
 * @param progname name of program
 *
 * @return quit from program with successful exit code
 **/
static void
print_help(const char *progname)
{
  assert(progname != NULL);

  printf(_("%s: Your private financial manager\n\n"
         "Usage: %s [option] [file]\n"
         "  -v\tenable verbose mode\n"
         "  -V\tprint version and exit\n"
         "  -h\tprint this help and exit\n"),
         progname, progname);

  exit(EXIT_SUCCESS);
}


/**
 * Print version of program and copyright.
 *
 * Function prints version and copyright. Get name of program via first
 * argument (usually it is argv[0]) and version from VERSION macros
 * (which comes from config.h).
 *
 * @param progname name of program
 *
 * @return quit program with successful exit code
 **/
static void
print_version(const char *progname)
{
  assert(progname != NULL);

  printf("%s: %s %s\n"
         "Copyright (C) 2006 Slava Semushin <php-coder at altlinux.ru>\n",
         progname, _("version"), VERSION);

  exit(EXIT_SUCCESS);
}


/**
 * Parse command line options.
 *
 * Function parses command line options. If unknown option was found
 * then function would quit from program with failure exit code.
 *
 * @param argc program arguments counter
 * @param argv list of program arguments
 * @param verbose level of verbose
 *
 * @return number of first non-option element in argv
 **/
static int
parse_cmd_line(int argc, char **argv, unsigned int *verbose)
{
  int option;

  assert(argc > 0);
  assert(argv != NULL);

  while ((option = getopt(argc, argv, "vVh")) != -1) {
    switch (option) {

      case 'v': /* enable verbose mode */
        (*verbose)++;
        break;

      case 'V':
        /* Print version of program and exit */
        print_version(argv[0]);
        break;

      case 'h':
        /* Print help and exit */
        print_help(argv[0]);
        break;

      case '?': /* for unknown options */
        exit(EXIT_FAILURE);

      default:  /* this case never happens */
        fprintf(stderr, "getopt: %s %c\n", _("return"), option);
        break;

    }
  }

  if (*verbose >= 1) {
      printf("-> %s %u\n", _("NOTE: Set verbose level to"), *verbose);
  }

  return optind;
}


/**
 * Parse command line arguments.
 *
 * Function parses command line arguments and can initial ofm and dbfile.
 *
 * Valid arguments for program are:
 *
 * <tt>add (cost|profit) $amount $comment</tt>\n
 * <tt>add cetegory $category</tt>\n
 * <tt>show (costs|profits|balance|fullstat|categories)</tt>
 *
 * Also user can gives path to data file.
 *
 * @note Only first name of file will be verified and used as data file
 * @warning If dbfile was change (not NULL after) then don't forget to free
 * memory with free() function.
 *
 * @param ofm struct with program settings
 * @param argc program arguments counter
 * @param argv list of program arguments
 * @param start number of first non-option element in argv
 **/
static void
analyze_arguments(struct settings *ofm, int argc, char **argv, int start)
{

  assert(ofm != NULL);
  assert(argc > 0);
  assert(argv != NULL);
  assert(start > 0);

  /* if action "add" was chosen */
  if (strcmp(argv[start], "add") == 0) {
      ofm->act = ADD;

  /* if action "show" was chosen */
  } else if (strcmp(argv[start], "show") == 0) {
      ofm->act = SHOW;

  /* if unknown action then interpret this as data file */
  } else {
      /* we not set ofm->act to NONE bacause it is done in main() */
      if (!is_file_exist_and_regular(argv[start], ofm->verbose)) {
          fprintf(stderr, "%s\n", _("Using default data file..."));
      } else {
          ofm->dbfile = strdup(argv[start]);
          if (ofm->dbfile == NULL) {
              fprintf(stderr, "strdup: %s\n%s\n",
                      _("cannot allocate memory"),
                      _("Using default data file..."));
          }
      }
      return;
  } /* end check for first argument */

  if (argc - start == 1) {
      fprintf(stderr, "%s \"%s\"\n",
              _("Not enough arguments for action"),
              (ofm->act == ADD) ? "add" : "show");
      exit(EXIT_FAILURE);
  }

  start++; /* forward to next argument */

  /* for action "add" */
  if (ofm->act == ADD) {
      /* if argument "cost" was given */
      if (strcmp(argv[start], "cost") == 0) {
          ofm->arg = COST;

      /* if argument "profit" was given */
      } else if (strcmp(argv[start], "profit") == 0) {
          ofm->arg = PROFIT;

      /* if argument "category" was given */
      } else if (strcmp(argv[start], "category") == 0) {
          ofm->arg = CATEGORY;

      /* if unknown argument */
      } else {
          fprintf(stderr, "%s \"add\": %s\n",
                  _("Wrong argument for action"), argv[start]);
          exit(EXIT_FAILURE);
      } /* end for action "add" */

  /* for action "show" */
  } else if (ofm->act == SHOW) {
      /* if argument "costs" was given */
      if (strcmp(argv[start], "costs") == 0) {
          ofm->arg = COST;

      /* if argument "profits" was given */
      } else if (strcmp(argv[start], "profits") == 0) {
          ofm->arg = PROFIT;

      /* if argument "balance" was given */
      } else if (strcmp(argv[start], "balance") == 0) {
          ofm->arg = BALANCE;

      /* if argument "fullstat" was given */
      } else if (strcmp(argv[start], "fullstat") == 0) {
          ofm->arg = FULLSTAT;

      /* if argument "categories" was given */
      } else if (strcmp(argv[start], "categories") == 0) {
          ofm->arg = CATEGORY;

      /* if unknown argument */
      } else {
          fprintf(stderr, "%s \"show\": %s\n",
                  _("Wrong argument for action"), argv[start]);
          exit(EXIT_FAILURE);
      } /* end for action "show" */

  } /* end for actions */


  /* when program was called as "openfm add (cost|profit|category)" */
  if (ofm->act == ADD && argc - start == 1) {
      fprintf(stderr, "%s \"add\"\n", _("Not enough arguments for action"));
      exit(EXIT_FAILURE);
  }

}


/**
 * Get path to file with data that will be used by default.
 *
 * Function gets and returns path to data file which will be used by
 * default. Path is "$HOME/DATA_FILE". For name of file uses \ref DATA_FILE
 * macros which you can change. If error occurs then function will quit
 * from program with failure code exit.
 *
 * @warning Don't forget to free memory after! Use free() for that.
 *
 * @param verbose level of verbose
 *
 * @return path to default file with data
 **/
static char *
get_path_to_datafile(unsigned int verbose)
{
  char  *homedir;       /* path to home directory */
  char  *dbfile;        /* result path */
  size_t filename_size; /* size of result path */
  int    ret;           /* for storage snprintf() return value */

  /* both variables uses only when getenv() fails */
  struct passwd *user;  /* for storage getpwuid() return value */
  uid_t user_id;        /* for storage getuid() return value */

  homedir = getenv("HOME");
  if (homedir == NULL) {
      fprintf(stderr, "getenv: %s\n", _("cannot get value for $HOME variable"));

      errno = 0;                /* set to zero as said in man page */
      user_id = getuid();       /* get uid */
      user = getpwuid(user_id); /* search data about user with this uid */
      if (user == NULL) {
          fprintf(stderr, "getpwuid: %s %u\n", _("cannot find user with uid"), user_id);
          perror("getpwuid");
          exit(EXIT_FAILURE);
      }

      /* use value returned by getpwuid() */
      homedir = user->pw_dir;
  }

  if (verbose >= 1) {
      printf("-> %s '%s'\n", _("Your home directory is"), homedir);
  }

  /* Allocate memory for path to file with data:
   * strlen("/home/coder" + "/" + "finance.db" + "\0")
   **/
  filename_size = strlen(homedir) + strlen(DATA_FILE) + 2;
  dbfile = malloc(filename_size);
  if (dbfile == NULL) {
      fprintf(stderr, "malloc: %s\n", _("cannot allocate memory"));
      exit(EXIT_FAILURE);
  }

  ret = snprintf(dbfile, filename_size, "%s/%s", homedir, DATA_FILE);
  if ( (ret > 0) && ((size_t)ret >= filename_size) ) {
      /* FIXME:
       * - Should we print more information ?
       * - May error occur when we cast ret to size_t type ?
       *
       * TODO:
       * - NOTES on man page say that some version of glibc can
       *   return -1 when output was truncated. We should correct
       *   handle this case too.
       *
       *   (BTW, does the way to determine what glibc is used exist?)
       **/
      fprintf(stderr, _("Writing data was truncated.\n"
                        "Please notify the author about this incident!\n"));
      fprintf(stderr, "snprintf: %s %d\n", _("return"), ret);
      exit(EXIT_FAILURE);
  } else if (ret < 0) {
     fprintf(stderr, _("Failed to write to the character string\n"));
     fprintf(stderr, "snprintf: %s %d\n", _("return"), ret);
     exit(EXIT_FAILURE);
  }

  return dbfile;
}


/**
 * Read file, parse him and print short statistics.
 *
 * Function open data file and read him string by string. Each string
 * would be checked with \ref is_string_confirm_to_format() function.
 * As result will prints short statistics about user's money.
 *
 * @param ofm struct with program settings
 **/
static void
read_and_parse_datafile(const struct settings *ofm)
{
  FILE *fp;
  int   ret; /* for storage fclose() return value */

  char  sign;
  float plus, minus, curr;

  /* current line from file */
  char *curline;

  /* for storage strchr() return value */
  char *pos;

  /* counter for lines in file */
  unsigned long lineno;

  /* counter for records in file */
  unsigned long record_count;

  /* counter for wrong lines in file */
  int fails;

  assert(ofm != NULL);

  if (ofm->verbose >= 1) {
      printf("-> %s (%s)\n", _("Open data file"), ofm->dbfile);
  }

  /* open data file */
  fp = fopen(ofm->dbfile, "r");
  if (fp == NULL) {
      fprintf(stderr, "%s: %s\n", _("Failed to open file"), ofm->dbfile);
      perror("fopen");
      exit(EXIT_FAILURE);
  }

  /* free memory for path to data file */
  free(ofm->dbfile);

  if (ofm->verbose >= 1) {
      printf("-> %s\n", _("Reading data..."));
  }

  curline = calloc(LINE_MAX + 1, sizeof(char));
  if (curline == NULL) {
      fprintf(stderr, "calloc: %s\n", _("cannot allocate memory"));
      exit(EXIT_FAILURE);
  }

  plus = minus = 0.0;
  lineno = 0UL;
  record_count = 0UL;
  fails  = 0;

  /* read and parse data file */
  /**
   * @todo
   * - fgets() returns NULL also when error occurs. We should correct
   *   handle this situation.
   **/
  while (fgets(curline, LINE_MAX + 1, fp) != NULL) {
    lineno++;

    /* skip empty lines */
    if (strlen(curline) == 1 && curline[0] == '\n') {
        continue;
    }

    /* kill trailing newline */
    pos = strchr(curline, '\n');
    if (pos != NULL) {
        *pos = '\0';
    }

    if (ofm->verbose >= 3) {
        printf("---> %lu: '%s'\n", lineno, curline);
    }

    if (fails == MAX_WRONG_LINES) {
        fprintf(stderr, _("Too many wrong lines in database. Exit.\n"));
        exit(EXIT_FAILURE);
    }

    if (!is_string_confirm_to_format(curline, lineno)) {
        fails++;
        continue;
    }

    if (sscanf(curline, "%c|%*2u.%*2u.%*4u|%*u|%f|%*s", &sign, &curr) != 2) {
        fprintf(stderr, "sscanf: %s\n", _("error occurs"));
        fails++;
        continue;
    }

    record_count++;

    if (sign == '-') {
        minus += curr;
    } else {
        plus += curr;
    }

  } /* end for fgets() */

  /* free memory for input lines */
  free(curline);

  /**
   * @todo
   * - Deal with plural forms. Use ngettext()
   **/
  if (ofm->verbose >= 1) {
      printf(_("-> Reads %lu strings"), lineno);
      if (lineno > record_count)
          printf(_(" and %lu records"), record_count);
      printf(" %s\n", _("from data file"));
  }

  /* print short statistics */
  printf(_("Finance statistics:\n"
         "Profit:  %8.2f\n"
         "Costs:   %8.2f\n" /* eight because point belongs to digital */
         "Balance: %8.2f\n"),
         plus, minus, plus - minus);

  /* close data file */
  ret = fclose(fp);
  if (ret != 0) {
     perror("fclose");
     exit(EXIT_FAILURE);
  }

}


/**
 * Set settings for using gettext() functions.
 *
 * Function just calls setlocale() and textdomain(). Those functions
 * needs for correct work gettext() functions.
 **/
#ifdef NLS
void
turn_on_localization(void)
{
  char *locale = NULL; /* for storage setlocale() return value */
  char *domain = NULL; /* for storage textdomain() return value */

  /* set locale according to the environment variables */
  locale = setlocale(LC_ALL, "");
  if (locale == NULL) {
      fprintf(stderr, "setlocale: cannot set locale according to the"
              "environment variables! Using \"C\" locale by default.\n");
  }

  /* set domain name for gettext() */
  domain = textdomain("openfm");
  if (domain == NULL) {
      perror("textdomain");
  }

}
#endif /* NLS */

/**
 * @mainpage Open Finance Manager
 * @section main
 * This is a documentation for OpenFM
 **/
