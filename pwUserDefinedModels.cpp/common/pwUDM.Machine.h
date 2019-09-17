#ifndef PW_UDM_MACHINE
#define PW_UDM_MACHINE

#include "pwUDM.h"

// Machine Hard-Coded Signal Indices
// Note: These are the SAME for all machines and are hard-cded values that Simulator ALWAYS passes to the DLL
// If additional inputs are needed from Simulator, then you MUST define them in "Algebraics"
const	int HARDCODE_MACHINE_TSGenFieldV = 0;
const	int HARDCODE_MACHINE_TSPmech     = 1;
const	int HARDCODE_MACHINE_InitVreal   = 2;
const	int HARDCODE_MACHINE_InitVimag   = 3;
const	int HARDCODE_MACHINE_InitIreal   = 4;
const	int HARDCODE_MACHINE_InitIimag   = 5;
const	int HARDCODE_MACHINE_TSstateId   = 6;
const	int HARDCODE_MACHINE_TSstateIq   = 7;

// These functions need to be exported in the DLL
PW_UDM_DllExport(double) MachineSpeedDeviationOut(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(void) MachineTheveninImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* theR, double* theX);
PW_UDM_DllExport(void) MachineTheveninVoltage(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* Delta, double* Vd, double* Vq);
PW_UDM_DllExport(double) MachineFieldCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(double) MachineElectricalTorque(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(double) MachineNortonCurrent(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* IReal, double* IImag);
PW_UDM_DllExport(double) MachineHighVReactiveCurrentLim(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions);
PW_UDM_DllExport(void) MachineLowVActiveCurrentPoints(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* LVPnt1, double* LVPnt0);
PW_UDM_DllExport(void) MachineCompensatingImpedance(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, double* RComp, double* XComp);

#endif // PW_UDM_EXCITER