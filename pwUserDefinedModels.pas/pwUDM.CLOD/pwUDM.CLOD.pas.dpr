library pwUDM.CLOD.pas;

uses
  pwUDM in '..\common\pwUDM.pas',
  SysUtils;
// -----------------------------------------------------------------------------
const
  // For the Model
  N_F_PARAMS   =  9;     // number of float model params
  N_I_PARAMS   =  0;     // number of int model params
  N_S_PARAMS   =  0;     // number of string model params
  N_STATES     =  6;     // number of model states
  N_ALGEBRAICS = 30;     // number of model algebraics
  // For the NonWindUpLimits
  N_NONWINDUPLIMITS = 0; // number of nonwindup

  // ---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.

  // Double Parameters
  PARAM_PercLmotor =  0;
  PARAM_PercSmotor =  1;
  PARAM_PercTex    =  2;
  PARAM_PercDis    =  3;
  PARAM_PercP      =  4;
  PARAM_Kp         =  5;
  PARAM_Vi         =  6;
  PARAM_Ti         =  7;
  PARAM_Tb         =  8;

  // State Value Indices.
  STATE_IM1SpeedWr = 0;
  STATE_IM1Epr     = 1;
  STATE_IM1Epi     = 2;
  STATE_IM2SpeedWr = 3;
  STATE_IM2Epr     = 4;
  STATE_IM2Epi     = 5;

  // Algebraic Value Indices
  // Large Motor Algebraics
  ALG_IML_Rs          =  0;
  ALG_IML_Xs          =  1;
  ALG_IML_fWo         =  2;
  ALG_IMLfInvTp       =  3;
  ALG_IML_Xp          =  4;
  ALG_IML_X           =  5;
  ALG_IML_SlipInitial =  6;
  ALG_IML_fPInitial   =  7;
  ALG_IML_fQInitial   =  8;
  ALG_IML_Tnom        =  9;
  // Small Motor Algebraics
  ALG_IMS_Rs          = 10;
  ALG_IMS_Xs          = 11;
  ALG_IMS_fWo         = 12;
  ALG_IMSfInvTp       = 13;
  ALG_IMS_Xp          = 14;
  ALG_IMS_X           = 15;
  ALG_IMS_SlipInitial = 16;
  ALG_IMS_fPInitial   = 17;
  ALG_IMS_fQInitial   = 18;
  ALG_IMS_Tnom        = 19;
  // Discharge Lighting algebraics
  ALG_DL_P_at_1_PU    = 20;
  ALG_DL_Q_at_1_PU    = 21;
  // ZIP Model algebraics
  ALG_ZIP_ZMW         = 22;
  ALG_ZIP_IMW         = 23;
  ALG_ZIP_SMW         = 24;
  ALG_ZIP_ZMVR        = 25;
  ALG_ZIP_IMVR        = 26;
  ALG_ZIP_SMVR        = 27;
  // Timer for Load Tripping
  ALG_TIMER_LowVoltTimeSet = 28;
  ALG_TIMER_LowVoltTime = 29;

  // Load Hard-Coded Input Signal Indices
  // Note: These are the SAME for all loads and are hard-cded values that
  // Simulator ALWAYS passes to the DLL
  // If additional inputs are needed from Simulator, then you MUST define them
  // using the "Algebraics" signals and include them at the BEGINNING of the ALG vector
  HARDCODE_LOAD_DeviceVPU      =  0;
  HARDCODE_LOAD_DeviceAngleRad =  1;
  HARDCODE_LOAD_DeltaFreqPU    =  2;
  HARDCODE_LOAD_DeviceStatus   =  3;
  HARDCODE_LOAD_LoadScalar     =  4;

  // These are hard-coded parameters for modeling the discharge lighting load
  GLOBAL_DL_PCoeff  = 1.0; // constant CURRENT Real Power
  GLOBAL_DL_QCoeff  = 4.5; // much higher coefficent for Reactive Power
  GLOBAL_DL_VoltBP  = 0.75;
  GLOBAL_DL_VoltExt = 0.65;

type
  // Complex Number object to make some of the code easier to read.
  PWDComplex = Object
    r,i : Double;
    Procedure Init(tr,ti : double);
    Function CInc(tempCom : PWDComplex) : PWDComplex;
    Function CDec(tempCom : PWDComplex) : PWDComplex;
    Function Mag : Double;
    Function Multiply(aValue : PWDComplex) : PWDComplex;
    Function Divide(aValue : PWDComplex) : PWDComplex;
  End;


{$R *.res}

//******************************************************************************
Procedure PWDComplex.Init(tr,ti : double);
begin
  r := tr;
  i := ti;
end;

Function PWDComplex.CInc(TempCom : PWDComplex) : PWDComplex;
Begin
  result.r := r + TempCom.R;
  result.i := i + TempCom.I;
End;

Function PWDComplex.CDec(TempCom : PWDComplex) : PWDComplex;
Begin
  result.r := r - TempCom.R;
  result.i := i - TempCom.I;
End;

Function PWDComplex.Mag : Double;
Begin
  result := Sqrt(Sqr(r) + Sqr(i));
End;

Function PWDComplex.Multiply(aValue : PWDComplex) : PWDComplex; // Returns self multiplied with aValue
Begin
  result.r := (r*aValue.r - i*aValue.i);
  result.i := (i*aValue.r + r*aValue.i);
End;

Function PWDComplex.Divide(aValue : PWDComplex) : PWDComplex; // Returns self divided by aValue
Var t : Double;
Begin
  t := 1 / (Sqr(avalue.r) + Sqr(avalue.i));
  Result.r := (r * avalue.r + i * avalue.i) * t;
  Result.i := (-r * avalue.i + i * avalue.r) * t;
End;

//******************************************************************************
Function Math_Y_to_X(base,exponent : double) : Double;
Begin
  If base = 0 Then Result := 0
  Else Begin
    If exponent = 0 Then Result := 1
    Else If exponent = 1 Then Result := base
    Else If exponent = 2 Then Result := Sqr(base)
    Else If exponent = 3 Then Result := base*Sqr(base)
    Else If exponent = 4 Then Result := Sqr(base)*Sqr(base)
    Else Result := Exp(Exponent*Ln(base));
  End;
End;

//******************************************************************************
Function  PWDComplex_Init(r,i : Double) : PWDComplex;
Begin
  Result.r := r;
  Result.i := i;
End;

Function  PWDComplex_InitPolarRad(tmag,tangle : Double) : PWDComplex;
Begin
  Result.r := tmag * cos(tangle);
  Result.i := tmag * sin(tangle);
End;

Function  PWDComplex_Multiply (q1,q2 : PWDComplex) : PWDComplex;
Begin
  result.r := q1.r * q2.r - q1.i * q2.i;
  result.i := q1.r * q2.i + q1.i * q2.r;
End;

Function PWDComplex_Divide (num,den : PWDComplex) : PWDComplex;
Var t : Double;
Begin
  t := 1 / (Sqr(den.r) + Sqr(den.i));
  Result.r := (num.r * den.r + num.i * den.i) * t;
  Result.i := (-num.r * den.i + num.i * den.r) * t;
End;

Function PWDComplex_Parallel(complex1,complex2 : PWDComplex) : PWDComplex;
Var temp_num,temp_Den : PWDComplex;
Begin
  temp_Num := PWDComplex_Multiply(complex1,complex2);
  temp_Den.r := complex1.r + complex2.r;
  temp_Den.i := complex1.i + complex2.i;
  Result := PWDComplex_Divide(temp_Num,temp_Den);
End;

//******************************************************************************
Procedure TxInductionMachine_CalculateAtSlip(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag : Single;
                                             impedanceSystem : PWDComplex; aSlip : Double;
                                             out iStator,eInternal : PWDComplex;
                                             out totalP,totalQ,local_PF,Telec : Double);
Var rotorZ1 : PWDComplex;  // stator and rotor impedances
    zRotor,zTotal : PWDComplex;  // Total rotor and total impedance
    //L012 : Double;  // L0 in series with (L1 in parallel with L2)
    fzM,fstatorZ : PWDComplex;
    fvTerminal : PWDComplex;
Begin
  fzM := PWDComplex_Init(0,fXm);
  fstatorZ.Init(fRs,fXs);
  fvTerminal.Init(fBusVoltageMag,0);
  If (aSlip <> 0) Then Begin
    rotorZ1.Init(fR1/aSlip,fX1);
    zRotor := rotorZ1;
    zTotal := PWDComplex_Parallel(zRotor,fzM).cInc(fstatorZ);
   End
  Else zTotal := fstatorZ.cInc(fzM);
  iStator := PWDComplex_Divide(fvTerminal,zTotal);
  totalP := fvTerminal.r*iStator.r;  // fVoltmag.i is always zero
  totalQ := -fvTerminal.r*iStator.i;
  local_PF := totalP/Sqrt(Sqr(totalP)+Sqr(totalQ));
  eInternal := fvTerminal.cDec(iStator.Multiply(impedanceSystem));
  tElec := eInternal.r*iStator.r + eInternal.i*iStator.i;
End;

//******************************************************************************
Function TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag : Single;
                                                 impedanceSystem : PWDComplex; aSlip : Double) : Double;
Var temp_iStator,temp_eInternal : PWDComplex;
    temp_Q,temp_PF,temp_Telec : Double;
Begin
  TxInductionMachine_CalculateAtSlip(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,
                                     impedanceSystem,aSlip,temp_iStator,temp_eInternal,
                                     Result,temp_Q,temp_PF,temp_Telec);
End;

//******************************************************************************
// Function TTxGenericDoubleCageInductionMotorModel.CalculateInitialSlip
// iteratively determines the initial slip, returning true is such a slip exists.
// For this algorithm the assumption is there are at most three slip values that
// solve for the P.  Algorithm returns the one closest to zero slip.  Note,
// there also may be only two, one solution (between 0 and 1) or no solutions.
Function TxInductionMachine_CalculateInitialSlip(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag : Single; impedanceSystem : PWDComplex; aPower,aTol : Double; out theSlip,initialQPU,theTElec : Double) : Byte; // return 0 if optimal; 1 if P too high, 2 if P too low
Var slipH,slipL,slipM : Double;
    phigh,plow,pmid : Double;
    i : Integer;
    deltaSlip : Double;
    temp_iStator,temp_eInternal : PWDComplex;
    temp_PF,temp_P : Double;
