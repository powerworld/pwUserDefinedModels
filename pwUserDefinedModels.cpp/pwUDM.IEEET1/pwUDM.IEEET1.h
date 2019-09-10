#ifndef PW_UDM_IEEET1
#define PW_UDM_IEEET1

// This implements the existing IEEET1 model as an example
// ----------------------------------------------------------------------------------------------
// For the Model
const	int	N_F_PARAMS		= 15;	// number of float model params
const	int N_I_PARAMS		= 0;	// number of int model params
const	int N_S_PARAMS		= 0;	// number of string model params
const	int N_STATES		= 4;	// number of model states
const	int N_ALGEBRAICS	= 0;	// number of model algebraics

// For the NonWindUpLimits
const	int N_NONWINDUPLIMITS		= 1;	// number of nonwindup
// ----------------------------------------------------------------------------------------------

// These are just to make the code easier to read
// Parameter Value indices
const int PARAM_Tr						= 0;
const int PARAM_Ka						= 1;
const int PARAM_Ta						= 2;
const int PARAM_Vrmax					= 3;
const int PARAM_Vrmin					= 4;
const int PARAM_Ke						= 5;
const int PARAM_Te						= 6;
const int PARAM_Kf						= 7;
const int PARAM_Tf						= 8;
const int PARAM_Switch					= 9;
const int PARAM_E1						= 10;
const int PARAM_SE1						= 11;
const int PARAM_E2						= 12;
const int PARAM_SE2						= 13;
const int PARAM_Spdmlt					= 14;

// State Value indices
const int STATE_Efield					= 0;
const int STATE_SensedVt				= 1;
const int STATE_Vr						= 2;
const int STATE_Vf						= 3;

const int NONWINDUPINDEX_VR = 0; // index for list of non-windup limited states

//  These functions are internal to the DLL
void	LOCAL_SaturationCalculateValues(double tempE1, double tempS1,double tempE2, double tempS2, double& fSatA, double& fSatB);
void	LOCAL_AutoSetVrminVrmax(double local_E2, double local_Ke, double fSatA, double fSatB, double& local_Vrmin, double& local_Vrmax);
double	LOCAL_AutoSetKeNew(double local_VrMax, double local_VrMin, double local_Efd, double fSatA, double fSatB);
void	LOCAL_PWSolveQuadraticEq(double a, double b, double c, double& x1, double& x2, bool& imag);
double	LOCAL_saturationValue(double fSatA, double fSatB, double input);

#endif // PW_UDM_IEEET1