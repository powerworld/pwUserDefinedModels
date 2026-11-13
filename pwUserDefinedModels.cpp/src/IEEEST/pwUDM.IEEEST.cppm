// Model constants for IEEEST: parameter, state and algebraic counts and indices.
export module pwUDM.IEEEST;

import std;

export namespace pwUDM::IEEEST {

    // This implements the existing IEEET1 model as an example
    // ----------------------------------------------------------------------------------------------
    // For the Model
    inline constexpr	int	N_F_PARAMS				= 18;	// number of float model params
    inline constexpr	int N_I_PARAMS				= 1;	// number of int model params
    inline constexpr	int N_S_PARAMS				= 0;	// number of string model params
    inline constexpr	int N_STATES				= 7;	// number of model states
    inline constexpr	int N_ALGEBRAICS			= 0;	// number of model algebraics

    // For the NonWindUpLimits
    inline constexpr	int N_NONWINDUPLIMITS		= 0;	// number of nonwindup

    // ---------------------------------------------------------------------------
    // These are just to make the code easier to read
    // Parameter Value indices.

    // Integers
    inline constexpr	int PARAM_Ics    =  0;

    // Doubles
    inline constexpr	int PARAM_A1     =  0;
    inline constexpr	int PARAM_A2     =  1;
    inline constexpr	int PARAM_A3     =  2;
    inline constexpr	int PARAM_A4     =  3;
    inline constexpr	int PARAM_A5     =  4;
    inline constexpr	int PARAM_A6     =  5;
    inline constexpr	int PARAM_T1     =  6;
    inline constexpr	int PARAM_T2     =  7;
    inline constexpr	int PARAM_T3     =  8;
    inline constexpr	int PARAM_T4     =  9;
    inline constexpr	int PARAM_T5     = 10;
    inline constexpr	int PARAM_T6     = 11;
    inline constexpr	int PARAM_Ks     = 12;
    inline constexpr	int PARAM_Lsmax  = 13;
    inline constexpr	int PARAM_Lsmin  = 14;
    inline constexpr	int PARAM_Vcu    = 15;
    inline constexpr	int PARAM_Vcl    = 16;
    inline constexpr	int PARAM_Tdelay = 17;

    // State Value Indices.
    inline constexpr	int STATE_Filter1         = 0;
    inline constexpr	int STATE_Filter2         = 1;
    inline constexpr	int STATE_Filter3         = 2;
    inline constexpr	int STATE_FilterOut       = 3;
    inline constexpr	int STATE_LL1             = 4;
    inline constexpr	int STATE_LL2             = 5;
    inline constexpr	int STATE_UnlimitedSignal = 6;

    // Algebraic Value Indices
    // None ALG_TSXXX             = 0;

}
