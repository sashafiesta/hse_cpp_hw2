#include <iostream>
#include <memory>
#include <cmath>
#include <string>
#include <unordered_map>
#include <complex>
#include <stack>
#include "symbolic.hpp"

double parse_number_real(const std::string& source) {
    if (source.empty()) {
        throw std::runtime_error("Number cannot be empty");
    }
    try {
        return (source.front() == '.') ? std::stod("0" + source) : std::stod(source);
    }
    catch(std::exception) {
        throw std::runtime_error("Wrong number format: " + source);
    }
}
std::complex<double> parse_number_complex(const std::string& source) {
    std::string copy = source;
    if (copy.empty()) {
        throw std::runtime_error("Number cannot be empty");
    }
    bool complex_flag = source.back() == 'i';
    if (complex_flag)
        copy.pop_back();
    size_t op_position = copy.size();
    for (size_t ptr = 1; ptr < copy.size(); ptr++) {
        if (copy[ptr] == '+' || copy[ptr] == '-') {
            op_position = ptr;
            break;
        }
    }
    if (op_position == copy.size() && complex_flag)
        return std::complex<double>(0, parse_number_real(copy));
    if (op_position == copy.size() && !complex_flag)
        return std::complex<double>(parse_number_real(copy), 0);
    if (op_position != copy.size() && complex_flag)
        return std::complex<double>(parse_number_real(copy.substr(0, op_position)), parse_number_real(copy.substr(op_position + 1)));
    throw std::runtime_error("Cannot create complex number without an operation");
}

std::shared_ptr<Expression<std::complex<double>>> parse(const std::string& source) {
    std::stringstream ss(source);
    std::string field;
    std::vector<std::string> split;
    while (true) {
        if (ss >> field)
            split.push_back(field);
        else if (ss.eof())
            break;
        ss.clear();
    }
    std::stack<std::shared_ptr<Expression<std::complex<double>>>> rpn;
    for (std::string lexem : split) {
        if (std::isdigit(lexem.front()) || lexem.front() == '.')
        {
            rpn.push(ConstExpression<std::complex<double>>(parse_number_complex(lexem)).clone());
            continue;
        }
        if (lexem == "+") {
            auto y = rpn.top(); rpn.pop();
            rpn.top() = (*rpn.top() + *y).clone();
            continue;
        }
        if (lexem == "-") {
            auto y = rpn.top(); rpn.pop();
            rpn.top() = (*rpn.top() - *y).clone();
            continue;
        }
        if (lexem == "*") {
            auto y = rpn.top(); rpn.pop();
            rpn.top() = (*rpn.top() * *y).clone();
            continue;
        }
        if (lexem == "/") {
            auto y = rpn.top(); rpn.pop();
            rpn.top() = (*rpn.top() / *y).clone();
            continue;
        }
        if (lexem == "/") {
            auto y = rpn.top(); rpn.pop();
            rpn.top() = (*rpn.top() / *y).clone();
            continue;
        }
        if (lexem == "^") {
            auto y = rpn.top(); rpn.pop();
            rpn.top() = (*rpn.top() ^ *y).clone();
            continue;
        }
        if (lexem == "sin") {
            rpn.top() = Expression<std::complex<double>>::sin(*rpn.top()).clone();
            continue;
        }
        if (lexem == "cos") {
            rpn.top() = Expression<std::complex<double>>::cos(*rpn.top()).clone();
            continue;
        }
        if (lexem == "ln") {
            rpn.top() = Expression<std::complex<double>>::ln(*rpn.top()).clone();
            continue;
        }
        if (lexem == "exp") {
            rpn.top() = Expression<std::complex<double>>::exp(*rpn.top()).clone();
            continue;
        }
        rpn.push(VariableExpression<std::complex<double>>(lexem).clone());
    }
    return rpn.top();
}
int main(int argc, char** argv) {
    std::string input;
    if (argc == 1) {
        std::cout << "Please input an expression in RPN form:\n";
        std::string inp;
        std::getline(std::cin, input);
    }
    else {
        for (int i = 1; i < argc; i++) {
            input += std::string(argv[i]);
            input += " ";
        }
        if (!input.empty())
            input.pop_back();
    }
    std::shared_ptr<Expression<std::complex<double>>> expr = parse(input);
    std::cout << "You entered:" << '\n' << expr->to_string() << '\n';
    std::cout << "Derivative:" << '\n' << expr->differentiate("x")->to_string() << '\n';
    return 0;
}
