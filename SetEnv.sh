#!/usr/bin/bash

# Define environment variables for performing calibration
export DIR_BASE=$PWD

export DIR_HEADER="$DIR_BASE/headerFiles"
export DIR_COSMIC_TRACKED_FILE="$DIR_BASE/CosmicTrackedTTree"

export DIR_SET_CC_THRESH="$DIR_BASE/SetCCThresh"
export DIR_TIME_CAL="$DIR_BASE/AnaCalibration_Time_Revised"
export DIR_ENE_CAL="$DIR_BASE/AnaCalibration_Ene_Revised"
export DIR_CHECK_DCV_SIG="$DIR_BASE/AnaCheckDCVSignal"

# Print out settings
echo -e "-----------------------------------------------------------"
echo -e "Environment set for DCV Calibration\n"
printf "Base: ${DIR_BASE}\n"
printf "Cosmic Tracked Files in: ${DIR_COSMIC_TRACKED_FILE}\n"
printf "Setting threshold for cc modules in: ${DIR_SET_CC_THRESH}\n"
printf "Timing calibration in: ${DIR_TIME_CAL}\n"
printf "Energy calibration in: ${DIR_ENE_CAL}\n"
printf "Checking DCV signals in: ${DIR_CHECK_DCV_SIG}\n"
echo -e "------------------------------------------------------------"
