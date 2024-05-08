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

void printSequence(std::ostream &str, const simdjson::simdjson_result<simdjson::dom::object> &sequence)
{
    for (const simdjson::dom::key_value_pair field : sequence)
    {
        str << field.key << ": " ;
        if (field.value.is_string())
            str << field.value.get_string().value() << '\n';
        else if (field.value.is_bool())
            str << (field.value.get_bool().value() ? "true\n" : "false\n");
        else if (field.value.is_number())
            str << field.value.get_int64() << '\n';
        else
        {
            throw std::runtime_error("Unknown type for field '" + std::string{field.key} + "'");
        }
    }
}

class JSONDatabase : public Database
{
public:
    JSONDatabase(const std::filesystem::path &jsonDir);
    void printScriptSequences(std::ostream &str, const std::string &name) override;
    void printPencilSequences(std::ostream &str, const std::string &name) override;
    void printInkSequences(std::ostream &str, const std::string &name) override;
    void printColorSequences(std::ostream &str, const std::string &name) override;

private:
    simdjson::dom::parser parser;
    simdjson::simdjson_result<simdjson::dom::element> m_issues;
    simdjson::simdjson_result<simdjson::dom::element> m_sequences;
};

JSONDatabase::JSONDatabase(const std::filesystem::path &jsonDir)
{
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
            if (!m_issues.is_array())
            {
                throw std::runtime_error("JSON issues file should be an array of objects");
            }
        }
        else if (endsWith(filename, "sequences.json"))
        {
            std::cout << "Reading sequences...\n";
            m_sequences = parser.load(path.string());
            foundSequences = true;
            std::cout << "done.\n";
            if (!m_sequences.is_array())
            {
                throw std::runtime_error("JSON sequences file should be an array of objects");
            }
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
    bool first{true};
    for (simdjson::dom::element &record : m_sequences.get_array())
    {
        if (!record.is_object())
        {
            throw std::runtime_error("Sequence array element should be an object");
        }

        for (const simdjson::dom::key_value_pair &field : record.get_object())
        {
            if (field.key == "script")
            {
                const simdjson::dom::element &value = field.value;
                if (!value.is_string())
                {
                    throw std::runtime_error("Value of script field should be a string");
                }
                if (value.get_string().value().find(name) != std::string::npos)
                {
                    if (!first)
                    {
                        str << '\n';
                    }
                    printSequence(str, record.get_object());
                    first = false;
                }
                break;
            }
        }
    }
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
