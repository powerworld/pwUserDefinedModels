module pwUDM.TGOV1;

import std;
import pwUDM.Core;
import pwUDM.Governor;

namespace pwUDM::TGOV1 {

//******************************************************************************
extern "C" __declspec(dllexport) int DLLVersion()
{
	return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int modelClassName(int* StrSize, char16_t* StrBuf, int dummy)
{	
    int result = pwProvideString(StrBuf, StrSize, pwModelClassString(ModelClass::Governor));
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) void allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds)
{
	// -------The model-specific parameters-----
	numbersOfEverything->nFloatParams		= N_F_PARAMS;
	numbersOfEverything->nIntParams		= N_I_PARAMS;
	numbersOfEverything->nStrParams		= N_S_PARAMS;
	numbersOfEverything->nStates			= N_STATES;
	numbersOfEverything->nAlgebraics		= N_ALGEBRAICS;	

	// ----For nonwindup limits of the model---
	numbersOfEverything->nNonWindUpLimits	= N_NONWINDUPLIMITS;
}

//******************************************************************************
extern "C" __declspec(dllexport) int parameterName(int* ParamNum, int* StrSize, char16_t* StrBuf, int dummy)
{
	const char16_t* ts;

	switch (*ParamNum)
	{
		case PARAM_R:     ts = u"R"; break;
		case PARAM_T1:    ts = u"T1"; break;
		case PARAM_Vmax:  ts = u"Vmax"; break;
		case PARAM_Vmin:  ts = u"Vmin"; break;
		case PARAM_T2:    ts = u"T2"; break;
		case PARAM_T3:    ts = u"T3"; break;
		case PARAM_Dt:    ts = u"Dt"; break;
		case PARAM_Trate: ts = u"Trate"; break;
		default:          ts = u""; break;
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
		case STATE_TurbPower: ts = u"Turbine Power"; break;
		case STATE_ValvePos:  ts = u"Valve Position"; break;
		default:              ts = u""; break;
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

	// Example TGOV1 parameters
	pDoubArr = ParamsAndStates->FloatParams;
	(*pDoubArr)[PARAM_R] = 0.05;
	(*pDoubArr)[PARAM_T1] = 0.5;
	(*pDoubArr)[PARAM_Vmax] = 1.0;
	(*pDoubArr)[PARAM_Vmin] = 0.0;
	(*pDoubArr)[PARAM_T2] = 2.5;
	(*pDoubArr)[PARAM_T3] = 7.5;
	(*pDoubArr)[PARAM_Dt] = 0.0;
	(*pDoubArr)[PARAM_Trate] = 0.0;
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
	double fR, fTrate;
	double local_PMech, PMechObjectInitInternal, GenObjectMVABase;
	unsigned char GovResponseLimits;

	DoubleArray* pHardCodedArr;
	DoubleArray* pFloatParamArr;
	DoubleArray* pStatesArr;

	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pHardCodedArr = ParamsAndStates->HardCodedSignals;
	pFloatParamArr = ParamsAndStates->FloatParams;
	pStatesArr = ParamsAndStates->States;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	fR		= (*pFloatParamArr)[PARAM_R    ];
	fTrate	= (*pFloatParamArr)[PARAM_Trate];
	
	PMechObjectInitInternal	= (*pHardCodedArr)[HARDCODE_GOV_InitPmech]; 
	GenObjectMVABase		= (*pHardCodedArr)[HARDCODE_GOV_GenMVABase];
	// Same as Delphi Round: halves go to even, and the integer result wraps modulo 256 into the byte
	GovResponseLimits		= std::llrint((*pHardCodedArr)[HARDCODE_GOV_GovResponseLimits]);

	if (fTrate == 0) {
		fTrate = GenObjectMVABase;
	}

	//--------------------------------------------------------------------
	// Back solve to determine the initial states.
	//--------------------------------------------------------------------
	local_PMech	= PMechObjectInitInternal;
	local_PMech = local_PMech*GenObjectMVABase/fTrate;  // jamie 2/9/09

	// Handle the Governor Response Limits Options
    // A value of 1 means it's "Down Only", 2 means it's "Fixed"
    // This automatically change the Vmax and Vmin parameters as necessary.

	switch (GovResponseLimits)
	{
	case 1:  // "Down Only"
		(*pFloatParamArr)[PARAM_Vmax] = local_PMech;
		// Make sure Min is less than Max still also...
		if ((*pFloatParamArr)[PARAM_Vmin] > local_PMech) {
			(*pFloatParamArr)[PARAM_Vmin] = local_PMech;
		}
		break;
	case 2:  // "Fixed"
		(*pFloatParamArr)[PARAM_Vmax] = local_PMech;
		(*pFloatParamArr)[PARAM_Vmin] = local_PMech;
		break;
	default:  // do nothing otherwise
		break;
	}

	// No Ignored States

	// Must Set the initial PRef value
	// The following parameters should actually get SET, not read in!
	(*pHardCodedArr)[HARDCODE_GOV_Pref] = local_PMech*fR;

	// Write out initial state vector
	(*pStatesArr)[STATE_TurbPower] = local_PMech;
	(*pStatesArr)[STATE_ValvePos ] = local_PMech;
}

//******************************************************************************
extern "C" __declspec(dllexport) void calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{
	// Parameters
	double fR, fT1, fT2, fT3;
	double fInvR, fInvT1, fInvT3;

	// State variables
	double stateTurbinePower, stateValve;

	// FofX or xdot,
	double dotStateTurbinePower, dotStateValve;
	double Pref_In, deltaWPU_In;

	unsigned char fActiveLimitHLState2;

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
	stateTurbinePower = (*pStatesArr)[STATE_TurbPower];
	stateValve        = (*pStatesArr)[STATE_ValvePos];

	fR  = (*pFloatParamArr)[PARAM_R ];
	fT1 = (*pFloatParamArr)[PARAM_T1];
	fT2 = (*pFloatParamArr)[PARAM_T2];
	fT3 = (*pFloatParamArr)[PARAM_T3];

	Pref_In     = (*pHardCodedArr)[HARDCODE_GOV_Pref               ];
	deltaWPU_In = (*pHardCodedArr)[HARDCODE_GOV_GenSpeedDeviationPU];

	// Calculate Inverse of appropriate values
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

	//--------------------------------------------------------------------
	// fActiveLimitHL = 0 means not active, 1 means high limit, 2 means low limit
	fActiveLimitHLState2 = (*pByteActiveLimitArr)[NONWINDUPINDEX_ValvePos];  // Only one nonwindup limit, so index is 0

	// dot state 2
	dotStateValve = fInvT1*((Pref_In - deltaWPU_In)*fInvR - stateValve);

	// Note: state 2 has a limit, and when that limit is active, we want dotStateValve to be zero.
	if (fActiveLimitHLState2 > 0) {
		// 1 indicates it's stuck at the HIGH limit
		// 2 indicates it's stuck at the LOW limit
		if ((fActiveLimitHLState2 == 1) && (dotStateValve > 0)) {
			dotStateValve = 0;
		}
		else if ((fActiveLimitHLState2 == 2) && (dotStateValve < 0)) {
			dotStateValve = 0;
		}
	}

	// dot state 1
	dotStateTurbinePower = fInvT3*(stateValve + fT2*dotStateValve - stateTurbinePower);

	//--------------------------------------------------------------------
	// Push the resulting derivatives to the appropriate memory
	//--------------------------------------------------------------------
	(*pDotXArr)[STATE_TurbPower] = dotStateTurbinePower;
	(*pDotXArr)[STATE_ValvePos ] = dotStateValve;
}

//******************************************************************************
extern "C" __declspec(dllexport) void PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    // no ignored states or handling of algebraics here
}

//******************************************************************************
extern "C" __declspec(dllexport) int getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
	DoubleArray* pFloatParamArr;

