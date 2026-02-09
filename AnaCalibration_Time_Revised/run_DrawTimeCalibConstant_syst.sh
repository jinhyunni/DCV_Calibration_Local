#!/bin/bash

#configuration to kill this process
cleanup() {
  echo "Killed by user input\n"
  exit 0
}

trap cleanup SIGINT

#run the analysis
runarray=("92b")
drawRef=1
drawT1=0
drawT2=0
drawT3=0
drawTot=1

for run in ${runarray[@]}; do
  for ((period=0; period<11; period++)); do
  runId="$run"
  root -l -b -q "plotMake_CheckTimeCalibConstant_Syst.cpp(\"${runId}\", ${period}, ${drawRef}, ${drawT1}, ${drawT2}, ${drawT3}, ${drawTot})"
  done
done
