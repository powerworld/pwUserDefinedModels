library pwUDM;

// One project, one DLL per model. The build configuration (pwUDM.CLOD_Release, pwUDM.TGOV1_Debug, ...) defines
// MODEL_<NAME>, which picks the model unit that is compiled in and names the DLL pwUDM.<NAME>.pas.dll.
// Each model unit lists its own exports.

{$IF     DEFINED(MODEL_CLOD)}   {$LIBSUFFIX '.CLOD.pas'}
{$ELSEIF DEFINED(MODEL_GENCLS)} {$LIBSUFFIX '.GENCLS.pas'}
{$ELSEIF DEFINED(MODEL_IEEEST)} {$LIBSUFFIX '.IEEEST.pas'}
{$ELSEIF DEFINED(MODEL_IEEET1)} {$LIBSUFFIX '.IEEET1.pas'}
{$ELSEIF DEFINED(MODEL_TGOV1)}  {$LIBSUFFIX '.TGOV1.pas'}
{$ELSE}
  {$MESSAGE FATAL 'No model selected: use a build configuration such as pwUDM.TGOV1_Release (it defines MODEL_TGOV1)'}
{$IFEND}

uses
  pwUDM.Core in 'src\Core\pwUDM.Core.pas',
{$IFDEF MODEL_CLOD}
  pwUDM.Load in 'src\Core\pwUDM.Load.pas',
  pwUDM.CLOD in 'src\CLOD\pwUDM.CLOD.pas'
{$ENDIF}
{$IFDEF MODEL_GENCLS}
  pwUDM.Machine in 'src\Core\pwUDM.Machine.pas',
  pwUDM.GENCLS in 'src\GENCLS\pwUDM.GENCLS.pas'
{$ENDIF}
{$IFDEF MODEL_IEEEST}
  pwUDM.Stabilizer in 'src\Core\pwUDM.Stabilizer.pas',
  pwUDM.IEEEST in 'src\IEEEST\pwUDM.IEEEST.pas'
{$ENDIF}
{$IFDEF MODEL_IEEET1}
  pwUDM.Exciter in 'src\Core\pwUDM.Exciter.pas',
  pwUDM.IEEET1 in 'src\IEEET1\pwUDM.IEEET1.pas'
{$ENDIF}
{$IFDEF MODEL_TGOV1}
  pwUDM.Governor in 'src\Core\pwUDM.Governor.pas',
  pwUDM.TGOV1 in 'src\TGOV1\pwUDM.TGOV1.pas'
{$ENDIF}
  ;

{$R *.res}

begin
end.
