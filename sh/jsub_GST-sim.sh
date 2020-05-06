#!/bin/bash

#####################################################################
# ./jsub_GST-sim.sh --targ <target> --dir1 <ndir1> --dir2 <ndir2>   #
#     <target> = (D, C, Fe, Pb)                                     #
#     <ndir>   = (0, ..., 24) (D)                                   #
#                (0, ..., 7)  (C)                                   #
#                (0, ..., 10) (Fe)                                  #
#                (0, ..., 3)  (Pb)                                  #
#                                                                   #
# EG: ./run_FilterNCombine.sh --targ Pb --dir1 0 --dir2 3           #
#                                                                   #
#####################################################################
#                                                                   #
# Version:                                                          #
# --- It works only for jlab cluster                                #
# --- Only purpose to filter jlab simulations                       #
#                                                                   #
#####################################################################

#####
# Functions
###

function get_run()
{
  sr=$1
  srn=""
  if [ $sr -lt 10 ]; then
    srn="0$sr"
  else
    srn="$sr"
  fi
  echo $srn
}

#####
# Main
###

inputArray=("$@")

ic=0
while [ $ic -le $((${#inputArray[@]}-1)) ]; do
  if [ "${inputArray[$ic]}" == "--targ" ]; then
    tarName=${inputArray[$((ic+1))]}
  elif [ "${inputArray[$ic]}" == "--dir1" ]; then
    nDir1=${inputArray[$((ic+1))]}
  elif [ "${inputArray[$ic]}" == "--dir2" ]; then
    nDir2=${inputArray[$((ic+1))]}
  else
    printf "*** Aborting: Unrecognized argument: ${inputArray[$((ic))]}. ***\n\n";
  fi
  ((ic+=2))
done

TMPDIR="${PRODIR}/tmp/GST"
mkdir -p ${TMPDIR}
cd ${TMPDIR}

# Set environment variables
source ~/.bashrc

for (( id=$nDir1; id<=$nDir2; id++ )); do
  sdir=$(get_run "$id")

  OUDIR=${PRODIR}/out/prunedSim/jlab/${tarName}/${sdir}
  
  jobfile="${TMPDIR}/job_jlab-${tarName}-${sdir}.xml"
  jobname="GST_jlab-${tarName}-${sdir}"
  
  echo ${jobname}

  opt="-t${tarName} -Sjlab -n${sdir}"
  
  echo "<Request>"                                                                            > $jobfile
  echo "  <Project name=\"eg2a\"></Project>"                                                 >> $jobfile
  echo "  <Track name=\"analysis\"></Track>"                                                 >> $jobfile
  echo "  <Email email=\"andres.borquez.14@sansano.usm.cl\" request=\"true\" job=\"true\"/>" >> $jobfile
  echo "  <TimeLimit time=\"500\" unit=\"minutes\"></TimeLimit>"                             >> $jobfile
  echo "  <DiskSpace space=\"500\" unit=\"MB\"></DiskSpace>"                                 >> $jobfile
  echo "  <Memory space=\"1000\" unit=\"MB\"></Memory>"                                      >> $jobfile
  echo "  <CPU core=\"1\"></CPU>"                                                            >> $jobfile
  echo "  <OS name=\"general\"></OS>"                                                        >> $jobfile
  echo "  <Job>"                                                                             >> $jobfile
  echo "    <Name name=\"${jobname}\"></Name>"                                               >> $jobfile
  echo "    <Command><![CDATA["                                                              >> $jobfile
  echo "      source ~/.bashrc"                                                              >> $jobfile
  echo "      cd ${PRODIR}"                                                                  >> $jobfile
  for (( rn=0; rn<100; rn++ )); do
    srn=$(get_run "$rn")
    echo "      ./bin/GetSimpleTuple ${opt} -r${srn}"                                           >> $jobfile
  done  
  echo "    ]]></Command>"                                                                   >> $jobfile
  echo "  </Job>"                                                                            >> $jobfile
  echo "</Request>"                                                                          >> $jobfile
  
  echo "Running job ${jobfile}"
  jsub --xml ${jobfile}
done