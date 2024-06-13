#ifndef QUATER_H
#define QUATER_H

#include "data_type.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>

/// 基本块
class BasicBlock {
public:
    std::vector<Quater> instructions;

    void addInstruction(const Quater& instr) {
        instructions.push_back(instr);
    }

    void print() {
        std::cout << "Basic Block:" << std::endl;
        for (const auto& instr : instructions) {
            std::cout << "(" << instr.op << ", " << instr.arg1 << ", " << instr.arg2 << ", " << instr.result << ")\n";
        }
    }
};

/// tokens token序列
/// quater 四元式
/// semStack 语义栈 储存中间结果、操作数、操作符和其他必要的语义值
class quaterGen
{
private:
    std::vector <Token> tokens;
    std::vector <Quater> quater;
    std::vector<BasicBlock> basicBlocks;
    std::stack <std::string> semStack; 
    std::stack <std::string> opStack; 
    std::stack<std::string> blockStack; 
    int currentToken = 0;
    int tempVarCount = 0;
    int LabelCount = 0;
    bool processingIfCondition = false;  // 追踪是否正在处理if条件表达式
    bool processingWhileCondition = false;  // 追踪是否正在处理while条件表达式
    bool processingElseBlock = false;
    int whiletime=0;
    int ifcount=0;
    int whilecount=0;

    std::map<std::string, int> precedence = {
        {"(",4},
        {"*", 3}, {"/", 3},   // 高优先级
        {"+", 2}, {"-", 2},   // 低优先级
        {">", 1}, {"<", 1}, {">=", 1}, {"<=", 1} , {"==", 1}, {"!=", 1},// 比较操作优先级通常低于算术操作
        {"=",0} 
    };

public:
    /// @brief 
    /// @param inputTokens 输入的Token序列
    quaterGen(const std::vector <Token>& inputTokens) :tokens(inputTokens){}

    Token getNextToken() {
        if (currentToken < tokens.size()) {
            return tokens[currentToken++];
        }
        return {"", "", 0, 0};
    }

    void generate()
    {
         for (auto& token : tokens)
        {
            
            if (token.type == "P" || token.type == "K")   //界符或关键字
            {
                if(token.value=="if"||token.value=="while"||token.value=="else"||token.value=="elseif")
                {
                    if(token.value=="if")
                        ifcount++;
                    if(token.value=="while")
                        whilecount++;
                    handleControlFlow(token);
                }
                else if (token.value == ";") 
                {
                   finalizeOperations();  // 在语句结束时调用
                }
                else if(token.value=="{") 
                {
                    blockStack.push(token.value);
                }
                else if(token.value=="}")
                {
                    handleOperator(token); 
                }
                else if(token.value=="int"||token.value=="float"||token.value=="void"||token.value=="long"||token.value=="long long")
                {
                }
                else
                    handleOperator(token); 
            }
            else
            {
                semStack.push(token.value);
            }
        }
    }

