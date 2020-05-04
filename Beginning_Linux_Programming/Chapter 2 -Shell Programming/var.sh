#!/bin/sh
myvar=”Hi there”
echo
echo
echo
echo
$myvar
"$myvar"
'$myvar'
\$myvar
echo Enter some text
read myvar
echo '$myvar' now equals $myvar
exit 0
