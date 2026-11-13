// check_rules: deterministic gate for CPP_PROGRAMMING_RULES.md (Orthodox C++23 & Pure Modules).
// Run from the repository root; scans ./src. Exit code 0 means every rule passed.
// Built with -fno-exceptions: only error_code filesystem overloads are used.
import std;

namespace fs = std::filesystem;

namespace {

constexpr std::size_t MaxCppmLines = 250;

struct Report
{
    int violations = 0;

    void Fail(const fs::path& file, int line, std::string_view message)
    {
        ++violations;
        std::println(std::cerr, "{}:{}: rule violation: {}", file.generic_string(), line, message);
    }
};

struct SourceFile
{
    fs::path path;
    std::vector<std::string> raw;      // lines as written
    std::vector<std::string> code;     // lines with comments and literal contents blanked
};

bool IsIdentChar(char c)
{
    return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

// True when `word` appears in `text` delimited by non-identifier characters.
bool ContainsWord(std::string_view text, std::string_view word)
{
    std::size_t pos = text.find(word);
    while (pos != std::string_view::npos)
    {
        const bool leftOk = (pos == 0) || !IsIdentChar(text[pos - 1]);
        const std::size_t end = pos + word.size();
        const bool rightOk = (end >= text.size()) || !IsIdentChar(text[end]);
        if (leftOk && rightOk)
        {
            return true;
        }
        pos = text.find(word, pos + 1);
    }
    return false;
}

std::string Trim(std::string_view text)
{
    std::size_t first = 0;
    while (first < text.size() && std::isspace(static_cast<unsigned char>(text[first])) != 0)
    {
        ++first;
    }
    std::size_t last = text.size();
    while (last > first && std::isspace(static_cast<unsigned char>(text[last - 1])) != 0)
    {
        --last;
    }
    return std::string(text.substr(first, last - first));
}

// Replaces comments and the insides of string/char literals with spaces, keeping line structure.
std::vector<std::string> BlankCommentsAndLiterals(const std::vector<std::string>& lines)
{
    std::vector<std::string> result;
    bool inBlockComment = false;
    for (const std::string& line : lines)
    {
        std::string out(line.size(), ' ');
        char quote = 0;
        for (std::size_t i = 0; i < line.size(); ++i)
        {
            const char c = line[i];
            const char next = (i + 1 < line.size()) ? line[i + 1] : '\0';
            if (inBlockComment)
            {
                if (c == '*' && next == '/')
                {
                    inBlockComment = false;
                    ++i;
                }
            }
            else if (quote != 0)
            {
                if (c == '\\') { ++i; }
                else if (c == quote) { quote = 0; out[i] = c; }
            }
            else if (c == '/' && next == '/') { break; }
            else if (c == '/' && next == '*') { inBlockComment = true; ++i; }
            else if (c == '"' || c == '\'') { quote = c; out[i] = c; }
            else { out[i] = c; }
        }
        result.push_back(out);
    }
    return result;
}

bool LoadSource(const fs::path& path, SourceFile& file)
{
    std::ifstream stream(path);
    if (!stream)
    {
        return false;
    }
    file.path = path;
    std::string line;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        file.raw.push_back(line);
    }
    file.code = BlankCommentsAndLiterals(file.raw);
    return true;
}

// ---------------------------------------------------------------------------------------------
// Rules that apply to every .cppm and .cpp
// ---------------------------------------------------------------------------------------------

void CheckForbiddenWords(const SourceFile& file, Report& report)
{
    constexpr std::array<std::string_view, 6> words = {
        "template", "try", "catch", "throw", "dynamic_cast", "typeid"
    };
    for (std::size_t i = 0; i < file.code.size(); ++i)
    {
        const int lineNo = static_cast<int>(i) + 1;
        for (std::string_view word : words)
        {
            if (ContainsWord(file.code[i], word))
            {
                report.Fail(file.path, lineNo, std::format("forbidden construct '{}'", word));
            }
        }
        if (ContainsWord(file.code[i], "import") && file.code[i].find("export import") != std::string::npos)
        {
            report.Fail(file.path, lineNo, "'export import' chains are banned");
        }
    }
}

void CheckSymbolicOperators(const SourceFile& file, Report& report)
{
    for (std::size_t i = 0; i < file.code.size(); ++i)
    {
        const std::string& line = file.code[i];
        std::size_t pos = line.find("operator");
        while (pos != std::string::npos)
        {
            const std::size_t after = line.find_first_not_of(' ', pos + 8);
            const bool isAssign = (after != std::string::npos) && line[after] == '='
                && !(after + 1 < line.size() && line[after + 1] == '=');
            const bool leftOk = (pos == 0) || !IsIdentChar(line[pos - 1]);
            const bool rightOk = (pos + 8 >= line.size()) || !IsIdentChar(line[pos + 8]);
            if (leftOk && rightOk && !isAssign)
            {
                report.Fail(file.path, static_cast<int>(i) + 1, "symbolic operator overloading (only operator= may be declared)");
            }
            pos = line.find("operator", pos + 8);
        }
    }
}

// Preprocessor use is limited to #include inside the global module fragment (before the module declaration).
void CheckPreprocessor(const SourceFile& file, Report& report)
{
    bool inPurview = false;
    for (std::size_t i = 0; i < file.code.size(); ++i)
    {
        const std::string line = Trim(file.code[i]);
        const int lineNo = static_cast<int>(i) + 1;
        if (line.starts_with("export module ") || (line.starts_with("module ") && line != "module;"))
        {
            inPurview = true;
        }
        if (!line.starts_with('#'))
        {
            continue;
        }
        const bool isInclude = line.starts_with("#include");
        if (!isInclude)
        {
            report.Fail(file.path, lineNo, "preprocessor directive other than #include (no #define / #if / #pragma)");
        }
        else if (inPurview)
        {
            report.Fail(file.path, lineNo, "#include after the module declaration (use import)");
        }
    }
}

// ---------------------------------------------------------------------------------------------
// .cppm: exported contract only
// ---------------------------------------------------------------------------------------------

bool IsAllowedBlockOpener(const std::string& statement)
{
    const std::string text = Trim(statement);
    return ContainsWord(text, "namespace") || ContainsWord(text, "class") || ContainsWord(text, "struct")
        || ContainsWord(text, "enum");
}

// Any '{' that does not open a namespace, class, struct or enum is a body or initializer.
void CheckCppmBodies(const SourceFile& file, Report& report)
{
    std::string statement;
    for (std::size_t i = 0; i < file.code.size(); ++i)
    {
        for (const char c : file.code[i])
        {
            if (c == '{')
            {
                if (!IsAllowedBlockOpener(statement))
                {
                    report.Fail(file.path, static_cast<int>(i) + 1, "function body or brace initializer in .cppm (declarations only)");
                }
                statement.clear();
            }
            else if (c == ';' || c == '}')
            {
                statement.clear();
            }
            else
            {
                statement.push_back(c);
            }
        }
        statement.push_back(' ');
    }
}

void CheckCppmWords(const SourceFile& file, Report& report)
{
    for (std::size_t i = 0; i < file.code.size(); ++i)
    {
        const std::string& line = file.code[i];
        const int lineNo = static_cast<int>(i) + 1;
        if (ContainsWord(line, "auto"))
        {
            report.Fail(file.path, lineNo, "'auto' in a public interface");
        }
        if (ContainsWord(line, "static"))
        {
            report.Fail(file.path, lineNo, "'static' helper or state in .cppm");
        }
        if (line.find("using namespace") != std::string::npos)
        {
            report.Fail(file.path, lineNo, "'using namespace' in .cppm");
        }
        if (ContainsWord(line, "inline") && !ContainsWord(line, "constexpr"))
        {
            report.Fail(file.path, lineNo, "'inline' function in .cppm (only 'inline constexpr' constants)");
        }
        if (line.find("= default") != std::string::npos && line.find("virtual ~I") == std::string::npos)
        {
            report.Fail(file.path, lineNo, "'= default' outside a pure virtual interface destructor (put it in the .cpp)");
        }
    }
}

std::string ModuleNameOf(const SourceFile& file, std::string_view prefix)
{
    for (const std::string& codeLine : file.code)
    {
        const std::string line = Trim(codeLine);
        if (line.starts_with(prefix))
        {
            const std::size_t semi = line.find(';');
            return Trim(line.substr(prefix.size(), semi - prefix.size()));
        }
    }
    return "";
}

void CheckCppm(const SourceFile& file, Report& report)
{
    if (file.raw.size() > MaxCppmLines)
    {
        report.Fail(file.path, static_cast<int>(file.raw.size()), std::format(".cppm exceeds {} lines", MaxCppmLines));
    }
    CheckCppmBodies(file, report);
    CheckCppmWords(file, report);

    const std::string moduleName = ModuleNameOf(file, "export module ");
    if (moduleName != file.path.stem().string())
    {
        report.Fail(file.path, 1, std::format("'export module {};' must match the file name", file.path.stem().string()));
    }
}

// A .cppm needs a paired .cpp only when it declares something that must be defined: a function or a
// class member function. Types, constants and `extern "C"` contract declarations (defined by the
// models that implement them) need no .cpp, so an empty .cpp is not created for them.
bool NeedsImplementation(const SourceFile& file)
{
    std::string statement;
    const auto needsDefinition = [&statement]() {
        const std::string text = Trim(statement);
        if (text.find('(') == std::string::npos)
        {
            return false;
        }
        constexpr std::array<std::string_view, 5> exempt = { "extern", "inline constexpr", "using", "virtual", "static_assert" };
        return std::ranges::none_of(exempt, [&text](std::string_view prefix) { return text.starts_with(prefix); });
    };
    for (const std::string& line : file.code)
    {
        for (const char c : line)
        {
            if (c == ';' || c == '{' || c == '}')
            {
                if (needsDefinition())
                {
                    return true;
                }
                statement.clear();
            }
            else
            {
                statement.push_back(c);
            }
        }
        statement.push_back(' ');
    }
    return false;
}

// True when the file only has the module declaration, imports and empty namespace braces.
bool IsEffectivelyEmpty(const SourceFile& file)
{
    for (const std::string& codeLine : file.code)
    {
        const std::string line = Trim(codeLine);
        const bool boilerplate = line.empty() || line == "module;" || line.starts_with("module ") || line.starts_with("import ")
            || line == "{" || line == "}" || (line.starts_with("namespace ") && line.ends_with("{"));
        if (!boilerplate)
        {
            return false;
        }
    }
    return true;
}

void CheckCpp(const SourceFile& file, Report& report)
{
    if (IsEffectivelyEmpty(file))
    {
        report.Fail(file.path, 1, "empty .cpp: delete it (a declarations-only .cppm needs no .cpp)");
    }
    for (std::size_t i = 0; i < file.code.size(); ++i)
    {
        if (Trim(file.code[i]).starts_with("export "))
        {
            report.Fail(file.path, static_cast<int>(i) + 1, "'export' in a .cpp (exports belong in the .cppm)");
        }
    }
    const std::string moduleName = ModuleNameOf(file, "module ");
    if (moduleName != file.path.stem().string())
    {
        report.Fail(file.path, 1, std::format("'module {};' must match the file name", file.path.stem().string()));
    }
}

// ---------------------------------------------------------------------------------------------
// Repository-wide rules
// ---------------------------------------------------------------------------------------------

bool IsHeaderExtension(const std::string& extension)
{
    return extension == ".h" || extension == ".hpp" || extension == ".hxx" || extension == ".inl" || extension == ".inc";
}

std::vector<fs::path> ListFiles(const fs::path& root)
{
    std::vector<fs::path> files;
    std::error_code ec;
    fs::recursive_directory_iterator it(root, ec);
    const fs::recursive_directory_iterator end;
    while (!ec && it != end)
    {
        if (it->is_regular_file(ec))
        {
            files.push_back(it->path());
        }
        it.increment(ec);
    }
    std::ranges::sort(files);
    return files;
}

void CheckFile(const fs::path& path, Report& report)
{
    const std::string extension = path.extension().string();
    if (IsHeaderExtension(extension))
    {
        report.Fail(path, 1, "header files are forbidden (use .cppm + .cpp)");
        return;
    }
    if (extension != ".cppm" && extension != ".cpp")
    {
        return;
    }
    SourceFile file;
    if (!LoadSource(path, file))
    {
        report.Fail(path, 1, "cannot read file");
        return;
    }
    CheckForbiddenWords(file, report);
    CheckSymbolicOperators(file, report);
    CheckPreprocessor(file, report);
    if (extension == ".cppm")
    {
        CheckCppm(file, report);
        fs::path partner = path;
        partner.replace_extension(".cpp");
        std::error_code ec;
        if (NeedsImplementation(file) && !fs::exists(partner, ec))
        {
            report.Fail(path, 1, std::format("declares functions but has no paired implementation file {}", partner.filename().string()));
        }
    }
    else
    {
        CheckCpp(file, report);
        fs::path partner = path;
        partner.replace_extension(".cppm");
        std::error_code ec;
        if (!fs::exists(partner, ec) && path.stem() != "main")
        {
            report.Fail(path, 1, std::format("missing paired interface file {}", partner.filename().string()));
        }
    }
}

} // namespace

int main(int argc, char** argv)
{
    const fs::path root = (argc > 1) ? fs::path(argv[1]) : fs::path("src");
    std::error_code ec;
    if (!fs::is_directory(root, ec))
    {
        std::println(std::cerr, "check_rules: '{}' is not a directory (run from the repository root)", root.generic_string());
        return 2;
    }

    Report report;
    const std::vector<fs::path> files = ListFiles(root);
    for (const fs::path& path : files)
    {
        CheckFile(path, report);
    }

    if (report.violations != 0)
    {
        std::println(std::cerr, "check_rules: {} violation(s) in {} file(s)", report.violations, files.size());
        return 1;
    }
    std::println("check_rules: OK ({} files)", files.size());
    return 0;
}
