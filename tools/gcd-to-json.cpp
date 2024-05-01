#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

namespace tool
{

inline bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.length() >= suffix.length() && text.substr(text.length()-suffix.length()) == suffix;
}

void convertIssues(const fs::path & path)
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert issues at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    std::ofstream json(outPath);
    int recordCount{};
    std::string line;
    while (tsv)
    {
        if (!std::getline(tsv, line))
        {
            break;
        }
        ++recordCount;
    }
    std::cout << recordCount << " records processed.\n";
}

void convertSequences(const fs::path & path)
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert sequences at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    std::ofstream json(outPath);
    int recordCount{};
    std::string line;
    while (tsv)
    {
        if (!std::getline(tsv, line))
        {
            break;
        }
        ++recordCount;
    }
    std::cout << recordCount << " records processed.\n";
}

void gcdToJSON(const std::string &dataDir)
{
    for ( const fs::directory_entry &entry : fs::directory_iterator(dataDir))
    {
        const fs::path &path = entry.path();
        if (!(entry.is_regular_file() && path.extension().string() == ".tsv"))
        {
            continue;
        }

        if (endsWith(path.stem().string(), "issues"))
        {
            convertIssues(path);
        }
        else if (endsWith(path.stem().string(), "sequences"))
        {
            convertSequences(path);
        }
    }
}

} // namespace tool

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: gcd-to-json <datadir>\n";
        return 1;
    }

    const std::string dataDir{argv[1]};
    tool::gcdToJSON(dataDir);
}