	pFloatParamArr = ParamsAndStates->FloatParams;
	(*nonWindUpLimits->LimitStates)[NONWINDUPINDEX_ValvePos] = STATE_ValvePos;

	(*nonWindUpLimits->minLimits)[NONWINDUPINDEX_ValvePos] = (*pFloatParamArr)[PARAM_Vmin];

	(*nonWindUpLimits->maxLimits)[NONWINDUPINDEX_ValvePos] = (*pFloatParamArr)[PARAM_Vmax];

	return(1);   // only one state has a non windup limit
}

extern "C" __declspec(dllexport) double GovernorPmechOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
	double fDt, fTrate;
	double stateTurbinePower;
	double GenObjectMVABase, deltaWPU_In;
	DoubleArray* pFloatParamArr;
	DoubleArray* pStatesArr;
	DoubleArray* pHardCodedArr;

	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pFloatParamArr = ParamsAndStates->FloatParams;
	pStatesArr = ParamsAndStates->States;
	pHardCodedArr = ParamsAndStates->HardCodedSignals;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	stateTurbinePower = (*pStatesArr)[STATE_TurbPower];

	fDt		= (*pFloatParamArr)[PARAM_Dt   ];
	fTrate	= (*pFloatParamArr)[PARAM_Trate];

	GenObjectMVABase = (*pHardCodedArr)[HARDCODE_GOV_GenMVABase         ];
	deltaWPU_In      = (*pHardCodedArr)[HARDCODE_GOV_GenSpeedDeviationPU];

	if (fTrate == 0) {
		fTrate = GenObjectMVABase;
	}

	//--------------------------------------------------------------------
	// Calculate Pmech output
	//--------------------------------------------------------------------
	return((stateTurbinePower - fDt*deltaWPU_In)/GenObjectMVABase*fTrate);
}

}