Begin
  Result := 0;
  initialQPU := 0;
  slipL := 0;
  theTElec := 0;
  deltaSlip := 0.02;
  // Check for the unlikely case where power is below the power just from stator losses
  plow := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipL);
  If plow >= aPower Then Begin
    If plow > aPower Then Result := 2;
    theSlip := 0;
   End
  // Work to bracket the slip.  If phigh-aPower > 0 then we know we have it bracketed.
  // Otherwise if phigh-aPower < 0 and it is going down we've got a case in which
  // we've bracketed two solutions.  This will take more work.
  Else Begin
    slipH := deltaSlip;
    phigh := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipH);
    While (phigh-aPower <=0) and (phigh > plow) and (slipH <= 1) Do Begin
      plow := phigh;
      slipH := slipH + deltaSlip;  // Most slips will be small so this should be done only once or twice usually
      phigh := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipH);
      If slipH >= 0.1 Then deltaSlip := 0.05;
    End;
    If (phigh-aPower > 0) Then Begin // We have a single value bracketed between slipH and 0.  Iterate until P mismatch is small
      i := 0;
      Repeat
        slipM := (slipH+slipL)*0.5;
        pmid := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipM);
        If pmid-apower > 0 Then slipH := slipM
        Else slipL := slipM;
        Inc(i);
      Until (Abs(pMid-aPower) < aTol) or (i > 20);

      If i > 20 Then Result := 1  // This shouldn't occur
      // Found the value; return with the slip and the initial Q
      Else Begin
        theSlip := slipM;
        TxInductionMachine_CalculateAtSlip(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,
                                           impedanceSystem,theSlip,temp_iStator,
                                           temp_eInternal,temp_P,initialQPU,temp_PF,theTelec);
      End;
    End
    Else If (slipH > 1) and (phigh > plow) Then Begin  // power continually increasing so and we don't have enough
      Result := 1;
      theslip := 1;
      TxInductionMachine_CalculateAtSlip(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,
                                         impedanceSystem,theSlip,temp_iStator,
                                         temp_eInternal,temp_P,initialQPU,temp_PF,theTelec);
    End
    Else Begin
      // This is the tricky case, but thankfully one that shouldn't occur too often --
      // it requires operating with a motor pretty close to stalling. A solution
      // may or may not exist. If it exists then there are usually two solutions.
      // As a quick and dirty solution just set phigh to plow-deltaSlip, which will
      // guarantee we are starting at a slip below the max (towards slip = 0).
      // Then increment the value by a small about until either get a value above
      // power or hit the maximum.  If above power then redo above iteration;
      // otherwise exit.  Note this approach will miss operating points very near
      // the max slip, but these are not realistic transient stability solutions anyways.
      slipL := slipL - deltaSlip;
      If slipL < 0 Then slipL := 0;
      theSlip := slipH;  // temp storage of maximum value to check
      plow := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipL);
      slipH := slipL + deltaSlip/10;
      phigh := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipH);
      While (slipH < theSlip) and (phigh-apower < 0) and (phigh > plow) Do Begin
        plow := phigh;
        slipL := slipH;
        slipH := slipL + deltaSlip/10;
        phigh := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipH);
      End;
      If phigh-apower >= 0 Then Begin
        i := 0;
        Repeat
          slipM := (slipH+slipL)*0.5;
          pmid := TxInductionMachine_CalculateAtSlipJustP(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,impedanceSystem,slipM);
          If pmid-apower > 0 Then slipH := slipM
          Else slipL := slipM;
          Inc(i);
        Until (Abs(pMid-aPower) < aTol) or (i > 20);
        theSlip := slipM;
        If i > 20 Then Result := 1  // shouldn't occur
        // Found the value; return with the slip and the initial Q
        Else TxInductionMachine_CalculateAtSlip(fRs,fXs,fXm,fR1,fX1,fBusVoltageMag,
                                                impedanceSystem,theSlip,temp_iStator,
                                                temp_eInternal,temp_P,initialQPU,temp_PF,theTelec);
       End
      Else Begin
        Result := 1;
        theSlip := slipH;
      End;
    End;
  End;
End;

