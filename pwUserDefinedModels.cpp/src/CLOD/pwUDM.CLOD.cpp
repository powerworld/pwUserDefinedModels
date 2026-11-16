module pwUDM.CLOD;

import std;
import pwUDM;
import pwUDM.Load;

namespace {

class PWDComplex
{
public:
	double r;
	double i;
	bool EqualTo(PWDComplex NewNumber);
	bool IsZero();
	void Init(double tr, double ti);
	PWDComplex Inc(double tr, double ti);
	void Self_Inc(double tr, double ti);
	PWDComplex CInc(PWDComplex tempCom);
	PWDComplex Dec(double tr, double ti);
	PWDComplex CDec(PWDComplex tempCom);
	PWDComplex Scale(double t);
	void Self_Scale(double t);
	PWDComplex ScaleJ(double t);
	double Mag();
	double MagSquared();
	double Angle();
	PWDComplex Multiply(PWDComplex aValue);
	PWDComplex Divide(PWDComplex aValue);
	PWDComplex SquareRoot();
	PWDComplex Invert();
	PWDComplex Conjugate();
	void Self_Conjugate();
	PWDComplex Negate();
	double DotProduct(PWDComplex q1);
	PWDComplex Square();
	PWDComplex Power(double aExponent);
	PWDComplex ShiftAngle(double angleBaseValue);
	PWDComplex ShiftAngleBack(double angleBaseValue);
};

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
	return(std::sqrt(r*r + i*i));
}

double PWDComplex::MagSquared()
{
	return(std::pow(r, 2) + std::pow(i, 2));
}

double PWDComplex::Angle()
{
	return(std::atan2(i, r));
}

PWDComplex PWDComplex::Multiply(PWDComplex aValue)
{
	PWDComplex temp;
	temp.r = (r*aValue.r - i*aValue.i);
	temp.i = (i*aValue.r + r*aValue.i);
	return(temp);
}

// Returns self divided by aValue.
PWDComplex PWDComplex::Divide(PWDComplex aValue)
{
	PWDComplex temp;
	double t = 1/(aValue.r*aValue.r + aValue.i*aValue.i);
	temp.r = (r*aValue.r + i*aValue.i)*t;
	temp.i = (-r*aValue.i + i*aValue.r)*t;
	return(temp);
}

PWDComplex PWDComplex::SquareRoot()
{
	PWDComplex temp;
	double A, B;
	A = std::sqrt(Mag());
	B = std::atan2(i,r);
    temp.r = A*std::cos(B/2);
    temp.i = A*std::sin(B/2);
	return(temp);
}

PWDComplex PWDComplex::Invert()
{
	PWDComplex temp;
	double t = std::pow(r, 2) + std::pow(i, 2);
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
	temp.r = std::pow(r, 2) - std::pow(i, 2);
	temp.i = 2*r*i;
	return(temp);
}

PWDComplex PWDComplex::Power(double aExponent)
{
	PWDComplex temp;
	double local_Mag, local_Angle;
	local_Mag = std::pow(Mag(), aExponent);
	local_Angle = Angle()*aExponent;
	temp.r = local_Mag*std::cos(local_Angle);
	temp.i = local_Mag*std::sin(local_Angle);
	return(temp);
}

PWDComplex PWDComplex::ShiftAngle(double angleBaseValue)
{
	PWDComplex temp;
	double local_Mag, local_Angle;
	local_Angle = Angle() - angleBaseValue;
	local_Mag = Mag();
	temp.r = local_Mag*std::cos(local_Angle);
	temp.i = local_Mag*std::sin(local_Angle);
	return(temp);
}

PWDComplex PWDComplex::ShiftAngleBack(double angleBaseValue)
{
	PWDComplex temp;
	double local_Mag, local_Angle;
	local_Angle = Angle() + angleBaseValue;
	local_Mag = Mag();
	temp.r = local_Mag*std::cos(local_Angle);
	temp.i = local_Mag*std::sin(local_Angle);
	return(temp);
}

[[maybe_unused]] bool DoubleToBoolean(double thedoub)
{
	if (std::floor(thedoub + 0.5) == 0)
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
	temp.r = tmag*std::cos(tangle);
	temp.i = tmag*std::sin(tangle);
	return(temp);
}

PWDComplex PWDComplex_Init(double r, double i)
{
	PWDComplex temp;
	temp.r = r;
	temp.i = i;
	return(temp);
}

}

