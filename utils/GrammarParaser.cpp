#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <vector>

using namespace std;

// 类型定义嵌套的 unordered_map 结构
using AnalysisTable = unordered_map<string, unordered_map<string, vector<string>>>;

class decodeJson
{
public:
    AnalysisTable getAnal(const std::string &filename)
    {
        string jsonContent = readFile(filename);
        size_t pos = 0;
        return parseAnalysisTable(jsonContent, pos);
    }

private:
    string readFile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            throw runtime_error("Error opening file");
        }
        stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    string parseString(const string &json, size_t &pos)
    {
        string result;
        ++pos; // skip the opening quote
        while (pos < json.size() && json[pos] != '"')
        {
            if (json[pos] == '\\')
            {
                ++pos; // skip the escape character
            }
            result += json[pos++];
        }
        ++pos; // skip the closing quote
        return result;
    }

    string parseInt(const string &json, size_t &pos)
    {
        string result;
        while (pos < json.size())
        {
            result += json[pos++];
        }
        return result;
    }

    void skipWhitespace(const string &json, size_t &pos)
    {
        while (pos < json.size() && isspace(json[pos]))
        {
            ++pos;
        }
    }

    std::vector<std::string> split(const std::string &str)
    {
        std::istringstream iss(str);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token)
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    unordered_map<string, vector<string>> parseInnerMap(const string &json, size_t &pos)
    {
        unordered_map<string, vector<string>> innerMap;
        ++pos; // skip the opening brace
        skipWhitespace(json, pos);
        while (pos < json.size() && json[pos] != '}')
        {
            skipWhitespace(json, pos);
            string key = parseString(json, pos);
            skipWhitespace(json, pos);
            if (json[pos] != ':')
            {
                throw runtime_error("Expected ':' after key inner");
            }
            ++pos; // skip the colon
            skipWhitespace(json, pos);
            string value = parseInt(json, pos);
            skipWhitespace(json, pos);
            innerMap[key] = split(value);
            if (json[pos] == ',')
            {
                ++pos; // skip the comma
                skipWhitespace(json, pos);
            }
        }
        ++pos; // skip the closing brace
        return innerMap;
    }

    AnalysisTable parseAnalysisTable(const string &json, size_t &pos)
    {
        AnalysisTable table;
        ++pos; // skip the opening brace
        skipWhitespace(json, pos);
        while (pos < json.size() && json[pos] != '}')
        {
            skipWhitespace(json, pos);
            string key = parseString(json, pos);
            skipWhitespace(json, pos);
            if (json[pos] != ':')
            {
                throw runtime_error("Expected ':' after key outer");
            }
            ++pos; // skip the colon
            skipWhitespace(json, pos);
            if (json[pos] != '{')
            {
                throw runtime_error("Expected '{' for nested map");
            }
            unordered_map<string, vector<string>> innerMap = parseInnerMap(json, pos);
            table[key] = innerMap;
            skipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ',')
            {
                ++pos; // skip the comma
                skipWhitespace(json, pos);
            }
        }
        ++pos; // skip the closing brace
        return table;
    }
};

class JSONParser {
public:
    AnalysisTable parse(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Failed to open file: " + filename);
        }
        
        string json((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        size_t pos = 0;
        return parseObject(json, pos);
    }

    void print(const AnalysisTable& table) {
        for (const auto& nonTerminal : table) {
            cout << nonTerminal.first << ":\n";
            for (const auto& terminal : nonTerminal.second) {
                cout << "  " << terminal.first << " -> ";
                for (const auto& symbol : terminal.second) {
                    cout << symbol << " ";
                }
                cout << "\n";
            }
        }
    }

private:
    string parseString(const string& json, size_t& pos) {
        string result;
        while (pos < json.size() && json[pos] != '\"') pos++;
        pos++; // skip the opening quote
        while (pos < json.size() && json[pos] != '\"') {
            if (json[pos] == '\\') {
                pos++;
                if (pos < json.size() && json[pos] == '\"') {
                    result += '\"';
                }
            } else {
                result += json[pos];
            }
            pos++;
        }
        pos++; // skip the closing quote
        return result;
    }

    AnalysisTable parseObject(const string& json, size_t& pos) {
        AnalysisTable table;
        while (pos < json.size() && json[pos] != '{') pos++;
        pos++; // skip the opening brace

        while (pos < json.size() && json[pos] != '}') {
            string key = parseString(json, pos);
            while (pos < json.size() && json[pos] != ':') pos++;
            pos++; // skip the colon
            table[key] = parseInnerObject(json, pos);
            while (pos < json.size() && json[pos] != ',' && json[pos] != '}') pos++;
            if (json[pos] == ',') pos++; // skip the comma
        }
        pos++; // skip the closing brace
        return table;
    }

    unordered_map<string, vector<string>> parseInnerObject(const string& json, size_t& pos) {
        unordered_map<string, vector<string>> innerMap;
        while (pos < json.size() && json[pos] != '{') pos++;
        pos++; // skip the opening brace

        while (pos < json.size() && json[pos] != '}') {
            string key = parseString(json, pos);
            while (pos < json.size() && json[pos] != ':') pos++;
            pos++; // skip the colon
            vector<string> production = parseArray(json, pos);
            innerMap[key] = production;
            while (pos < json.size() && json[pos] != ',' && json[pos] != '}') pos++;
            if (json[pos] == ',') pos++; // skip the comma
        }
        pos++; // skip the closing brace
        return innerMap;
    }

    vector<string> parseArray(const string& json, size_t& pos) {
        vector<string> array;
        string value;
        while (pos < json.size() && json[pos] != '\"') pos++;
        pos++; // skip the opening quote
        while (pos < json.size() && json[pos] != '\"') {
            if (json[pos] == '\\') {
                pos++;
                if (pos < json.size() && json[pos] == '\"') {
                    value += '\"';
                }
            } else {
                value += json[pos];
            }
            pos++;
        }
        pos++; // skip the closing quote
        stringstream ss(value);
        string token;
        while (ss >> token) {
            array.push_back(token);
        }
        return array;
    }
};

// int main()
// {
//     try
//     {
//         decodeJson decoder;
//         auto analysis_table = decoder.getAnal("utils\\AnalysisTable.json");

//         fstream file("D:\\Programming\\GrammarAnalyzer\\utils\\AnalysisTable.txt");
//         // 示例输出解析后的数据
//         for (const auto &it : analysis_table)
//         {
//             file << setw(20) << "Non-terminal:";
//             // cout << "Non-terminal: " << it.first << endl;
//             for (const auto &it2 : it.second)
//             {
//                 // cout << it2.first << ' ';
//                 file << setw(10) << it2.first << ' ';
//             }
//             file << endl;
//             break;
//         }
//         for (const auto &it : analysis_table)
//         {
//             // cout << it.first << ' ';
//             file << setw(20) << it.first << ' ';
//             for (const auto &it2 : it.second)
//             {
//                 // cout << it2.second << ' ';
//                 for (const auto &it3 : it2.second)
//                 {
//                     // cout << it3 << ' ';
//                     file << setw(10) << it3 << ' ';
//                 }
//             }
//             // cout << endl;
//             file << endl;
//         }
//     }
//     catch (const exception &ex)
//     {
//         cerr << "Error: " << ex.what() << endl;
//     }
//     system("pause");
//     return 0;
// }
