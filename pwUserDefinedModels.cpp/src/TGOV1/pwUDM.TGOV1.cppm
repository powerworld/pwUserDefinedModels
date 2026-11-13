// Model constants for TGOV1: parameter, state and algebraic counts and indices.
export module pwUDM.TGOV1;

import std;

export namespace pwUDM::TGOV1 {

    // This implements the existing TGOV1 model as an example
    // ----------------------------------------------------------------------------------------------
    // For the Model
    inline constexpr	int	N_F_PARAMS				= 8;	// number of float model params
    inline constexpr	int N_I_PARAMS				= 0;	// number of int model params
    inline constexpr	int N_S_PARAMS				= 0;	// number of string model params
    inline constexpr	int N_STATES				= 2;	// number of model states
    inline constexpr	int N_ALGEBRAICS			= 0;	// number of model algebraics

    // For the NonWindUpLimits
    inline constexpr	int N_NONWINDUPLIMITS		= 1;	// number of nonwindup

    // These are just to make the code easier to read
    // Parameter Value indices.
    inline constexpr	int	PARAM_R     = 0;
    inline constexpr	int	PARAM_T1    = 1;
    inline constexpr	int	PARAM_Vmax  = 2;
    inline constexpr	int	PARAM_Vmin  = 3;
    inline constexpr	int	PARAM_T2    = 4;
    inline constexpr	int	PARAM_T3    = 5;
    inline constexpr	int	PARAM_Dt    = 6;
    inline constexpr	int	PARAM_Trate = 7;

    // State Value Indices.
    inline constexpr	int	STATE_TurbPower = 0;
    inline constexpr	int	STATE_ValvePos  = 1;

    inline constexpr	int NONWINDUPINDEX_ValvePos = 0; // index for list of non-windup limited states

    // Algebraic Value Indices
    // None ALG_TSXXX             = 0;

}
