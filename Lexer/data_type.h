#include <string>
#include <vector>
#include <iostream>
#ifndef DATA_TYPE_H
#define DATA_TYPE_H
struct Token
{
    std::string type;
    std::string value;
    int line;
    int id;

    Token(std::string type, std::string value, int id, int line) : type(type), value(value), id(id), line(line) {}
    Token() : type(""), value(""), id(0), line(0) {}
    friend std::ostream &operator<<(std::ostream &os, const Token &token)
    {
        os << "(Token:" << "type: " << token.type << " value: " << token.value << " id: " << token.id << ")";
        return os;
    }
};

using TokenList = std::vector<Token>;

struct msg
{
    std::string value;
    int line;
    std::string error;
};

struct Quater
{
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

/*
Token type:
"K" "P" "Con" "Ch" "St" "Arr" "I"
*/
#endif