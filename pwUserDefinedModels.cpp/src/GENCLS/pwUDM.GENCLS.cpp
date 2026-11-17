module pwUDM.GENCLS;

import std;
import pwUDM.Core;
import pwUDM.Machine;

namespace pwUDM::GENCLS {

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
	return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{	
    int result = pwStringCopy(StrBuf, StrSize, pwModelClassString(ModelClass::Machine));
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds)
{
	// The model-specific parameters
	numbersOfEverything->nFloatParams = N_F_PARAMS;
	numbersOfEverything->nIntParams = N_I_PARAMS;
	numbersOfEverything->nStrParams = N_S_PARAMS;
	numbersOfEverything->nStates = N_STATES;
	numbersOfEverything->nAlgebraics = N_ALGEBRAICS;	
	// For nonwindup limits of the model
	numbersOfEverything->nNonWindUpLimits	= N_NONWINDUPLIMITS;
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* ts;

	switch (*ParamNum)
	{
		case PARAM_H:     ts = L"H"; break;
		case PARAM_D:     ts = L"D"; break;
		case PARAM_Ra:    ts = L"Ra"; break;
		case PARAM_Xdp:   ts = L"Xdp"; break;
		case PARAM_Rcomp: ts = L"Rcomp"; break;
		case PARAM_Xcomp: ts = L"Xcomp"; break;
		default:          ts = L""; break;
	}

	int result = pwStringCopy(StrBuf, StrSize, ts);
	return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* ts;

	switch (*StateNum)
	{
		case STATE_Angle: ts = L"Angle"; break;
		case STATE_Speed: ts = L"Speed"; break;
		default:          ts = L""; break;
	}

	int result = pwStringCopy(StrBuf, StrSize, ts);
	return result;
}

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int __stdcall OtherObjectClass(int* Num, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* ts;

    switch (*Num)
    {
        case 0:   ts = L"Bus"; break;
        default:  ts = L""; break;
    }

    int result = pwStringCopy(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int __stdcall OtherObjectDescription(int* Num, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* ts;

    switch (*Num)
    {
        case 0:   ts = L"Signal Bus"; break;
        default:  ts = L""; break;
    }

    int result = pwStringCopy(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TxMyModelData* ParamsAndStates)
{
	DoubleArray* pDoubArr;

	// Example TGOV1 parameters
	pDoubArr = ParamsAndStates->FloatParams;
	(*pDoubArr)[PARAM_H] = 3.0;
	(*pDoubArr)[PARAM_D] = 0.0;
	(*pDoubArr)[PARAM_Ra] = 0.0;
	(*pDoubArr)[PARAM_Xdp] = 0.2;
	(*pDoubArr)[PARAM_Rcomp] = 0.0;
	(*pDoubArr)[PARAM_Xcomp] = 0.0;
}

/* non specified for this model
extern "C" __declspec(dllexport) int __stdcall getStringParamDefaultValue(int* Num, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* ts;

    switch (*Num)
    {
        case 0:   ts = L"DefaultString0"; break;
        default:  ts = L""; break;
    }

    int result = pwStringCopy(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) int __stdcall signalSelection(int* Num, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* ts;

    ts = L""; // there are none!
    switch (*Num)
    {
        case ALG_XXX: ts = L"TSXXX"; break;
        default:  ts = L""; break;
    }

    int result = pwStringCopy(StrBuf, StrSize, ts);
    return result;
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates,	double* TimeStepSeconds)
{
	// Return an integer value between 0 and 7
	// This function returns the EXPONENT of 2 for the number subintervals.
	// Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
	// The maximum number of subintervals allowed is 128, so (2^7) = 128
	return(0);
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall initializeYourself(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    /****************************************************************************************************************/
    /* Four quadrant arctangent function.  Returns values between -Pi and Pi */
    // from MathUnit
    auto LOCAL_ArcTn2 = [](double y, double x) -> double {
        double result;
        if (x != 0.0) result = std::atan(y/x);
        else
            if (y < 0.0) result = -std::numbers::pi / 2.0;
            else result = std::numbers::pi/2.0;
        if ((x < 0) && (result <= 0)) result = result + std::numbers::pi;       // KJP 2/5/01 - Added equality as well, since if x < 0 and y = 0, you always get -pi,
        else if ((x < 0) && (result > 0)) result = result - std::numbers::pi;  // which conflicts with other results when y <> 0
        return result;
    };

	double local_Vd,local_Vq;  // Vdq voltage used locally
    double local_PsiD,local_PsiQ;  // Initial psiD and psiQ values
    double Pmech_In, Efd_In;
    double fRa, fXdp;

    double fStateId,fStateIq;

    double local_Delta;
    double local_Itr, local_Iti;
    double local_Vtr, local_Vti;
    double local_Vintr, local_Vinti;
    double CosDelta, SinDelta;

	DoubleArray* pFloatParamArr;
	DoubleArray* pStatesArr;
	DoubleArray* pHardCodedArr;

	// dq stator currents

	//--------------------------------------------------------------------
	// Setup local array variables to make code easier to read
	//--------------------------------------------------------------------
	pFloatParamArr = ParamsAndStates->FloatParams;
	pStatesArr = ParamsAndStates->States;
	pHardCodedArr = ParamsAndStates->HardCodedSignals;

	//--------------------------------------------------------------------
	// Grab States, Params, Algebraics Needed for this method
	//--------------------------------------------------------------------
	fRa   = (*pFloatParamArr)[PARAM_Ra ];
	fXdp  = (*pFloatParamArr)[PARAM_Xdp];

	local_Vtr = (*pHardCodedArr)[HARDCODE_MACHINE_InitVreal];
	local_Vti = (*pHardCodedArr)[HARDCODE_MACHINE_InitVimag];
	local_Itr = (*pHardCodedArr)[HARDCODE_MACHINE_InitIreal];
	local_Iti = (*pHardCodedArr)[HARDCODE_MACHINE_InitIimag];

	//--------------------------------------------------------------------
	// Calculate the Internal Voltage
	//--------------------------------------------------------------------
	// = Itr + jIti
	// Iterm-->
	// /------Ra+jXdp-----------             Vint = Vterm + Iterm*(Ra+jXdp)
	// |+                 +                       = Vtr+jVti + (Itr+jIti)*(Ra+jXdp)
	// Vint              Vterm = Vtr+jVti          = (Vtr + Itr*Ra - Iti*Xdp) + j(Vti + Iti*Ra + Itr*Xdp)
	// |-                 -                       = Vintr + jVinti
	// Ground

	local_Vintr = local_Vtr + local_Itr*fRa - local_Iti*fXdp;
	local_Vinti = local_Vti + local_Iti*fRa + local_Itr*fXdp;
	// Get Angle of Internal Voltage
	local_Delta = LOCAL_ArcTn2(local_Vinti,local_Vintr);

	//--------------------------------------------------------------------
	// Convert terminal Current and Voltage to the "DQ reference frame"
	// Simular the Figure 3.4 from Sauer/Pai book
	//--------------------------------------------------------------------
	CosDelta = std::cos(local_Delta);
	SinDelta = std::sin(local_Delta);
	fStateId = -CosDelta*local_Iti + SinDelta*local_Itr;
	fStateIq =  CosDelta*local_Itr + SinDelta*local_Iti;
	local_Vd = -CosDelta*local_Vti + SinDelta*local_Vtr;
	local_Vq =  CosDelta*local_Vtr + SinDelta*local_Vti;

	//--------------------------------------------------------------------
	// Perform calculations
	//--------------------------------------------------------------------
	// fpsiD and fpsiQ are found using 3.215 and 3.216 in Sauer/Pai book
	local_PsiD =  (local_Vq + fRa*fStateIq);
	local_PsiQ = -(local_Vd + fRa*fStateId);

	// These initial values of the inputs should actually be SET here
	Efd_In = local_PsiD + fStateId*fXdp;
	Pmech_In =(local_PsiD * fStateIq - local_PsiQ * fStateId);

	//--------------------------------------------------------------------
	// Write out initial state vector
	//--------------------------------------------------------------------
	(*pStatesArr)[STATE_Angle] = local_Delta;
	(*pStatesArr)[STATE_Speed] = 0;

	//--------------------------------------------------------------------
	// Pass back initial inputs
	// exciter input Efd
	// governor input Pmech
	//--------------------------------------------------------------------
	(*pHardCodedArr)[HARDCODE_MACHINE_TSGenFieldV] = Efd_In;
	(*pHardCodedArr)[HARDCODE_MACHINE_TSPmech] = Pmech_In;

	(*pHardCodedArr)[HARDCODE_MACHINE_TSstateId] = fStateId;
	(*pHardCodedArr)[HARDCODE_MACHINE_TSstateIq] = fStateIq;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{
	// First there are the parameters,
	double H, fInv2H, fD;
	double speedState;  // state at the beginning of the time is passed in

	// dq stator currents always referred to gen side of any off-nominal step-up turns ratio
	double fStateId, fStateIq;

	double StateEInternalD, StateEInternalQ, StateTElec;
	double SystemOmegaBase, ActualPMech, fInputFromExciter;

	DoubleArray* pFloatParamArr;
	DoubleArray* pStatesArr;
	DoubleArray* pHardCodedArr;
	DoubleArray* pDotXArr;

	// Setup local array variables to make code easier to read
	pFloatParamArr = ParamsAndStates->FloatParams;
	pStatesArr = ParamsAndStates->States;
	pHardCodedArr = ParamsAndStates->HardCodedSignals;
	pDotXArr = dotX;

	// Grab States, Params, Algebraics Needed for this method
	speedState = (*pStatesArr)[STATE_Speed];

	H  = (*pFloatParamArr)[PARAM_H];
	fD = (*pFloatParamArr)[PARAM_D];
	fInv2H = 1/(2*H);

	SystemOmegaBase   = SystemOptions->WBase;
	ActualPMech       = (*pHardCodedArr)[HARDCODE_MACHINE_TSPmech    ];
	fInputFromExciter = (*pHardCodedArr)[HARDCODE_MACHINE_TSGenFieldV];
	fStateId          = (*pHardCodedArr)[HARDCODE_MACHINE_TSstateId  ];
	fStateIq          = (*pHardCodedArr)[HARDCODE_MACHINE_TSstateIq  ];

	// Perform Calculations necessary
	// StateEInternalD is always zero for gencls
	StateEInternalD = 0;
	StateEInternalQ = fInputFromExciter;
	StateTElec = StateEInternalD * fStateId + StateEInternalQ*fStateIq;

	// ??? kate 1/5/12 figure out support for subintervals after basic stuff works!
	// If SubInterval2PowerSpecified > 0 Then UpdateStateCurrent(GenObject.DeviceBusVoltage);  // Sets the state current values for the generators

	// Push the resulting derivatives to the appropriate memory
	(*pDotXArr)[STATE_Angle] = speedState*SystemOmegaBase; //  
	(*pDotXArr)[STATE_Speed] = fInv2H*((ActualPMech - fD*speedState)/(1+speedState)  - StateTElec);
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    // no ignored states or handling of algebraics here
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
	return(0);
}

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) double __stdcall MachineHighVReactiveCurrentLim(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    return (*pFloatParamArr)[PARAM_VHighLimit];
}
*/

//******************************************************************************
/* non specified for this model
extern "C" __declspec(dllexport) void __stdcall MachineLowVActiveCurrentPoints(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* Lvpnt1, double* Lvpnt0)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    *Lvpnt1 = (*pFloatParamArr)[PARAM_Lvpnt1];
    *Lvpnt0 = (*pFloatParamArr)[PARAM_Lvpnt0];
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) double __stdcall MachineSpeedDeviationOut(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
	DoubleArray* pStatesArr;

	pStatesArr = ParamsAndStates->States;
	return((*pStatesArr)[STATE_Speed]);
}


// Either MachineNortonCurrent is required,
// or MachineTheveninVoltage and MachineTheveninImpedance are required.
// This model uses the Thevenin pair. To use MachineNortonCurrent instead, define it
// (declared in pwUDM.Machine) and remove the two Thevenin functions below.

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall MachineTheveninVoltage(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* Delta, double* Vd, double* Vq)
{
    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;

    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    *Delta = (*pStatesArr)[STATE_Angle];
    *Vd = 0;  // ALWAYS zero for a Classical Machine
    *Vq = (*pHardCodedArr)[HARDCODE_MACHINE_TSGenFieldV];
}

/* non specified for this model
extern "C" __declspec(dllexport) void __stdcall MachineNortonCurrent(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* IReal, double* IImag)
{
}
*/

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall MachineTheveninImpedance(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* theR, double* theX)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    *theR = (*pFloatParamArr)[PARAM_Ra];
    *theX = (*pFloatParamArr)[PARAM_Xdp];
}

extern "C" __declspec(dllexport) double __stdcall MachineFieldCurrent(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
	DoubleArray* pHardCodedArr;

	// Field Current and Voltage are the same for this model
	pHardCodedArr = ParamsAndStates->HardCodedSignals;
	return((*pHardCodedArr)[HARDCODE_MACHINE_TSGenFieldV]);
}

extern "C" __declspec(dllexport) double __stdcall MachineElectricalTorque(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
	DoubleArray* pHardCodedArr;
	double fVd, fVq, fStateId, fStateIq;

	pHardCodedArr = ParamsAndStates->HardCodedSignals;
	fVd      = 0;  // ALWAYS zero for a Classical Machine
	fVq      = (*pHardCodedArr)[HARDCODE_MACHINE_TSGenFieldV];
	fStateId = (*pHardCodedArr)[HARDCODE_MACHINE_TSstateId  ];
	fStateIq = (*pHardCodedArr)[HARDCODE_MACHINE_TSstateIq  ];
	return(fVd*fStateId + fVq * fStateIq);
}

/* non specified for this model
extern "C" __declspec(dllexport) void __stdcall MachineCompensatingImpedance(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* Rcomp, double* Xcomp)
{
    DoubleArray* pFloatParamArr;

    pFloatParamArr = ParamsAndStates->FloatParams;
    *RComp = (*pFloatParamArr)[PARAM_RComp];
    *XComp = (*pFloatParamArr)[PARAM_XComp];
}
*/


// Optional exports not implemented by this model (declared in pwUDM.Machine):
// MachineHighVReactiveCurrentLim, MachineLowVActiveCurrentPoints, MachineCompensatingImpedance.
// Define any of them here to provide that behavior.

}
