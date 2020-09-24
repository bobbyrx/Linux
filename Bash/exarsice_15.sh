#!/bin/bash 

if [ $(id -u) -ne 0 ];
   then 
      exit 0
fi

FILE=$(cat /etc/passwd | cut -d":" -f 1,6 --output-delimiter=' ' )

while read USER HOME;
  do
    if [ ! -d "${HOME}" ] || sudo -u "${USER}" [ ! -w "${HOME}" ];
       then
          echo "${USER}"
    fi
done < "${FILE}"
