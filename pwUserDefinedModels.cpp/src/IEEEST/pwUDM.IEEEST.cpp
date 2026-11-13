module pwUDM.IEEEST;

import std;
import pwUDM;
import pwUDM.Stabilizer;

namespace pwUDM::IEEEST {

extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
	return(1);
}


extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{	
    ModelClass udmClass;
	const wchar_t* str;
	int stringLength;
	
    udmClass = ModelClass::Stabilizer;

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

	if (*ParamNum == PARAM_Ics) {
		str = L"Ics";
	}
	else if (*ParamNum == PARAM_A1+N_I_PARAMS) {
		str = L"A1";
	}
	else if (*ParamNum == PARAM_A2+N_I_PARAMS) {
		str = L"A2";
	}
	else if (*ParamNum == PARAM_A3+N_I_PARAMS) {
		str = L"A3";
	}
	else if (*ParamNum == PARAM_A4+N_I_PARAMS) {
		str = L"A4";
	}
	else if (*ParamNum == PARAM_A5+N_I_PARAMS) {
		str = L"A5";
	}
	else if (*ParamNum == PARAM_A6+N_I_PARAMS) {
		str = L"A6";
	}
	else if (*ParamNum == PARAM_T1+N_I_PARAMS) {
		str = L"T1";
	}
	else if (*ParamNum == PARAM_T2+N_I_PARAMS) {
		str = L"T2";
	}
	else if (*ParamNum == PARAM_T3+N_I_PARAMS) {
		str = L"T3";
	}
	else if (*ParamNum == PARAM_T4+N_I_PARAMS) {
		str = L"T4";
	}
	else if (*ParamNum == PARAM_T5+N_I_PARAMS) {
		str = L"T5";
	}
	else if (*ParamNum == PARAM_T6+N_I_PARAMS) {
		str = L"T6";
	}
	else if (*ParamNum == PARAM_Ks+N_I_PARAMS) {
		str = L"Ks";
	}
	else if (*ParamNum == PARAM_Lsmax+N_I_PARAMS) {
		str = L"Lsmax";
	}
	else if (*ParamNum == PARAM_Lsmin+N_I_PARAMS) {
		str = L"Lsmin";
	}
	else if (*ParamNum == PARAM_Vcu+N_I_PARAMS) {
		str = L"Vcu";
	}
	else if (*ParamNum == PARAM_Vcl+N_I_PARAMS) {
		str = L"Vcl";
	}
	else if (*ParamNum == PARAM_Tdelay+N_I_PARAMS) {
		str = L"Tdelay";
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
	
	if (*StateNum == STATE_Filter1) {
		str = L"Filter 1";
	}
	else if (*StateNum == STATE_Filter2) {
		str = L"Filter 2";
	}
	else if (*StateNum == STATE_Filter3) {
		str = L"Filter 3";
	}
	else if (*StateNum == STATE_FilterOut) {
		str = L"Filter Out";
	}
	else if (*StateNum == STATE_LL1) {
		str = L"LL1";
	}
	else if (*StateNum == STATE_LL2) {
		str = L"LL2";
	}
	else if (*StateNum == STATE_UnlimitedSignal) {
		str = L"Unlimited Signal";
	}
    else {
        str = L"";
    }

    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


extern "C" __declspec(dllexport) int __stdcall OtherObjectClass(int* Num, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* str;
	int stringLength;

	if (*Num == 0) {
		str = L"Bus";
	}
    else {
        str = L"";
    }

    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


extern "C" __declspec(dllexport) int __stdcall OtherObjectDescription(int* Num, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* str;
	int stringLength;

	if (*Num == 0) {
		str = L"Signal Bus";
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
	TIntegerArray& intParams = *ParamsAndStates->IntParams;

	intParams[PARAM_Ics] = 1;

	floatParams[PARAM_A1    ] =  1.013;
	floatParams[PARAM_A2    ] =  0.013;
	floatParams[PARAM_A3    ] =  0.0;
	floatParams[PARAM_A4    ] =  0.0;
	floatParams[PARAM_A5    ] =  1.013;
	floatParams[PARAM_A6    ] =  0.113;
	floatParams[PARAM_T1    ] =  0.0;
	floatParams[PARAM_T2    ] =  0.02;
	floatParams[PARAM_T3    ] =  0.0;
	floatParams[PARAM_T4    ] =  0.0;
	floatParams[PARAM_T5    ] =  1.65;
	floatParams[PARAM_T6    ] =  1.65;
	floatParams[PARAM_Ks    ] =  3.00;
	floatParams[PARAM_Lsmax ] =  0.1;
	floatParams[PARAM_Lsmin ] = -0.1;
	floatParams[PARAM_Vcu   ] =  0.0;
	floatParams[PARAM_Vcl   ] =  0.0;
	floatParams[PARAM_Tdelay] =  0.0;
}


extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates,	double* timeStepSeconds)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;

	double fA1, fA2, fA3, fA4, timeStep;

	fA1 = floatParams[PARAM_A1];
	fA2 = floatParams[PARAM_A2];
	fA3 = floatParams[PARAM_A3];
	fA4 = floatParams[PARAM_A4];
	timeStep = *timeStepSeconds;

	if (((fA1 != 0) && (fA1 < 2*timeStep) && (std::abs(fA2) < 2*timeStep)) || ((fA3 != 0) && (fA3 < 2*timeStep) && (std::abs(fA4) < 2*timeStep))) {
		return(6);
	}
	else if (((fA3 != 0) && (fA2 < 2*timeStep) && (std::abs(fA2) < 4*timeStep)) || ((fA3 != 0) && (fA3 < 2*timeStep) && (std::abs(fA4) < 4*timeStep))) {
		return(4);
	}
	else {
		return(0);
	}
}


extern "C" __declspec(dllexport) void __stdcall initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TIntegerArray& intParams = *ParamsAndStates->IntParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
	TBooleanArray& ignoreStates = *ParamsAndStates->IgnoreStates;

	double local_Signal;
	double coef_a0, coef_a1, coef_a2, coef_a3, coef_a4;
	int fIcs;
	double fA1, fA2, fA3, fA4, fT2, fT4;

	fIcs = intParams[PARAM_Ics];

	fA1 = floatParams[PARAM_A1];
	fA2 = floatParams[PARAM_A2];
	fA3 = floatParams[PARAM_A3];
	fA4 = floatParams[PARAM_A4];
	fT2 = floatParams[PARAM_T2];
	fT4 = floatParams[PARAM_T4];

	if ((fIcs > 5) | (fIcs < 0)) {
		local_Signal = 0;
	}
	else {
		local_Signal = hardCodedSignals[fIcs];
	}

	states[STATE_Filter1        ] = local_Signal;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
	states[STATE_Filter2        ] = local_Signal;
	states[STATE_Filter3        ] = local_Signal;
	states[STATE_FilterOut      ] = local_Signal;
	states[STATE_LL1            ] = local_Signal;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
	states[STATE_LL2            ] = local_Signal;
	states[STATE_UnlimitedSignal] = 0;

	coef_a0 = 1;
	coef_a1 = fA1 + fA3;
	coef_a2 = fA2 + fA1*fA3 + fA4;
	coef_a3 = fA3*fA2 + fA1*fA4;
	coef_a4 = fA2*fA4;

	if ((coef_a4 == 0) & (coef_a3 != 0)) {
		ignoreStates[STATE_FilterOut] = 1;
	}
	else if ((coef_a4 == 0) & (coef_a3 == 0) & (coef_a2 != 0)) {
		ignoreStates[STATE_FilterOut] = 1;
		ignoreStates[STATE_Filter3  ] = 1;
	}
	else if ((coef_a4 == 0) & (coef_a3 == 0) & (coef_a2 == 0) & (coef_a1 != 0)) {
		ignoreStates[STATE_FilterOut] = 1;
		ignoreStates[STATE_Filter3  ] = 1;
		ignoreStates[STATE_Filter2  ] = 1;
	}
	else if ((coef_a4 == 0) & (coef_a3 == 0) & (coef_a2 == 0) & (coef_a1 == 0) & (coef_a0 != 0)) {
		ignoreStates[STATE_FilterOut] = 1;
		ignoreStates[STATE_Filter3  ] = 1;
		ignoreStates[STATE_Filter2  ] = 1;
		ignoreStates[STATE_Filter1  ] = 1;
	}

	if (fT2 == 0) {
		ignoreStates[STATE_LL1      ] = 1;
	}
	if (fT4 == 0) {
		ignoreStates[STATE_LL2      ] = 1;
	}
}


extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TIntegerArray& intParams = *ParamsAndStates->IntParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	int fIcs;
	double fA1, fA2, fA3, fA4, fA5, fA6, fT1, fT2, fT3, fT4, fT5, fT6, fKs;
	double fInvT2, fInvT4, fInvT6;
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

	stateFilter[0]       = states[STATE_Filter1        ];
	stateFilter[1]       = states[STATE_Filter2        ];
	stateFilter[2]       = states[STATE_Filter3        ];
	stateFilter[3]       = states[STATE_FilterOut      ];
	stateLL1             = states[STATE_LL1            ];
	stateLL2             = states[STATE_LL2            ];
	stateUnlimitedSignal = states[STATE_UnlimitedSignal];

	fIcs	= intParams[PARAM_Ics];

	fA1     = floatParams[PARAM_A1    ];
	fA2     = floatParams[PARAM_A2    ];
	fA3     = floatParams[PARAM_A3    ];
	fA4     = floatParams[PARAM_A4    ];
	fA5     = floatParams[PARAM_A5    ];
	fA6     = floatParams[PARAM_A6    ];
	fT1     = floatParams[PARAM_T1    ];
	fT2     = floatParams[PARAM_T2    ];
	fT3     = floatParams[PARAM_T3    ];
	fT4     = floatParams[PARAM_T4    ];
	fT5     = floatParams[PARAM_T5    ];
	fT6     = floatParams[PARAM_T6    ];
	fKs     = floatParams[PARAM_Ks    ];

    dotStateFilter[0] = 0;
    dotStateFilter[1] = 0;
    dotStateFilter[2] = 0;
    dotStateFilter[3] = 0;

	if ((fIcs > 5) | (fIcs < 0)) {
		local_Signal = 0;
	}
	else {
		local_Signal = hardCodedSignals[fIcs];
	}

	coefB[0] = 1;
	coefB[1] = fA5;
	coefB[2] = fA6;
	coefB[3] = 0;
	coefB[4] = 0;

	coefA[0] = 1;
	coefA[1] = fA1+fA3;
	coefA[2] = fA2 + fA1*fA3 + fA4;
	coefA[3] = fA3*fA2 + fA1*fA4;
	coefA[4] = fA2*fA4;

	DivideBy = 1;
	MaxOrder = -1;

	for (i = 4; i >= 0; i = i - 1) {
		if (coefA[i] != 0) {
			DivideBy = coefA[i];
			MaxOrder = i;
			break;
		}
	}

	for (i = 0; i <= 4; i = i + 1) {
		coefA[i] = coefA[i]/DivideBy;
		coefB[i] = coefB[i]/DivideBy;
	}

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
			dotStateFilter[i] = temp - coefA[i]*tempOUT + coefB[i]*tempIN;
		}
		for (i = MaxOrder; i <= 3; i = i + 1) {
			dotStateFilter[i] = dotStateFilter[i-1];
		}
	}
    else
    {
        
    }

	if (fT2 == 0) {
		dotStateLL1 = dotStateFilter[3];
	}
	else {
		fInvT2 = 1/fT2;
		dotStateLL1 = fInvT2*(-stateLL1 + stateFilter[3] + fT1*dotStateFilter[3]);
	}

	if (fT4 == 0) {
		dotStateLL2 = dotStateLL1;
	}
	else {
		fInvT4 = 1/fT4;
		dotStateLL2 = fInvT4*(-stateLL2 + stateLL1 + fT3*dotStateLL1);
	}

	if (fT6 == 0) {
		dotStateUnlimitedSignal = dotStateLL2;
	}
	else {
		fInvT6 = 1/fT6;
		dotStateUnlimitedSignal = fInvT6*(-stateUnlimitedSignal + fKs*fT5*dotStateLL2);
	}

	(*dotX)[STATE_Filter1        ] = dotStateFilter[0];
	(*dotX)[STATE_Filter2        ] = dotStateFilter[1];
	(*dotX)[STATE_Filter3        ] = dotStateFilter[2];
	(*dotX)[STATE_FilterOut      ] = dotStateFilter[3];
	(*dotX)[STATE_LL1            ] = dotStateLL1;
	(*dotX)[STATE_LL2            ] = dotStateLL2;
	(*dotX)[STATE_UnlimitedSignal] = dotStateUnlimitedSignal;
}


extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TIntegerArray& intParams = *ParamsAndStates->IntParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
	TBooleanArray& ignoreStates = *ParamsAndStates->IgnoreStates;

	int fIcs;
	double local_Signal;

	fIcs = intParams[PARAM_Ics];

	if ((fIcs > 5) | (fIcs < 0)) {
		local_Signal = 0;
	}
	else {
		local_Signal = hardCodedSignals[fIcs];
	}

	if (ignoreStates[STATE_Filter1  ]) {
		states[STATE_Filter1  ] = local_Signal;
	}
	if (ignoreStates[STATE_Filter2  ]) {
		states[STATE_Filter2  ] = states[STATE_Filter1  ] ;
	}
	if (ignoreStates[STATE_Filter3  ]) {
		states[STATE_Filter3  ] = states[STATE_Filter2  ];
	}
	if (ignoreStates[STATE_FilterOut]) {
		states[STATE_FilterOut] = states[STATE_Filter3  ];
	}
	if (ignoreStates[STATE_LL1      ]) {
		states[STATE_LL1      ] = states[STATE_FilterOut];
	}
	if (ignoreStates[STATE_LL2      ]) {
		states[STATE_LL2      ] = states[STATE_LL1      ];
	}
}


extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
	return(N_NONWINDUPLIMITS);
}


