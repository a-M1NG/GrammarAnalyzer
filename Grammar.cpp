#include "Lexer/Lexer.h"
#include "Lexer/Token.h"
#include "Lexer/includes.h"
#include "utils/GrammarParaser.cpp"
#include <string>
#include <vector>
class LL1
{
public:
    LL1(const TokenList tokens) : tokens(tokens)
    {
        decodeJson decoder;
        std::string path_for_Linux = "../utils/AnalysisTable.json";
        std::string path_for_Windows = "utils/AnalysisTable.json";
        analysisTable = decoder.getAnal(path_for_Linux);

        // 打印分析表
        // for (const auto &item : analysisTable)
        // {
        //     cout << item.first << " : " << endl;
        //     for (const auto &item2 : item.second)
        //     {
        //         cout << item2.first << " : ";
        //         for (const auto &item3 : item2.second)
        //         {
        //             cout << item3 << " ";
        //         }
        //         cout << endl;
        //     }
        // }

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

    // string Paraser()
    // {
    //     stack<string> stk;
    //     stk.push("$");
    //     stk.push("Program");
    //     int index = 0;
    //     while (!stk.empty())
    //     {
    //         string top = stk.top();
    //         stk.pop();
    //         auto curr = tokens[index];
    //         if (top == tokens[index].type)
    //         {
    //             index++;
    //         }
    //         else if (analysisTable.find(top) != analysisTable.end() && analysisTable[top].find(tokens[index].type) != analysisTable[top].end())
    //         {
    //             auto production = analysisTable[top][tokens[index].type];
    //             if (production[0] != "-1")
    //             {
    //                 // 逆序入栈
    //                 for (int i = production.size() - 1; i >= 0; i--)
    //                 {
    //                     stk.push(production[i]);
    //                 }
    //             }
    //             else
    //             {
    //                 return "error";
    //             }
    //         }
    //         else
    //         {
    //             return "error";
    //         }
    //     }
    // }

    LL1(const TokenList& tokens, const AnalysisTable& table) : tokens(tokens), analysisTable(table) {

        JSONParser parser;
        std::string path_for_Linux = "../utils/AnalysisTable.json";
        std::string path_for_Windows = "utils/AnalysisTable.json";
        analysisTable = parser.parse(path_for_Linux);

        for (const auto &item : analysisTable) {
            Vn.insert(item.first);
        }
        auto it = analysisTable.begin();
        for (const auto &item : it->second) {
            Vt.insert(item.first);
        }

        // 打印分析表
        parser.print(analysisTable);
        
        // 打印非终结符
        this->printVn();

        // 打印终结符
        this->printVt();

        initializeStacks();
    }

    bool parse();

private:
    TokenList tokens;
    // ll(1) 分析表
    AnalysisTable analysisTable;
    unordered_set<string> Vn;
    unordered_set<string> Vt;

    std::stack<std::string> analysis_stack_; // 分析栈
    std::stack<Token> input_stack_; // 输入栈

    void initializeStacks();
    bool applyProduction(const std::string& nonTerminal, const std::string& terminal);
    void printStacks() const;

    bool is_non_terminal(const std::string& symbol) {
        return Vn.find(symbol) != Vn.end();
    }

    bool is_terminal(const std::string& symbol) {
        return Vt.find(symbol) != Vt.end();
    }

    void printVn() {
        for (const auto& non_terminal : Vn) {
            std::cout << non_terminal << std::endl;
        }
    }

    void printVt() {
        for (const auto& terminal : Vt) {
            std::cout << terminal << std::endl;
        }
    }
};


void LL1::initializeStacks() {
    input_stack_.push(Token("#", "#", 0));

    // 将tokens反向推入输入栈
    for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
        input_stack_.push(*it);
    }
    // 初始化分析栈，假设起始符号为"S"
    analysis_stack_.push("#"); // 终结符号
    analysis_stack_.push("Program"); // 起始符号
}

bool LL1::applyProduction(const std::string& nonTerminal, const std::string& terminal) {
    std::vector<std::string> production = analysisTable[nonTerminal][terminal];
    std::cout << "production: " << nonTerminal << " -> ";
    for (const auto& symbol : production) {
        std::cout << symbol << " ";
    }
    std::cout << std::endl;
    if (production.empty()) {
        return false; // 无法应用产生式
    }
    if (production[0] == "-1") {
        return false; // 无法应用产生式
    }
    // 弹出非终结符
    analysis_stack_.pop();
    // 反向推入产生式中的符号
    for (auto it = production.rbegin(); it != production.rend(); ++it) {
        if (*it != "$") { // 假设$表示空产生式
            analysis_stack_.push(*it);
        }
    }
    return true;
}

void LL1::printStacks() const {
    std::cout << "Analysis stack: ";
    std::stack<std::string> analysis_stack = analysis_stack_;
    while (!analysis_stack.empty()) {
        std::cout << analysis_stack.top() << " ";
        analysis_stack.pop();
    }
    std::cout << std::endl;

    std::cout << "Input stack: ";
    std::stack<Token> input_stack = input_stack_;
    while (!input_stack.empty()) {
        std::cout << input_stack.top().value << " ";
        input_stack.pop();
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

bool LL1::parse() {
    while (!analysis_stack_.empty() && !input_stack_.empty()) {
        std::string top = analysis_stack_.top();
        Token currentToken = input_stack_.top();

        printStacks();
        
        if (top == currentToken.value || top == "$") { // 匹配终结符或空产生式
            // std::cout << "Matched " << top << std::endl;
            analysis_stack_.pop();
            input_stack_.pop();
        }
        else if (top == "ID" && currentToken.type == "I") {
            // std::cout << "Matched ID" << std::endl;
            analysis_stack_.pop();
            input_stack_.pop();
        } 
        else if (top == "IDList" && currentToken.type == "I") {
            analysis_stack_.pop();
            auto production = analysisTable["IDList"]["ID"];
            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                if (*it != "$") {
                    analysis_stack_.push(*it);
                }
            }
        }
        else if (top == "Expression" && currentToken.type == "Con") {
            // std::cout << "Matched Expression" << std::endl;
            analysis_stack_.pop();
            input_stack_.pop();
        } 
        else if (isupper(top[0])) { // 非终结符
            if (!applyProduction(top, currentToken.value)) {
                std::cerr << "Failed to apply production for non-terminal " << top << " and terminal " << currentToken.value << std::endl;
                return false; // 无法应用产生式
            }
        }
        else if (top == "#" && currentToken.value == "#") {
            std::cout << "Parsing successful!" << std::endl;
            return true; // 分析成功
        } 
        else {
            return false; // 语法错误
        }
    }
    return analysis_stack_.empty() && input_stack_.empty();
}


int main()
{
    TokenSequence toseq;
    std::string path_for_Linux = "../test.txt";
    std::string path_for_Windows = "test.txt";
    auto tokens = toseq.getToken_list(path_for_Linux);
    // toseq.printToken();
    LL1 ll1(tokens, AnalysisTable());
    if (ll1.parse()) {
        std::cout << "Parsing successful!" << std::endl;
    } else {
        std::cerr << "Parsing failed!" << std::endl;
    }
    return 0;
}