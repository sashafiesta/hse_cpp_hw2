#include <iostream>
#include <memory>
#include <cmath>
#include <string>
#include <unordered_map>
#include <complex>
template <typename T> class BinaryOperationExpression;
template <typename T> class UnaryOperationExpression;
template <typename T> class VariableOperationExpression;
template <typename T> class ConstantExpression;


template <typename T>
class Expression {
public:
    virtual ~Expression() = default;
    virtual T eval(const std::unordered_map<std::string, T>& environment) const = 0;
    virtual std::string to_string() const = 0;
    virtual std::shared_ptr<Expression<T>> clone() const = 0;
    virtual std::shared_ptr<Expression<T>> substitute(const std::string& variable, T val) const = 0;
    virtual std::shared_ptr<Expression<T>> differentiate(const std::string& variable, bool& contains_variable) const = 0;
    virtual std::shared_ptr<Expression<T>> differentiate(const std::string& variable) const = 0;
    BinaryOperationExpression<T> operator+(const Expression<T>& other) const {
        return BinaryOperationExpression<T>(BinaryOperationExpression<T>::Type::ADD, *this, other);
    }
    BinaryOperationExpression<T> operator-(const Expression<T>& other) const {
        return BinaryOperationExpression<T>(BinaryOperationExpression<T>::Type::SUB, *this, other);
    }
    BinaryOperationExpression<T> operator*(const Expression<T>& other) const {
        return BinaryOperationExpression<T>(BinaryOperationExpression<T>::Type::MUL, *this, other);
    }
    BinaryOperationExpression<T> operator/(const Expression<T>& other) const {
        return BinaryOperationExpression<T>(BinaryOperationExpression<T>::Type::DIV, *this, other);
    }
    BinaryOperationExpression<T> operator^(const Expression<T>& other) const {
        return BinaryOperationExpression<T>(BinaryOperationExpression<T>::Type::POW, *this, other);
    }

    UnaryOperationExpression<T> operator-() const {
        return UnaryOperationExpression<T>(UnaryOperationExpression<T>::Type::INV, *this);
    }
    static UnaryOperationExpression<T> sin(const Expression<T>& other) {
        return UnaryOperationExpression<T>(UnaryOperationExpression<T>::Type::SIN, other);
    }
    static UnaryOperationExpression<T> cos(const Expression<T>& other) {
        return UnaryOperationExpression<T>(UnaryOperationExpression<T>::Type::COS, other);
    }
    static UnaryOperationExpression<T> exp(const Expression<T>& other) {
        return UnaryOperationExpression<T>(UnaryOperationExpression<T>::Type::EXP, other);
    }
    static UnaryOperationExpression<T> ln(const Expression<T>& other) {
        return UnaryOperationExpression<T>(UnaryOperationExpression<T>::Type::LOG, other);
    }
};
template <typename T>
class ConstExpression : public Expression<T> {
public:
    T value;
    ~ConstExpression() = default;
    T eval(const std::unordered_map<std::string, T>& environment) const override {
        return value;
    }
    ConstExpression(T val)
        : value(val) {}

    std::string to_string() const {
        return to_string_modified(value);
    }
    std::shared_ptr<Expression<T>> clone() const override {
        return std::make_shared<ConstExpression<T>>(*this);
    }
    std::shared_ptr<Expression<T>> substitute(const std::string& variable, T val) const override {
        return clone();
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable, bool& contains_variable) const override {
        contains_variable = false;
        return std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>(0));
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable) const override {
        bool cv = false;
        return differentiate(variable, cv);
    }
