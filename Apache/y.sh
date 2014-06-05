#!/usr/bin/bash
RFID=`echo "$QUERY_STRING" | grep -oE "(^|[?&])rfid=[0-9ABCDEF]+" | cut -f 2 -d "=" | head -n1`
RFID=$(echo i"$RFID"x)
#echo "$RFID"
if grep --quiet "$RFID" rfidlist.txt  
then
	echo -n act=O
else
	echo -n act=C
fi
