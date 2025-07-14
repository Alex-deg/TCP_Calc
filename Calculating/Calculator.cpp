#include "Calculator.h"

int Calculator::precedence(char op){
    if(op == '+' || op == '-') return 1;
    if(op == '*' || op == '/') return 2;
    return 0;
}

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

// Применение оператора к двум числам (целочисленное деление)

// Основная функция вычисления выражения
int Calculator::calculateExpression(const std::string& expr) {
    std::stack<int> values;
    std::stack<char> ops;
    for (size_t i = 0; i < expr.length(); ++i) {
        // Пропускаем пробелы
        if (expr[i] == ' ') continue;

        // Если текущий символ - цифра, читаем всё число
        else if (isdigit(expr[i])) {
            std::string numStr;
            while (i < expr.length() && isdigit(expr[i])) {
                numStr += expr[i];
                i++;
            }
            i--; // Возвращаемся на один символ назад

            int num = stoi(numStr);
            values.push(num);
        }
        // Если текущий символ - оператор
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

    // Вычисляем оставшиеся операции
    while (!ops.empty()) {
        int b = values.top(); values.pop();
        int a = values.top(); values.pop();
        char op = ops.top(); ops.pop();
        values.push(applyOp(a, b, op));
    }

    // Результат - единственное оставшееся число в стеке
    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }
    return values.top();
}