// At least one of StabilizerVsOut and StabilizerPitchOut is required (declared in pwUDM.Stabilizer).
// This model implements StabilizerVsOut only.
extern "C" __declspec(dllexport) double __stdcall StabilizerVsOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	double fLsmax, fLsmin, fVcu, fVcl;
    double stateUnlimitedSignal;
    double GenVcomp;
	double VsOut;

	stateUnlimitedSignal = states[STATE_UnlimitedSignal];
    fLsmax   = floatParams[PARAM_Lsmax];
    fLsmin   = floatParams[PARAM_Lsmin];

    GenVcomp = hardCodedSignals[HARDCODE_STAB_GenVcomp];
    fVcu     = floatParams[PARAM_Vcu];
    fVcl     = floatParams[PARAM_Vcl];

    VsOut = stateUnlimitedSignal;
    if (SystemOptions->IgnoreLimitChecking == 0) {
		if (VsOut < fLsmin) {
			VsOut = fLsmin;
		}
		else if (VsOut > fLsmax) {
			VsOut = fLsmax;
		}
	}

	if ((fVcu != 0) && (fVcu < 999) && (GenVcomp > fVcu)) {
		VsOut = 0;
	}
	else if ((fVcl != 0) && (fVcl > -999) && (GenVcomp < fVcl)) {
		VsOut = 0;
	}
	return(VsOut);
}

}
