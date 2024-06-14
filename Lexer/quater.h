#ifndef TEST_H
#define TEST_H

#include "data_type.h"
#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>

struct Node {
    std::string value;
    std::string op;
    std::vector<Node*> children;
    

    Node(std::string val, std::string operation = "") : value(val), op(operation) {}
};

Node* createOrGetNode(std::unordered_map<std::string, Node*>& nodes, const std::string& key, const std::string& op = "") {
    if (nodes.find(key) == nodes.end()) {
        nodes[key] = new Node(key, op);
    }
    return nodes[key];
}

std::unordered_map<std::string, Node*> buildDAG(const std::vector<Quater>& quads) {
    std::unordered_map<std::string, Node*> nodes;
    std::unordered_map<std::string, Node*> expressions;

    for (const auto& quad : quads) {
        std::string expr_key = quad.op + quad.arg1 + (quad.arg2.empty() ? "" : quad.arg2) + quad.result;
        //std::cout << "Processing: " << quad.op << " " << quad.arg1 << " " << quad.arg2 << " -> " << quad.result << std::endl;

        if (expressions.find(expr_key) != expressions.end()) {
            // 公共子表达式已存在，重用节点
            //std::cout << "Found common subexpression for " << expr_key << ", result: " << quad.result << std::endl;
            nodes[quad.result] = expressions[expr_key];
        } else {
            Node* left = createOrGetNode(nodes, quad.arg1);
            Node* right = quad.arg2.empty() ? nullptr : createOrGetNode(nodes, quad.arg2);
            Node* result = new Node(quad.result, quad.op);

            // 连接节点
            if (!quad.arg1.empty()) result->children.push_back(left);
            if (right) result->children.push_back(right);

            // 更新DAG和表达式映射
            nodes[quad.result] = result;
            expressions[expr_key] = result;
        }
    }
    return nodes;
}


void regenerateQuadruplesFromDAG(const std::unordered_map<std::string, Node*>& nodes, std::vector<Quater>& optimizedQuads, const std::vector<Quater>& originalQuads) {
    optimizedQuads.clear();
    std::unordered_set<std::string> usedExpressions;

    for (const auto& quad : originalQuads) {
        if (nodes.find(quad.result) != nodes.end()) {
            //std::cout<<quad.op<<"  "<<quad.arg1<<"  "<<quad.arg2<<"  "<<quad.result<<"  "<<std::endl;
            Node* node = nodes.at(quad.result);
            //std::cout<<quad.op<<"  "<<quad.arg1<<"  "<<quad.arg2<<"  "<<quad.result<<"  "<<std::endl;
            std::string expr_key = quad.op+quad.arg1+quad.arg2;
            //std::cout<<expr_key<<std::endl;
            if (usedExpressions.find(expr_key) == usedExpressions.end()) {
                // 仅保留首次出现的子表达式
                Quater newQuad;
                newQuad.op = quad.op;
                newQuad.arg1 = quad.arg1;
                newQuad.arg2 = quad.arg2;
                newQuad.result = quad.result;
                // newQuad.op = node->op;
                // newQuad.arg1 = node->children.size() > 0 ? node->children[0]->value : "";
                // newQuad.arg2 = node->children.size() > 1 ? node->children[1]->value : "";
                // newQuad.result = node->value;
                // std::cout << "Adding Quad: " << newQuad.op << " " << newQuad.arg1 << " " << newQuad.arg2 << " -> " << newQuad.result << std::endl;
                optimizedQuads.push_back(newQuad);
                usedExpressions.insert(expr_key);
            }
        }
    }
}


void printOptimizedQuadruples(const std::vector<Quater>& quads) {
    for (const auto& quad : quads) {
        std::cout << "(" << quad.op << ", " << quad.arg1 << ", " << quad.arg2 << ", " << quad.result << ")" << std::endl;
    }
}

std::map<std::string, int> precedence = {
    {"(", 4}, {"*", 3}, {"/", 3},   // 高优先级
    {"+", 2}, {"-", 2},   // 低优先级
    {">", 1}, {"<", 1}, {">=", 1}, {"<=", 1}, {"==", 1}, {"!=", 1}, // 比较操作优先级通常低于算术操作
    {"=", 0}
};

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

