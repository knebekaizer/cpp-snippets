#!/bin/sh

. getopts_long

verbose=false opt_bar=false bar=default_bar foo=default_foo
opt_s=false opt_long=false
OPTLIND=1
while getopts_long :sf:b::vh opt \
  long 0 \
  foo required_argument \
  bar 2 \
  verbose no_argument \
  help 0 "" "$@"
do
  case "$opt" in
    s) opt_s=true;;
    long) opt_long=true;;
    v|verbose) verbose=true;;
    h|help) usage; exit 0;;
    f|foo) foo=$OPTLARG;;
    b|bar) bar=${OPTLARG-$bar};;
    :) printf >&2 '%s: %s\n' "${0##*/}" "$OPTLERR"
       usage
       exit 1;;
  esac
done
shift "$(($OPTLIND - 1))"
# process the remaining arguments

echo "verbose=$verbose opt_bar=$opt_bar bar=$bar foo=$foo opt_s=$opt_s opt_long=$opt_long"

exit
