#include "..\common\pwUDM.h"
#include "..\common\pwUDM.Load.h"
#include "..\common\pwUDM.Helper.h"
#include "pwUDM.CLOD.h"
#include <math.h>
#include <stdexcept>

PW_UDM_DllExport(int) DLLVersion()
{
	return(1);
}

PW_UDM_DllExport(void) allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds)
{
	(*numbersOfEverything).nFloatParams		= N_F_PARAMS;
	(*numbersOfEverything).nIntParams		= N_I_PARAMS;
	(*numbersOfEverything).nStrParams		= N_S_PARAMS;
	(*numbersOfEverything).nStates			= N_STATES;
	(*numbersOfEverything).nAlgebraics		= N_ALGEBRAICS;
	// The network-specific parameters
	// (*numbersOfEverything).nNetworkInputs	= N_F_NETWORKINPUTS;
	// (*numbersOfEverything).nNetworkOutputs	= N_F_NETWORKOUTPUTS;
	// (*numbersOfEverything).nFixedInputs		= N_F_FIXEDINPUTS;
	// For nonwindup limits of the model
	(*numbersOfEverything).nNonWindUpLimits	= N_NONWINDUPLIMITS;
}

PW_UDM_DllExport(int) parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	wchar_t* str;
	int stringLength;
	
	if (*ParamNum == 0) {
		str = L"%Lmotor";
	}
	else if (*ParamNum == 1) {
		str = L"%Smotor";
	}
	else if (*ParamNum == 2) {
		str = L"%Tex";
	}
	else if (*ParamNum == 3) {
		str = L"%Dis";
	}
	else if (*ParamNum == 4) {
		str = L"%P";
	}
	else if (*ParamNum == 5) {
		str = L"Kp";
	}
	else if (*ParamNum == 6) {
		str = L"R";
	}
	else if (*ParamNum == 7) {
		str = L"X";
	}
	else if (*ParamNum == 8) {
		str = L"Vi";
	}
	else if (*ParamNum == 9) {
		str = L"Ti(cycles)";
	}
	else if (*ParamNum == 10) {
		str = L"Tb(cycles)";
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
	
	if (*StateNum == 0) {
		str = L"M 1 Speed wr";
	}
	else if (*StateNum == 1) {
		str = L"IM 1 Epr";
	}
	else if (*StateNum == 2) {
		str = L"M 1 Epi";
	}
	else if (*StateNum == 3) {
		str = L"IM 2 Speed wr";
	}
	else if (*StateNum == 4) {
		str = L"IM 2 Epr";
	}
	else if (*StateNum == 5) {
		str = L"IM 2 Epi";
	}
    else {
        str = L"";
    }

    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}

// START HERE!!!!

PW_UDM_DllExport(void) getDefaultParameterValues(TTxMyModelData* ParamsAndStates)
{
	double fLmotor = 24.0;
	double fSmotor = 24.0;
	double fTex = 1.0; // only non-zero values will work for this DLL
	double fDis = 20.0;
	double fP = 1.0; // only non-zero values will work for this DLL
	double fKp = 1.0;
	double fR = 0.1; // only non-zero values will work for this DLL
	double fX = 0.1; // only non-zero values will work for this DLL
	double fVi = 0.75;
	double fTi = 60.0;
	double fTb = 0.1; // only non-zero values will work for this DLL

	// only non-zero values will work for this DLL
	(*((*ParamsAndStates).FloatParams))[0]	=	fLmotor;
	(*((*ParamsAndStates).FloatParams))[1]	=	fSmotor;
	(*((*ParamsAndStates).FloatParams))[2]	=	fTex;
	(*((*ParamsAndStates).FloatParams))[3]	=	fDis;
	(*((*ParamsAndStates).FloatParams))[4]	=	fP;
	(*((*ParamsAndStates).FloatParams))[5]	=	fKp;
	(*((*ParamsAndStates).FloatParams))[6]	=	fR;
	(*((*ParamsAndStates).FloatParams))[7]	=	fX;
	(*((*ParamsAndStates).FloatParams))[8]	=	fVi;
	(*((*ParamsAndStates).FloatParams))[9]	=	fTi;
	(*((*ParamsAndStates).FloatParams))[10]	=	fTb;
}

PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    /*
	const double DegToRad = 4*atan(1.0)/180;

	double fLmotor, fSmotor, fTex, fDis, fP, fKp, fR, fX, fVi, fTi, fTb;

	double IMLSpeedState, IMLEprState, IMLEpiState, IMSSpeedState, IMSEprState, IMSEpiState;

	double dotIMLSpeedState, dotIMLEprState, dotIMLEpiState, dotIMSSpeedState, dotIMSEprState, dotIMSEpiState;

	double IML_fPInitial, IMS_fPInitial, IML_fQInitial, IMS_fQInitial;
	double IML_fSlipInitial, IMS_fSlipInitial;

	double IML_Rs, IML_Xs, IML_Xm, IML_R1, IML_X1, IML_E1, IML_S1, IML_E2, IML_S2 , IML_MBase, IML_PMult, IML_H, IML_Vi, IML_Ti, IML_Tb, IML_D, IML_Tnom;

	double IMS_Rs, IMS_Xs, IMS_Xm, IMS_R1, IMS_X1, IMS_E1, IMS_S1, IMS_E2, IMS_S2 , IMS_MBase, IMS_PMult, IMS_H, IMS_Vi, IMS_Ti, IMS_Tb, IMS_D, IMS_Tnom;

	double IML_tScale, IMS_tScale;
	double IML_fInv2H, IMS_fInv2H;

	double IML_Xp, IMS_Xp;
	double IML_X, IMS_X;

	double IMLfStateIr_In,   IMLfStateIi_In, IMSfStateIr_In,  IMSfStateIi_In;

	double IML_StateElectTorque, IMS_StateElectTorque;
	double IML_MechanicalTorque, IMS_MechanicalTorque;
	double IML_stateSlip, IMS_stateSlip;

	double IML_fWo, IMS_fWo;

	double IMLfInvTp, IMSfInvTp;
	double IMLfxA, IMLfxB, IMLfxC, IMLfxD, IMLfxE;
	double IMSfxA, IMSfxB, IMSfxC, IMSfxD, IMSfxE;

	double reLoadObjBusVolt, imLoadObjBusVolt;
	PWDComplex LoadObjBusVolt_In;

	bool inService_In;
	double loadObjNomFreq_In, local_Ti, local_Tb;

	double localUsedP, localUsedQ;

	PWDComplex IMLlocal_Ep, IMSlocal_Ep;
	PWDComplex IML_local_IIn, IMS_local_IIn;
	PWDComplex IML_local_impedanceSystem, IMS_local_impedanceSystem;

	double t3, t4, t5;

	double local_Ekr, local_Eki, tQInit;

	double fSatA, fSatB;
	double fErpp, fEipp;

	double theSystemMVABase;

	PWDComplex IML_localCurrent, IMS_localCurrent;

	double dTemp;

	double LoadObjV_In, LoadObjThetaDeg_In;

	fLmotor    = (*((*ParamsAndStates).FloatParams))[0];
	fSmotor    = (*((*ParamsAndStates).FloatParams))[1];
	fTex       = (*((*ParamsAndStates).FloatParams))[2];
	fDis       = (*((*ParamsAndStates).FloatParams))[3];
	fP         = (*((*ParamsAndStates).FloatParams))[4];
	fKp        = (*((*ParamsAndStates).FloatParams))[5];
	fR         = (*((*ParamsAndStates).FloatParams))[6];
	fX         = (*((*ParamsAndStates).FloatParams))[7];
	fVi        = (*((*ParamsAndStates).FloatParams))[8];
	fTi        = (*((*ParamsAndStates).FloatParams))[9];
	fTb        = (*((*ParamsAndStates).FloatParams))[10];

	LoadObjV_In           = (*((*ParamsAndStates).HardCodedSignals))[0];
	LoadObjThetaDeg_In    = (*((*ParamsAndStates).HardCodedSignals))[1];
	dTemp               = (*((*ParamsAndStates).HardCodedSignals))[2];
	loadObjNomFreq_In   = (*((*ParamsAndStates).HardCodedSignals))[3];
	theSystemMVABase    = (*((*ParamsAndStates).HardCodedSignals))[4];

	inService_In = DoubleToBoolean(dTemp);

	localUsedP          = (*((*ParamsAndStates).HardCodedSignals))[5];
	localUsedQ          = (*((*ParamsAndStates).HardCodedSignals))[6];

	IML_Tnom            = (*((*ParamsAndStates).HardCodedSignals))[7];
	IMS_Tnom            = (*((*ParamsAndStates).HardCodedSignals))[8];
	IML_fWo             = (*((*ParamsAndStates).HardCodedSignals))[9];
	IMS_fWo             = (*((*ParamsAndStates).HardCodedSignals))[10];

	IMLfStateIr_In      = (*((*ParamsAndStates).HardCodedSignals))[11];
	IMLfStateIi_In      = (*((*ParamsAndStates).HardCodedSignals))[12];
	IMSfStateIr_In      = (*((*ParamsAndStates).HardCodedSignals))[13];
	IMSfStateIi_In      = (*((*ParamsAndStates).HardCodedSignals))[14];
	IMLfInvTp           = (*((*ParamsAndStates).HardCodedSignals))[15];
	IMSfInvTp           = (*((*ParamsAndStates).HardCodedSignals))[16];
	IML_Xp              = (*((*ParamsAndStates).HardCodedSignals))[17];
	IMS_Xp              = (*((*ParamsAndStates).HardCodedSignals))[18];
	IML_X               = (*((*ParamsAndStates).HardCodedSignals))[19];
	IMS_X               = (*((*ParamsAndStates).HardCodedSignals))[20];
	IML_fPInitial       = (*((*ParamsAndStates).HardCodedSignals))[21];
	IMS_fPInitial       = (*((*ParamsAndStates).HardCodedSignals))[22];

	LoadObjThetaDeg_In = LoadObjThetaDeg_In*DegToRad;
	LoadObjBusVolt_In = PWDComplex_InitPolarRad(LoadObjV_In,LoadObjThetaDeg_In);

	local_Ti = fTi/loadObjNomFreq_In;
	local_Tb = fTb/loadObjNomFreq_In;

	IML_Rs = 0.013;
	IML_Xs = 0.067;
	IML_Xm = 3.8;
	IML_R1 = 0.009;
	IML_X1 = 0.17;
	IML_E1 = 0.0;
	IML_S1 = 0.0;
	IML_E2 = 0.0;
	IML_S2 = 0.0;
	
	IML_PMult = 1;
	IML_H = 1.5;
	IML_Vi = fVi;
	IML_Ti = local_Ti;
	IML_Tb = local_Tb;
	IML_D = 2;
	
	IML_MBase = IML_fPInitial*theSystemMVABase;
	IML_tScale = theSystemMVABase/IML_MBase;

	IML_fInv2H = (1/(2*IML_H))*(1/IML_fPInitial);

	IML_Rs = IML_Rs*IML_tScale;
	IML_Xs = IML_Xs*IML_tScale;
	
	IMS_Rs = 0.031;
	IMS_Xs = 0.1;
	IMS_Xm = 3.2;
	IMS_R1 = 0.018;
	IMS_X1 = 0.18;
	IMS_E1 = 0.0;
	IMS_S1 = 0.0;
	IMS_E2 = 0.0;
	IMS_S2 = 0.0;
	
	IMS_PMult = 1;
	IMS_H = 0.7;
	IMS_Vi = fVi;
	IMS_Ti = local_Ti;
	IMS_Tb = local_Tb;
	IMS_D = 2;
	
	IMS_MBase = IMS_fPInitial*theSystemMVABase;
	IMS_tScale = theSystemMVABase/IMS_MBase;

	IMS_fInv2H = (1/(2*IMS_H))*(1/IMS_fPInitial);

	IMS_Rs = IMS_Rs*IMS_tScale;
	IMS_Xs = IMS_Xs*IMS_tScale;
    /*
	if (inService_In)
	{
		IML_fQInitial = 0;
		IML_localCurrent = PWDComplex_Init(IML_fPInitial,IML_fQInitial).Divide(LoadObjBusVolt_In);
		IMLfStateIr_In = IML_localCurrent.r;
		IMS_fQInitial = 0;
		IMLfStateIi_In = - IML_localCurrent.i;
		IMS_localCurrent = PWDComplex_Init(IMS_fPInitial,IMS_fQInitial).Divide(LoadObjBusVolt_In);
		IMSfStateIr_In = IMS_localCurrent.r;
		IMSfStateIi_In = - IMS_localCurrent.i;

		IML_local_IIn = PWDComplex_Init(IMLfStateIr_In, IMLfStateIi_In);
		IML_local_impedanceSystem = PWDComplex_Init(IML_Rs, IML_Xp);
	
		IMLlocal_Ep = LoadObjBusVolt_In.CDec(IML_local_IIn.Multiply(IML_local_impedanceSystem));
	
		t3 = -IMLfStateIr_In;
		t4 = -IMLfStateIi_In;
		t5 = -IML_fPInitial;

		fSatA = 0;
		fSatB = 0;
		IML_fSlipInitial = 0;
		tQInit = 0;

        

		// TxInductionMachine_GetInitialConditions(True, False, IML_fWo, t5, IML_Rs, IML_X, IML_Xp, 0, IML_Xs, IMLfInvTp, 0, fSatA, fSatB, LoadObjBusVolt_In, 10, 1e-5, IML_fSlipInitial, IMLlocal_Ep.r, IMLlocal_Ep.i, fErpp, fEipp, local_Ekr, local_Eki, t3, t4, tQInit);
	
		IMLfStateIr_In = -t3;
		IMLfStateIi_In = -t4;
			
		IMLSpeedState = 1 - IML_fSlipInitial;
		IMLEprState = IMLlocal_Ep.r;
		IMLEpiState = IMLlocal_Ep.i;

		IML_StateElectTorque = IMLEprState*IMLfStateIr_In + IMLEpiState*IMLfStateIi_In;
		// IML_Tnom = IML_StateElectTorque/Math_Y_to_X(1-IML_fSlipInitial, IML_D);

	
		IMS_local_IIn = PWDComplex_Init(IMSfStateIr_In, IMSfStateIi_In);
		IMS_local_impedanceSystem = PWDComplex_Init(IMS_Rs, IMS_Xp); 



		IMSlocal_Ep = LoadObjBusVolt_In.CDec(IMS_local_IIn.Multiply(IMS_local_impedanceSystem));

	
		t3 = -IMSfStateIr_In;
		t4 = -IMSfStateIi_In;
		t5 = -IMS_fPInitial; 

	
		fSatA = 0;
		fSatB = 0;
		IMS_fSlipInitial = 0; 
		fErpp = 0;
		fEipp = 0;
		local_Ekr = 0;
		local_Eki = 0;
		tQInit = 0; //

		//TxInductionMachine_GetInitialConditions(True,False,IMS_fWo,t5,IMS_Rs,IMS_X,IMS_Xp,0,IMS_Xs,IMSfInvTp,0,fSatA,fSatB,LoadObjBusVolt_In,10,1e-5,IMS_fSlipInitial,IMSlocal_Ep.r,IMSlocal_Ep.i,fErpp,fEipp,local_Ekr,local_Eki,t3,t4,tQInit); 
		IMSfStateIr_In = -t3;
		IMSfStateIi_In = -t4;

		{ Tell the integration object the initial values }
	
		IMSSpeedState = 1- IMS_fSlipInitial;
		IMSEprState = IMSlocal_Ep.r;
		IMSEpiState = IMSlocal_Ep.i;

		IMS_StateElectTorque = IMSEprState*IMSfStateIr_In + IMSEpiState*IMSfStateIi_In;
		IMS_Tnom = IMS_StateElectTorque/Math_Y_to_X(1-IMS_fSlipInitial, IMS_D);

	
	}
	Else Begin // If out-of-service then set all to zero
	IMLSpeedState = 0; //   IO.InitialStateVector[firstState] = 0;
	IMLEprState = 0; //  IO.InitialStateVector[firstState+1] = 0;
	IMLEpiState = 0; //  IO.InitialStateVector[firstState+2] = 0;

	IMSSpeedState = 0;
	IMSEprState = 0;
	IMSEpiState = 0;

	end;


	// kate 2/1/12 not sure what to do with these functions but they probably need to be supported somewhere in here.
	//// TJO 4/15/08 - the next six functions are for mapping back load data to the power flow model.
	//    Function mapSMW(local_Volt : Double; local_BusPUDeltaFreq : Single) : Double; virtual;
	//    Function mapSMVR(local_Volt : Double; local_BusPUDeltaFreq : Single) : Double; virtual;
	//    Function mapIMW(local_Volt : Double; local_BusPUDeltaFreq : Single) : Double; virtual;
	//    Function mapIMVR(local_Volt : Double; local_BusPUDeltaFreq : Single) : Double; virtual;
	//    Function mapZMW(local_Volt : Double; local_BusPUDeltaFreq : Single) : Double; virtual;
	//    Function mapZMVR(local_Volt : Double; local_BusPUDeltaFreq : Single) : Double; virtual;


	// For the state initialization, look at the initialization for TTxCIM5_SingleCage_LoadModel

	// Write out initial state vector,
	pStatesArr = PStatesArray(ParamsAndStates.States);
	pStatesArr^[0] = IMLSpeedState;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
	pStatesArr^[1] = IMLEprState;
	pStatesArr^[2] = IMLEpiState;
	pStatesArr^[3] = IMSSpeedState;
	pStatesArr^[4] = IMSEprState;
	pStatesArr^[5] = IMSEpiState;

	// // kate 1/5/12 other parameters that need to be initialized here
	// these will go in fInitialVals again
	// kate 2/1/12 always write out the integer number of parameters first that we are returning here.!!

	// kate 4/2/12
	//  numVals = 18; // could make this number a constant at the start of the file // kate 5/31/12 not needed here

	pFloatOutputNetworkArr  = PFloatNetworkOutputArray(SystemInputs.NetworkOutputs);
	//  pFloatOutputNetworkArr^[0]   = numVals;         // an integer count    // kate 5/31/12 not needed
	pFloatOutputNetworkArr^[{1}0]   = localUsedP;      // fInitialVals 1
	pFloatOutputNetworkArr^[{2}1]   = localUsedQ;      // fInitialVals 2
	pFloatOutputNetworkArr^[{3}2]   = IML_Tnom;        // fInitialVals 3
	pFloatOutputNetworkArr^[{4}3]   = IMS_Tnom;        // fInitialVals 4
	pFloatOutputNetworkArr^[{5}4]   = IML_fWo;         // fInitialVals 5
	pFloatOutputNetworkArr^[{6}5]   = IMS_fWo;         // fInitialVals 6
	pFloatOutputNetworkArr^[{7}6]   = IMLfStateIr_In;  // fInitialVals 7
	pFloatOutputNetworkArr^[{8}7]   = IMLfStateIi_In;  // fInitialVals 8
	pFloatOutputNetworkArr^[{9}8]   = IMSfStateIr_In;  // fInitialVals 9
	pFloatOutputNetworkArr^[{10}9]  = IMSfStateIi_In;  // fInitialVals 10
	pFloatOutputNetworkArr^[{11}10]  = IMLfInvTp;       // fInitialVals 11
	pFloatOutputNetworkArr^[{12}11]  = IMSfInvTp;       // fInitialVals 12
	pFloatOutputNetworkArr^[{13}12]  = IML_Xp;          // fInitialVals 13
	pFloatOutputNetworkArr^[{14}13]  = IMS_Xp;          // fInitialVals 14
	pFloatOutputNetworkArr^[{15}14]  = IML_X;           // fInitialVals 15
	pFloatOutputNetworkArr^[{16}15]  = IMS_X;           // fInitialVals 16
	pFloatOutputNetworkArr^[{17}16]  = IML_fPInitial;   // fInitialVals 17
	pFloatOutputNetworkArr^[{18}17]  = IMS_fPInitial;   // fInitialVals 18


    */


}

PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	// NOT FILLED IN YET
}
/*
PW_UDM_DllExport(void) calculateAlgebraic(TTxMyModelData* ParamsAndStates, TTxInputSignals* SystemInputs)
{
	// NOT FILLED IN YET
}
*/

PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
    return(N_NONWINDUPLIMITS);
}

PW_UDM_DllExport(int) numberOfSubIntervals()
{
	return(0);
}


PW_UDM_DllExport(int) modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
	if ((*StrSize) >= int(strlen("UserDefinedLoadModel")))
	{
		wcsncpy_s(StrBuf, *StrSize+1, L"UserDefinedLoadModel", *StrSize);
	}
	return(int(strlen("UserDefinedLoadModel")));
}

bool PWDComplex::EqualTo(PWDComplex NewNumber)
{
	if ((NewNumber.r == r) & (NewNumber.i == i)) return(1);
	else return(0);
}

bool PWDComplex::IsZero()
{
	if ((r != 0) | (i != 0)) return(0);
	else return(1);
}

void PWDComplex::Init(double tr, double ti)
{
	r = tr;
	i = ti;
}

PWDComplex PWDComplex::Inc(double tr, double ti)
{
	PWDComplex temp;
	temp.r = r + tr;
	temp.i = i + ti;
	return(temp);
}

