#!/bin/sh

BRUTUS="$1"
let "progsOk = 0"
let "nbProgs = 0"
for progs in *.c
do
    let "nbProgs = nbProgs + 1"
    $BRUTUS $progs > /dev/null
    returncode=$?
    if [[ $returncode == 0 ]]
    then let "progsOk = progsOk + 1"
    fi
done
let "ratio = progsOk*100/nbProgs"
echo "Number of programs : $nbProgs"
echo "Number of tests passed : $progsOk"
echo "ratio : $ratio % "
exit $progsOk
