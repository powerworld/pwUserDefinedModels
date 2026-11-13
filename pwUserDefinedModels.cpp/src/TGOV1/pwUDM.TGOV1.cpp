module pwUDM.TGOV1;

import std;
import pwUDM;
import pwUDM.Governor;

namespace pwUDM::TGOV1 {

extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
	return(1);
}


extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{	
    ModelClass udmClass;
    const wchar_t* str;
	int stringLength;
	
    udmClass = ModelClass::Governor;

    str = pwModelClassString(udmClass);
	stringLength = pwStringCopy(StrBuf, StrSize, str);
	return(stringLength);
}


extern "C" __declspec(dllexport) void __stdcall allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds)
{
	numbersOfEverything->nFloatParams		= N_F_PARAMS;
	numbersOfEverything->nIntParams		= N_I_PARAMS;
	numbersOfEverything->nStrParams		= N_S_PARAMS;
	numbersOfEverything->nStates			= N_STATES;
	numbersOfEverything->nAlgebraics		= N_ALGEBRAICS;	
	numbersOfEverything->nNonWindUpLimits	= N_NONWINDUPLIMITS;
}


extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* str;
    int stringLength;
	
	if (*ParamNum == PARAM_R) {
		str = L"R";
	}
	else if (*ParamNum == PARAM_T1) {
		str = L"T1";
	}
	else if (*ParamNum == PARAM_Vmax) {
		str = L"Vmax";
	}
	else if (*ParamNum == PARAM_Vmin) {
		str = L"Vmin";
	}
	else if (*ParamNum == PARAM_T2) {
		str = L"T2";
	}
	else if (*ParamNum == PARAM_T3) {
		str = L"T3";
	}
	else if (*ParamNum == PARAM_Dt) {
		str = L"Dt";
	}
	else if (*ParamNum == PARAM_Trate) {
		str = L"Trate";
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

	if (*StateNum == STATE_TurbPower) {
		str = L"Turbine Power";
	}
	else if (*StateNum == STATE_ValvePos) {
		str = L"Valve Position";
	}
	else {
        str = L"";
	}
	
    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;

	floatParams[PARAM_R] = 0.05;
	floatParams[PARAM_T1] = 0.5;
	floatParams[PARAM_Vmax] = 1.0;
	floatParams[PARAM_Vmin] = 0.0;
	floatParams[PARAM_T2] = 2.5;
	floatParams[PARAM_T3] = 7.5;
	floatParams[PARAM_Dt] = 0.0;
	floatParams[PARAM_Trate] = 0.0;
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

	double fR, fTrate;
	double local_PMech, PMechObjectInitInternal, GenObjectMVABase;
	unsigned char GovResponseLimits;
		
	fR		= floatParams[PARAM_R    ];
	fTrate	= floatParams[PARAM_Trate];
	
	PMechObjectInitInternal	= hardCodedSignals[HARDCODE_GOV_InitPmech]; 
	GenObjectMVABase		= hardCodedSignals[HARDCODE_GOV_GenMVABase];
	GovResponseLimits		= (unsigned char) hardCodedSignals[HARDCODE_GOV_GovResponseLimits];

	if (fTrate == 0) {
		fTrate = GenObjectMVABase;
	}

	local_PMech	= PMechObjectInitInternal;
	local_PMech = local_PMech*GenObjectMVABase/fTrate;

	// Handle the Governor Response Limits Options
    // A value of 1 means it's "Down Only", 2 means it's "Fixed"
    // This automatically change the Vmax and Vmin parameters as necessary.
	switch (GovResponseLimits)
	{
	case 1:
		floatParams[PARAM_Vmax] = local_PMech;
		if (floatParams[PARAM_Vmin] > local_PMech) {
			floatParams[PARAM_Vmin] = local_PMech;
		}
		break;
	case 2:
		floatParams[PARAM_Vmax] = local_PMech;
		floatParams[PARAM_Vmin] = local_PMech;
		break;
	default:
		break;
	}

	hardCodedSignals[HARDCODE_GOV_Pref] = local_PMech*fR;

	states[STATE_TurbPower] = local_PMech;
	states[STATE_ValvePos ] = local_PMech;
}


extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
	TByteArray& activeLimits = *nonWindUpLimits->activeLimits;

	double fR, fT1, fT2, fT3;
	double fInvR, fInvT1, fInvT3;
	double stateTurbinePower, stateValve;
	double dotStateTurbinePower, dotStateValve;
	double Pref_In, deltaWPU_In;
	unsigned char fActiveLimitHLState2;

	stateTurbinePower = states[STATE_TurbPower];
	stateValve        = states[STATE_ValvePos];

	fR  = floatParams[PARAM_R ];
	fT1 = floatParams[PARAM_T1];
	fT2 = floatParams[PARAM_T2];
	fT3 = floatParams[PARAM_T3];

	Pref_In     = hardCodedSignals[HARDCODE_GOV_Pref               ];
	deltaWPU_In = hardCodedSignals[HARDCODE_GOV_GenSpeedDeviationPU];

	if (fR  == 0) {
		fInvR  = 0;
	}
	else {
		fInvR  = 1/fR;
	}
	if (fT1 == 0) {
		fInvT1 = 0;
	}
	else {
		fInvT1 = 1/fT1;
	}
	if (fT3 == 0) {
		fInvT3 = 0;
	}
	else {
		fInvT3 = 1/fT3;
	}

	fActiveLimitHLState2 = activeLimits[NONWINDUPINDEX_ValvePos];

	dotStateValve = fInvT1*((Pref_In - deltaWPU_In)*fInvR - stateValve);

	if (fActiveLimitHLState2 > 0) {
		if ((fActiveLimitHLState2 = 1) & (dotStateValve > 0)) {
			dotStateValve = 0;
		}
		else if ((fActiveLimitHLState2 = 2) & (dotStateValve < 0)) {
			dotStateValve = 0;
		}
	}

	dotStateTurbinePower = fInvT3*(stateValve + fT2*dotStateValve - stateTurbinePower);

	(*dotX)[STATE_TurbPower] = dotStateTurbinePower;
	(*dotX)[STATE_ValvePos ] = dotStateValve;
}


extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{

}


extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TDoubleArray& minLimits = *nonWindUpLimits->minLimits;
	TDoubleArray& maxLimits = *nonWindUpLimits->maxLimits;
	TIntegerArray& limitStates = *nonWindUpLimits->limitStates;


	limitStates[NONWINDUPINDEX_ValvePos] = STATE_ValvePos;

	minLimits[NONWINDUPINDEX_ValvePos] = floatParams[PARAM_Vmin];

	maxLimits[NONWINDUPINDEX_ValvePos] = floatParams[PARAM_Vmax];

	return(N_NONWINDUPLIMITS);
}


extern "C" __declspec(dllexport) double __stdcall GovernorPmechOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	double fDt, fTrate;
    double stateTurbinePower;
    double GenObjectMVABase, deltaWPU_In;

	stateTurbinePower = states[STATE_TurbPower];

	fDt		= floatParams[PARAM_Dt   ];
	fTrate	= floatParams[PARAM_Trate];

	GenObjectMVABase = hardCodedSignals[HARDCODE_GOV_GenMVABase         ];
	deltaWPU_In      = hardCodedSignals[HARDCODE_GOV_GenSpeedDeviationPU];

	if (fTrate == 0) {
		fTrate = GenObjectMVABase;
	}

	return((stateTurbinePower - fDt*deltaWPU_In)/GenObjectMVABase*fTrate);
}

}
