#!/bin/bash
if [ $# -ne 1 ];
   then 
      echo "Invalid number of arguments: Using $0 <name> "
      exit 1
fi

if [ $1 -ne 0 ];
   then 
      echo "We only want root: Using $0 <name> "
      exit 0
fi

N="${1}"
if ! grep -qE '^[+-]?[0-9]+$' <(echo "${N}" );
   then
      echo "Invalid argument"
      exit 3
fi

USER=$(mktemp)

ps -e -o user= |sort|uniq > "${USER}"

while read Line1
do
USER_RSS=0
   while read Line2_1 Line2_2
   do
    USER_RSS=$(expr $USER_RSS + $Line2_2 )
    LAST_PID=$(expr $Line2_1)
   done < <(ps -u "${Line1}" -o pid=,rss=|sort -n -k2)
if [ ${USER_RSS} -gt ${N} ];
   then
   kill -s SIGTERM "${LAST_PID}"
   sleep 1
   kill -s SIGKILL "${LAST_PID}"
fi
done < "${USER}"
rm -- "${USER}"
