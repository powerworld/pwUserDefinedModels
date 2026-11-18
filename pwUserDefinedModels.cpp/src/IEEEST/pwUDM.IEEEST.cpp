module pwUDM.IEEEST;

import std;
import pwUDM.Core;
import pwUDM.Stabilizer;

namespace pwUDM::IEEEST {

//******************************************************************************
extern "C" __declspec(dllexport) int DLLVersion()
{
	return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int modelClassName(int* StrSize, char16_t* StrBuf, int dummy)
{	
    int result = pwProvideString(StrBuf, StrSize, pwModelClassString(ModelClass::Stabilizer));
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

	// Integer parameters
	switch (*ParamNum)
	{
		case PARAM_Ics:    ts = u"Ics"; break;
		default:           ts = u""; break;
	}

	// Double parameters
	if (std::char_traits<char16_t>::length(ts) == 0)
	{
		switch (*ParamNum - N_I_PARAMS)
		{
			case PARAM_A1:     ts = u"A1"; break;
			case PARAM_A2:     ts = u"A2"; break;
			case PARAM_A3:     ts = u"A3"; break;
			case PARAM_A4:     ts = u"A4"; break;
			case PARAM_A5:     ts = u"A5"; break;
			case PARAM_A6:     ts = u"A6"; break;
			case PARAM_T1:     ts = u"T1"; break;
			case PARAM_T2:     ts = u"T2"; break;
			case PARAM_T3:     ts = u"T3"; break;
			case PARAM_T4:     ts = u"T4"; break;
			case PARAM_T5:     ts = u"T5"; break;
			case PARAM_T6:     ts = u"T6"; break;
			case PARAM_Ks:     ts = u"Ks"; break;
			case PARAM_Lsmax:  ts = u"Lsmax"; break;
			case PARAM_Lsmin:  ts = u"Lsmin"; break;
			case PARAM_Vcu:    ts = u"Vcu"; break;
			case PARAM_Vcl:    ts = u"Vcl"; break;
			case PARAM_Tdelay: ts = u"Tdelay"; break;
			default:             ts = u""; break;
		}
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
		case STATE_Filter1:         ts = u"Filter 1"; break;
		case STATE_Filter2:         ts = u"Filter 2"; break;
		case STATE_Filter3:         ts = u"Filter 3"; break;
		case STATE_FilterOut:       ts = u"Filter Out"; break;
		case STATE_LL1:             ts = u"LL1"; break;
		case STATE_LL2:             ts = u"LL2"; break;
		case STATE_UnlimitedSignal: ts = u"Unlimited Signal"; break;
		default:                    ts = u""; break;
	}

	int result = pwProvideString(StrBuf, StrSize, ts);
	return result;
}

//******************************************************************************
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

//******************************************************************************
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

//******************************************************************************
extern "C" __declspec(dllexport) void getDefaultParameterValue(TxMyModelData* ParamsAndStates)
{
	DoubleArray* pDoubArr;
	IntegerArray* pIntegerArr;

	// Integers
	pIntegerArr = ParamsAndStates->IntParams;
	(*pIntegerArr)[PARAM_Ics] = 1;

	// Doubles
	pDoubArr = ParamsAndStates->FloatParams;
	(*pDoubArr)[PARAM_A1    ] =  1.013;
	(*pDoubArr)[PARAM_A2    ] =  0.013;
	(*pDoubArr)[PARAM_A3    ] =  0.0;
	(*pDoubArr)[PARAM_A4    ] =  0.0;
	(*pDoubArr)[PARAM_A5    ] =  1.013;
	(*pDoubArr)[PARAM_A6    ] =  0.113;
	(*pDoubArr)[PARAM_T1    ] =  0.0;
	(*pDoubArr)[PARAM_T2    ] =  0.02;
	(*pDoubArr)[PARAM_T3    ] =  0.0;
	(*pDoubArr)[PARAM_T4    ] =  0.0;
	(*pDoubArr)[PARAM_T5    ] =  1.65;
	(*pDoubArr)[PARAM_T6    ] =  1.65;
	(*pDoubArr)[PARAM_Ks    ] =  3.00;
	(*pDoubArr)[PARAM_Lsmax ] =  0.1;
	(*pDoubArr)[PARAM_Lsmin ] = -0.1;
	(*pDoubArr)[PARAM_Vcu   ] =  0.0;
	(*pDoubArr)[PARAM_Vcl   ] =  0.0;
	(*pDoubArr)[PARAM_Tdelay] =  0.0;
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
extern "C" __declspec(dllexport) int SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates,	double* TimeStepSeconds)
{
	double fA1, fA2, fA3, fA4, timeStep;
	DoubleArray* pFloatParamArr;

	// Return an integer value between 0 and 7
	// This function returns the EXPONENT of 2 for the number subintervals.
	// Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
	// The maximum number of subintervals allowed is 128, so (2^7) = 128
	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pFloatParamArr = ParamsAndStates->FloatParams;
	fA1 = (*pFloatParamArr)[PARAM_A1];
	fA2 = (*pFloatParamArr)[PARAM_A2];
	fA3 = (*pFloatParamArr)[PARAM_A3];
	fA4 = (*pFloatParamArr)[PARAM_A4];
	timeStep = *TimeStepSeconds;

	if (((fA1 != 0) && (fA1 < 2*timeStep) && (std::abs(fA2) < 2*timeStep)) || ((fA3 != 0) && (fA3 < 2*timeStep) && (std::abs(fA4) < 2*timeStep))) {
		return(6);
	}
	else if (((fA1 != 0) && (fA1 < 4*timeStep) && (std::abs(fA2) < 4*timeStep)) || ((fA3 != 0) && (fA3 < 4*timeStep) && (std::abs(fA4) < 4*timeStep))) {
		return(4);
	}
	else {
		return(0);
	}
}

//******************************************************************************
extern "C" __declspec(dllexport) void initializeYourself(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
	double local_Signal;
	double coef_a0, coef_a1, coef_a2, coef_a3, coef_a4;
	int fIcs;
	double fA1, fA2, fA3, fA4, fT2, fT4;

	IntegerArray* pIntParamArr;
	DoubleArray* pFloatParamArr;
	DoubleArray* pHardCodedArr;
	DoubleArray* pStatesArr;
	BooleanArray* pIgnoreStatesArr;

	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pIntParamArr = ParamsAndStates->IntParams;
	pFloatParamArr = ParamsAndStates->FloatParams;
	pStatesArr = ParamsAndStates->States;
	pHardCodedArr = ParamsAndStates->HardCodedSignals;
	pIgnoreStatesArr = ParamsAndStates->IgnoreStates;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	fIcs = (*pIntParamArr)[PARAM_Ics];

	fA1 = (*pFloatParamArr)[PARAM_A1];
	fA2 = (*pFloatParamArr)[PARAM_A2];
	fA3 = (*pFloatParamArr)[PARAM_A3];
	fA4 = (*pFloatParamArr)[PARAM_A4];
	fT2 = (*pFloatParamArr)[PARAM_T2];
	fT4 = (*pFloatParamArr)[PARAM_T4];

	if ((fIcs > 5) | (fIcs < 0)) {  // error check for bad input
		local_Signal = 0;
	}
	else {
		local_Signal = (*pHardCodedArr)[fIcs];
	}

	// IEEEST example
	// Write out initial state vector
	(*pStatesArr)[STATE_Filter1        ] = local_Signal;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
	(*pStatesArr)[STATE_Filter2        ] = local_Signal;
	(*pStatesArr)[STATE_Filter3        ] = local_Signal;
	(*pStatesArr)[STATE_FilterOut      ] = local_Signal;
	(*pStatesArr)[STATE_LL1            ] = local_Signal;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
	(*pStatesArr)[STATE_LL2            ] = local_Signal;
	(*pStatesArr)[STATE_UnlimitedSignal] = 0;

	//--------------------------------------------------------------------
	// Determine which States are ignored
	//--------------------------------------------------------------------
	coef_a0 = 1;
	coef_a1 = fA1 + fA3;
	coef_a2 = fA2 + fA1*fA3 + fA4;
	coef_a3 = fA3*fA2 + fA1*fA4;
	coef_a4 = fA2*fA4;

	if ((coef_a4 == 0) & (coef_a3 != 0)) {
		(*pIgnoreStatesArr)[STATE_FilterOut] = 1;
	}
	else if ((coef_a4 == 0) & (coef_a3 == 0) & (coef_a2 != 0)) {
		(*pIgnoreStatesArr)[STATE_FilterOut] = 1;
		(*pIgnoreStatesArr)[STATE_Filter3  ] = 1;
	}
	else if ((coef_a4 == 0) & (coef_a3 == 0) & (coef_a2 == 0) & (coef_a1 != 0)) {
		(*pIgnoreStatesArr)[STATE_FilterOut] = 1;
		(*pIgnoreStatesArr)[STATE_Filter3  ] = 1;
		(*pIgnoreStatesArr)[STATE_Filter2  ] = 1;
	}
	else if ((coef_a4 == 0) & (coef_a3 == 0) & (coef_a2 == 0) & (coef_a1 == 0) & (coef_a0 != 0)) {
		(*pIgnoreStatesArr)[STATE_FilterOut] = 1;
		(*pIgnoreStatesArr)[STATE_Filter3  ] = 1;
		(*pIgnoreStatesArr)[STATE_Filter2  ] = 1;
		(*pIgnoreStatesArr)[STATE_Filter1  ] = 1;
	}

	if (fT2 == 0) {
		(*pIgnoreStatesArr)[STATE_LL1      ] = 1;
	}
	if (fT4 == 0) {
		(*pIgnoreStatesArr)[STATE_LL2      ] = 1;
	}
}

//******************************************************************************
extern "C" __declspec(dllexport) void calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{

	// parameters
	int fIcs;
	double fA1, fA2, fA3, fA4, fA5, fA6, fT1, fT2, fT3, fT4, fT5, fT6, fKs;
	double fInvT2, fInvT4, fInvT6;
	// The state variables,
	int i;
	double stateFilter[4];
	double dotStateFilter[4];
	double coefA[5];
	double coefB[5];
	double DivideBy;
	int MaxOrder;
	double tempIN;
	double tempOUT;
	double temp;

	double stateLL1, stateLL2, stateUnlimitedSignal;
	double dotStateLL1, dotStateLL2, dotStateUnlimitedSignal;
	double local_Signal;

	IntegerArray* pIntParamArr;
	DoubleArray* pFloatParamArr;
	DoubleArray* pStatesArr;
	DoubleArray* pHardCodedArr;
	DoubleArray* pDotXArr;

	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pIntParamArr = ParamsAndStates->IntParams;
	pFloatParamArr = ParamsAndStates->FloatParams;
	pStatesArr = ParamsAndStates->States;
	pHardCodedArr = ParamsAndStates->HardCodedSignals;
	pDotXArr = dotX;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	stateFilter[0]       = (*pStatesArr)[STATE_Filter1        ];
	stateFilter[1]       = (*pStatesArr)[STATE_Filter2        ];
	stateFilter[2]       = (*pStatesArr)[STATE_Filter3        ];
	stateFilter[3]       = (*pStatesArr)[STATE_FilterOut      ];
	stateLL1             = (*pStatesArr)[STATE_LL1            ];
	stateLL2             = (*pStatesArr)[STATE_LL2            ];
	stateUnlimitedSignal = (*pStatesArr)[STATE_UnlimitedSignal];

	fIcs	= (*pIntParamArr)[PARAM_Ics];

	fA1     = (*pFloatParamArr)[PARAM_A1    ];
	fA2     = (*pFloatParamArr)[PARAM_A2    ];
	fA3     = (*pFloatParamArr)[PARAM_A3    ];
	fA4     = (*pFloatParamArr)[PARAM_A4    ];
	fA5     = (*pFloatParamArr)[PARAM_A5    ];
	fA6     = (*pFloatParamArr)[PARAM_A6    ];
	fT1     = (*pFloatParamArr)[PARAM_T1    ];
	fT2     = (*pFloatParamArr)[PARAM_T2    ];
	fT3     = (*pFloatParamArr)[PARAM_T3    ];
	fT4     = (*pFloatParamArr)[PARAM_T4    ];
	fT5     = (*pFloatParamArr)[PARAM_T5    ];
	fT6     = (*pFloatParamArr)[PARAM_T6    ];
	fKs     = (*pFloatParamArr)[PARAM_Ks    ];

    dotStateFilter[0] = 0;
    dotStateFilter[1] = 0;
    dotStateFilter[2] = 0;
    dotStateFilter[3] = 0;

	// kate 1/5/12 Reading in parameters we need for the network:  inNetworkParams
	if ((fIcs > 5) | (fIcs < 0)) {  // error check for bad input
		local_Signal = 0;
	}
	else {
		local_Signal = (*pHardCodedArr)[fIcs];
	}

	//----------------------------------------------------------------------------
	// General Filter Block
	//----------------------------------------------------------------------------
	// seems that this is the way the big filter block is handled in Simulator
	// b's = numerator, a's = denominator

	// | b0 + b1*s + b2*s^2 + b3*s^3 + b4*s^4 |
	// INPUT --> |--------------------------------------| --> OUTPUT
	// | a0 + a1*s + a2*s^2 + a3*s^3 + 1*s^4  |

	// Writing these in "observable cononical form" (look in a control system theory book)

	// INPUT = localsignal
	// OUTPUT = x3; (last state is output)
	// dotx0 =    - a0*OUTPUT + (b0 - a0*b4)*INPUT
	// dotx1 = x0 - a1*OUTPUT + (b1 - a1*b4)*INPUT
	// dotx2 = x1 - a2*OUTPUT + (b2 - a2*b4)*INPUT
	// dotx3 = x2 - a3*OUTPUT + (b3 - a3*b4)*INPUT
	// Written generically for entry "k" assuming highest order is "N"
	// dotx(k) = X(k-1) - a(k)*OUTPUT + [b(k) - a(k)*b(N)]*INPUT
	//----------------------------------------------------------------------------
	// For our example, Numerator is   ( 1 + A5*s + A6*s^2 )
	coefB[0] = 1;
	coefB[1] = fA5;
	coefB[2] = fA6;
	coefB[3] = 0;
	coefB[4] = 0;

	// Denominator is ( 1 + A1s + A2s^2) * ( 1 + A3s + A4s^2) =
	// This gives   =   1 + (A1+A3)s + (A2 + A1*A3 + A5)s^2 + (A3*A2 + A1*A4)s^3 + (A2+A4)s^4
	coefA[0] = 1;
	coefA[1] = fA1+fA3;
	coefA[2] = fA2 + fA1*fA3 + fA4;
	coefA[3] = fA3*fA2 + fA1*fA4;
	coefA[4] = fA2*fA4;

	// Must also convert coefficients so that the "a4 = 1", so write as follows
	// Must also convert so coefficent is "1" on the highest order term in denominator
	DivideBy = 1;
	MaxOrder = -1;

	for (i = 4; i >= 0; i = i - 1) {
		if (coefA[i] != 0) {
			DivideBy = coefA[i];
			MaxOrder = i;
			break; // jump OUT of the for i loop
		}
	}

	// Divide all coefficients so that the highest order non-zero A termi is 1.
	for (i = 0; i <= 4; i = i + 1) {
		coefA[i] = coefA[i]/DivideBy;
		coefB[i] = coefB[i]/DivideBy;
	}

	// INPUT = localsignal
	// OUTPUT = x3; (last state is output)
	// dotx0 =    - a0*OUTPUT + (b0 - a0*b4)*INPUT
	// dotx1 = x0 - a1*OUTPUT + (b1 - a1*b4)*INPUT
	// dotx2 = x1 - a2*OUTPUT + (b2 - a2*b4)*INPUT

	if (MaxOrder > 0) {
		tempOUT = stateFilter[MaxOrder-1];
		tempIN = local_Signal;
		for (i = 0; i <= MaxOrder-1; i = i + 1) {
			if (i > 0) {
				temp = stateFilter[i-1];
			}
			else {
				temp = 0;
			}
			// we can ignore the terms which multiply by b4 because it's ALWAYS zero in this example
			// dotx(k) = X(k-1) - a(k)*OUTPUT + [b(k) - a(k)*b(N)]*INPUT
			dotStateFilter[i] = temp - coefA[i]*tempOUT + coefB[i]*tempIN;
		}
		// Make all ignored states have the same derivative
		// needed because I'm treating the OUTPUT variable as the state of this polyintegration object!
		for (i = MaxOrder; i <= 3; i = i + 1) {
			dotStateFilter[i] = dotStateFilter[i-1];
		}
	}
    else
    {
        
    }

	//----------------------------------------------------------------------------
	// Lead-lag blocks
	//----------------------------------------------------------------------------
	// | b0+b1s |                 | b1     b0-b1/a1 |
	// u --> |------- | --> y           |---- + ----------|
	// | a0+a1s |                 | a1      1+a1*s  |

	// dotOUT = 1/a1*(-OUT*a0 + b0*IN + b1*dotIN);
	// Treat the STATE as the OUTPUT of the lead/lag block

	// Block for State 5 --
	// a0 = 1
	// b0 = 1
	// a1 = T2
	// b1 = T1

	// For State 6 --
	// a0 = 1
	// b0 = 1
	// a1 = T4
	// b1 = T3

	// For State 7 --
	// a0 = 0
	// b0 = 0
	// a1 = T6
	// b1 = Ks*T5
	//----------------------------------------------------------------------------
	if (fT2 == 0) {  // treat as though fT1 is also 0.0    // State 5
		dotStateLL1 = dotStateFilter[3];  // ignored state.
	}
	else {
		fInvT2 = 1/fT2;
		dotStateLL1 = fInvT2*(-stateLL1 + stateFilter[3] + fT1*dotStateFilter[3]);  // kate 3/18/13
	}

	if (fT4 == 0) {  // treat as though fT3 is also 0.0     // State 6
		dotStateLL2 = dotStateLL1;  // ignored state.
	}
	else {
		fInvT4 = 1/fT4;
		dotStateLL2 = fInvT4*(-stateLL2 + stateLL1 + fT3*dotStateLL1);  // kate 3/18/13
	}

	//----------------------------------------------------------------------------
	// Derivative block is similar math to the lead/lag block
	//----------------------------------------------------------------------------
	if (fT6 == 0) {  // State 7
		dotStateUnlimitedSignal = dotStateLL2;  // ignore state
	}
	else {
		fInvT6 = 1/fT6;
		dotStateUnlimitedSignal = fInvT6*(-stateUnlimitedSignal + fKs*fT5*dotStateLL2);  // kate 3/18/13
	}

	//--------------------------------------------------------------------
	// Push the resulting derivatives to the appropriate memory
	//--------------------------------------------------------------------
	(*pDotXArr)[STATE_Filter1        ] = dotStateFilter[0];
	(*pDotXArr)[STATE_Filter2        ] = dotStateFilter[1];
	(*pDotXArr)[STATE_Filter3        ] = dotStateFilter[2];
	(*pDotXArr)[STATE_FilterOut      ] = dotStateFilter[3];
	(*pDotXArr)[STATE_LL1            ] = dotStateLL1;
	(*pDotXArr)[STATE_LL2            ] = dotStateLL2;
	(*pDotXArr)[STATE_UnlimitedSignal] = dotStateUnlimitedSignal;
}

//******************************************************************************
extern "C" __declspec(dllexport) void PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
	int fIcs;
	double local_Signal;

	IntegerArray* pIntParamArr;
	DoubleArray* pHardCodedArr;
	DoubleArray* pStatesArr;
	BooleanArray* pIgnoreStatesArr;

	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pStatesArr = ParamsAndStates->States;
	pIgnoreStatesArr = ParamsAndStates->IgnoreStates;
	pHardCodedArr = ParamsAndStates->HardCodedSignals;

	pIntParamArr = ParamsAndStates->IntParams;
	fIcs = (*pIntParamArr)[PARAM_Ics];

	if ((fIcs > 5) | (fIcs < 0)) {  // error check for bad input
		local_Signal = 0;
	}
	else {
		local_Signal = (*pHardCodedArr)[fIcs];
	}

	//--------------------------------------------------------------------
	// Propagate Ignored States
	//--------------------------------------------------------------------
	if ((*pIgnoreStatesArr)[STATE_Filter1  ]) {
		(*pStatesArr)[STATE_Filter1  ] = local_Signal;
	}
	if ((*pIgnoreStatesArr)[STATE_Filter2  ]) {
		(*pStatesArr)[STATE_Filter2  ] = (*pStatesArr)[STATE_Filter1  ] ;
	}
	if ((*pIgnoreStatesArr)[STATE_Filter3  ]) {
		(*pStatesArr)[STATE_Filter3  ] = (*pStatesArr)[STATE_Filter2  ];
	}
	if ((*pIgnoreStatesArr)[STATE_FilterOut]) {
		(*pStatesArr)[STATE_FilterOut] = (*pStatesArr)[STATE_Filter3  ];
	}
	if ((*pIgnoreStatesArr)[STATE_LL1      ]) {
		(*pStatesArr)[STATE_LL1      ] = (*pStatesArr)[STATE_FilterOut];
	}
	if ((*pIgnoreStatesArr)[STATE_LL2      ]) {
		(*pStatesArr)[STATE_LL2      ] = (*pStatesArr)[STATE_LL1      ];
	}
}

//******************************************************************************
extern "C" __declspec(dllexport) int getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
	return(0);   // None
}


