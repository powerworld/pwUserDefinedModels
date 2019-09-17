#include <limits>

// For the Model
const	int	N_F_PARAMS				= 8;	// number of float model params
const	int N_I_PARAMS				= 0;	// number of int model params
const	int N_S_PARAMS				= 0;	// number of string model params
const	int N_STATES				= 2;	// number of model states
const	int N_ALGEBRAICS			= 0;	// number of model algebraics

// For the NonWindUpLimits
const	int N_NONWINDUPLIMITS		= 1;	// number of nonwindup

// These are just to make the code easier to read
// Parameter Value indices.
const	int	PARAM_R     = 0;
const	int	PARAM_T1    = 1;
const	int	PARAM_Vmax  = 2;
const	int	PARAM_Vmin  = 3;
const	int	PARAM_T2    = 4;
const	int	PARAM_T3    = 5;
const	int	PARAM_Dt    = 6;
const	int	PARAM_Trate = 7;

// State Value Indices.
const	int	STATE_TurbPower = 0;
const	int	STATE_ValvePos  = 1;

const	int NONWINDUPINDEX_ValvePos = 0; // index for list of non-windup limited states

// Algebraic Value Indices
// None ALG_TSXXX             = 0;

// These functions WILL be in the Export Directory
PW_UDM_DllExport(int) DLLVersion();
PW_UDM_DllExport(int) modelClassName(int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(void) allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds);
PW_UDM_DllExport(int) parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(int) stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy);
PW_UDM_DllExport(void) getDefaultParameterValue(TTxMyModelData* ParamsAndStates);
PW_UDM_DllExport(int) SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds);
PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX);
PW_UDM_DllExport(void) PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits);