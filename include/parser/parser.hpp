#pragma once

#include "ast/base/node.hpp"
#include "object/base.hpp"
#include "lexer/lexer.hpp"

void unknown_pre_expr_error();
void unknown_in_expr_error();
void unhandled_compiler_error();
void end_not_correct_error();
void format_error(std::string msg);
void invalid_number_error();
void unknown_infix_error();
void unknown_prefix_error();
void unknown_assign_error();
void import_error();

class Parser {
private:
    enum class OperatorPriority {
        Lowest, Assign, LogicalOr, LogicalAnd, BitwiseOr, BitwiseXor, BitwiseAnd,
        Equals, LessGreater, BitwiseMovement, Sum, Product, Single, Suffix
    };
    std::shared_ptr<Token> _current, _prev;
    Lexer lexer;
    void parse_token() {
        _prev = _current;
        _current = lexer.parseNext();
    }
public:
    Parser(const std::string code) : lexer(code) {
        parse_token();
    }
    std::shared_ptr<Node> parse_program();
private:
    std::shared_ptr<Node> import_file(std::string filePath);
    std::shared_ptr<Node> parse_statement();
    std::shared_ptr<Node> parse_scope();
    std::shared_ptr<Node> parse_expr();
    std::shared_ptr<Node> parse_expr_level(OperatorPriority pri);
    std::shared_ptr<Node> parse_if();
    std::shared_ptr<Node> parse_for();
    std::shared_ptr<Node> parse_while();
    std::shared_ptr<Node> parse_function();
    std::shared_ptr<Node> parse_class_creation();
    std::shared_ptr<Node> parse_enumerate_creation();
    std::shared_ptr<Node> parse_creation();
    std::shared_ptr<Node> parse_number();
    std::shared_ptr<Node> parse_string();
    std::shared_ptr<Node> parse_boolean();
    std::shared_ptr<Node> parse_array();
    std::shared_ptr<Node> parse_return();
    std::shared_ptr<Node> parse_infix(std::shared_ptr<Node> left);
    std::shared_ptr<Node> parse_prefix();
    std::shared_ptr<Node> parse_remove();
    std::shared_ptr<Node> parse_import();
    void parse_import_now(std::shared_ptr<ScopeNode> defr);
    std::shared_ptr<Node> parse_assign(std::shared_ptr<Node> left);
    std::shared_ptr<Node> parse_ternary(std::shared_ptr<Node> cond);
    std::shared_ptr<Node> parse_group();
    std::shared_ptr<Node> parse_call(std::shared_ptr<Node> left);
    std::shared_ptr<Node> parse_index(std::shared_ptr<Node> left);
    std::shared_ptr<Node> parse_identifier();
    std::shared_ptr<Node> parse_in_decrement_before();
    std::shared_ptr<Node> parse_named_constant();
    std::shared_ptr<Node> parse_in_decrement_after(std::shared_ptr<Node> left);
    std::shared_ptr<Node> parse_function_creation();
private:
    std::shared_ptr<Node> make_error();
    std::map<Token::Type, OperatorPriority> priorityTable = {
        {Token::Type::Assign,                 OperatorPriority::Assign},
        {Token::Type::AsteriskAssign,         OperatorPriority::Assign},
        {Token::Type::BitwiseAndAssign,     OperatorPriority::Assign},
        {Token::Type::BitwiseLeftAssign,     OperatorPriority::Assign},
        {Token::Type::BitwiseOrAssign,         OperatorPriority::Assign},
        {Token::Type::BitwiseRightAssign,     OperatorPriority::Assign},
        {Token::Type::BitwiseXorAssign,     OperatorPriority::Assign},
        {Token::Type::MinusAssign,             OperatorPriority::Assign},
        {Token::Type::ModulusAssign,         OperatorPriority::Assign},
        {Token::Type::PlusAssign,             OperatorPriority::Assign},
        {Token::Type::SlashAssign,             OperatorPriority::Assign},
        {Token::Type::Ternary,                OperatorPriority::Assign},

        {Token::Type::LogicalOr,             OperatorPriority::LogicalOr},
        {Token::Type::LogicalAnd,             OperatorPriority::LogicalAnd},
        {Token::Type::BitwiseOr,             OperatorPriority::BitwiseOr},
        {Token::Type::BitwiseXor,             OperatorPriority::BitwiseXor},
        {Token::Type::BitwiseAnd,             OperatorPriority::BitwiseAnd},
        
        {Token::Type::Equal,                 OperatorPriority::Equals},
        {Token::Type::NotEqual,             OperatorPriority::Equals},
        {Token::Type::Greater,                 OperatorPriority::LessGreater},
        {Token::Type::GreaterEqual,         OperatorPriority::LessGreater},
        {Token::Type::Less,                    OperatorPriority::LessGreater},
        {Token::Type::LessEqual,            OperatorPriority::LessGreater},

        {Token::Type::BitwiseLeft,            OperatorPriority::BitwiseMovement},
        {Token::Type::BitwiseRight,            OperatorPriority::BitwiseMovement},
        {Token::Type::Minus,                OperatorPriority::Sum},
        {Token::Type::Plus,                    OperatorPriority::Sum},
        {Token::Type::Asterisk,                OperatorPriority::Product},
        {Token::Type::Modulus,                OperatorPriority::Product},
        {Token::Type::Slash,                OperatorPriority::Product},

        {Token::Type::LogicalNot,            OperatorPriority::Single},
        {Token::Type::BitwiseNot,            OperatorPriority::Single},
        {Token::Type::LBracket,                OperatorPriority::Suffix},
        {Token::Type::LParan,                OperatorPriority::Suffix},
        {Token::Type::Extand,                 OperatorPriority::Suffix}
    };
    bool pricmp(OperatorPriority a, OperatorPriority b) {
        if (a > b) {
            return false;
        }
        if (a < b) {
            return true;
        }
        return a == OperatorPriority::Assign;
    }
    OperatorPriority getpri(Token::Type type) {
        auto it = priorityTable.find(type);
        if (it == priorityTable.end()) {
            return OperatorPriority::Lowest;
        }
        return it->second;
    }
    std::shared_ptr<Node> lookupIn(Token::Type type, std::shared_ptr<Node> left) {
        switch (type) {
        case Token::Type::Assign:
        case Token::Type::AsteriskAssign:
        case Token::Type::BitwiseAndAssign:
        case Token::Type::BitwiseLeftAssign:
        case Token::Type::BitwiseOrAssign:
        case Token::Type::BitwiseRightAssign:
        case Token::Type::BitwiseXorAssign:
        case Token::Type::MinusAssign:
        case Token::Type::ModulusAssign:
        case Token::Type::PlusAssign:
        case Token::Type::SlashAssign:
            return parse_assign(left);
        case Token::Type::Asterisk:
        case Token::Type::BitwiseAnd:
        case Token::Type::BitwiseLeft:
        case Token::Type::BitwiseOr:
        case Token::Type::BitwiseRight:
        case Token::Type::BitwiseXor:
        case Token::Type::Minus:
        case Token::Type::Modulus:
        case Token::Type::Plus:
        case Token::Type::Slash:
        case Token::Type::LogicalAnd:
        case Token::Type::LogicalOr:
        case Token::Type::Equal:
        case Token::Type::NotEqual:
        case Token::Type::Less:
        case Token::Type::LessEqual:
        case Token::Type::Greater:
        case Token::Type::GreaterEqual:
        case Token::Type::Extand:
            return parse_infix(left);
        case Token::Type::LParan:
            return parse_call(left);
        case Token::Type::LBracket:
            return parse_index(left);
        case Token::Type::Increment:
        case Token::Type::Decrement:
            return parse_in_decrement_after(left);
        case Token::Type::Ternary:
            return parse_ternary(left);
        default:
            unknown_in_expr_error();
            return make_error();
        }
    }
    std::shared_ptr<Node> lookupPre(Token::Type type) {
        switch (type) {
        case Token::Type::Integer:
        case Token::Type::Float:
            return parse_number();
        case Token::Type::False:
        case Token::Type::True:
            return parse_boolean();
        case Token::Type::Identifier:
            return parse_identifier();
        case Token::Type::Lambda:
        case Token::Type::Function:
            return parse_function();
        case Token::Type::LBracket:
            return parse_array();
        case Token::Type::Null:
            return parse_named_constant();
        case Token::Type::String:
            return parse_string();
        case Token::Type::LParan:
            return parse_group();
        case Token::Type::Plus:
        case Token::Type::Minus:
        case Token::Type::LogicalNot:
        case Token::Type::BitwiseNot:
            return parse_prefix();
        case Token::Type::Increment:
        case Token::Type::Decrement:
            return parse_in_decrement_before();
        default:
            unknown_pre_expr_error();
            return make_error();
        }
    }

    bool shouldEnd() {
        switch (_current->type) {
        case Token::Type::End:
        case Token::Type::Semicolon:
        case Token::Type::RBrace:
            return true;
        default:
            return false;
        }
    }
};