#!/bin/bash

while read -r line
 do
  line1=$(echo "$line" | cut -d':' -f 5 )
  line2=$(echo "$line" | cut -d':' -f 6 )
 if [[ $(echo "$line2" | cut -d'/' -f 3) == "SI" ]]; 
  then 
   echo "$line1" | cut -d',' -f 1 | tr '\n' ':'
   echo "$line2"
 fi
done < "$1" 

exit  
