#!/bin/bash

if [ "$#" -ne 1 ];
   then
     echo "Invalid number of arguments. Usage: ${0} <dirname>"; exit 1;
fi
if [ ! -d "${1}" -o ! -r "${1}" ];
   then
   echo "Not directory or not readable"; exit 2;
fi
find "${1}" -type l -printf "%Y %p\n" 2>/dev/null | grep -e "^[^NL]" | cut -c 3-
