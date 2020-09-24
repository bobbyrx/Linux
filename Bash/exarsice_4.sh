#!/bin/bash

n1=0
n2=0
name1=$(echo "$1"|cut -d'.' -f 1)
name2=$(echo "$2"|cut -d'.' -f 1)
while read -r line
  do
    if [[ $(echo "$line"| cut -d' ' -f 3 ) == "$name1" ]];
      then 
        n1=$(( n1+1 ))
    fi
done < "$1"

while read -r line
  do
    if [[ $(echo "$line"| cut -d' ' -f 3 ) == "$name2" ]];
      then 
         n2=$((n2+1))
    fi
done < "$2"

if (( "$n1" >= "$n2" ));
   then 
      name="$1"
   else
      name="$2"
fi

namefinal=$(echo "$name"|cut -d'.' -f 1)

name=$(cut -d' ' -f 5- "$name"| sort)

echo "$name" > "$namefinal".song

exit
