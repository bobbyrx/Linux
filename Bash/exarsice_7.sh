#!/bin/bash
#$(echo "$line" | cut -d'/' -f 2 ) == s[0-9]*$
while read -r line
  do 
    line=$(echo "$line" |cut -d':' -f 6 | cut -d'/' -f 3-4)
 if [[ $(echo "$line" | cut -d '/' -f 2 ) == s[0-9][0-9][0-9][0-9][0-9] && $(echo "$line" | cut -d'/' -f 1 ) == "Inf" ]];
       then
          cat "$1" | cut -d':' -f 1 | cut -c 3-4 | grep -c $(echo "$line" | cut -d'/' -f 2 | cut -c 3-4 ) | tr '\n' ' '
          echo "$line" | cut -d'/' -f 2 | cut -c 3-4 
 fi
done < "$1"
