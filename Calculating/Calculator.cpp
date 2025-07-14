#include "Calculator.h"

int Calculator::precedence(char op){
    if(op == '+' || op == '-') return 1;
    if(op == '*' || op == '/') return 2;
    return 0;
}

// Применение оператора к двум операндам
int Calculator::applyOp(int a, int b, char op){
    switch(op){
        case '+': return a+b;
        case '-': return a-b;
        case '*': return a*b;
        case '/': 
            if(b == 0)
                throw std::invalid_argument("Division by zero");
            return a / b;
        default: throw std::runtime_error("Invalide operator");
    }
}

int Calculator::calculateExpression(const std::string& expr) {
    std::stack<int> values;
    std::stack<char> ops;
    for (size_t i = 0; i < expr.length(); ++i) {
        if (isdigit(expr[i])) {
            std::string numStr;
            while (i < expr.length() && isdigit(expr[i])) {
                numStr += expr[i];
                i++;
            }
            i--;
            int num = stoi(numStr);
            values.push(num);
        }
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            // Пока оператор в стеке имеет больший или равный приоритет, вычисляем
            while (!ops.empty() && precedence(ops.top()) >= precedence(expr[i])) {
                int b = values.top(); values.pop();
                int a = values.top(); values.pop();
                char op = ops.top(); ops.pop();
                values.push(applyOp(a, b, op));
            }
            ops.push(expr[i]);
        }
    }

    while (!ops.empty()) {
        int b = values.top(); values.pop();
        int a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(a, b, op));
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }
    return values.top();
}
