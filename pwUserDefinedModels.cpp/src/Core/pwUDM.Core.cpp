module pwUDM.Core;

import std;

namespace pwUDM {

const char16_t* pwModelClassString(ModelClass modelClass)
{
    switch (modelClass)
    {
        case ModelClass::Exciter:       return u"UserDefinedExciter";
        case ModelClass::Governor:      return u"UserDefinedGovernor";
        case ModelClass::Load:          return u"UserDefinedLoadModel";
        case ModelClass::Machine:       return u"UserDefinedMachineModel";
        case ModelClass::Stabilizer:    return u"UserDefinedStabilizer";
        case ModelClass::MTDCConvertor: return u"UserDefinedMTDCConverter";
        case ModelClass::MTDCLine:      return u"UserDefinedMultiTerminalDC";
        // Add here in the future
    }
    return u"";
}

int pwProvideString(char16_t* StrBuf, int* StrSize, const char16_t* SourceString)
{
    const int result = int(std::char_traits<char16_t>::length(SourceString));
    if ((result > 0) && (*StrSize >= result))
    {
        std::char_traits<char16_t>::copy(StrBuf, SourceString, std::size_t(result));
        StrBuf[result] = u'\0';
    }
    return result;
}

}
