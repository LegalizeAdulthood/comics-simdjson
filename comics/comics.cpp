#include <simdjson.h>

#include "comics.h"

namespace comics
{

inline bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.length() >= suffix.length() && text.substr(text.length() - suffix.length()) == suffix;
}

namespace
{
class JSONDatabase : public Database
{
public:
    JSONDatabase(const std::filesystem::path &jsonDir);
    void printScriptSequences(std::ostream &str, const std::string &name) override;
    void printPencilSequences(std::ostream &str, const std::string &name) override;
    void printInkSequences(std::ostream &str, const std::string &name) override;
    void printColorSequences(std::ostream &str, const std::string &name) override;

private:
    simdjson::dom::element m_issues;
    simdjson::dom::element m_sequences;
};

JSONDatabase::JSONDatabase(const std::filesystem::path &jsonDir)
{
    simdjson::dom::parser parser;
    bool foundIssues{false};
    bool foundSequences{false};
    for (const auto &entry : std::filesystem::directory_iterator(jsonDir))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        const std::filesystem::path &path{entry.path()};
        const std::string filename{path.filename().string()};
        if (endsWith(filename, "issues.json"))
        {
            std::cout << "Reading issues...\n";
            m_issues = parser.load(path.string());
            foundIssues = true;
            std::cout << "done.\n";
        }
        else if (endsWith(filename, "sequences.json"))
        {
            std::cout << "Reading sequences...\n";
            m_sequences = parser.load(path.string());
            foundSequences = true;
            std::cout << "done.\n";
        }
    }
    if (!(foundIssues && foundSequences))
    {
        if (foundIssues)
        {
            throw std::runtime_error("Couldn't find sequences JSON file in " + jsonDir.string());
        }
        if (foundSequences)
        {
            throw std::runtime_error("Couldn't find issues JSON file in " + jsonDir.string());
        }
        throw std::runtime_error("Couldn't find either issues or sequences JSON file in " + jsonDir.string());
    }
}

void JSONDatabase::printScriptSequences(std::ostream &str, const std::string &name)
{
}

void JSONDatabase::printPencilSequences(std::ostream &str, const std::string &name)
{
}

void JSONDatabase::printInkSequences(std::ostream &str, const std::string &name)
{
}

void JSONDatabase::printColorSequences(std::ostream &str, const std::string &name)
{
}

} // namespace

std::shared_ptr<Database> createDatabase(const std::filesystem::path &jsonDir)
{
    return std::make_shared<JSONDatabase>(jsonDir);
}
} // namespace comics