private:
    static std::string to_string_modified(const T num) {
        std::ostringstream oss;
        oss << num;
        return oss.str();
    }
    template<typename Y>
    static std::string to_string_modified(const std::complex<Y> num) {
        std::ostringstream oss;
        oss << num.real();
        if (num.imag() < 0)
            oss << "-";
        else
            oss << "+";
        oss << std::abs(num.imag()) << "i";
        return oss.str();
    }
    template<typename Y>
    bool is_complex(const Y&) {
        return false;
    }

};
template <typename T>
class UnaryOperationExpression : public Expression<T> {
public:
    enum Type { INV, EXP, SIN, COS, LOG };
    Type type;
    std::shared_ptr<Expression<T>> operand;
    ~UnaryOperationExpression() = default;
    T eval(const std::unordered_map<std::string, T>& environment) const override {
        switch (type) {
        case INV: return -operand->eval(environment);
        case EXP: return std::exp(operand->eval(environment));
        case SIN: return std::sin(operand->eval(environment));
        case COS: return std::cos(operand->eval(environment));
        case LOG: return std::log(operand->eval(environment));
        default: throw std::runtime_error("Unknown unary operation");
        };
    }
    UnaryOperationExpression(Type op_type, const Expression<T>& expr)
        : type(op_type),
        operand(expr.clone()) {}
    std::string to_string() const override {
        switch (type) {
        case INV: return "(-" + operand->to_string() + ")";
        case EXP: return "exp(" + operand->to_string() + ")";
        case SIN: return "sin(" + operand->to_string() + ")";
        case COS: return "cos(" + operand->to_string() + ")";
        case LOG: return "log(" + operand->to_string() + ")";
        default: throw std::runtime_error("Unknown unary operation");
        };
    }
    std::shared_ptr<Expression<T>> clone() const override {
        return std::make_shared<UnaryOperationExpression<T>>(*this);
    }
    std::shared_ptr<Expression<T>> substitute(const std::string& variable, T val) const override {
        return std::make_shared<UnaryOperationExpression<T>>(type, *operand->substitute(variable, val));
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable, bool& contains_variable) const override {
        auto diff = operand->differentiate(variable, contains_variable);
        if (!contains_variable)
            return std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>(0));
        switch (type) {
        case INV: return std::make_shared<UnaryOperationExpression<T>>(-*diff);
        case EXP: return std::make_shared<BinaryOperationExpression<T>>(UnaryOperationExpression<T>(EXP, *operand) * (*diff));
        case SIN: return std::make_shared<BinaryOperationExpression<T>>(UnaryOperationExpression<T>(COS, *operand) * (*diff));
        case COS: return std::make_shared<BinaryOperationExpression<T>>((-UnaryOperationExpression<T>(SIN, *operand)) * (*diff));
        case LOG: return std::make_shared<BinaryOperationExpression<T>>(((ConstExpression<T>(1) / *operand)) * (*diff));
        default: throw std::runtime_error("Unknown unary operation");
        };
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable) const override {
        bool cv = false;
        return differentiate(variable, cv);
    }
};
template <typename T>
class BinaryOperationExpression : public Expression<T> {
public:
    enum Type { ADD, SUB, MUL, DIV, POW };
    Type type;
    std::shared_ptr<Expression<T>> operand_left, operand_right;
    ~BinaryOperationExpression() = default;
    T eval(const std::unordered_map<std::string, T>& environment) const override {
        switch (type) {
        case ADD: return operand_left->eval(environment) + operand_right->eval(environment);
        case SUB: return operand_left->eval(environment) - operand_right->eval(environment);
        case MUL: return operand_left->eval(environment) * operand_right->eval(environment);
        case DIV: return operand_left->eval(environment) / operand_right->eval(environment);
        case POW: return std::exp(operand_right->eval(environment) * std::log(operand_left->eval(environment)));
        default: throw std::runtime_error("Unknown binary operation");
        }
    }
    BinaryOperationExpression(Type op_type, const Expression<T>& expr_left, const Expression<T>& expr_right)
        : type(op_type),
        operand_left(expr_left.clone()),
        operand_right(expr_right.clone()) {}
    std::string to_string() const override {
        switch (type) {
        case ADD: return "(" + operand_left->to_string() + " + " + operand_right->to_string() + ")";
        case SUB: return "(" + operand_left->to_string() + " - " + operand_right->to_string() + ")";
        case MUL: return "(" + operand_left->to_string() + " * " + operand_right->to_string() + ")";
        case DIV: return "(" + operand_left->to_string() + " / " + operand_right->to_string() + ")";
        case POW: return "(" + operand_left->to_string() + " ^ " + operand_right->to_string() + ")";
        default: throw std::runtime_error("Unknown binary operation");
        };
    }
    std::shared_ptr<Expression<T>> clone() const override {
        return std::make_shared<BinaryOperationExpression<T>>(*this);
    }

    std::shared_ptr<Expression<T>> substitute(const std::string& variable, T val) const override {
        return std::make_shared<BinaryOperationExpression<T>>(type, *operand_left->substitute(variable, val), *operand_right->substitute(variable, val));
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable, bool& contains_variable) const override {
        bool cv_l = false;
        auto diff_l = operand_left->differentiate(variable, cv_l);
        bool cv_r = false;
        auto diff_r = operand_right->differentiate(variable, cv_r);
        contains_variable = cv_l || cv_r;
        if (!contains_variable)
            return std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>(0));
        switch (type) {
        case ADD: return std::make_shared<BinaryOperationExpression<T>>(*diff_l + *diff_r);
        case SUB: return std::make_shared<BinaryOperationExpression<T>>(*diff_l - *diff_r);
        case MUL: return std::make_shared<BinaryOperationExpression<T>>(((*diff_l) * (*operand_right)) + ((*operand_left) * (*diff_r)));
        case DIV: return std::make_shared<BinaryOperationExpression<T>>((((*diff_l) * (*operand_left)) - ((*operand_left) * (*diff_r))) / ((*operand_right) * (*operand_right)));
        case POW: return std::make_shared<BinaryOperationExpression<T>>(((*operand_right) * ((*operand_left) ^ (*operand_right - ConstExpression<T>(1))) * (*diff_l)) - ((((*operand_left) ^ (*operand_right)) * (*diff_r)) * UnaryOperationExpression<T>(UnaryOperationExpression<T>::Type::LOG, *operand_left)));
        default: throw std::runtime_error("Unknown unary operation");
        };
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable) const override {
        bool cv = false;
        return differentiate(variable, cv);
    }
};

template <typename T>
class VariableExpression : public Expression<T> {
public:
    std::string name;
    ~VariableExpression() = default;
    virtual T eval(const std::unordered_map<std::string, T>& environment)  const override {
        if (environment.find(name) == environment.end())
            throw std::runtime_error("Unknown variable: " + name);
        else
            return environment.find(name)->second;
    }
    VariableExpression(const std::string& var_name)
        : name(var_name) {};
    std::string to_string()  const override {
        return name;
    }
    std::shared_ptr<Expression<T>> clone() const override {
        return std::make_shared<VariableExpression<T>>(*this);
    }
    std::shared_ptr<Expression<T>> substitute(const std::string& variable, T val) const override {
        if (variable == name)
            return std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>(val));
        else
            return clone();
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable, bool& contains_variable) const override {
        contains_variable = (name == variable);
        //return (contains_variable) ? std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>(1)) : std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>(0));
        return std::shared_ptr<Expression<T>>(std::make_shared<ConstExpression<T>>((contains_variable) ? 1 : 0));
    }
    std::shared_ptr<Expression<T>> differentiate(const std::string& variable) const override {
        bool cv = false;
        return differentiate(variable, cv);
    }
};
