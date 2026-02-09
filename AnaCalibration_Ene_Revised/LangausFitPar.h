#ifndef __LANGAUSFITPAR__
#define __LANGAUSFITPAR__

double parLimitLow[3][4] = {
  {4, 50.0, 1e0, 10.0},
  {0.05, 0.5, 1e0, 0.0},
  {0.05, 0.5, 1e0, 0.0}
};

double parLimitHigh[3][4] = {
  {70.0, 500.0, 1e6, 100},
  {1.0, 2.0, 1e5, 1e0},
  {1.0, 4.0, 1e5, 1e0}
};
#endif