class quaterGen {
private:
    std::vector<Token> tokens;
    std::vector<Quater> quater;
    std::vector<BasicBlock> basicBlocks;
    std::stack<std::string> semStack;
    std::stack<std::string> opStack;
    std::stack<std::string> blockStack;
    int currentToken = 0;
    int tempVarCount = 0;
    int LabelCount = 0;
    bool processingIfCondition = false;  // 追踪是否正在处理if条件表达式
    bool processingWhileCondition = false;  // 追踪是否正在处理while条件表达式
    bool processingElseBlock = false;
    int whiletime = 0;
    int ifcount = 0;
    int whilecount = 0;

public:
    quaterGen(const std::vector<Token>& inputTokens) : tokens(inputTokens) {}

    Token getNextToken() {
        if (currentToken < tokens.size()) {
            return tokens[currentToken++];
        }
        return { "", "", 0, 0};
    }

    void generate() {
        for (auto& token : tokens) {
            if (token.type == "P" || token.type == "K") {   // 界符或关键字
                if (token.value == "if" || token.value == "while" || token.value == "else" || token.value == "elseif") {
                    if (token.value == "if")
                        ifcount++;
                    if (token.value == "while")
                        whilecount++;
                    handleControlFlow(token);
                } else if (token.value == ";") {
                    finalizeOperations();  // 在语句结束时调用
                } else if (token.value == "{") {
                    blockStack.push(token.value);
                } else if (token.value == "}") {
                    handleOperator(token);
                } else if (token.value == "int" || token.value == "float" || token.value == "void" || token.value == "long" || token.value == "long long") {
                } else {
                    handleOperator(token);
                }
            } else {
                semStack.push(token.value);
            }
        }
    }

    void handleOperator(const Token& token) {
        if (token.value == "}") {
            if (!blockStack.empty() && blockStack.top() == "{") {
                blockStack.pop();
                if (whiletime && blockStack.empty()) {
                    quater.push_back({ "we", "_", "_", "_" });
                    whiletime = 0;
                    processingWhileCondition = false; // 重置处理循环条件标志
                }
            }
        } else if (token.value == ")") {
            finalizeOperations();  // 确保条件表达式内的所有操作都被处理
            return;
        }
        if (token.value.empty() || precedence.find(token.value) == precedence.end()) {
            return;  // 如果token是空的，直接返回，避免无效操作
        }

        while (!opStack.empty() && precedence[opStack.top()] >= precedence[token.value]) {
            std::string op = opStack.top();
            opStack.pop();

            if (op == "(") {
                opStack.push(token.value);
                return;
            } else if (op == "=") {
                std::string rvalue = semStack.top();
                semStack.pop();
                std::string lvalue = semStack.top();
                semStack.pop();
                quater.push_back({ op, lvalue, "''", rvalue });
            } else {
                std::string operand2 = semStack.top();
                semStack.pop();
                std::string operand1 = semStack.top();
                semStack.pop();

                std::string tempVar = createTempVar();
                quater.push_back({ op, operand1, operand2, tempVar });
                semStack.push(tempVar);
            }
        }
        if (precedence.find(token.value) != precedence.end()) { // 确保操作符有效
            opStack.push(token.value);
        }
    }

    void handleControlFlow(const Token& token) {
        if (token.value == "if") {
            processingIfCondition = true;
        } else if (token.value == "else") {
            processingElseBlock = true;
            std::string tempLabel = createLabel();
            quater.push_back({ "else", "_", "_", tempLabel });
        } else if (token.value == "elseif") {
            std::string tempLabel = createLabel();
            quater.push_back({ "elseif", "_", "_", tempLabel });
        } else if (token.value == "while") {
            processingWhileCondition = true;
            whiletime = 1;
            quater.push_back({ "while", "_", "_", "_" });
        }
    }

