#ifndef DCVATTPATHL
#define DCVATTPATHL

// Values needed to calculate FCalib
// Taken from previous studies

bool thisFTrigCtr = false;

const float lambda_DCV1 = 2469;			// Attenuation Length of DCV1
const float lambda_DCV1_Err = 165.1;
const float pos_DCV1 = 7835;			// Center position of DCV1
const float pos_CC04 = 7540; 			// Center position of CC04
const float fullL_DCV1 = 1410.0;		// Length of DCV1
const float halfL_DCV1 = fullL_DCV1/2;	// Half-length of DCV1

// Attenuation factor of DCV1
//*const float att1_u = exp((pos_DCV1 - pos_CC04)/lambda_DCV1);
//*const float att1_d = exp(-(pos_DCV1 - pos_CC04)/lambda_DCV1);
const float att1_u = exp(-(halfL_DCV1-(pos_DCV1-pos_CC04))/lambda_DCV1); //! pos_DCV1 - pos_CC04 returns positive value
const float att1_d = exp(-(halfL_DCV1+(pos_DCV1-pos_CC04))/lambda_DCV1); //! pos_DCV1 - pos_CC04 returns positive value

const float lambda_DCV2 = 2566;			// Attenuation Length of DCV2
const float lambda_DCV2_Err = 166.0;
const float pos_DCV2 = 9346; 			// Center position of DCV2
//const float pos_CC05 = 8899; 			// Center position of CC05
const float pos_CC05 = 8918.5; 			// Center position of CC05 -> Fixed based on MC generation information
const float fullL_DCV2 = 1525.0;		// Length of DCV2
const float halfL_DCV2 = fullL_DCV2/2;	// Half-length of DCV2

// Attenuation factor of DCV2
//*const float att2_u = exp((pos_DCV2 - pos_CC05)/lambda_DCV2);
//*const float att2_d = exp(-(pos_DCV2 - pos_CC05)/lambda_DCV2);
const float att2_u = exp(-(halfL_DCV2-(pos_DCV2-pos_CC05))/lambda_DCV2); //! pos_DCV2 - pos_CC05 returns positive value
const float att2_d = exp(-(halfL_DCV2+(pos_DCV2-pos_CC05))/lambda_DCV2); //! pos_DCV2 - pos_CC05 returns positive value

const float att[32] = {

					att1_u,att1_u,att1_d,att1_d,
					att1_u,att1_u,att1_d,att1_d,
					att1_u,att1_u,att1_d,att1_d,
					att1_u,att1_u,att1_d,att1_d,

					att2_u,att2_u,att2_d,att2_d,
					att2_u,att2_u,att2_d,att2_d,
					att2_u,att2_u,att2_d,att2_d,
					att2_u,att2_u,att2_d,att2_d 
};

const float MPV_MC[32] = {	
				0.9762,0.9805,0.9762,0.9805,
				0.7971,0.7954,0.7971,0.7954,
				0.9830,0.9833,0.9830,0.9833,
				0.7950,0.7904,0.7950,0.7904,

				0.9900,0.9946,0.9900,0.9946,
				0.7966,0.7892,0.7966,0.7892,
				0.9797,0.9894,0.9797,0.9894,
				0.7946,0.7964,0.7946,0.7964 
};

const float MPV_MC_Err[32] = {
				0.0020,0.0019,0.0020,0.0019,
				0.0023,0.0023,0.0023,0.0023,
				0.002,0.0020, 0.002,0.0020,
				0.0020,0.0021,0.0020,0.0021,

				0.00, 0.0020, 0.00, 0.0020,
				0.0021,0.0020,0.0021,0.0020,
				0.0020,0.0021,0.0020,0.0021,
				0.0020,0.0021,0.0020,0.0021 
}; 

const float CosmicPathLength_TrackID[12] = { 
				0.796, 0.796, 0.981, 0.981, 0.981, 0.981, 
				0.794, 0.794, 0.988, 0.988, 0.988, 0.988 
};

#if 0
// Imported from previous analysis
const float CosmicPathLength[32] = {
	
	0.981, 0.981, 0.981, 0.981,
	0.796, 0.796, 0.796, 0.796,
	0.981, 0.981, 0.981, 0.981,
	0.796, 0.796, 0.796, 0.796,
	
	0.988, 0.988, 0.988, 0.988,
	0.794, 0.794, 0.794, 0.794,
	0.988, 0.988, 0.988, 0.988,
	0.794, 0.794, 0.794, 0.794
};
#endif
// Extracted by fitting Run91-Period16 MC data with new module energy cal method
// ! Assigned to each channel

// Helper function to return CosmicPathLength MPV value and error
float returnVal(float prev, float updated, bool ifTrigCtr) {
	if (ifTrigCtr) {
		return updated;
	} else {
		return prev;
	}
}

const float CosmicPathLength[32] = {
	0.986, 0.982, 0.986, 0.982,
	returnVal(0.792, 0.838, thisFTrigCtr), returnVal(0.802, 0.834, thisFTrigCtr), returnVal(0.792, 0.838, thisFTrigCtr), returnVal(0.802, 0.834, thisFTrigCtr),
	0.984, 0.980, 0.984, 0.980,
	returnVal(0.797, 0.834, thisFTrigCtr), returnVal(0.804, 0.837, thisFTrigCtr), returnVal(0.797, 0.834, thisFTrigCtr), returnVal(0.804, 0.837, thisFTrigCtr),

	0.991, 1.009, 0.991, 1.009,
	returnVal(0.813, 0.832, thisFTrigCtr), returnVal(0.811, 0.831, thisFTrigCtr), returnVal(0.813, 0.832, thisFTrigCtr), returnVal(0.811, 0.831, thisFTrigCtr),
	0.988, 0.990, 0.988, 0.990,
	returnVal(0.801, 0.833, thisFTrigCtr), returnVal(0.797, 0.830, thisFTrigCtr), returnVal(0.801, 0.833, thisFTrigCtr), returnVal(0.791, 0.830, thisFTrigCtr),
};

const float CosmicPathLength_Err[32] = {
	
	0.004, 0.004, 0.004, 0.004,
	returnVal(0.005, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.005, 0.002, thisFTrigCtr), returnVal(0.006,0.002, thisFTrigCtr),
	0.004, 0.004, 0.004, 0.004,
	returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr),

	0.004, 0.004, 0.004, 0.004,
	returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr),
	0.004, 0.004, 0.004, 0.004,
	returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.005, 0.002, thisFTrigCtr), returnVal(0.006, 0.002, thisFTrigCtr), returnVal(0.005, 0.002, thisFTrigCtr)
};
#if 0
const float CosmicPathLength[32] = {
	0.986, 0.982, 0.986, 0.982,
	0.792, 0.802, 0.792, 0.802,
	0.984, 0.980, 0.984, 0.980,
	0.797, 0.804, 0.797, 0.804,

	0.991, 1.009, 0.991, 1.009,
	0.813, 0.811, 0.813, 0.811,
	0.988, 0.990, 0.988, 0.990,
	0.801, 0.797, 0.801, 0.791
};

const float CosmicPathLength_Err[32] = {
	
	0.004, 0.004, 0.004, 0.004,
	0.005, 0.006, 0.005, 0.006,
	0.004, 0.004, 0.004, 0.004,
	0.006, 0.006, 0.006, 0.006,

	0.004, 0.004, 0.004, 0.004,
	0.006, 0.006, 0.006, 0.006,
	0.004, 0.004, 0.004, 0.004,
	0.006 ,0.005, 0.006, 0.005
};
#endif

#endif  
