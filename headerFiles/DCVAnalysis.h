#ifndef DCVANALYSIS
#define DCVANALYSIS
//-----------------------------------------------------------------------------------------------
class DCVFitParameter
{
	public:

		// Struct for N2 fitting
		struct N2Set
		{
			int switchSubQC;
			double returnValue_off;
			double returnValue_on;
		};

		//Constructor
		DCVFitParameter();
		DCVFitParameter(
				int CC04A, int CC04B, int CC05A, int CC05B,
				double FinalA, double FinalB,
				double N1FitA, double N1FitB,
				N2Set N2InputsMin, N2Set N2InputsMax,
				double N3FitA, double N3FitB);

		//Destructor
		~DCVFitParameter();

		// Method
		// ======
		double SetterN2FitFactor(int OptionN2SubQC, double returnOff, double returnOn)
		{
			if( OptionN2SubQC == 0 )
			{
				return returnOff;

			} else if(OptionN2SubQC == 1){

				return returnOn;

			} else{
				//cout << "Given wrong option. Return Trash value" << endl;
				return 99999.;
			}
		}

		//Configurables for first fitting
		int CC04FirstFitStart;		//start axis range for first-fit CC04
		int CC04FirstFitFinish;		//finish axis range for first-fit CC04
		int CC05FirstFitStart;		//start axis range for first-fit CC05
		int CC05FirstFitFinish;		//finish axis range for first-fit CC05

		//Determining final fit range
		// fit start	: MPV - aFactor * width
		// fit finish	: MPV + bFactor * width
		// Systemetic study of how aFactor, bFactor affects gainFactor
		// These a,b Facotrs are just trial values
		double aFactor;
		double bFactor;

		//Determining languas fitting range for Calculating Nx
		// fit start : h1 -> GetMean() * NxFitMinFactor
		double N1FitMinFactor;
		double N1FitMaxFactor;
		double N2FitMinFactor;
		double N2FitMaxFactor;
		double N3FitMinFactor;
		double N3FitMaxFactor;

};

// Basic constructor
DCVFitParameter::DCVFitParameter()
{
	CC04FirstFitStart = 25;
	CC04FirstFitFinish= 90;
	CC05FirstFitStart = 45;
	CC05FirstFitFinish= 90;

	aFactor = 0.7;
	bFactor = 7.0;

	N1FitMinFactor = 0.30;
	N1FitMaxFactor = 3.00;
	N2FitMinFactor = 0.30;
	N2FitMaxFactor = 3.00;
	N3FitMinFactor = 0.25;
	N3FitMaxFactor = 4.00;

}

// Constructor
DCVFitParameter::DCVFitParameter(int CC04A, int CC04B, int CC05A, int CC05B, double FinalA, double FinalB, double N1FitA, double N1FitB, N2Set N2InputsMin, N2Set N2InputsMax, double N3FitA, double N3FitB)
{
	CC04FirstFitStart	= CC04A;
	CC04FirstFitFinish 	= CC04B;
	CC05FirstFitStart	= CC05A;
	CC05FirstFitFinish	= CC05B;

	aFactor = FinalA;
	bFactor = FinalB;

	N1FitMinFactor = N1FitA;
	N1FitMaxFactor = N1FitB;

	N2FitMinFactor = DCVFitParameter::SetterN2FitFactor( N2InputsMin.switchSubQC, N2InputsMin.returnValue_off, N2InputsMin.returnValue_on );
	N2FitMaxFactor = DCVFitParameter::SetterN2FitFactor( N2InputsMax.switchSubQC, N2InputsMax.returnValue_off, N2InputsMax.returnValue_on );

	N3FitMinFactor = N3FitA;
	N3FitMaxFactor = N3FitB;
}

// Destructor
DCVFitParameter::~DCVFitParameter(){}
//-----------------------------------------------------------------------------------------------
class DCVCut
{
	public:

		// Struct to hold cut information
		struct DCVCutType
		{
			TString CutName;
			float LE; // Cut low edge
			float HE; // Cut high edge
		};