void PWDComplex::Self_Inc(double tr, double ti)
{
	r = r + tr;
	i = i + ti;
}

PWDComplex PWDComplex::CInc(PWDComplex tempCom)
{
	PWDComplex temp;
	temp.r = r + tempCom.r;
	temp.i = i + tempCom.i;
	return(temp);
}

PWDComplex PWDComplex::Dec(double tr, double ti)
{
	PWDComplex temp;
	temp.r = r - tr;
	temp.i = i - ti;
	return(temp);
}

PWDComplex PWDComplex::CDec(PWDComplex tempCom)
{
	PWDComplex temp;
	temp.r = r - tempCom.r;
	temp.i = i - tempCom.i;
	return(temp);
}

PWDComplex PWDComplex::Scale(double t)
{
	PWDComplex temp;
	temp.r = r*t;
	temp.i = i*t;
	return(temp);
}

void PWDComplex::Self_Scale(double t)
{
	r = r*t;
	i = i*t;
}

PWDComplex PWDComplex::ScaleJ(double t)
{
	PWDComplex temp;
	temp.r = -i*t;
	temp.i = r*t;
	return(temp);
}

double PWDComplex::Mag()
{
	return(sqrt(pow(r, 2) + pow(i, 2)));
}

double PWDComplex::MagSquared()
{
	return(pow(r, 2) + pow(i, 2));
}

