#!/bin/bash

while read -r line
do 
  line1=$(echo "$line" | cut -d':' -f 6 )
   if [[ $(echo "$line1" | cut -d'/' -f 3) == "SI" ]];
    then
      echo "$line1" | cut -d'/' -f 4 | cut -c 2- | tr '\n' '\t'
      line2=$(echo "$line" | cut -d':' -f 5 )
      echo "$line2" | cut -d',' -f 1 
   fi
done < "$1"
      
   
