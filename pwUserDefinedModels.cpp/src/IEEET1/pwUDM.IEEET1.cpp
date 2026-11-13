module pwUDM.IEEET1;

import std;
import pwUDM;
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
        return(fSatB*(input - fSatA)*(input - fSatA));
    }
}


void LOCAL_PWSolveQuadraticEq (double a, double b, double c, double& x1, double& x2, bool& Imag)
{
    double    t;

    t    = b*b - 4*a*c;
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
    double tempA, tempB, tempC, result1, result2;
    bool isImag;

    if (tempS1 <= 0)
    {
        fSatA = tempE1;
        if (tempE2 == tempE1)
        {
            fSatB = 0;
        }
        else
        {
            fSatB = tempS2/((tempE2 - fSatA)*(tempE2 - fSatA));
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
        tempC = tempS2*(tempE1)*(tempE1) - tempS1*(tempE2)*(tempE2);
        LOCAL_PWSolveQuadraticEq(tempA, tempB, tempC, result1, result2, isImag);
        fSatA = result2;
        fSatB = (tempS1)/((tempE1-fSatA)*(tempE1-fSatA));
    }
}


void LOCAL_AutoSetVrminVrmax(double local_E2, double local_Ke, double fSatA, double fSatB, double& local_Vrmin, double& local_Vrmax)
{
    if (local_Ke <= 0)
    {
        if (local_E2 <= fSatA)
        {
            local_Vrmax = 0; // no saturation
        }
        else
        {
            local_Vrmax = fSatB*local_E2*(local_E2-fSatA)*(local_E2-fSatA);
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

extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
    return(1);
}


extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
    ModelClass udmClass;
    const wchar_t* str;
    int stringLength;    
        
    udmClass = ModelClass::Exciter;

    str = pwModelClassString(udmClass);
    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


extern "C" __declspec(dllexport) void __stdcall allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds)
{
    numbersOfEverything->nFloatParams = N_F_PARAMS;
    numbersOfEverything->nIntParams = N_I_PARAMS;
    numbersOfEverything->nStrParams = N_S_PARAMS;
    numbersOfEverything->nStates = N_STATES;
    numbersOfEverything->nAlgebraics = N_ALGEBRAICS;
    numbersOfEverything->nNonWindUpLimits = N_NONWINDUPLIMITS;
}


extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* str;
    int stringLength;
    
    if (*ParamNum == PARAM_Tr) {
        str = L"Tr";
    }
    else if (*ParamNum == PARAM_Ka) {
        str = L"Ka";
    }
    else if (*ParamNum == PARAM_Ta) {
        str = L"Ta";
    }
    else if (*ParamNum == PARAM_Vrmax) {
        str = L"Vrmax";
    }
    else if (*ParamNum == PARAM_Vrmin) {
        str = L"Vrmin";
    }
    else if (*ParamNum == PARAM_Ke) {
        str = L"Ke";
    }
    else if (*ParamNum == PARAM_Te) {
        str = L"Te";
    }
    else if (*ParamNum == PARAM_Kf) {
        str = L"Kf";
    }
    else if (*ParamNum == PARAM_Tf) {
        str = L"Tf";
    }
    else if (*ParamNum == PARAM_Switch) {
        str = L"Switch";
    }
    else if (*ParamNum == PARAM_E1) {
        str = L"E1";
    }
    else if (*ParamNum == PARAM_SE1) {
        str = L"SE(E1)";
    }
    else if (*ParamNum == PARAM_E2) {
        str = L"E2";
    }
    else if (*ParamNum == PARAM_SE2) {
        str = L"SE(E2)";
    }
    else if (*ParamNum == PARAM_Spdmlt) {
        str = L"Spdmlt";
    }
    else {
        str = L"";
    }
    
    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


extern "C" __declspec(dllexport) int __stdcall stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* str;
    int stringLength;

    if (*StateNum == STATE_Efield) {
        str = L"EField";
    }
    else if (*StateNum == STATE_SensedVt) {
        str = L"Sensed Vt";
    }
    else if (*StateNum == STATE_Vr) {
        str = L"VR";
    }
    else if (*StateNum == STATE_Vf) {
        str = L"VF";
    }
    else {
        str =L"";
    }
    
    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;

    floatParams[PARAM_Tr] = 0.0;
    floatParams[PARAM_Ka] = 50.0;
    floatParams[PARAM_Ta] = 0.04;
    floatParams[PARAM_Vrmax] = 1.0;
    floatParams[PARAM_Vrmin] = -1.0;
    floatParams[PARAM_Ke] = 0.06;
    floatParams[PARAM_Te] = 0.06;
    floatParams[PARAM_Kf] = 0.09;
    floatParams[PARAM_Tf] = 1.46;
    floatParams[PARAM_Switch] = 0.0;
    floatParams[PARAM_E1] = 2.80;
    floatParams[PARAM_SE1] = 0.04;
    floatParams[PARAM_E2] = 3.73;
    floatParams[PARAM_SE2] = 0.33;
    floatParams[PARAM_Spdmlt] = 0.0;
}


extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds)
{
    return(0);
}


