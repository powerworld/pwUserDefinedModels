export module pwUDM.MTDCCONV;
// This implements a generic MTDC converter model as an example

export namespace pwUDM::MTDCCONV {

    // For the Model
    inline constexpr int N_F_PARAMS   = 9;  // number of float model params
    inline constexpr int N_I_PARAMS   = 0;  // number of integer model params
    inline constexpr int N_S_PARAMS   = 0;  // number of string model params
    inline constexpr int N_STATES     = 4;  // number of model states
    inline constexpr int N_ALGEBRAICS = 0;  // number of model algebraics

    // For the NonWindUpLimits
    inline constexpr int N_NONWINDUPLIMITS = 1;  // number of nonwindup limits

    //---------------------------------------------------------------------------
    // These are just to make the code easier to read
    // Parameter Value indices.
    inline constexpr int PARAM_Tf_Idc    = 0;
    inline constexpr int PARAM_Tf_Vdc    = 1;
    inline constexpr int PARAM_Tf_Vac    = 2;
    inline constexpr int PARAM_Ki        = 3;
    inline constexpr int PARAM_Id_base   = 4;
    inline constexpr int PARAM_CosMin    = 5;
    inline constexpr int PARAM_CosMax    = 6;
    inline constexpr int PARAM_IdRefLim  = 7;
    inline constexpr int PARAM_Id_margin = 8;

    // State Value Indices.
    inline constexpr int STATE_IdcSense = 0;
    inline constexpr int STATE_VdcSense = 1;
    inline constexpr int STATE_VacSense = 2;
    inline constexpr int STATE_CosAngle = 3;

    inline constexpr int NONWINDUPINDEX_CosAngle = 0;  // index for list of non-windup limited states

    // Algebraic Value Indices
    // None ALG_TSXXX             = 0;

}
