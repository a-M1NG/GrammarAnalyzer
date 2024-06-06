#include "includes.h"
#include "Token.h"
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
    unordered_map<string, int> KeyWord;    // �ؼ��ֱ�K
    unordered_map<string, int> Punctuator; // �����,P
    unordered_map<string, int> Constant;   // ������,����������С�� ,Con
    unordered_map<string, int> Identifier; // ��ʶ������,I
    unordered_map<string, int> Character;  // �ַ�����,Ch
    unordered_map<string, int> String;     // �ַ�������,St
    unordered_map<string, int> Array;      // ����� Arr
    unordered_map<string, int> Error;      // ������
    TokenList Token_list;                  // token���б�
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
        cout << "Cin�ļ���ȡʧ��" << "\n";
        return;
    }
    // test();
    string buf;
    while (getline(infile, buf)) // 1Ϊ���֣�2Ϊ��ĸ��3Ϊ�������4Ϊ�ָ�����5Ϊ����
    {
        for (int i = 0; i < buf.size(); i++)
        {
            if (buf[i] == ' ')
                continue;
            else if (get_kind(buf[i]) == 2 || buf[i] == '_')
            { // ���ַ�Ϊ��ĸ��Ϊ��ʶ�����߹ؼ���
                string tmp;
                bool is_Identifier = false;
                bool is_Aarry = false;                             // �ж��Ƿ�Ϊ��ʶ�����Ƿ�Ϊ����
                int leftNum = 0, rightNum = 0, lPos = 0, rPos = 0; // ��¼[,]���������ʹ洢ѭ����ʼλ��
                while (get_kind(buf[i]) == 2 || get_kind(buf[i]) == 1 || buf[i] == '_' || buf[i] == '[' || buf[i] == ']')
                {
                    if (buf[i] == '[')
                        leftNum++, lPos = i;
                    if (buf[i] == ']')
                        rightNum++, rPos = i;
                    if (get_kind(buf[i]) == 1)
                        is_Identifier = true; // �������ֳ���
                    tmp += buf[i];
                    i++;
                    if (i >= buf.size())
                        break;
                }
                i--;
                if (leftNum || rightNum)
                {                          // �����[,����]���֣���Ϊ����
                    bool is_Error = false; // �ж��Ƿ�Ϊ��������
                    string tmpNum;
                    for (int j = lPos + 1; j < rPos; j++)
                    {
                        tmpNum += buf[j]; // �����[]֮����ַ��������һ�������ֵı�ʶ��Ҳ�ǶԵģ�����������ж�
                        if (get_kind(buf[j]) != 1)
                        { //[��]֮����ַ�ֻ��Ϊ����
                            is_Error = true;
                        }
                    }

                    if (leftNum != 1 || rightNum != 1)
                        is_Error = true; //[��]��ֻ�ܳ���һ��
                    if (is_Error)
                    {
                        if (Error.count(tmp) == 0)
                        { // ���Error���Ѿ�����
                            Error[tmp] = IdError++;
                        }
                        Token_list.push_back({"Error", tmp, Error[tmp]}); // �Ž�Token��
                    }
                    else
                    {
                        if (Array.count(tmp) == 0)
                        { // û����Ļ��Ž�Array��
                            Array[tmp] = IdArray++;
                        }
                        Token_list.push_back({"Arr", tmp, Array[tmp]});
                    }
                }
                else if (is_Identifier)
                { // �������飬���������֣��Ǿ��Ǳ�ʶ��
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
                    { // �ؼ��ֱ��Ҳ����Ǿ��Ǳ�ʶ��
                        if (Identifier.count(tmp) == 0)
                        {
                            Identifier[tmp] = IdIdentifier++;
                        }
                        Token_list.push_back({"I", tmp, Identifier[tmp]});
                    }
                }
            }

            else if (get_kind(buf[i]) == 1)
            {                // ������
                int num = 0; // С���������
                bool is_Error = false;
                string tmp;
                // ��û����;���������
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
            {                // �����ַ�����
                int num = 0; // ��'������
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
                int num = 0; // ��"������
                int Pos = 0; // �����һ��"���ֵ�λ�ã�Ӧ��������� string a="abcd"ab; ����Ҳ��Error
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
            { // ������ͨ���
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
    infile.open("./Lexer/KeyWord.txt", ios::in);
    if (!infile.is_open())
    {
        cout << "KeyWord�ļ���ȡʧ��" << "\n";
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
    infile.open("./Lexer/Delimiters.txt", ios::in);
    if (!infile.is_open())
    {
        cout << "Delimiters�ļ���ȡʧ��" << "\n";
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
        return 1; // ����
    if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z')
        return 2; // ��ĸ
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '|' || ch == '^' || ch == '&' || ch == '>' || ch == '<' || ch == '=')
        return 3; // �����
    else if (ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == '(' || ch == ')' || ch == ';' || ch == '\'' || ch == '.')
        return 4; // �ָ���
    else
        return 5; // ��������
}

void TokenSequence::test()
{
    cout << "����test\n";
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

// һЩ���۵�����
// int ab=1;  int a b =1;
// string a="  abwa "    ; string b="aw"abcd;