namespace pwUDM::CLOD {

namespace {

constexpr double Pi = std::numbers::pi;
constexpr double TinyVoltageSquared = 1e-5*1e-5;   // sqr(1E-5) in the Delphi source

double Sqr(double x)
{
    return(x*x);
}

// Delphi Math_Y_to_X: base raised to exponent, with exact shortcuts for small whole exponents.
double MathYToX(double base, double exponent)
{
    if (base == 0) return(0);
    if (exponent == 0) return(1);
    if (exponent == 1) return(base);
    if (exponent == 2) return(Sqr(base));
    if (exponent == 3) return(base*Sqr(base));
    if (exponent == 4) return(Sqr(base)*Sqr(base));
    return(std::exp(exponent*std::log(base)));
}

PWDComplex Parallel(PWDComplex complex1, PWDComplex complex2)
{
    PWDComplex numerator = complex1.Multiply(complex2);
    PWDComplex denominator = PWDComplex_Init(complex1.r + complex2.r, complex1.i + complex2.i);
    return(numerator.Divide(denominator));
}

// ----------------------------------------------------------------------------------------------
// Induction motor equivalent circuit. The Delphi source declares these values as Single, so they
// are floats here too.
// ----------------------------------------------------------------------------------------------
struct MotorCircuit
{
    float rs = 0;
    float xs = 0;
    float xm = 0;
    float r1 = 0;
    float x1 = 0;
    float busVoltageMag = 0;
    PWDComplex impedanceSystem = { 0, 0 };
};

struct SlipSolution
{
    PWDComplex iStator = { 0, 0 };
    PWDComplex eInternal = { 0, 0 };
    double totalP = 0;
    double totalQ = 0;
    double powerFactor = 0;
    double telec = 0;
};

SlipSolution CalculateAtSlip(const MotorCircuit& motor, double slip)
{
    SlipSolution out;
    PWDComplex zM = PWDComplex_Init(0, motor.xm);
    PWDComplex statorZ = PWDComplex_Init(motor.rs, motor.xs);
    PWDComplex vTerminal = PWDComplex_Init(motor.busVoltageMag, 0);
    PWDComplex zTotal;
    if (slip != 0)
    {
        PWDComplex zRotor = PWDComplex_Init(motor.r1/slip, motor.x1);
        zTotal = Parallel(zRotor, zM).CInc(statorZ);
    }
    else
    {
        zTotal = statorZ.CInc(zM);
    }
    out.iStator = vTerminal.Divide(zTotal);
    out.totalP = vTerminal.r*out.iStator.r;   // the imaginary part of the terminal voltage is always zero
    out.totalQ = -vTerminal.r*out.iStator.i;
    out.powerFactor = out.totalP/std::sqrt(Sqr(out.totalP) + Sqr(out.totalQ));
    out.eInternal = vTerminal.CDec(out.iStator.Multiply(motor.impedanceSystem));
    out.telec = out.eInternal.r*out.iStator.r + out.eInternal.i*out.iStator.i;
    return(out);
}

double PowerAtSlip(const MotorCircuit& motor, double slip)
{
    return(CalculateAtSlip(motor, slip).totalP);
}

// Bisects between slipL and slipH until the power mismatch is below aTol or 20 passes have been made.
double BisectSlip(const MotorCircuit& motor, double aPower, double aTol, double& slipL, double& slipH, int& passes)
{
    double slipM;
    double pMid;
    passes = 0;
    do
    {
        slipM = (slipH + slipL)*0.5;
        pMid = PowerAtSlip(motor, slipM);
        if (pMid - aPower > 0) slipH = slipM;
        else slipL = slipM;
        ++passes;
    } while (!((std::abs(pMid - aPower) < aTol) || (passes > 20)));
    return(slipM);
}

void SetReactivePowerAndTorque(const MotorCircuit& motor, double slip, double& initialQPU, double& telec)
{
    SlipSolution solution = CalculateAtSlip(motor, slip);
    initialQPU = solution.totalQ;
    telec = solution.telec;
}

// Delphi TxInductionMachine_CalculateInitialSlip. Returns 0 if optimal, 1 if P is too high, 2 if P is too low.
// Assumes at most three slips solve for P and returns the one closest to zero slip.
unsigned char CalculateInitialSlip(const MotorCircuit& motor, double aPower, double aTol, double& theSlip, double& initialQPU, double& theTElec)
{
    unsigned char result = 0;
    double slipL = 0;
    double slipH;
    double deltaSlip = 0.02;
    int passes;
    initialQPU = 0;
    theTElec = 0;

    // The unlikely case where the power is below what the stator losses alone draw
    double pLow = PowerAtSlip(motor, slipL);
    if (pLow >= aPower)
    {
        if (pLow > aPower) result = 2;
        theSlip = 0;
        return(result);
    }

    // Bracket the slip. If pHigh is above aPower then it is bracketed.
    slipH = deltaSlip;
    double pHigh = PowerAtSlip(motor, slipH);
    while ((pHigh - aPower <= 0) && (pHigh > pLow) && (slipH <= 1))
    {
        pLow = pHigh;
        slipH = slipH + deltaSlip;
        pHigh = PowerAtSlip(motor, slipH);
        if (slipH >= 0.1) deltaSlip = 0.05;
    }

    if (pHigh - aPower > 0)
    {
        double slipM = BisectSlip(motor, aPower, aTol, slipL, slipH, passes);
        if (passes > 20)
        {
            result = 1;   // this shouldn't occur
        }
        else
        {
            theSlip = slipM;
            SetReactivePowerAndTorque(motor, theSlip, initialQPU, theTElec);
        }
        return(result);
    }

    if ((slipH > 1) && (pHigh > pLow))
    {
        // Power keeps increasing and there is not enough of it
        theSlip = 1;
        SetReactivePowerAndTorque(motor, theSlip, initialQPU, theTElec);
        return(1);
    }

    // The motor is operating close to stalling. Back up below the maximum and step up again until above aPower.
    slipL = slipL - deltaSlip;
    if (slipL < 0) slipL = 0;
    theSlip = slipH;   // temporary storage of the maximum value to check
    pLow = PowerAtSlip(motor, slipL);
    slipH = slipL + deltaSlip/10;
    pHigh = PowerAtSlip(motor, slipH);
    while ((slipH < theSlip) && (pHigh - aPower < 0) && (pHigh > pLow))
    {
        pLow = pHigh;
        slipL = slipH;
        slipH = slipL + deltaSlip/10;
        pHigh = PowerAtSlip(motor, slipH);
    }
    if (pHigh - aPower >= 0)
    {
        double slipM = BisectSlip(motor, aPower, aTol, slipL, slipH, passes);
        theSlip = slipM;
        if (passes > 20) result = 1;   // shouldn't occur
        else SetReactivePowerAndTorque(motor, theSlip, initialQPU, theTElec);
        return(result);
    }
    theSlip = slipH;
    return(1);
}

// ----------------------------------------------------------------------------------------------
// Newton solution for the initial slip and internal voltage of one induction motor. Vectors and
// matrices are indexed from 1, exactly as in the Delphi source.
// ----------------------------------------------------------------------------------------------
constexpr int MatSize = 3;
using Vector3 = double[MatSize + 1];
using Matrix3 = double[MatSize + 1][MatSize + 1];
using IntVector3 = int[MatSize + 1];

double VectorNorm2(const Vector3& theVector)
{
    double result = 0;
    for (int i = 1; i <= MatSize; ++i)
    {
        result = result + Sqr(theVector[i]);
    }
    return(std::sqrt(result));
}

void MatrixZeroOut(Matrix3& theMatrix)
{
    for (int theRow = 1; theRow <= MatSize; ++theRow)
    {
        for (int theCol = 1; theCol <= MatSize; ++theCol)
        {
            theMatrix[theRow][theCol] = 0;
        }
    }
}

// LU factorization with row pivoting.
void MatrixFactor(Matrix3& theMatrix, IntVector3& rowPerm)
{
    int imax = 0;
    double sum;
    double dum;
    double big;
    Vector3 vv;

    for (int i = 1; i <= MatSize; ++i)
    {
        rowPerm[i] = i;
        big = 0.0;
        for (int j = 1; j <= MatSize; ++j)
        {
            if (std::abs(theMatrix[i][j]) > big) big = std::abs(theMatrix[i][j]);
        }
        vv[i] = 1.0/big;
    }

    for (int j = 1; j <= MatSize; ++j)
    {
        for (int i = 1; i <= j - 1; ++i)   // upper triangular elements, except the diagonal
        {
            sum = theMatrix[i][j];
            for (int k = 1; k <= i - 1; ++k) sum = sum - theMatrix[i][k]*theMatrix[k][j];
            theMatrix[i][j] = sum;
        }
        big = 0.0;
        for (int i = j; i <= MatSize; ++i)   // diagonal element: pick the remaining row with the largest value
        {
            sum = theMatrix[i][j];
            for (int k = 1; k <= j - 1; ++k) sum = sum - theMatrix[i][k]*theMatrix[k][j];
            theMatrix[i][j] = sum;
            dum = vv[i]*std::abs(sum);
            if (dum >= big)
            {
                big = dum;
                imax = i;
            }
        }
        if (j != imax)   // a pivot occurred, so interchange the rows
        {
            for (int k = 1; k <= MatSize; ++k)
            {
                dum = theMatrix[imax][k];
                theMatrix[imax][k] = theMatrix[j][k];
                theMatrix[j][k] = dum;
            }
            vv[imax] = vv[j];
        }
        rowPerm[j] = imax;
        if (j != MatSize)   // divide by the pivot element
        {
            dum = 1.0/theMatrix[j][j];
            for (int i = j + 1; i <= MatSize; ++i) theMatrix[i][j] = theMatrix[i][j]*dum;
        }
    }
}

void MatrixForwardBackward(const Matrix3& theMatrix, Vector3& theVector, const IntVector3& rowPerm)
{
    int ii = 0;
    double sum;
    for (int i = 1; i <= MatSize; ++i)
    {
        int ip = rowPerm[i];
        sum = theVector[ip];
        theVector[ip] = theVector[i];
        if (ii != 0)
        {
            for (int j = ii; j <= i - 1; ++j) sum = sum - theMatrix[i][j]*theVector[j];
        }
        else if (sum != 0.0)
        {
            ii = i;
        }
        theVector[i] = sum;
    }
    for (int i = MatSize; i >= 1; --i)
    {
        sum = theVector[i];
        for (int j = i + 1; j <= MatSize; ++j) sum = sum - theMatrix[i][j]*theVector[j];
        theVector[i] = sum/theMatrix[i][i];
    }
}

struct InductionSolve
{
    double wo = 0;
    double pInit = 0;
    double invTp = 0;
    double c1 = 0;
    double vr = 0;
    double vi = 0;
    double g = 0;
    double b = 0;
    double slip = 0;
    double erp = 0;
    double eip = 0;
    double id = 0;
    double iq = 0;
};

// Sets slip, internal voltage and current from the unknowns, and the mismatch of the three equations.
void SetMismatch(InductionSolve& s, const Vector3& x, Vector3& mismatch)
{
    s.slip = x[1];
    s.erp = x[2];
    s.eip = x[3];
    s.id = (s.erp - s.vr)*s.g - (s.eip - s.vi)*s.b;
    s.iq = (s.eip - s.vi)*s.g + (s.erp - s.vr)*s.b;
    mismatch[1] = s.vr*s.id + s.vi*s.iq - s.pInit;   // real power balance
    mismatch[2] = s.wo*s.slip*s.eip - s.invTp*(s.erp + s.c1*(-s.iq));
    mismatch[3] = -s.wo*s.slip*s.erp - s.invTp*(s.eip + s.c1*(s.id));
}

// Delphi TxInductionMachine_GetInitialConditions. slip, erp and eip are the starting guess on entry and the
// solution on exit. Returns true if it converged.
bool InductionInitialConditions(double wo, double pInit, double ra, double x, double xp, double invTp, PWDComplex termVolt,
                                int maxItr, double itrTol, double& slip, double& erp, double& eip, double& id, double& iq, double& qInit)
{
    InductionSolve s;
    s.wo = wo;
    s.pInit = pInit;
    s.invTp = invTp;
    s.vr = termVolt.r;
    s.vi = termVolt.i;
    s.c1 = x - xp;
    double t = Sqr(ra) + Sqr(xp);
    s.g = ra/t;
    s.b = -xp/t;

    Vector3 theX = { 0, slip, erp, eip };
    Vector3 theB = { 0, 0, 0, 0 };
    SetMismatch(s, theX, theB);

    // Partials of the currents with respect to the internal voltages
    double dIdErp = s.g;
    double dIdEip = -s.b;
    double dIqErp = s.b;
    double dIqEip = s.g;
    int itr = 0;
    while ((VectorNorm2(theB) > itrTol) && (itr < maxItr))
    {
        Matrix3 theMatrix;
        IntVector3 rowPerm = { 0, 0, 0, 0 };
        MatrixZeroOut(theMatrix);
        theMatrix[1][2] = s.vr*s.g + s.vi*s.b;
        theMatrix[1][3] = -s.vr*s.b + s.vi*s.g;
        theMatrix[2][1] = s.wo*s.eip;
        theMatrix[3][1] = -s.wo*s.erp;
        theMatrix[2][2] = -s.invTp*(1 + s.c1*(-1)*dIqErp);
        theMatrix[2][3] = s.wo*s.slip - s.invTp*s.c1*(-1)*dIqEip;
        theMatrix[3][2] = -s.wo*s.slip - s.invTp*s.c1*(1)*dIdErp;
        theMatrix[3][3] = -s.invTp*(1 + s.c1*(1)*dIdEip);
        MatrixFactor(theMatrix, rowPerm);
        MatrixForwardBackward(theMatrix, theB, rowPerm);
        for (int i = 1; i <= MatSize; ++i) theX[i] = theX[i] - theB[i];
        SetMismatch(s, theX, theB);
        ++itr;
    }

    slip = s.slip;
    erp = s.erp;
    eip = s.eip;
    id = s.id;
    iq = s.iq;
    qInit = -s.vr*s.iq + s.vi*s.id;   // all the other results were set by SetMismatch
    return(itr < maxItr);
}

}

extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
	return(1);
}


extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
    ModelClass udmClass;
    const wchar_t* str;
    int stringLength;

    udmClass = ModelClass::Load;

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


extern "C" __declspec(dllexport) int __stdcall stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
	const wchar_t* str;
	int stringLength;
	
	if (*StateNum == 0) {
		str = L"IM 1 Speed wr";
	}
	else if (*StateNum == 1) {
		str = L"IM 1 Epr";
	}
	else if (*StateNum == 2) {
		str = L"IM 1 Epi";
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


extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;

    // only non-zero values will work for this DLL
    floatParams[PARAM_PercLmotor] = 25.0;
    floatParams[PARAM_PercSmotor] = 25.0;
    floatParams[PARAM_PercTex] = 0.0;
    floatParams[PARAM_PercDis] = 20.0;
    floatParams[PARAM_PercP] = 0.0;
    floatParams[PARAM_Kp] = 1.0;
    floatParams[PARAM_Vi] = 0.0;
    floatParams[PARAM_Ti] = 0.0;
    floatParams[PARAM_Tb] = 0.05;
}


extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds)
{
    return(0);
}


namespace {

// The large motor algebraics (ALG_IML_*) and the small motor algebraics (ALG_IMS_*) share one layout.
constexpr int OFFSET_Rs = 0;
constexpr int OFFSET_Xs = 1;
constexpr int OFFSET_fWo = 2;
constexpr int OFFSET_fInvTp = 3;
constexpr int OFFSET_Xp = 4;
constexpr int OFFSET_X = 5;
constexpr int OFFSET_SlipInitial = 6;
constexpr int OFFSET_fPInitial = 7;
constexpr int OFFSET_fQInitial = 8;
constexpr int OFFSET_Tnom = 9;

static_assert((ALG_IML_Xs == ALG_IML_Rs + OFFSET_Xs) && (ALG_IML_fWo == ALG_IML_Rs + OFFSET_fWo) && (ALG_IMLfInvTp == ALG_IML_Rs + OFFSET_fInvTp)
    && (ALG_IML_Xp == ALG_IML_Rs + OFFSET_Xp) && (ALG_IML_X == ALG_IML_Rs + OFFSET_X) && (ALG_IML_SlipInitial == ALG_IML_Rs + OFFSET_SlipInitial)
    && (ALG_IML_fPInitial == ALG_IML_Rs + OFFSET_fPInitial) && (ALG_IML_fQInitial == ALG_IML_Rs + OFFSET_fQInitial) && (ALG_IML_Tnom == ALG_IML_Rs + OFFSET_Tnom),
    "large motor algebraics must follow the shared layout");
static_assert((ALG_IMS_Xs == ALG_IMS_Rs + OFFSET_Xs) && (ALG_IMS_fWo == ALG_IMS_Rs + OFFSET_fWo) && (ALG_IMSfInvTp == ALG_IMS_Rs + OFFSET_fInvTp)
    && (ALG_IMS_Xp == ALG_IMS_Rs + OFFSET_Xp) && (ALG_IMS_X == ALG_IMS_Rs + OFFSET_X) && (ALG_IMS_SlipInitial == ALG_IMS_Rs + OFFSET_SlipInitial)
    && (ALG_IMS_fPInitial == ALG_IMS_Rs + OFFSET_fPInitial) && (ALG_IMS_fQInitial == ALG_IMS_Rs + OFFSET_fQInitial) && (ALG_IMS_Tnom == ALG_IMS_Rs + OFFSET_Tnom),
    "small motor algebraics must follow the shared layout");

struct MotorAlgebraics
{
    double rs = 0;
    double xs = 0;
    double wo = 0;
    double invTp = 0;
    double xp = 0;
    double x = 0;
    double slipInitial = 0;
    double pInitial = 0;
    double qInitial = 0;
    double tnom = 0;
};

// Speed and the real and imaginary internal voltage of one motor, or their derivatives.
struct MotorTriple
{
    double speed = 0;
    double epr = 0;
    double epi = 0;
};

MotorAlgebraics ReadMotorAlgebraics(const TDoubleArray& algebraics, int firstIndex)
{
    MotorAlgebraics m;
    m.rs = algebraics[firstIndex + OFFSET_Rs];
    m.xs = algebraics[firstIndex + OFFSET_Xs];
    m.wo = algebraics[firstIndex + OFFSET_fWo];
    m.invTp = algebraics[firstIndex + OFFSET_fInvTp];
    m.xp = algebraics[firstIndex + OFFSET_Xp];
    m.x = algebraics[firstIndex + OFFSET_X];
    m.slipInitial = algebraics[firstIndex + OFFSET_SlipInitial];
    m.pInitial = algebraics[firstIndex + OFFSET_fPInitial];
    m.qInitial = algebraics[firstIndex + OFFSET_fQInitial];
    m.tnom = algebraics[firstIndex + OFFSET_Tnom];
    return(m);
}

// Returns the initial speed and internal voltage in state, and the nominal torque as the result.
double InitializeMotor(PWDComplex busVolt, const MotorAlgebraics& m, MotorTriple& state)
{
    PWDComplex localCurrent = PWDComplex_Init(m.pInitial, m.qInitial).Divide(busVolt);
    double stateIr = localCurrent.r;
    double stateIi = -localCurrent.i;
    PWDComplex currentIn = PWDComplex_Init(stateIr, stateIi);
    PWDComplex impedanceSystem = PWDComplex_Init(m.rs, m.xp);
    // The initial internal voltage, in the network reference frame
    PWDComplex localEp = busVolt.CDec(currentIn.Multiply(impedanceSystem));

    double t3 = -stateIr;
    double t4 = -stateIi;
    double t5 = -m.pInitial;
    double qInit = 0;
    double slip = m.slipInitial;
    InductionInitialConditions(m.wo, t5, m.rs, m.x, m.xp, m.invTp, busVolt, 10, 1e-5, slip, localEp.r, localEp.i, t3, t4, qInit);
    stateIr = -t3;
    stateIi = -t4;

    state.speed = 1 - slip;
    state.epr = localEp.r;
    state.epi = localEp.i;

    double electricalTorque = state.epr*stateIr + state.epi*stateIi;
    double damping = 2;   // same as fDamp
    return(electricalTorque/MathYToX(1 - slip, damping));
}

}

