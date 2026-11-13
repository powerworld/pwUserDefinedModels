// Model constants for IEEET1: parameter, state and algebraic counts and indices.
export module pwUDM.IEEET1;

import std;

export namespace pwUDM::IEEET1 {

    // This implements the existing IEEET1 model as an example
    // ----------------------------------------------------------------------------------------------
    // For the Model
    inline constexpr int N_F_PARAMS = 15; // number of float model params
    inline constexpr int N_I_PARAMS = 0; // number of int model params
    inline constexpr int N_S_PARAMS = 0; // number of string model params
    inline constexpr int N_STATES = 4; // number of model states
    inline constexpr int N_ALGEBRAICS = 0; // number of model algebraics

    // For the NonWindUpLimits
    inline constexpr int N_NONWINDUPLIMITS  = 1; // number of nonwindup
    // ----------------------------------------------------------------------------------------------

    // These are just to make the code easier to read
    // Parameter Value indices
    inline constexpr int PARAM_Tr = 0;
    inline constexpr int PARAM_Ka = 1;
    inline constexpr int PARAM_Ta = 2;
    inline constexpr int PARAM_Vrmax = 3;
    inline constexpr int PARAM_Vrmin = 4;
    inline constexpr int PARAM_Ke = 5;
    inline constexpr int PARAM_Te = 6;
    inline constexpr int PARAM_Kf = 7;
    inline constexpr int PARAM_Tf = 8;
    inline constexpr int PARAM_Switch = 9;
    inline constexpr int PARAM_E1 = 10;
    inline constexpr int PARAM_SE1 = 11;
    inline constexpr int PARAM_E2 = 12;
    inline constexpr int PARAM_SE2 = 13;
    inline constexpr int PARAM_Spdmlt = 14;

    // State Value indices
    inline constexpr int STATE_Efield = 0;
    inline constexpr int STATE_SensedVt = 1;
    inline constexpr int STATE_Vr = 2;
    inline constexpr int STATE_Vf = 3;

    inline constexpr int NONWINDUPINDEX_VR = 0; // index for list of non-windup limited states

}
