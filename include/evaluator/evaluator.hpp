#pragma once

#include "ast/_all.hpp"
#include "object/_all.hpp"
#include "parser/parser.hpp"

void arg_data_error();
void unhandled_error(std::string reason);
void program_error();
void node_type_error();
void args_size_error(std::string msg);
void invalid_constructor_error();
void data_type_error(std::string msg);
void data_value_error();
void not_found_error(std::string name);
void more_type_error(std::string msg);
void not_mutable_error();
void type_different_error(std::string l, std::string r);
void extand_nothing_error(std::string name);
void lvl_type_error();
void conde_type_error();
void already_valid_error(std::string name);
void for_elem_error(std::string name);
void idc_error(std::string name);
void out_of_range_error(std::string text);
void invalid_error(std::string name);
void sub_error(std::string name);
class Evaluator {
public:
    Evaluator() {}
public:
    static std::shared_ptr<Object> evaluate_program(std::shared_ptr<ProgramNode> _prog, Environment* env);
    static std::shared_ptr<Object> evaluate_constant(std::shared_ptr<ConstantNode> _constant);
    static std::shared_ptr<Object> evaluate_region(std::shared_ptr<RegionNode> _region, Environment* env, bool no_isolate = true);
    static std::shared_ptr<Object> evaluate_function(std::shared_ptr<FunctionNode> _func, Environment* env);
    static std::shared_ptr<Object> evaluate_call(std::shared_ptr<CallNode> _call, Environment* env);
    static std::shared_ptr<Object> evaluate_array(std::shared_ptr<ArrayNode> _arr, Environment* env);
    static std::shared_ptr<Object> evaluate_assign(std::shared_ptr<AssignNode> _assign, Environment* env);
    static std::shared_ptr<Object> evaluate_class(std::shared_ptr<ClassNode> _class, Environment* env);
    static std::shared_ptr<Object> evaluate_if(std::shared_ptr<IfNode> _if, Environment* env);
    static std::shared_ptr<Object> evaluate_creation(std::shared_ptr<CreationNode> _cr, Environment* env);
    static std::shared_ptr<Object> evaluate_enumerate(std::shared_ptr<EnumerateNode> _enum, Environment* env);
    static std::shared_ptr<Object> evaluate_expr(std::shared_ptr<ExprNode> _expr, Environment* env);
    static std::shared_ptr<Object> evaluate_for(std::shared_ptr<ForNode> _for, Environment* env);
    static std::shared_ptr<Object> evaluate_id(std::shared_ptr<IdentifierNode> _id, Environment* env);
    static std::shared_ptr<Object> evaluate_indecrement(std::shared_ptr<InDecrementNode> _idc, Environment* env);
    static std::shared_ptr<Object> evaluate_index(std::shared_ptr<IndexNode> _idx, Environment* env);
    static std::shared_ptr<Object> evaluate_infix(std::shared_ptr<InfixNode> _infix, Environment* env);
    static std::shared_ptr<Object> evaluate_prefix(std::shared_ptr<PrefixNode> _prefix, Environment* env);
    static std::shared_ptr<Object> evaluate_remove(std::shared_ptr<RemoveNode> _rem, Environment* env);
    static std::shared_ptr<Object> evaluate_return(std::shared_ptr<ReturnNode> _ret, Environment* env);
    static std::shared_ptr<Object> evaluate_statement(std::shared_ptr<StatementNode> _sta, Environment* env);
    static std::shared_ptr<Object> evaluate_ternary(std::shared_ptr<TernaryNode> _ter, Environment* env);
    static std::shared_ptr<Object> evaluate_while(std::shared_ptr<WhileNode> _while, Environment* env);
    static std::shared_ptr<Object> evaluate_value(std::shared_ptr<Node> _val, Environment* env);
    static std::shared_ptr<Object> evaluate_one(std::shared_ptr<Node> _sta, Environment* env);
public:
	static bool check_class_relationship(std::shared_ptr<Object> a, std::shared_ptr<Object> b);
    static std::shared_ptr<Object> calcuate_infix(std::shared_ptr<Object> left, std::shared_ptr<Object> right, std::string op);
	static std::shared_ptr<Object> calcuate_prefix(std::shared_ptr<Object> body, std::string op);
	static std::shared_ptr<Integer> calcuate_infix_integer(std::shared_ptr<Integer> left, std::shared_ptr<Integer> right, std::string op);
	static std::shared_ptr<Float> calcuate_infix_float(std::shared_ptr<Float> left, std::shared_ptr<Float> right, std::string op);
	static std::shared_ptr<String> calcuate_infix_string(std::shared_ptr<String> left, std::shared_ptr<String> right, std::string op);
	static std::shared_ptr<Boolean> calcuate_infix_boolean(std::shared_ptr<Boolean> left, std::shared_ptr<Boolean> right, std::string op);
	static std::shared_ptr<Object> calcuate_infix_array(std::shared_ptr<Array> left, std::shared_ptr<Array> right, std::string op);
    static bool isTrue(std::shared_ptr<Object> obj);
public:
    static bool import_file(std::shared_ptr<ImportNode> _imp, Environment* toAttach) {
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

    static std::shared_ptr<Object> make_error() {
        return std::make_shared<Error>();
    }

    static void bind_args(std::vector<std::shared_ptr<Object>> v, std::vector<std::string> name, std::string more, std::shared_ptr<Environment> env);
	static void bind_args(std::vector<std::shared_ptr<Object>> v, std::vector<std::string> name, std::string more, Environment* env);
};