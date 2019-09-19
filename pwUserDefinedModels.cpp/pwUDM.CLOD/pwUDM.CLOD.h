#ifndef PW_UDM_CLOD
#define PW_UDM_CLOD

// This implements the existing CLOD model as an example
// ----------------------------------------------------------------------------------------------
// For the Model
const int N_F_PARAMS = 9;     // number of float model params
const int N_I_PARAMS = 0;     // number of int model params
const int N_S_PARAMS = 0;     // number of string model params
const int N_STATES = 6;     // number of model states
const int N_ALGEBRAICS = 30;     // number of model algebraics

// For the NonWindUpLimits
const int N_NONWINDUPLIMITS		= 0;	// number of nonwindup

// ---------------------------------------------------------------------------
  // These are just to make the code easier to read
  // Parameter Value indices.

// Integers
const int PARAM_Ics = 0;

  // Double Parameters
const int PARAM_PercLmotor = 0;
const int PARAM_PercSmotor = 1;
const int PARAM_PercTex = 2;
const int PARAM_PercDis = 3;
const int PARAM_PercP = 4;
const int PARAM_Kp = 5;
const int PARAM_Vi = 6;
const int PARAM_Ti = 7;
const int PARAM_Tb = 8;

// State Value Indices.
const int STATE_IM1SpeedWr = 0;
const int STATE_IM1Epr = 1;
const int STATE_IM1Epi = 2;
const int STATE_IM2SpeedWr = 3;
const int STATE_IM2Epr = 4;
const int STATE_IM2Epi = 5;

// Algebraic Value Indices
// Large Motor Algebraics
const int ALG_IML_Rs = 0;
const int ALG_IML_Xs = 1;
const int ALG_IML_fWo = 2;
const int ALG_IMLfInvTp = 3;
const int ALG_IML_Xp = 4;
const int ALG_IML_X = 5;
const int ALG_IML_SlipInitial = 6;
const int ALG_IML_fPInitial = 7;
const int ALG_IML_fQInitial = 8;
const int ALG_IML_Tnom = 9;
// Small Motor Algebraics
const int ALG_IMS_Rs = 10;
const int ALG_IMS_Xs = 11;
const int ALG_IMS_fWo = 12;
const int ALG_IMSfInvTp = 13;
const int ALG_IMS_Xp = 14;
const int ALG_IMS_X = 15;
const int ALG_IMS_SlipInitial = 16;
const int ALG_IMS_fPInitial = 17;
const int ALG_IMS_fQInitial = 18;
const int ALG_IMS_Tnom = 19;
// Discharge Lighting algebraics
const int ALG_DL_P_at_1_PU = 20;
const int ALG_DL_Q_at_1_PU = 21;
// ZIP Model algebraics
const int ALG_ZIP_ZMW = 22;
const int ALG_ZIP_IMW = 23;
const int ALG_ZIP_SMW = 24;
const int ALG_ZIP_ZMVR = 25;
const int ALG_ZIP_IMVR = 26;
const int ALG_ZIP_SMVR = 27;
// Timer for Load Tripping
const int ALG_TIMER_LowVoltTimeSet = 28;
const int ALG_TIMER_LowVoltTime = 29;

// These are hard-coded parameters for modeling the discharge lighting load
const double GLOBAL_DL_PCoeff = 1.0; // constant CURRENT Real Power
const double GLOBAL_DL_QCoeff = 4.5; // much higher coefficent for Reactive Power
const double GLOBAL_DL_VoltBP = 0.75;
const double GLOBAL_DL_VoltExt = 0.65;


// Some classes specific to this module
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

// saurav 05/25/12 These functions are internal to the .dll file
bool	DoubleToBoolean(double thedoub);
PWDComplex	PWDComplex_InitPolarRad(double tmag, double tangle);
PWDComplex	PWDComplex_Init(double r, double i);

#endif //PW_UDM_CLOD