//******************************************************************************
// Stabilizer-Specific Functions
//******************************************************************************
// At least one of StabilizerVsOut and StabilizerPitchOut is required (declared in pwUDM.Stabilizer).
// This model implements StabilizerVsOut only.
extern "C" __declspec(dllexport) double StabilizerVsOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{

	double fLsmax, fLsmin, fVcu, fVcl;
    double stateUnlimitedSignal;
    double GenVcomp;
	double result;

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
	// -------------------------------+-------------------------------------
	stateUnlimitedSignal = (*pStatesArr)[STATE_UnlimitedSignal];
    fLsmax   = (*pFloatParamArr)[PARAM_Lsmax];
    fLsmin   = (*pFloatParamArr)[PARAM_Lsmin];

    GenVcomp = (*pHardCodedArr)[HARDCODE_STAB_GenVcomp];
    fVcu     = (*pFloatParamArr)[PARAM_Vcu];
    fVcl     = (*pFloatParamArr)[PARAM_Vcl];

    //--------------------------------------------------------------------
    // Perform Calculations
    //--------------------------------------------------------------------
    result = stateUnlimitedSignal;
    if (SystemOptions->IgnoreLimitChecking == 0) {
		if (result < fLsmin) {
			result = fLsmin;
		}
		else if (result > fLsmax) {
			result = fLsmax;
		}
	}

	// Check for output limiter signals.
	if ((fVcu != 0) && (fVcu < 999) && (GenVcomp > fVcu)) {
		result = 0;
	}
	else if ((fVcl != 0) && (fVcl > -999) && (GenVcomp < fVcl)) {
		result = 0;
	}
	return result;
}

/* NOT defined for this object
extern "C" __declspec(dllexport) double StabilizerPitchOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions);
*/

}
