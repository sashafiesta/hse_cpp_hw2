#include <iostream>
#include <memory>
#include <cmath>
#include <string>
#include <unordered_map>
#include <complex>
#include "symbolic.hpp"

int main2()
{
    std::cout << "Testing ConstExpression creation (2)...\n";
    auto e1 = ConstExpression<double>(2);
    std::cout << "Test PASSED\n";
    std::cout << "Testing VariableExpression creation (x)...\n";
    auto e2 = VariableExpression<double>("x");
    std::cout << "Test PASSED\n";

    std::cout << "Testing BinaryOperationExpression creation...\n";
    std::cout << "ADD: " << BinaryOperationExpression<double>(BinaryOperationExpression<double>::Type::ADD, e1, e2).to_string() << '\n';
    std::cout << "SUB: " << BinaryOperationExpression<double>(BinaryOperationExpression<double>::Type::SUB, e1, e2).to_string() << '\n';
    std::cout << "MUL: " << BinaryOperationExpression<double>(BinaryOperationExpression<double>::Type::MUL, e1, e2).to_string() << '\n';
    std::cout << "DIV: " << BinaryOperationExpression<double>(BinaryOperationExpression<double>::Type::DIV, e1, e2).to_string() << '\n';
    std::cout << "POW: " << BinaryOperationExpression<double>(BinaryOperationExpression<double>::Type::POW, e1, e2).to_string() << '\n';
    std::cout << "Test PASSED\n";

    std::cout << "Testing UnaryOperationExpression creation...\n";
    std::cout << "INV: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::INV, e2).to_string() << '\n';
    std::cout << "SIN: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::SIN, e2).to_string() << '\n';
    std::cout << "COS: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::COS, e2).to_string() << '\n';
    std::cout << "LOG: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::LOG, e2).to_string() << '\n';
    std::cout << "EXP: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::EXP, e2).to_string() << '\n';
    std::cout << "Test PASSED\n";

    std::cout << "Testing UnaryOperationExpression creation...\n";
    std::cout << "INV: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::INV, e2).to_string() << '\n';
    std::cout << "SIN: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::SIN, e2).to_string() << '\n';
    std::cout << "COS: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::COS, e2).to_string() << '\n';
    std::cout << "LOG: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::LOG, e2).to_string() << '\n';
    std::cout << "EXP: " << UnaryOperationExpression<double>(UnaryOperationExpression<double>::Type::EXP, e2).to_string() << '\n';
    std::cout << "Test PASSED\n";

    std::cout << "Testing Expression combination...\n";
    auto op = (e1 + e2 + (-e2) * (e1 / e2) - (e1 ^ e2)) + (-e2) * Expression<double>::sin(e2) * Expression<double>::cos(e2) * Expression<double>::exp(e2) * Expression<double>::ln(e2);
    std::cout << op.to_string() << '\n';
    std::cout << "Test PASSED\n";

    std::cout << "Testing differentiation...\n";
    std::cout << op.differentiate("x")->to_string() << '\n';
    std::cout << "Test PASSED\n";

    std::cout << "Testing substitution...\n";
    auto op2 = op + VariableExpression<double>("y");
    std::cout << op2.to_string() << '\n';
    std::cout << op2.substitute("y", 5)->to_string() << "\n";
    std::cout << "Test PASSED\n";

    std::cout << "Testing evaluation...\n";
    std::unordered_map<std::string, double> env; env["x"] = 1;
    std::cout << "eval[" << op.to_string() << "] = " << op.eval(env) << '\n';
    if (op.eval(env) == -1)
        std::cout << "Test PASSED\n";
    else 
        std::cout << "Test FAILED\n";
    return 0;
}