module pwUDM.Core;

import std;

namespace pwUDM {

const wchar_t* pwModelClassString(ModelClass modelClass)
{
    switch (modelClass)
    {
        case ModelClass::Exciter:       return L"UserDefinedExciter";
        case ModelClass::Governor:      return L"UserDefinedGovernor";
        case ModelClass::Load:          return L"UserDefinedLoadModel";
        case ModelClass::Machine:       return L"UserDefinedMachineModel";
        case ModelClass::Stabilizer:    return L"UserDefinedStabilizer";
        case ModelClass::MTDCConvertor: return L"UserDefinedMTDCConverter";
        case ModelClass::MTDCLine:      return L"UserDefinedMultiTerminalDC";
        // Add here in the future
    }
    return L"";
}

int pwStringCopy(wchar_t* StrBuf, int* StrSize, const wchar_t* SourceString)
{
    const int result = int(std::wcslen(SourceString));
    if ((result > 0) && (*StrSize >= result))
    {
        std::wmemcpy(StrBuf, SourceString, std::size_t(result));
        StrBuf[result] = L'\0';
    }
    return result;
}

}
