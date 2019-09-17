#include "..\common\pwUDM.h"
#include "..\common\pwUDM.Machine.h"
#include "..\common\pwUDM.Helper.h"
#include "pwUDM.GENCLS.h"
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
	
    udmClass = mcnMachine;

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
	(*numbersOfEverything).nNonWindUpLimits	= N_NONWINDUPLIMITS;
}


PW_UDM_DllExport(int) parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	wchar_t* str;
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


PW_UDM_DllExport(int) stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	wchar_t* str;
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


PW_UDM_DllExport(void) getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
	(*((*ParamsAndStates).FloatParams))[PARAM_H] = 3.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_D] = 0.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_Ra] = 0.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_Xdp] = 0.2;
	(*((*ParamsAndStates).FloatParams))[PARAM_Rcomp] = 0.0;
	(*((*ParamsAndStates).FloatParams))[PARAM_Xcomp] = 0.0;
}


PW_UDM_DllExport(int) SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates,	double* timeStepSeconds)
{
	return(0);
}


PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
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

	fRa   = (*((*ParamsAndStates).FloatParams))[PARAM_Ra ];
	fXdp  = (*((*ParamsAndStates).FloatParams))[PARAM_Xdp];

	local_Vtr = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_InitVreal];
	local_Vti = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_InitVimag];
	local_Itr = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_InitIreal];
	local_Iti = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_InitIimag];

	local_Vintr = local_Vtr + local_Itr*fRa - local_Iti*fXdp;
	local_Vinti = local_Vti + local_Iti*fRa + local_Itr*fXdp;
	local_Delta = atan2(local_Vinti,local_Vintr);

	CosDelta = cos(local_Delta);
	SinDelta = sin(local_Delta);
	fStateId = -CosDelta*local_Iti + SinDelta*local_Itr;
	fStateIq =  CosDelta*local_Itr + SinDelta*local_Iti;
	local_Vd = -CosDelta*local_Vti + SinDelta*local_Vtr;
	local_Vq =  CosDelta*local_Vtr + SinDelta*local_Vti;

	local_PsiD =  (local_Vq + fRa*fStateIq);
	local_PsiQ = -(local_Vd + fRa*fStateId);

	Efd_In = local_PsiD + fStateId*fXdp;
	Pmech_In =(local_PsiD * fStateIq - local_PsiQ * fStateId);

	(*((*ParamsAndStates).States))[STATE_Angle] = local_Delta;
	(*((*ParamsAndStates).States))[STATE_Speed] = 0;

	(*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSGenFieldV] = Efd_In;
	(*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSPmech] = Pmech_In;

	(*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSstateId] = fStateId;
	(*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSstateIq] = fStateIq;
}


PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	double H , fInv2H, fD;
    double speedState;
    
    double fStateId, fStateIq;

    double StateEInternalD, StateEInternalQ, StateTElec;
    double SystemOmegaBase, ActualPMech, fInputFromExciter;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	speedState = (*((*ParamsAndStates).States))[STATE_Speed];

	H  = (*((*ParamsAndStates).FloatParams))[PARAM_H];
	fD = (*((*ParamsAndStates).FloatParams))[PARAM_D];
	fInv2H = 1/(2*H);

	SystemOmegaBase   = (*SystemOptions).WBase;
	ActualPMech       = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSPmech    ];
	fInputFromExciter = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSGenFieldV];
	fStateId          = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSstateId  ];
	fStateIq          = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSstateIq  ];

	StateEInternalD = 0;
	StateEInternalQ = fInputFromExciter;
	StateTElec = StateEInternalD * fStateId + StateEInternalQ*fStateIq;

	(*dotX)[STATE_Angle] = speedState*SystemOmegaBase; //  
	(*dotX)[STATE_Speed] = fInv2H*((ActualPMech - fD*speedState)/(1+speedState)  - StateTElec);
}


PW_UDM_DllExport(void) PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{

}


PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
	return(N_NONWINDUPLIMITS);
}


PW_UDM_DllExport(double) MachineSpeedDeviationOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	return((*((*ParamsAndStates).States))[STATE_Speed]);
}


PW_UDM_DllExport(void) MachineTheveninImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theR, double* theX)
{
	*theR = (*((*ParamsAndStates).FloatParams))[PARAM_Ra ];
	*theX = (*((*ParamsAndStates).FloatParams))[PARAM_Xdp];
}


PW_UDM_DllExport(void) MachineTheveninVoltage(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* Delta, double* Vd, double* Vq)
{
	*Delta = (*((*ParamsAndStates).States))[STATE_Angle];
	*Vd = 0;
	*Vq = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSGenFieldV];
}


PW_UDM_DllExport(double) MachineFieldCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	return((*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSGenFieldV]);
}


PW_UDM_DllExport(double) MachineElectricalTorque(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
	double fVd, fVq, fStateId, fStateIq;

	fVd      = 0;
	fVq      = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSGenFieldV];
	fStateId = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSstateId  ];
	fStateIq = (*((*ParamsAndStates).HardCodedSignals))[HARDCODE_MACHINE_TSstateIq  ];
	return(fVd*fStateId + fVq * fStateIq);
}


PW_UDM_DllExport(double) MachineNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag)
{
    return(0);
}


PW_UDM_DllExport(double) MachineHighVReactiveCurrentLim(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    return(0);
}


PW_UDM_DllExport(void) MachineLowVActiveCurrentPoints(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* LVPnt1, double* LVPnt0)
{

}


PW_UDM_DllExport(void) MachineCompensatingImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* RComp, double* XComp)
{

}