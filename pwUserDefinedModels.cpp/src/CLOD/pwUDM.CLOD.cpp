module pwUDM.CLOD;

import std;
import pwUDM.Core;
import pwUDM.Load;

namespace {

// Complex Number object to make some of the code easier to read.
struct PWDComplex
{
    double r, i;
    void Init(double tr, double ti);
    PWDComplex CInc(PWDComplex tempCom);
    PWDComplex CDec(PWDComplex tempCom);
    double Mag();
    PWDComplex Multiply(PWDComplex aValue);
    PWDComplex Divide(PWDComplex aValue);
};

//******************************************************************************
void PWDComplex::Init(double tr, double ti)
{
    r = tr;
    i = ti;
}

PWDComplex PWDComplex::CInc(PWDComplex TempCom)
{
    PWDComplex result;
    result.r = r + TempCom.r;
    result.i = i + TempCom.i;
    return result;
}

PWDComplex PWDComplex::CDec(PWDComplex TempCom)
{
    PWDComplex result;
    result.r = r - TempCom.r;
    result.i = i - TempCom.i;
    return result;
}

double PWDComplex::Mag()
{
    double result;
    result = std::sqrt(std::pow(r, 2) + std::pow(i, 2));
    return result;
}

PWDComplex PWDComplex::Multiply(PWDComplex aValue) // Returns self multiplied with aValue
{
    PWDComplex result;
    result.r = (r*aValue.r - i*aValue.i);
    result.i = (i*aValue.r + r*aValue.i);
    return result;
}

PWDComplex PWDComplex::Divide(PWDComplex aValue) // Returns self divided by aValue
{
    PWDComplex result;
    double t;
    t = 1 / (std::pow(aValue.r, 2) + std::pow(aValue.i, 2));
    result.r = (r * aValue.r + i * aValue.i) * t;
    result.i = (-r * aValue.i + i * aValue.r) * t;
    return result;
}

//******************************************************************************
double Math_Y_to_X(double base, double exponent)
{
    double result;
    if (base == 0) result = 0;
    else {
        if (exponent == 0) result = 1;
        else if (exponent == 1) result = base;
        else if (exponent == 2) result = std::pow(base, 2);
        else if (exponent == 3) result = base*std::pow(base, 2);
        else if (exponent == 4) result = std::pow(base, 2)*std::pow(base, 2);
        else result = std::exp(exponent*std::log(base));
    }
    return result;
}

//******************************************************************************
PWDComplex PWDComplex_Init(double r, double i)
{
    PWDComplex result;
    result.r = r;
    result.i = i;
    return result;
}

PWDComplex PWDComplex_InitPolarRad(double tmag, double tangle)
{
    PWDComplex result;
    result.r = tmag * std::cos(tangle);
    result.i = tmag * std::sin(tangle);
    return result;
}

PWDComplex PWDComplex_Multiply(PWDComplex q1, PWDComplex q2)
{
    PWDComplex result;
    result.r = q1.r * q2.r - q1.i * q2.i;
    result.i = q1.r * q2.i + q1.i * q2.r;
    return result;
}

PWDComplex PWDComplex_Divide(PWDComplex num, PWDComplex den)
{
    PWDComplex result;
    double t;
    t = 1 / (std::pow(den.r, 2) + std::pow(den.i, 2));
    result.r = (num.r * den.r + num.i * den.i) * t;
    result.i = (-num.r * den.i + num.i * den.r) * t;
    return result;
}

PWDComplex PWDComplex_Parallel(PWDComplex complex1, PWDComplex complex2)
{
    PWDComplex temp_num, temp_Den;
    temp_num = PWDComplex_Multiply(complex1, complex2);
    temp_Den.r = complex1.r + complex2.r;
    temp_Den.i = complex1.i + complex2.i;
    PWDComplex result = PWDComplex_Divide(temp_num, temp_Den);
    return result;
}

//******************************************************************************
void TxInductionMachine_CalculateAtSlip(float fRs, float fXs, float fXm, float fR1, float fX1, float fBusVoltageMag,
                                        PWDComplex impedanceSystem, double aSlip,
                                        PWDComplex& iStator, PWDComplex& eInternal,
                                        double& totalP, double& totalQ, double& local_PF, double& Telec)
{
    PWDComplex rotorZ1;  // stator and rotor impedances
    PWDComplex zRotor, zTotal;  // Total rotor and total impedance
    //L012 : Double;  // L0 in series with (L1 in parallel with L2)
    PWDComplex fzM, fstatorZ;
    PWDComplex fvTerminal;

    fzM = PWDComplex_Init(0, fXm);
    fstatorZ.Init(fRs, fXs);
    fvTerminal.Init(fBusVoltageMag, 0);
    if (aSlip != 0) {
        rotorZ1.Init(fR1/aSlip, fX1);
        zRotor = rotorZ1;
        zTotal = PWDComplex_Parallel(zRotor, fzM).CInc(fstatorZ);
    }
    else zTotal = fstatorZ.CInc(fzM);
    iStator = PWDComplex_Divide(fvTerminal, zTotal);
    totalP = fvTerminal.r*iStator.r;  // fVoltmag.i is always zero
    totalQ = -fvTerminal.r*iStator.i;
    local_PF = totalP/std::sqrt(std::pow(totalP, 2) + std::pow(totalQ, 2));
    eInternal = fvTerminal.CDec(iStator.Multiply(impedanceSystem));
    Telec = eInternal.r*iStator.r + eInternal.i*iStator.i;
}

//******************************************************************************
double TxInductionMachine_CalculateAtSlipJustP(float fRs, float fXs, float fXm, float fR1, float fX1, float fBusVoltageMag,
                                               PWDComplex impedanceSystem, double aSlip)
{
    PWDComplex temp_iStator, temp_eInternal;
    double temp_Q, temp_PF, temp_Telec;
    double result;

    TxInductionMachine_CalculateAtSlip(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag,
                                       impedanceSystem, aSlip, temp_iStator, temp_eInternal,
                                       result, temp_Q, temp_PF, temp_Telec);
    return result;
}

//******************************************************************************
// Function TTxGenericDoubleCageInductionMotorModel.CalculateInitialSlip
// iteratively determines the initial slip, returning true is such a slip exists.
// For this algorithm the assumption is there are at most three slip values that
// solve for the P.  Algorithm returns the one closest to zero slip.  Note,
// there also may be only two, one solution (between 0 and 1) or no solutions.
unsigned char TxInductionMachine_CalculateInitialSlip(float fRs, float fXs, float fXm, float fR1, float fX1, float fBusVoltageMag, PWDComplex impedanceSystem, double aPower, double aTol, double& theSlip, double& initialQPU, double& theTElec) // return 0 if optimal; 1 if P too high, 2 if P too low
{
    double slipH, slipL, slipM;
    double phigh, plow, pmid;
    int i;
    double deltaSlip;
    PWDComplex temp_iStator, temp_eInternal;
    double temp_PF, temp_P;
    unsigned char result;

    result = 0;
    initialQPU = 0;
    slipL = 0;
    theTElec = 0;
    deltaSlip = 0.02;
    // Check for the unlikely case where power is below the power just from stator losses
    plow = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipL);
    if (plow >= aPower) {
        if (plow > aPower) result = 2;
        theSlip = 0;
    }
    // Work to bracket the slip.  If phigh-aPower > 0 then we know we have it bracketed.
    // Otherwise if phigh-aPower < 0 and it is going down we've got a case in which
    // we've bracketed two solutions.  This will take more work.
    else {
        slipH = deltaSlip;
        phigh = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipH);
        while ((phigh - aPower <= 0) && (phigh > plow) && (slipH <= 1)) {
            plow = phigh;
            slipH = slipH + deltaSlip;  // Most slips will be small so this should be done only once or twice usually
            phigh = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipH);
            if (slipH >= 0.1) deltaSlip = 0.05;
        }
        if ((phigh - aPower > 0)) { // We have a single value bracketed between slipH and 0.  Iterate until P mismatch is small
            i = 0;
            do {
                slipM = (slipH + slipL)*0.5;
                pmid = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipM);
                if (pmid - aPower > 0) slipH = slipM;
                else slipL = slipM;
                i++;
            } while (!((std::fabs(pmid - aPower) < aTol) || (i > 20)));

            if (i > 20) result = 1;  // This shouldn't occur
            // Found the value; return with the slip and the initial Q
            else {
                theSlip = slipM;
                TxInductionMachine_CalculateAtSlip(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag,
                                                   impedanceSystem, theSlip, temp_iStator,
                                                   temp_eInternal, temp_P, initialQPU, temp_PF, theTElec);
            }
        }
        else if ((slipH > 1) && (phigh > plow)) {  // power continually increasing so and we don't have enough
            result = 1;
            theSlip = 1;
            TxInductionMachine_CalculateAtSlip(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag,
                                               impedanceSystem, theSlip, temp_iStator,
                                               temp_eInternal, temp_P, initialQPU, temp_PF, theTElec);
        }
        else {
            // This is the tricky case, but thankfully one that shouldn't occur too often --
            // it requires operating with a motor pretty close to stalling. A solution
            // may or may not exist. If it exists then there are usually two solutions.
            // As a quick and dirty solution just set phigh to plow-deltaSlip, which will
            // guarantee we are starting at a slip below the max (towards slip = 0).
            // Then increment the value by a small about until either get a value above
            // power or hit the maximum.  If above power then redo above iteration;
            // otherwise exit.  Note this approach will miss operating points very near
            // the max slip, but these are not realistic transient stability solutions anyways.
            slipL = slipL - deltaSlip;
            if (slipL < 0) slipL = 0;
            theSlip = slipH;  // temp storage of maximum value to check
            plow = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipL);
            slipH = slipL + deltaSlip/10;
            phigh = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipH);
            while ((slipH < theSlip) && (phigh - aPower < 0) && (phigh > plow)) {
                plow = phigh;
                slipL = slipH;
                slipH = slipL + deltaSlip/10;
                phigh = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipH);
            }
            if (phigh - aPower >= 0) {
                i = 0;
                do {
                    slipM = (slipH + slipL)*0.5;
                    pmid = TxInductionMachine_CalculateAtSlipJustP(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag, impedanceSystem, slipM);
                    if (pmid - aPower > 0) slipH = slipM;
                    else slipL = slipM;
                    i++;
                } while (!((std::fabs(pmid - aPower) < aTol) || (i > 20)));
                theSlip = slipM;
                if (i > 20) result = 1;  // shouldn't occur
                // Found the value; return with the slip and the initial Q
                else TxInductionMachine_CalculateAtSlip(fRs, fXs, fXm, fR1, fX1, fBusVoltageMag,
                                                        impedanceSystem, theSlip, temp_iStator,
                                                        temp_eInternal, temp_P, initialQPU, temp_PF, theTElec);
            }
            else {
                result = 1;
                theSlip = slipH;
            }
        }
    }
    return result;
}

