export module pwUDM.GENCLS;
// This implements the existing GENCLS model as an example

export namespace pwUDM::GENCLS {

    // For the Model
    inline constexpr int N_F_PARAMS   = 6;  // number of float model params
    inline constexpr int N_I_PARAMS   = 0;  // number of integer model params
    inline constexpr int N_S_PARAMS   = 0;  // number of string model params
    inline constexpr int N_STATES     = 2;  // number of model states
    inline constexpr int N_ALGEBRAICS = 0;  // number of model algebraics

    // For the NonWindUpLimits
    inline constexpr int N_NONWINDUPLIMITS = 0;  // number of nonwindup

    //---------------------------------------------------------------------------
    // These are just to make the code easier to read
    // Parameter Value indices.
    inline constexpr int PARAM_H     = 0;
    inline constexpr int PARAM_D     = 1;
    inline constexpr int PARAM_Ra    = 2;
    inline constexpr int PARAM_Xdp   = 3;
    inline constexpr int PARAM_Rcomp = 4;
    inline constexpr int PARAM_Xcomp = 5;

    // State Value Indices.
    inline constexpr int STATE_Angle = 0;
    inline constexpr int STATE_Speed = 1;

    // Algebraic Value Indices
    // None ALG_TSXXX             = 0;

}
