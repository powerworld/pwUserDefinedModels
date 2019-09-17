#ifndef PW_UDM_GENCLS
#define PW_UDM_GENCLS

// This implements the existing TGOV1 model as an example
// ----------------------------------------------------------------------------------------------
// For the Model
const	int	N_F_PARAMS				= 6;	// number of float model params
const	int N_I_PARAMS				= 0;	// number of int model params
const	int N_S_PARAMS				= 0;	// number of string model params
const	int N_STATES				= 2;	// number of model states
const	int N_ALGEBRAICS			= 0;	// number of model algebraics

// For the NonWindUpLimits
const	int N_NONWINDUPLIMITS		= 0;	// number of nonwindup

// ---------------------------------------------------------------------------
// These are just to make the code easier to read
// Parameter Value indices.
const	int PARAM_H     = 0;
const	int PARAM_D     = 1;
const	int PARAM_Ra    = 2;
const	int PARAM_Xdp   = 3;
const	int PARAM_Rcomp = 4;
const	int PARAM_Xcomp = 5;

// State Value Indices.
const	int STATE_Angle = 0;
const	int STATE_Speed = 1;

// Algebraic Value Indices
// None ALG_TSXXX             = 0;

#endif //PW_UDM_GENCLS