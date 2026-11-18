module pwUDM.IEEET1;

import std;
import pwUDM.Core;
import pwUDM.Exciter;

namespace {

double LOCAL_saturationValue(double fSatA, double fSatB, double input)
{
    if (input <= fSatA)
    {
        return(0);
    }
    else
    {
        return(fSatB*std::pow(input - fSatA, 2));
    }
}


// Special functions for handling saturation
void LOCAL_PWSolveQuadraticEq (double a, double b, double c, double& x1, double& x2, bool& Imag)
{
    double    t;

    t    = std::pow(b, 2) - 4*a*c;
    if (t > 0)
    {
        Imag = 0;
        t = std::sqrt(t);
        x1 = (-b+t)/(2*a);
        x2 = (-b-t)/(2*a);
    }
    else
    {
        Imag = 1;
        x1 = -b/(2*a);
        x2 = std::sqrt(-t)/(2*a);
    }
}

void LOCAL_SaturationCalculateValues(double tempE1, double tempS1, double tempE2, double tempS2, double& fSatA, double& fSatB)
{
    double tempA, tempB, tempC, result1, result2;  // quadratic equation coefficients and results
    bool isImag;  // True if quadratic equation results are imaginary

    if (tempS1 <= 0)  // Should only occur for zero
    {
        fSatA = tempE1;
        if (tempE2 == tempE1)  // usual situation is all zeros
        {
            fSatB = 0;
        }
        else
        {
            fSatB = tempS2/std::pow(tempE2 - fSatA, 2);
        }
    }
    else
    {
        if (tempS2 == tempS1)
        {
            tempS2 = tempS1*1.001;
        }
        tempA = tempS2 - tempS1;
        tempB = 2*(-tempS2*tempE1 + tempS1*tempE2);
        tempC = tempS2*std::pow(tempE1, 2) - tempS1*std::pow(tempE2, 2);
        LOCAL_PWSolveQuadraticEq(tempA, tempB, tempC, result1, result2, isImag);
        fSatA = result2;
        fSatB = (tempS1)/std::pow(tempE1-fSatA, 2);
    }
}

void LOCAL_AutoSetVrminVrmax(double local_E2, double local_Ke, double fSatA, double fSatB, double& local_Vrmin, double& local_Vrmax)  // TJO 5/28/08 // kate 1/12/12 also pass in fSatA and fSatB
{
    if (local_Ke <= 0)
    {
        if (local_E2 <= fSatA)
        {
            local_Vrmax = 0; // no saturation
        }
        else
        {
            local_Vrmax = fSatB*local_E2*std::pow(local_E2-fSatA, 2);
        }
    }
    else 
    {
        local_Vrmax = (LOCAL_saturationValue(fSatA, fSatB, local_E2) + local_Ke)*local_E2;
    }
    local_Vrmin = -local_Vrmax;
}

double LOCAL_AutoSetKeNew(double local_VrMax, double local_VrMin, double local_Efd, double fSatA, double fSatB)
{
    if (local_VrMin <= 0)
    {
        return(-LOCAL_saturationValue(fSatA, fSatB, local_Efd));
    }
    else
    {
        return(local_VrMin - LOCAL_saturationValue(fSatA, fSatB, local_Efd));
    }
}

}

