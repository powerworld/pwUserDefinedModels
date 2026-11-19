export module pwUDM.MTDCLINE;
// This implements a generic multi-terminal DC model as an example

export namespace pwUDM::MTDCLINE {

    // This example handles up to MAX_TERMINALS converters, one state and one algebraic for each.
    // Simulator does not tell the DLL how many converters the line has, so the integer parameter
    // NumTerminals must be set to that number (from 1 to MAX_TERMINALS).
    inline constexpr int MAX_TERMINALS = 8;

    // For the Model
    inline constexpr int N_F_PARAMS   = 2;               // number of float model params
    inline constexpr int N_I_PARAMS   = 1;               // number of integer model params
    inline constexpr int N_S_PARAMS   = 0;               // number of string model params
    inline constexpr int N_STATES     = MAX_TERMINALS;   // number of model states
    inline constexpr int N_ALGEBRAICS = MAX_TERMINALS;   // number of model algebraics

    // For the NonWindUpLimits
    inline constexpr int N_NONWINDUPLIMITS = 0;  // number of nonwindup limits

    //---------------------------------------------------------------------------
    // These are just to make the code easier to read
    // Parameter Value indices.
    // Note: Simulator numbers the integer parameters first, then the float parameters, so the
    //       parameter number that parameterName receives for a float parameter is N_I_PARAMS + PARAM_xxx
    inline constexpr int PARAM_Tord      = 0;
    inline constexpr int PARAM_IordScale = 1;

    // Integer Parameter Value indices.
    inline constexpr int IPARAM_NumTerminals = 0;

    // State Value Indices.
    // The current order of converter number i (counting from 0) is state STATE_IdRef + i
    inline constexpr int STATE_IdRef = 0;

    // Algebraic Value Indices
    // The initial current order of converter number i (counting from 0) is algebraic ALG_IdRefInit + i
    inline constexpr int ALG_IdRefInit = 0;

}
