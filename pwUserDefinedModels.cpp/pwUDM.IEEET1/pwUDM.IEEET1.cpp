#include "..\common\pwUDM.h"
#include "..\common\pwUDM.Exciter.h"
#include "..\common\pwUDM.Helper.h"
#include "pwUDM.IEEET1.h"
#include <stdexcept>


PW_UDM_DllExport(int) DLLVersion()
{
    return(1);
}


PW_UDM_DllExport(int) modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
    enumModelClass udmClass;
    wchar_t* str;
    int stringLength;    
        
    udmClass = mcnExciter;

    str = pwModelClassString(udmClass);
    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


PW_UDM_DllExport(void) allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds)
{
    (*numbersOfEverything).nFloatParams = N_F_PARAMS;
    (*numbersOfEverything).nIntParams = N_I_PARAMS;
    (*numbersOfEverything).nStrParams = N_S_PARAMS;
    (*numbersOfEverything).nStates = N_STATES;
    (*numbersOfEverything).nAlgebraics = N_ALGEBRAICS;
    (*numbersOfEverything).nNonWindUpLimits = N_NONWINDUPLIMITS;
}


PW_UDM_DllExport(int) parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    wchar_t* str;
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


PW_UDM_DllExport(int) stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    wchar_t* str;
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


PW_UDM_DllExport(void) getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
    (*((*ParamsAndStates).FloatParams))[PARAM_Tr] = 0.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Ka] = 50.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Ta] = 0.04;
    (*((*ParamsAndStates).FloatParams))[PARAM_Vrmax] = 1.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Vrmin] = -1.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Ke] = 0.06;
    (*((*ParamsAndStates).FloatParams))[PARAM_Te] = 0.06;
    (*((*ParamsAndStates).FloatParams))[PARAM_Kf] = 0.09;
    (*((*ParamsAndStates).FloatParams))[PARAM_Tf] = 1.46;
    (*((*ParamsAndStates).FloatParams))[PARAM_Switch] = 0.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_E1] = 2.80;
    (*((*ParamsAndStates).FloatParams))[PARAM_SE1] = 0.04;
    (*((*ParamsAndStates).FloatParams))[PARAM_E2] = 3.73;
    (*((*ParamsAndStates).FloatParams))[PARAM_SE2] = 0.33;
    (*((*ParamsAndStates).FloatParams))[PARAM_Spdmlt] = 0.0;
}


PW_UDM_DllExport(int) SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds)
{
    return(0);
}


PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    double fTr, fKa, fTa, fVrmax, fVrmin, fKe, fKf, fTf, fE1, fSEE1, fE2, fSEE2;
    double fSatA, fSatB;
    double local_Vrmax;
    double fVRef; 
    double local_EField;
    double local_VR;
    double local_VTerminalSensed;
    double local_VF;
    double fInvKa;
    
    fTr = (*((*ParamsAndStates).FloatParams))[PARAM_Tr];
    fKa = (*((*ParamsAndStates).FloatParams))[PARAM_Ka];
    fTa = (*((*ParamsAndStates).FloatParams))[PARAM_Ta];
    fVrmax = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmax];
    fVrmin = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmin];
    fKe = (*((*ParamsAndStates).FloatParams))[PARAM_Ke];
    fKf = (*((*ParamsAndStates).FloatParams))[PARAM_Kf];
    fTf = (*((*ParamsAndStates).FloatParams))[PARAM_Tf];
    fE1 = (*((*ParamsAndStates).FloatParams))[PARAM_E1];
    fSEE1 = (*((*ParamsAndStates).FloatParams))[PARAM_SE1];
    fE2 = (*((*ParamsAndStates).FloatParams))[PARAM_E2];
    fSEE2 = (*((*ParamsAndStates).FloatParams))[PARAM_SE2];

    if (fTf == 0)
    {
        fKf = 0;
    }

    local_EField = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_InitFieldVoltage]; 
    local_VTerminalSensed  = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_GenVComp]; 

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

    if (fTr == 0) { (*((*ParamsAndStates).IgnoreStates))[STATE_SensedVt] = 1;}
    if (fTa == 0) { (*((*ParamsAndStates).IgnoreStates))[STATE_Vr] = 1;}

    (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_Vref] = fVRef;    
    local_VF = (fVRef - local_VTerminalSensed) - local_VR*fInvKa;
    
    (*((*ParamsAndStates).States))[STATE_Efield] = local_EField;
    (*((*ParamsAndStates).States))[STATE_SensedVt] = local_VTerminalSensed;
    (*((*ParamsAndStates).States))[STATE_Vr] = local_VR;
    (*((*ParamsAndStates).States))[STATE_Vf] = local_VF;
}


PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
    double    fTr, fKa, fTa, fVrmax, fVrmin, fKe, fTe, fKf, fTf, fSwitch, fE1, fSEE1, fE2, fSEE2, fSpdmlt;
    double    fInvTE, fInvTR, fInvTA, fInvTF;
    double    stateEField, stateSensedVt, stateVR, stateVF;
    double    dotEFieldBeforeWindupLimit, dotSensedVt, dotVr, dotVf;

    double    fSatA, fSatB;
    double    tempSatValTimesInput, vInputState;
    double    GenObjectVTerminalMayBeCompensated_In , VRef_In, StateVsWith_OEL_UEL_In;
    double    fPIOFeedbackoutput;

    unsigned __int8 fActiveLimitHLStateVr;

    stateEField = (*((*ParamsAndStates).States))[STATE_Efield];
    stateSensedVt = (*((*ParamsAndStates).States))[STATE_SensedVt];
    stateVR = (*((*ParamsAndStates).States))[STATE_Vr];
    stateVF = (*((*ParamsAndStates).States))[STATE_Vf];

    fTr = (*((*ParamsAndStates).FloatParams))[PARAM_Tr];
    fKa = (*((*ParamsAndStates).FloatParams))[PARAM_Ka];
    fTa = (*((*ParamsAndStates).FloatParams))[PARAM_Ta];
    fVrmax = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmax];
    fVrmin = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmin];
    fKe = (*((*ParamsAndStates).FloatParams))[PARAM_Ke];
    fTe = (*((*ParamsAndStates).FloatParams))[PARAM_Te];
    fKf = (*((*ParamsAndStates).FloatParams))[PARAM_Kf];
    fTf = (*((*ParamsAndStates).FloatParams))[PARAM_Tf];
    fSwitch = (*((*ParamsAndStates).FloatParams))[PARAM_Switch];
    fE1 = (*((*ParamsAndStates).FloatParams))[PARAM_E1];
    fSEE1 = (*((*ParamsAndStates).FloatParams))[PARAM_SE1];
    fE2 = (*((*ParamsAndStates).FloatParams))[PARAM_E2];
    fSEE2 = (*((*ParamsAndStates).FloatParams))[PARAM_SE2];
    fSpdmlt = (*((*ParamsAndStates).FloatParams))[PARAM_Spdmlt];

    StateVsWith_OEL_UEL_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_OELSignal];
    GenObjectVTerminalMayBeCompensated_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_GenVComp];
    VRef_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_Vref];

    if ((*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_OELActive] > 0)
    {
        StateVsWith_OEL_UEL_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_OELSignal];
    }
    if ((*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_UELActive] > 0)
    {
        StateVsWith_OEL_UEL_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_UELSignal];
    }

    if (fTr == 0) { fInvTR = 0;} else { fInvTR = 1/fTr; }
    if (fTe == 0) { fInvTE = 0;} else { fInvTE = 1/fTe; }
    if (fTa == 0) { fInvTA = 0;} else { fInvTA = 1/fTa; }
    if (fTf == 0) { fInvTF = 0;} else { fInvTF = 1/fTf; }

    fActiveLimitHLStateVr = (*((*nonWindUpLimits).activeLimits))[NONWINDUPINDEX_VR];

    LOCAL_SaturationCalculateValues(fE1, fSEE1, fE2, fSEE2, fSatA, fSatB);

    stateEField = stateEField;
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
    fPIOFeedbackoutput = stateVF + fKf*fInvTF*stateEField;
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


PW_UDM_DllExport(void) PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    double GenObjectVTerminalMayBeCompensated_In, VRef_In, StateVsWith_OEL_UEL_In, stateSensedVt, fKa, fVrmax, fVrmin, NewValue;

    if ((*((*ParamsAndStates).IgnoreStates))[STATE_SensedVt])
    {
        GenObjectVTerminalMayBeCompensated_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_GenVComp];
        (*((*ParamsAndStates).States))[STATE_SensedVt] = GenObjectVTerminalMayBeCompensated_In;
    }

    if ((*((*ParamsAndStates).IgnoreStates))[STATE_Vr])
    {
        stateSensedVt = (*((*ParamsAndStates).States))[STATE_SensedVt];
        fKa = (*((*ParamsAndStates).FloatParams))[PARAM_Ka];
        fVrmax = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmax];
        fVrmin = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmin];
        VRef_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_Vref];
        StateVsWith_OEL_UEL_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_StabilizerSignal];

        if ((*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_OELActive] > 0) 
        {
            StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_OELSignal];
        }
        if ((*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_UELActive] > 0) 
        {
            StateVsWith_OEL_UEL_In = StateVsWith_OEL_UEL_In + (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_UELSignal];
        }
        NewValue = (VRef_In - stateSensedVt + StateVsWith_OEL_UEL_In)*fKa;
        if (NewValue > fVrmax) { NewValue = fVrmax;} else if (NewValue < fVrmin) {NewValue = fVrmin;}
        (*((*ParamsAndStates).States))[STATE_Vr] = NewValue;
    }

}


PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{    
    (*((*nonWindUpLimits).limitStates))[NONWINDUPINDEX_VR] = STATE_Vr;
    (*((*nonWindUpLimits).minLimits))[NONWINDUPINDEX_VR] = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmin];
    (*((*nonWindUpLimits).maxLimits))[NONWINDUPINDEX_VR] = (*((*ParamsAndStates).FloatParams))[PARAM_Vrmax];
    return(N_NONWINDUPLIMITS);
}


PW_UDM_DllExport(double) ExciterEfieldOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{    
    return((*((*ParamsAndStates).States))[STATE_Efield]*(1 + (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_EXCITER_GenSpeedDeviationPU])); 
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


void LOCAL_PWSolveQuadraticEq (double a, double b, double c, double& x1, double& x2, bool& Imag)
{
    double    t;

    t    = b*b - 4*a*c;
    if (t > 0)
    {
        Imag = 0;
        t = sqrt(t);
        x1 = (-b+t)/(2*a);
        x2 = (-b-t)/(2*a);
    }
    else
    {
        Imag = 1;
        x1 = -b/(2*a);
        x2 = sqrt(-t)/(2*a);
    }
}


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