namespace pwUDM::IEEET1 {

//******************************************************************************
extern "C" __declspec(dllexport) int DLLVersion()
{
    return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int modelClassName(int* StrSize, char16_t* StrBuf, int dummy)
{
    int result = pwProvideString(StrBuf, StrSize, pwModelClassString(ModelClass::Exciter));
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) void allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds)
{
    // -------The model-specific parameters-----
    numbersOfEverything->nFloatParams = N_F_PARAMS;
    numbersOfEverything->nIntParams = N_I_PARAMS;
    numbersOfEverything->nStrParams = N_S_PARAMS;
    numbersOfEverything->nStates = N_STATES;
    numbersOfEverything->nAlgebraics = N_ALGEBRAICS;
    // ----For nonwindup limits of the model---
    numbersOfEverything->nNonWindUpLimits = N_NONWINDUPLIMITS;
}

//******************************************************************************
extern "C" __declspec(dllexport) int parameterName(int* ParamNum, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*ParamNum)
    {
        case PARAM_Tr:     ts = u"Tr"; break;
        case PARAM_Ka:     ts = u"Ka"; break;
        case PARAM_Ta:     ts = u"Ta"; break;
        case PARAM_Vrmax:  ts = u"Vrmax"; break;
        case PARAM_Vrmin:  ts = u"Vrmin"; break;
        case PARAM_Ke:     ts = u"Ke"; break;
        case PARAM_Te:     ts = u"Te"; break;
        case PARAM_Kf:     ts = u"Kf"; break;
        case PARAM_Tf:     ts = u"Tf"; break;
        case PARAM_Switch: ts = u"Switch"; break;
        case PARAM_E1:     ts = u"E1"; break;
        case PARAM_SE1:    ts = u"SE(E1)"; break;
        case PARAM_E2:     ts = u"E2"; break;
        case PARAM_SE2:    ts = u"SE(E2)"; break;
        case PARAM_Spdmlt: ts = u"Spdmlt"; break;
        default:           ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) int stateName(int* StateNum, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*StateNum)
    {
        case STATE_EField:   ts = u"EField"; break;
        case STATE_SensedVt: ts = u"Sensed Vt"; break;
        case STATE_VR:       ts = u"VR"; break;
        case STATE_VF:       ts = u"VF"; break;
        default:             ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}

//******************************************************************************
/* none for this object
extern "C" __declspec(dllexport) int OtherObjectClass(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*Num)
    {
        case 0:   ts = u"Bus"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
/* none for this object
extern "C" __declspec(dllexport) int OtherObjectDescription(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*Num)
    {
        case 0:   ts = u"Signal Bus"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) void getDefaultParameterValue(TxMyModelData* ParamsAndStates)
{
    DoubleArray* pDoubArr;

    // Example TGOV1 parameters
    pDoubArr = ParamsAndStates->FloatParams;
    (*pDoubArr)[PARAM_Tr] = 0.0;
    (*pDoubArr)[PARAM_Ka] = 50.0;
    (*pDoubArr)[PARAM_Ta] = 0.04;
    (*pDoubArr)[PARAM_Vrmax] = 1.0;
    (*pDoubArr)[PARAM_Vrmin] = -1.0;
    (*pDoubArr)[PARAM_Ke] = -0.06;
    (*pDoubArr)[PARAM_Te] = 0.6;
    (*pDoubArr)[PARAM_Kf] = 0.09;
    (*pDoubArr)[PARAM_Tf] = 1.46;
    (*pDoubArr)[PARAM_Switch] = 0.0;
    (*pDoubArr)[PARAM_E1] = 2.80;
    (*pDoubArr)[PARAM_SE1] = 0.04;
    (*pDoubArr)[PARAM_E2] = 3.73;
    (*pDoubArr)[PARAM_SE2] = 0.33;
    (*pDoubArr)[PARAM_Spdmlt] = 0.0;
}

/* non specified for this model
extern "C" __declspec(dllexport) int getStringParamDefaultValue(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    switch (*Num)
    {
        case 0:   ts = u"DefaultString0"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int signalSelection(int* Num, int* StrSize, char16_t* StrBuf, int dummy)
{
    const char16_t* ts;

    ts = u""; // there are none!
    switch (*Num)
    {
        case ALG_XXX: ts = u"TSXXX"; break;
        default:  ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) int SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates, double* TimeStepSeconds)
{
    // Return an integer value between 0 and 7
    // This function returns the EXPONENT of 2 for the number subintervals.
    // Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
    // The maximum number of subintervals allowed is 128, so (2^7) = 128
    return(0);
}

//******************************************************************************
extern "C" __declspec(dllexport) void initializeYourself(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{

    // Right now this example is IEEET1 exciter
    // Parameters
    double fTr, fKa, fTa, fVrmax, fVrmin, fKe, fKf, fTf, fE1, fSEE1, fE2, fSEE2;
    double fSatA, fSatB;
    double local_Vrmax;
    double fVRef; 
    double local_EField;  // Intial field value from generator
    double local_VR;  // VR initial value
    double local_VTerminalSensed;  // Sensed terminal voltage
    double local_VF;
    double finvTf;

    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    BooleanArray* pIgnoreStatesArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    //--------------------------------------------------------------------
    // Grab States, Params, Algebraics Needed for this method
    //--------------------------------------------------------------------
    fTr = (*pFloatParamArr)[PARAM_Tr];
    fKa = (*pFloatParamArr)[PARAM_Ka];
    fTa = (*pFloatParamArr)[PARAM_Ta];
    fVrmax = (*pFloatParamArr)[PARAM_Vrmax];
    fVrmin = (*pFloatParamArr)[PARAM_Vrmin];
    fKe = (*pFloatParamArr)[PARAM_Ke];
    fKf = (*pFloatParamArr)[PARAM_Kf];
    fTf = (*pFloatParamArr)[PARAM_Tf];
    fE1 = (*pFloatParamArr)[PARAM_E1];
    fSEE1 = (*pFloatParamArr)[PARAM_SE1];
    fE2 = (*pFloatParamArr)[PARAM_E2];
    fSEE2 = (*pFloatParamArr)[PARAM_SE2];
    if (fTf == 0) { fKf = 0; }

    local_EField = (*pHardCodedArr)[HARDCODE_EXCITER_InitFieldVoltage]; 
    local_VTerminalSensed  = (*pHardCodedArr)[HARDCODE_EXCITER_GenVcomp]; 

    // This function returns fSatA and fSatB constants for a quadratic model.
    LOCAL_SaturationCalculateValues(fE1, fSEE1, fE2, fSEE2, fSatA, fSatB); 

    if (fVrmax == 0)
    {
        LOCAL_AutoSetVrminVrmax(fE2, fKe, fSatA, fSatB, fVrmin, fVrmax);  
    }

    if (fKe == 0)
    {
        fKe = LOCAL_AutoSetKeNew(fVrmax, fVrmin, local_EField, fSatA, fSatB);
    }

    // local_VR     := local_EField*fKE + SaturationValueTimesInput(local_EField);
    // kate 1/12/12 this is effectively doing what SaturationValueTimesInput did.
    if (local_EField <= fSatA)  // no saturation
    {
        local_Vrmax    = 0;
    }
    else
    {
        local_Vrmax = fSatB*local_EField*std::pow(local_EField-fSatA, 2);
    }
    local_VR = local_EField*fKe + local_Vrmax;

    fVRef = local_VTerminalSensed + local_VR/fKa;

    // Ignored States
    if (fTr == 0) { (*pIgnoreStatesArr)[STATE_SensedVt] = 1;}
    if (fTa == 0) { (*pIgnoreStatesArr)[STATE_VR] = 1;}

    // The following parameters should actually get SET, not read in!
    (*pHardCodedArr)[HARDCODE_EXCITER_Vref] = fVRef;    

    //--------------------------------------------------------
    // Model of a Derivative "Washout" block
    // Ks                     /  -K/T        \
    // OUT = ------ * IN  ==>>  OUT = | ------ + K/T | * IN
    // [1+Ts]                   \ [1+Ts]       /
    // OUT = (StateInter + K/T) * IN
    //--------------------------------------------------------
    if (fTf == 0) { finvTf = 0; } else { finvTf = 1/fTf; }
    local_VF = -fKf*finvTf*local_EField;
    // local_VF := (fVRef - local_VTerminalSensed) - local_VR/fKa; // kate 3/15/13
    
    // // Write out initial state vector
    (*pStatesArr)[STATE_EField] = local_EField;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
    (*pStatesArr)[STATE_SensedVt] = local_VTerminalSensed;
    (*pStatesArr)[STATE_VR] = local_VR;
    (*pStatesArr)[STATE_VF] = local_VF;
}

//******************************************************************************
extern "C" __declspec(dllexport) void calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{
    // First there are the parameters,
    double fTr, fKa, fTa, fKe, fTe, fKf, fTf, fE1, fSEE1, fE2, fSEE2;
    double fInvTE, fInvTR, finvTA, fInvTF;

    // The state variables, state at the beginning of the time is passed in
    double stateEField, stateSensedVt, stateVR, stateVF;

    // The FofX or xdot,
    double dotEFieldBeforeWindupLimit, dotSensedVt, dotVr, dotVf;

    double fSatA, fSatB;

    double tempSatValTimesInput, vInputState;

    double GenObjectVTerminalMayBeCompensated_In, VRef_In, StateVsWith_OEL_UEL_In;

    double fPIOFeedbackoutput;
    unsigned char fActiveLimitHLStateVr;

    DoubleArray* pFloatParamArr;
    DoubleArray* pHardCodedArr;
    DoubleArray* pStatesArr;
    ByteArray* pByteActiveLimitArr;
    DoubleArray* pDotXArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pByteActiveLimitArr = nonWindUpLimits->activeLimits;
    pDotXArr = dotX;

    //--------------------------------------------------------------------
    // Grab States, Params, Algebraics Needed for this method
    //--------------------------------------------------------------------
    stateEField = (*pStatesArr)[STATE_EField];
    stateSensedVt = (*pStatesArr)[STATE_SensedVt];
    stateVR = (*pStatesArr)[STATE_VR];
    stateVF = (*pStatesArr)[STATE_VF];

    fTr = (*pFloatParamArr)[PARAM_Tr];
    fKa = (*pFloatParamArr)[PARAM_Ka];
    fTa = (*pFloatParamArr)[PARAM_Ta];
    fKe = (*pFloatParamArr)[PARAM_Ke];
    fTe = (*pFloatParamArr)[PARAM_Te];
    fKf = (*pFloatParamArr)[PARAM_Kf];
    fTf = (*pFloatParamArr)[PARAM_Tf];
    fE1 = (*pFloatParamArr)[PARAM_E1];
    fSEE1 = (*pFloatParamArr)[PARAM_SE1];
    fE2 = (*pFloatParamArr)[PARAM_E2];
    fSEE2 = (*pFloatParamArr)[PARAM_SE2];

    StateVsWith_OEL_UEL_In = (*pHardCodedArr)[HARDCODE_EXCITER_StabilizerSignal];
    GenObjectVTerminalMayBeCompensated_In = (*pHardCodedArr)[HARDCODE_EXCITER_GenVcomp];

    VRef_In = (*pHardCodedArr)[HARDCODE_EXCITER_Vref];

    if ((*pHardCodedArr)[HARDCODE_EXCITER_OELActive] > 0)
    {
        StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + (*pHardCodedArr)[HARDCODE_EXCITER_OELSignal];
    }
    if ((*pHardCodedArr)[HARDCODE_EXCITER_UELActive] > 0)
    {
        StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + (*pHardCodedArr)[HARDCODE_EXCITER_UELSignal];
    }

    // Calculate Inverse of appropriate values
    if (fTr == 0) { fInvTR = 0;} else { fInvTR = 1/fTr; }
    if (fTe == 0) { fInvTE = 0;} else { fInvTE = 1/fTe; }
    if (fTa == 0) { finvTA = 0;} else { finvTA = 1/fTa; }
    if (fTf == 0) { fInvTF = 0;} else { fInvTF = 1/fTf; }

    //--------------------------------------------------------------------
    // fActiveLimitHL = 0 means not active, 1 means high limit, 2 means low limit
    fActiveLimitHLStateVr = (*pByteActiveLimitArr)[NONWINDUPINDEX_VR];  // kate 8/8/12 only one nonwindup limit, so index is 1 (indexing of this array starts at 1)

    LOCAL_SaturationCalculateValues(fE1, fSEE1, fE2, fSEE2, fSatA, fSatB);

    if (stateEField <= fSatA)  // no saturation
    {
        tempSatValTimesInput = 0;
    }
    else 
    {
        tempSatValTimesInput = fSatB*stateEField*std::pow(stateEField - fSatA, 2);
    }
    dotEFieldBeforeWindupLimit = fInvTE*(stateVR - stateEField*fKe - tempSatValTimesInput);
    
    // Calculate initial value of the second state.
    dotSensedVt = fInvTR*(GenObjectVTerminalMayBeCompensated_In - stateSensedVt);

    //--------------------------------------------------------
    // Model of a Derivative "Washout" block
    // Ks                      -K/T
    // OUT = ------ * IN  ==>>  OUT = ------ * IN   +   K/T * IN
    // [1+Ts]                   [1+Ts]
    // OUT = StateInter + K/T * IN
    //--------------------------------------------------------
    fPIOFeedbackoutput = stateVF + fKf*fInvTF*stateEField;  // this value should be zero until something happens

    // Calculate 3rd state init val
    vInputState = VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In;

    dotVr = finvTA*(fKa*(vInputState - fPIOFeedbackoutput) - stateVR);
    // dotVR := finvTA*(fKA*(vInputState - stateVf) - stateVR); // kate 3/15/13 test

    // kate 8/8/12 add limit checking, not sure this is necessary since it already seemed to match the built in model
    if (fActiveLimitHLStateVr > 0)
    {
        if ((fActiveLimitHLStateVr == 1) & (dotVr > 0)) dotVr = 0;
        else if ((fActiveLimitHLStateVr == 2) & (dotVr < 0)) dotVr = 0;
    }

    //--------------------------------------------------------
    // -K/T
    // StateVf = ------ * StateEField
    // [1+Ts]

    // DOTStateVf = 1/T * (-K/T*StateEField - StateVf)
    //--------------------------------------------------------
    dotVf = - fInvTF*(fPIOFeedbackoutput); // kate 3/7/12
    // dotVf := fInvTf*( fKf*dotEFieldBeforeWindupLimit - stateVf ); // kate 3/15/13 test

    // Push the resulting derivatives to the appropriate memory
    (*pDotXArr)[STATE_EField] = dotEFieldBeforeWindupLimit;  // dotEFieldBeforeWindupLimit
    (*pDotXArr)[STATE_SensedVt] = dotSensedVt;  // dotVTerminalSensed
    (*pDotXArr)[STATE_VR] = dotVr;  // dotVR
    (*pDotXArr)[STATE_VF] = dotVf;  // fPIOFeedback.xdot(1)
}

//******************************************************************************
extern "C" __declspec(dllexport) void PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    double GenObjectVTerminalMayBeCompensated_In;
    double VRef_In;
    double StateVsWith_OEL_UEL_In;
    double stateSensedVt, stateEField, stateVf;
    double fKa, fVrmax, fVrmin, fKf, fTf, finvTf;
    double fPIOFeedbackoutput;
    double NewValue;

    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    BooleanArray* pIgnoreStatesArr;
    DoubleArray* pFloatParamArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    if ((*pIgnoreStatesArr)[STATE_SensedVt])
    {
        GenObjectVTerminalMayBeCompensated_In = (*pHardCodedArr)[HARDCODE_EXCITER_GenVcomp];
        (*pStatesArr)[STATE_SensedVt] = GenObjectVTerminalMayBeCompensated_In;
    }

    if ((*pIgnoreStatesArr)[STATE_VR])
    {
        stateSensedVt = (*pStatesArr)[STATE_SensedVt];
        stateEField = (*pStatesArr)[STATE_EField];
        stateVf = (*pStatesArr)[STATE_VF];
        fKa = (*pFloatParamArr)[PARAM_Ka];
        fVrmax = (*pFloatParamArr)[PARAM_Vrmax];
        fVrmin = (*pFloatParamArr)[PARAM_Vrmin];
        fKf = (*pFloatParamArr)[PARAM_Kf];
        fTf = (*pFloatParamArr)[PARAM_Tf];

        VRef_In = (*pHardCodedArr)[HARDCODE_EXCITER_Vref];

        StateVsWith_OEL_UEL_In = (*pHardCodedArr)[HARDCODE_EXCITER_StabilizerSignal];

        if ((*pHardCodedArr)[HARDCODE_EXCITER_OELActive] > 0) 
        {
            StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + (*pHardCodedArr)[HARDCODE_EXCITER_OELSignal];
        }
        if ((*pHardCodedArr)[HARDCODE_EXCITER_UELActive] > 0) 
        {
            StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + (*pHardCodedArr)[HARDCODE_EXCITER_UELSignal];
        }
        if (fTf == 0) { finvTf = 0; } else { finvTf = 1/fTf; }
        fPIOFeedbackoutput = stateVf + fKf*finvTf*stateEField;
        NewValue = (VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In - fPIOFeedbackoutput)*fKa;
        if (NewValue > fVrmax) { NewValue = fVrmax;} else if (NewValue < fVrmin) {NewValue = fVrmin;}
        (*pStatesArr)[STATE_VR] = NewValue;
    }

}

//******************************************************************************
extern "C" __declspec(dllexport) int getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    (*nonWindUpLimits->LimitStates)[NONWINDUPINDEX_VR] = STATE_VR;
    (*nonWindUpLimits->minLimits)[NONWINDUPINDEX_VR] = (*pFloatParamArr)[PARAM_Vrmin];
    (*nonWindUpLimits->maxLimits)[NONWINDUPINDEX_VR] = (*pFloatParamArr)[PARAM_Vrmax];
    return(N_NONWINDUPLIMITS);   // only one state has a non windup limit
}

extern "C" __declspec(dllexport) double ExciterEfieldOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;

    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;

    return((*pStatesArr)[STATE_EField]*(1 + (*pHardCodedArr)[HARDCODE_EXCITER_GenSpeedDeviationPU])); 
}

}
