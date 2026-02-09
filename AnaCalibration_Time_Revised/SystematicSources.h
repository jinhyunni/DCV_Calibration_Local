// Cut values of IADP
// Very loosened cut : 0~20
#ifndef _SYSTEMATICSOURCES_
#define _SYSTEMATICSOURCES_

namespace SYSTEMATIC
{

// Reference set #
const int RefIADP=1;
const int RefFitR=1;
//const int RefSubAdc; // -> Not determined yet(2025.10.31)

// 1. IADP selection to main channel peaks
// Caution! If IADP cut is applied too tightely, than peak distribution could be far away from gaussian distribution...
struct IADPCuts {
  float min;
  float max;
};

std::vector<float> SystIadp = {
 0, // Dummy entry for MB(No IADP selection
 3,
 2,
 1 
};

// 2. Fit range tunning parameter
// Fit range to main channel time peak is determined in the following way :
//    a. Extract sigma of data within peak +- 1 interval
//    b. Define gaussian fit range to peak +- sig*fitRangeTunePar
//std::vector<float> SystFitRangeTunePar = {5.0, 4.0, 3.0, 2.0}; // -> 2.0 was deleted since it gives strange values(2025.10.04)
std::vector<float> SystFitRangeTunePar = {5.0, 4.0, 3.0};


// 3. Sub ch's ADC cut
//  This cut is used when checking time difference between adjoint channels.
//  THis cut is introduced because requiring selection on main ch's pulse doesn't assure good peak of sub channel.
//  Thus, delta time between main channel and sub channel can be biased toward negative or positive value, depending on bkg signals of sub channel

std::vector<float> SystSubChAdcCut = {
 0, // Dummy entry for MB
 50,
 75,
 100,
 //150 -> Too high, leaving scarce stats 
};

#if 0
std::vector<float> SystModEneCut = {
 0.0, // No cut
 0.2,
 0.4,
 0.6,
 0.8,
 1.0,
 1.2,
 1.4,
 1.6,
 1.8,
 2.0,
 2.2,
 2.4,
 2.6,
 2.8,
 3.0,
};
#endif
std::vector<float> SystModEneCut = {
 0.0, // No cut
 1.5,
 2.0, // -> Specially added
 2.5, // -> Specially added
 3.0,
 3.5, // -> Specially added
 5.0,
};


}
#endif
