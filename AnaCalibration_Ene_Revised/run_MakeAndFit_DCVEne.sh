#!/usr/bin/bash

# Configuration to kill this process by user input
cleanup() {
  echo "Killed by user input"
  exit 0
}

trap cleanup SIGINT

# Configuration
runarray=("92")
objarray=(0 1 2)
cutoption=3 # 0~3(0:MB/1:TIME/2:IADP/3:IADPTIME)
saveaspdf=1

for run in ${runarray[@]}; do
  for ((period=0;period<10;period++)); do
    runId="$run"
    LOGFILE="Logs/CalculateNx_Run${runId}_Period${period}_CutOption${cutoption}.txt"
    exec > "$LOGFILE" 2>&1
    for obj in ${objarray[@]}; do
      # Perform making & fitting DCV energy distribution
      root -l -b -q "analysis_MakeAndFit_DCVEne.cpp(\"${runId}\", ${period}, ${obj}, ${cutoption}, ${saveaspdf})"
    done
    # Calculate calibration factor
    root -l -b -q "analysis_CalFactor_GainFactor.cpp(\"${runId}\", ${period}, ${cutoption})"
  done
done
