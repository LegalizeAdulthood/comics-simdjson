#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace tool
{

inline bool endsWith(const std::string &text, const std::string &suffix)
{
    return text.length() >= suffix.length() && text.substr(text.length()-suffix.length()) == suffix;
}

inline std::string quoted(std::string text)
{
    for (std::string::size_type pos = text.find('"'); pos != std::string::npos; pos = text.find('"', pos + 2))
    {
        text.insert(pos, 1, '\\');
    }
    return text;
}

void convertIssues(const fs::path & path)
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert issues at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    //std::ofstream json(outPath);
    std::ostream &json{std::cout};
    int recordCount{};
    std::string line;
    std::map<std::string, std::string> record;
    int lastRecordId{-1};
    bool firstRecord{true};
    auto printRecord = [&]
    {
        if (record.empty())
            return;
        if (!firstRecord)
            json << ",\n";
        json << "{";
        bool first{true};
        for (const auto &pair : record)
        {
            if (!first)
            {
                json << ",";
            }
            json << "\n    \"" << quoted(pair.first) << "\": ";
            if (pair.second == "True" || pair.second == "False")
            {
                json << (pair.second == "True" ? "true" : "false");
            }
            else
            {
                json << "\"" << quoted(pair.second) << "\"";
            }
            first = false;
        }
        json << "\n}";
        firstRecord = false;
    };
    while (tsv)
    {
        if (!std::getline(tsv, line))
        {
            break;
        }
        ++recordCount;

        std::vector<std::string> fields;
        split(fields, line, boost::algorithm::is_any_of("\t"));
        if (fields.size() != 3)
        {
            throw std::runtime_error("Expected 3 fields, got " + std::to_string(fields.size()));
        }
        for (std::string &field : fields)
        {
            field = field.substr(1, field.length() - 2);
        }
        const int recordId{std::stoi(fields[0])};
        if (recordId != lastRecordId)
        {
            if (!record.empty())
            {
                printRecord();
            }
            record.clear();
            record["id"] = fields[0];
            lastRecordId = recordId;
        }
        record[fields[1]] = fields[2];
    }
    printRecord();
    json << "\n";
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
