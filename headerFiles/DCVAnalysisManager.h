#ifndef DCVANALYSISMANAGER
#define DCVANALYSISMANAGER

#include "DCVAnalysis.h"
#include "DCVAttPathL.h"
#include "DCVAnalysisLogger.h"

const int DEFAULTSTARTRUNID = 81;

std::map<std::string, int> RunIndex = {
  {"81", 0},
  {"82", 1},
  {"83", 2},
  {"84", 3},
  {"85", 4},
  {"86", 5},
  {"87", 6},
  {"88", 7},
  {"89", 8},
  {"90", 9},
  {"91", 10},
  {"92", 11},
  {"92b", 12},
  {"93", 13},
};

int ReturnRunIndex(string input) {
  // Search if key exist
  auto it = RunIndex.find(input);

  if (it == RunIndex.end()) {
    DCV_LOG_ERROR("Input run id does not exists: " << input);
    return -1;
  } else {
    DCV_LOG_INFO("Input run id has parameter index : " << it->second);
    return it->second;
  } 
}

/*
 *	! Caution
 *	If any of member data of DCVAnalysis class has been changed,
 *	to see the effect of chage, re-run CalNx.pl code.
 *	Or at least run analysis_MakeHis_~.cpp codes to apply the changes.
 */ 

vector<vector<DCVAnalysis*>> ParForCal=
{
	//! Run81
	{
		new DCVAnalysis(new DCVFitParameter(25, 90, 45, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 4.00), 2),	//Period00
		new DCVAnalysis(new DCVFitParameter(25, 90, 45, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 6.00)),	//Period01
		new DCVAnalysis(new DCVFitParameter(25, 90, 45, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 6.00), 2),	//Period02
		new DCVAnalysis(new DCVFitParameter(25, 90, 45, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 6.00), 2),	//Period03
		new DCVAnalysis(new DCVFitParameter(25, 90, 45, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 6.00)),	//Period04
		new DCVAnalysis(new DCVFitParameter(25, 90, 45, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 6.00)),	//Period05
	},

	//! Run82
	{
	},

	//! Run83
	{
	},

	//! Run84
	{
	},

	//! Run85
	{
	},

	//! Run86
	{
	},

	//! Run87
	{
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.6, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period00 => Testing for improvement(2025.3.2)
		//new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.6, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00)),	//Period00 => Testing for improvement(2025.3.2)
		//new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.6, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00)),	//Period00 => Testing for improvement(2025.3.2)
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.6, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period01
		//new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.6, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00)),	//Period01
		//new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.6, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00)),	//Period01
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period02
		//new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00)),	//Period02
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period03
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period04
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.20, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period05
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period06
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00), new DCVCut({"Time", 24.0, 31.0})),	//Period07
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00)),	//Period08
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.00}, 0.25, 4.00)),	//Period09
	},

	//! Run88
	{
	},

	//! Run89
	{
	},

	//! Run90
	{
		new DCVAnalysis(new DCVFitParameter(20, 90, 20, 90, 1.2, 7.0, 0.30, 3.00, {false, 0.40, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00)),	//Period00
	},

	//! Run91
	{
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 2),	//Period00
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.20}, {false, 5.00, 4.00}, 0.25, 4.00), 2),	//Period01
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.40}, {false, 3.00, 3.00}, 0.25, 4.00), 1),	//Period02
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 3.00}, 0.25, 4.00), 1),	//Period03
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.40, 0.25}, {false, 4.00, 4.00}, 0.25, 4.00), 1),	//Period04
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.20, 0.30}, {false, 4.00, 3.00}, 0.20, 5.00), 1),	//Period05
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.40, 0.35}, {false, 4.00, 4.00}, 0.25, 4.00), 1),	//Period06
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.35}, {false, 5.00, 5.00}, 0.25, 4.00), 1),	//Period07
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 5.00), 1),	//Period08
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 5.00, 3.50}, 0.25, 6.00), 1),	//Period09
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.30, 4.00, {false, 0.20, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 1),	//Period10
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 2),	//Period11 : MC
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 2),	//Period12 : MC
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 2),	//Period13 : MC
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 2),	//Period14 : MC X
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00), 2),	//Period15 : MC X
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.30, 4.00), 1),	//Period16 : MC -> Energy calculation fixed
		//new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.50, 5.00), 1),	//Period16 : MC -> Energy calculation fixed
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.50, 5.00), 2),	//Period17 : MC -> Energy calculation fixed
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.50, 5.00), 2),	//Period18 : MC -> Energy calculation fixed
		new DCVAnalysis(new DCVFitParameter(20, 90, 40, 90, 0.7, 7.0, 0.45, 4.00, {false, 0.30, 0.30}, {false, 4.00, 4.00}, 0.50, 5.00), 2),	//Period19 : MC -> Energy calculation fixed
	},

	//! Run92
	{
		#if 0 // For default trigger option
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00)),	//Period00 : Updated(2025.3.3)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 3.00, 4.00}, 0.25, 4.00)),	//Period01
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 4.00, 4.00}, 0.25, 4.00)),	//Period02
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 3.00, 4.00}, 0.25, 4.00)),	//Period03
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00)),	//Period04
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00)),	//Period05
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00)),	//Period06
		#endif	

		#if 1 // For updated trigger option -> Integrated
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.30, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 26.0}), 1),	//Period00 : Updated(2025.6.22)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 26.0}), 1),	//Period01 : Updated(2025.6.22)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0}), 1),	//Period02 : Updated(2025.6.22)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0}), 1),	//Period03 : Updated(2025.6.22)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0}), 1),	//Period04 : Updated(2025.6.22)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0}), 1),	//Period05 : Updated(2025.6.22)
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 6.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0}), 1),	//Period06 : Updated(2025.6.22)
		#endif	
	},

	//! Run92b
	{
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0})),	//Period00 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00)),	//Period01
		//new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00)),	//Period02 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0})),//Period02 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.5, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0})),	//Period03
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 20.0, 26.0}, {"IADP", 0.0, 20.0})),	//Period04 
	},

  //! Run93
  {
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 21.0, 27.0}, {"IADP", 0.0, 20.0})),	//Period00 
		//new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 22.0, 28.0}, {"IADP", 0.0, 20.0})),	//Period00 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 21.0, 27.0}, {"IADP", 0.0, 20.0})),	//Period01 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 21.0, 27.0}, {"IADP", 0.0, 20.0})),	//Period02 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 21.0, 27.0}, {"IADP", 0.0, 20.0})),	//Period03 
		new DCVAnalysis(new DCVFitParameter(30, 90, 40, 90, 0.7, 7.0, 0.30, 3.00, {false, 0.25, 0.30}, {false, 6.00, 4.00}, 0.25, 4.00), new DCVCut(true, {"Time", 21.0, 27.0}, {"IADP", 0.0, 20.0})),	//Period04 
  },

};

#endif
