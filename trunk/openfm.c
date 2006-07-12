/*
 * Open Financial Manager (OpenFM)
 *
 * License: GPL
 *
 **/

/**
 * @file openfm.c contains main() function and related functions
 * @author Slava Semushin <php-coder at altlinux.ru>
 * @since 23.04.2006
 * @date  10.07.2006
 **/

/* for getpwuid()
 *     getuid()
 **/
#include <sys/types.h>

/* for getpwuid() */
#include <pwd.h>

/* for getuid() */
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
 **/
#include <string.h>

/* for LINE_MAX constant */
#include <limits.h>

/** Use self-defined value if POSIX2 is not supported */
#ifndef LINE_MAX
#define LINE_MAX 2048
#endif

/* for setlocale() */
#include <locale.h>

/* for getopt() */
#include <unistd.h>

/* local file */
#include "common.h"


/** Version of the program */
#define VERSION "0.5"

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
typedef struct {
  actions   act; /**< see description for \ref actions */
  arguments arg; /**< see description for \ref arguments */
} settings_t;


/* Prototypes */
static void print_help(const char *progname);
static void print_version(const char *progname);
static  int parse_cmd_line(int argc, char **argv, unsigned int *verbose);
static void analyze_arguments(char **argv, int start, char **dbfile, unsigned int verbose, settings_t *ofm);
static char *get_path_to_datafile(unsigned int verbose);
static void read_and_parse_datafile(char *dbfile, unsigned int verbose);
static void turn_on_localization(void);


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

 /* for storage path to data file */
 char *dbfile = NULL;

 /* level of verbose. Turn off by default */
 unsigned int verbose = 0;

 /* temporary variable. Used only as routine between parse_cmd_line()
  * and analyze_arguments()
  **/
 int opt_num;

 /* program settings which will get from command line */
 settings_t ofm;


 /* no actions should be perform by default */
 ofm.act = NONE;

 /* enable localizaion */
 turn_on_localization();

 /* look at command line options */
 opt_num = parse_cmd_line(argc, argv, &verbose);

 /* parse another arguments if they exists */
 if (opt_num < argc) {
     analyze_arguments(argv, opt_num, &dbfile, verbose, &ofm);
 }

 /* if user does not give data file */
 if (dbfile == NULL) {
     dbfile = get_path_to_datafile(verbose);
 }


 switch (ofm.act) {
     case NONE:
         /* read datafile, parse him and print statistics */
         read_and_parse_datafile(dbfile, verbose);
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
  printf(_("%s: Your private financial manager\n\n"
         "Usage: %s [option]\n"
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
 * argument (usually it is argv[0]) and version from \ref VERSION
 * macros.
 *
 * @param progname name of program
 *
 * @return quit program with successful exit code
 **/
static void
print_version(const char *progname)
{
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
 * @param argv list of arguments of program
 * @param verbose level of verbose
 *
 * @return number of first non-option element in argv
 **/
static int
parse_cmd_line(int argc, char **argv, unsigned int *verbose)
{
  int option;

  while ((option = getopt(argc, argv, "vVh")) != -1) {
    switch (option) {
      case 'v': /* enable verbose mode */
        (*verbose)++;
        break;
      case 'V': /* print version of program and exit */
        print_version(argv[0]);
        break;
      case 'h': /* print help and exit */
        print_help(argv[0]);
        break;
      case '?': /* for unknown options */
        exit(EXIT_FAILURE);
      default:  /* this case never happens */
        fprintf(stderr, "getopt: %s %c\n", _("return"), option);
        break;
    }
  }

  if (*verbose >= 1)
      printf(_("-> NOTE: Set verbose level to %u\n"), *verbose);


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
 * @param argv list of arguments of program
 * @param start number of first non-option element in argv
 * @param dbfile variable contains full path to data file
 * @param verbose level of verbose
 * @param ofm struct with program settings
 **/
static void
analyze_arguments(char **argv, int start, char **dbfile, unsigned int verbose, settings_t *ofm)
{

  /* if action "add" was chosen */
  if (strcmp(argv[start], "add") == 0) {
      ofm->act = ADD;

  /* if action "show" was chosen */
  } else if (strcmp(argv[start], "show") == 0) {
      ofm->act = SHOW;

  /* if unknown action then interpret this as data file */
  } else {
      /* we not set ofm->act to NONE bacause it is done in main() */
      if (is_file_exist_and_regular(argv[start], verbose)) {
          fprintf(stderr, "%s\n", _("Using default data file..."));
      } else {
          *dbfile = strdup(argv[start]);
          if (*dbfile == NULL ) {
              fprintf(stderr, "strdup: %s\n%s\n",
                      _("Cannot allocate memory"),
                      _("Using default data file..."));
          }
      }
      return;
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

  } /* for actions */

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
  int    ret;           /* for storage snprintf() value */

  /* both variables uses only when getenv() fails */
  struct passwd *user;  /* for storage getpwuid() return value */
  uid_t user_id;        /* for storage getuid() reurn value */

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

  if (verbose >= 1)
      printf(_("-> Your home directory is '%s'\n"), homedir);

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
  }
  if (ret < 0) {
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
 * @param dbfile variable contains full path to data file
 * @param verbose level of verbose
 **/
static void
read_and_parse_datafile(char *dbfile, unsigned int verbose)
{
  FILE *fp;
  int   ret; /* for storage fclose() return value */

  char  sign;
  float plus, minus, curr;

  /* current line from file */
  char *curline;

  /* counter for lines in file */
  unsigned long lineno;

  /* counter for records in file */
  unsigned long record_count;

  /* counter for wrong lines in file */
  int fails;

  if (verbose >= 1)
      printf(_("-> Open data file (%s)\n"), dbfile);

  /* open data file */
  fp = fopen(dbfile, "r");
  if (fp == NULL) {
      fprintf(stderr, _("Failed to open file: %s\n"), dbfile);
      perror("fopen");
      exit(EXIT_FAILURE);
  }

  /* free memory for path to data file */
  free(dbfile);

  if (verbose >= 1)
      printf(_("-> Reading data...\n"));

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

    if (verbose >= 3) {
        /* FIXME:
         * Just think what will happen if one symbol is now in buffer
         * */
        curline[strlen(curline) - 1] = '\0';
        printf("---> %lu: '%s'\n", lineno, curline);
    }

    if (fails == MAX_WRONG_LINES) {
        fprintf(stderr, _("Too many wrong lines in database. Exit.\n"));
        exit(EXIT_FAILURE);
    }

    if (is_string_confirm_to_format(curline, lineno)) {
        fails++;
        continue;
    }

    record_count++;

    /**
     * @todo
     * - correctly handle return value from sscanf()
     **/
    (void)sscanf(curline, "%c|%*2u.%*2u.%*4u|%f|%*s\n", &sign, &curr);

    if (sign == '-') {
        minus += curr;
    } else {
        plus += curr;
    }

  }

  /* free memory for input lines */
  free(curline);

  /**
   * @todo
   * - Deal with plural forms. Use ngettext()
   **/
  if (verbose >= 1) {
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

/**
 * @mainpage Open Finance Manager
 * @section main
 * This is a documentation for OpenFM
 **/
