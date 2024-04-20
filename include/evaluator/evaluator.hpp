#pragma once

#include "ast/_all.hpp"
#include "object/_all.hpp"
#include "parser/parser.hpp"

void arg_data_error();
void unhandled_error();
void program_error();
void node_type_error();
void args_size_error();
void invalid_constructor_error();
void data_type_error();
void data_value_error();
void not_found_error();
void more_type_error();
void not_mutable_error();
void type_different_error(std::string l, std::string r);
void extand_nothing_error(std::string name);
void lvl_type_error();
void conde_type_error();
void already_valid_error(std::string name);
void for_elem_error(std::string name);
class Evaluator {
public:
    Evaluator() {}
public:
    std::shared_ptr<Object> evaluate_program(std::shared_ptr<ProgramNode> _prog, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_constant(std::shared_ptr<ConstantNode> _constant);
    std::shared_ptr<Object> evaluate_region(std::shared_ptr<RegionNode> _region, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_function(std::shared_ptr<FunctionNode> _func, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_call(std::shared_ptr<CallNode> _call, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_array(std::shared_ptr<ArrayNode> _arr, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_assign(std::shared_ptr<AssignNode> _assign, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_class(std::shared_ptr<ClassNode> _class, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_if(std::shared_ptr<IfNode> _if, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_creation(std::shared_ptr<CreationNode> _cr, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_enumerate(std::shared_ptr<EnumerateNode> _enum, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_expr(std::shared_ptr<ExprNode> _expr, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_for(std::shared_ptr<ForNode> _for, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_id(std::shared_ptr<IdentifierNode> _id, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_indecrement(std::shared_ptr<InDecrementNode> _idc, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_index(std::shared_ptr<IndexNode> _idx, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_infix(std::shared_ptr<InfixNode> _infix, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_prefix(std::shared_ptr<PrefixNode> _prefix, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_remove(std::shared_ptr<RemoveNode> _rem, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_return(std::shared_ptr<ReturnNode> _ret, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_statement(std::shared_ptr<StatementNode> _stat, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_ternary(std::shared_ptr<TernaryNode> _ter, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_while(std::shared_ptr<WhileNode> _while, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_value(std::shared_ptr<Node> _val, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> evaluate_one(std::shared_ptr<Node> _stat, std::shared_ptr<Environment> env);
public:
    std::shared_ptr<Object> calcuate_infix(std::shared_ptr<Object> left, std::shared_ptr<Object> right, std::string op);
    bool isTrue(std::shared_ptr<Object> obj);
public:
    bool import_file(std::shared_ptr<ImportNode> _imp, std::shared_ptr<Environment> toAttach) {
        std::string filePath = _imp->item;
        std::shared_ptr<ProgramNode> _innerProg;
        std::ifstream is(filePath.substr(filePath.length() - 6) == ".precf" ? filePath : (filePath + ".precf"), std::ios::in | std::ios::binary);
        if (is.good()) {
            _innerProg = std::make_shared<ProgramNode>(nullptr);
            _innerProg->readFrom(is);
        }
        else {
            std::string _input_src = "", _input_line;
            std::ifstream is(filePath);
            if (!is.good()) {
                import_error();
                return true;
            }
            while (!is.eof()) {
                std::getline(is, _input_line);
                _input_src += "\n" + _input_line;
            }
            Parser parser(_input_src);
            auto _prog = parser.parse_program();
            if (_prog->type != Node::Type::Program) {
                import_error();
                return true;
            }
            _innerProg = std::dynamic_pointer_cast<ProgramNode>(_prog);
        }
        evaluate_program(_innerProg, toAttach);
        return false;
    }

    std::shared_ptr<Object> make_error() {
        return std::make_shared<Error>();
    }

    void bind_args(std::vector<std::shared_ptr<Object>> v, std::vector<std::string> name, std::string more, std::shared_ptr<Environment> env);
};