// new_module: scaffolds a paired <Name>.cppm + <Name>.cpp and registers them in CMakeLists.txt.
//
//   new_module <SubLibrary> <ModuleName> [--private] [--declarations-only]
//                                                      adds a module to a static sub-library;
//                                                      --declarations-only creates the .cppm alone (no empty .cpp)
//   new_module --model <ModelName>                     adds a model DLL (module pwUDM.<ModelName>)
//
// Run from the repository root. Built with -fno-exceptions.
import std;

namespace fs = std::filesystem;

namespace {

constexpr std::string_view ModelsMarker = "# [new_module:models]";

constexpr std::string_view InterfaceTemplate = R"(export module @MODULE@;

import std;

export namespace pwUDM {

}
)";

constexpr std::string_view ImplementationTemplate = R"(module @MODULE@;

import std;

namespace pwUDM {

}
)";

constexpr std::string_view ModelInterfaceTemplate = R"(// Model constants for @NAME@: parameter, state and algebraic counts and indices.
export module pwUDM.@NAME@;

import std;

export namespace pwUDM::@NAME@ {

    // For the Model
    inline constexpr int N_F_PARAMS = 0;     // number of float model params
    inline constexpr int N_I_PARAMS = 0;     // number of int model params
    inline constexpr int N_S_PARAMS = 0;     // number of string model params
    inline constexpr int N_STATES = 0;       // number of model states
    inline constexpr int N_ALGEBRAICS = 0;   // number of model algebraics

    // For the NonWindUpLimits
    inline constexpr int N_NONWINDUPLIMITS = 0; // number of nonwindup

    // Parameter, State and Algebraic Value indices go here.

}
)";

constexpr std::string_view ModelImplementationTemplate = R"(module pwUDM.@NAME@;

import std;
import pwUDM;
// Also import the model class module (pwUDM.Machine, .Exciter, .Governor, .Stabilizer or .Load)
// and define the exports it declares.

namespace pwUDM::@NAME@ {

extern "C" __declspec(dllexport) int __stdcall DLLVersion()
{
    return 1;
}

extern "C" __declspec(dllexport) int __stdcall modelClassName(int* StrSize, wchar_t* StrBuf, int dummy)
{
    // Choose the class this model implements.
    return pwStringCopy(StrBuf, StrSize, pwModelClassString(ModelClass::Machine));
}

extern "C" __declspec(dllexport) void __stdcall allParamCounts(TTxParamCounts* numbersOfEverything, double* timeStepSeconds)
{
    numbersOfEverything->nFloatParams = N_F_PARAMS;
    numbersOfEverything->nIntParams = N_I_PARAMS;
    numbersOfEverything->nStrParams = N_S_PARAMS;
    numbersOfEverything->nStates = N_STATES;
    numbersOfEverything->nAlgebraics = N_ALGEBRAICS;
    numbersOfEverything->nNonWindUpLimits = N_NONWINDUPLIMITS;
}

extern "C" __declspec(dllexport) int __stdcall parameterName(int* ParamNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    return pwStringCopy(StrBuf, StrSize, L"");
}

extern "C" __declspec(dllexport) int __stdcall stateName(int* StateNum, int* StrSize, wchar_t* StrBuf, int dummy)
{
    return pwStringCopy(StrBuf, StrSize, L"");
}

extern "C" __declspec(dllexport) void __stdcall getDefaultParameterValue(TTxMyModelData* ParamsAndStates)
{
}

extern "C" __declspec(dllexport) int __stdcall SubIntervalPower2Exponent(TTxMyModelData* ParamsAndStates, double* timeStepSeconds)
{
    return 0;
}

extern "C" __declspec(dllexport) void __stdcall initializeYourself(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
}

extern "C" __declspec(dllexport) void __stdcall calculateFofX(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits, TDoubleArray* dotX)
{
}

extern "C" __declspec(dllexport) void __stdcall PropagateIgnoredStateAndInput(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions)
{
}

extern "C" __declspec(dllexport) int __stdcall getNonWindUpLimits(TTxMyModelData* ParamsAndStates, TTxSystemOptions* SystemOptions, TTxNonWindUpLimits* nonWindUpLimits)
{
    return N_NONWINDUPLIMITS;
}

}
)";

