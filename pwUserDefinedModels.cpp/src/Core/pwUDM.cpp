module pwUDM;

import std;

namespace pwUDM {

int pwStringCopy(wchar_t* buffer, const int* bufferSize, const wchar_t* string)
{
    const std::size_t stringLength = std::wcslen(string);
    if ((0 < stringLength) && (stringLength <= std::size_t(*bufferSize)))
    {
        std::wmemcpy(buffer, string, stringLength);
        buffer[stringLength] = L'\0';
    }
    return int(stringLength);
}

const wchar_t* pwModelClassString(ModelClass udmClass)
{
    switch (udmClass)
    {
        case ModelClass::Machine:       return L"UserDefinedMachineModel";
        case ModelClass::Exciter:       return L"UserDefinedExciter";
        case ModelClass::Governor:      return L"UserDefinedGovernor";
        case ModelClass::Stabilizer:    return L"UserDefinedStabilizer";
        case ModelClass::Load:          return L"UserDefinedLoadModel";
        case ModelClass::MTDCConvertor: return L"UserDefinedMTDCConverter";
        case ModelClass::MTDCLine:      return L"UserDefinedMultiTerminalDC";
        // Add here in the future
    }
    return L"UserDefinedModel";
}

}
