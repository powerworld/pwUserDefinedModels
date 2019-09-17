#ifndef PW_UDM_TGOV1
#define PW_UDM_TGOV1

// This implements the existing TGOV1 model as an example
// ----------------------------------------------------------------------------------------------
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

#endif //PW_UDM_TGOV1