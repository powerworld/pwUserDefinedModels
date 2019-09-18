#ifndef PW_UDM_IEEEST
#define PW_UDM_IEEEST

// This implements the existing IEEET1 model as an example
// ----------------------------------------------------------------------------------------------
// For the Model
const	int	N_F_PARAMS				= 18;	// number of float model params
const	int N_I_PARAMS				= 1;	// number of int model params
const	int N_S_PARAMS				= 0;	// number of string model params
const	int N_STATES				= 7;	// number of model states
const	int N_ALGEBRAICS			= 0;	// number of model algebraics

// For the NonWindUpLimits
const	int N_NONWINDUPLIMITS		= 0;	// number of nonwindup

// ---------------------------------------------------------------------------
// These are just to make the code easier to read
// Parameter Value indices.

// Integers
const	int PARAM_Ics    =  0;

// Doubles
const	int PARAM_A1     =  0;
const	int PARAM_A2     =  1;
const	int PARAM_A3     =  2;
const	int PARAM_A4     =  3;
const	int PARAM_A5     =  4;
const	int PARAM_A6     =  5;
const	int PARAM_T1     =  6;
const	int PARAM_T2     =  7;
const	int PARAM_T3     =  8;
const	int PARAM_T4     =  9;
const	int PARAM_T5     = 10;
const	int PARAM_T6     = 11;
const	int PARAM_Ks     = 12;
const	int PARAM_Lsmax  = 13;
const	int PARAM_Lsmin  = 14;
const	int PARAM_Vcu    = 15;
const	int PARAM_Vcl    = 16;
const	int PARAM_Tdelay = 17;

// State Value Indices.
const	int STATE_Filter1         = 0;
const	int STATE_Filter2         = 1;
const	int STATE_Filter3         = 2;
const	int STATE_FilterOut       = 3;
const	int STATE_LL1             = 4;
const	int STATE_LL2             = 5;
const	int STATE_UnlimitedSignal = 6;

// Algebraic Value Indices
// None ALG_TSXXX             = 0;

#endif //PW_UDM_IEEEST