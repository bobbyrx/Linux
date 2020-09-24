#!/bin/bash
echo -n "Number of imputs: "; read imput
i=0
newFile=$(mktemp)
while [ $i -lt $(( $imput)) ]; do
	echo -n "Imput number[${i}]: "; read number
        if [ $(echo $number | egrep "^[-]?[1-9][0-9]*$" | wc -l) -eq 1 ]; then
		echo $number >> $newFile
	fi	
	i=$(($i+1))
done	
if [ $(cat $newFile| wc -l ) -eq 0 ]; then
	echo "You haven't imputed any real whole numbers"
	exit 1
fi
max=-1
newFile2=$(mktemp)
while read num;do
	echo $num | tr "\n" " " >> $newFile2
	if [ $num -le 0 ]; then
		num=$(($num*(-1)))
	fi	
	count=0
	while [ $(($num)) -ne 0 ]; do
		count=$(($count+$num%10))
		num=$(($num/10))		
	done 	
	if [ $max -lt $count ]; then
		max=$count
	fi	
	echo $count >> $newFile2
done < $newFile	

cat $newFile2 | egrep "${max}" | cut -d" " -f1  | sort -n  | uniq | head -n 1

rm -- $newFile
rm -- $newFile2

exit 0

