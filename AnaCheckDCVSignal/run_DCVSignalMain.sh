#!/usr/bin/bash

# Exit if user wants
cleanup() {
  echo "Killed by user"
  exit 0
}

trap cleanup SIGINT

# Configurations
runarray=("92")
makeSubChCut=0
changeConfig=1
changeAttPathL=1
changeN2SubQc=0
changeModEneUpdated=1
changeTrigCtr=1

for run in ${runarray[@]}; do
  for ((period=0;period<15;period++)); do
    runId="$run"
    root -l -b -q "analysis_DCVSignalMain.cpp(\"${runId}\", ${period}, ${makeSubChCut}, ${changeConfig}, ${changeAttPathL}, ${changeN2SubQc}, ${changeModEneUpdated}, ${changeTrigCtr})"
  done
done