    // structor
		DCVCut();
		DCVCut(bool option, DCVCutType time, DCVCutType integratedAdcDivPeak);
		DCVCut(DCVCutType time1, DCVCutType integratedAdcDivPeak1, DCVCutType time2, DCVCutType integratedAdcDivPeak2);
		DCVCut(DCVCutType cut);
		~DCVCut();

		// Member data
		// Default cut value
		DCVCutType TimeCut;
		DCVCutType IADPCut;
		// Cut value for UT data sets
		DCVCutType TimeCutUT;
		DCVCutType IADPCutUT;
};

// Default constructor
// ! Holds default cut value
DCVCut::DCVCut()
{
	TimeCut={"Time", 20., 27.};
	IADPCut={"IADP", 0., 20.};
	TimeCutUT={"Time", 20., 26.};
	IADPCutUT={"IADP", 0., 20.};
}

// Constructor with input cut infos
// ! With this constructor, users can set cut values by runs and periods
// ! Must set two cuts at the same time
DCVCut::DCVCut(DCVCutType time1, DCVCutType integratedAdcDivPeak1, DCVCutType time2, DCVCutType integratedAdcDivPeak2)
{
	TimeCut = time1;
	IADPCut = integratedAdcDivPeak1;
	TimeCutUT= time2;
	IADPCutUT= integratedAdcDivPeak2;
}

// Constructor to change either default cut value or cut values for UT dataset
DCVCut::DCVCut(bool option, DCVCutType time, DCVCutType iadp)
{
	if (!option) {	// Set default cut values
		TimeCut = time;
		IADPCut = iadp;
		TimeCutUT={"Time", 20., 26.};
		IADPCutUT={"IADP", 0., 20.};
	} else {		// Set UT dataset's cut values
		TimeCut={"Time", 20., 27.};
		IADPCut={"IADP", 0., 20.};
		TimeCutUT = time;
		IADPCutUT = iadp;
	}
}

// Constructor with input cut infos
// ! With this constructor, users can set cut values by runs and periods
// ! Specify time cut or IADP cut -> Just for default cut
// ! Not intuitive....FIXME
DCVCut::DCVCut(DCVCutType cut)
{

	if( cut.CutName.Contains("Time") )
	{
		TimeCut = cut;
		IADPCut = {"IADP", 0., 20.};
	}

	if( cut.CutName.Contains("IADP") )
	{
		TimeCut = {"Time", 20., 27.};
		IADPCut = cut;
	}
}

DCVCut::~DCVCut(){}

//-----------------------------------------------------------------------------------------------
class DCVAnalysis : public DCVCut, public DCVFitParameter
{
	public:
		DCVAnalysis();																												                                                                  // -> Basic constructor
		DCVAnalysis(DCVFitParameter* fitpara);																						                                                      // -> Set FitParameters
		DCVAnalysis(DCVFitParameter* fitpara, DCVCut* cut);																			                                                // -> Set FitParameters, DCVCut
		DCVAnalysis(DCVFitParameter* fitpara, int nRebin);																			                                                // -> Set FitParameters, fNRebin
		DCVAnalysis(DCVFitParameter* fitpara, DCVCut* cut, int nRebin);																                                          // -> Set FitParameters, DCVCut, fNRebin
		DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitpara);					                  // -> Set FitParameters
		DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitpara, DCVCut* cut);              // -> Set FitParameters, DCVCut
		DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitpara, int nRebin);	              // -> Set FitParameters, fNRebin
		DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitpara, DCVCut* cut, int nRebin);  // -> Set FitParameters, DCVCut, fNRebin
	
		~DCVAnalysis();

		bool fAttPathL;
		bool fN2SubQC;
		bool fModEneUpdated;
		bool fTrigCtr;

		int fNRebin=1;

		TString infoAttPathL;
		TString infoN2SubQC;
		TString infoModEneUpdated;
		TString infoTrigCtr;

		// Methods to set configurations
		void setAttPathL(bool option) {
			fAttPathL		= option;
			infoAttPathL 	= (fAttPathL) ? "" : "_NormTo1";
		}

		void setN2SubQC(bool option) {
			fN2SubQC		= option;
			infoN2SubQC 	= (fN2SubQC) ? "_SubModQCApplied" : "";
		}

		void setModEneUpdated(bool option) {
			fModEneUpdated		= option;
			infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
		}

		void setTrigCtr(bool option) {
			fTrigCtr	= option;
			infoTrigCtr	= (fTrigCtr) ? "_UT" : "";
			if (option) {
				TimeCut = TimeCutUT;
				IADPCut = IADPCutUT;
			}
		}

    // Method to change cut values manually
    void setCutValue(DCVCutType givenCut) {
      if (givenCut.CutName == "Time") {
        TimeCut = givenCut;  
      } 
      if (givenCut.CutName == "IADP") {
        IADPCut = givenCut;
      }
    }
};

