#!/bin/bash
List=$(find *.[st][hx]* -maxdepth 1 -type f)
R1=$1
R2=$2
while read -r Curline
 do
  if [ $(wc -l "$Curline"|cut -d" " -f 1) -lt $R1 ]; 
   then 
      cp "$Curline" a
  elif [[ $(wc -l "$Curline"|cut -d" " -f 1) -gt $R1 && $(wc -l "$Curline"|cut -d" " -f 1) -lt $R2 ]];
    then 
      cp "$Curline" b
  else 
      cp "$Curline" c
  fi
done <<<"$List"
exit
