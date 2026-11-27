#!/bin/sh

ls *.bmp | awk \
'{
	newname=$1
	sub(/bmp/,"zbm",newname) 
	print "gzip " $1 " -c > " newname
}' | /bin/sh

ls *.wav | awk \
'{
	newname=$1
	sub(/wav/,"zwv",newname) 
	print "gzip " $1 " -c > " newname
}' | /bin/sh


tar czvf zombie-data.tar.gz *.zbm *.zwv *.png

rm *.zbm
rm *.zwv