extern "C" __declspec(dllexport) void __stdcall initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    double loadObjV_In = hardCodedSignals[HARDCODE_LOAD_DeviceVPU];
    double loadObjThetaRadian_In = hardCodedSignals[HARDCODE_LOAD_DeviceAngleRad];
    bool inService_In = hardCodedSignals[HARDCODE_LOAD_DeviceStatus] > 0.5;
    PWDComplex loadObjBusVolt_In = PWDComplex_InitPolarRad(loadObjV_In, loadObjThetaRadian_In);

    // These were all initialized in the LoadInitializeAlgebraic call, so they are just copied over here
    MotorAlgebraics largeMotor = ReadMotorAlgebraics(algebraics, ALG_IML_Rs);
    MotorAlgebraics smallMotor = ReadMotorAlgebraics(algebraics, ALG_IMS_Rs);
    MotorTriple largeState;
    MotorTriple smallState;
    double largeTnom = 0;
    double smallTnom = 0;

    if (inService_In)
    {
        largeTnom = InitializeMotor(loadObjBusVolt_In, largeMotor, largeState);
        smallTnom = InitializeMotor(loadObjBusVolt_In, smallMotor, smallState);
    }
    // If out-of-service then everything stays at zero

    states[STATE_IM1SpeedWr] = largeState.speed;
    states[STATE_IM1Epr] = largeState.epr;
    states[STATE_IM1Epi] = largeState.epi;
    states[STATE_IM2SpeedWr] = smallState.speed;
    states[STATE_IM2Epr] = smallState.epr;
    states[STATE_IM2Epi] = smallState.epi;

    // These values just need to be passed around
    algebraics[ALG_IML_Tnom] = largeTnom;
    algebraics[ALG_IMS_Tnom] = smallTnom;
}


namespace {

MotorTriple MotorDerivatives(PWDComplex busVolt, bool inService, double loadScalar, const MotorAlgebraics& m,
                             double damping, double inertiaH, const MotorTriple& state)
{
    MotorTriple dot;
    PWDComplex internalVolt = PWDComplex_Init(state.epr, state.epi);
    PWDComplex impedanceSystem = PWDComplex_Init(m.rs, m.xp);
    internalVolt = busVolt.CDec(internalVolt);
    PWDComplex stateCurrent = internalVolt.Divide(impedanceSystem);
    double stateIr = stateCurrent.r;
    double stateIi = stateCurrent.i;

    double electricalTorque = state.epr*stateIr + state.epi*stateIi;
    double mechanicalTorque = 0;
    if (inService) mechanicalTorque = MathYToX(state.speed, damping)*m.tnom;
    double inv2H = (1/(2*inertiaH))*(1/m.pInitial);
    dot.speed = inv2H*(electricalTorque - mechanicalTorque);

    double stateSlip = 1 - state.speed;
    dot.epr = (stateSlip*state.epi*m.wo - m.invTp*(state.epr + (m.x - m.xp)*loadScalar*stateIi));
    dot.epi = (-stateSlip*state.epr*m.wo - m.invTp*(state.epi - (m.x - m.xp)*loadScalar*stateIr));
    return(dot);
}

}

extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    MotorTriple largeState = { states[STATE_IM1SpeedWr], states[STATE_IM1Epr], states[STATE_IM1Epi] };
    MotorTriple smallState = { states[STATE_IM2SpeedWr], states[STATE_IM2Epr], states[STATE_IM2Epi] };

    bool inService_In = hardCodedSignals[HARDCODE_LOAD_DeviceStatus] > 0.5;
    double loadScalar_in = hardCodedSignals[HARDCODE_LOAD_LoadScalar];

    MotorAlgebraics largeMotor = ReadMotorAlgebraics(algebraics, ALG_IML_Rs);
    MotorAlgebraics smallMotor = ReadMotorAlgebraics(algebraics, ALG_IMS_Rs);

    // The network voltage presently
    double busVoltMag = hardCodedSignals[HARDCODE_LOAD_DeviceVPU];
    double loadObjThetaRadian_In = hardCodedSignals[HARDCODE_LOAD_DeviceAngleRad];
    PWDComplex loadObjBusVolt_In = PWDComplex_InitPolarRad(busVoltMag, loadObjThetaRadian_In);

    MotorTriple largeDot = MotorDerivatives(loadObjBusVolt_In, inService_In, loadScalar_in, largeMotor, 2, 1.5, largeState);
    MotorTriple smallDot = MotorDerivatives(loadObjBusVolt_In, inService_In, loadScalar_in, smallMotor, 2, 0.7, smallState);

    (*dotX)[STATE_IM1SpeedWr] = largeDot.speed;
    (*dotX)[STATE_IM1Epr] = largeDot.epr;
    (*dotX)[STATE_IM1Epi] = largeDot.epi;
    (*dotX)[STATE_IM2SpeedWr] = smallDot.speed;
    (*dotX)[STATE_IM2Epr] = smallDot.epr;
    (*dotX)[STATE_IM2Epi] = smallDot.epi;
}


extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
    // no ignored states or algebraics are handled here
}


extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
    return(N_NONWINDUPLIMITS);
}

extern "C" __declspec(dllexport) bool __stdcall TimeStepEnd(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    bool result = false;   // Only return true if it actually needs to do something
    *EventTime = 0;
    *ExtraObjectIndex = -1;
    *MaxPossibleEventIndex = 0;
    if (*index != 0)
    {
        return(result);
    }

    bool inService_In = hardCodedSignals[HARDCODE_LOAD_DeviceStatus] > 0.5;
    double fVi = floatParams[PARAM_Vi];
    double fTi = floatParams[PARAM_Ti];
    double fTb = floatParams[PARAM_Tb];
    double local_VoltMag = hardCodedSignals[HARDCODE_LOAD_DeviceVPU];
    bool fLowVoltTimeSet = algebraics[ALG_TIMER_LowVoltTimeSet] != 0;
    double fLowVoltTime = algebraics[ALG_TIMER_LowVoltTime];
    double local_NewTime = SystemOptions->SimulationTimeSeconds;

    if (inService_In)
    {
        if (local_VoltMag >= fVi)
        {
            algebraics[ALG_TIMER_LowVoltTimeSet] = 0;   // Above voltage, so reset
        }
        else
        {
            if (!fLowVoltTimeSet)
            {
                fLowVoltTime = local_NewTime;   // First time below voltage
                algebraics[ALG_TIMER_LowVoltTime] = fLowVoltTime;
                algebraics[ALG_TIMER_LowVoltTimeSet] = 1;
            }
            if ((local_NewTime - fLowVoltTime) >= fTi)   // Trip threshold exceeded
            {
                *EventTime = local_NewTime + fTb;
                *ExtraObjectIndex = -1;   // trip itself
                result = true;
            }
        }
    }
    return(result);
}


