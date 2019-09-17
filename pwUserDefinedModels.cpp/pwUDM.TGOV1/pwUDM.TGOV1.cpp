#include "..\common\pwUDM.h"
#include "..\common\pwUDM.Governor.h"
#include "..\common\pwUDM.Helper.h"
#include "pwUDM.TGOV1.h"
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
	
    udmClass = mcnGovernor;

    str = pwModelClassString(udmClass);
	stringLength = pwStringCopy(StrBuf, StrSize, str);
	return(stringLength);
}


PW_UDM_DllExport(void) allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds)
{
	(*numbersOfEverything).nFloatParams		= N_F_PARAMS;
	(*numbersOfEverything).nIntParams		= N_I_PARAMS;
	(*numbersOfEverything).nStrParams		= N_S_PARAMS;
	(*numbersOfEverything).nStates			= N_STATES;
	(*numbersOfEverything).nAlgebraics		= N_ALGEBRAICS;	
	(*numbersOfEverything).nNonWindUpLimits	= N_NONWINDUPLIMITS;
}


PW_UDM_DllExport(int) parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	wchar_t* str;
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


PW_UDM_DllExport(int) stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	wchar_t* str;
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


PW_UDM_DllExport(void) getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
	(*((*ParamsAndStates).FloatParams))[PARAM_R] = 0.05;
	(*((*ParamsAndStates).FloatParams))[PARAM_T1] = 0.5;
	(*((*ParamsAndStates).FloatParams))[PARAM_Vmax] = 1.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_Vmin] = 0.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_T2] = 2.5;
	(*((*ParamsAndStates).FloatParams))[PARAM_T3] = 7.5;
	(*((*ParamsAndStates).FloatParams))[PARAM_Dt] = 0.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_Trate] = 0.0;
}


PW_UDM_DllExport(int) SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds)
{
	return(0);
}


PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	double fR, fTrate;
	double local_PMech, PMechObjectInitInternal, GenObjectMVABase;
	unsigned __int8 GovResponseLimits;
		
	fR		= (*((*ParamsAndStates).FloatParams))[PARAM_R    ];
	fTrate	= (*((*ParamsAndStates).FloatParams))[PARAM_Trate];
	
	PMechObjectInitInternal	= (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_InitPmech]; 
	GenObjectMVABase		= (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_GenMVABase];
	GovResponseLimits		= (unsigned __int8) (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_GovResponseLimits];

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
		(*((*ParamsAndStates).FloatParams))[PARAM_Vmax] = local_PMech;
		if ((*((*ParamsAndStates).FloatParams))[PARAM_Vmin] > local_PMech) {
			(*((*ParamsAndStates).FloatParams))[PARAM_Vmin] = local_PMech;
		}
		break;
	case 2:
		(*((*ParamsAndStates).FloatParams))[PARAM_Vmax] = local_PMech;
		(*((*ParamsAndStates).FloatParams))[PARAM_Vmin] = local_PMech;
		break;
	default:
		break;
	}

	(*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_Pref] = local_PMech*fR;

	(*((*ParamsAndStates).States))[STATE_TurbPower] = local_PMech;
	(*((*ParamsAndStates).States))[STATE_ValvePos ] = local_PMech;
}


PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	double fR, fT1, fT2, fT3;
	double fInvR, fInvT1, fInvT3;
	double stateTurbinePower, stateValve;
	double dotStateTurbinePower, dotStateValve;
	double Pref_In, deltaWPU_In;
	unsigned __int8 fActiveLimitHLState2;

	stateTurbinePower = (*((*ParamsAndStates).States))[STATE_TurbPower];
	stateValve        = (*((*ParamsAndStates).States))[STATE_ValvePos];

	fR  = (*((*ParamsAndStates).FloatParams))[PARAM_R ];
	fT1 = (*((*ParamsAndStates).FloatParams))[PARAM_T1];
	fT2 = (*((*ParamsAndStates).FloatParams))[PARAM_T2];
	fT3 = (*((*ParamsAndStates).FloatParams))[PARAM_T3];

	Pref_In     = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_Pref               ];
	deltaWPU_In = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_GenSpeedDeviationPU];

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

	fActiveLimitHLState2 = (*((*nonWindUpLimits).activeLimits))[NONWINDUPINDEX_ValvePos];

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


PW_UDM_DllExport(void) PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{

}


PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{

	(*((*nonWindUpLimits).limitStates))[NONWINDUPINDEX_ValvePos] = STATE_ValvePos;

	(*((*nonWindUpLimits).minLimits))[NONWINDUPINDEX_ValvePos] = (*((*ParamsAndStates).FloatParams))[PARAM_Vmin];

	(*((*nonWindUpLimits).maxLimits))[NONWINDUPINDEX_ValvePos] = (*((*ParamsAndStates).FloatParams))[PARAM_Vmax];

	return(N_NONWINDUPLIMITS);
}


PW_UDM_DllExport(double) GovernorPmechOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	double fDt, fTrate;
    double stateTurbinePower;
    double GenObjectMVABase, deltaWPU_In;

	stateTurbinePower = (*((*ParamsAndStates).States))[STATE_TurbPower];

	fDt		= (*((*ParamsAndStates).FloatParams))[PARAM_Dt   ];
	fTrate	= (*((*ParamsAndStates).FloatParams))[PARAM_Trate];

	GenObjectMVABase = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_GenMVABase         ];
	deltaWPU_In      = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_GOV_GenSpeedDeviationPU];

	if (fTrate == 0) {
		fTrate = GenObjectMVABase;
	}

	return((stateTurbinePower - fDt*deltaWPU_In)/GenObjectMVABase*fTrate);
}