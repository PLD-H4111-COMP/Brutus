#!/bin/bash

target="a.out"

if [ -z "$BRUTUS" ]; then
    BRUTUS=./Brutus
fi

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
	$BRUTUS -o .tmp.s $1
	if [ $? -ne 0 ]; then
	    echo "Problem encountered when compiling with Brutus..."
        exit 1
	else
    	echo "Brutus compilation finished."
	fi

	gcc .tmp.s -o $target
	if [ $? -ne 0 ]; then
	    echo "Problem encountered with gcc"
        exit 2
	else
    	echo "\"$target\" was generated successfully."
	fi

    if [ -f ".tmp.s" ]; then
        rm -f .tmp.s
    fi
fi

