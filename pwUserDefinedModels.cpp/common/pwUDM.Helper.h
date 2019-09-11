#ifndef PW_UDM_HELPER
#define PW_UDM_HELPER

enum enumModelClass
{
    mcnExciter
};

inline int pwStringCopy(wchar_t* buffer, int* bufferSize, wchar_t* string)
{
    size_t stringLength;

    stringLength = wcslen(string);    
    if ((0 < stringLength) && (stringLength <= size_t(*bufferSize)))
    {
        wcsncpy_s(buffer, stringLength + 1, string, stringLength);
    }
    return(int(stringLength));
}

inline wchar_t* pwModelClassString(const enumModelClass udmClass)
{
    wchar_t* str;

    switch (udmClass)
    {
        case mcnExciter: str = L"UserDefinedExciter";
                         break;
        // Add here in the future
        default: str = L"UserDefinedModel";
    }
    return(str);
}

#endif // PW_UDM_HELPER