double PWDComplex::Angle()
{
	return(atan2(i, r));
}

PWDComplex PWDComplex::Multiply(PWDComplex aValue)
{
	PWDComplex temp;
	temp.r = (r*aValue.r - i*aValue.i);
	temp.i = (i*aValue.r + r*aValue.i);
	return(temp);
}

PWDComplex PWDComplex::Divide(PWDComplex aValue)
{
	PWDComplex temp;
	double t = 1/(pow(r, 2) + pow(i, 2));
	temp.r = (r*aValue.r + i*aValue.i)*t;
	temp.i = (-r*aValue.i + i*aValue.r)*t;
	return(temp);
}

PWDComplex PWDComplex::SquareRoot()
{
	PWDComplex temp;
	double A, B;
	A = sqrt(Mag());
	B = atan2(i,r);
    temp.r = A*cos(B/2);
    temp.i = A*sin(B/2);
	return(temp);
}

PWDComplex PWDComplex::Invert()
{
	PWDComplex temp;
	double t = pow(r, 2) + pow(i, 2);
	temp.r = r/t;
	temp.i = i/t;
	return(temp);
}

PWDComplex PWDComplex::Conjugate()
{
	PWDComplex temp;
	temp.r = r;
	temp.i = -i;
	return(temp);
}

void PWDComplex::Self_Conjugate()
{
	i = -i;
}