//******************************************************************************
bool TxInductionMachine_GetInitialConditions(double fWo,
                                             double fPInit,
                                             double fRa, double fX, double fXp, double fXl, double fInvTp, PWDComplex fTermVolt, int fMaxItr, double fItrTol,
                                             double& fSlip, double& fErp, double& fEip, double& fId, double& fIq, double& fQInit)
{
    //---------------------------------------------------------------------------
    constexpr int MatSize = 3;
    using TMyMatrix = double[MatSize + 1][MatSize + 1];
    using TMyVector = double[MatSize + 1];
    using TMyIntVector = int[MatSize + 1];

    //---------------------------------------------------------------------------
    auto VectorNorm2 = [&](const TMyVector& theVector) -> double {
        double result;
        int i;
        result = 0;
        for (i = 1; i <= MatSize; i++) {
            result = result + std::pow(theVector[i], 2);
        }
        result = std::sqrt(result);
        return result;
    };

    //---------------------------------------------------------------------------
    auto MatrixZeroOut = [&](TMyMatrix& theMatrix) {
        int theRow, theCol;
        for (theRow = 1; theRow <= MatSize; theRow++) {
            for (theCol = 1; theCol <= MatSize; theCol++) {
                theMatrix[theRow][theCol] = 0;
            }
        }
    };

    //---------------------------------------------------------------------------
    auto MatrixFactor = [&](TMyMatrix& theMatrix, TMyIntVector& RowPerm) {
        int k, j, imax, i;
        double sum, dum, big;
        TMyVector vv;

        imax = 0;

        for (i = 1; i <= MatSize; i++) {
            RowPerm[i] = i;
            big = 0.0;
            for (j = 1; j <= MatSize; j++) {
                if (std::fabs(theMatrix[i][j]) > big) big = std::fabs(theMatrix[i][j]);
            }
            vv[i] = 1.0/big;
        }

        for (j = 1; j <= MatSize; j++) {
            for (i = 1; i <= j - 1; i++) {  // Solve for the upper triangular elements, except for the diagonal when i = j
                sum = theMatrix[i][j];
                for (k = 1; k <= i - 1; k++) {
                    sum = sum - theMatrix[i][k]*theMatrix[k][j];
                }
                theMatrix[i][j] = sum;
            }
            big = 0.0;
            for (i = j; i <= MatSize; i++) {   // For diagonal element use remaining row with the largest values
                sum = theMatrix[i][j];
                for (k = 1; k <= j - 1; k++) {
                    sum = sum - theMatrix[i][k]*theMatrix[k][j];
                }
                theMatrix[i][j] = sum;
                dum = vv[i]*std::fabs(sum);  // Normalize
                if (dum >= big) {  // Pick as the pivot row
                    big = dum;
                    imax = i;
                }
            }

            if (j != imax) { // If true then we need to interchange the row -- a pivot occurred
                for (k = 1; k <= MatSize; k++) {
                    dum = theMatrix[imax][k];
                    theMatrix[imax][k] = theMatrix[j][k];
                    theMatrix[j][k] = dum;
                }
                vv[imax] = vv[j];
            }

            RowPerm[j] = imax;    /* Keep track of the pivots we've done */

            if (j != MatSize) { /* Finally divide by the pivot element */
                dum = 1.0/theMatrix[j][j];
                for (i = j + 1; i <= MatSize; i++) theMatrix[i][j] = theMatrix[i][j]*dum;
            }
        }
    };

    //---------------------------------------------------------------------------
    auto MatrixForwardBackward = [&](TMyMatrix& theMatrix, TMyVector& theVector, TMyIntVector& RowPerm) {
        int j, i, ip, ii;
        double sum;

        ii = 0;
        for (i = 1; i <= MatSize; i++) {
            ip = RowPerm[i];
            sum = theVector[ip];
            theVector[ip] = theVector[i];
            if (ii != 0) {
                for (j = ii; j <= i - 1; j++) {
                    sum = sum - theMatrix[i][j]*theVector[j];
                }
            }
            else if (sum != 0.0) ii = i;
            theVector[i] = sum;
        }
        for (i = MatSize; i >= 1; i--) {
            sum = theVector[i];
            for (j = i + 1; j <= MatSize; j++) {
                sum = sum - theMatrix[i][j]*theVector[j];
            }
            theVector[i] = sum/theMatrix[i][i];
        }
    };

    //---------------------------------------------------------------------------
    auto local_GetIntialConditions_SetMismatch = [&](double fC1, double fVr, double fVi, double local_G, double local_B,
                                                     TMyVector& theB, TMyVector& theX) {
        // Set local values, including slip, internal voltage and current
        fSlip = theX[1];
        fErp  = theX[2];
        fEip  = theX[3];
        fId = (fErp - fVr)*local_G - (fEip - fVi)*local_B;
        fIq = (fEip - fVi)*local_G + (fErp - fVr)*local_B;
        // Set the mismatch
        theB[1] = fVr*fId + fVi*fIq - fPInit;   // Real power balance
        theB[2] =  fWo*fSlip*fEip - fInvTp*(fErp + fC1*(-fIq));
        theB[3] = -fWo*fSlip*fErp - fInvTp*(fEip + fC1*( fId));
    };

    double fC1;  // Constants that depend on the impedances
    double local_G, local_B;
    double fVr, fVi;  // Terminal voltage, real/imagninary
    TMyVector theB;
    TMyVector theX;
    TMyMatrix theMatrix;
    TMyIntVector RowPerm;
    int local_Itr;  // Iteration counter
    double local_T;
    int i;
    double dIdErp, dIdEip;  // Partial of Id w.r.t. each voltage
    double dIqErp, dIqEip;  // Partial of Iq w.r.t. each voltage

    local_Itr = 0;
    fVr = fTermVolt.r;
    fVi = fTermVolt.i;
    fC1 = fX - fXp;

// Set the initial conditions
    theX[1] = fSlip;
    theX[2] = fErp;
    theX[3] = fEip;
    local_T = std::pow(fRa, 2) + std::pow(fXp, 2);
    local_G =  fRa/local_T;
    local_B = -fXp/local_T;

    local_GetIntialConditions_SetMismatch(fC1, fVr, fVi, local_G, local_B, theB, theX);
    // Get partial of currents w.r.t voltages
    dIdErp =  local_G;
    dIdEip = -local_B;
    dIqErp =  local_B;
    dIqEip =  local_G;
    while ((VectorNorm2(theB) > fItrTol) && (local_Itr < fMaxItr)) {
        // Set the Jacobian.  Zero out initially then just set the non-zeros; The power equation
        MatrixZeroOut(theMatrix);
        theMatrix[1][2] =  fVr*local_G + fVi*local_B;
        theMatrix[1][3] = -fVr*local_B + fVi*local_G;
        theMatrix[2][1] =  fWo*fEip;
        theMatrix[3][1] = -fWo*fErp;
        theMatrix[2][2] = -fInvTp*(1 + fC1*(-1)*dIqErp);
        theMatrix[2][3] = fWo*fSlip - fInvTp*fC1*(-1)*dIqEip;
        theMatrix[3][2] = -fWo*fSlip -fInvTp*fC1*(1)*dIdErp;
        theMatrix[3][3] = -fInvTp*(1 + fC1*(1)*dIdEip);
        // Solve Ax=b
        MatrixFactor(theMatrix, RowPerm);
        MatrixForwardBackward(theMatrix, theB, RowPerm);
        for (i = 1; i <= MatSize; i++) theX[i] = theX[i] - theB[i];
        local_GetIntialConditions_SetMismatch(fC1, fVr, fVi, local_G, local_B, theB, theX);
        local_Itr++;
    }

    bool result = (local_Itr < fMaxItr);  // Return true if initialized
    // Note, excepting the reactive power, the final return values have been set in the Set_Mismatch function
    fQInit = -fVr*fIq + fVi*fId;
    return result;
}

//******************************************************************************
// Load-Specific Functions
//******************************************************************************
double GetDL_Total(double local_VoltMag, double DL_Coeff, double DL_VoltBP, double DL_VoltExt, double DL_at_1_PU)
{
    double result;
    if (local_VoltMag <= DL_VoltExt) result = 0;
    else {
        if (local_VoltMag >= DL_VoltBP)
            result = Math_Y_to_X(local_VoltMag, DL_Coeff);
        else result = Math_Y_to_X(DL_VoltBP, DL_Coeff)*(local_VoltMag - DL_VoltExt)/(DL_VoltBP - DL_VoltExt);
        result = result*DL_at_1_PU;
    }
    return result;
}

double GetZIP_Total(double local_VoltMag, double fZVal, double fIVal, double fSVal, double minVoltSLoad_in, double minVoltILoad_in)
{
    double result;
    result = std::pow(local_VoltMag, 2)*fZVal;
// Check reduction in constant power load
    if (local_VoltMag >= minVoltSLoad_in) result = result + fSVal;
    else result = result + 0.5 *( 1 - std::cos(std::numbers::pi*local_VoltMag/minVoltSLoad_in))*fSVal;
// Check reduction in constant current load
    if (local_VoltMag >= minVoltILoad_in) result = result + local_VoltMag*fIVal;
    else result = result + std::sin(std::numbers::pi*0.5*local_VoltMag/minVoltILoad_in)*local_VoltMag*fIVal;
    return result;
}

}