extern "C" __declspec(dllexport) int __stdcall TimeStepEndAction(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, int* index, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* str;
    int stringLength;

    // Anything after the last pipe character | is treated as an extra log message to show in event reporting
    switch (*index)
    {
        case 0: str = L"OPEN|User_CLOD Under Voltage Trip: ";
            break;
        default: str = L"";
    }

    stringLength = pwStringCopy(StrBuf, StrSize, str);
    return(stringLength);
}


namespace {

// Discharge lighting power at voltage localVoltMag. It is zero at or below the extinction voltage and ramps up
// linearly to the breakpoint voltage, above which it follows localVoltMag^coefficient.
double GetDL_Total(double localVoltMag, double dlCoeff, double dlVoltBP, double dlVoltExt, double dlAt1PU)
{
    double result;
    if (localVoltMag <= dlVoltExt) return(0);
    if (localVoltMag >= dlVoltBP) result = MathYToX(localVoltMag, dlCoeff);
    else result = MathYToX(dlVoltBP, dlCoeff)*(localVoltMag - dlVoltExt)/(dlVoltBP - dlVoltExt);
    return(result*dlAt1PU);
}

// Constant impedance, constant current and constant power at voltage localVoltMag, with the reductions below the
// minimum voltages for the constant power and constant current parts.
double GetZIP_Total(double localVoltMag, double zVal, double iVal, double sVal, double minVoltSLoad, double minVoltILoad)
{
    double result = Sqr(localVoltMag)*zVal;
    if (localVoltMag >= minVoltSLoad) result = result + sVal;
    else result = result + 0.5*(1 - std::cos(Pi*localVoltMag/minVoltSLoad))*sVal;
    if (localVoltMag >= minVoltILoad) result = result + localVoltMag*iVal;
    else result = result + std::sin(Pi*0.5*localVoltMag/minVoltILoad)*localVoltMag*iVal;
    return(result);
}

}

extern "C" __declspec(dllexport) void __stdcall LoadNortonAdmittance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theG, double* theB)
{
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    *theG = 0;
    *theB = 0;

    // Norton equivalent from the large motor
    double IML_Rs = algebraics[ALG_IML_Rs];
    double IML_Xp = algebraics[ALG_IML_Xp];
    double t = Sqr(IML_Rs) + Sqr(IML_Xp);
    if (t != 0)
    {
        *theG = *theG + (IML_Rs)/t;
        *theB = *theB - (IML_Xp)/t;
    }

    // Norton equivalent from the small motor
    double IMS_Rs = algebraics[ALG_IMS_Rs];
    double IMS_Xp = algebraics[ALG_IMS_Xp];
    t = Sqr(IMS_Rs) + Sqr(IMS_Xp);
    if (t != 0)
    {
        *theG = *theG + (IMS_Rs)/t;
        *theB = *theB - (IMS_Xp)/t;
    }

    // The constant impedance portion of the load is in here too
    double ZIP_ZMW = algebraics[ALG_ZIP_ZMW];
    double ZIP_ZMVR = algebraics[ALG_ZIP_ZMVR];
    *theG = *theG + ZIP_ZMW;
    *theB = *theB - ZIP_ZMVR;   // the sign is not certain

    // The load scalar changes these as well
    double loadScalar_in = hardCodedSignals[HARDCODE_LOAD_LoadScalar];
    *theG = *theG*loadScalar_in;
    *theB = *theB*loadScalar_in;
}


namespace {

// A motor's Norton current is its internal voltage over its impedance. It is subtracted because the equation is for a generator.
void SubtractMotorNortonCurrent(double epr, double epi, double rsAlgebraic, double xpAlgebraic, double loadScalar, double& iReal, double& iImag)
{
    PWDComplex impedanceSystem = PWDComplex_Init(rsAlgebraic/loadScalar, xpAlgebraic/loadScalar);
    PWDComplex internalVolt = PWDComplex_Init(epr, epi);
    PWDComplex nortonCurrent = internalVolt.Divide(impedanceSystem);
    iReal = iReal - nortonCurrent.r;
    iImag = iImag - nortonCurrent.i;
}

// The constant power part of the ZIP load, with the reduction below the minimum voltage.
//   Ir = ks*(ge*P + gf*Q)   Ii = ks*(gf*P - ge*Q)   with ge = e/V^2 and gf = f/V^2
void AddConstantPowerNortonCurrent(PWDComplex busVolt, double busVoltMag, double minVoltSLoad, double smw, double smvr, double& iReal, double& iImag)
{
    if ((smw == 0) && (smvr == 0)) return;
    double v2Inv = Sqr(busVolt.r) + Sqr(busVolt.i);
    if (v2Inv > TinyVoltageSquared) v2Inv = 1/(v2Inv);
    else v2Inv = 1e6;   // avoid a divide by zero
    double ge = busVolt.r*v2Inv;
    double gf = busVolt.i*v2Inv;
    double ks = 1;
    if (busVoltMag < minVoltSLoad) ks = 0.5*(1 - std::cos(Pi*busVoltMag/minVoltSLoad));
    iReal = iReal + ks*(ge*smw + gf*smvr);
    iImag = iImag + ks*(gf*smw - ge*smvr);
}

// The constant current part of the ZIP load, with the reduction below the minimum voltage.
//   Ir = ki*(he*P + hf*Q)   Ii = ki*(hf*P - he*Q)   with he = e/V and hf = f/V
void AddConstantCurrentNortonCurrent(PWDComplex busVolt, double busVoltMag, double minVoltILoad, double imw, double imvr, double& iReal, double& iImag)
{
    if ((imw == 0) && (imvr == 0)) return;
    double busVoltMagInv = 1/busVoltMag;
    double he = busVolt.r*busVoltMagInv;
    double hf = busVolt.i*busVoltMagInv;
    double ki = 1;
    if (busVoltMag < minVoltILoad) ki = std::sin(Pi*0.5*busVoltMag/minVoltILoad);
    iReal = iReal + ki*(he*imw + hf*imvr);
    iImag = iImag + ki*(hf*imw - he*imvr);
}

}

