#!/bin/sh

cd bin
./compile.sh xtsh 
./compile.sh print
./compile.sh share

dd if=/dev/zero of=xtfs.img bs=512 count=4096 2> /dev/null
../format 
../copy xtsh 1
../copy print 1
../copy share 1

rm -f xtsh print share
mv xtfs.img ../../run
cd ../