std::string Replace(std::string text, std::string_view token, std::string_view value)
{
    std::size_t pos = text.find(token);
    while (pos != std::string::npos)
    {
        text.replace(pos, token.size(), value);
        pos = text.find(token, pos + value.size());
    }
    return text;
}

bool IsValidName(std::string_view name)
{
    if (name.empty() || std::isalpha(static_cast<unsigned char>(name[0])) == 0)
    {
        return false;
    }
    for (const char c : name)
    {
        if (std::isalnum(static_cast<unsigned char>(c)) == 0 && c != '_' && c != '.')
        {
            return false;
        }
    }
    return true;
}

bool ReadLines(const fs::path& path, std::vector<std::string>& lines)
{
    std::ifstream stream(path);
    if (!stream)
    {
        return false;
    }
    std::string line;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        lines.push_back(line);
    }
    return true;
}

bool WriteLines(const fs::path& path, const std::vector<std::string>& lines)
{
    std::ofstream stream(path, std::ios::binary);
    if (!stream)
    {
        return false;
    }
    for (const std::string& line : lines)
    {
        stream << line << '\n';
    }
    return static_cast<bool>(stream);
}

bool WriteText(const fs::path& path, const std::string& text)
{
    std::ofstream stream(path, std::ios::binary);
    if (!stream)
    {
        return false;
    }
    stream << text;
    return static_cast<bool>(stream);
}

bool StartsWithTrimmed(const std::string& line, std::string_view prefix)
{
    const std::size_t first = line.find_first_not_of(" \t");
    return first != std::string::npos && std::string_view(line).substr(first).starts_with(prefix);
}

bool IsTrimmedEqual(const std::string& line, std::string_view text)
{
    const std::size_t first = line.find_first_not_of(" \t");
    return first != std::string::npos && std::string_view(line).substr(first) == text;
}

// Finds the end of the target_sources(<target> ... ) block that starts at `begin`.
std::size_t FindBlockEnd(const std::vector<std::string>& lines, std::size_t begin)
{
    for (std::size_t i = begin; i < lines.size(); ++i)
    {
        if (IsTrimmedEqual(lines[i], ")"))
        {
            return i;
        }
    }
    return lines.size();
}

// Inserts `.cpp` under the first bare PRIVATE and `.cppm` under the public (CXX_MODULES) or private
// (private_modules) FILE_SET line.
bool RegisterInSubLibrary(std::vector<std::string>& lines, std::string_view subLibrary, std::string_view stem, bool isPrivate, bool withCpp)
{
    const std::string header = std::format("target_sources({}", subLibrary);
    std::size_t begin = 0;
    while (begin < lines.size() && !StartsWithTrimmed(lines[begin], header))
    {
        ++begin;
    }
    const std::size_t end = FindBlockEnd(lines, begin);
    if (begin >= lines.size() || end >= lines.size())
    {
        return false;
    }

    const std::string fileSetLine = isPrivate ? "FILE_SET private_modules TYPE CXX_MODULES FILES" : "FILE_SET CXX_MODULES FILES";
    std::size_t sourcesLine = lines.size();
    std::size_t moduleLine = lines.size();
    for (std::size_t i = begin + 1; i < end; ++i)
    {
        if (sourcesLine == lines.size() && IsTrimmedEqual(lines[i], "PRIVATE"))
        {
            sourcesLine = i;
        }
        else if (IsTrimmedEqual(lines[i], fileSetLine) && IsTrimmedEqual(lines[i - 1], isPrivate ? "PRIVATE" : "PUBLIC"))
        {
            moduleLine = i;
        }
    }
    if (moduleLine == lines.size() || (withCpp && sourcesLine == lines.size()))
    {
        return false;
    }

    // Insert the later line first so the earlier index stays valid.
    const std::string cppm = std::format("        src/{}/{}.cppm", subLibrary, stem);
    const std::string cpp = std::format("        src/{}/{}.cpp", subLibrary, stem);
    if (!withCpp)
    {
        lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(moduleLine) + 1, cppm);
        return true;
    }
    const std::size_t first = std::min(sourcesLine, moduleLine);
    const std::size_t second = std::max(sourcesLine, moduleLine);
    const std::string& firstText = (first == sourcesLine) ? cpp : cppm;
    const std::string& secondText = (second == sourcesLine) ? cpp : cppm;
    lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(second) + 1, secondText);
    lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(first) + 1, firstText);
    return true;
}