extern "C" __declspec(dllexport) void __stdcall initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TBooleanArray& ignoreStates = *ParamsAndStates->IgnoreStates;

    double fTr, fKa, fTa, fVrmax, fVrmin, fKe, fE1, fSEE1, fE2, fSEE2;
    double fSatA, fSatB;
    double local_Vrmax;
    double fVRef; 
    double local_EField;
    double local_VR;
    double local_VTerminalSensed;
    double local_VF;
    double fInvKa;
    
    fTr = floatParams[PARAM_Tr];
    fKa = floatParams[PARAM_Ka];
    fTa = floatParams[PARAM_Ta];
    fVrmax = floatParams[PARAM_Vrmax];
    fVrmin = floatParams[PARAM_Vrmin];
    fKe = floatParams[PARAM_Ke];
    fE1 = floatParams[PARAM_E1];
    fSEE1 = floatParams[PARAM_SE1];
    fE2 = floatParams[PARAM_E2];
    fSEE2 = floatParams[PARAM_SE2];

    local_EField = hardCodedSignals[HARDCODE_EXCITER_InitFieldVoltage]; 
    local_VTerminalSensed  = hardCodedSignals[HARDCODE_EXCITER_GenVComp]; 

    LOCAL_SaturationCalculateValues(fE1, fSEE1, fE2, fSEE2, fSatA, fSatB); 

    if (fVrmax == 0)
    {
        LOCAL_AutoSetVrminVrmax(fE2, fKe, fSatA, fSatB, fVrmin, fVrmax);  
    }

    if (fKe == 0)
    {
        fKe = LOCAL_AutoSetKeNew(fVrmax, fVrmin, local_EField, fSatA, fSatB);
    }

    if (local_EField <= fSatA)
    {
        local_Vrmax    = 0;
    }
    else
    {
        local_Vrmax = fSatB*local_EField*(local_EField-fSatA)*(local_EField-fSatA);
    }
    local_VR = local_EField*fKe + local_Vrmax;

    if (fKa == 0) { fInvKa = 0;} else { fInvKa = 1/fKa; }
    fVRef = local_VTerminalSensed + local_VR*fInvKa; 

    if (fTr == 0) { ignoreStates[STATE_SensedVt] = 1;}
    if (fTa == 0) { ignoreStates[STATE_Vr] = 1;}

    hardCodedSignals[HARDCODE_EXCITER_Vref] = fVRef;    
    local_VF = (fVRef - local_VTerminalSensed) - local_VR*fInvKa;
    
    states[STATE_Efield] = local_EField;
    states[STATE_SensedVt] = local_VTerminalSensed;
    states[STATE_Vr] = local_VR;
    states[STATE_Vf] = local_VF;
}


extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TByteArray& activeLimits = *nonWindUpLimits->activeLimits;

    double fTr, fKa, fTa, fKe, fTe, fKf, fTf, fE1, fSEE1, fE2, fSEE2;
    double    fInvTE, fInvTR, fInvTA, fInvTF;
    double    stateEField, stateSensedVt, stateVR, stateVF;
    double    dotEFieldBeforeWindupLimit, dotSensedVt, dotVr, dotVf;

    double    fSatA, fSatB;
    double    tempSatValTimesInput, vInputState;
    double    GenObjectVTerminalMayBeCompensated_In , VRef_In, StateVsWith_OEL_UEL_In;
    unsigned char fActiveLimitHLStateVr;

    stateEField = states[STATE_Efield];
    stateSensedVt = states[STATE_SensedVt];
    stateVR = states[STATE_Vr];
    stateVF = states[STATE_Vf];

    fTr = floatParams[PARAM_Tr];
    fKa = floatParams[PARAM_Ka];
    fTa = floatParams[PARAM_Ta];
    fKe = floatParams[PARAM_Ke];
    fTe = floatParams[PARAM_Te];
    fKf = floatParams[PARAM_Kf];
    fTf = floatParams[PARAM_Tf];
    fE1 = floatParams[PARAM_E1];
    fSEE1 = floatParams[PARAM_SE1];
    fE2 = floatParams[PARAM_E2];
    fSEE2 = floatParams[PARAM_SE2];

    StateVsWith_OEL_UEL_In = hardCodedSignals[HARDCODE_EXCITER_OELSignal];
    GenObjectVTerminalMayBeCompensated_In = hardCodedSignals[HARDCODE_EXCITER_GenVComp];
    VRef_In = hardCodedSignals[HARDCODE_EXCITER_Vref];

    if (hardCodedSignals[HARDCODE_EXCITER_OELActive] > 0)
    {
        StateVsWith_OEL_UEL_In = hardCodedSignals[HARDCODE_EXCITER_OELSignal];
    }
    if (hardCodedSignals[HARDCODE_EXCITER_UELActive] > 0)
    {
        StateVsWith_OEL_UEL_In = hardCodedSignals[HARDCODE_EXCITER_UELSignal];
    }

    if (fTr == 0) { fInvTR = 0;} else { fInvTR = 1/fTr; }
    if (fTe == 0) { fInvTE = 0;} else { fInvTE = 1/fTe; }
    if (fTa == 0) { fInvTA = 0;} else { fInvTA = 1/fTa; }
    if (fTf == 0) { fInvTF = 0;} else { fInvTF = 1/fTf; }

    fActiveLimitHLStateVr = activeLimits[NONWINDUPINDEX_VR];

    LOCAL_SaturationCalculateValues(fE1, fSEE1, fE2, fSEE2, fSatA, fSatB);

    if (stateEField <= fSatA)
    {
        tempSatValTimesInput = 0;
    }
    else 
    {
        tempSatValTimesInput = fSatB*stateEField*(stateEField - fSatA)*(stateEField - fSatA);
    }
    dotEFieldBeforeWindupLimit = fInvTE*(stateVR - stateEField*fKe - tempSatValTimesInput);
    
    dotSensedVt = fInvTR*(GenObjectVTerminalMayBeCompensated_In - stateSensedVt);
    vInputState = VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In;
    dotVr = fInvTA*(fKa*(vInputState - stateVF) - stateVR);
    if (fActiveLimitHLStateVr > 0)
    {
        if ((fActiveLimitHLStateVr == 1) & (dotVr > 0)) dotVr = 0;
        else if ((fActiveLimitHLStateVr == 2) & (dotVr < 0)) dotVr = 0;
    }
    dotVf                = fInvTF*(fKf*dotEFieldBeforeWindupLimit - stateVF);

    (*dotX)[STATE_Efield] = dotEFieldBeforeWindupLimit;
    (*dotX)[STATE_SensedVt] = dotSensedVt;
    (*dotX)[STATE_Vr] = dotVr;
    (*dotX)[STATE_Vf] = dotVf;
}


extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TBooleanArray& ignoreStates = *ParamsAndStates->IgnoreStates;

    double GenObjectVTerminalMayBeCompensated_In, VRef_In, StateVsWith_OEL_UEL_In, stateSensedVt, fKa, fVrmax, fVrmin, NewValue;

    if (ignoreStates[STATE_SensedVt])
    {
        GenObjectVTerminalMayBeCompensated_In = hardCodedSignals[HARDCODE_EXCITER_GenVComp];
        states[STATE_SensedVt] = GenObjectVTerminalMayBeCompensated_In;
    }

    if (ignoreStates[STATE_Vr])
    {
        stateSensedVt = states[STATE_SensedVt];
        fKa = floatParams[PARAM_Ka];
        fVrmax = floatParams[PARAM_Vrmax];
        fVrmin = floatParams[PARAM_Vrmin];
        VRef_In = hardCodedSignals[HARDCODE_EXCITER_Vref];
        StateVsWith_OEL_UEL_In = hardCodedSignals[HARDCODE_EXCITER_StabilizerSignal];

        if (hardCodedSignals[HARDCODE_EXCITER_OELActive] > 0) 
        {
            StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + hardCodedSignals[HARDCODE_EXCITER_OELSignal];
        }
        if (hardCodedSignals[HARDCODE_EXCITER_UELActive] > 0) 
        {
            StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + hardCodedSignals[HARDCODE_EXCITER_UELSignal];
        }
        NewValue = (VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In)*fKa;
        if (NewValue > fVrmax) { NewValue = fVrmax;} else if (NewValue < fVrmin) {NewValue = fVrmin;}
        states[STATE_Vr] = NewValue;
    }

}


extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
    TDoubleArray& minLimits = *nonWindUpLimits->minLimits;
    TDoubleArray& maxLimits = *nonWindUpLimits->maxLimits;
    TIntegerArray& limitStates = *nonWindUpLimits->limitStates;
    
    limitStates[NONWINDUPINDEX_VR] = STATE_Vr;
    minLimits[NONWINDUPINDEX_VR] = floatParams[PARAM_Vrmin];
    maxLimits[NONWINDUPINDEX_VR] = floatParams[PARAM_Vrmax];
    return(N_NONWINDUPLIMITS);
}


extern "C" __declspec(dllexport) double __stdcall ExciterEfieldOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    
    return(states[STATE_Efield]*(1 + hardCodedSignals[HARDCODE_EXCITER_GenSpeedDeviationPU])); 
}

}
