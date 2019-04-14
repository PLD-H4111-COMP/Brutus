#!/bin/bash

BRUTUS="./compile.sh"
let "progsOk = 0"
let "nbProgs = 0"
for progs in $(find progs/customTests -name "*.c")
do
    echo "Testing" $progs :
    let "nbProgs = nbProgs + 1"
    $BRUTUS -o /dev/null $progs
    returncode=$?
    if [[ $returncode == 0 ]]
    then echo "OK" && let "progsOk = progsOk + 1"
    else echo "Error"
    fi
    echo ""
done
let "ratio = progsOk*100/nbProgs"
echo "Number of programs : $nbProgs"
echo "Number of tests passed : $progsOk"
echo "ratio : $ratio % "

if [[ $progsOk == $nbProgs ]]
then exit 0
else exit $(($nbProgs - $progsOk))
fi
