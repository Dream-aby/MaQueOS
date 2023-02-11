#!/bin/bash

debug=$1
if [[ "$debug" == "-d" ]];then
	debug="-s -S"
fi

ps -ef|grep system-loongarch64 | awk '{if($4!="0") {cmd="kill -9 " $2; print cmd; system(cmd);}}'
rm -f kernel

cd ../kernel 
make clean
make  
cp kernel ../run  
make clean 
cd ../run 

../../cross-tool/qemu-system-loongarch64 -vga std -m 2G -smp 1 \
-bios ../../cross-tool/loongarch_bios_0310_debug.bin \
-kernel kernel ${debug} &&

rm -f kernel
