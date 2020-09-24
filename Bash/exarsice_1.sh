#!/bin/bash
echo -n "Number of lines : "  
grep -v [a-w] ./philip-j-fry.txt | grep -c [02468]
exit