    void handleOperator(const Token& token)
    {
        if (token.value == "}") {
            if (!blockStack.empty() && blockStack.top() == "{")
            {
                blockStack.pop();
                if(whiletime&&blockStack.empty())
                {
                    quater.push_back({"we", "_","_", "_"});
                    whiletime=0;
                    processingWhileCondition = false; // 重置处理循环条件标志
                }

            }
        }

        else if (token.value == ")") 
        {
            finalizeOperations();  // 确保条件表达式内的所有操作都被处理
            return;
        }
        if (token.value.empty()|| precedence.find(token.value) == precedence.end()) {
            return;  // 如果token是空的，直接返回，避免无效操作
        }
        ///调试代码

        while (!opStack.empty()&& precedence[opStack.top()] >= precedence[token.value]) 
        {
            std::string op = opStack.top(); 
            opStack.pop();

            if(op=="(")
            {
                opStack.push(token.value);
                return;
            }
            else if(op=="=")
            {
                std::string rvalue = semStack.top();
                semStack.pop();
                std::string lvalue = semStack.top();
                semStack.pop();
                quater.push_back({op, lvalue,"''", rvalue});
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
        if (precedence.find(token.value) != precedence.end()) { // 确保操作符有效
            opStack.push(token.value);
        }
    }

    void handleControlFlow(const Token& token) {
    if (token.value == "if") {
        // 设置正在处理if条件
        processingIfCondition = true;
        // 预期在解析完整个条件表达式后生成跳转四元式
    } 
    else if(token.value=="else")
    {
        processingElseBlock = true;
        std::string tempLabel=createLabel();
        quater.push_back({"else", "_", "_", tempLabel});
    }
    else if(token.value=="elseif")
    {
        std::string tempLabel=createLabel();
        quater.push_back({"elseif", "_", "_", tempLabel});
    }
    else if (token.value == "while") {
        processingWhileCondition = true;
        whiletime=1;
        quater.push_back({"while", "_", "_", "_"});
        
    } 
}

    void finalizeOperations() {
    while (!opStack.empty()) 
    {
        std::string op = opStack.top();
        opStack.pop();
        
        if(op=="=")
        {
            std::string operand2 = semStack.top(); 
            semStack.pop();
            std::string operand1 = semStack.top(); 
            semStack.pop();
            quater.push_back({op, operand1, "_",operand2});
            semStack.push("_");
        }
        else if(op!="(")//实现常数表达式的直接计算
        {
            std::string operand2 = semStack.top(); 
            semStack.pop();
            std::string operand1 = semStack.top(); 
            semStack.pop();
            //std::string tempVar = createTempVar();

            if (isNumber(operand1) && isNumber(operand2)) {
            int result = compute(op, std::stoi(operand1), std::stoi(operand2));
            semStack.push(std::to_string(result));
            //quater.push_back({"=", std::to_string(result), "_", createTempVar()});
        } else {
            std::string tempVar = createTempVar();
            quater.push_back({op, operand1, operand2, tempVar});
            semStack.push(tempVar);
        }
        }
    }

    if (processingIfCondition) {
        std::string conditionResult = semStack.top();
        semStack.pop();
        std::string labelFalse = createLabel();
        quater.push_back({"ifFalse", conditionResult, "_", labelFalse});
        processingIfCondition = false;  // 重置if条件处理标志

    }
    else if(processingElseBlock)
    {
        quater.push_back({"ifend", "_", "_", "_"});  // 在else块结束后添加
        processingElseBlock = false;  // 结束else块处理

    }
    else if(processingWhileCondition)
    {
        std::string conditionResult = semStack.top();
        semStack.pop();
        std::string labelEnd = createLabel();
        std::string startLabel = semStack.top();
        semStack.pop();

        quater.push_back({"do", conditionResult, "_", "_"}); // 循环体开始标记
        processingWhileCondition = false;
    }
}
    
    bool isNumber(const std::string& s) 
    {
        return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    int compute(const std::string& op, int operand1, int operand2) 
    {
        if (op == "+") return operand1 + operand2;
        else if (op == "-") return operand1 - operand2;
        else if (op == "*") return operand1 * operand2;
        else if (op == "/") return operand2 != 0 ? operand1 / operand2 : 0;
        return 0;
    }


    std::string createTempVar()   //t1 t2等
    {
        return "@t"+std::to_string(tempVarCount++);
    }

    std::string createLabel()    //L1 L2等
    {
        return "L"+std::to_string(LabelCount++);
    }

    void identifyBasicBlocks() {
        BasicBlock currentBlock;
        bool startNewBlock = true;

        for (auto& instr : quater) {
            if (startNewBlock) {
                if (!currentBlock.instructions.empty()) {
                    basicBlocks.push_back(currentBlock);
                    currentBlock = BasicBlock();
                }
                startNewBlock = false;
            }

            currentBlock.addInstruction(instr);

            // 检查是否需要在这个点开始一个新的基本块
            if (instr.op == "ifFalse" || instr.op == "we"|| instr.op == "elseif"|| instr.op == "else") {
                startNewBlock = true;
            }
        }

        // 添加最后一个块，如果有的话
        if (!currentBlock.instructions.empty()) {
            basicBlocks.push_back(currentBlock);
        }
    }

    void optimizeBasicBlock(BasicBlock& block) {
    std::map<std::tuple<std::string, std::string, std::string>, std::string> expressionCache;

    for (auto& instr : block.instructions) {
        std::tuple<std::string, std::string, std::string> exprKey(instr.op, instr.arg1, instr.arg2);
        if (expressionCache.find(exprKey) != expressionCache.end()) {
            // 重用之前的计算结果
            instr.result = expressionCache[exprKey];
        } else {
            // 将新的表达式和结果添加到缓存中
            expressionCache[exprKey] = instr.result;
        }
    }
}



    void printQuaters()
    {
        for (const auto& q : quater)
        {
            std::cout << "(" << q.op << ", " << q.arg1 << ", " << q.arg2 << ", " << q.result << ")\n";
        }
    }

    void printBasicBlocks() {
        for (auto& block : basicBlocks) {
            block.print();
        }
    }

    void performOptimizations() 
    {
        for (auto& block : basicBlocks) 
        {
            optimizeBasicBlock(block);
        }
    }


};


#endif // QUATER_H