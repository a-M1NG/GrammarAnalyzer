#include "Lexer/Lexer.h"
#include "Lexer/data_type.h"
#include "Lexer/includes.h"
#include "Lexer/quater.h"
#include "utils/GrammarParaser.cpp"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
class LL1
{
public:
    LL1(const TokenList tokens) : tokens(tokens)
    {
        decodeJson decoder;
        std::string file_path;
        #if defined(_WIN32) || defined(_WIN64)
            file_path = "utils/AnalysisTable.json";
        #elif defined(__linux__)
            file_path = "../utils/AnalysisTable.json";
        #else
            std::cout << "Unknown OS" << std::endl;
        #endif
        analysisTable = decoder.getAnal(file_path);

        for (const auto &item : analysisTable)
        {
            Vn.insert(item.first);
        }
        auto it = analysisTable.begin();
        for (const auto &item : it->second)
        {
            Vt.insert(item.first);
        }
    }

    LL1(const TokenList &tokens, const AnalysisTable &table, bool debug = false) : tokens(tokens), analysisTable(table), debug(debug)
    {

        JSONParser parser;
        std::string file_path;
        #if defined(_WIN32) || defined(_WIN64)
            file_path = "utils/AnalysisTable.json";
        #elif defined(__linux__)
            file_path = "../utils/AnalysisTable.json";
        #else
            std::cout << "Unknown OS" << std::endl;
        #endif
        analysisTable = parser.parse(file_path);

        for (const auto &item : analysisTable)
        {
            Vn.insert(item.first);
        }
        auto it = analysisTable.begin();
        for (const auto &item : it->second)
        {
            Vt.insert(item.first);
        }

        if (debug == true)
        {
            std::cout << "Vn: " << std::endl;
            printVn();
            std::cout << "Vt: " << std::endl;
            printVt();
            // std::cout << "Analysis Table: " << std::endl;
            // parser.print(analysisTable);
        }

        initializeStacks();
    }

    std::vector<std::vector<Token>> getFuncBlocks()
    {
        return funcBlocks;
    }

    void createErrorList(const std::string &nonTerminal, const Token &terminal)
    {
        if (nonTerminal == "")
        {
            std::cerr << "Unexpected token " << terminal.value << std::endl;
            Error_List.push_back(msg{terminal.value, terminal.line, "Unexpected token"});
        }
        else if (nonTerminal == "-1")
        {
            std::cerr << "Unexpected token " << terminal.value << std::endl;
            Error_List.push_back(msg{terminal.value, terminal.line, "Unexpected token"});
        }
        else if (nonTerminal == "Expression")
        {
            std::cerr << terminal.value << " is not a valid expression" << std::endl;
            std::string error = terminal.value + " is not a valid expression";
            Error_List.push_back(msg{terminal.value, terminal.line, error});
        }
        else if (nonTerminal == "IDList")
        {
            std::cerr << terminal.value << " is not a valid terminal" << std::endl;
            std::string error = terminal.value + " is not a valid terminal";
            Error_List.push_back(msg{terminal.value, terminal.line, error});
        }
        else
        {
            std::cerr << "Other error" << std::endl;
            std::cerr << "Failed to apply production for non-terminal " << nonTerminal << " and terminal " << terminal.value << std::endl;
            std::string error = "Failed to apply production for non-terminal " + nonTerminal + " and terminal " + terminal.value;
            Error_List.push_back(msg{terminal.value, terminal.line, error});
        }
    }

    bool parse();
    void printFunc()
    {
        std::cout << "\nFunction blocks:--------" << std::endl;
        for (const auto &func : funcBlocks)
        {
            std::cout << func.size() << std::endl;
            for (const auto &token : func)
            {
                std::cout << token.value << " ";
            }
            std::cout << std::endl;
        }
        std::cout << "-------------------------" << std::endl;
    }

private:
    TokenList tokens;
    // ll(1) 分析表
    AnalysisTable analysisTable;
    unordered_set<string> Vn;
    unordered_set<string> Vt;
    std::vector<Token> funcBlock;
    std::vector<std::vector<Token>> funcBlocks;

    std::stack<std::string> analysis_stack_; // 分析栈
    std::stack<Token> input_stack_;          // 输入栈

    std::vector<msg> Error_List;

    bool debug = false;

    void initializeStacks();
    bool applyProduction(const std::string &nonTerminal, const Token &terminal);
    void printStacks() const;

    bool is_non_terminal(const std::string &symbol)
    {
        return Vn.find(symbol) != Vn.end();
    }

    bool is_terminal(const std::string &symbol)
    {
        return Vt.find(symbol) != Vt.end();
    }

    void printVn()
    {
        for (const auto &non_terminal : Vn)
        {
            std::cout << non_terminal << std::endl;
        }
    }

    void printVt()
    {
        for (const auto &terminal : Vt)
        {
            std::cout << terminal << std::endl;
        }
    }
};

