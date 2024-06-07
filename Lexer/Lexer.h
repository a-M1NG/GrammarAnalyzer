#ifndef LEXER_H
#define LEXER_H

#include "includes.h"
#include "Token.h"
#include <string>
using namespace std;

class TokenSequence
{
public:
    // TokenSequence();
    void scan(string filepath);
    void printAll();
    void printToken();
    void initKeyWord();
    void initDelimiters();
    int get_kind(char ch);
    void test();

    TokenList getToken_list(string filepath)
    {
        initKeyWord();
        initDelimiters();

        scan(filepath);
        return Token_list;
    }

private:
    unordered_map<string, int> KeyWord;    // 关键字表，K
    unordered_map<string, int> Punctuator; // 界符表,P
    unordered_map<string, int> Constant;   // 常数表,包括整数与小数 ,Con
    unordered_map<string, int> Identifier; // 标识符序列,I
    unordered_map<string, int> Character;  // 字符常数,Ch
    unordered_map<string, int> String;     // 字符串常数,St
    unordered_map<string, int> Array;      // 数组表 Arr
    unordered_map<string, int> Error;      // 错误项
    TokenList Token_list;                  // token序列表
    int IdKey = 1;
    int IdPunctuator = 1;
    int IdConstant = 1;
    int IdToken = 1;
    int IdIdentifier = 1;
    int IdCharacter = 1;
    int IdString = 1;
    int IdArray = 1;
    int IdError = 1;
};

