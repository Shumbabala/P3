#!/bin/bash

power=${1:-40}
r1=${2:-0.6} 
rlag=${3:-0.6} 
z=${4:-1300}
thr=${5:-0.0}
offset=${6:-0.0}

# Establecemos que el código de retorno de un pipeline sea el del último programa con código de retorno
# distinto de cero, o cero si todos devuelven cero.
set -o pipefail

# Put here the program (maybe with path)
GETF0="get_pitch -p $power --r1 $r1 --rlag $rlag -z $z --thr $thr --offset $offset"

for fwav in pitch_db/train/*.wav; do
    ff0=${fwav/.wav/.f0}
    #echo "$GETF0 $fwav $ff0 ----"
	$GETF0 $fwav $ff0 > /dev/null || ( echo -e "\nError in $GETF0 $fwav $ff0" && exit 1 )
done

pitch_evaluate pitch_db/train/*.f0ref

exit 0
