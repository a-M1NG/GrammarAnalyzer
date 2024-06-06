#include "Lexer/Lexer.cpp"
#include "Lexer/Token.h"
#include "Lexer/includes.h"
#include "utils/GrammarParaser.cpp"
class LL1
{
public:
    LL1(const TokenList tokens) : tokens(tokens)
    {
        decodeJson decoder;
        analysisTable = decoder.getAnal("./utils/AnalysisTable.json");
    }

    string Paraser()
    {
        stack<string> stk;
        stk.push("$");
        stk.push("Program");
        int index = 0;
        while (!stk.empty())
        {
            string top = stk.top();
            stk.pop();
            if (top == tokens[index].type)
            {
                index++;
            }
            else if (analysisTable.find(top) != analysisTable.end() && analysisTable[top].find(tokens[index].type) != analysisTable[top].end())
            {
                auto production = analysisTable[top][tokens[index].type];
                if (production[0] != "-1")
                {
                    // 逆序入栈
                    for (int i = production.size() - 1; i >= 0; i--)
                    {
                        stk.push(production[i]);
                    }
                }
                else
                {
                    return "error";
                }
            }
            else
            {
                return "error";
            }
        }
    }

private:
    TokenList tokens;
    // ll(1) 分析表
    AnalysisTable analysisTable;
};

int main()
{
    TokenSequence toseq;
    auto tokens = toseq.getToken_list("test.txt");
    for (auto token : tokens)
    {
        std::cout << token.type << " " << token.value << std::endl;
    }
    std::cout << "commit test" << std::endl;
    system("pause");
    return 0;
}