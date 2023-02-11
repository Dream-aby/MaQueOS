#!/bin/sh

cd bin
./compile.sh xtsh 
./compile.sh print

dd if=/dev/zero of=xtfs.img bs=512 count=4096 2> /dev/null
../format 
../copy xtsh 1
../copy print 1

rm -f xtsh print
mv xtfs.img ../../run
cd ../