void LL1::initializeStacks()
{
    input_stack_.push(Token("#", "#", 0, 0)); // 输入栈底

    // 将tokens反向推入输入栈
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it)
    {
        input_stack_.push(*it);
    }
    // 初始化分析栈，假设起始符号为"S"
    analysis_stack_.push("#");       // 终结符号
    analysis_stack_.push("Program"); // 起始符号
}

bool LL1::applyProduction(const std::string &nonTerminal, const Token &terminal)
{

    std::vector<std::string> production;
    production.clear();

    if (terminal.type == "I" || terminal.type == "Con")
    {
        production = analysisTable[nonTerminal]["ID"];
    }
    else if (nonTerminal == "IDList" && terminal.type == "I")
    {
        analysis_stack_.pop();
        production = analysisTable["IDList"]["ID"];
    }
    else
        production = analysisTable[nonTerminal][terminal.value];

    if (debug)
    {
        std::cout << "production: " << nonTerminal << " -> ";
        for (const auto &symbol : production)
        {
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }

    if (nonTerminal == "ID" && (terminal.type == "I" || terminal.type == "Con"))
    {
        analysis_stack_.pop();
        input_stack_.pop();
        funcBlock.push_back(terminal);
        return true;
    }

    if (production.empty())
    {
        return false; // 无法应用产生式
    }
    if (production[0] == "-1")
    {
        return false; // 无法应用产生式
    }
    // 弹出非终结符
    analysis_stack_.pop();
    // 反向推入产生式中的符号
    for (auto it = production.rbegin(); it != production.rend(); ++it)
    {
        if (*it != "$")
        { // 假设$表示空产生式
            analysis_stack_.push(*it);
        }
    }
    return true;
}

void LL1::printStacks() const
{
    std::cout << "Analysis stack: ";
    std::stack<std::string> analysis_stack = analysis_stack_;
    while (!analysis_stack.empty())
    {
        std::cout << analysis_stack.top() << " ";
        analysis_stack.pop();
    }
    std::cout << std::endl;

    std::cout << "Input stack: ";
    std::stack<Token> input_stack = input_stack_;
    while (!input_stack.empty())
    {
        std::cout << input_stack.top().value << " ";
        input_stack.pop();
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

bool LL1::parse()
{
    while (!analysis_stack_.empty() && !input_stack_.empty())
    {
        std::string top = analysis_stack_.top();
        Token currentToken = input_stack_.top();
        if (top == "FuncDefs")
        {
            funcBlocks.push_back(funcBlock);
            funcBlock.clear();
        }
        if (debug)
            printStacks();

        // 前期在词法中做的处理，界符和关键字都是终结符且能直接匹配，用户自定义标识符和常数需要进一步处理
        if (top == currentToken.value && (currentToken.type == "K" || currentToken.type == "P"))
        { // 匹配终结符
            if (debug)
                std::cout << "Matching " << top << " with " << currentToken.value << std::endl;
            analysis_stack_.pop();
            input_stack_.pop();

            funcBlock.push_back(currentToken);
        }
        else if (isupper(top[0]))
        // else if (is_non_terminal(top))
        { // 非终结符
            std::cout << "fuck 114514 fuck: " << top[0] << std::endl;
            // std::cout << "fuck 114514 fuck: " << top << std::endl;
            if (!applyProduction(top, currentToken))
            {
                std::cerr << "Error at line " << currentToken.line << ": ";
                createErrorList(top, currentToken);
                return false; // 无法应用产生式
            }
        }
        else if (top == "#" && currentToken.value == "#")
        {
            funcBlocks.push_back(funcBlock);
            // std::cout << "Parsing successful!" << std::endl;
            return true; // 分析成功
        }
        else
        {
            return false; // 语法错误
        }
    }
    return analysis_stack_.empty() && input_stack_.empty();
}

int main()
{
    std::string file_path;
    #if defined(_WIN32) || defined(_WIN64)
        file_path = "test.txt";
    #elif defined(__linux__)
        file_path = "../test.txt";
    #else
        std::cout << "Unknown OS" << std::endl;
    #endif
    TokenSequence toseq;
    auto tokens = toseq.getToken_list(file_path);
    toseq.printToken();
    LL1 ll1(tokens, AnalysisTable(), true);
    if (ll1.parse())
    {
        std::cout << "Parsing successful!" << std::endl;
    }
    else
    {
        std::cerr << "Parsing failed!" << std::endl;
    }
    ll1.printFunc();
    auto funcs = ll1.getFuncBlocks();
    for (const auto &func : funcs)
    {
        if (func.size() == 0)
            continue;
        quaterGen qt(func);
        std::cout << "Quater generation:" << std::endl;
        qt.generate();
        std::cout << "Quater list:" << std::endl;
        qt.printQuaters();
    }
    #if defined(_WIN32) || defined(_WIN64)
        system("pause");
    #elif defined(__linux__)
        std::cout << "Run on Linux" << std::endl;
    #else
        std::cout << "Unknown OS" << std::endl;
    #endif
    return 0;
}