namespace pwUDM::CLOD {

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
    return(1);
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
    int result = pwStringCopy(StrBuf, StrSize, pwModelClassString(ModelClass::Load));
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall allParamCounts(TxParamCounts* numbersOfEverything, double* TimeStepSeconds)
{
    // -------The model-specific parameters-----
    numbersOfEverything->nFloatParams  = N_F_PARAMS;
    numbersOfEverything->nIntParams    = N_I_PARAMS;
    numbersOfEverything->nStrParams    = N_S_PARAMS;
    numbersOfEverything->nStates       = N_STATES;
    numbersOfEverything->nAlgebraics   = N_ALGEBRAICS;
    // ----For nonwindup limits of the model---
    numbersOfEverything->nNonWindUpLimits = N_NONWINDUPLIMITS;
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* ts;

    // Double parameters
    switch ((*ParamNum))
    {
        case PARAM_PercLmotor: ts = L"%Lmotor"; break;
        case PARAM_PercSmotor: ts = L"%Smotor"; break;
        case PARAM_PercTex:    ts = L"%Tex"; break;
        case PARAM_PercDis:    ts = L"%Dis"; break;
        case PARAM_PercP:      ts = L"%P"; break;
        case PARAM_Kp:         ts = L"Kp"; break;
        case PARAM_Vi:         ts = L"Low V Trip Pickup (pu)"; break;
        case PARAM_Ti:         ts = L"Low V Trip Timer (sec)"; break;
        case PARAM_Tb:         ts = L"Low V Trip Breaker Delay (sec)"; break;
        default:               ts = L""; break;
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
        case STATE_IM1SpeedWr: ts = L"IM 1 Speed wr"; break;
        case STATE_IM1Epr:     ts = L"IM 1 Epr"; break;
        case STATE_IM1Epi:     ts = L"IM 1 Epi"; break;
        case STATE_IM2SpeedWr: ts = L"IM 2 Speed wr"; break;
        case STATE_IM2Epr:     ts = L"IM 2 Epr"; break;
        case STATE_IM2Epi:     ts = L"IM 2 Epi"; break;
        default:               ts = L""; break;
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

    pDoubArr = ParamsAndStates->FloatParams;
    (*pDoubArr)[PARAM_PercLmotor]  = 25.0;
    (*pDoubArr)[PARAM_PercSmotor]  = 25.0;
    (*pDoubArr)[PARAM_PercTex   ]  =  0.0;
    (*pDoubArr)[PARAM_PercDis   ]  = 20.0;
    (*pDoubArr)[PARAM_PercP     ]  =  0.0;
    (*pDoubArr)[PARAM_Kp        ]  =  1.0;
    (*pDoubArr)[PARAM_Vi        ]  =  0.0;
    (*pDoubArr)[PARAM_Ti        ]  =  0.0;
    (*pDoubArr)[PARAM_Tb        ]  =  0.05;
}

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
extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TxMyModelData* ParamsAndStates, double* TimeStepSeconds)
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
    //--------------------------------------------------------------------------
    auto LOCAL_InitalizeMotor = [&](PWDComplex LoadObjBusVolt_In,
                                    double _fPInitial, double _fQInitial, double _SlipInitial, double _Rs, double _Xs, double _fWo, double fInvTp, double _Xp, double _X,
                                    double& SpeedState, double& EprState, double& EpiState, double& _Tnom) {
        double Damping;
        PWDComplex localCurrent;
        PWDComplex local_Ep;
        PWDComplex local_IIn;
        PWDComplex local_impedanceSystem;
        double fStateIr_In, fStateIi_In;
        double StateElectTorque;
        double t3, t4, t5;
        double tQInit;

        localCurrent = PWDComplex_Init(_fPInitial, _fQInitial).Divide(LoadObjBusVolt_In);
        fStateIr_In = localCurrent.r;
        fStateIi_In = - localCurrent.i;
        local_IIn = PWDComplex_Init(fStateIr_In, fStateIi_In);
        local_impedanceSystem = PWDComplex_Init(_Rs, _Xp);
        // Get the initial internal volage and current.  Note, these values are in the network reference frame.
        local_Ep = LoadObjBusVolt_In.CDec(local_IIn.Multiply(local_impedanceSystem));

        t3 = -fStateIr_In;
        t4 = -fStateIi_In;
        t5 = -_fPInitial;

        TxInductionMachine_GetInitialConditions(_fWo,
                                                t5,
                                                _Rs, _X, _Xp, _Xs, fInvTp, LoadObjBusVolt_In, 10, 1e-5,
                                                _SlipInitial, local_Ep.r, local_Ep.i,
                                                t3, t4, tQInit);
        fStateIr_In = -t3;
        fStateIi_In = -t4;

        // The state variables,
        SpeedState = 1 - _SlipInitial;
        EprState = local_Ep.r;
        EpiState = local_Ep.i;

        StateElectTorque = EprState*fStateIr_In + EpiState*fStateIi_In;
        Damping = 2;   // damping, same as fDamp
        _Tnom = StateElectTorque/Math_Y_to_X(1 - _SlipInitial, Damping);
    };
    //--------------------------------------------------------------------------

    // The state variables,
    double IMLSpeedState, IMLEprState, IMLEpiState,
           IMSSpeedState, IMSEprState, IMSEpiState;

    double IML_fPInitial, IML_fQInitial, IML_SlipInitial, IML_Rs, IML_Xs, IML_Xp, IML_X, IML_fWo, IMLfInvTp, IML_Tnom;
    double IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial, IMS_Rs, IMS_Xs, IMS_Xp, IMS_X, IMS_fWo, IMSfInvTp, IMS_Tnom;

    PWDComplex LoadObjBusVolt_In;
    double dTemp;
    double LoadObjV_In, LoadObjThetaRadian_In;
    bool inService_In;

    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    DoubleArray* pAlgebraicsArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pAlgebraicsArr = ParamsAndStates->Algebraics;

    LoadObjV_In           = (*pHardCodedArr)[HARDCODE_LOAD_DeviceVPU];
    LoadObjThetaRadian_In = (*pHardCodedArr)[HARDCODE_LOAD_DeviceAngleRad];
    dTemp                 = (*pHardCodedArr)[HARDCODE_LOAD_DeviceStatus];    // Inservice
    inService_In = dTemp > 0.5;
    LoadObjBusVolt_In = PWDComplex_InitPolarRad(LoadObjV_In, LoadObjThetaRadian_In);

    // following were all initialized in LoadInitializeAlgebraic call
    // Just copy them over here!
    IML_Rs         = (*pAlgebraicsArr)[ALG_IML_Rs        ];
    IML_Xs         = (*pAlgebraicsArr)[ALG_IML_Xs        ];
    IML_fWo        = (*pAlgebraicsArr)[ALG_IML_fWo       ];
    IMLfInvTp      = (*pAlgebraicsArr)[ALG_IMLfInvTp     ];
    IML_Xp         = (*pAlgebraicsArr)[ALG_IML_Xp        ];
    IML_X          = (*pAlgebraicsArr)[ALG_IML_X         ];
    IML_SlipInitial= (*pAlgebraicsArr)[ALG_IML_SlipInitial];
    IML_fPInitial  = (*pAlgebraicsArr)[ALG_IML_fPInitial ];
    IML_fQInitial  = (*pAlgebraicsArr)[ALG_IML_fQInitial ];

    IMS_Rs         = (*pAlgebraicsArr)[ALG_IMS_Rs         ];
    IMS_Xs         = (*pAlgebraicsArr)[ALG_IMS_Xs         ];
    IMS_fWo        = (*pAlgebraicsArr)[ALG_IMS_fWo        ];
    IMSfInvTp      = (*pAlgebraicsArr)[ALG_IMSfInvTp      ];
    IMS_Xp         = (*pAlgebraicsArr)[ALG_IMS_Xp         ];
    IMS_X          = (*pAlgebraicsArr)[ALG_IMS_X          ];
    IMS_SlipInitial= (*pAlgebraicsArr)[ALG_IMS_SlipInitial];
    IMS_fPInitial  = (*pAlgebraicsArr)[ALG_IMS_fPInitial  ];
    IMS_fQInitial  = (*pAlgebraicsArr)[ALG_IMS_fQInitial  ];

    // LARGE INDUCTION MOTOR
    if (inService_In) {
        //--------------------------------------------------------------------------
        // LARGE MOTOR
        LOCAL_InitalizeMotor(LoadObjBusVolt_In,
                             IML_fPInitial, IML_fQInitial, IML_SlipInitial, IML_Rs, IML_Xs, IML_fWo, IMLfInvTp, IML_Xp, IML_X,
                             IMLSpeedState, IMLEprState, IMLEpiState, IML_Tnom);
        //--------------------------------------------------------------------------
        // SMALL MOTOR
        LOCAL_InitalizeMotor(LoadObjBusVolt_In,
                             IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial, IMS_Rs, IMS_Xs, IMS_fWo, IMSfInvTp, IMS_Xp, IMS_X,
                             IMSSpeedState, IMSEprState, IMSEpiState, IMS_Tnom);
    }
    else { // If out-of-service then set all to zero
        IMLSpeedState = 0;
        IMLEprState = 0;
        IMLEpiState = 0;
        IML_Tnom = 0;

        IMSSpeedState = 0;
        IMSEprState = 0;
        IMSEpiState = 0;
        IMS_Tnom = 0;
    }

    // Write out initial state vector,
    (*pStatesArr)[STATE_IM1SpeedWr] = IMLSpeedState;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
    (*pStatesArr)[STATE_IM1Epr    ] = IMLEprState;
    (*pStatesArr)[STATE_IM1Epi    ] = IMLEpiState;
    (*pStatesArr)[STATE_IM2SpeedWr] = IMSSpeedState;
    (*pStatesArr)[STATE_IM2Epr    ] = IMSEprState;
    (*pStatesArr)[STATE_IM2Epi    ] = IMSEpiState;

