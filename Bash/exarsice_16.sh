#!/bin/bash

while read -r line
  do
   if [ $(echo $line | cut -d'=' -f 1 ) == $2 ];
      then 
         value1=$(echo $line | cut -d'=' -f 2 )
   elif [ $(echo $line | cut -d'=' -f 1 ) == $3 ];
      then 
         value2=$(echo $line | cut -d'=' -f 2 )
   fi
done < $1

for line2 in $value2 
 do
   boolean=false
   for line1 in $value1
     do 
     if [ $line2 == $line1 ];
        then 
           boolean=true
           break
     fi
   done
    
   if [ $boolean == "false" ] ;
      then
         value+="$line2 "
   fi 
   
done

replace="$3=$value2"
replace_with="$3=$value"

sed -i "s/$replace/$replace_with/g" "$1"
