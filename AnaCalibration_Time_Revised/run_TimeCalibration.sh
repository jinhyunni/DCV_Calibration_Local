#!/bin/bash

# Configuration to kill this process
cleanup() {
  echo "Killed by user input"
  exit 0
}

trap cleanup SIGINT

# Configurations

runarray=("91" "92" "92b" "93")
saveTcTxt=1

for run in ${runarray[@]}; do
  for ((period=0;period<=15;period++))
  do 
    runId="$run"
    #root -l -b -q "analysis_Make_TimePeakCorr.cpp(\"${runId}\", ${period})"
    #root -l -b -q "analysis_MakeAndFit_MainChTimePeak_Syst.cpp(\"${runId}\", ${period})"
    root -l -b -q "analysis_Make_TimeCalibration_Syst.cpp(\"${runId}\", ${period}, ${saveTcTxt})"
  done
done
