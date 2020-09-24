#!/bin/bash
echo -n "Number of imputs: "; read imput
i=0
newFile=$(mktemp)
while [ $i -lt $(($imput)) ]; do
	echo -n "Imput number[${i}]: ";read number
	if [ $(echo $number | egrep "^[-]?[1-9][0-9]*$" | wc -l) -eq 1 ]; then
		echo $number >> $newFile
	fi	
	i=$(( $i + 1))
done	

if [ $( cat $newFile | wc -l ) -eq 0 ]; then
	echo "You haven't imputed any real whole numbers"
	exit 1
fi	

maxAbs=0

while read num; do
	if [ $num -lt 0 ]; then
		num=$(( $num * (-1)))
	fi	
	if [ $maxAbs -lt $num ]; then
		maxAbs=$num
	fi	
done < $newFile	
cat $newFile | grep "${maxAbs}" | sort -n | uniq 
rm -- $newFile
exit 0
