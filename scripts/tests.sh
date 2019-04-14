#!/bin/sh

BRUTUS="$(readlink -f Brutus)"
cd progs/tests
let "progsOk = 0"
let "nbProgs = 0"
for folder in *
    do
    cd $folder
    for progs in *.c
    do
        let "nbProgs = nbProgs + 1"
    done
    echo "Folder $folder : "
    ./launchTests.sh $BRUTUS
    let "progsOk = progsOk + $?"
    cd ..
    echo ""
done
echo ""
echo "Total number of programs : $nbProgs"
echo "Total number of tests passed : $progsOk"
let "ratio = progsOk*100/nbProgs"
echo "Total ratio : $ratio % "
exit
