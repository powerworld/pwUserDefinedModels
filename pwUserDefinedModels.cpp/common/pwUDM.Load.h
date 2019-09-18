#ifndef PW_UDM_LOAD
#define PW_UDM_LOAD

#include "pwUDM.h"

// Exciter Hard-Coded Input Signal Indices
// Note: These are the SAME for all laods and are hard-coded values that Simulator ALWAYS passes to the DLL.  
// If additional inputs are needed from Simulator, then you must define them in "Algebraics"
// const int HARDCODE_LOAD_xxx = 0;
// const int HARDCODE_LOAD_xxxxx = 1;
// const int HARDCODE_LOAD_xx = 2;

// These functions need to be exported in the DLL
PW_UDM_DllExport(double) LoadNortonAdmittance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theG, double* theB);
PW_UDM_DllExport(double) LoadNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag);
PW_UDM_DllExport(double) LoadNortonCurrentAlgebraicDerivative(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* dIr_de, double* dIr_df, double* dIi_de, double* dIi_df);
PW_UDM_DllExport(bool) LoadInitializeAlgebraic(TTxMyModelData * ParamsAndStates, TTxSystemOptions * SystemOptions, double* thePUTol, double* P, double* Q, double* V, double* UsedP, double* UsedQ);

#endif // PW_UDM_LOAD


