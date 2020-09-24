#!/bin/bash
Name=$(cut -d" " -f 4- $1)
Counter=1
while read -r CurLine
 do 
  echo "$Counter. $CurLine"
  ((Counter++))
done <<<"$Name"
exit
