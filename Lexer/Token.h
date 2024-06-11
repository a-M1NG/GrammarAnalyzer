#include <string>
#include <vector>
#ifndef TOKEN_H
#define TOKEN_H
struct Token
{
    std::string type;
    std::string value;
    int line;
    int id;

    Token(std::string type, std::string value, int id, int line) : type(type), value(value), id(id), line(0) {}
    Token() : type(""), value(""), id(0), line(0) {}
};
using TokenList = std::vector<Token>;
/*
Token type:
"K" "P" "Con" "Ch" "St" "Arr" "I"
*/
#endif