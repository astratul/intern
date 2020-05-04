#!/bin/sh
for file in $(ls *.sh); do
lpr $file
done
exit 0
