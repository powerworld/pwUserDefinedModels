// Model constants for CLOD: parameter, state and algebraic counts and indices.
export module pwUDM.CLOD;

import std;

export namespace pwUDM::CLOD {

    // This implements the existing CLOD model as an example
    // ----------------------------------------------------------------------------------------------
    // For the Model
    inline constexpr int N_F_PARAMS = 9;     // number of float model params
    inline constexpr int N_I_PARAMS = 0;     // number of int model params
    inline constexpr int N_S_PARAMS = 0;     // number of string model params
    inline constexpr int N_STATES = 6;     // number of model states
    inline constexpr int N_ALGEBRAICS = 30;     // number of model algebraics

    // For the NonWindUpLimits
    inline constexpr int N_NONWINDUPLIMITS		= 0;	// number of nonwindup

    // ---------------------------------------------------------------------------
      // These are just to make the code easier to read
      // Parameter Value indices.

    // Integers
    inline constexpr int PARAM_Ics = 0;

      // Double Parameters
    inline constexpr int PARAM_PercLmotor = 0;
    inline constexpr int PARAM_PercSmotor = 1;
    inline constexpr int PARAM_PercTex = 2;
    inline constexpr int PARAM_PercDis = 3;
    inline constexpr int PARAM_PercP = 4;
    inline constexpr int PARAM_Kp = 5;
    inline constexpr int PARAM_Vi = 6;
    inline constexpr int PARAM_Ti = 7;
    inline constexpr int PARAM_Tb = 8;

    // State Value Indices.
    inline constexpr int STATE_IM1SpeedWr = 0;
    inline constexpr int STATE_IM1Epr = 1;
    inline constexpr int STATE_IM1Epi = 2;
    inline constexpr int STATE_IM2SpeedWr = 3;
    inline constexpr int STATE_IM2Epr = 4;
    inline constexpr int STATE_IM2Epi = 5;

    // Algebraic Value Indices
    // Large Motor Algebraics
    inline constexpr int ALG_IML_Rs = 0;
    inline constexpr int ALG_IML_Xs = 1;
    inline constexpr int ALG_IML_fWo = 2;
    inline constexpr int ALG_IMLfInvTp = 3;
    inline constexpr int ALG_IML_Xp = 4;
    inline constexpr int ALG_IML_X = 5;
    inline constexpr int ALG_IML_SlipInitial = 6;
    inline constexpr int ALG_IML_fPInitial = 7;
    inline constexpr int ALG_IML_fQInitial = 8;
    inline constexpr int ALG_IML_Tnom = 9;
    // Small Motor Algebraics
    inline constexpr int ALG_IMS_Rs = 10;
    inline constexpr int ALG_IMS_Xs = 11;
    inline constexpr int ALG_IMS_fWo = 12;
    inline constexpr int ALG_IMSfInvTp = 13;
    inline constexpr int ALG_IMS_Xp = 14;
    inline constexpr int ALG_IMS_X = 15;
    inline constexpr int ALG_IMS_SlipInitial = 16;
    inline constexpr int ALG_IMS_fPInitial = 17;
    inline constexpr int ALG_IMS_fQInitial = 18;
    inline constexpr int ALG_IMS_Tnom = 19;
    // Discharge Lighting algebraics
    inline constexpr int ALG_DL_P_at_1_PU = 20;
    inline constexpr int ALG_DL_Q_at_1_PU = 21;
    // ZIP Model algebraics
    inline constexpr int ALG_ZIP_ZMW = 22;
    inline constexpr int ALG_ZIP_IMW = 23;
    inline constexpr int ALG_ZIP_SMW = 24;
    inline constexpr int ALG_ZIP_ZMVR = 25;
    inline constexpr int ALG_ZIP_IMVR = 26;
    inline constexpr int ALG_ZIP_SMVR = 27;
    // Timer for Load Tripping
    inline constexpr int ALG_TIMER_LowVoltTimeSet = 28;
    inline constexpr int ALG_TIMER_LowVoltTime = 29;

    // These are hard-coded parameters for modeling the discharge lighting load
    inline constexpr double GLOBAL_DL_PCoeff = 1.0; // constant CURRENT Real Power
    inline constexpr double GLOBAL_DL_QCoeff = 4.5; // much higher coefficent for Reactive Power
    inline constexpr double GLOBAL_DL_VoltBP = 0.75;
    inline constexpr double GLOBAL_DL_VoltExt = 0.65;

}
