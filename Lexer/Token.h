#include <string>
#include <vector>
#ifndef TOKEN_H
#define TOKEN_H
struct Token
{
    std::string type;
    std::string value;
    int id;
};
using TokenList = std::vector<Token>;
/*
Token type:
"K" "P" "Con" "Ch" "St" "Arr" "I"
*/
#endif