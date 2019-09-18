#ifndef PW_UDM_CLOD
#define PW_UDM_CLOD

// This implements the existing CLOD model as an example
// ----------------------------------------------------------------------------------------------
// For the Model
const	int	N_F_PARAMS				= 11;	// number of float model params
const	int N_I_PARAMS				= 0;	// number of int model params
const	int N_S_PARAMS				= 0;	// number of string model params
const	int N_STATES				= 6;	// number of model states
const	int N_ALGEBRAICS			= 33;	// number of model algebraics

// For the Network
const	int N_F_NETWORKINPUTS		= 31;	// number of network inputs
const	int N_F_NETWORKOUTPUTS		= 19;	// number of network outputs
const	int N_F_FIXEDINPUTS			= 0;	// number of fixed inputs

// For the NonWindUpLimits
const	int N_NONWINDUPLIMITS		= 0;	// number of nonwindup

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