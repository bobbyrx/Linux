#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Incorrect number of arguments!"; exit 1
fi

if [ ! -d $1 -o ! -r $1 ]; then
	echo "Argument is not a directory or is not readable!";exit 2
fi

find $1 -printf "%Y %p\n" | grep ^[NL] | cut -d " " -f2

exit