//******************************************************************************
Function TxInductionMachine_GetInitialConditions(fWo : Double;
                                                 fPInit : Double;
                                                 fRa,fX,fXp,fXl,fInvTp : Double; fTermVolt : PWDComplex; fMaxItr : Integer; fItrTol : Double;
                                                 var fSlip,fErp,fEip,fId,fIq,fQInit : Double) : Boolean;
  //---------------------------------------------------------------------------
  const MatSize = 3;
  type TMyMatrix = Array[1..MatSize, 1..MatSize] of double;
       TMyVector = Array[1..MatSize] of double;
       TMyIntVector = Array[1..MatSize] of integer;

  //---------------------------------------------------------------------------
  function VectorNorm2(theVector : TMyVector) : double;
  var i : integer;
  begin
    result := 0;
    for i := 1 to MatSize do begin
      result := result + sqr(theVector[i]);
    end;
    result := sqrt(result);
  end;

  //---------------------------------------------------------------------------
  procedure MatrixZeroOut(var theMatrix : TMyMatrix);
  var theRow, theCol : integer;
  begin
    for theRow := 1 to MatSize do begin
      for theCol := 1 to MatSize do begin
        theMatrix[theRow, theCol] := 0;
      end;
    end;
  end;

  //---------------------------------------------------------------------------
  procedure MatrixFactor(var theMatrix : TMyMatrix; var RowPerm : TMyIntVector);
  Var k,j,imax,i  : Integer;
      sum,dum,big : double;
      vv          : TMyVector;
  Begin
    imax := 0;

    For i := 1 to MatSize Do Begin
      RowPerm[i] := i;
      big := 0.0;
      For j := 1 to MatSize Do begin
        If abs(theMatrix[i,j]) > big Then big := abs(theMatrix[i,j]);
      end;
      vv[i] := 1.0/big;
    End;

    For j := 1 to MatSize Do Begin
      For i := 1 to j-1 Do Begin  // Solve for the upper triangular elements, except for the diagonal when i = j
        sum := theMatrix[i,j];
        For k := 1 to i-1 Do begin
          sum := sum - theMatrix[i,k]*theMatrix[k,j];
        end;
        theMatrix[i,j] := sum;
      End;
      big := 0.0;
      For i := j to MatSize Do Begin   // For diagonal element use remaining row with the largest values
        sum := theMatrix[i,j];
        For k := 1 to j-1 Do begin
          sum := sum - theMatrix[i,k]*theMatrix[k,j];
        end;
        theMatrix[i,j] := sum;
        dum := vv[i]*abs(sum);  // Normalize
        If dum >= big Then Begin  // Pick as the pivot row
          big := dum;
          imax := i;
        End
      End;

      If j <> imax Then Begin // If true then we need to interchange the row -- a pivot occurred
        For k := 1 to MatSize Do Begin
          dum := theMatrix[imax,k];
          theMatrix[imax,k] := theMatrix[j,k];
          theMatrix[j,k] := dum;
        End;
        vv[imax] := vv[j];
      End;

      RowPerm[j] := imax;    { Keep track of the pivots we've done }

      If j <> MatSize Then Begin { Finally divide by the pivot element }
        dum := 1.0/theMatrix[j,j];
        For i := j+1 to MatSize Do theMatrix[i,j] := theMatrix[i,j]*dum;
      End
    End;
  end;

  //---------------------------------------------------------------------------
  procedure MatrixForwardBackward(var theMatrix : TMyMatrix; var theVector : TMyVector; var RowPerm : TMyIntVector);
  Var j,i,ip,ii : Integer;
      sum       : double;
  Begin
    ii := 0;
    For i := 1 to MatSize Do Begin
      ip := RowPerm[i];
      sum := theVector[ip];
      theVector[ip] := theVector[i];
      If ii <> 0 Then begin
        For j := ii to i-1 Do begin
          sum := sum - theMatrix[i,j]*theVector[j]
        end;
      end
      Else If sum <> 0.0 Then ii := i;
      theVector[i] := sum;
    End;
    For i := MatSize downto 1 Do Begin
      sum := theVector[i];
      For j := i+1 to MatSize Do begin
        sum := sum - theMatrix[i,j]*theVector[j];
      end;
      theVector[i] := sum/theMatrix[i,i];
    End;
  end;

  //---------------------------------------------------------------------------
  Procedure local_GetIntialConditions_SetMismatch(fC1, fVr, fVi, local_G, local_B : double;
                                                  var theB, theX : TMyVector);
  Begin
    // Set local values, including slip, internal voltage and current
    fSlip := theX[1];
    fErp  := theX[2];
    fEip  := theX[3];
    fId := (fErp - fVr)*local_G - (fEip - fVi)*local_B;
    fIq := (fEip - fVi)*local_G + (fErp - fVr)*local_B;
    // Set the mismatch
    theB[1] := fVr*fId + fVi*fIq - fPInit;   // Real power balance
    theB[2] :=  fWo*fSlip*fEip - fInvTp*(fErp + fC1*(-fIq));
    theB[3] := -fWo*fSlip*fErp - fInvTp*(fEip + fC1*( fId));
  End;

var
    fC1 : Double;  // Constants that depend on the impedances
    local_G,local_B : Double;
    fVr,fVi : Double;  // Terminal voltage, real/imagninary
    theB      : TMyVector;
    theX      : TMyVector;
    theMatrix : TMyMatrix;
    RowPerm   : TMyIntVector;
    local_Itr : Integer;  // Iteration counter
    local_T : double;
    i : Integer;
    dIdErp,dIdEip : Double;  // Partial of Id w.r.t. each voltage
    dIqErp,dIqEip : Double;  // Partial of Iq w.r.t. each voltage

Begin
  local_Itr := 0;
  fVr := fTermVolt.r;
  fVi := fTermVolt.i;
  fC1 := fX - fXp;

// Set the initial conditions
  theX[1] := fSlip;
  theX[2] := fErp;
  theX[3] := fEip;
  local_T := Sqr(fRa) + Sqr(fXp);
  local_G :=  fRa/local_T;
  local_B := -fXp/local_T;

  local_GetIntialConditions_SetMismatch(fC1, fVr, fVi, local_G, local_B, theB, theX);
  // Get partial of currents w.r.t voltages
  dIdErp :=  local_G;
  dIdEip := -local_B;
  dIqErp :=  local_B;
  dIqEip :=  local_G;
  While (VectorNorm2(theB) > fItrTol) and (local_Itr < fMaxItr) Do Begin
    // Set the Jacobian.  Zero out initially then just set the non-zeros; The power equation
    MatrixZeroOut(theMatrix);
    theMatrix[1,2] :=  fVr*local_G + fVi*local_B;
    theMatrix[1,3] := -fVr*local_B + fVi*local_G;
    theMatrix[2,1] :=  fWo*fEip;
    theMatrix[3,1] := -fWo*fErp;
    theMatrix[2,2] := -fInvTp*(1 + fC1*(-1)*dIqErp);
    theMatrix[2,3] := fWo*fSlip - fInvTp*fC1*(-1)*dIqEip;
    theMatrix[3,2] := -fWo*fSlip -fInvTp*fC1*(1)*dIdErp;
    theMatrix[3,3] := -fInvTp*(1 + fC1*(1)*dIdEip);
    // Solve Ax=b
    MatrixFactor(theMatrix, RowPerm);
    MatrixForwardBackward(theMatrix, theB, RowPerm);
    For i := 1 to MatSize Do theX[i] := theX[i] - theB[i];
    local_GetIntialConditions_SetMismatch(fC1, fVr, fVi, local_G, local_B, theB, theX);
    Inc(local_Itr);
  End;

  Result := (local_Itr < fMaxItr);  // Return true if initialized
  // Note, excepting the reactive power, the final return values have been set in the Set_Mismatch function
  fQInit := -fVr*fIq + fVi*fId;
End;

//******************************************************************************
// This functions handles the exchange of string length and copying the string
function LOCAL_StringProcess(StrBuf: PChar; StrSize: PInteger; SourceString: PChar): Integer;
begin
  result := length(SourceString);
  if (result > 0) and (StrSize^ >= Result) then strcopy(StrBuf, SourceString);
end;

//******************************************************************************
function DLLVersion:Integer; stdcall;
begin
  result := 1;
end;

//******************************************************************************
function modelClassName(StrSize: PInteger;
                        StrBuf: PChar;
                        dummy: Integer): Integer; stdcall;
var ts : PChar;
begin
	ts := 'UserDefinedLoadModel';
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
procedure allParamCounts(var numbersOfEverything : TTxParamCounts;
                         TimeStepSeconds : double); stdcall;
begin
  with numbersOfEverything do begin
    // -------The model-specific parameters-----
    nFloatParams  := N_F_PARAMS;
    nIntParams    := N_I_PARAMS;
    nStrParams    := N_S_PARAMS;
    nStates       := N_STATES;
    nAlgebraics   := N_ALGEBRAICS;
    // ----For nonwindup limits of the model---
    nNonWindUpLimits := N_NONWINDUPLIMITS;
  end;
end;

//******************************************************************************
function parameterName(ParamNum: PInteger;
                       StrSize: PInteger;
                       StrBuf: PChar;
                       dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  // Double parameters
  case (ParamNum^) of
    PARAM_PercLmotor  : ts := '%Lmotor';
    PARAM_PercSmotor  : ts := '%Smotor';
    PARAM_PercTex     : ts := '%Tex';
    PARAM_PercDis     : ts := '%Dis';
    PARAM_PercP       : ts := '%P';
    PARAM_Kp          : ts := 'Kp';
    PARAM_Vi          : ts := 'Low V Trip Pickup (pu)';
    PARAM_Ti          : ts := 'Low V Trip Timer (sec)';
    PARAM_Tb          : ts := 'Low V Trip Breaker Delay (sec)';
   else                 ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
function stateName(StateNum: PInteger;
                   StrSize: PInteger;
                   StrBuf: PChar;
                   dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case StateNum^ of
    STATE_IM1SpeedWr : ts := 'IM 1 Speed wr';
    STATE_IM1Epr     : ts := 'IM 1 Epr';
    STATE_IM1Epi     : ts := 'IM 1 Epi';
    STATE_IM2SpeedWr : ts := 'IM 2 Speed wr';
    STATE_IM2Epr     : ts := 'IM 2 Epr';
    STATE_IM2Epi     : ts := 'IM 2 Epi';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
{ non specified for this model
function OtherObjectClass(Num: PInteger;
                          StrSize: PInteger;
                          StrBuf: PChar;
                          dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Bus';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
{ non specified for this model
function OtherObjectDescription(Num: PInteger;
                                StrSize: PInteger;
                                StrBuf: PChar;
                                dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  case Num^ of
    0 : ts := 'Signal Bus';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
procedure getDefaultParameterValue(ParamsAndStates : PTxMyModelData); stdcall;
var pDoubArr: PDoubleArray;
begin
  pDoubArr := PDoubleArray(ParamsAndStates^.FloatParams);
  pDoubArr^[PARAM_PercLmotor]  := 25.0;
  pDoubArr^[PARAM_PercSmotor]  := 25.0;
  pDoubArr^[PARAM_PercTex   ]  :=  0.0;
  pDoubArr^[PARAM_PercDis   ]  := 20.0;
  pDoubArr^[PARAM_PercP     ]  :=  0.0;
  pDoubArr^[PARAM_Kp        ]  :=  1.0;
  pDoubArr^[PARAM_Vi        ]  :=  0.0;
  pDoubArr^[PARAM_Ti        ]  :=  0.0;
  pDoubArr^[PARAM_Tb        ]  :=  0.05;
end;

{ non specified for this model
//******************************************************************************
function signalSelection(Num: PInteger;
                         StrSize: PInteger;
                         StrBuf: PChar;
                         dummy: Integer): Integer; stdcall;
var ts : PChar;
begin
  ts := ''; // there are none!
  case Num^ of
    ALG_XXX         : ts := 'TSXXX';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;
}

//******************************************************************************
function SubIntervalPower2Exponent(ParamsAndStates : PTxMyModelData; TimeStepSeconds : double) : integer; stdcall;
begin
  // Return an integer value between 0 and 7
  // This function returns the EXPONENT of 2 for the number subintervals.
  // Thus, if you want 8 subintervals, then this should return 3 because 2^3 = 8
  // The maximum number of subintervals allowed is 128, so (2^7) = 128
  result := 0;
end;

//******************************************************************************
procedure initializeYourself(ParamsAndStates : PTxMyModelData;
                             SystemOptions : PTxSystemOptions); stdcall;

  //--------------------------------------------------------------------------
  procedure LOCAL_InitalizeMotor(LoadObjBusVolt_In : PWDComplex;
                                 _fPInitial, _fQInitial, _SlipInitial, _Rs, _Xs, _fWo, fInvTp, _Xp, _X : double;
                                 var SpeedState, EprState, EpiState, _Tnom : double);
  var Damping : double;
      localCurrent : PWDComplex;
      local_Ep : PWDComplex;
      local_IIn : PWDComplex;
      local_impedanceSystem : PWDComplex;
      fStateIr_In, fStateIi_In : Double;
      StateElectTorque : Double;
      t3, t4, t5 : Double;
      tQInit : Double;
  begin
    localCurrent := PWDComplex_Init(_fPInitial,_fQInitial).Divide(LoadObjBusVolt_In);
    fStateIr_In := localCurrent.r;
    fStateIi_In := - localCurrent.i;
    local_IIn := PWDComplex_Init(fStateIr_In, fStateIi_In);
    local_impedanceSystem := PWDComplex_Init(_Rs, _Xp);
    // Get the initial internal volage and current.  Note, these values are in the network reference frame.
    local_Ep := LoadObjBusVolt_In.CDec(local_IIn.Multiply(local_impedanceSystem));

    t3 := -fStateIr_In;
    t4 := -fStateIi_In;
    t5 := -_fPInitial;

    TxInductionMachine_GetInitialConditions(_fWo,
                                            t5,
                                            _Rs,_X,_Xp,_Xs,fInvTp,LoadObjBusVolt_In,10,1e-5,
                                            _SlipInitial,local_Ep.r,local_Ep.i,
                                            t3,t4,tQInit);
    fStateIr_In := -t3;
    fStateIi_In := -t4;

    // The state variables,
    SpeedState := 1- _SlipInitial;
    EprState := local_Ep.r;
    EpiState := local_Ep.i;

    StateElectTorque := EprState*fStateIr_In + EpiState*fStateIi_In;
    Damping := 2;   // damping, same as fDamp
    _Tnom := StateElectTorque/Math_Y_to_X(1-_SlipInitial, Damping);
  end;
  //--------------------------------------------------------------------------

var
  // The state variables,
  IMLSpeedState, IMLEprState, IMLEpiState,
  IMSSpeedState, IMSEprState, IMSEpiState : Double;

  IML_fPInitial, IML_fQInitial, IML_SlipInitial, IML_Rs, IML_Xs, IML_Xp, IML_X, IML_fWo, IMLfInvTp, IML_Tnom : Double;
  IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial, IMS_Rs, IMS_Xs, IMS_Xp, IMS_X, IMS_fWo, IMSfInvTp, IMS_Tnom : Double;

  LoadObjBusVolt_In : PWDComplex;
  dTemp : Double;
  LoadObjV_In, LoadObjThetaRadian_In : Double;
  inService_In : Boolean;

  pStatesArr : PDoubleArray;
  pHardCodedArr : PDoubleArray;
  pAlgebraicsArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);

  LoadObjV_In           := pHardCodedArr^[HARDCODE_LOAD_DeviceVPU];
  LoadObjThetaRadian_In := pHardCodedArr^[HARDCODE_LOAD_DeviceAngleRad];
  dTemp                 := pHardCodedArr^[HARDCODE_LOAD_DeviceStatus];    // Inservice
  inService_In := dTemp > 0.5;
  LoadObjBusVolt_In := PWDComplex_InitPolarRad(LoadObjV_In,LoadObjThetaRadian_In);

  // following were all initialized in LoadInitializeAlgebraic call
  // Just copy them over here!
  IML_Rs         := pAlgebraicsArr^[ALG_IML_Rs        ];
  IML_Xs         := pAlgebraicsArr^[ALG_IML_Xs        ];
  IML_fWo        := pAlgebraicsArr^[ALG_IML_fWo       ];
  IMLfInvTp      := pAlgebraicsArr^[ALG_IMLfInvTp     ];
  IML_Xp         := pAlgebraicsArr^[ALG_IML_Xp        ];
  IML_X          := pAlgebraicsArr^[ALG_IML_X         ];
  IML_SlipInitial:= pAlgebraicsArr^[ALG_IML_SlipInitial];
  IML_fPInitial  := pAlgebraicsArr^[ALG_IML_fPInitial ];
  IML_fQInitial  := pAlgebraicsArr^[ALG_IML_fQInitial ];

  IMS_Rs         := pAlgebraicsArr^[ALG_IMS_Rs         ];
  IMS_Xs         := pAlgebraicsArr^[ALG_IMS_Xs         ];
  IMS_fWo        := pAlgebraicsArr^[ALG_IMS_fWo        ];
  IMSfInvTp      := pAlgebraicsArr^[ALG_IMSfInvTp      ];
  IMS_Xp         := pAlgebraicsArr^[ALG_IMS_Xp         ];
  IMS_X          := pAlgebraicsArr^[ALG_IMS_X          ];
  IMS_SlipInitial:= pAlgebraicsArr^[ALG_IMS_SlipInitial];
  IMS_fPInitial  := pAlgebraicsArr^[ALG_IMS_fPInitial  ];
  IMS_fQInitial  := pAlgebraicsArr^[ALG_IMS_fQInitial  ];

  // LARGE INDUCTION MOTOR
  if inService_In then begin
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
  end
  else Begin // If out-of-service then set all to zero
    IMLSpeedState := 0;
    IMLEprState := 0;
    IMLEpiState := 0;
    IML_Tnom := 0;

    IMSSpeedState := 0;
    IMSEprState := 0;
    IMSEpiState := 0;
    IMS_Tnom := 0;
  end;

  // Write out initial state vector,
  pStatesArr^[STATE_IM1SpeedWr] := IMLSpeedState;  // local_EField should be the same thing as fEfdInit, which is effecitvely just read in from Simulator.
  pStatesArr^[STATE_IM1Epr    ] := IMLEprState;
  pStatesArr^[STATE_IM1Epi    ] := IMLEpiState;
  pStatesArr^[STATE_IM2SpeedWr] := IMSSpeedState;
  pStatesArr^[STATE_IM2Epr    ] := IMSEprState;
  pStatesArr^[STATE_IM2Epi    ] := IMSEpiState;

  // These values just need to be passed around
  pAlgebraicsArr^[ALG_IML_Tnom      ] := IML_Tnom;
  pAlgebraicsArr^[ALG_IMS_Tnom      ] := IMS_Tnom;
end;

//******************************************************************************
procedure calculateFofX(ParamsAndStates : PTxMyModelData;
                        SystemOptions : PTxSystemOptions;
                        nonWindUpLimits : PTxNonWindUpLimits;
                        dotX : PDouble
                        ); stdcall;

  procedure LOCAL_GetMotorDerivatives(LoadObjBusVolt_In : PWDComplex;
                                      inService_In : boolean;
                                      loadScalar_in : double;
                                      _fPInitial, _Rs, _fWo, fInvTp, _Xp, _X, _D, _fInv2H,
                                      SpeedState, EprState, EpiState, _Tnom : double;
                                      var dotSpeedState, dotEprState, dotEpiState : double
                                      );
  var fStateIr_In, fStateIi_In : Double;
      StateElectTorque : Double;
      MechanicalTorque : Double;
      stateSlip : Double;
      StateCurrent, IntVolt, ImpSys : PWDComplex;
  begin
    IntVolt := PWDComplex_Init(EprState, EpiState);
    ImpSys := PWDComplex_Init(_Rs, _Xp);
    IntVolt := LoadObjBusVolt_In.CDec(IntVolt);
    StateCurrent := PWDComplex_Divide(IntVolt, ImpSys);
    fStateIr_In := StateCurrent.r;
    fstateIi_In := StateCurrent.i;

    StateElectTorque := EprState*fStateIr_In + EpiState*fStateIi_In;
    If inService_In Then MechanicalTorque := Math_Y_to_X(SpeedState, _D)*_Tnom
    Else MechanicalTorque := 0;
    dotSpeedState := _fInv2H*(StateElectTorque - MechanicalTorque);

  // For the large induction motor dotEpr
    stateSlip := 1 - SpeedState;
    dotEprState := (stateSlip*EpiState*_fWo - fInvTp*(EprState + (_X-_Xp)*loadScalar_in*fStateIi_In));

  // For the large induction motor dotEpi (see TTxGenericInductionMotorModel.dotEpi):
    dotEpiState := (-stateSlip*EprState*_fWo - fInvTp*(EpiState - (_X-_Xp)*loadScalar_in*fStateIr_In));
  end;


var
  loadScalar_in,
  local_BusVoltMag,
  LoadObjThetaRadian_In : double;
  LoadObjBusVolt_In : PWDComplex;

  dtemp : Double;
  inService_In : Boolean;

  // The state variables,
  IMLSpeedState, IMLEprState, IMLEpiState,
  IMSSpeedState, IMSEprState, IMSEpiState : Double;

  // The FofX or xdot,
  dotIMLSpeedState, dotIMLEprState, dotIMLEpiState,
  dotIMSSpeedState, dotIMSEprState, dotIMSEpiState : Double;

  IML_fPInitial, IML_fWo, IMLfInvTp, IML_Rs, IML_Xs, IML_Xp, IML_X, IML_H, IML_D, IML_fInv2H, IML_Tnom : Double;
  IMS_fPInitial, IMS_fWo, IMSfInvTp, IMS_Rs, IMS_Xs, IMS_Xp, IMS_X, IMS_H, IMS_D, IMS_fInv2H, IMS_Tnom : Double;

  pStatesArr : PDoubleArray;
  pHardCodedArr : PDoubleArray;
  pAlgebraicsArr : PDoubleArray;
  pDotXArr : PDoubleArray;
begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);
  pDotXArr := PDoubleArray(dotX);

  IMLSpeedState := pStatesArr^[STATE_IM1SpeedWr];
  IMLEprState   := pStatesArr^[STATE_IM1Epr    ];
  IMLEpiState   := pStatesArr^[STATE_IM1Epi    ];
  IMSSpeedState := pStatesArr^[STATE_IM2SpeedWr];
  IMSEprState   := pStatesArr^[STATE_IM2Epr    ];
  IMSEpiState   := pStatesArr^[STATE_IM2Epi    ];

  dTemp                 := pHardCodedArr^[HARDCODE_LOAD_DeviceStatus];    // Inservice
  inService_In := dTemp > 0.5;
  loadScalar_in         := pHardCodedArr^[HARDCODE_LOAD_LoadScalar];

  // Following values are all calculation
  IML_Rs         := pAlgebraicsArr^[ALG_IML_Rs        ];
  IML_Xs         := pAlgebraicsArr^[ALG_IML_Xs        ];
  IML_fWo        := pAlgebraicsArr^[ALG_IML_fWo       ];
  IMLfInvTp      := pAlgebraicsArr^[ALG_IMLfInvTp     ];
  IML_Xp         := pAlgebraicsArr^[ALG_IML_Xp        ];
  IML_X          := pAlgebraicsArr^[ALG_IML_X         ];
  IML_fPInitial  := pAlgebraicsArr^[ALG_IML_fPInitial ];

  IMS_Rs         := pAlgebraicsArr^[ALG_IMS_Rs        ];
  IMS_Xs         := pAlgebraicsArr^[ALG_IMS_Xs        ];
  IMS_fWo        := pAlgebraicsArr^[ALG_IMS_fWo       ];
  IMSfInvTp      := pAlgebraicsArr^[ALG_IMSfInvTp     ];
  IMS_Xp         := pAlgebraicsArr^[ALG_IMS_Xp        ];
  IMS_X          := pAlgebraicsArr^[ALG_IMS_X         ];
  IMS_fPInitial  := pAlgebraicsArr^[ALG_IMS_fPInitial ];

  IML_Tnom       := pAlgebraicsArr^[ALG_IML_Tnom      ];
  IMS_Tnom       := pAlgebraicsArr^[ALG_IMS_Tnom      ];

  // Calculate the Currents
  local_BusVoltMag := pHardCodedArr^[HARDCODE_LOAD_DeviceVPU];
  LoadObjThetaRadian_In := pHardCodedArr^[HARDCODE_LOAD_DeviceAngleRad];
  LoadObjBusVolt_In := PWDComplex_InitPolarRad(local_BusVoltMag,LoadObjThetaRadian_In);

  //----------------------------------------------------------------------------
  // For the large induction motor dotWr:
  //----------------------------------------------------------------------------
  IML_H := 1.5;
  IML_D := 2;   // damping, same as fDamp
  IML_fInv2H := (1/(2*IML_H))*(1/IML_fPInitial);

  LOCAL_GetMotorDerivatives(LoadObjBusVolt_In, inService_In, loadScalar_in,
                            IML_fPInitial, IML_Rs, IML_fWo, IMLfInvTp, IML_Xp, IML_X, IML_D, IML_fInv2H,
                            IMLSpeedState, IMLEprState, IMLEpiState, IML_Tnom,
                            dotIMLSpeedState, dotIMLEprState, dotIMLEpiState
                            );

  //----------------------------------------------------------------------------
  // For the small induction motor dotWr:
  //----------------------------------------------------------------------------
  IMS_H := 0.7;
  IMS_D := 2;
  IMS_fInv2H := (1/(2*IMS_H))*(1/IMS_fPInitial);

  LOCAL_GetMotorDerivatives(LoadObjBusVolt_In, inService_In, loadScalar_in,
                            IMS_fPInitial, IMS_Rs, IMS_fWo, IMSfInvTp, IMS_Xp, IMS_X, IMS_D, IMS_fInv2H,
                            IMSSpeedState, IMSEprState, IMSEpiState, IMS_Tnom,
                            dotIMSSpeedState, dotIMSEprState, dotIMSEpiState
                            );

// write out new fofx
  pDotXArr^[STATE_IM1SpeedWr] := dotIMLSpeedState;
  pDotXArr^[STATE_IM1Epr    ] := dotIMLEprState;
  pDotXArr^[STATE_IM1Epi    ] := dotIMLEpiState;
  pDotXArr^[STATE_IM2SpeedWr] := dotIMSSpeedState;
  pDotXArr^[STATE_IM2Epr    ] := dotIMSEprState;
  pDotXArr^[STATE_IM2Epi    ] := dotIMSEpiState;
end;

//******************************************************************************
procedure PropogateIgnoredStateAndInput(ParamsAndStates : PTxMyModelData;
                                        SystemOptions : PTxSystemOptions); stdcall;
begin
  // nothing here
end;

//******************************************************************************
function getNonWindUpLimits(ParamsAndStates : PTxMyModelData;
                            SystemOptions : PTxSystemOptions;
                            nonWindUpLimits : PTxNonWindUpLimits): Integer; stdcall;
begin
  Result := 0;   // None
end;

//------------------------------------------------------------------------------
// 'TimeStepEnd'
function TimeStepEnd(ParamsAndStates : PTxMyModelData;
                     SystemOptions : PTxSystemOptions;
                     index : PInteger;
                     MaxPossibleEventIndex : PInteger;
                     EventTime : PDouble;
                     ExtraObjectIndex : PInteger
                     ) : boolean; stdcall;
var local_NewTime   : double;
    fVi, fTi, fTb   : double;
    local_VoltMag   : double;
    fLowVoltTime    : double;
    fLowVoltTimeSet : boolean;
    inService_In    : boolean;

    pFloatParamArr,
    pHardCodedArr,
    pAlgebraicsArr : PDoubleArray;
begin
  result := false; // Only return TRUE if it actually needs to do something!
  EventTime^ := 0;
  ExtraObjectIndex^ := -1;
  MaxPossibleEventIndex^ := 0;
  if index^ = 0 then begin
    //--------------------------------------------------------------------
    // Setup local array variables to make code easier to read
    //--------------------------------------------------------------------
    pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
    pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
    pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);

    inService_In    := pHardCodedArr^[HARDCODE_LOAD_DeviceStatus] > 0.5; // Inservice
    fVi := pFloatParamArr^[PARAM_Vi];
    fTi := pFloatParamArr^[PARAM_Ti];
    fTb := pFloatParamArr^[PARAM_Tb];
    local_VoltMag   := pHardCodedArr^[HARDCODE_LOAD_DeviceVPU];
    fLowVoltTimeSet := pAlgebraicsArr^[ALG_TIMER_LowVoltTimeSet] <> 0;
    fLowVoltTime    := pAlgebraicsArr^[ALG_TIMER_LowVoltTime];
    local_NewTime   := SystemOptions^.SimulationTimeSeconds;

    If inService_In Then Begin
      If (local_VoltMag >= fVi) Then begin
        pAlgebraicsArr^[ALG_TIMER_LowVoltTimeSet] := 0; // Above voltage, so reset
      end
      Else Begin
        If not fLowVoltTimeSet Then Begin
          fLowVoltTime := local_NewTime; // First time below voltage, so
          pAlgebraicsArr^[ALG_TIMER_LowVoltTime] := fLowVoltTime;
          pAlgebraicsArr^[ALG_TIMER_LowVoltTimeSet] := 1;
        End;
        If (local_NewTime - fLowVoltTime) >= fTi Then Begin // Trip threshold exceeded
          EventTime^ := local_NewTime + fTb;
          ExtraObjectIndex^ := -1; // trip itself!
          Result := true; // It needs to do something!
        End;
      End;
    End;
  end
  else begin
    result := false;
  end;
end;

//******************************************************************************
function TimeStepEndAction(ParamsAndStates : PTxMyModelData;
                           SystemOptions : PTxSystemOptions;
                           index : Pinteger;
                           StrSize: PInteger;
                           StrBuf: PChar;
                           dummy : Integer): Integer; stdcall;
var ts : PChar;
begin
  // Anything after the last Pipe character | will be treated as an extra log message to show in event reporting
  case index^ of
    0 : ts := 'OPEN|User_CLOD Under Voltage Trip: ';
   else ts := '';
  end;
  result := LOCAL_StringProcess(StrBuf, StrSize, ts);
end;

//******************************************************************************
// Load-Specific Functions
//******************************************************************************
Function GetDL_Total(local_VoltMag, DL_Coeff, DL_VoltBP, DL_VoltExt, DL_at_1_PU : Double) : Double;
Begin
  If local_VoltMag <= DL_VoltExt Then Result := 0
  Else Begin
    If local_VoltMag >= DL_VoltBP
    Then Result := Math_Y_to_X(local_VoltMag, DL_Coeff)
    Else Result := Math_Y_to_X(DL_VoltBP,DL_Coeff)*(local_VoltMag-DL_VoltExt)/(DL_VoltBP-DL_VoltExt);
    Result := Result*DL_at_1_PU;
  End;
End;

Function GetZIP_Total(local_VoltMag, fZVal, fIVal, fSVal, minVoltSLoad_in, minVoltILoad_in : Double) : Double;
Begin
  Result := Sqr(local_VoltMag)*fZVal;
// Check reduction in constant power load
  If local_VoltMag >= minVoltSLoad_in Then Result := Result + fSVal
  Else Result := Result + 0.5 *( 1 - cos(pi*local_VoltMag/minVoltSLoad_in))*fSVal;
// Check reduction in constant current load
  If local_VoltMag >= minVoltILoad_in Then Result := Result + local_VoltMag*fIVal
  Else Result := Result + sin(pi*0.5*local_VoltMag/minVoltILoad_in)*local_VoltMag*fIVal;
End;

procedure LoadNortonAdmittance(ParamsAndStates : PTxMyModelData;
                               SystemOptions : PTxSystemOptions;
                               theG : PDouble;
                               theB : PDouble
                               ); stdcall;
var t : double;
    IML_Rs, IML_Xp : double;
    IMS_Rs, IMS_Xp : double;
    ZIP_ZMW, ZIP_ZMVR : double;
    loadScalar_in : double;

    pHardCodedArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
Begin
  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);

  theG^ := 0;
  theB^ := 0;

  // Norton Equivalent from the Large Motor
  IML_Rs := pAlgebraicsArr^[ALG_IML_Rs];
  IML_Xp := pAlgebraicsArr^[ALG_IML_Xp];
  t := Sqr(IML_Rs) + Sqr(IML_Xp);
  If t <> 0 Then Begin
    theG^ := theG^ + (IML_Rs)/t;
    theB^ := theB^ - (IML_Xp)/t;
  end;

  // Norton Equivalent from the Small Motor
  IMS_Rs := pAlgebraicsArr^[ALG_IMS_Rs];
  IMS_Xp := pAlgebraicsArr^[ALG_IMS_Xp];
  t := Sqr(IMS_Rs) + Sqr(IMS_Xp);
  If t <> 0 Then Begin
    theG^ := theG^ + (IMS_Rs)/t;
    theB^ := theB^ - (IMS_Xp)/t;
  end;

  // Constant impedance protion of load in here too
  ZIP_ZMW := pAlgebraicsArr^[ALG_ZIP_ZMW];
  ZIP_ZMVR := pAlgebraicsArr^[ALG_ZIP_ZMVR];
  theG^ := theG^ + ZIP_ZMW;
  theB^ := theB^ - ZIP_ZMVR; // ??? sign not sure of

  // Handle the Load Scalar changing these as well
  loadScalar_in := pHardCodedArr^[HARDCODE_LOAD_LoadScalar];
  theG^ := theG^ * loadScalar_in;
  theB^ := theB^ * loadScalar_in;
end;

//******************************************************************************
procedure LoadNortonCurrent(ParamsAndStates : PTxMyModelData;
                            SystemOptions : PTxSystemOptions;
                            IReal : PDouble;
                            IImag : PDouble
                            ); stdcall;
var ThevR, ThevX : double;
    ImpSys : PWDComplex;
    IntVolt : PWDComplex;
    NortonCurrent : PWDComplex;
    LoadObjBusVolt_In : PWDComplex;

    DL_TotalP, DL_TotalQ,
    IML_Rs, IML_Xp,
    IMS_Rs, IMS_Xp,
    IMLEprState, IMLEpiState,
    IMSEprState, IMSEpiState : Double;

    local_BusVoltMag,
    LoadObjThetaRadian_In : double;

    DL_P_at_1_PU, DL_Q_at_1_PU,
    ZIP_ZMW, ZIP_IMW, ZIP_SMW,
    ZIP_ZMVR, ZIP_IMVR, ZIP_SMVR : double;

    loadScalar_in,
    local_e, local_f, local_v2Inv, local_BusVoltMagInv,
    local_ge, local_he, local_gf, local_hf,
    local_ki, local_ks : Double;

    minVoltSLoad_in, minVoltILoad_in : double;

    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
begin
  IReal^ := 0;
  IImag^ := 0;

  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);

  loadScalar_in := pHardCodedArr^[HARDCODE_LOAD_LoadScalar];
  minVoltSLoad_in := SystemOptions^.MinVoltSLoad;
  minVoltILoad_in := SystemOptions^.MinVoltILoad;

  // Network Voltage Presently
  local_BusVoltMag := pHardCodedArr^[HARDCODE_LOAD_DeviceVPU];
  LoadObjThetaRadian_In := pHardCodedArr^[HARDCODE_LOAD_DeviceAngleRad];
  LoadObjBusVolt_In := PWDComplex_InitPolarRad(local_BusVoltMag,LoadObjThetaRadian_In);

  //--------------------------------------------------------------------
  // Large Motor
  //--------------------------------------------------------------------
  IMLEprState := pStatesArr^[STATE_IM1Epr    ];
  IMLEpiState := pStatesArr^[STATE_IM1Epi    ];
  IML_Rs := pAlgebraicsArr^[ALG_IML_Rs]/LoadScalar_In;
  IML_Xp := pAlgebraicsArr^[ALG_IML_Xp]/LoadScalar_In;
  ImpSys := PWDComplex_Init(IML_Rs, IML_Xp);
  IntVolt := PWDComplex_Init(IMLEprState, IMLEpiState);
  NortonCurrent := PWDComplex_Divide(IntVolt, ImpSys);
  // must take NEGATIVE here because the equation is for "generator"
  IReal^ := IReal^ - NortonCurrent.r;
  IImag^ := IImag^ - NortonCurrent.i;

  //--------------------------------------------------------------------
  // Small Motor
  //--------------------------------------------------------------------
  IMSEprState := pStatesArr^[STATE_IM2Epr    ];
  IMSEpiState := pStatesArr^[STATE_IM2Epi    ];
  IMS_Rs := pAlgebraicsArr^[ALG_IMS_Rs]/LoadScalar_In;
  IMS_Xp := pAlgebraicsArr^[ALG_IMS_Xp]/LoadScalar_In;
  ImpSys := PWDComplex_Init(IMS_Rs, IMS_Xp);
  IntVolt := PWDComplex_Init(IMSEprState, IMSEpiState);
  NortonCurrent := PWDComplex_Divide(IntVolt, ImpSys);
  // must take NEGATIVE here because the equation is for "generator"
  IReal^ := IReal^ - NortonCurrent.r;
  IImag^ := IImag^ - NortonCurrent.i;

  //--------------------------------------------------------------------
  // DISCHARGE LIGHTING VALUES
  //--------------------------------------------------------------------
  DL_P_at_1_PU := pAlgebraicsArr^[ALG_DL_P_at_1_PU]*loadScalar_in;
  DL_Q_at_1_PU := pAlgebraicsArr^[ALG_DL_Q_at_1_PU]*loadScalar_in;
  DL_TotalP := GetDL_Total(local_BusVoltMag, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
  DL_TotalQ := GetDL_Total(local_BusVoltMag, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
  NortonCurrent := PWDComplex_Init(DL_TotalP, DL_TotalQ).Divide(LoadObjBusVolt_In);
  NortonCurrent.i := - NortonCurrent.i; // conjugate!  I = (S/V)*
  IReal^ := IReal^ + NortonCurrent.r;
  IImag^ := IImag^ + NortonCurrent.i;

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
  local_e := LoadObjBusVolt_In.r;
  local_f := LoadObjBusVolt_In.i;

  NortonCurrent.r := 0;
  NortonCurrent.i := 0;
  ZIP_SMW      := pAlgebraicsArr^[ALG_ZIP_SMW ]*loadScalar_in;
  ZIP_SMVR     := pAlgebraicsArr^[ALG_ZIP_SMVR]*loadScalar_in;
  if (ZIP_SMW <> 0) or (ZIP_SMvr <> 0) then begin
    // avoid a divide by zero!
    local_v2Inv := sqr(local_e) + sqr(local_f);
    if local_v2Inv > sqr(1E-5) then local_v2Inv := 1/(local_v2Inv)
    else local_v2Inv := 1E6; // just set it to something!
    local_ge := local_e*local_v2Inv;
    local_gf := local_f*local_v2Inv;
    if (local_BusVoltMag < minVoltSLoad_in) then begin
      local_ks := 0.5 * ( 1 - cos(pi*local_BusVoltMag/minVoltSLoad_in) );
      NortonCurrent.r := NortonCurrent.r + local_ks*(local_ge*ZIP_SMW + local_gf*ZIP_SMvr);
      NortonCurrent.i := NortonCurrent.i + local_ks*(local_gf*ZIP_SMW - local_ge*ZIP_SMvr);
    end
    else begin
      NortonCurrent.r := NortonCurrent.r + (local_ge*ZIP_SMW + local_gf*ZIP_SMvr);
      NortonCurrent.i := NortonCurrent.i + (local_gf*ZIP_SMW - local_ge*ZIP_SMvr);
    end;
  end;

  ZIP_IMW      := pAlgebraicsArr^[ALG_ZIP_IMW]*loadScalar_in;
  ZIP_IMVR     := pAlgebraicsArr^[ALG_ZIP_IMVR]*loadScalar_in;
  if (ZIP_iMW <> 0) or (ZIP_iMvr <> 0) then begin
    local_BusVoltMagInv := 1/local_BusVoltMag;
    local_he := local_e*local_BusVoltMagInv;
    local_hf := local_f*local_BusVoltMagInv;
    if (local_BusVoltMag < minVoltILoad_in) then begin
      local_Ki := sin(pi*0.5*local_BusVoltMag/minVoltILoad_in);
      NortonCurrent.r := NortonCurrent.r + local_ki*(local_he*ZIP_iMW + local_hf*ZIP_iMvr);
      NortonCurrent.i := NortonCurrent.i + local_ki*(local_hf*ZIP_iMW - local_he*ZIP_iMvr);
    end
    else begin
      NortonCurrent.r := NortonCurrent.r + (local_he*ZIP_iMW + local_hf*ZIP_iMvr);
      NortonCurrent.i := NortonCurrent.i + (local_hf*ZIP_iMW - local_he*ZIP_iMvr);
    end;
  end;
  IReal^ := IReal^ + NortonCurrent.r;
  IImag^ := IImag^ + NortonCurrent.i;
end;

//******************************************************************************
procedure LoadNortonCurrentAlgebraicDerivative(ParamsAndStates : PTxMyModelData;
                                               SystemOptions : PTxSystemOptions;
                                               IReal_dVreal : PDouble;
                                               IReal_dVimag : PDouble;
                                               IImag_dVreal : PDouble;
                                               IImag_dVimag : PDouble
                                               ); stdcall;
//{****************************************************************************************************************}
//  { Class Procedure TTxDynamicLoadModel.CurrentDerivativesExponentialVoltage returns with the derivatives of the current w.r.t. to the rectangular
//  voltage for functions of the form local_Po*V^local_Pn and local_Qo*V^local_Qn. }
  Procedure CurrentDerivativesExponentialVoltage(local_Po,local_Qo,local_Pn,local_Qn : Double; local_BusVoltage : PWDComplex;
                                    Var dIr_de, dIr_df, dIi_de, dIi_df : double);
  Var local_V,local_e,local_f : Double;  // Voltage magnitude, e and f values
      vp2,vp4,vq2,vq4 : Double;  // Exponential functions
  Begin
  { For this model we then it is just an exponential model, with Po*V^(Pn) + Qo*V^(Qn).
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
   }
    local_V := local_BusVoltage.mag;
    local_e := local_BusVoltage.r;
    local_f := local_BusVoltage.i;
    vp2 := local_Po*Math_Y_to_X(local_V,local_Pn-2);
    vp4 := (local_Pn-2)*local_Po*Math_Y_to_X(local_V,local_Pn-4);  // Use -4 for exponent since it includes /V from partial of e or f w.r.t V
    vq2 := local_Qo*Math_Y_to_X(local_V,local_Qn-2);
    vq4 := (local_Qn-2)*local_Qo*Math_Y_to_X(local_V,local_Qn-4);  // Use -4 for exponent since it includes /V from partial of e or f w.r.t V

    dIr_de := dIr_de + vp2 + (vp4*local_e + vq4*local_f)*local_e;
    dIr_df := dIr_df + vq2 + (vp4*local_e + vq4*local_f)*local_f;
    dIi_de := dIi_de - vq2 + (vp4*local_f - vq4*local_e)*local_e;
    dIi_df := dIi_df + vp2 + (vp4*local_f - vq4*local_e)*local_f;
  End;

var ThevR, ThevX : double;
    ImpSys : PWDComplex;
    IntVolt : PWDComplex;
    NortonCurrent : PWDComplex;
    LoadObjBusVolt_In : PWDComplex;

    DL_TotalP, DL_TotalQ,
    local_BusVoltMag,
    LoadObjThetaRadian_In : double;

    DL_P_at_1_PU, DL_Q_at_1_PU,
    ZIP_ZMW, ZIP_IMW, ZIP_SMW,
    ZIP_ZMVR, ZIP_IMVR, ZIP_SMVR : double;

    loadScalar_in,
    local_Inv_v2,
    local_v,
    local_e, local_f, local_v2, local_BusVoltMagInv,
    local_ge, local_he, local_gf, local_hf,
    local_ki, local_ks,
    local_Ir_Num, local_Ii_Num,
    dks_dv, dks_de, dks_df, local_Inv_v4_ks,
    dki_dv, dki_de, dki_df, local_eOverV, local_fOverV
      : Double;

    minVoltSLoad_in, minVoltILoad_in : double;
    ZIP_dIr_de, ZIP_dIr_df, ZIP_dIi_de, ZIP_dIi_df,
    DL_dIr_de, DL_dIr_df, DL_dIi_de, DL_dIi_df : double;

    pStatesArr : PDoubleArray;
    pHardCodedArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
begin
  DL_dIr_de := 0;
  DL_dIr_df := 0;
  DL_dIi_de := 0;
  DL_dIi_df := 0;

  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pStatesArr := PDoubleArray(ParamsAndStates.States);
  pHardCodedArr := PDoubleArray(ParamsAndStates.HardCodedSignals);
  pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);

  loadScalar_in := pHardCodedArr^[HARDCODE_LOAD_LoadScalar];
  minVoltSLoad_in := SystemOptions^.MinVoltSLoad;
  minVoltILoad_in := SystemOptions^.MinVoltILoad;

  // Network Voltage Presently
  local_BusVoltMag   := pHardCodedArr^[HARDCODE_LOAD_DeviceVPU];
  LoadObjThetaRadian_In := pHardCodedArr^[HARDCODE_LOAD_DeviceAngleRad];
  LoadObjBusVolt_In := PWDComplex_InitPolarRad(local_BusVoltMag,LoadObjThetaRadian_In);

  //----------------------------------------------------------------------------
  // DISCHARGE LIGHTING LOAD
  //----------------------------------------------------------------------------
  if local_BusVoltMag > sqr(1E-5) then begin
    DL_P_at_1_PU := pAlgebraicsArr^[ALG_DL_P_at_1_PU]*loadScalar_in;
    DL_Q_at_1_PU := pAlgebraicsArr^[ALG_DL_Q_at_1_PU]*loadScalar_in;

  // Easy case is if voltage <= DL_VoltExt; then load is extinguished so all derivatives are zero so we don't do anything
    If local_BusVoltMag > GLOBAL_DL_VoltExt Then Begin
    { If voltage is greater than the breakpoint voltage, then it is just an exponential model, with fP^fPCoeff + fQ^fQCoeff.  That is, with expontial
      models we have Ir + jIi = (Po*V^m - jQo*V^n)/(e-jf)*(e+jf)/(e+fj) = Po*V^m - jQo*V^n/(e^2 + f^2)*(e+jf) = Po*V^(m-2) - jQ*Vo^(n-2)*(e+fj).  So
      use the chain rule to get the values. }

      If local_BusVoltMag >= GLOBAL_DL_VoltBP Then Begin
        CurrentDerivativesExponentialVoltage(DL_P_at_1_PU, DL_Q_at_1_PU,
                                             GLOBAL_DL_PCoeff, GLOBAL_DL_QCoeff, LoadObjBusVolt_In,
                                             DL_dIr_de, DL_dIr_df, DL_dIi_de, DL_dIi_df);
      End
      { Otherwise voltage is between the extinction value and the breakpoint voltage, then real and reactive power is being linearly ramped according
        to equation (P(DL_VoltBP)+jQ(DL_VoltBP))*(V - Vext)/(Vbp - Vext). }
      Else Begin
        DL_TotalP := GetDL_Total(local_BusVoltMag, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
        DL_TotalQ := GetDL_Total(local_BusVoltMag, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);

        local_e := LoadObjBusVolt_In.r;
        local_f := LoadObjBusVolt_In.i;
        local_v2 := sqr(local_BusVoltMag);
        local_Inv_v2 := 1/local_v2;

        // Handle the Constant Power Portion of the Load.  The equation being solved here is I = (S/V)*, which becomes
        // (P-jQ)/(e-jf) = (P-jQ)*(e+jf)/(e^2 + f^2).  Derivative values are calculated using d(u/v)/dx = (du*v - dv*u)/(v^2)
        // Model with Ir = (P*e + Q*f)/(e^2 + f^2) and Ii = (P*f - Q*e)/(e^2 + f^2)

        // Calculate the numerator portion of Ir and Ii.
        local_Ir_Num := DL_TotalP*local_e + DL_TotalQ*local_f;
        local_Ii_Num := DL_TotalQ*local_f - DL_TotalQ*local_e;

        // Since voltage is between extinction value and breakpoint value, calculate the scalar, ks, and its sensitivities.
        // Then use the chain rule for differentiation.
        local_ks := (local_BusVoltMag - GLOBAL_DL_VoltExt)/(GLOBAL_DL_VoltBP - GLOBAL_DL_VoltExt);
        local_Inv_v4_ks := local_ks*sqr(local_Inv_v2);

        // Calculate the derivatives (portion that assumes fixed local_ks)
        DL_dIr_de := DL_dIr_de + (DL_TotalP*local_V2  - local_Ir_Num*2*local_e)*local_Inv_v4_ks;
        DL_dIr_df := DL_dIr_df + (DL_TotalQ*local_V2  - local_Ir_num*2*local_f)*local_Inv_v4_ks;
        DL_dIi_de := DL_dIi_de + (-DL_TotalQ*local_V2 - local_Ii_num*2*local_e)*local_Inv_v4_ks;
        DL_dIi_df := DL_dIi_df + (DL_TotalP*local_V2  - local_Ii_num*2*local_f)*local_Inv_v4_ks;

        // Now include the impact of local_ks being a function of voltage }
        dks_de := local_e/(local_BusVoltMag*(GLOBAL_DL_VoltBP-GLOBAL_DL_VoltExt));
        dks_df := local_f/(local_BusVoltMag*(GLOBAL_DL_VoltBP-GLOBAL_DL_VoltExt));
        DL_dIr_de := DL_dIr_de + (local_Ir_num*local_Inv_v2) * dks_de;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
        DL_dIr_df := DL_dIr_df + (local_Ir_num*local_Inv_v2) * dks_df;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
        DL_dIi_de := DL_dIi_de + (local_Ii_num*local_Inv_v2) * dks_de;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
        DL_dIi_df := DL_dIi_df + (local_Ii_num*local_Inv_v2) * dks_df;      // Result for TTXDLIGHTLoadModel.CurrentNonImpedanceDerivatives
      end;
    end;
  end;

  ZIP_dIr_de := 0;
  ZIP_dIr_df := 0;
  ZIP_dIi_de := 0;
  ZIP_dIi_df := 0;

  local_e := LoadObjBusVolt_In.r;
  local_f := LoadObjBusVolt_In.i;
  local_v2 := sqr(local_e) + sqr(local_f);
  // Avoid a divide by zero
  If local_v2 > sqr(1E-5) Then local_Inv_v2 := 1/local_v2
  else local_Inv_v2 := 1E6; // must set it to something then!
  local_v := sqrt(local_v2);
  //----------------------------------------------------------------------------
  // CONSTANT POWER PORTION
  //----------------------------------------------------------------------------
  ZIP_SMW     := pAlgebraicsArr^[ALG_ZIP_SMW ]*loadScalar_in;
  ZIP_SMVR    := pAlgebraicsArr^[ALG_ZIP_SMVR]*loadScalar_in;
  If (local_v2 > 0) and (ZIP_SMW <> 0) or (ZIP_sMvr <> 0) then begin
    // Calculate the numerator portion of Ir and Ii.
    local_Ir_num := ZIP_SMW*local_e + ZIP_sMvr*local_f;
    local_Ii_num := ZIP_SMW*local_f - ZIP_sMvr*local_e;

    // If voltage less than voltage cutoff then include reduction in level (local_ks) keeping in
    // mind local_ks is also a funtion of voltage.  Use chain rule for differentiation
    If local_v < minVoltSLoad_in
    Then local_ks := 0.5 * ( 1 - cos(pi*local_v/minVoltSLoad_in))
    Else local_ks := 1;
    local_Inv_v4_ks := local_ks*sqr(local_Inv_v2);

    // Calculate the derivatives (portion that assumes fixed local_ks)
    ZIP_dIr_de := ZIP_dIr_de + ( ZIP_SMW *local_V2 - local_Ir_num*2*local_e)*local_Inv_v4_ks;
    ZIP_dIr_df := ZIP_dIr_df + ( ZIP_sMvr*local_V2 - local_Ir_num*2*local_f)*local_Inv_v4_ks;
    ZIP_dIi_de := ZIP_dIi_de + (-ZIP_sMvr*local_V2 - local_Ii_num*2*local_e)*local_Inv_v4_ks;
    ZIP_dIi_df := ZIP_dIi_df + ( ZIP_SMW *local_V2 - local_Ii_num*2*local_f)*local_Inv_v4_ks;

    // If local_v < minVoltSLoad_in then also include impact of local_ks being a function of voltage
    If (local_v < minVoltSLoad_in) then begin
      dks_dv := pi/(2*minVoltSLoad_in) * sin(pi*local_v/minVoltSLoad_in);
      dks_de := dks_dv * local_e/local_v;
      dks_df := dks_dv * local_f/local_v;
      ZIP_dIr_de := ZIP_dIr_de + (local_Ir_num*local_Inv_v2) * dks_de;
      ZIP_dIr_df := ZIP_dIr_df + (local_Ir_num*local_Inv_v2) * dks_df;
      ZIP_dIi_de := ZIP_dIi_de + (local_Ii_num*local_Inv_v2) * dks_de;
      ZIP_dIi_df := ZIP_dIi_df + (local_Ii_num*local_Inv_v2) * dks_df;
    End;
  end;

  //----------------------------------------------------------------------------
  // CONSTANT CURRENT PORTION
  //----------------------------------------------------------------------------
  // Handle the portion of the power linearly dependent upon the voltage magnitude.  The equation being differentiated here is
  // ((P-jQ)*sqrt(e^2+f^2)/(e-jf) = (P-jQ)*(e+jf)/(Sqrt(e^2+f^2).  Similar to constant power loads, if voltage is below a threshold we
  // need to consider impact of this scalar on voltage.  Again we calculate derivative values using d(u/v)/dx = (du*v - dv*u)/(v^2)
  ZIP_IMW     := pAlgebraicsArr^[ALG_ZIP_IMW]*loadScalar_in;
  ZIP_IMVR    := pAlgebraicsArr^[ALG_ZIP_IMVR]*loadScalar_in;
  if (local_v2 > 0) and (ZIP_imw <> 0) or (ZIP_imvr <> 0) then begin
    // Calculate the numerator portion of Ir and Ii.
    local_Ir_num := ZIP_imw*local_e + ZIP_imvr*local_f;
    local_Ii_num := ZIP_imw*local_f - ZIP_imvr*local_e;

    // If voltage less than voltage cutoff then include reduction in level (local_ki) keeping in
    // mind local_ki is also a funtion of voltage.  Use chain rule for differentiation
    if (local_V < minVoltILoad_in)
    then local_ki := sin(pi*0.5*local_v/minVoltILoad_in)
    Else local_ki := 1;

    local_eOverV := local_e/local_v;
    local_fOverV := local_f/local_v;
    ZIP_dIr_de := ZIP_dIr_de + ( ZIP_imw *local_v - local_Ir_num*local_eOverV)*local_Inv_v2*local_ki;
    ZIP_dIr_df := ZIP_dIr_df + ( ZIP_imvr*local_v - local_Ir_num*local_fOverV)*local_Inv_v2*local_ki;
    ZIP_dIi_de := ZIP_dIi_de + (-ZIP_imvr*local_v - local_Ii_num*local_eOverV)*local_Inv_v2*local_ki;
    ZIP_dIi_df := ZIP_dIi_df + ( ZIP_imw *local_v - local_Ii_num*local_fOverV)*local_Inv_v2*local_ki;

  // If local_v < minVoltILoad_in then also include impact of local_ki being a function of voltage
    If local_V < minVoltILoad_in Then Begin
      dki_dv := pi*0.5/minVoltILoad_in*cos(pi*0.5*local_v/minVoltILoad_in);
      dki_de := dki_dv*local_eOverV;
      dki_df := dki_dv*local_fOverV;
      ZIP_dIr_de := ZIP_dIr_de + local_Ir_num/local_v*dki_de;
      ZIP_dIr_df := ZIP_dIr_df + local_Ir_num/local_v*dki_df;
      ZIP_dIi_de := ZIP_dIi_de + local_Ii_num/local_v*dki_de;
      ZIP_dIi_df := ZIP_dIi_df + local_Ii_num/local_v*dki_df;
    End;
  end;

  IReal_dVreal^ := DL_dIr_de + ZIP_dIr_de;
  IReal_dVimag^ := DL_dIr_df + ZIP_dIr_df;
  IImag_dVreal^ := DL_dIi_de + ZIP_dIi_de;
  IImag_dVimag^ := DL_dIi_df + ZIP_dIi_df;
end;

//******************************************************************************
function LoadInitializeAlgebraic(ParamsAndStates : PTxMyModelData;
                                 SystemOptions : PTxSystemOptions;

                                 INPUT_PUTol, SteadyStateP, SteadyStateQ, SteadyStateV : Double;

                                 // InitLoadP and InitLoadQ will most frequently
                                 // be equal to the SteadyStateP and SteadyStateQ
                                 // Any difference will be automatically made-up for within
                                 // Simulator by embedding an impedance with the load that makes the
                                 // NetP and NetQ match SteadyStateP and SteadyStateQ initially
                                 InitLoadP, InitLoadQ : PDouble
                                 ) : boolean; stdcall;
  //----------------------------------------------------------------------------
  procedure LOCAL_InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV : double;
                                           var _fPInitial, _fQInitial, _SlipInitial,
                                               _Rs, _Xs, _Xm, _R1, _X1,
                                               _fWo, _Xp, _X, _fInvTp : double
                                               );
  var local_Scale : double;
      tempSlipRes : Byte;
      local_impedanceSystem : PWDComplex;
      local_TElect : double;
  begin
    local_Scale := 1/_fPInitial;
    _Rs := _Rs*local_Scale;
    _Xs := _Xs*local_Scale;
    _Xm := _Xm*local_Scale;
    _R1 := _R1*local_Scale;
    _X1 := _X1*local_Scale;
     _fWo := 2*Pi*loadObjNomFreq_In;
    _Xp := _Xs + _Xm*_X1/(_Xm+_X1); // Thevenin reactance set
    _X  := _Xs + _Xm;  // for a reference see 7.102 of Kundar
     // This is needed in the setfofx function, so we'll set it here
    _fInvTp := _R1/(_X1 + _Xm)*_fWo;  // inverse of Tp
    local_impedanceSystem := PWDComplex_Init(_Rs, _Xp);
    tempSlipRes := TxInductionMachine_CalculateInitialSlip(_Rs, _Xs, _Xm, _R1, _X1,
                                                          SteadyStateV, local_impedanceSystem,
                                                          _fPInitial, INPUT_PUtol,
                                                          // following are calculated values
                                                          _SlipInitial, _fQInitial, local_TElect);
    // return 0 if optimal; 1 if P too high, 2 if P too low
    If (tempSlipRes <> 0) Then Begin
      _fPInitial := 0;
      _fQInitial := 0;
    End;
  end;
  //----------------------------------------------------------------------------

Var fLmotor, fSmotor, fTex, fDis, fP, fKp : Double;
    loadObjNomFreq_In : Double;
    localUsedP_out, localUsedQ_out : Double;

    // Large Motor Variables
    IML_fPInitial, IML_fQInitial, IML_SlipInitial, IMLfInvTp,
    IML_Rs, IML_Xs, IML_Xm, IML_R1, IML_X1, IML_fWo, IML_Xp, IML_X : Double;
    // Small Motor Variables
    IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial, IMSfInvTp,
    IMS_Rs, IMS_Xs, IMS_Xm, IMS_R1, IMS_X1, IMS_fWo, IMS_Xp, IMS_X : Double;

    // Discharge Lighting variables
    DL_Per  : Double;
    DL_P_at_1_PU, DL_Q_at_1_PU : Double;  // Power at unity voltage
    DL_R : Boolean; // variable to hold true/false result
    DL_TotalP, DL_TotalQ : Double;
    pIMLight : Double;

    // ZIP model local parameters
    tsmw, tsmvr , timw, timvr, tzMW, tzMVR : Double;
    local_Scale : Double;
    zip_TotalP, zip_TotalQ : Double;
    minVoltSLoad_in, minVoltILoad_in : Double;

    pFloatParamArr : PDoubleArray;
    pAlgebraicsArr : PDoubleArray;
begin

  If SteadyStateV = 0 Then SteadyStateV := 1;  // Set to unity if not in service

  //--------------------------------------------------------------------
  // Setup local array variables to make code easier to read
  //--------------------------------------------------------------------
  pFloatParamArr := PDoubleArray(ParamsAndStates.FloatParams);
  pAlgebraicsArr := PDoubleArray(ParamsAndStates.Algebraics);

  fLmotor    := pFloatParamArr^[PARAM_PercLmotor];
  fSmotor    := pFloatParamArr^[PARAM_PercSmotor];
  fTex       := pFloatParamArr^[PARAM_PercTex   ];
  fDis       := pFloatParamArr^[PARAM_PercDis   ];
  fP         := pFloatParamArr^[PARAM_PercP     ];
  fKp        := pFloatParamArr^[PARAM_Kp        ];

  loadObjNomFreq_In := SystemOptions^.WBase/(2*pi);
  minVoltSLoad_in   := SystemOptions^.MinVoltSLoad;
  minVoltILoad_in   := SystemOptions^.MinVoltILoad;

  localUsedP_out := 0;
  localUsedQ_out := 0;
  If (SteadyStateV > 0) Then Begin
    //--------------------------------------------------------------------------
    // large motor first
    //--------------------------------------------------------------------------
    IML_fPInitial := SteadyStateP*(fLmotor/100);
    // then q will get figured out by the calculateinitialslip function
    IML_Rs := 0.013;
    IML_Xs := 0.067;
    IML_Xm := 3.8;
    IML_R1 := 0.009;
    IML_X1 := 0.17;

    LOCAL_InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV,
                                   IML_fPInitial, IML_fQInitial, IML_SlipInitial,
                                   IML_Rs, IML_Xs, IML_Xm, IML_R1, IML_X1,
                                   IML_fWo, IML_Xp, IML_X, IMLfInvTp
                                   );
    localUsedP_out := IML_fPInitial;
    localUsedQ_out := IML_fQInitial;

    //--------------------------------------------------------------------------
    // large motor first
    //--------------------------------------------------------------------------
    IMS_fPInitial := SteadyStateP*(fSmotor/100);
    // then q will get figured out by the calculateinitialslip function
    IMS_Rs := 0.031;
    IMS_Xs := 0.1;
    IMS_Xm := 3.2;
    IMS_R1 := 0.018;
    IMS_X1 := 0.18;
    LOCAL_InitializeAlgebraicMotor(INPUT_PUTol, loadObjNomFreq_In, SteadyStateV,
                                   IMS_fPInitial, IMS_fQInitial, IMS_SlipInitial,
                                   IMS_Rs, IMS_Xs, IMS_Xm, IMS_R1, IMS_X1,
                                   IMS_fWo, IMS_Xp, IMS_X, IMSfInvTp
                                   );
    localUsedP_out := localUsedP_out + IMS_fPInitial;
    localUsedQ_out := localUsedQ_out + IMS_fQInitial;
  end;

  //----------------------------------------------------------------------------
  // Discharge Lighting
  //----------------------------------------------------------------------------
  If fDis > 0 Then Begin
    DL_Per     := fDis/100;

    DL_P_at_1_PU := 1;
    DL_Q_at_1_PU := 1;
    DL_R := True;

    DL_TotalP := GetDL_Total(SteadyStateV, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
    If DL_TotalP <> 0 Then begin
      DL_P_at_1_PU := SteadyStateP/DL_TotalP
    end
    Else Begin
      If SteadyStateP <> 0 Then DL_R := False  // Error in initialization caused by voltage being too low
      Else DL_P_at_1_PU := 0;
    End;

    DL_TotalQ := GetDL_Total(SteadyStateV, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
    // If real OK then set the reactive power
    If DL_R Then Begin
      If DL_TotalQ <> 0 Then begin
        DL_Q_at_1_PU := SteadyStateQ/DL_TotalQ;
      end
      Else Begin
        If SteadyStateQ <> 0 Then DL_R := False
        Else DL_Q_at_1_PU := 0;
      End;
    End;
    // Normalize load based on the percent and return total power allocated
    DL_P_at_1_PU := DL_P_at_1_PU*DL_Per;
    DL_Q_at_1_PU := DL_Q_at_1_PU*DL_Per;

    DL_TotalP := GetDL_Total(SteadyStateV, GLOBAL_DL_PCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_P_at_1_PU);
    DL_TotalQ := GetDL_Total(SteadyStateV, GLOBAL_DL_QCoeff, GLOBAL_DL_VoltBP, GLOBAL_DL_VoltExt, DL_Q_at_1_PU);
    localUsedP_out  := localUsedP_out + DL_TotalP;
    localUsedQ_out  := localUsedQ_out + DL_TotalQ;
  End
  else begin
    DL_P_at_1_PU := 0;
    DL_Q_at_1_PU := 0;
  end;;

  pIMLight := fLmotor + fSmotor + fDis;  // Total of induction motors, lighting and constant power

  //----------------------------------------------------------------------------
  // Treat the rest as constant ZIP model
  //----------------------------------------------------------------------------
  // fTex : Treat Transformer Saturation as a constant power
  // fP : Treat constant power portion
  tsmw := fTex + fP;
  timw := 0;
  tzMW := 0;
  tsmvr := tsmw;
  timvr := 0;
  tzmvr := 0;
  If Round(fKp) = 0 Then tsmw := 100 - pIMLight  // All the rest as constant power
  Else If Round(fKp) = 1 Then tiMW := 100 - pIMLight - tsMW
  Else tzMW := 100 - pIMLight - tsMW; // Constant impedance is what is left
  tzMVR := 100 - pIMLight - tsMW;  // Constant impedance reactive for what is left

  zip_TotalP := GetZIP_Total(SteadyStateV, tzMW, tIMW, tSMW, minVoltSLoad_in, minVoltILoad_in);
  zip_TotalQ := GetZIP_Total(SteadyStateV, tZMVR, tIMVR, tSMVR, minVoltSLoad_in, minVoltILoad_in);

  If zip_TotalP <> 0 Then local_Scale := (SteadyStateP - localUsedP_Out)/zip_TotalP
  Else local_Scale := 1;
  tzMW := tzMW*local_Scale;
  timw := timw*local_Scale;
  tsmw := tsmw*local_Scale;

  If zip_TotalQ <> 0 Then local_Scale := (SteadyStateQ - localUsedQ_Out)/zip_TotalQ
  Else local_Scale := 1;
  tzMVR  := tzMVR*local_Scale;
  tiMVR  := tiMVR*local_Scale;
  tsMVR  := tsMVR*local_Scale;

  // Specifies that all of the power is allocated...
  InitLoadP^ := SteadyStateP;
  InitLoadQ^  := SteadyStateQ;

  // LARGE MOTOR STUFF
  pAlgebraicsArr^[ALG_IML_Rs         ] := IML_Rs;
  pAlgebraicsArr^[ALG_IML_Xs         ] := IML_Xs;
  pAlgebraicsArr^[ALG_IML_fWo        ] := IML_fWo;
  pAlgebraicsArr^[ALG_IMLfInvTp      ] := IMLfInvTp;
  pAlgebraicsArr^[ALG_IML_Xp         ] := IML_Xp;
  pAlgebraicsArr^[ALG_IML_X          ] := IML_X;
  pAlgebraicsArr^[ALG_IML_SlipInitial] := IML_SlipInitial;
  pAlgebraicsArr^[ALG_IML_fPInitial  ] := IML_fPInitial;
  pAlgebraicsArr^[ALG_IML_fQInitial  ] := IML_fQInitial;

  // SMALL MOTOR STUFF
  pAlgebraicsArr^[ALG_IMS_Rs         ] := IMS_Rs;
  pAlgebraicsArr^[ALG_IMS_Xs         ] := IMS_Xs;
  pAlgebraicsArr^[ALG_IMS_fWo        ] := IMS_fWo;
  pAlgebraicsArr^[ALG_IMSfInvTp      ] := IMSfInvTp;
  pAlgebraicsArr^[ALG_IMS_Xp         ] := IMS_Xp;
  pAlgebraicsArr^[ALG_IMS_X          ] := IMS_X;
  pAlgebraicsArr^[ALG_IMS_SlipInitial] := IMS_SlipInitial;
  pAlgebraicsArr^[ALG_IMS_fPInitial  ] := IMS_fPInitial;
  pAlgebraicsArr^[ALG_IMS_fQInitial  ] := IMS_fQInitial;

  // DISCHARGE LIGHTING VALUES
  pAlgebraicsArr^[ALG_DL_P_at_1_PU   ] := DL_P_at_1_PU;
  pAlgebraicsArr^[ALG_DL_Q_at_1_PU   ] := DL_Q_at_1_PU;

  // ZIP LOAD VALUES
  pAlgebraicsArr^[ALG_ZIP_ZMW        ] := tZMW;
  pAlgebraicsArr^[ALG_ZIP_IMW        ] := tIMW;
  pAlgebraicsArr^[ALG_ZIP_SMW        ] := tSMW;
  pAlgebraicsArr^[ALG_ZIP_ZMVR       ] := tZMVR;
  pAlgebraicsArr^[ALG_ZIP_IMVR       ] := tIMVR;
  pAlgebraicsArr^[ALG_ZIP_SMVR       ] := tSMVR;

  // Low Voltage Trip Timer
  pAlgebraicsArr^[ALG_TIMER_LowVoltTimeSet] := 0;
  pAlgebraicsArr^[ALG_TIMER_LowVoltTime] := 0;

  Result := true;
end;

exports DLLVersion
        , modelClassName
        , allParamCounts
        , parameterName
        , stateName
     // , OtherObjectClass
     // , OtherObjectDescription
        , getDefaultParameterValue
     // , getStringParamDefaultValue
     // , signalSelection

        , SubIntervalPower2Exponent
        , initializeYourself
        , calculateFofX
        , PropogateIgnoredStateAndInput
        , getNonWindUpLimits
        , TimeStepEnd
        , TimeStepEndAction

        , LoadNortonAdmittance
        , LoadNortonCurrent
        , LoadNortonCurrentAlgebraicDerivative
        , LoadInitializeAlgebraic

        ;

begin
//
end.