// Default constructor
DCVAnalysis::DCVAnalysis()
{
	DCVFitParameter basicFitParameter;
	DCVCut basicCut;

	CC04FirstFitStart	= basicFitParameter.CC04FirstFitStart;
	CC04FirstFitFinish	= basicFitParameter.CC04FirstFitFinish; 
	CC05FirstFitStart	= basicFitParameter.CC05FirstFitStart;
	CC05FirstFitFinish	= basicFitParameter.CC05FirstFitFinish;
	aFactor				= basicFitParameter.aFactor;
	bFactor				= basicFitParameter.bFactor;
	N1FitMinFactor		= basicFitParameter.N1FitMinFactor;
	N1FitMaxFactor		= basicFitParameter.N1FitMaxFactor;
	N2FitMinFactor		= basicFitParameter.N2FitMinFactor;
	N2FitMaxFactor		= basicFitParameter.N2FitMaxFactor;
	N3FitMinFactor		= basicFitParameter.N3FitMinFactor;
	N3FitMaxFactor		= basicFitParameter.N3FitMaxFactor;
	
	TimeCut				= basicCut.TimeCut;
	IADPCut				= basicCut.IADPCut;
	TimeCutUT			= basicCut.TimeCutUT;
	IADPCutUT			= basicCut.IADPCutUT;

	fAttPathL			= true;
	fN2SubQC			= false;
	fModEneUpdated		= true;
	fTrigCtr			= false;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= 1;
}

// Set FitParameters
DCVAnalysis::DCVAnalysis(DCVFitParameter* fitparams)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	DCVCut defaultCut;
	TimeCut				= defaultCut.TimeCut;
	IADPCut				= defaultCut.IADPCut;
	TimeCutUT			= defaultCut.TimeCutUT;
	IADPCutUT			= defaultCut.IADPCutUT;

	fAttPathL			= true;
	fN2SubQC			= false;
	fModEneUpdated		= true;
	fTrigCtr			= false;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= 1;
}

// Set FitParameters, DCVCut
DCVAnalysis::DCVAnalysis(DCVFitParameter* fitparams, DCVCut* cuts)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	TimeCut				= cuts -> TimeCut;
	IADPCut				= cuts -> IADPCut;
	TimeCutUT			= cuts -> TimeCutUT;
	IADPCutUT			= cuts -> IADPCutUT;

	fAttPathL			= true;
	fN2SubQC			= false;
	fModEneUpdated		= true;
	fTrigCtr			= false;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= 1;
}

// Set FitParameters, fNRebin 
DCVAnalysis::DCVAnalysis(DCVFitParameter* fitparams, int nRebin)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	DCVCut defaultCut;

	TimeCut				= defaultCut.TimeCut;
	IADPCut 			= defaultCut.IADPCut;
	TimeCutUT			= defaultCut.TimeCutUT;
	IADPCutUT			= defaultCut.IADPCutUT;

	fAttPathL			= true;
	fN2SubQC			= false;
	fModEneUpdated		= true;
	fTrigCtr			= false;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin 			= nRebin;
}

// Set FitParameters, DCVCuts, fNRebin 
DCVAnalysis::DCVAnalysis(DCVFitParameter* fitparams, DCVCut* cuts, int nRebin)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	TimeCut				= cuts -> TimeCut;
	IADPCut				= cuts -> IADPCut;
	TimeCutUT			= cuts -> TimeCutUT;
	IADPCutUT			= cuts -> IADPCutUT;

	fAttPathL			= true;
	fN2SubQC			= false;
	fModEneUpdated		= true;
	fTrigCtr			= false;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= nRebin;
}

