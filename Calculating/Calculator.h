#pragma once

#include <string>
#include <vector>
#include <stack>
#include <stdexcept>

class Calculator{
public:
    static int calculateExpression(const std::string& expr);
    static int precedence(char op);
    static int applyOp(int a, int b, char op);
};