extern "C" __declspec(dllexport) void __stdcall LoadNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag)
{
    TDoubleArray& states = *ParamsAndStates->States;
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    *IReal = 0;
    *IImag = 0;

    double loadScalar_in = hardCodedSignals[HARDCODE_LOAD_LoadScalar];
    double minVoltSLoad_in = SystemOptions->MinVoltSLoad;
    double minVoltILoad_in = SystemOptions->MinVoltILoad;

    // The network voltage presently
    double local_BusVoltMag = hardCodedSignals[HARDCODE_LOAD_DeviceVPU];
    double loadObjThetaRadian_In = hardCodedSignals[HARDCODE_LOAD_DeviceAngleRad];
    PWDComplex loadObjBusVolt_In = PWDComplex_InitPolarRad(local_BusVoltMag, loadObjThetaRadian_In);

    // Large motor and small motor
    SubtractMotorNortonCurrent(states[STATE_IM1Epr], states[STATE_IM1Epi], algebraics[ALG_IML_Rs], algebraics[ALG_IML_Xp], loadScalar_in, *IReal, *IImag);
    SubtractMotorNortonCurrent(states[STATE_IM2Epr], states[STATE_IM2Epi], algebraics[ALG_IMS_Rs], algebraics[ALG_IMS_Xp], loadScalar_in, *IReal, *IImag);

    // Discharge lighting, I = (S/V)*
    double DL_P_at_1_PU = algebraics[ALG_DL_P_at_1_PU]*loadScalar_in;
    double DL_Q_at_1_PU = algebraics[ALG_DL_Q_at_1_PU]*loadScalar_in;
    double DL_TotalP = GetDL_Total(local_BusVoltMag, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
    double DL_TotalQ = GetDL_Total(local_BusVoltMag, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
    PWDComplex nortonCurrent = PWDComplex_Init(DL_TotalP, DL_TotalQ).Divide(loadObjBusVolt_In);
    nortonCurrent.i = -nortonCurrent.i;   // conjugate
    *IReal = *IReal + nortonCurrent.r;
    *IImag = *IImag + nortonCurrent.i;

    // Constant power and constant current portions of the ZIP load. The constant impedance
    // portion is handled in the Norton admittance.
    nortonCurrent.r = 0;
    nortonCurrent.i = 0;
    double ZIP_SMW = algebraics[ALG_ZIP_SMW]*loadScalar_in;
    double ZIP_SMVR = algebraics[ALG_ZIP_SMVR]*loadScalar_in;
    AddConstantPowerNortonCurrent(loadObjBusVolt_In, local_BusVoltMag, minVoltSLoad_in, ZIP_SMW, ZIP_SMVR, nortonCurrent.r, nortonCurrent.i);
    double ZIP_IMW = algebraics[ALG_ZIP_IMW]*loadScalar_in;
    double ZIP_IMVR = algebraics[ALG_ZIP_IMVR]*loadScalar_in;
    AddConstantCurrentNortonCurrent(loadObjBusVolt_In, local_BusVoltMag, minVoltILoad_in, ZIP_IMW, ZIP_IMVR, nortonCurrent.r, nortonCurrent.i);
    *IReal = *IReal + nortonCurrent.r;
    *IImag = *IImag + nortonCurrent.i;
}


namespace {

// Derivatives of the real and imaginary current with respect to the real (e) and imaginary (f) voltage.
struct CurrentDerivatives
{
    double dIrDe = 0;
    double dIrDf = 0;
    double dIiDe = 0;
    double dIiDf = 0;
};

// For a load of the form Po*V^Pn + Qo*V^Qn, the current is I = S*/V* so
//   Ir = e*Po*V^(Pn-2) + f*Qo*V^(Qn-2)    Ii = f*Po*V^(Pn-2) - e*Qo*V^(Qn-2)
// and the chain rule (dV/de = e/V, dV/df = f/V) gives the derivatives.
void AddExponentialVoltageDerivatives(double po, double qo, double pn, double qn, PWDComplex busVoltage, CurrentDerivatives& d)
{
    double v = busVoltage.Mag();
    double e = busVoltage.r;
    double f = busVoltage.i;
    double vp2 = po*MathYToX(v, pn - 2);
    double vp4 = (pn - 2)*po*MathYToX(v, pn - 4);   // exponent -4 because it includes the /V from the partial of e or f
    double vq2 = qo*MathYToX(v, qn - 2);
    double vq4 = (qn - 2)*qo*MathYToX(v, qn - 4);

    d.dIrDe = d.dIrDe + vp2 + (vp4*e + vq4*f)*e;
    d.dIrDf = d.dIrDf + vq2 + (vp4*e + vq4*f)*f;
    d.dIiDe = d.dIiDe - vq2 + (vp4*f - vq4*e)*e;
    d.dIiDf = d.dIiDf + vp2 + (vp4*f - vq4*e)*f;
}

// Between the extinction and the breakpoint voltage the power ramps linearly: P = Pbp*ks and Q = Qbp*ks, where
// Pbp and Qbp are the power at the breakpoint voltage and ks = (V - Vext)/(Vbp - Vext) is a function of voltage.
// This is the constant power equation with a voltage-dependent scalar, so d(u/v)/dx = (du*v - dv*u)/(v^2) is used.
void AddDischargeLightingRampDerivatives(PWDComplex busVolt, double busVoltMag, double dlPAt1PU, double dlQAt1PU, CurrentDerivatives& d)
{
    // Power at the breakpoint voltage. ks supplies the ramp, so using the power at the present voltage here would apply it twice.
    double dlTotalP = GetDL_Total(GLOBAL_DL_VoltBP, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, dlPAt1PU);
    double dlTotalQ = GetDL_Total(GLOBAL_DL_VoltBP, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, dlQAt1PU);

    double e = busVolt.r;
    double f = busVolt.i;
    double v2 = Sqr(busVoltMag);
    double invV2 = 1/v2;

    // The numerators of Ir and Ii
    double irNum = dlTotalP*e + dlTotalQ*f;
    double iiNum = dlTotalP*f - dlTotalQ*e;

    double ks = (busVoltMag - GLOBAL_DL_VoltExt)/(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt);
    double invV4Ks = ks*Sqr(invV2);

    // The portion that assumes a fixed ks
    d.dIrDe = d.dIrDe + (dlTotalP*v2 - irNum*2*e)*invV4Ks;
    d.dIrDf = d.dIrDf + (dlTotalQ*v2 - irNum*2*f)*invV4Ks;
    d.dIiDe = d.dIiDe + (-dlTotalQ*v2 - iiNum*2*e)*invV4Ks;
    d.dIiDf = d.dIiDf + (dlTotalP*v2 - iiNum*2*f)*invV4Ks;

    // The impact of ks being a function of voltage
    double dksDe = e/(busVoltMag*(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt));
    double dksDf = f/(busVoltMag*(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt));
    d.dIrDe = d.dIrDe + (irNum*invV2)*dksDe;
    d.dIrDf = d.dIrDf + (irNum*invV2)*dksDf;
    d.dIiDe = d.dIiDe + (iiNum*invV2)*dksDe;
    d.dIiDf = d.dIiDf + (iiNum*invV2)*dksDf;
}

CurrentDerivatives DischargeLightingDerivatives(PWDComplex busVolt, double busVoltMag, double loadScalar, const TDoubleArray& algebraics)
{
    CurrentDerivatives d;
    if (busVoltMag > TinyVoltageSquared)
    {
        double dlPAt1PU = algebraics[ALG_DL_P_at_1_PU]*loadScalar;
        double dlQAt1PU = algebraics[ALG_DL_Q_at_1_PU]*loadScalar;
        // At or below the extinction voltage the load is out, so all derivatives are zero
        if (busVoltMag > GLOBAL_DL_VoltExt)
        {
            if (busVoltMag >= GLOBAL_DL_VoltBP)
            {
                AddExponentialVoltageDerivatives(dlPAt1PU, dlQAt1PU, GLOBAL_DL_PCoeff, GLOBAL_DL_QCoeff, busVolt, d);
            }
            else
            {
                AddDischargeLightingRampDerivatives(busVolt, busVoltMag, dlPAt1PU, dlQAt1PU, d);
            }
        }
    }
    return(d);
}

// Rectangular voltage and its magnitude for the ZIP derivatives.
struct ZipVoltage
{
    double e = 0;
    double f = 0;
    double v2 = 0;
    double invV2 = 0;
    double v = 0;
};

// Constant power: Ir = ks*(P*e + Q*f)/V^2 and Ii = ks*(P*f - Q*e)/V^2, with ks reducing the load below minVoltSLoad.
void AddConstantPowerDerivatives(const ZipVoltage& u, double minVoltSLoad, double smw, double smvr, CurrentDerivatives& d)
{
    // as in the Delphi source, where "and" binds tighter than "or"
    if (!(((u.v2 > 0) && (smw != 0)) || (smvr != 0))) return;
    double irNum = smw*u.e + smvr*u.f;
    double iiNum = smw*u.f - smvr*u.e;

    double ks = 1;
    if (u.v < minVoltSLoad) ks = 0.5*(1 - std::cos(Pi*u.v/minVoltSLoad));
    double invV4Ks = ks*Sqr(u.invV2);

    // The portion that assumes a fixed ks
    d.dIrDe = d.dIrDe + (smw*u.v2 - irNum*2*u.e)*invV4Ks;
    d.dIrDf = d.dIrDf + (smvr*u.v2 - irNum*2*u.f)*invV4Ks;
    d.dIiDe = d.dIiDe + (-smvr*u.v2 - iiNum*2*u.e)*invV4Ks;
    d.dIiDf = d.dIiDf + (smw*u.v2 - iiNum*2*u.f)*invV4Ks;

    // Below minVoltSLoad the impact of ks being a function of voltage
    if (u.v < minVoltSLoad)
    {
        double dksDv = Pi/(2*minVoltSLoad)*std::sin(Pi*u.v/minVoltSLoad);
        double dksDe = dksDv*u.e/u.v;
        double dksDf = dksDv*u.f/u.v;
        d.dIrDe = d.dIrDe + (irNum*u.invV2)*dksDe;
        d.dIrDf = d.dIrDf + (irNum*u.invV2)*dksDf;
        d.dIiDe = d.dIiDe + (iiNum*u.invV2)*dksDe;
        d.dIiDf = d.dIiDf + (iiNum*u.invV2)*dksDf;
    }
}

// Constant current: (P - jQ)*(e + jf)/V, with ki reducing the load below minVoltILoad.
void AddConstantCurrentDerivatives(const ZipVoltage& u, double minVoltILoad, double imw, double imvr, CurrentDerivatives& d)
{
    // as in the Delphi source, where "and" binds tighter than "or"
    if (!(((u.v2 > 0) && (imw != 0)) || (imvr != 0))) return;
    double irNum = imw*u.e + imvr*u.f;
    double iiNum = imw*u.f - imvr*u.e;

    double ki = 1;
    if (u.v < minVoltILoad) ki = std::sin(Pi*0.5*u.v/minVoltILoad);

    double eOverV = u.e/u.v;
    double fOverV = u.f/u.v;
    d.dIrDe = d.dIrDe + (imw*u.v - irNum*eOverV)*u.invV2*ki;
    d.dIrDf = d.dIrDf + (imvr*u.v - irNum*fOverV)*u.invV2*ki;
    d.dIiDe = d.dIiDe + (-imvr*u.v - iiNum*eOverV)*u.invV2*ki;
    d.dIiDf = d.dIiDf + (imw*u.v - iiNum*fOverV)*u.invV2*ki;

    // Below minVoltILoad the impact of ki being a function of voltage
    if (u.v < minVoltILoad)
    {
        double dkiDv = Pi*0.5/minVoltILoad*std::cos(Pi*0.5*u.v/minVoltILoad);
        double dkiDe = dkiDv*eOverV;
        double dkiDf = dkiDv*fOverV;
        d.dIrDe = d.dIrDe + irNum/u.v*dkiDe;
        d.dIrDf = d.dIrDf + irNum/u.v*dkiDf;
        d.dIiDe = d.dIiDe + iiNum/u.v*dkiDe;
        d.dIiDf = d.dIiDf + iiNum/u.v*dkiDf;
    }
}

}

extern "C" __declspec(dllexport) void __stdcall LoadNortonCurrentAlgebraicDerivative(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal_dVreal, double* IReal_dVimag, double* IImag_dVreal, double* IImag_dVimag)
{
    TDoubleArray& hardCodedSignals = *ParamsAndStates->HardCodedSignals;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    double loadScalar_in = hardCodedSignals[HARDCODE_LOAD_LoadScalar];
    double minVoltSLoad_in = SystemOptions->MinVoltSLoad;
    double minVoltILoad_in = SystemOptions->MinVoltILoad;

    // The network voltage presently
    double local_BusVoltMag = hardCodedSignals[HARDCODE_LOAD_DeviceVPU];
    double loadObjThetaRadian_In = hardCodedSignals[HARDCODE_LOAD_DeviceAngleRad];
    PWDComplex loadObjBusVolt_In = PWDComplex_InitPolarRad(local_BusVoltMag, loadObjThetaRadian_In);

    CurrentDerivatives dischargeLighting = DischargeLightingDerivatives(loadObjBusVolt_In, local_BusVoltMag, loadScalar_in, algebraics);

    ZipVoltage zipVoltage;
    zipVoltage.e = loadObjBusVolt_In.r;
    zipVoltage.f = loadObjBusVolt_In.i;
    zipVoltage.v2 = Sqr(zipVoltage.e) + Sqr(zipVoltage.f);
    if (zipVoltage.v2 > TinyVoltageSquared) zipVoltage.invV2 = 1/zipVoltage.v2;
    else zipVoltage.invV2 = 1e6;   // avoid a divide by zero
    zipVoltage.v = std::sqrt(zipVoltage.v2);

    CurrentDerivatives zip;
    double ZIP_SMW = algebraics[ALG_ZIP_SMW]*loadScalar_in;
    double ZIP_SMVR = algebraics[ALG_ZIP_SMVR]*loadScalar_in;
    AddConstantPowerDerivatives(zipVoltage, minVoltSLoad_in, ZIP_SMW, ZIP_SMVR, zip);
    double ZIP_IMW = algebraics[ALG_ZIP_IMW]*loadScalar_in;
    double ZIP_IMVR = algebraics[ALG_ZIP_IMVR]*loadScalar_in;
    AddConstantCurrentDerivatives(zipVoltage, minVoltILoad_in, ZIP_IMW, ZIP_IMVR, zip);

    *IReal_dVreal = dischargeLighting.dIrDe + zip.dIrDe;
    *IReal_dVimag = dischargeLighting.dIrDf + zip.dIrDf;
    *IImag_dVreal = dischargeLighting.dIiDe + zip.dIiDe;
    *IImag_dVimag = dischargeLighting.dIiDf + zip.dIiDf;
}


namespace {

// The equivalent circuit, initial power and initial slip of one induction motor.
struct MotorSetup
{
    double pInitial = 0;
    double qInitial = 0;
    double slipInitial = 0;
    double rs = 0;
    double xs = 0;
    double xm = 0;
    double r1 = 0;
    double x1 = 0;
    double wo = 0;
    double xp = 0;
    double x = 0;
    double invTp = 0;
};

// Scales the circuit to the motor's power, finds the initial slip and reactive power, and
// zeroes the power if no slip supplies it.
void InitializeAlgebraicMotor(double puTol, double nomFreq, double steadyStateV, MotorSetup& m)
{
    double scale = 1/m.pInitial;
    m.rs = m.rs*scale;
    m.xs = m.xs*scale;
    m.xm = m.xm*scale;
    m.r1 = m.r1*scale;
    m.x1 = m.x1*scale;
    m.wo = 2*Pi*nomFreq;
    m.xp = m.xs + m.xm*m.x1/(m.xm + m.x1);   // the Thevenin reactance
    m.x = m.xs + m.xm;                       // for a reference see 7.102 of Kundur
    m.invTp = m.r1/(m.x1 + m.xm)*m.wo;       // needed in calculateFofX, so it is set here

    MotorCircuit circuit;
    circuit.rs = static_cast<float>(m.rs);
    circuit.xs = static_cast<float>(m.xs);
    circuit.xm = static_cast<float>(m.xm);
    circuit.r1 = static_cast<float>(m.r1);
    circuit.x1 = static_cast<float>(m.x1);
    circuit.busVoltageMag = static_cast<float>(steadyStateV);
    circuit.impedanceSystem = PWDComplex_Init(m.rs, m.xp);
    double torqueElectrical = 0;
    unsigned char slipResult = CalculateInitialSlip(circuit, m.pInitial, puTol, m.slipInitial, m.qInitial, torqueElectrical);
    // 0 if optimal, 1 if P is too high, 2 if P is too low
    if (slipResult != 0)
    {
        m.pInitial = 0;
        m.qInitial = 0;
    }
}

void WriteMotorAlgebraics(TDoubleArray& algebraics, int firstIndex, const MotorSetup& m)
{
    algebraics[firstIndex + OFFSET_Rs] = m.rs;
    algebraics[firstIndex + OFFSET_Xs] = m.xs;
    algebraics[firstIndex + OFFSET_fWo] = m.wo;
    algebraics[firstIndex + OFFSET_fInvTp] = m.invTp;
    algebraics[firstIndex + OFFSET_Xp] = m.xp;
    algebraics[firstIndex + OFFSET_X] = m.x;
    algebraics[firstIndex + OFFSET_SlipInitial] = m.slipInitial;
    algebraics[firstIndex + OFFSET_fPInitial] = m.pInitial;
    algebraics[firstIndex + OFFSET_fQInitial] = m.qInitial;
}

struct DischargeLighting
{
    double pAt1PU = 0;
    double qAt1PU = 0;
    double totalP = 0;
    double totalQ = 0;
};

// Sizes the discharge lighting so it takes fDis percent of the load at the steady state voltage.
DischargeLighting InitializeDischargeLighting(double fDis, double steadyStateP, double steadyStateQ, double steadyStateV)
{
    DischargeLighting dl;
    if (!(fDis > 0)) return(dl);
    double dlPer = fDis/100;
    dl.pAt1PU = 1;
    dl.qAt1PU = 1;
    bool dlR = true;   // false if the voltage is too low to initialize

    double dlTotalP = GetDL_Total(steadyStateV, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, dl.pAt1PU);
    if (dlTotalP != 0)
    {
        dl.pAt1PU = steadyStateP/dlTotalP;
    }
    else
    {
        if (steadyStateP != 0) dlR = false;
        else dl.pAt1PU = 0;
    }

    double dlTotalQ = GetDL_Total(steadyStateV, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, dl.qAt1PU);
    if (dlR)
    {
        if (dlTotalQ != 0)
        {
            dl.qAt1PU = steadyStateQ/dlTotalQ;
        }
        else
        {
            if (steadyStateQ != 0) dlR = false;
            else dl.qAt1PU = 0;
        }
    }

    // Normalize the load by the percent and return the total power allocated
    dl.pAt1PU = dl.pAt1PU*dlPer;
    dl.qAt1PU = dl.qAt1PU*dlPer;
    dl.totalP = GetDL_Total(steadyStateV, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, dl.pAt1PU);
    dl.totalQ = GetDL_Total(steadyStateV, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, dl.qAt1PU);
    return(dl);
}

// The constant impedance, constant current and constant power portions that make up the rest of the load.
struct ZipLoad
{
    double zMW = 0;
    double iMW = 0;
    double sMW = 0;
    double zMVR = 0;
    double iMVR = 0;
    double sMVR = 0;
};

ZipLoad InitializeZipLoad(double fTex, double fP, double fKp, double percentOtherLoad, double steadyStateP, double steadyStateQ,
                          double steadyStateV, double usedP, double usedQ, double minVoltSLoad, double minVoltILoad)
{
    ZipLoad z;
    // fTex: transformer saturation is treated as constant power, and so is fP
    z.sMW = fTex + fP;
    z.sMVR = z.sMW;
    double roundedKp = std::nearbyint(fKp);   // Delphi Round
    if (roundedKp == 0) z.sMW = 100 - percentOtherLoad;                // all the rest as constant power
    else if (roundedKp == 1) z.iMW = 100 - percentOtherLoad - z.sMW;
    else z.zMW = 100 - percentOtherLoad - z.sMW;                       // constant impedance is what is left
    z.zMVR = 100 - percentOtherLoad - z.sMW;                           // constant impedance reactive is what is left

    double zipTotalP = GetZIP_Total(steadyStateV, z.zMW, z.iMW, z.sMW, minVoltSLoad, minVoltILoad);
    double zipTotalQ = GetZIP_Total(steadyStateV, z.zMVR, z.iMVR, z.sMVR, minVoltSLoad, minVoltILoad);

    double scale = 1;
    if (zipTotalP != 0) scale = (steadyStateP - usedP)/zipTotalP;
    z.zMW = z.zMW*scale;
    z.iMW = z.iMW*scale;
    z.sMW = z.sMW*scale;

    scale = 1;
    if (zipTotalQ != 0) scale = (steadyStateQ - usedQ)/zipTotalQ;
    z.zMVR = z.zMVR*scale;
    z.iMVR = z.iMVR*scale;
    z.sMVR = z.sMVR*scale;
    return(z);
}

}

extern "C" __declspec(dllexport) bool __stdcall LoadInitializeAlgebraic(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double INPUT_PUTol, double SteadyStateP, double SteadyStateQ, double SteadyStateV, double* InitLoadP, double* InitLoadQ)
{
    TDoubleArray& floatParams = *ParamsAndStates->FloatParams;
    TDoubleArray& algebraics = *ParamsAndStates->Algebraics;

    if (SteadyStateV == 0) SteadyStateV = 1;   // Set to unity if not in service

    double fLmotor = floatParams[PARAM_PercLmotor];
    double fSmotor = floatParams[PARAM_PercSmotor];
    double fTex = floatParams[PARAM_PercTex];
    double fDis = floatParams[PARAM_PercDis];
    double fP = floatParams[PARAM_PercP];
    double fKp = floatParams[PARAM_Kp];

    double loadObjNomFreq_In = SystemOptions->WBase/(2*Pi);
    double minVoltSLoad_in = SystemOptions->MinVoltSLoad;
    double minVoltILoad_in = SystemOptions->MinVoltILoad;

    double localUsedP_out = 0;
    double localUsedQ_out = 0;
    MotorSetup largeMotor;
    MotorSetup smallMotor;
    if (SteadyStateV > 0)
    {
        // The large motor first. The reactive power is figured out by the initial slip calculation.
        largeMotor.pInitial = SteadyStateP*(fLmotor/100);
        largeMotor.rs = 0.013;
        largeMotor.xs = 0.067;
        largeMotor.xm = 3.8;
        largeMotor.r1 = 0.009;
        largeMotor.x1 = 0.17;
        InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV, largeMotor);
        localUsedP_out = largeMotor.pInitial;
        localUsedQ_out = largeMotor.qInitial;

        // Then the small motor
        smallMotor.pInitial = SteadyStateP*(fSmotor/100);
        smallMotor.rs = 0.031;
        smallMotor.xs = 0.1;
        smallMotor.xm = 3.2;
        smallMotor.r1 = 0.018;
        smallMotor.x1 = 0.18;
        InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV, smallMotor);
        localUsedP_out = localUsedP_out + smallMotor.pInitial;
        localUsedQ_out = localUsedQ_out + smallMotor.qInitial;
    }

    DischargeLighting dischargeLighting = InitializeDischargeLighting(fDis, SteadyStateP, SteadyStateQ, SteadyStateV);
    localUsedP_out = localUsedP_out + dischargeLighting.totalP;
    localUsedQ_out = localUsedQ_out + dischargeLighting.totalQ;

    // The total of induction motors, lighting and constant power. Everything else is the ZIP model.
    double percentOtherLoad = fLmotor + fSmotor + fDis;
    ZipLoad zip = InitializeZipLoad(fTex, fP, fKp, percentOtherLoad, SteadyStateP, SteadyStateQ, SteadyStateV,
                                    localUsedP_out, localUsedQ_out, minVoltSLoad_in, minVoltILoad_in);

    // All of the power is allocated
    *InitLoadP = SteadyStateP;
    *InitLoadQ = SteadyStateQ;

    WriteMotorAlgebraics(algebraics, ALG_IML_Rs, largeMotor);
    WriteMotorAlgebraics(algebraics, ALG_IMS_Rs, smallMotor);

    algebraics[ALG_DL_P_at_1_PU] = dischargeLighting.pAt1PU;
    algebraics[ALG_DL_Q_at_1_PU] = dischargeLighting.qAt1PU;

    algebraics[ALG_ZIP_ZMW] = zip.zMW;
    algebraics[ALG_ZIP_IMW] = zip.iMW;
    algebraics[ALG_ZIP_SMW] = zip.sMW;
    algebraics[ALG_ZIP_ZMVR] = zip.zMVR;
    algebraics[ALG_ZIP_IMVR] = zip.iMVR;
    algebraics[ALG_ZIP_SMVR] = zip.sMVR;

    // Low voltage trip timer
    algebraics[ALG_TIMER_LowVoltTimeSet] = 0;
    algebraics[ALG_TIMER_LowVoltTime] = 0;

    return(true);
}

}