// Set FitParameters
DCVAnalysis::DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitparams)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	DCVCut defaultCut;
	TimeCut				= defaultCut.TimeCut;
	IADPCut				= defaultCut.IADPCut;
	TimeCutUT			= defaultCut.TimeCutUT;
	IADPCutUT			= defaultCut.IADPCutUT;

	// Analysis options configured
	fAttPathL			= ffAttPathL;
	fN2SubQC			= ffN2SubQC;
	fModEneUpdated		= ffModEneUpdated;
	fTrigCtr			= ffTrigCtr;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= 1;
}

// Set FitParameters, DCVCut
DCVAnalysis::DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitparams, DCVCut* cuts)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	TimeCut				= cuts -> TimeCut;
	IADPCut				= cuts -> IADPCut;
	TimeCutUT			= cuts -> TimeCutUT;
	IADPCutUT			= cuts -> IADPCutUT;

	// Analysis options configured
	fAttPathL			= ffAttPathL;
	fN2SubQC			= ffN2SubQC;
	fModEneUpdated		= ffModEneUpdated;
	fTrigCtr			= ffTrigCtr;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= 1;
}

// Set FitParameters, fNRebin 
DCVAnalysis::DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitparams, int nRebin)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	DCVCut defaultCut;

	TimeCut				= defaultCut.TimeCut;
	IADPCut 			= defaultCut.IADPCut;
	TimeCutUT			= defaultCut.TimeCutUT;
	IADPCutUT			= defaultCut.IADPCutUT;
	
	// Analysis options configured
	fAttPathL			= ffAttPathL;
	fN2SubQC			= ffN2SubQC;
	fModEneUpdated		= ffModEneUpdated;
	fTrigCtr			= ffTrigCtr;

	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin 			= nRebin;
}

// Set FitParameters, DCVCuts, fNRebin 
DCVAnalysis::DCVAnalysis(bool ffAttPathL, bool ffN2SubQC, bool ffModEneUpdated, bool ffTrigCtr, DCVFitParameter* fitparams, DCVCut* cuts, int nRebin)
{
	CC04FirstFitStart	= fitparams -> CC04FirstFitStart;
	CC04FirstFitFinish	= fitparams -> CC04FirstFitFinish;
	CC05FirstFitStart	= fitparams -> CC05FirstFitStart;
	CC05FirstFitFinish	= fitparams -> CC05FirstFitFinish;
	aFactor				= fitparams -> aFactor;
	bFactor				= fitparams -> bFactor;
	N1FitMinFactor		= fitparams -> N1FitMinFactor;
	N1FitMaxFactor		= fitparams -> N1FitMaxFactor;
	N2FitMinFactor		= fitparams -> N2FitMinFactor;
	N2FitMaxFactor		= fitparams -> N2FitMaxFactor;
	N3FitMinFactor		= fitparams -> N3FitMinFactor;
	N3FitMaxFactor		= fitparams -> N3FitMaxFactor;

	TimeCut				= cuts -> TimeCut;
	IADPCut				= cuts -> IADPCut;
	TimeCutUT			= cuts -> TimeCutUT;
	IADPCutUT			= cuts -> IADPCutUT;

	// Analysis options configured
	fAttPathL			= ffAttPathL;
	fN2SubQC			= ffN2SubQC;
	fModEneUpdated		= ffModEneUpdated;
	fTrigCtr			= ffTrigCtr;
	
	infoAttPathL 		= (fAttPathL) ? "" : "_NormTo1";
	infoN2SubQC 		= (fN2SubQC) ? "_SubModQCApplied" : "";
	infoModEneUpdated 	= (fModEneUpdated) ? "_ModEneUpdated" : "";
	infoTrigCtr 		= (fTrigCtr) ? "_UT" : "";

	fNRebin				= nRebin;
}

DCVAnalysis::~DCVAnalysis(){}

#endif
