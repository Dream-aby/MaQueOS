#!/bin/bash

bin=$1

GNU=../../../cross-tool/bin/loongarch64-unknown-linux-gnu-

${GNU}gcc -nostdinc -c ${bin}.S -o ${bin}.o
${GNU}ld -z max-page-size=4096 -N -e start -Ttext 0 -o ${bin}.tmp ${bin}.o
${GNU}objcopy -S -O binary ${bin}.tmp ${bin}.out

let size=`stat -c %s ${bin}.out`
let size=($size+0xfff)/0x1000
let count=$size*8+1
dd if=/dev/zero of=${bin} bs=512 count=${count} 2> /dev/null
dd if=${bin}.out of=${bin} bs=512 seek=1 conv=notrunc 2> /dev/null

let size=$size*0x1000
size=`printf "%08x" $size`

for ((i=0;i<4;i++))
do
    length+=`echo ${size:0-2:2}`
    size=`echo ${size%??}`
done

echo -n "xt" > header
echo $length | xxd -p -r >> header
dd if=header of=${bin} bs=512 seek=0 conv=notrunc 2> /dev/null

chmod 0777 ${bin}

rm -f ${bin}.o ${bin}.d ${bin}.tmp ${bin}.out header