    void finalizeOperations() {
        while (!opStack.empty()) {
            std::string op = opStack.top();
            opStack.pop();

            if (op == "=") {
                std::string operand2 = semStack.top();
                semStack.pop();
                std::string operand1 = semStack.top();
                semStack.pop();
                quater.push_back({ op, operand1, "_", operand2 });
                semStack.push("_");
            } else if (op != "(") { // 实现常数表达式的直接计算
                std::string operand2 = semStack.top();
                semStack.pop();
                std::string operand1 = semStack.top();
                semStack.pop();
                if (isNumber(operand1) && isNumber(operand2)) {
                    int result = compute(op, std::stoi(operand1), std::stoi(operand2));
                    semStack.push(std::to_string(result));
                } else {
                    std::string tempVar = createTempVar();
                    quater.push_back({ op, operand1, operand2, tempVar });
                    semStack.push(tempVar);
                }
            }
        }

        if (processingIfCondition) {
            std::string conditionResult = semStack.top();
            semStack.pop();
            std::string labelFalse = createLabel();
            quater.push_back({ "ifFalse", conditionResult, "_", labelFalse });
            processingIfCondition = false;  // 重置if条件处理标志
        } else if (processingElseBlock) {
            quater.push_back({ "ifend", "_", "_", "_" });  // 在else块结束后添加
            processingElseBlock = false;  // 结束else块处理
        } else if (processingWhileCondition) {
            std::string conditionResult = semStack.top();
            semStack.pop();
            std::string labelEnd = createLabel();
            std::string startLabel = semStack.top();
            semStack.pop();

            quater.push_back({ "do", conditionResult, "_", "_" }); // 循环体开始标记
            processingWhileCondition = false;
        }
    }

    bool isNumber(const std::string& s) {
        return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    int compute(const std::string& op, int operand1, int operand2) {
        if (op == "+") return operand1 + operand2;
        else if (op == "-") return operand1 - operand2;
        else if (op == "*") return operand1 * operand2;
        else if (op == "/") return operand2 != 0 ? operand1 / operand2 : 0;
        return 0;
    }

    std::string createTempVar() {
        return "@t" + std::to_string(tempVarCount++);
    }

    std::string createLabel() {
        return "L" + std::to_string(LabelCount++);
    }

    void identifyBasicBlocks() {
        std::unordered_set<int> leaders; // 基本块的入口点
        leaders.insert(0); // 第一个指令是入口点

        for (int i = 0; i < quater.size(); ++i) {
            const auto& instr = quater[i];
            if (instr.op == "ifFalse" || instr.op == "while" || instr.op == "elseif" || instr.op == "else") {
                if (i + 1 < quater.size()) {
                    leaders.insert(i + 1); // 条件跳转指令的下一个指令是入口点
                }
                if (!instr.result.empty()) {
                    // 目标指令是入口点
                    for (int j = 0; j < quater.size(); ++j) {
                        if (quater[j].result == instr.result) {
                            leaders.insert(j);
                            break;
                        }
                    }
                }
            }
        }

        BasicBlock currentBlock;
        for (int i = 0; i < quater.size(); ++i) {
            if (leaders.find(i) != leaders.end()) {
                if (!currentBlock.instructions.empty()) {
                    basicBlocks.push_back(currentBlock);
                    currentBlock = BasicBlock();
                }
            }
            currentBlock.addInstruction(quater[i]);
        }
        if (!currentBlock.instructions.empty()) {
            basicBlocks.push_back(currentBlock);
        }
    }

    const std::vector<BasicBlock>& getBasicBlocks() const {
        return basicBlocks;
    }

    void printQuaters() {
        for (const auto& q : quater) {
            std::cout << "(" << q.op << ", " << q.arg1 << ", " << q.arg2 << ", " << q.result << ")\n";
        }
    }

    void printBasicBlocks() {
        for (auto& block : basicBlocks) {
            block.print();
        }
    }
};

void optimizeBasicBlocks(std::vector<BasicBlock>& basicBlocks) {
    for (auto& block : basicBlocks) {
        //std::cout << "Optimizing basic block:" << std::endl;
        for (const auto& instr : block.instructions) {
            std::cout << "(" << instr.op << ", " << instr.arg1 << ", " << instr.arg2 << ", " << instr.result << ")" << std::endl;
        }
        std::unordered_map<std::string, Node*> dag = buildDAG(block.instructions);
        
        std::vector<Quater> optimizedQuads;
        //std::cout << "DAG Nodes before regenerating quadruples:" << std::endl;
        for (const auto& nodePair : dag) {
            Node* node = nodePair.second;
            //std::cout << "Node: " << node->value << ", op: " << node->op << ", children: ";
            // for (Node* child : node->children) {
            //     //std::cout << child->value << " ";
            // }
            //std::cout << std::endl;
        }
        regenerateQuadruplesFromDAG(dag, optimizedQuads, block.instructions);
        
        block.instructions = optimizedQuads;
    }
}

#endif // TEST_H