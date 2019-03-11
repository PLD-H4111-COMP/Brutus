#!/bin/bash


set -e
set -u
set -o pipefail

target="default_output"


# fetch options
while getopts 'o:' OPTION; do
  case "$OPTION" in
    o)
      target="$OPTARG"
      ;;
    ?)
      echo "script usage: source_file [-o output]" >&2
      exit 1
      ;;
  esac
done
shift "$(($OPTIND -1))"



# compile
if [ $# -eq 0 ]; then
	echo "Your command line doesn't contain a target file !"
	exit 1
else
	./Brutus $1 > tmp.s
	if [ $? -ne 0 ]; then
	    echo "problem encountred when compiling with Brutus..."
	else
    	echo "Brutus compilation finished."
	fi
	
	gcc tmp.s -o $target
	if [ $? -ne 0 ]; then
	    echo "problem encountred with gcc"
	else
    	echo "\"$target\" was generated successfully."	
	fi
fi





