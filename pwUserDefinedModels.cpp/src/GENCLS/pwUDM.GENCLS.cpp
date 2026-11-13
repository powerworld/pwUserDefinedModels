module pwUDM.GENCLS;

import std;
import pwUDM;
import pwUDM.Machine;

namespace pwUDM::GENCLS {

extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
	return(1);
}


extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{	
    ModelClass udmClass;
	const wchar_t* str;
	int stringLength;
	
    udmClass = ModelClass::Machine;

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
	numbersOfEverything->nNonWindUpLimits	= N_NONWINDUPLIMITS;
}


extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* str;
    int stringLength;
	
	if (*ParamNum == PARAM_H) {
		str = L"H";
	}
	else if (*ParamNum == PARAM_D) {
		str = L"D";
	}
	else if (*ParamNum == PARAM_Ra) {
		str = L"Ra";
	}
	else if (*ParamNum == PARAM_Xdp) {
		str = L"Xdp";
	}
	else if (*ParamNum == PARAM_Rcomp) {
		str = L"Rcomp";
	}
	else if (*ParamNum == PARAM_Xcomp) {
		str = L"Xcomp";
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

	if (*StateNum == STATE_Angle) {
		str = L"Angle";
	}
	else if (*StateNum == STATE_Speed) {
		str = L"Speed";
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

	floatParams[PARAM_H] = 3.0;
	floatParams[PARAM_D] = 0.0;
	floatParams[PARAM_Ra] = 0.0;
	floatParams[PARAM_Xdp] = 0.2;
	floatParams[PARAM_Rcomp] = 0.0;
	floatParams[PARAM_Xcomp] = 0.0;
}


extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates,	double* timeStepSeconds)
{
	return(0);
}


extern "C" __declspec(dllexport) void __stdcall initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	double local_Vd,local_Vq;
    double local_PsiD,local_PsiQ;
    double Pmech_In, Efd_In;
    double fRa, fXdp;

    double fStateId,fStateIq;

    double local_Delta;
    double local_Itr, local_Iti;
    double local_Vtr, local_Vti;
    double local_Vintr, local_Vinti;
    double CosDelta, SinDelta;

	fRa   = floatParams[PARAM_Ra ];
	fXdp  = floatParams[PARAM_Xdp];

	local_Vtr = hardCodedSignals[HARDCODE_MACHINE_InitVreal];
	local_Vti = hardCodedSignals[HARDCODE_MACHINE_InitVimag];
	local_Itr = hardCodedSignals[HARDCODE_MACHINE_InitIreal];
	local_Iti = hardCodedSignals[HARDCODE_MACHINE_InitIimag];

	local_Vintr = local_Vtr + local_Itr*fRa - local_Iti*fXdp;
	local_Vinti = local_Vti + local_Iti*fRa + local_Itr*fXdp;
	local_Delta = std::atan2(local_Vinti,local_Vintr);

	CosDelta = std::cos(local_Delta);
	SinDelta = std::sin(local_Delta);
	fStateId = -CosDelta*local_Iti + SinDelta*local_Itr;
	fStateIq =  CosDelta*local_Itr + SinDelta*local_Iti;
	local_Vd = -CosDelta*local_Vti + SinDelta*local_Vtr;
	local_Vq =  CosDelta*local_Vtr + SinDelta*local_Vti;

	local_PsiD =  (local_Vq + fRa*fStateIq);
	local_PsiQ = -(local_Vd + fRa*fStateId);

	Efd_In = local_PsiD + fStateId*fXdp;
	Pmech_In =(local_PsiD * fStateIq - local_PsiQ * fStateId);

	states[STATE_Angle] = local_Delta;
	states[STATE_Speed] = 0;

	hardCodedSignals[HARDCODE_MACHINE_TSGenFieldV] = Efd_In;
	hardCodedSignals[HARDCODE_MACHINE_TSPmech] = Pmech_In;

	hardCodedSignals[HARDCODE_MACHINE_TSstateId] = fStateId;
	hardCodedSignals[HARDCODE_MACHINE_TSstateIq] = fStateIq;
}


extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
	TDoubleArray& states = *ParamsAndStates->States;
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	double H , fInv2H, fD;
    double speedState;
    
    double fStateId, fStateIq;

    double StateEInternalD, StateEInternalQ, StateTElec;
    double SystemOmegaBase, ActualPMech, fInputFromExciter;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	speedState = states[STATE_Speed];

	H  = floatParams[PARAM_H];
	fD = floatParams[PARAM_D];
	fInv2H = 1/(2*H);

	SystemOmegaBase   = SystemOptions->WBase;
	ActualPMech       = hardCodedSignals[HARDCODE_MACHINE_TSPmech    ];
	fInputFromExciter = hardCodedSignals[HARDCODE_MACHINE_TSGenFieldV];
	fStateId          = hardCodedSignals[HARDCODE_MACHINE_TSstateId  ];
	fStateIq          = hardCodedSignals[HARDCODE_MACHINE_TSstateIq  ];

	StateEInternalD = 0;
	StateEInternalQ = fInputFromExciter;
	StateTElec = StateEInternalD * fStateId + StateEInternalQ*fStateIq;

	(*dotX)[STATE_Angle] = speedState*SystemOmegaBase; //  
	(*dotX)[STATE_Speed] = fInv2H*((ActualPMech - fD*speedState)/(1+speedState)  - StateTElec);
}


extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{

}


extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
	return(N_NONWINDUPLIMITS);
}


extern "C" __declspec(dllexport) double __stdcall MachineSpeedDeviationOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& states = *ParamsAndStates->States;

	return(states[STATE_Speed]);
}


// Either MachineNortonCurrent is required,
// or MachineTheveninVoltage and MachineTheveninImpedance are required.
// This model uses the Thevenin pair. To use MachineNortonCurrent instead, define it
// (declared in pwUDM.Machine) and remove the two Thevenin functions below.
extern "C" __declspec(dllexport) void __stdcall MachineTheveninVoltage(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* Delta, double* Vd, double* Vq)
{
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

    *Delta = states[STATE_Angle];
    *Vd = 0;
    *Vq = hardCodedSignals[HARDCODE_MACHINE_TSGenFieldV];
}


extern "C" __declspec(dllexport) void __stdcall MachineTheveninImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theR, double* theX)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;

    *theR = floatParams[PARAM_Ra];
    *theX = floatParams[PARAM_Xdp];
}


extern "C" __declspec(dllexport) double __stdcall MachineFieldCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	return(hardCodedSignals[HARDCODE_MACHINE_TSGenFieldV]);
}


extern "C" __declspec(dllexport) double __stdcall MachineElectricalTorque(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;

	double fVd, fVq, fStateId, fStateIq;

	fVd      = 0;
	fVq      = hardCodedSignals[HARDCODE_MACHINE_TSGenFieldV];
	fStateId = hardCodedSignals[HARDCODE_MACHINE_TSstateId  ];
	fStateIq = hardCodedSignals[HARDCODE_MACHINE_TSstateIq  ];
	return(fVd*fStateId + fVq * fStateIq);
}


// Optional exports not implemented by this model (declared in pwUDM.Machine):
// MachineHighVReactiveCurrentLim, MachineLowVActiveCurrentPoints, MachineCompensatingImpedance.
// Define any of them here to provide that behavior.

}