bool RegisterModel(std::vector<std::string>& lines, std::string_view name)
{
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        if (IsTrimmedEqual(lines[i], ModelsMarker))
        {
            lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(i) + 1, std::format("pwudm_add_model({})", name));
            return true;
        }
    }
    return false;
}

int Fail(std::string_view message)
{
    std::println(std::cerr, "new_module: {}", message);
    return 1;
}

int Usage()
{
    std::println(std::cerr, "usage: new_module <SubLibrary> <ModuleName> [--private] [--declarations-only]");
    std::println(std::cerr, "       new_module --model <ModelName>");
    return 2;
}

} // namespace

int main(int argc, char** argv)
{
    std::vector<std::string> args;
    bool isPrivate = false;
    bool isModel = false;
    bool declarationsOnly = false;
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "--private") { isPrivate = true; }
        else if (arg == "--model") { isModel = true; }
        else if (arg == "--declarations-only") { declarationsOnly = true; }
        else { args.push_back(arg); }
    }
    if (args.size() != (isModel ? 1U : 2U) || (isModel && declarationsOnly))
    {
        return Usage();
    }

    const std::string subLibrary = isModel ? "" : args[0];
    const std::string& name = args.back();
    if (!IsValidName(name) || (!isModel && !IsValidName(subLibrary)))
    {
        return Fail("names may only contain letters, digits, '_' and '.'");
    }

    const std::string stem = isModel ? ("pwUDM." + name) : name;
    const fs::path directory = isModel ? fs::path("src") / name : fs::path("src") / subLibrary;
    std::error_code ec;
    if (!isModel && !fs::is_directory(directory, ec))
    {
        return Fail(std::format("unknown sub-library '{}' (expected a folder under src/)", subLibrary));
    }

    const fs::path cppmPath = directory / (stem + ".cppm");
    const fs::path cppPath = directory / (stem + ".cpp");
    const bool withCpp = !declarationsOnly;
    if (fs::exists(cppmPath, ec) || (withCpp && fs::exists(cppPath, ec)))
    {
        return Fail(std::format("{} already exists", stem));
    }

    std::vector<std::string> cmakeLines;
    if (!ReadLines("CMakeLists.txt", cmakeLines))
    {
        return Fail("cannot read CMakeLists.txt (run from the repository root)");
    }
    const bool registered = isModel ? RegisterModel(cmakeLines, name) : RegisterInSubLibrary(cmakeLines, subLibrary, stem, isPrivate, withCpp);
    if (!registered)
    {
        return Fail("could not find the registration point in CMakeLists.txt");
    }

    fs::create_directories(directory, ec);
    const std::string_view interfaceText = isModel ? ModelInterfaceTemplate : InterfaceTemplate;
    const std::string_view implementationText = isModel ? ModelImplementationTemplate : ImplementationTemplate;
    const bool written = WriteText(cppmPath, Replace(Replace(std::string(interfaceText), "@MODULE@", stem), "@NAME@", name))
        && (!withCpp || WriteText(cppPath, Replace(Replace(std::string(implementationText), "@MODULE@", stem), "@NAME@", name)))
        && WriteLines("CMakeLists.txt", cmakeLines);
    if (!written)
    {
        return Fail("failed to write files");
    }

    if (withCpp)
    {
        std::println("created {} and {}, registered in CMakeLists.txt", cppmPath.generic_string(), cppPath.generic_string());
    }
    else
    {
        std::println("created {} (declarations only, no .cpp), registered in CMakeLists.txt", cppmPath.generic_string());
    }
    return 0;
}
