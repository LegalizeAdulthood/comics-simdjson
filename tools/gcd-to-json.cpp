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
    return text.length() >= suffix.length() && text.substr(text.length() - suffix.length()) == suffix;
}

inline std::string quoted(std::string text)
{
    // replace "" with \"
    for (std::string::size_type pos = text.find("\"\""); pos != std::string::npos; pos = text.find("\"\"", pos + 2))
    {
        text[pos] = '\\';
    }
    return text;
}

void split(std::vector<std::string> &fields, const std::string &text)
{
    fields.clear();
    std::size_t start{1};
    for (std::size_t split = text.find("\"\t\""); split != std::string::npos; split = text.find("\"\t\"", split + 1))
    {
        fields.emplace_back(text.substr(start, split - start));
        start = split + 3;
    }
    fields.emplace_back(text.substr(start, text.length() - start - 1));
}

void convertIssues(const fs::path &path)
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert issues at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    std::ofstream json(outPath);
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
        if (recordCount % 1000 == 0)
        {
            std::cout << recordCount << " records...\r";
        }

        std::vector<std::string> fields;
        split(fields, line);
        if (fields.size() != 3)
        {
            throw std::runtime_error("Expected 3 fields, got " + std::to_string(fields.size()));
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
    std::cout << '\n' << recordCount << " records processed.\n";
}

void convertSequences(const fs::path &path)
{
    fs::path outPath{fs::path(path).replace_extension(".json")};
    std::cout << "Convert sequences at " << path.string() << " to " << outPath.string() << '\n';
    std::ifstream tsv(path);
    std::ofstream json(outPath);
    int recordCount{};
    std::string line;
    std::map<std::string, std::string> record;
    int lastRecordId{-1};
    int lastSequenceId{-1};
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
        if (recordCount % 1000 == 0)
        {
            std::cout << recordCount << " records...\r";
        }

        std::vector<std::string> fields;
        split(fields, line);
        if (fields.size() != 4)
        {
            throw std::runtime_error("Expected 4 fields, got " + std::to_string(fields.size()));
        }
        const int recordId{std::stoi(fields[0])};
        const int sequenceId{std::stoi(fields[1])};
        if (recordId != lastRecordId || sequenceId != lastSequenceId)
        {
            printRecord();
            record.clear();
            record["issue"] = fields[0];
            lastRecordId = recordId;
            lastSequenceId = sequenceId;
        }
        record[fields[2]] = fields[3];
    }
    printRecord();
    json << "\n";
    std::cout << '\n' << recordCount << " records processed.\n";
}

void gcdToJSON(const std::string &dataDir)
{
    for (const fs::directory_entry &entry : fs::directory_iterator(dataDir))
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
    try
    {
        tool::gcdToJSON(dataDir);
    }
    catch (const std::exception &bang)
    {
        std::cerr << "\nUnexpected exception: " << bang.what() << '\n';
        return 1;
    }
    catch (...)
    {
        std::cerr << "\nUnexpected exception\n";
        return 2;
    }
    return 0;
}
