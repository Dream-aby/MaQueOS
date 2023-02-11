#!/bin/sh

cd bin
./compile.sh xtsh 
./compile.sh print
./compile.sh share
./compile.sh shmem
./compile.sh hello

dd if=/dev/zero of=xtfs.img bs=512 count=4096 2> /dev/null
../format 
../copy xtsh 1
../copy print 1
../copy share 1
../copy shmem 1
../copy hello 1

rm -f xtsh print share shmem hello
mv xtfs.img ../../run
cd ../
