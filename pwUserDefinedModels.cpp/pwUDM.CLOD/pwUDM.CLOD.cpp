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


PW_UDM_DllExport(int) modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
    enumModelClass udmClass;
    wchar_t* str;
    int stringLength;

    udmClass = mcnLoad;

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

    // Double parameters
    switch (*ParamNum)
    {
        case PARAM_PercLmotor: str = L"%Lmotor";
            break;
        case PARAM_PercSmotor: str = L"%Smotor";
            break;
        case PARAM_PercTex: str = L"%Tex";
            break;
        case PARAM_PercDis: str = L"%Dis";
            break;
        case PARAM_PercP: str = L"%P";
            break;
        case PARAM_Kp: str = L"Kp";
            break;
        case PARAM_Vi: str = L"Low V Trip Pickup (pu)";
            break;
        case PARAM_Ti: str = L"Low V Trip Timer (sec)";
            break;
        case PARAM_Tb: str = L"Low V Trip Breaker Delay (sec)";
            break;
        default: str = L"";
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


PW_UDM_DllExport(void) getDefaultParameterValues(TTxMyModelData* ParamsAndStates)
{
    // only non-zero values will work for this DLL
    (*((*ParamsAndStates).FloatParams))[PARAM_PercLmotor] = 25.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_PercSmotor] = 25.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_PercTex] = 0.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_PercDis] = 20.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_PercP] = 0.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Kp] = 1.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Vi] = 0.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Ti] = 0.0;
    (*((*ParamsAndStates).FloatParams))[PARAM_Tb] = 0.05;
}


PW_UDM_DllExport(int) SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds)
{
    return(0);
}


PW_UDM_DllExport(void) initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    // NOT FILLED IN YET
}


PW_UDM_DllExport(void) calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
	// NOT FILLED IN YET
}


PW_UDM_DllExport(void) PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    // NOT FILLED IN YET
}


PW_UDM_DllExport(int) getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
    return(N_NONWINDUPLIMITS);
}

PW_UDM_DllExport(bool) TimeStepEnd(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex)
{
    // NOT FILLED IN YET
    return(0);
}


PW_UDM_DllExport(int) TimeStepEndAction(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* StrSize, wchar_t* StrBuf, int dummy)
{
    // NOT FILLED IN YET
    return(0);
}


PW_UDM_DllExport(double) LoadNortonAdmittance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theG, double* theB)
{
    // NOT FILLED IN YET
    return(0);
}


PW_UDM_DllExport(double) LoadNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag)
{
    // NOT FILLED IN YET
    return(0);
}


PW_UDM_DllExport(double) LoadNortonCurrentAlgebraicDerivative(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* dIr_de, double* dIr_df, double* dIi_de, double* dIi_df)
{
    // NOT FILLED IN YET
    return(0);
}


PW_UDM_DllExport(bool) LoadInitializeAlgebraic(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* thePUTol, double* P, double* Q, double* V, double* UsedP, double* UsedQ)
{
    // NOT FILLED IN YET
    return(0);
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