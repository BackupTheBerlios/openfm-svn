#!/bin/sh
#
# This script is part of testing suite for OpenFM
# Copyright (C) 2006 Slava Semushin <php-coder at altlinux.ru>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.  You should have received a
# copy of the GNU General Public License along with this program; if
# not, write to the Free Software Foundation, Inc., 51 Franklin
# Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# $Id$
#

#####################################################################
#                          Set variables                            #
#####################################################################

export LANG=C
OPENFM="./openfm"


#####################################################################
#                         Define functions                          #
#####################################################################

compare_files() {
  local ORIG="$1"
  local MODIF="$2"

  cmp -s "$ORIG" "$MODIF"
  if [ $? -eq 0 ]; then
     rm -f "$MODIF"
     echo -e "\033[32;40mok\033[0m"
  else
     echo -e "\033[31;40mfailed\033[0m"
     diff -U0 "$ORIG" "$MODIF"
  fi
}


print_message() {
  echo -n "Testing ${1}... "
}


#####################################################################
#                           Start program                           #
#####################################################################

# check arguments
if [ $# -ne 1 ]; then
   echo "Usage: $0 number_of_test" >&2
   exit 1
fi

# preparation for start
if [ ! -L ./openfm ]; then
   if [ ! -x ../src/openfm ]; then
      echo "Executable openfm file not found!" >&2
      echo "Probably you have not compile program" >&2
      exit 2
   fi
   ln -s ../src/openfm openfm
fi

if [ -f finance.db ]; then
    echo "Error! File finance.db should not exist!" >&2
    exit 3
fi

#####################################################################
#                             Start tests                           #
#####################################################################

case $1 in
    1)
      print_message "-V option"
      ($OPENFM -V; echo rc=$?) 2>&1 >"$1.txt"
      ;;
    2)
      print_message "-h option"
      ($OPENFM -h; echo rc=$?) 2>&1 >"$1.txt"
      ;;
    3)
      print_message "absent datafile"
      (HOME=. $OPENFM 2>&1; echo rc=$?) >"$1.txt"
      ;;
    4)
      print_message "empty datafile"
      ($OPENFM "$1.in"; echo rc=$?) >"$1.txt"
      ;;
    5)
      print_message "'openfm show' command"
      ($OPENFM show 2>&1; echo rc=$?) >"$1.txt"
      ;;
    6)
      print_message "'openfm add' command"
      ($OPENFM add 2>&1; echo rc=$?) >"$1.txt"
      ;;
    7)
      print_message "'openfm add qwerty12345' command"
      ($OPENFM add qwerty12345 2>&1; echo rc=$?) >"$1.txt"
      ;;
    8)
      print_message "'openfm show qwerty12345' command"
      ($OPENFM show qwerty12345 2>&1; echo rc=$?) >"$1.txt"
      ;;
    9)
      print_message "'openfm add cost' command"
      ($OPENFM add cost 2>&1; echo rc=$?) >"$1.txt"
      ;;
    10)
      print_message "'openfm add profit' command"
      ($OPENFM add profit 2>&1; echo rc=$?) >"$1.txt"
      ;;
    11)
      print_message "'openfm add category' command"
      ($OPENFM add category 2>&1; echo rc=$?) >"$1.txt"
      ;;
    *)
      echo "Wrong number for test: $1" >&2
      exit 3
      ;;
esac

compare_files "$1.out" "$1.txt"