    // These values just need to be passed around
    (*pAlgebraicsArr)[ALG_IML_Tnom      ] = IML_Tnom;
    (*pAlgebraicsArr)[ALG_IMS_Tnom      ] = IMS_Tnom;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall calculateFofX(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits, DoubleArray* dotX)
{
    auto LOCAL_GetMotorDerivatives = [&](PWDComplex LoadObjBusVolt_In,
                                         bool inService_In,
                                         double loadScalar_in,
                                         double _fPInitial, double _Rs, double _fWo, double fInvTp, double _Xp, double _X, double _D, double _fInv2H,
                                         double SpeedState, double EprState, double EpiState, double _Tnom,
                                         double& dotSpeedState, double& dotEprState, double& dotEpiState) {
        double fStateIr_In, fStateIi_In;
        double StateElectTorque;
        double MechanicalTorque;
        double stateSlip;
        PWDComplex StateCurrent, IntVolt, ImpSys;

        IntVolt = PWDComplex_Init(EprState, EpiState);
        ImpSys = PWDComplex_Init(_Rs, _Xp);
        IntVolt = LoadObjBusVolt_In.CDec(IntVolt);
        StateCurrent = PWDComplex_Divide(IntVolt, ImpSys);
        fStateIr_In = StateCurrent.r;
        fStateIi_In = StateCurrent.i;

        StateElectTorque = EprState*fStateIr_In + EpiState*fStateIi_In;
        if (inService_In) MechanicalTorque = Math_Y_to_X(SpeedState, _D)*_Tnom;
        else MechanicalTorque = 0;
        dotSpeedState = _fInv2H*(StateElectTorque - MechanicalTorque);

    // For the large induction motor dotEpr
        stateSlip = 1 - SpeedState;
        dotEprState = (stateSlip*EpiState*_fWo - fInvTp*(EprState + (_X-_Xp)*loadScalar_in*fStateIi_In));

    // For the large induction motor dotEpi (see TTxGenericInductionMotorModel.dotEpi):
        dotEpiState = (-stateSlip*EprState*_fWo - fInvTp*(EpiState - (_X-_Xp)*loadScalar_in*fStateIr_In));
    };


    double loadScalar_in,
           local_BusVoltMag,
           LoadObjThetaRadian_In;
    PWDComplex LoadObjBusVolt_In;

    double dtemp;
    bool inService_In;

    // The state variables,
    double IMLSpeedState, IMLEprState, IMLEpiState,
           IMSSpeedState, IMSEprState, IMSEpiState;

    // The FofX or xdot,
    double dotIMLSpeedState, dotIMLEprState, dotIMLEpiState,
           dotIMSSpeedState, dotIMSEprState, dotIMSEpiState;

    double IML_fPInitial, IML_fWo, IMLfInvTp, IML_Rs, IML_Xp, IML_X, IML_H, IML_D, IML_fInv2H, IML_Tnom;
    double IMS_fPInitial, IMS_fWo, IMSfInvTp, IMS_Rs, IMS_Xp, IMS_X, IMS_H, IMS_D, IMS_fInv2H, IMS_Tnom;

    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    DoubleArray* pAlgebraicsArr;
    DoubleArray* pDotXArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pAlgebraicsArr = ParamsAndStates->Algebraics;
    pDotXArr = dotX;

    IMLSpeedState = (*pStatesArr)[STATE_IM1SpeedWr];
    IMLEprState   = (*pStatesArr)[STATE_IM1Epr    ];
    IMLEpiState   = (*pStatesArr)[STATE_IM1Epi    ];
    IMSSpeedState = (*pStatesArr)[STATE_IM2SpeedWr];
    IMSEprState   = (*pStatesArr)[STATE_IM2Epr    ];
    IMSEpiState   = (*pStatesArr)[STATE_IM2Epi    ];

    dtemp                 = (*pHardCodedArr)[HARDCODE_LOAD_DeviceStatus];    // Inservice
    inService_In = dtemp > 0.5;
    loadScalar_in         = (*pHardCodedArr)[HARDCODE_LOAD_LoadScalar];

    // Following values are all calculation
    IML_Rs         = (*pAlgebraicsArr)[ALG_IML_Rs        ];
    IML_fWo        = (*pAlgebraicsArr)[ALG_IML_fWo       ];
    IMLfInvTp      = (*pAlgebraicsArr)[ALG_IMLfInvTp     ];
    IML_Xp         = (*pAlgebraicsArr)[ALG_IML_Xp        ];
    IML_X          = (*pAlgebraicsArr)[ALG_IML_X         ];
    IML_fPInitial  = (*pAlgebraicsArr)[ALG_IML_fPInitial ];

    IMS_Rs         = (*pAlgebraicsArr)[ALG_IMS_Rs        ];
    IMS_fWo        = (*pAlgebraicsArr)[ALG_IMS_fWo       ];
    IMSfInvTp      = (*pAlgebraicsArr)[ALG_IMSfInvTp     ];
    IMS_Xp         = (*pAlgebraicsArr)[ALG_IMS_Xp        ];
    IMS_X          = (*pAlgebraicsArr)[ALG_IMS_X         ];
    IMS_fPInitial  = (*pAlgebraicsArr)[ALG_IMS_fPInitial ];

    IML_Tnom       = (*pAlgebraicsArr)[ALG_IML_Tnom      ];
    IMS_Tnom       = (*pAlgebraicsArr)[ALG_IMS_Tnom      ];

    // Calculate the Currents
    local_BusVoltMag = (*pHardCodedArr)[HARDCODE_LOAD_DeviceVPU];
    LoadObjThetaRadian_In = (*pHardCodedArr)[HARDCODE_LOAD_DeviceAngleRad];
    LoadObjBusVolt_In = PWDComplex_InitPolarRad(local_BusVoltMag, LoadObjThetaRadian_In);

    //----------------------------------------------------------------------------
    // For the large induction motor dotWr:
    //----------------------------------------------------------------------------
    IML_H = 1.5;
    IML_D = 2;   // damping, same as fDamp
    IML_fInv2H = (1/(2*IML_H))*(1/IML_fPInitial);

    LOCAL_GetMotorDerivatives(LoadObjBusVolt_In, inService_In, loadScalar_in,
                              IML_fPInitial, IML_Rs, IML_fWo, IMLfInvTp, IML_Xp, IML_X, IML_D, IML_fInv2H,
                              IMLSpeedState, IMLEprState, IMLEpiState, IML_Tnom,
                              dotIMLSpeedState, dotIMLEprState, dotIMLEpiState
                              );

    //----------------------------------------------------------------------------
    // For the small induction motor dotWr:
    //----------------------------------------------------------------------------
    IMS_H = 0.7;
    IMS_D = 2;
    IMS_fInv2H = (1/(2*IMS_H))*(1/IMS_fPInitial);

    LOCAL_GetMotorDerivatives(LoadObjBusVolt_In, inService_In, loadScalar_in,
                              IMS_fPInitial, IMS_Rs, IMS_fWo, IMSfInvTp, IMS_Xp, IMS_X, IMS_D, IMS_fInv2H,
                              IMSSpeedState, IMSEprState, IMSEpiState, IMS_Tnom,
                              dotIMSSpeedState, dotIMSEprState, dotIMSEpiState
                              );

// write out new fofx
    (*pDotXArr)[STATE_IM1SpeedWr] = dotIMLSpeedState;
    (*pDotXArr)[STATE_IM1Epr    ] = dotIMLEprState;
    (*pDotXArr)[STATE_IM1Epi    ] = dotIMLEpiState;
    (*pDotXArr)[STATE_IM2SpeedWr] = dotIMSSpeedState;
    (*pDotXArr)[STATE_IM2Epr    ] = dotIMSEprState;
    (*pDotXArr)[STATE_IM2Epi    ] = dotIMSEpiState;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions)
{
    // nothing here
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, TxNonWindUpLimits* nonWindUpLimits)
{
    return(0);   // None
}

//------------------------------------------------------------------------------
// 'TimeStepEnd'
extern "C" __declspec(dllexport) bool __stdcall TimeStepEnd(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* index, int* MaxPossibleEventIndex, double* EventTime, int* ExtraObjectIndex)
{
    double local_NewTime;
    double fVi, fTi, fTb;
    double local_VoltMag;
    double fLowVoltTime;
    bool fLowVoltTimeSet;
    bool inService_In;

    DoubleArray* pFloatParamArr;
    DoubleArray* pHardCodedArr;
    DoubleArray* pAlgebraicsArr;
    bool result;

    result = false; // Only return TRUE if it actually needs to do something!
    *EventTime = 0;
    *ExtraObjectIndex = -1;
    *MaxPossibleEventIndex = 0;
    if (*index == 0) {
        //--------------------------------------------------------------------
        // Setup local array variables to make code easier to read
        //--------------------------------------------------------------------
        pFloatParamArr = ParamsAndStates->FloatParams;
        pHardCodedArr = ParamsAndStates->HardCodedSignals;
        pAlgebraicsArr = ParamsAndStates->Algebraics;

        inService_In    = (*pHardCodedArr)[HARDCODE_LOAD_DeviceStatus] > 0.5; // Inservice
        fVi = (*pFloatParamArr)[PARAM_Vi];
        fTi = (*pFloatParamArr)[PARAM_Ti];
        fTb = (*pFloatParamArr)[PARAM_Tb];
        local_VoltMag   = (*pHardCodedArr)[HARDCODE_LOAD_DeviceVPU];
        fLowVoltTimeSet = (*pAlgebraicsArr)[ALG_TIMER_LowVoltTimeSet] != 0;
        fLowVoltTime    = (*pAlgebraicsArr)[ALG_TIMER_LowVoltTime];
        local_NewTime   = SystemOptions->SimulationTimeSeconds;

        if (inService_In) {
            if ((local_VoltMag >= fVi)) {
                (*pAlgebraicsArr)[ALG_TIMER_LowVoltTimeSet] = 0; // Above voltage, so reset
            }
            else {
                if (!fLowVoltTimeSet) {
                    fLowVoltTime = local_NewTime; // First time below voltage, so
                    (*pAlgebraicsArr)[ALG_TIMER_LowVoltTime] = fLowVoltTime;
                    (*pAlgebraicsArr)[ALG_TIMER_LowVoltTimeSet] = 1;
                }
                if ((local_NewTime - fLowVoltTime) >= fTi) { // Trip threshold exceeded
                    *EventTime = local_NewTime + fTb;
                    *ExtraObjectIndex = -1; // trip itself!
                    result = true; // It needs to do something!
                }
            }
        }
    }
    else {
        result = false;
    }
    return result;
}

//******************************************************************************
extern "C" __declspec(dllexport) int __stdcall TimeStepEndAction(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, int* index, int* StrSize, wchar_t* StrBuf, int dummy)
{
    const wchar_t* ts;

    // Anything after the last Pipe character | will be treated as an extra log message to show in event reporting
    switch (*index)
    {
        case 0: ts = L"OPEN|User_CLOD Under Voltage Trip: "; break;
        default: ts = L""; break;
    }

    int result = pwStringCopy(StrBuf, StrSize, ts);
    return result;
}

extern "C" __declspec(dllexport) void __stdcall LoadNortonAdmittance(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* theG, double* theB)
{
    double t;
    double IML_Rs, IML_Xp;
    double IMS_Rs, IMS_Xp;
    double ZIP_ZMW, ZIP_ZMVR;
    double loadScalar_in;

    DoubleArray* pHardCodedArr;
    DoubleArray* pAlgebraicsArr;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pAlgebraicsArr = ParamsAndStates->Algebraics;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;

    *theG = 0;
    *theB = 0;

    // Norton Equivalent from the Large Motor
    IML_Rs = (*pAlgebraicsArr)[ALG_IML_Rs];
    IML_Xp = (*pAlgebraicsArr)[ALG_IML_Xp];
    t = std::pow(IML_Rs, 2) + std::pow(IML_Xp, 2);
    if (t != 0) {
        *theG = *theG + (IML_Rs)/t;
        *theB = *theB - (IML_Xp)/t;
    }

    // Norton Equivalent from the Small Motor
    IMS_Rs = (*pAlgebraicsArr)[ALG_IMS_Rs];
    IMS_Xp = (*pAlgebraicsArr)[ALG_IMS_Xp];
    t = std::pow(IMS_Rs, 2) + std::pow(IMS_Xp, 2);
    if (t != 0) {
        *theG = *theG + (IMS_Rs)/t;
        *theB = *theB - (IMS_Xp)/t;
    }

    // Constant impedance protion of load in here too
    ZIP_ZMW = (*pAlgebraicsArr)[ALG_ZIP_ZMW];
    ZIP_ZMVR = (*pAlgebraicsArr)[ALG_ZIP_ZMVR];
    *theG = *theG + ZIP_ZMW;
    *theB = *theB - ZIP_ZMVR; // ??? sign not sure of

    // Handle the Load Scalar changing these as well
    loadScalar_in = (*pHardCodedArr)[HARDCODE_LOAD_LoadScalar];
    *theG = *theG * loadScalar_in;
    *theB = *theB * loadScalar_in;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall LoadNortonCurrent(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* IReal, double* IImag)
{
    PWDComplex ImpSys;
    PWDComplex IntVolt;
    PWDComplex NortonCurrent;
    PWDComplex LoadObjBusVolt_In;

    double DL_TotalP, DL_TotalQ,
           IML_Rs, IML_Xp,
           IMS_Rs, IMS_Xp,
           IMLEprState, IMLEpiState,
           IMSEprState, IMSEpiState;

    double local_BusVoltMag,
           LoadObjThetaRadian_In;

    double DL_P_at_1_PU, DL_Q_at_1_PU,
           ZIP_IMW, ZIP_SMW,
           ZIP_IMVR, ZIP_SMVR;

    double loadScalar_in,
           local_e, local_f, local_v2Inv, local_BusVoltMagInv,
           local_ge, local_he, local_gf, local_hf,
           local_ki, local_ks;

    double minVoltSLoad_in, minVoltILoad_in;

    DoubleArray* pStatesArr;
    DoubleArray* pHardCodedArr;
    DoubleArray* pAlgebraicsArr;

    *IReal = 0;
    *IImag = 0;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pStatesArr = ParamsAndStates->States;
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pAlgebraicsArr = ParamsAndStates->Algebraics;

    loadScalar_in = (*pHardCodedArr)[HARDCODE_LOAD_LoadScalar];
    minVoltSLoad_in = SystemOptions->MinVoltSLoad;
    minVoltILoad_in = SystemOptions->MinVoltILoad;

    // Network Voltage Presently
    local_BusVoltMag = (*pHardCodedArr)[HARDCODE_LOAD_DeviceVPU];
    LoadObjThetaRadian_In = (*pHardCodedArr)[HARDCODE_LOAD_DeviceAngleRad];
    LoadObjBusVolt_In = PWDComplex_InitPolarRad(local_BusVoltMag, LoadObjThetaRadian_In);

    //--------------------------------------------------------------------
    // Large Motor
    //--------------------------------------------------------------------
    IMLEprState = (*pStatesArr)[STATE_IM1Epr    ];
    IMLEpiState = (*pStatesArr)[STATE_IM1Epi    ];
    IML_Rs = (*pAlgebraicsArr)[ALG_IML_Rs]/loadScalar_in;
    IML_Xp = (*pAlgebraicsArr)[ALG_IML_Xp]/loadScalar_in;
    ImpSys = PWDComplex_Init(IML_Rs, IML_Xp);
    IntVolt = PWDComplex_Init(IMLEprState, IMLEpiState);
    NortonCurrent = PWDComplex_Divide(IntVolt, ImpSys);
    // must take NEGATIVE here because the equation is for "generator"
    *IReal = *IReal - NortonCurrent.r;
    *IImag = *IImag - NortonCurrent.i;

    //--------------------------------------------------------------------
    // Small Motor
    //--------------------------------------------------------------------
    IMSEprState = (*pStatesArr)[STATE_IM2Epr    ];
    IMSEpiState = (*pStatesArr)[STATE_IM2Epi    ];
    IMS_Rs = (*pAlgebraicsArr)[ALG_IMS_Rs]/loadScalar_in;
    IMS_Xp = (*pAlgebraicsArr)[ALG_IMS_Xp]/loadScalar_in;
    ImpSys = PWDComplex_Init(IMS_Rs, IMS_Xp);
    IntVolt = PWDComplex_Init(IMSEprState, IMSEpiState);
    NortonCurrent = PWDComplex_Divide(IntVolt, ImpSys);
    // must take NEGATIVE here because the equation is for "generator"
    *IReal = *IReal - NortonCurrent.r;
    *IImag = *IImag - NortonCurrent.i;

    //--------------------------------------------------------------------
    // DISCHARGE LIGHTING VALUES
    //--------------------------------------------------------------------
    DL_P_at_1_PU = (*pAlgebraicsArr)[ALG_DL_P_at_1_PU]*loadScalar_in;
    DL_Q_at_1_PU = (*pAlgebraicsArr)[ALG_DL_Q_at_1_PU]*loadScalar_in;
    DL_TotalP = GetDL_Total(local_BusVoltMag, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
    DL_TotalQ = GetDL_Total(local_BusVoltMag, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
    NortonCurrent = PWDComplex_Init(DL_TotalP, DL_TotalQ).Divide(LoadObjBusVolt_In);
    NortonCurrent.i = - NortonCurrent.i; // conjugate!  I = (S/V)*
    *IReal = *IReal + NortonCurrent.r;
    *IImag = *IImag + NortonCurrent.i;

    //----------------------------------------------------------------------------
    // Constant Current and Constant Power Portion of Load
    // Equations being solved for:
    //----------------------------------------------------------------------------
    // I = (S/V)* = (S*)/(V*)   --> conjugate
    //
    //                Constant Power                   Constant Current
    // Ir + jIi = ks*[(Psl - jQsl)/(e-jf)]       + ki*[(Pil - jQil)*sqrt(e^2+f^2)/(e-jf)]
    //          = ks*[(e+jf)(Psl - jQsl)/V^2]    + ki*[(e+jf)*(Pil - jQil)/V]
    //
    // Ir =       ks*[e/(V^2)*Psl + f/(V^2)*Qsl] + ki*[e/V*Pil + f/V*Qil]
    // Ii =       ks*[f/(V^2)*Psl - e/(V^2)*Qsl] + ki*[f/V*Pil - e/V*Qil]
    //
    // Now define
    //   ge = e/(V^2)   gf = f/(V^2)  he = e/V    hf = f/V
    //
    //         Constant Power             Constant Current
    // Ir = [ks * (ge*Psl + gf*Qsl)] + [ki * (he*Pil + hf*Qil)]
    // Ii = [ks * (gf*Psl - ge*Qsl)] + [ki * (hf*Pil - he*Qil)]
    //
    // where Psl, Qsl, Pil, Qil : contants
    //       ge, gf, he, hf     : functions of voltage
    //       ks, ki             : might be functions of voltage
    //                            (will be 1.0 if not functions)
    //----------------------------------------------------------------------------
    // Note: the constant impedance is handled in the Norton Admittance
    local_e = LoadObjBusVolt_In.r;
    local_f = LoadObjBusVolt_In.i;
    NortonCurrent.r = 0;
    NortonCurrent.i = 0;
    ZIP_SMW      = (*pAlgebraicsArr)[ALG_ZIP_SMW ]*loadScalar_in;
    ZIP_SMVR     = (*pAlgebraicsArr)[ALG_ZIP_SMVR]*loadScalar_in;
    if ((ZIP_SMW != 0) || (ZIP_SMVR != 0)) {
        // avoid a divide by zero!
        local_v2Inv = std::pow(local_e, 2) + std::pow(local_f, 2);
        if (local_v2Inv > std::pow(1E-5, 2)) local_v2Inv = 1/(local_v2Inv);
        else local_v2Inv = 1E6; // just set it to something!
        local_ge = local_e*local_v2Inv;
        local_gf = local_f*local_v2Inv;
        if ((local_BusVoltMag < minVoltSLoad_in)) {
            local_ks = 0.5 * ( 1 - std::cos(std::numbers::pi*local_BusVoltMag/minVoltSLoad_in) );
            NortonCurrent.r = NortonCurrent.r + local_ks*(local_ge*ZIP_SMW + local_gf*ZIP_SMVR);
            NortonCurrent.i = NortonCurrent.i + local_ks*(local_gf*ZIP_SMW - local_ge*ZIP_SMVR);
        }
        else {
            NortonCurrent.r = NortonCurrent.r + (local_ge*ZIP_SMW + local_gf*ZIP_SMVR);
            NortonCurrent.i = NortonCurrent.i + (local_gf*ZIP_SMW - local_ge*ZIP_SMVR);
        }
    }

    ZIP_IMW      = (*pAlgebraicsArr)[ALG_ZIP_IMW]*loadScalar_in;
    ZIP_IMVR     = (*pAlgebraicsArr)[ALG_ZIP_IMVR]*loadScalar_in;
    if ((ZIP_IMW != 0) || (ZIP_IMVR != 0)) {
        local_BusVoltMagInv = 1/local_BusVoltMag;
        local_he = local_e*local_BusVoltMagInv;
        local_hf = local_f*local_BusVoltMagInv;
        if ((local_BusVoltMag < minVoltILoad_in)) {
            local_ki = std::sin(std::numbers::pi*0.5*local_BusVoltMag/minVoltILoad_in);
            NortonCurrent.r = NortonCurrent.r + local_ki*(local_he*ZIP_IMW + local_hf*ZIP_IMVR);
            NortonCurrent.i = NortonCurrent.i + local_ki*(local_hf*ZIP_IMW - local_he*ZIP_IMVR);
        }
        else {
            NortonCurrent.r = NortonCurrent.r + (local_he*ZIP_IMW + local_hf*ZIP_IMVR);
            NortonCurrent.i = NortonCurrent.i + (local_hf*ZIP_IMW - local_he*ZIP_IMVR);
        }
    }
    *IReal = *IReal + NortonCurrent.r;
    *IImag = *IImag + NortonCurrent.i;
}

//******************************************************************************
extern "C" __declspec(dllexport) void __stdcall LoadNortonCurrentAlgebraicDerivative(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double* IReal_dVreal, double* IReal_dVimag, double* IImag_dVreal, double* IImag_dVimag)
{
//{****************************************************************************************************************}
//  { Class Procedure TTxDynamicLoadModel.CurrentDerivativesExponentialVoltage returns with the derivatives of the current w.r.t. to the rectangular
//  voltage for functions of the form local_Po*V^local_Pn and local_Qo*V^local_Qn. }
    auto CurrentDerivativesExponentialVoltage = [&](double local_Po, double local_Qo, double local_Pn, double local_Qn, PWDComplex local_BusVoltage,
                                                    double& dIr_de, double& dIr_df, double& dIi_de, double& dIi_df) {
        double local_V, local_e, local_f;  // Voltage magnitude, e and f values
        double vp2, vp4, vq2, vq4;  // Exponential functions

        /* For this model we then it is just an exponential model, with Po*V^(Pn) + Qo*V^(Qn).
          That is, with expontial models we have
          I = (S*)/(V*) (V* = complex conjugate of V)
          Ir + jIi = ( Po*V^(Pn)   - jQo*V^(Qn)   )/(e-jf)
                      multiply by (e+jf)/(e+fj)
                   = ( Po*V^(Pn)   - jQo*V^(Qn)   )/(e^2 + f^2)*(e+jf)
                   = ( Po*V^(Pn)   - jQo*V^(Qn)   )/(V^2)      *(e+jf)
                   = ( Po*V^(Pn-2) - jQo*V^(Qn-2) )*(e+fj)
                   = [ e*Po*V^(Pn-2) + f*Qo*V^(Qn-2) ] + j[ f*Po*V^(Pn-2) - e*Qo*V^(Qn-2) ]

            Ir = e*Po*V^(Pn-2) + f*Qo*V^(Qn-2)
            Ii = f*Po*V^(Pn-2) - e*Qo*V^(Qn-2)
          So use the chain rule to get the values.
            dIr/de = Po*V^(Pn-2) + e*Po*(Pn-2)V^(Pn-3)*dV/de + f*Qo*(Qn-2)V^(Qn-3)*dV/de
                     // use dV/de = e/V  (later you use dV/df = f/V)
                   = Po*V^(Pn-2) + e*Po*(Pn-2)V^(Pn-3)*e/V + f*Qo*(Qn-2)V^(Qn-3)*e/V
                   = Po*V^(Pn-2) + [e*Po*(Pn-2)V^(Pn-4) + f*Qo*(Qn-2)V^(Qn-4)]*e

            Let vp2 = Po*V^(Pn-2)
                vp4 = Po*(Pn-2)V^(Pn-4)
                vq4 = Qo*(Qn-2)V^(Qn-4)

            Then dIr/de = vp2 + (vp4*e + vq4*f)*e
         */
        local_V = local_BusVoltage.Mag();
        local_e = local_BusVoltage.r;
        local_f = local_BusVoltage.i;
        vp2 = local_Po*Math_Y_to_X(local_V, local_Pn - 2);
        vp4 = (local_Pn - 2)*local_Po*Math_Y_to_X(local_V, local_Pn - 4);  // Use -4 for exponent since it includes /V from partial of e or f w.r.t V
        vq2 = local_Qo*Math_Y_to_X(local_V, local_Qn - 2);
        vq4 = (local_Qn - 2)*local_Qo*Math_Y_to_X(local_V, local_Qn - 4);  // Use -4 for exponent since it includes /V from partial of e or f w.r.t V

        dIr_de = dIr_de + vp2 + (vp4*local_e + vq4*local_f)*local_e;
        dIr_df = dIr_df + vq2 + (vp4*local_e + vq4*local_f)*local_f;
        dIi_de = dIi_de - vq2 + (vp4*local_f - vq4*local_e)*local_e;
        dIi_df = dIi_df + vp2 + (vp4*local_f - vq4*local_e)*local_f;
    };

    PWDComplex LoadObjBusVolt_In;

    double DL_TotalP, DL_TotalQ,
           local_BusVoltMag,
           LoadObjThetaRadian_In;

    double DL_P_at_1_PU, DL_Q_at_1_PU,
           ZIP_IMW, ZIP_SMW,
           ZIP_IMVR, ZIP_SMVR;

    double loadScalar_in,
           local_Inv_v2,
           local_v,
           local_e, local_f, local_v2,
           local_ki, local_ks,
           local_Ir_Num, local_Ii_Num,
           dks_dv, dks_de, dks_df, local_Inv_v4_ks,
           dki_dv, dki_de, dki_df, local_eOverV, local_fOverV;

    double minVoltSLoad_in, minVoltILoad_in;
    double ZIP_dIr_de, ZIP_dIr_df, ZIP_dIi_de, ZIP_dIi_df,
           DL_dIr_de, DL_dIr_df, DL_dIi_de, DL_dIi_df;

    DoubleArray* pHardCodedArr;
    DoubleArray* pAlgebraicsArr;

    DL_dIr_de = 0;
    DL_dIr_df = 0;
    DL_dIi_de = 0;
    DL_dIi_df = 0;

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pHardCodedArr = ParamsAndStates->HardCodedSignals;
    pAlgebraicsArr = ParamsAndStates->Algebraics;

    loadScalar_in = (*pHardCodedArr)[HARDCODE_LOAD_LoadScalar];
    minVoltSLoad_in = SystemOptions->MinVoltSLoad;
    minVoltILoad_in = SystemOptions->MinVoltILoad;

    // Network Voltage Presently
    local_BusVoltMag   =(*pHardCodedArr)[HARDCODE_LOAD_DeviceVPU];
    LoadObjThetaRadian_In = (*pHardCodedArr)[HARDCODE_LOAD_DeviceAngleRad];
    LoadObjBusVolt_In = PWDComplex_InitPolarRad(local_BusVoltMag, LoadObjThetaRadian_In);

    //----------------------------------------------------------------------------
    // DISCHARGE LIGHTING LOAD
    //----------------------------------------------------------------------------
    if (local_BusVoltMag > std::pow(1E-5, 2)) {
        DL_P_at_1_PU = (*pAlgebraicsArr)[ALG_DL_P_at_1_PU]*loadScalar_in;
        DL_Q_at_1_PU = (*pAlgebraicsArr)[ALG_DL_Q_at_1_PU]*loadScalar_in;

    // Easy case is if voltage <= DL_VoltExt; then load is extinguished so all derivatives are zero so we don't do anything
        if (local_BusVoltMag > GLOBAL_DL_VoltExt) {
        /* If voltage is greater than the breakpoint voltage, then it is just an exponential model, with fP^fPCoeff + fQ^fQCoeff.  That is, with expontial
          models we have Ir + jIi = (Po*V^m - jQo*V^n)/(e-jf)*(e+jf)/(e+fj) = Po*V^m - jQo*V^n/(e^2 + f^2)*(e+jf) = Po*V^(m-2) - jQ*Vo^(n-2)*(e+fj).  So
          use the chain rule to get the values. */

            if (local_BusVoltMag >= GLOBAL_DL_VoltBP) {
                CurrentDerivativesExponentialVoltage(DL_P_at_1_PU, DL_Q_at_1_PU,
                                                     GLOBAL_DL_PCoeff, GLOBAL_DL_QCoeff, LoadObjBusVolt_In,
                                                     DL_dIr_de, DL_dIr_df, DL_dIi_de, DL_dIi_df);
            }
            /* Otherwise voltage is between the extinction value and the breakpoint voltage, then real and reactive power is being linearly ramped according
              to equation (P(DL_VoltBP)+jQ(DL_VoltBP))*(V - Vext)/(Vbp - Vext). */
            else {
                // Power at the breakpoint voltage. The voltage-dependent scalar local_ks below supplies the ramp,
                // so using the power at the present voltage here would apply the ramp twice.
                DL_TotalP = GetDL_Total(GLOBAL_DL_VoltBP, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
                DL_TotalQ = GetDL_Total(GLOBAL_DL_VoltBP, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);

                local_e = LoadObjBusVolt_In.r;
                local_f = LoadObjBusVolt_In.i;
                local_v2 = std::pow(local_BusVoltMag, 2);
                local_Inv_v2 = 1/local_v2;

                // Handle the Constant Power Portion of the Load.  The equation being solved here is I = (S/V)*, which becomes
                // (P-jQ)/(e-jf) = (P-jQ)*(e+jf)/(e^2 + f^2).  Derivative values are calculated using d(u/v)/dx = (du*v - dv*u)/(v^2)
                // Model with Ir = (P*e + Q*f)/(e^2 + f^2) and Ii = (P*f - Q*e)/(e^2 + f^2)

                // Calculate the numerator portion of Ir and Ii.
                local_Ir_Num = DL_TotalP*local_e + DL_TotalQ*local_f;
                local_Ii_Num = DL_TotalP*local_f - DL_TotalQ*local_e;

                // Since voltage is between extinction value and breakpoint value, calculate the scalar, ks, and its sensitivities.
                // Then use the chain rule for differentiation.
                local_ks = (local_BusVoltMag - GLOBAL_DL_VoltExt)/(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt);
                local_Inv_v4_ks = local_ks*std::pow(local_Inv_v2, 2);

                // Calculate the derivatives (portion that assumes fixed local_ks)
                DL_dIr_de = DL_dIr_de + (DL_TotalP*local_v2  - local_Ir_Num*2*local_e)*local_Inv_v4_ks;
                DL_dIr_df = DL_dIr_df + (DL_TotalQ*local_v2  - local_Ir_Num*2*local_f)*local_Inv_v4_ks;
                DL_dIi_de = DL_dIi_de + (-DL_TotalQ*local_v2 - local_Ii_Num*2*local_e)*local_Inv_v4_ks;
                DL_dIi_df = DL_dIi_df + (DL_TotalP*local_v2  - local_Ii_Num*2*local_f)*local_Inv_v4_ks;

                // Now include the impact of local_ks being a function of voltage }
                dks_de = local_e/(local_BusVoltMag*(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt));
                dks_df = local_f/(local_BusVoltMag*(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt));
                DL_dIr_de = DL_dIr_de + (local_Ir_Num*local_Inv_v2) * dks_de;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
                DL_dIr_df = DL_dIr_df + (local_Ir_Num*local_Inv_v2) * dks_df;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
                DL_dIi_de = DL_dIi_de + (local_Ii_Num*local_Inv_v2) * dks_de;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
                DL_dIi_df = DL_dIi_df + (local_Ii_Num*local_Inv_v2) * dks_df;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
            }
        }
    }

    ZIP_dIr_de = 0;
    ZIP_dIr_df = 0;
    ZIP_dIi_de = 0;
    ZIP_dIi_df = 0;

    local_e = LoadObjBusVolt_In.r;
    local_f = LoadObjBusVolt_In.i;
    local_v2 = std::pow(local_e, 2) + std::pow(local_f, 2);
    // Avoid a divide by zero
    if (local_v2 > std::pow(1E-5, 2)) local_Inv_v2 = 1/local_v2;
    else local_Inv_v2 = 1E6; // must set it to something then!
    local_v = std::sqrt(local_v2);
    //----------------------------------------------------------------------------
    // CONSTANT POWER PORTION
    //----------------------------------------------------------------------------
    ZIP_SMW     = (*pAlgebraicsArr)[ALG_ZIP_SMW ]*loadScalar_in;
    ZIP_SMVR    = (*pAlgebraicsArr)[ALG_ZIP_SMVR]*loadScalar_in;
    if (((local_v2 > 0) && (ZIP_SMW != 0)) || (ZIP_SMVR != 0)) {
        // Calculate the numerator portion of Ir and Ii.
        local_Ir_Num = ZIP_SMW*local_e + ZIP_SMVR*local_f;
        local_Ii_Num = ZIP_SMW*local_f - ZIP_SMVR*local_e;

        // If voltage less than voltage cutoff then include reduction in level (local_ks) keeping in
        // mind local_ks is also a funtion of voltage.  Use chain rule for differentiation
        if (local_v < minVoltSLoad_in)
            local_ks = 0.5 * ( 1 - std::cos(std::numbers::pi*local_v/minVoltSLoad_in));
        else local_ks = 1;
        local_Inv_v4_ks = local_ks*std::pow(local_Inv_v2, 2);

        // Calculate the derivatives (portion that assumes fixed local_ks)
        ZIP_dIr_de = ZIP_dIr_de + ( ZIP_SMW *local_v2 - local_Ir_Num*2*local_e)*local_Inv_v4_ks;
        ZIP_dIr_df = ZIP_dIr_df + ( ZIP_SMVR*local_v2 - local_Ir_Num*2*local_f)*local_Inv_v4_ks;
        ZIP_dIi_de = ZIP_dIi_de + (-ZIP_SMVR*local_v2 - local_Ii_Num*2*local_e)*local_Inv_v4_ks;
        ZIP_dIi_df = ZIP_dIi_df + ( ZIP_SMW *local_v2 - local_Ii_Num*2*local_f)*local_Inv_v4_ks;

        // If local_v < minVoltSLoad_in then also include impact of local_ks being a function of voltage
        if ((local_v < minVoltSLoad_in)) {
            dks_dv = std::numbers::pi/(2*minVoltSLoad_in) * std::sin(std::numbers::pi*local_v/minVoltSLoad_in);
            dks_de = dks_dv * local_e/local_v;
            dks_df = dks_dv * local_f/local_v;
            ZIP_dIr_de = ZIP_dIr_de + (local_Ir_Num*local_Inv_v2) * dks_de;
            ZIP_dIr_df = ZIP_dIr_df + (local_Ir_Num*local_Inv_v2) * dks_df;
            ZIP_dIi_de = ZIP_dIi_de + (local_Ii_Num*local_Inv_v2) * dks_de;
            ZIP_dIi_df = ZIP_dIi_df + (local_Ii_Num*local_Inv_v2) * dks_df;
        }
    }

    //----------------------------------------------------------------------------
    // CONSTANT CURRENT PORTION
    //----------------------------------------------------------------------------
    // Handle the portion of the power linearly dependent upon the voltage magnitude.  The equation being differentiated here is
    // ((P-jQ)*sqrt(e^2+f^2)/(e-jf) = (P-jQ)*(e+jf)/(Sqrt(e^2+f^2).  Similar to constant power loads, if voltage is below a threshold we
    // need to consider impact of this scalar on voltage.  Again we calculate derivative values using d(u/v)/dx = (du*v - dv*u)/(v^2)
    ZIP_IMW     = (*pAlgebraicsArr)[ALG_ZIP_IMW]*loadScalar_in;
    ZIP_IMVR    = (*pAlgebraicsArr)[ALG_ZIP_IMVR]*loadScalar_in;
    if (((local_v2 > 0) && (ZIP_IMW != 0)) || (ZIP_IMVR != 0)) {
        // Calculate the numerator portion of Ir and Ii.
        local_Ir_Num = ZIP_IMW*local_e + ZIP_IMVR*local_f;
        local_Ii_Num = ZIP_IMW*local_f - ZIP_IMVR*local_e;

        // If voltage less than voltage cutoff then include reduction in level (local_ki) keeping in
        // mind local_ki is also a funtion of voltage.  Use chain rule for differentiation
        if ((local_v < minVoltILoad_in))
            local_ki = std::sin(std::numbers::pi*0.5*local_v/minVoltILoad_in);
        else local_ki = 1;

        local_eOverV = local_e/local_v;
        local_fOverV = local_f/local_v;
        ZIP_dIr_de = ZIP_dIr_de + ( ZIP_IMW *local_v - local_Ir_Num*local_eOverV)*local_Inv_v2*local_ki;
        ZIP_dIr_df = ZIP_dIr_df + ( ZIP_IMVR*local_v - local_Ir_Num*local_fOverV)*local_Inv_v2*local_ki;
        ZIP_dIi_de = ZIP_dIi_de + (-ZIP_IMVR*local_v - local_Ii_Num*local_eOverV)*local_Inv_v2*local_ki;
        ZIP_dIi_df = ZIP_dIi_df + ( ZIP_IMW *local_v - local_Ii_Num*local_fOverV)*local_Inv_v2*local_ki;

    // If local_v < minVoltILoad_in then also include impact of local_ki being a function of voltage
        if (local_v < minVoltILoad_in) {
            dki_dv = std::numbers::pi*0.5/minVoltILoad_in*std::cos(std::numbers::pi*0.5*local_v/minVoltILoad_in);
            dki_de = dki_dv*local_eOverV;
            dki_df = dki_dv*local_fOverV;
            ZIP_dIr_de = ZIP_dIr_de + local_Ir_Num/local_v*dki_de;
            ZIP_dIr_df = ZIP_dIr_df + local_Ir_Num/local_v*dki_df;
            ZIP_dIi_de = ZIP_dIi_de + local_Ii_Num/local_v*dki_de;
            ZIP_dIi_df = ZIP_dIi_df + local_Ii_Num/local_v*dki_df;
        }
    }

    *IReal_dVreal = DL_dIr_de + ZIP_dIr_de;
    *IReal_dVimag = DL_dIr_df + ZIP_dIr_df;
    *IImag_dVreal = DL_dIi_de + ZIP_dIi_de;
    *IImag_dVimag = DL_dIi_df + ZIP_dIi_df;
}

//******************************************************************************
// InitLoadP and InitLoadQ will most frequently
// be equal to the SteadyStateP and SteadyStateQ
// Any difference will be automatically made-up for within
// Simulator by embedding an impedance with the load that makes the
// NetP and NetQ match SteadyStateP and SteadyStateQ initially
extern "C" __declspec(dllexport) bool __stdcall LoadInitializeAlgebraic(TxMyModelData* ParamsAndStates, TxSystemOptions* SystemOptions, double INPUT_PUTol, double SteadyStateP, double SteadyStateQ, double SteadyStateV, double* InitLoadP, double* InitLoadQ)
{
    //----------------------------------------------------------------------------
    auto LOCAL_InitializeAlgebraicMotor = [&](double INPUT_PUTol, double loadObjNomFreq_In, double SteadyStateV,
                                              double& _fPInitial, double& _fQInitial, double& _SlipInitial,
                                              double& _Rs, double& _Xs, double& _Xm, double& _R1, double& _X1,
                                              double& _fWo, double& _Xp, double& _X, double& _fInvTp) {
        double local_Scale;
        unsigned char tempSlipRes;
        PWDComplex local_impedanceSystem;
        double local_TElect;

        local_Scale = 1/_fPInitial;
        _Rs = _Rs*local_Scale;
        _Xs = _Xs*local_Scale;
        _Xm = _Xm*local_Scale;
        _R1 = _R1*local_Scale;
        _X1 = _X1*local_Scale;
         _fWo = 2*std::numbers::pi*loadObjNomFreq_In;
        _Xp = _Xs + _Xm*_X1/(_Xm + _X1); // Thevenin reactance set
        _X  = _Xs + _Xm;  // for a reference see 7.102 of Kundar
         // This is needed in the setfofx function, so we'll set it here
        _fInvTp = _R1/(_X1 + _Xm)*_fWo;  // inverse of Tp
        local_impedanceSystem = PWDComplex_Init(_Rs, _Xp);
        tempSlipRes = TxInductionMachine_CalculateInitialSlip(_Rs, _Xs, _Xm, _R1, _X1,
                                                              SteadyStateV, local_impedanceSystem,
                                                              _fPInitial, INPUT_PUTol,
                                                              // following are calculated values
                                                              _SlipInitial, _fQInitial, local_TElect);
        // return 0 if optimal; 1 if P too high, 2 if P too low
        if ((tempSlipRes != 0)) {
            _fPInitial = 0;
            _fQInitial = 0;
        }
    };
    //----------------------------------------------------------------------------

    double fLmotor, fSmotor, fTex, fDis, fP, fKp;
    double loadObjNomFreq_In;
    double localUsedP_out, localUsedQ_out;

    // Large Motor Variables
    double IML_fPInitial, IML_fQInitial, IML_SlipInitial, IMLfInvTp,
           IML_Rs, IML_Xs, IML_Xm, IML_R1, IML_X1, IML_fWo, IML_Xp, IML_X;
    // Small Motor Variables
    double IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial, IMSfInvTp,
           IMS_Rs, IMS_Xs, IMS_Xm, IMS_R1, IMS_X1, IMS_fWo, IMS_Xp, IMS_X;

    // Discharge Lighting variables
    double DL_Per;
    double DL_P_at_1_PU, DL_Q_at_1_PU;  // Power at unity voltage
    bool DL_R; // variable to hold true/false result
    double DL_TotalP, DL_TotalQ;
    double pIMLight;

    // ZIP model local parameters
    double tsmw, tsmvr, timw, timvr, tzMW, tzMVR;
    double local_Scale;
    double zip_TotalP, zip_TotalQ;
    double minVoltSLoad_in, minVoltILoad_in;

    DoubleArray* pFloatParamArr;
    DoubleArray* pAlgebraicsArr;

    if (SteadyStateV == 0) SteadyStateV = 1;  // Set to unity if not in service

    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr = ParamsAndStates->FloatParams;
    pAlgebraicsArr = ParamsAndStates->Algebraics;

    fLmotor    = (*pFloatParamArr)[PARAM_PercLmotor];
    fSmotor    = (*pFloatParamArr)[PARAM_PercSmotor];
    fTex       = (*pFloatParamArr)[PARAM_PercTex   ];
    fDis       = (*pFloatParamArr)[PARAM_PercDis   ];
    fP         = (*pFloatParamArr)[PARAM_PercP     ];
    fKp        = (*pFloatParamArr)[PARAM_Kp        ];

    loadObjNomFreq_In = SystemOptions->WBase/(2*std::numbers::pi);
    minVoltSLoad_in   = SystemOptions->MinVoltSLoad;
    minVoltILoad_in   = SystemOptions->MinVoltILoad;

    localUsedP_out = 0;
    localUsedQ_out = 0;
    if ((SteadyStateV > 0)) {
        //--------------------------------------------------------------------------
        // large motor first
        //--------------------------------------------------------------------------
        IML_fPInitial = SteadyStateP*(fLmotor/100);
        // then q will get figured out by the calculateinitialslip function
        IML_Rs = 0.013;
        IML_Xs = 0.067;
        IML_Xm = 3.8;
        IML_R1 = 0.009;
        IML_X1 = 0.17;

        LOCAL_InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV,
                                       IML_fPInitial, IML_fQInitial, IML_SlipInitial,
                                       IML_Rs, IML_Xs, IML_Xm, IML_R1, IML_X1,
                                       IML_fWo, IML_Xp, IML_X, IMLfInvTp
                                       );
        localUsedP_out = IML_fPInitial;
        localUsedQ_out = IML_fQInitial;

        //--------------------------------------------------------------------------
        // large motor first
        //--------------------------------------------------------------------------
        IMS_fPInitial = SteadyStateP*(fSmotor/100);
        // then q will get figured out by the calculateinitialslip function
        IMS_Rs = 0.031;
        IMS_Xs = 0.1;
        IMS_Xm = 3.2;
        IMS_R1 = 0.018;
        IMS_X1 = 0.18;
        LOCAL_InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV,
                                       IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial,
                                       IMS_Rs, IMS_Xs, IMS_Xm, IMS_R1, IMS_X1,
                                       IMS_fWo, IMS_Xp, IMS_X, IMSfInvTp
                                       );
        localUsedP_out = localUsedP_out + IMS_fPInitial;
        localUsedQ_out = localUsedQ_out + IMS_fQInitial;
    }
    else { // No voltage to initialize the motors with, so write zeros to their algebraics
        IML_Rs = 0; IML_Xs = 0; IML_fWo = 0; IMLfInvTp = 0; IML_Xp = 0; IML_X = 0;
        IML_SlipInitial = 0; IML_fPInitial = 0; IML_fQInitial = 0;
        IMS_Rs = 0; IMS_Xs = 0; IMS_fWo = 0; IMSfInvTp = 0; IMS_Xp = 0; IMS_X = 0;
        IMS_SlipInitial = 0; IMS_fPInitial = 0; IMS_fQInitial = 0;
    }

    //----------------------------------------------------------------------------
    // Discharge Lighting
    //----------------------------------------------------------------------------
    if (fDis > 0) {
        DL_Per     = fDis/100;

        DL_P_at_1_PU = 1;
        DL_Q_at_1_PU = 1;
        DL_R = true;

        DL_TotalP = GetDL_Total(SteadyStateV, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
        if (DL_TotalP != 0) {
            DL_P_at_1_PU = SteadyStateP/DL_TotalP;
        }
        else {
            if (SteadyStateP != 0) DL_R = false;  // Error in initialization caused by voltage being too low
            else DL_P_at_1_PU = 0;
        }

        DL_TotalQ = GetDL_Total(SteadyStateV, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
        // If real OK then set the reactive power
        if (DL_R) {
            if (DL_TotalQ != 0) {
                DL_Q_at_1_PU = SteadyStateQ/DL_TotalQ;
            }
            else {
                if (SteadyStateQ != 0) DL_R = false;
                else DL_Q_at_1_PU = 0;
            }
        }
        // Normalize load based on the percent and return total power allocated
        DL_P_at_1_PU = DL_P_at_1_PU*DL_Per;
        DL_Q_at_1_PU = DL_Q_at_1_PU*DL_Per;

        DL_TotalP = GetDL_Total(SteadyStateV, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
        DL_TotalQ = GetDL_Total(SteadyStateV, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
        localUsedP_out  = localUsedP_out + DL_TotalP;
        localUsedQ_out  = localUsedQ_out + DL_TotalQ;
    }
    else {
        DL_P_at_1_PU = 0;
        DL_Q_at_1_PU = 0;
    }

    pIMLight = fLmotor + fSmotor + fDis;  // Total of induction motors, lighting and constant power

    //----------------------------------------------------------------------------
    // Treat the rest as constant ZIP model
    //----------------------------------------------------------------------------
    // fTex : Treat Transformer Saturation as a constant power
    // fP : Treat constant power portion
    tsmw = fTex + fP;
    timw = 0;
    tzMW = 0;
    tsmvr = tsmw;
    timvr = 0;
    tzMVR = 0;
    if (std::llrint(fKp) == 0) tsmw = 100 - pIMLight;  // All the rest as constant power
    else if (std::llrint(fKp) == 1) timw = 100 - pIMLight - tsmw;
    else tzMW = 100 - pIMLight - tsmw; // Constant impedance is what is left
    tzMVR = 100 - pIMLight - tsmw;  // Constant impedance reactive for what is left

    zip_TotalP = GetZIP_Total(SteadyStateV, tzMW, timw, tsmw, minVoltSLoad_in, minVoltILoad_in);
    zip_TotalQ = GetZIP_Total(SteadyStateV, tzMVR, timvr, tsmvr, minVoltSLoad_in, minVoltILoad_in);

    if (zip_TotalP != 0) local_Scale = (SteadyStateP - localUsedP_out)/zip_TotalP;
    else local_Scale = 1;
    tzMW = tzMW*local_Scale;
    timw = timw*local_Scale;
    tsmw = tsmw*local_Scale;

    if (zip_TotalQ != 0) local_Scale = (SteadyStateQ - localUsedQ_out)/zip_TotalQ;
    else local_Scale = 1;
    tzMVR  = tzMVR*local_Scale;
    timvr  = timvr*local_Scale;
    tsmvr  = tsmvr*local_Scale;

    // Specifies that all of the power is allocated...
    *InitLoadP = SteadyStateP;
    *InitLoadQ = SteadyStateQ;

    // LARGE MOTOR STUFF
    (*pAlgebraicsArr)[ALG_IML_Rs         ] = IML_Rs;
    (*pAlgebraicsArr)[ALG_IML_Xs         ] = IML_Xs;
    (*pAlgebraicsArr)[ALG_IML_fWo        ] = IML_fWo;
    (*pAlgebraicsArr)[ALG_IMLfInvTp      ] = IMLfInvTp;
    (*pAlgebraicsArr)[ALG_IML_Xp         ] = IML_Xp;
    (*pAlgebraicsArr)[ALG_IML_X          ] = IML_X;
    (*pAlgebraicsArr)[ALG_IML_SlipInitial] = IML_SlipInitial;
    (*pAlgebraicsArr)[ALG_IML_fPInitial  ] = IML_fPInitial;
    (*pAlgebraicsArr)[ALG_IML_fQInitial  ] = IML_fQInitial;

    // SMALL MOTOR STUFF
    (*pAlgebraicsArr)[ALG_IMS_Rs         ] = IMS_Rs;
    (*pAlgebraicsArr)[ALG_IMS_Xs         ] = IMS_Xs;
    (*pAlgebraicsArr)[ALG_IMS_fWo        ] = IMS_fWo;
    (*pAlgebraicsArr)[ALG_IMSfInvTp      ] = IMSfInvTp;
    (*pAlgebraicsArr)[ALG_IMS_Xp         ] = IMS_Xp;
    (*pAlgebraicsArr)[ALG_IMS_X          ] = IMS_X;
    (*pAlgebraicsArr)[ALG_IMS_SlipInitial] = IMS_SlipInitial;
    (*pAlgebraicsArr)[ALG_IMS_fPInitial  ] = IMS_fPInitial;
    (*pAlgebraicsArr)[ALG_IMS_fQInitial  ] = IMS_fQInitial;

    // DISCHARGE LIGHTING VALUES
    (*pAlgebraicsArr)[ALG_DL_P_at_1_PU   ] = DL_P_at_1_PU;
    (*pAlgebraicsArr)[ALG_DL_Q_at_1_PU   ] = DL_Q_at_1_PU;

    // ZIP LOAD VALUES
    (*pAlgebraicsArr)[ALG_ZIP_ZMW        ] = tzMW;
    (*pAlgebraicsArr)[ALG_ZIP_IMW        ] = timw;
    (*pAlgebraicsArr)[ALG_ZIP_SMW        ] = tsmw;
    (*pAlgebraicsArr)[ALG_ZIP_ZMVR       ] = tzMVR;
    (*pAlgebraicsArr)[ALG_ZIP_IMVR       ] = timvr;
    (*pAlgebraicsArr)[ALG_ZIP_SMVR       ] = tsmvr;

    // Low Voltage Trip Timer
    (*pAlgebraicsArr)[ALG_TIMER_LowVoltTimeSet] = 0;
    (*pAlgebraicsArr)[ALG_TIMER_LowVoltTime] = 0;

    bool result = true;
    return result;
}

}
