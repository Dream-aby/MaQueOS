#!/bin/sh

cd bin
./compile.sh xtsh 
./compile.sh print
./compile.sh share
./compile.sh shmem
./compile.sh hello
./compile.sh read
./compile.sh write
./compile.sh create
./compile.sh destroy
./compile.sh sync

dd if=/dev/zero of=xtfs.img bs=512 count=4096 2> /dev/null
../format 
../copy xtsh 1
../copy print 1
../copy share 1
../copy shmem 1
../copy hello 1
../copy read 1
../copy write 1
../copy create 1
../copy destroy 1
../copy sync 1

rm -f xtsh print share shmem hello read write create destroy sync
mv xtfs.img ../../run
cd ../