PWDComplex PWDComplex::Negate()
{
	PWDComplex temp;
	temp.r = -r;
	temp.i = -i;
	return(temp);
}

double PWDComplex::DotProduct(PWDComplex q1)
{
	return(r*q1.r + i*q1.i);
}

PWDComplex PWDComplex::Square()
{
	PWDComplex temp;
	temp.r = pow(r, 2) - pow(i, 2);
	temp.i = 2*r*i;
	return(temp);
}

PWDComplex PWDComplex::Power(double aExponent)
{
	PWDComplex temp;
	double local_Mag, local_Angle;
	local_Mag = pow(Mag(), aExponent);
	local_Angle = Angle()*aExponent;
	temp.r = local_Mag*cos(local_Angle);
	temp.i = local_Mag*sin(local_Angle);
	return(temp);
}

PWDComplex PWDComplex::ShiftAngle(double angleBaseValue)
{
	PWDComplex temp;
	double local_Mag, local_Angle;
	local_Angle = Angle() - angleBaseValue;
	local_Mag = Mag();
	temp.r = local_Mag*cos(local_Angle);
	temp.i = local_Mag*sin(local_Angle);
	return(temp);
}

PWDComplex PWDComplex::ShiftAngleBack(double angleBaseValue)
{
	PWDComplex temp;
	double local_Mag, local_Angle;
	local_Angle = Angle() + angleBaseValue;
	local_Mag = Mag();
	temp.r = local_Mag*cos(local_Angle);
	temp.i = local_Mag*sin(local_Angle);
	return(temp);
}

bool DoubleToBoolean(double thedoub)
{
	if (floor(thedoub + 0.5) == 0)
	{
		return(0);
	}
	else
	{
		return(1);
	}
}

PWDComplex PWDComplex_InitPolarRad(double tmag, double tangle)
{
	PWDComplex temp;
	temp.r = tmag*cos(tangle);
	temp.i = tmag*sin(tangle);
	return(temp);
}

PWDComplex PWDComplex_Init(double r, double i)
{
	PWDComplex temp;
	temp.r = r;
	temp.i = i;
	return(temp);
}