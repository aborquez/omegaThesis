#!/bin/bash

#PBS -N OID_usm-Pb
#PBS -V
#PBS -q utfsm
#PBS -l walltime=24:00:00
#PBS -l cput=24:00:00
#PBS -m ae
#PBS -M andres.borquez.14@sansano.usm.cl

SHDIR=/user/b/borquez/omega/OmegaID/scripts
cd $SHDIR

./run_OID-simrec.sh usm-Pb
./run_OID-gsim.sh usm-Pb
