#!/bin/bash

if [ $# -ne 3 ];
   then
   echo "Invalid number of arguments"
   exit 1
fi

FILE="${1}"
KEY1="${2}"
KEY2="${3}"

if [ ! -f "${FILE}" ]; then
   echo "Invalid argumnet"
   exit 3
fi

LINE1=$( grep "^${KEY1}=" "${FILE}" )
LINE2=$( grep "^${KEY2}" "${FILE}" )

VALUE1=$( echo "${LINE1}" | cut -d"=" -f 2 )
VALUE2=$( echo "${LINE2}" | cut -d"=" -f 2 )

for CHAR_2 in ${VALUE2};
do
  boolean=false
  for CHAR_1 in ${VALUE1};
  do
    if [ "${CHAR_2}" == "${CHAR_1}" ];
       then
          boolean=true
          break
    fi
  done 

 if [ "${boolean}" == "false" ]; then
    VALUE+="${CHAR_2} "
 fi
done 

REPLACE="${KEY2}=${VALUE}"

sed -i "s/${LINE2}/${REPLACE}/g" "${FILE}"