void TokenSequence::scan(std::string filepath)
{
    ifstream infile;
    infile.open(filepath, ios::in);
    if (!infile.is_open())
    {
        cout << "Cin文件读取失败" << "\n";
        return;
    }
    // test();
    string buf;
    while (getline(infile, buf)) // 1为数字，2为字母，3为运算符，4为分隔符，5为其他
    {
        for (int i = 0; i < buf.size(); i++)
        {
            if (buf[i] == ' ')
                continue;
            else if (get_kind(buf[i]) == 2 || buf[i] == '_')
            { // 首字符为字母，为标识符或者关键字
                string tmp;
                bool is_Identifier = false;
                bool is_Aarry = false;                             // 判断是否为标识符，是否为数组
                int leftNum = 0, rightNum = 0, lPos = 0, rPos = 0; // 记录[,]的数量，和存储循环起始位置
                while (get_kind(buf[i]) == 2 || get_kind(buf[i]) == 1 || buf[i] == '_' || buf[i] == '[' || buf[i] == ']')
                {
                    if (buf[i] == '[')
                        leftNum++, lPos = i;
                    if (buf[i] == ']')
                        rightNum++, rPos = i;
                    if (get_kind(buf[i]) == 1)
                        is_Identifier = true; // 若有数字出现
                    tmp += buf[i];
                    i++;
                    if (i >= buf.size())
                        break;
                }
                i--;
                if (leftNum || rightNum)
                {                          // 如果有[,或者]出现，则为数组
                    bool is_Error = false; // 判断是否为错误数组
                    string tmpNum;
                    for (int j = lPos + 1; j < rPos; j++)
                    {
                        tmpNum += buf[j]; // 这里存[]之间的字符，如果是一个存数字的标识符也是对的，后面可用于判断
                        if (get_kind(buf[j]) != 1)
                        { //[和]之间的字符只能为数字
                            is_Error = true;
                        }
                    }

                    if (leftNum != 1 || rightNum != 1)
                        is_Error = true; //[和]都只能出现一次
                    if (is_Error)
                    {
                        if (Error.count(tmp) == 0)
                        { // 如果Error表已经包含
                            Error[tmp] = IdError++;
                        }
                        Token_list.push_back({"Error", tmp, Error[tmp]}); // 放进Token表
                    }
                    else
                    {
                        if (Array.count(tmp) == 0)
                        { // 没错误的话放进Array表
                            Array[tmp] = IdArray++;
                        }
                        Token_list.push_back({"Arr", tmp, Array[tmp]});
                    }
                }
                else if (is_Identifier)
                { // 不是数组，并且有数字，那就是标识符
                    if (Identifier.count(tmp) == 0)
                    {
                        Identifier[tmp] = IdIdentifier++;
                    }
                    Token_list.push_back({"I", tmp, Identifier[tmp]});
                }
                else
                {
                    if (KeyWord.count(tmp))
                    {
                        Token_list.push_back({"K", tmp, KeyWord[tmp]});
                    }
                    else
                    { // 关键字表找不到那就是标识符
                        if (Identifier.count(tmp) == 0)
                        {
                            Identifier[tmp] = IdIdentifier++;
                        }
                        Token_list.push_back({"I", tmp, Identifier[tmp]});
                    }
                }
            }

            else if (get_kind(buf[i]) == 1)
            {                // 处理常数
                int num = 0; // 小数点的数量
                bool is_Error = false;
                string tmp;
                // 当没遇到;或者运算符
                while (buf[i] != ';' && get_kind(buf[i]) != 3)
                { // int a=123a; Error
                    if (buf[i] == '.')
                        num++;
                    if (get_kind(buf[i]) != 1 && buf[i] != '.')
                        is_Error = true;
                    tmp += buf[i];
                    i++;
                    if (i >= buf.size())
                        break;
                }
                i--;
                if (num > 1)
                    is_Error = true;
                if (is_Error)
                {
                    if (Error.count(tmp) == 0)
                    {
                        Error[tmp] = IdError++;
                    }
                    Token_list.push_back({"Error", tmp, Error[tmp]});
                }
                else
                {
                    if (Constant.count(tmp) == 0)
                    {
                        Constant[tmp] = IdConstant++;
                    }
                    Token_list.push_back({"Con", tmp, Constant[tmp]});
                }
            }

            else if (buf[i] == '\'')
            {                // 处理字符常数
                int num = 0; // 存'的数量
                string tmp;
                while (buf[i] != ';')
                {
                    if (buf[i] == '\'')
                        num++;
                    tmp += buf[i];
                    i++;
                    if (i >= buf.size())
                        break;
                }
                i--;
                bool is_Error = false;
                if (num != 2)
                    is_Error = true;
                if ((int)tmp.size() - 2 != 1)
                    is_Error = true;
                if (is_Error)
                {
                    if (Error.count(tmp) == 0)
                    {
                        Error[tmp] = IdError++;
                    }
                    Token_list.push_back({"Error", tmp, Error[tmp]});
                }
                else
                {
                    if (Character.count(tmp) == 0)
                    {
                        Character[tmp] = IdCharacter++;
                    }
                    Token_list.push_back({"Ch", tmp, Character[tmp]});
                }
            }

            else if (buf[i] == '"')
            {                // string a=  "wag  a aw&&91 12"a  ;
                int num = 0; // 存"的数量
                int Pos = 0; // 存最后一个"出现的位置，应对这种情况 string a="abcd"ab; 这种也判Error
                string tmp;
                while (buf[i] != ';')
                {
                    if (buf[i] == '"')
                        num++, Pos = i;
                    tmp += buf[i];
                    i++;
                    if (i >= buf.size())
                        break;
                }
                i--;
                bool is_Error = false;
                if (num != 2)
                    is_Error = true;
                for (int j = Pos + 1; j <= i; j++)
                {
                    if (buf[j] != ' ')
                        is_Error = true;
                }
                if (is_Error)
                {
                    if (Error.count(tmp) == 0)
                    {
                        Error[tmp] = IdError++;
                    }
                    Token_list.push_back({"Error", tmp, Error[tmp]});
                }
                else
                {
                    if (String.count(tmp) == 0)
                    {
                        String[tmp] = IdString++;
                    }
                    Token_list.push_back({"St", tmp, String[tmp]});
                }
            }

            else if (get_kind(buf[i]) == 3 || get_kind(buf[i]) == 4)
            { // 处理普通界符
                string tmp;
                tmp += buf[i];
                if (buf[i] == '&' && buf[i + 1] == '&' || buf[i] == '|' && buf[i + 1] == '|' || buf[i] == '%' && buf[i + 1] == '=' || buf[i] == '>' && buf[i + 1] == '=' || buf[i] == '<' && buf[i + 1] == '=' || buf[i] == '=' && buf[i + 1] == '=' || buf[i] == '+' && buf[i + 1] == '+' || buf[i] == '-' && buf[i + 1] == '-' || buf[i] == '>' && buf[i + 1] == '>' || buf[i] == '<' && buf[i + 1] == '<')
                {
                    tmp += buf[i + 1];
                    i++;
                }
                Token_list.push_back({"P", tmp, Punctuator[tmp]});
            }
            else
            {
                string tmp;
                tmp += buf[i];
                if (Error.count(tmp) == 0)
                {
                    Error[tmp] = IdError++;
                }
                Token_list.push_back({"Error", tmp, Error[tmp]});
            }
        }
    }
}

