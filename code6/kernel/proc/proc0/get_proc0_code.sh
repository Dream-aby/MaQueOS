#!/bin/bash

GNU=../../../../cross-tool/bin/loongarch64-unknown-linux-gnu-

${GNU}gcc -nostdinc -c proc0.S -o proc0.o
${GNU}ld -N -e start -Ttext 0 -o proc0.out proc0.o
${GNU}objcopy -S -O binary proc0.out proc0

hexdump -Cv proc0 > t
awk '{print $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17}' t \
| sed 's/^/0x/g' | sed 's/ /, 0x/g' | sed 's/$/,/g' | sed 's/0x,//g' | sed 's/, 0x|.*|,//g' | sed ":a;$!N;s/,\n //g;ba"

rm -f proc0 proc0.o proc0.out t
