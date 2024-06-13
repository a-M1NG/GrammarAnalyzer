#ifndef QUATER_H
#define QUATER_H

#include "data_type.h"
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>

/// tokens token序列
/// quater 四元式
/// semStack 语义栈 储存中间结果、操作数、操作符和其他必要的语义值
class quaterGen
{
private:
    std::vector<Token> tokens;
    std::vector<Quater> quater;
    std::stack<std::string> semStack;
    std::stack<std::string> opStack;
    std::stack<std::string> blockStack; // 用于追踪代码块的开始和结束
    int currentToken = 0;
    int tempVarCount = 0;
    int LabelCount = 0;
    bool processingIfCondition = false;    // 追踪是否正在处理if条件表达式
    bool processingWhileCondition = false; // 追踪是否正在处理while条件表达式
    bool processingElseBlock = false;
    int whiletime = 0;
    int ifcount = 0;

    std::map<std::string, int> precedence = {
        {"(", 4},
        {"*", 3},
        {"/", 3}, // 高优先级
        {"+", 2},
        {"-", 2}, // 低优先级
        {">", 1},
        {"<", 1},
        {">=", 1},
        {"<=", 1},
        {"==", 1},
        {"!=", 1}, // 比较操作优先级通常低于算术操作
        {"=", 0}};

public:
    /// @brief
    /// @param inputTokens 输入的Token序列
    quaterGen(const std::vector<Token> &inputTokens) : tokens(inputTokens) {}

    Token getNextToken()
    {
        if (currentToken < tokens.size())
        {
            return tokens[currentToken++];
        }
        return {"", "", 0, 0};
    }

    void generate()
    {
        for (auto &token : tokens)
        {

            if (token.type == "P" || token.type == "K") // 界符或关键字
            {
                if (token.value == "if" || token.value == "while" || token.value == "else" || token.value == "else if")
                {
                    handleControlFlow(token);
                }
                else if (token.value == ";")
                {
                    finalizeOperations(); // 在语句结束时调用
                }
                else if (token.value == "{")
                {
                    blockStack.push(token.value);
                }
                else if (token.value == "}")
                {
                    handleOperator(token);
                }
                else if (token.value == "int" || token.value == "float" || token.value == "void" || token.value == "long" || token.value == "long long")
                {
                    // std::cout<<"666"<<std::endl;
                }
                else
                    handleOperator(token);
            }
            else if (token.value != "main")
            {
                semStack.push(token.value);
            }
        }
    }

    void handleOperator(const Token &token)
    {
        if (token.value == "}")
        {
            if (!blockStack.empty() && blockStack.top() == "{")
            {
                // std::cout<<blockStack.top()<<std::endl;
                std::string block = blockStack.top();
                blockStack.pop();
                // std::cout<<"block栈大小："<<blockStack.size()<<std::endl;
                // std::cout<<whiletime<<"    "<<block<<std::endl;
                if (whiletime && blockStack.empty())
                {
                    quater.push_back({"we", "-", "-", "-"});
                    whiletime = 0;
                    processingWhileCondition = false; // 重置处理循环条件标志
                }
            }
        }

        else if (token.value == ")")
        {
            finalizeOperations(); // 确保条件表达式内的所有操作都被处理
            return;
        }
        if (token.value.empty() || precedence.find(token.value) == precedence.end())
        {
            return; // 如果token是空的，直接返回，避免无效操作
        }
        /// 调试代码
        // std::cout << "Current opStack size: " << opStack.size() << std::endl;
        // if (!opStack.empty()) {
        //     std::cout << "Top of opStack: " << opStack.top() << ", Precedence: " << precedence[opStack.top()] << std::endl;
        // }
        // std::cout << "Current token: " << token.value << ", Precedence: " << precedence[token.value] << std::endl;

        while (!opStack.empty() && precedence[opStack.top()] >= precedence[token.value])
        {
            std::string op = opStack.top();
            opStack.pop();
            // std::cout<<semStack.top()<<std::endl;
            //  if (semStack.size() < 2)
            //  {  // 检查semStack中是否至少有两个元素
            //      std::cerr << "没有操作符对象： " << op << std::endl;
            //      return;
            //  }
            if (op == "(")
            {
                opStack.push(token.value);
                return;
            }
            else if (op == "=")
            {
                std::string rvalue = semStack.top();
                semStack.pop();
                std::string lvalue = semStack.top();
                semStack.pop();
                quater.push_back({op, lvalue, "''", rvalue});
            }
            else
            {
                std::string operand2 = semStack.top();
                semStack.pop();
                std::string operand1 = semStack.top();
                semStack.pop();

                std::string tempVar = createTempVar();
                quater.push_back({op, operand1, operand2, tempVar});
                semStack.push(tempVar);
            }
        }
        if (precedence.find(token.value) != precedence.end())
        { // 确保操作符有效
            opStack.push(token.value);
        }
    }

    void handleControlFlow(const Token &token)
    {
        if (token.value == "if")
        {
            // 设置正在处理if条件
            processingIfCondition = true;
            // 预期在解析完整个条件表达式后生成跳转四元式
        }
        else if (token.value == "else")
        {
            processingElseBlock = true;
            std::string tempLabel = createLabel();
            quater.push_back({"else", "-", "-", tempLabel});
        }
        else if (token.value == "else if")
        {
            // processingElseBlock = true;
            std::string tempLabel = createLabel();
            quater.push_back({"elseif", "-", "-", tempLabel});
        }
        else if (token.value == "while")
        {
            processingWhileCondition = true;
            whiletime = 1;
            // std::cout<<"whiletime= "<<whiletime<<std::endl;
            quater.push_back({"while", "-", "-", "-"});
        }
    }

    void finalizeOperations()
    {
        while (!opStack.empty())
        {
            std::string op = opStack.top();
            opStack.pop();

            if (op == "=")
            {
                std::string operand2 = semStack.top();
                semStack.pop();
                std::string operand1 = semStack.top();
                semStack.pop();
                quater.push_back({op, operand1, "-", operand2});
                semStack.push("-");
            }
            else if (op != "(")
            {
                std::string operand2 = semStack.top();
                semStack.pop();
                std::string operand1 = semStack.top();
                semStack.pop();
                std::string tempVar = createTempVar();
                quater.push_back({op, operand1, operand2, tempVar});
                semStack.push(tempVar);
            }
        }

        if (processingIfCondition)
        {
            std::string conditionResult = semStack.top();
            semStack.pop();
            std::string labelFalse = createLabel();
            quater.push_back({"ifFalse", conditionResult, "-", labelFalse});
            processingIfCondition = false; // 重置if条件处理标志
        }
        else if (processingElseBlock)
        {
            quater.push_back({"ifend", "-", "-", "-"}); // 在else块结束后添加
            processingElseBlock = false;                // 结束else块处理
        }
        else if (processingWhileCondition)
        {
            std::string conditionResult = semStack.top();
            semStack.pop();
            std::string labelEnd = createLabel();
            std::string startLabel = semStack.top();
            semStack.pop();

            quater.push_back({"do", conditionResult, "-", "-"}); // 循环体开始标记
            processingWhileCondition = false;
        }
    }

    std::string createTempVar() // t1 t2等
    {
        return "t" + std::to_string(tempVarCount++);
    }

    std::string createLabel() // L1 L2等
    {
        return "L" + std::to_string(LabelCount++);
    }

    void printQuaters()
    {
        for (const auto &q : quater)
        {
            std::cout << "(" << q.op << ", " << q.arg1 << ", " << q.arg2 << ", " << q.result << ")\n";
        }
    }
};

#endif // QUATER_H