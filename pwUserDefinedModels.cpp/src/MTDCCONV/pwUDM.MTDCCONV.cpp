module pwUDM.MTDCCONV;

import std;
import pwUDM.Core;
import pwUDM.MTDCConverter;

namespace pwUDM::MTDCCONV {

//******************************************************************************
extern "C" __declspec(dllexport) int DLLVersion()
{
    return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int modelClassName(int* StrSize, char16_t* StrBuf, int dummy)
{
    int result = pwProvideString(StrBuf, StrSize, pwModelClassString(ModelClass::MTDCConvertor));
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
        case PARAM_Tf_Idc:    ts = u"Tf_Idc"; break;
        case PARAM_Tf_Vdc:    ts = u"Tf_Vdc"; break;
        case PARAM_Tf_Vac:    ts = u"Tf_Vac"; break;
        case PARAM_Ki:        ts = u"Ki"; break;
        case PARAM_Id_base:   ts = u"Id_base"; break;
        case PARAM_CosMin:    ts = u"CosMin"; break;
        case PARAM_CosMax:    ts = u"CosMax"; break;
        case PARAM_IdRefLim:  ts = u"IdRefLim"; break;
        case PARAM_Id_margin: ts = u"Id_margin"; break;
        default:              ts = u""; break;
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
        case STATE_IdcSense: ts = u"Idc Sense"; break;
        case STATE_VdcSense: ts = u"Vdc Sense"; break;
        case STATE_VacSense: ts = u"Vac Sense"; break;
        case STATE_CosAngle: ts = u"Cos Angle"; break;
        default:             ts = u""; break;
    }

    int result = pwProvideString(StrBuf, StrSize, ts);
    return result;
}

//******************************************************************************
/* non specified for this model
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
/* non specified for this model
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

    // Example MTDCCONV parameters
    pDoubArr = ParamsAndStates->FloatParams;
    (*pDoubArr)[PARAM_Tf_Idc] = 0.02;
    (*pDoubArr)[PARAM_Tf_Vdc] = 0.02;
    (*pDoubArr)[PARAM_Tf_Vac] = 0.05;
    (*pDoubArr)[PARAM_Ki] = 5.0;
    (*pDoubArr)[PARAM_Id_base] = 1000.0;
    (*pDoubArr)[PARAM_CosMin] = -1.0;
    (*pDoubArr)[PARAM_CosMax] = 1.0;
    (*pDoubArr)[PARAM_IdRefLim] = 0.0;
    (*pDoubArr)[PARAM_Id_margin] = 0.0;
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
    // Parameters,
    double fTf_Idc, fTf_Vdc, fTf_Vac, fId_margin;
    // Signals from Simulator at the start of the simulation
    double local_Idc, local_Vdc, local_Vac, local_CosAngle;

    DoubleArray* pHardCodedArr;
    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    BooleanArray* pIgnoreStatesArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    //--------------------------------------------------------------------
    // Grab States, Params, Algebraics Needed for this method
    //--------------------------------------------------------------------
    fTf_Idc = (*pFloatParamArr)[PARAM_Tf_Idc];
    fTf_Vdc = (*pFloatParamArr)[PARAM_Tf_Vdc];
    fTf_Vac = (*pFloatParamArr)[PARAM_Tf_Vac];
    fId_margin = (*pFloatParamArr)[PARAM_Id_margin];

    local_Idc = (*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentDC];
    local_Vdc = (*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageDC];
    local_Vac = (*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageAC];
    local_CosAngle = (*pHardCodedArr)[HARDCODE_MTDCCONV_CosControlAngle];

    // Ignored States
    // A time constant of zero means the sensed value follows its input directly
    if (fTf_Idc == 0) { (*pIgnoreStatesArr)[STATE_IdcSense] = 1; }
    if (fTf_Vdc == 0) { (*pIgnoreStatesArr)[STATE_VdcSense] = 1; }
    if (fTf_Vac == 0) { (*pIgnoreStatesArr)[STATE_VacSense] = 1; }

    // The following parameters should actually get SET, not read in!
    // The regulator starts at a steady state: it sees no error when the current order
    // is the present current plus the margin.
    (*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentReference] = local_Idc + fId_margin;

    // Write out initial state vector
    (*pStatesArr)[STATE_IdcSense] = local_Idc;
    (*pStatesArr)[STATE_VdcSense] = local_Vdc;
    (*pStatesArr)[STATE_VacSense] = local_Vac;
    (*pStatesArr)[STATE_CosAngle] = local_CosAngle;
}

//******************************************************************************
extern "C" __declspec(dllexport) void calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{
    // Parameters
    double fTf_Idc, fTf_Vdc, fTf_Vac, fKi, fId_base, fId_margin;
    double fInvTf_Idc, fInvTf_Vdc, fInvTf_Vac;

    // State variables
    double stateIdcSense, stateVdcSense, stateVacSense;

    // FofX or xdot,
    double dotIdcSense, dotVdcSense, dotVacSense, dotCosAngle;
    double Id_Ref_In, Idc_In, Vdc_In, Vac_In;
    double fCurrentError;

    unsigned char fActiveLimitHLStateCos;

    DoubleArray* pFloatParamArr;
    DoubleArray* pHardCodedArr;
    DoubleArray* pStatesArr;
    ByteArray* pByteActiveLimitArr;
    DoubleArray* pDotXArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pByteActiveLimitArr = nonWindUpLimits->activeLimits;
    pDotXArr = dotX;

    //--------------------------------------------------------------------
    // Grab States, Params, Algebraics Needed for this method
    //--------------------------------------------------------------------
    stateIdcSense = (*pStatesArr)[STATE_IdcSense];
    stateVdcSense = (*pStatesArr)[STATE_VdcSense];
    stateVacSense = (*pStatesArr)[STATE_VacSense];

    fTf_Idc = (*pFloatParamArr)[PARAM_Tf_Idc];
    fTf_Vdc = (*pFloatParamArr)[PARAM_Tf_Vdc];
    fTf_Vac = (*pFloatParamArr)[PARAM_Tf_Vac];
    fKi = (*pFloatParamArr)[PARAM_Ki];
    fId_base = (*pFloatParamArr)[PARAM_Id_base];
    fId_margin = (*pFloatParamArr)[PARAM_Id_margin];

    Id_Ref_In = (*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentReference];
    Idc_In = (*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentDC];
    Vdc_In = (*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageDC];
    Vac_In = (*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageAC];

    // Calculate Inverse of appropriate values
    if (fTf_Idc == 0) { fInvTf_Idc = 0; } else { fInvTf_Idc = 1/fTf_Idc; }
    if (fTf_Vdc == 0) { fInvTf_Vdc = 0; } else { fInvTf_Vdc = 1/fTf_Vdc; }
    if (fTf_Vac == 0) { fInvTf_Vac = 0; } else { fInvTf_Vac = 1/fTf_Vac; }
    if (fId_base == 0) { fId_base = 1; }

    //--------------------------------------------------------------------
    // fActiveLimitHL = 0 means not active, 1 means high limit, 2 means low limit
    fActiveLimitHLStateCos = (*pByteActiveLimitArr)[NONWINDUPINDEX_CosAngle];  // Only one nonwindup limit, so index is 0

    // dot sensed values: first order lags of the DC current, the DC voltage and the AC voltage
    dotIdcSense = fInvTf_Idc*(Idc_In - stateIdcSense);
    dotVdcSense = fInvTf_Vdc*(Vdc_In - stateVdcSense);
    dotVacSense = fInvTf_Vac*(Vac_In - stateVacSense);

    // dot cos angle: an integral current regulator. The error is in pu of Id_base.
    fCurrentError = (Id_Ref_In - fId_margin - stateIdcSense)/fId_base;
    dotCosAngle = fKi*fCurrentError;
    // Note: the cos angle has a limit, and when that limit is active, we want dotCosAngle to be zero.
    if (fActiveLimitHLStateCos > 0) {
        // 1 indicates it's stuck at the HIGH limit
        // 2 indicates it's stuck at the LOW limit
        if ((fActiveLimitHLStateCos == 1) && (dotCosAngle > 0)) {
            dotCosAngle = 0;
        }
        else if ((fActiveLimitHLStateCos == 2) && (dotCosAngle < 0)) {
            dotCosAngle = 0;
        }
    }

    //--------------------------------------------------------------------
    // Push the resulting derivatives to the appropriate memory
    //--------------------------------------------------------------------
    (*pDotXArr)[STATE_IdcSense] = dotIdcSense;
    (*pDotXArr)[STATE_VdcSense] = dotVdcSense;
    (*pDotXArr)[STATE_VacSense] = dotVacSense;
    (*pDotXArr)[STATE_CosAngle] = dotCosAngle;
}

//******************************************************************************
extern "C" __declspec(dllexport) void PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    double fIdRefLim;

    DoubleArray* pHardCodedArr;
    DoubleArray* pFloatParamArr;
    DoubleArray* pStatesArr;
    BooleanArray* pIgnoreStatesArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pFloatParamArr = ParamsAndStates->FloatParams;
    pStatesArr = ParamsAndStates->States;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    // An ignored sensed value is just its input
    if ((*pIgnoreStatesArr)[STATE_IdcSense]) { (*pStatesArr)[STATE_IdcSense] = (*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentDC]; }
    if ((*pIgnoreStatesArr)[STATE_VdcSense]) { (*pStatesArr)[STATE_VdcSense] = (*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageDC]; }
    if ((*pIgnoreStatesArr)[STATE_VacSense]) { (*pStatesArr)[STATE_VacSense] = (*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageAC]; }

    // The current limit of the converter also limits the current order that Simulator passes in.
    // A limit of zero or less means no limit.
    fIdRefLim = (*pFloatParamArr)[PARAM_IdRefLim];
    if ((fIdRefLim > 0) && ((*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentReference] > fIdRefLim)) {
        (*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentReference] = fIdRefLim;
    }
}

//******************************************************************************
extern "C" __declspec(dllexport) int getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    (*nonWindUpLimits->LimitStates)[NONWINDUPINDEX_CosAngle] = STATE_CosAngle;
    (*nonWindUpLimits->minLimits)[NONWINDUPINDEX_CosAngle] = (*pFloatParamArr)[PARAM_CosMin];
    (*nonWindUpLimits->maxLimits)[NONWINDUPINDEX_CosAngle] = (*pFloatParamArr)[PARAM_CosMax];

    return(1);   // only one state has a non windup limit
}

//******************************************************************************
extern "C" __declspec(dllexport) double MTDCConverterCosControlAngle(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    DoubleArray* pStatesArr;

    pStatesArr = ParamsAndStates->States;

    // The cosine of the control angle is the output of the current regulator
    return((*pStatesArr)[STATE_CosAngle]);
}

//******************************************************************************
extern "C" __declspec(dllexport) double MTDCConverterIdcSense(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    BooleanArray* pIgnoreStatesArr;

    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    // An ignored sensed value is just its input
    if ((*pIgnoreStatesArr)[STATE_IdcSense]) { return((*pHardCodedArr)[HARDCODE_MTDCCONV_CurrentDC]); }
    else { return((*pStatesArr)[STATE_IdcSense]); }
}

//******************************************************************************
extern "C" __declspec(dllexport) double MTDCConverterVdcSense(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    BooleanArray* pIgnoreStatesArr;

    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    // An ignored sensed value is just its input
    if ((*pIgnoreStatesArr)[STATE_VdcSense]) { return((*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageDC]); }
    else { return((*pStatesArr)[STATE_VdcSense]); }
}

//******************************************************************************
extern "C" __declspec(dllexport) double MTDCConverterVacSense(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    BooleanArray* pIgnoreStatesArr;

    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

    // An ignored sensed value is just its input
    if ((*pIgnoreStatesArr)[STATE_VacSense]) { return((*pHardCodedArr)[HARDCODE_MTDCCONV_VoltageAC]); }
    else { return((*pStatesArr)[STATE_VacSense]); }
}

//******************************************************************************
extern "C" __declspec(dllexport) void MTDCConverterCurrentLimitAndMargin(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* IdRefLim, double* fid_Margin)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;

    // A limit of zero or less means no limit
    *IdRefLim = (*pFloatParamArr)[PARAM_IdRefLim];
    *fid_Margin = (*pFloatParamArr)[PARAM_Id_margin];
}

}
