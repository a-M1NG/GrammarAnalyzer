#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <stdexcept>

using namespace std;

// 类型定义嵌套的 unordered_map 结构
using AnalysisTable = unordered_map<string, unordered_map<string, int>>;

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

    int parseInt(const string &json, size_t &pos)
    {
        string result;
        while (pos < json.size() && (isdigit(json[pos]) || json[pos] == '-'))
        {
            result += json[pos++];
        }
        return stoi(result);
    }

    void skipWhitespace(const string &json, size_t &pos)
    {
        while (pos < json.size() && isspace(json[pos]))
        {
            ++pos;
        }
    }

    unordered_map<string, int> parseInnerMap(const string &json, size_t &pos)
    {
        unordered_map<string, int> innerMap;
        ++pos; // skip the opening brace
        skipWhitespace(json, pos);
        while (pos < json.size() && json[pos] != '}')
        {
            skipWhitespace(json, pos);
            string key = parseString(json, pos);
            skipWhitespace(json, pos);
            if (json[pos] != ':')
            {
                throw runtime_error("Expected ':' after key");
            }
            ++pos; // skip the colon
            skipWhitespace(json, pos);
            int value = parseInt(json, pos);
            skipWhitespace(json, pos);
            innerMap[key] = value;
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
                throw runtime_error("Expected ':' after key");
            }
            ++pos; // skip the colon
            skipWhitespace(json, pos);
            if (json[pos] != '{')
            {
                throw runtime_error("Expected '{' for nested map");
            }
            unordered_map<string, int> innerMap = parseInnerMap(json, pos);
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
// int main()
// {
//     try
//     {
//         string jsonContent = readFile("F:\\BaiduNetdiskDownload\\CompilerProject-master\\CompilerProject-master\\grammar_static\\AnalysisTable_2.json");
//         size_t pos = 0;
//         AnalysisTable analysis_table = parseAnalysisTable(jsonContent, pos);
//         fstream file("F:\\BaiduNetdiskDownload\\CompilerProject-master\\CompilerProject-master\\grammar_static\\AnalysisTable_2.txt");
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
//                 file << setw(10) << it2.second << ' ';
//             }
//             // cout << endl;
//             file << endl;
//         }
//     }
//     catch (const exception &ex)
//     {
//         cerr << "Error: " << ex.what() << endl;
//     }

//     return 0;
// }