void TokenSequence::printAll()
{

    cout << setw(58) << "KeyWord\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : KeyWord)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "Delimiters\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : Punctuator)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "Constant\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : Constant)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "Identifier\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : Identifier)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "Character\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : Character)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "String\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : String)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "Array\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : Array)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";

    cout << setw(58) << "Error\n";
    cout << setw(35) << "value" << setw(36) << "id\n\n";
    for (auto i : Error)
        cout << setw(35) << i.first << setw(35) << i.second << "\n";
}

void TokenSequence::printToken()
{
}

void TokenSequence::initKeyWord()
{
    ifstream infile;
    std::string path_for_Linux = "../Lexer/KeyWord.txt";
    std::string path_for_Windows = "./Lexer/KeyWord.txt";
    infile.open(path_for_Linux, ios::in);
    if (!infile.is_open())
    {
        cout << "KeyWord文件读取失败" << "\n";
        return;
    }
    string buf;
    while (getline(infile, buf))
    {
        KeyWord[buf] = IdKey++;
    }
}

void TokenSequence::initDelimiters()
{
    ifstream infile;
    std::string path_for_Linux = "../Lexer/Delimiters.txt";
    std::string path_for_Windows = "./Lexer/Delimiters.txt";
    infile.open(path_for_Linux, ios::in);
    if (!infile.is_open())
    {
        cout << "Delimiters文件读取失败" << "\n";
        return;
    }
    string buf;
    while (getline(infile, buf))
    {
        Punctuator[buf] = IdPunctuator++;
    }
}

int TokenSequence::get_kind(char ch)
{
    if (ch >= '0' && ch <= '9')
        return 1; // 整数
    if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
        return 2; // 字母
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '|' || ch == '^' || ch == '&' || ch == '>' || ch == '<' || ch == '=')
        return 3; // 运算符
    else if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == ';' || ch == '\'' || ch == '.')
        return 4; // 分隔符
    else
        return 5; // 其他符号
}

void TokenSequence::test()
{
    cout << "进入test\n";
}

// int main()
// {
//     string s, tmp;

//     TokenSequence CinToken;

//     CinToken.initKeyWord();
//     CinToken.initDelimiters();
//     CinToken.scan();
//     CinToken.printAll();

//     vector<Token> Token_list = CinToken.getToken_list();

//     cout << setw(58) << "Token_list:\n";
//     cout << setw(25) << "type" << setw(25) << "value" << setw(25) << "id" << "\n\n";
//     for (auto i : Token_list)
//         cout << setw(25) << i.type << setw(25) << i.value << setw(25) << i.id << " \n";

//     return 0;
// }

// 一些蛋疼的输入
// int ab=1;  int a b =1;
// string a="  abwa "    ; string b="aw"abcd;
#endif