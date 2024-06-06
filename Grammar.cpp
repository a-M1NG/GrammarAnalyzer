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