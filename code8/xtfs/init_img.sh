#!/bin/sh

echo -n "xtsh# " > tmp

dd if=/dev/zero of=xtfs.img bs=512 count=4096 2> /dev/null
dd if=tmp of=xtfs.img conv=notrunc 2> /dev/null

mv xtfs.img ../run

rm tmp