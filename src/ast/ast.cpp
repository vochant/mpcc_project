#include "ast/array.hpp"
#include "ast/assign.hpp"
#include "ast/boolean.hpp"
#include "ast/break_continue.hpp"
#include "ast/call.hpp"
#include "ast/cfor.hpp"
#include "ast/class.hpp"
#include "ast/classmember.hpp"
#include "ast/constructor.hpp"
#include "ast/creation.hpp"
#include "ast/decorate.hpp"
#include "ast/destructor.hpp"
#include "ast/enumerate.hpp"
#include "ast/expr.hpp"
#include "ast/float.hpp"
#include "ast/for.hpp"
#include "ast/function.hpp"
#include "ast/group.hpp"
#include "ast/identifier.hpp"
#include "ast/if.hpp"
#include "ast/indecrement.hpp"
#include "ast/index.hpp"
#include "ast/infix.hpp"
#include "ast/integer.hpp"
#include "ast/new.hpp"
#include "ast/null.hpp"
#include "ast/prefix.hpp"
#include "ast/program.hpp"
#include "ast/remove.hpp"
#include "ast/return.hpp"
#include "ast/scope.hpp"
#include "ast/string.hpp"
#include "ast/ternary.hpp"
#include "ast/while.hpp"

#include "compiler_error.hpp"
#include "util.hpp"

Node::Node(Type type) : type(type) {}

ArrayNode::ArrayNode() : Node(Node::Type::Array) {}
AssignNode::AssignNode(std::string _op) : _op(_op), Node(Node::Type::Assign) {}
BooleanNode::BooleanNode(bool value) : value(value), Node(Node::Type::Boolean) {}
BreakContinueNode::BreakContinueNode(bool isContinue) : Node(Node::Type::BreakContinue), isContinue(isContinue) {}
CallNode::CallNode(std::shared_ptr<Node> to_run) : to_run(to_run), Node(Node::Type::Call) {}
CForNode::CForNode() : Node(Node::Type::CFor) {}
ClassNode::ClassNode() : Node(Node::Type::Class), _ext(":") {}
ClassMember::ClassMember(std::string name, std::shared_ptr<Node> inner) : Node(Node::Type::ClassMember), name(name), inner(inner) {}
ConstructorNode::ConstructorNode() : Node(Node::Type::Constructor) {}
CreationNode::CreationNode(bool isGlobal, bool allowOverwrite, bool isConst) : isGlobal(isGlobal), allowOverwrite(allowOverwrite), isConst(isConst), Node(Node::Type::Creation) {}
DecorateNode::DecorateNode() : Node(Node::Type::Decorate) {}
DestructorNode::DestructorNode() : Node(Node::Type::Destructor) {}
EnumerateNode::EnumerateNode() : Node(Node::Type::Enumerate) {}
ErrorNode::ErrorNode() : Node(Node::Type::Error) {}
ExprNode::ExprNode(std::shared_ptr<Node> inner) : inner(inner), Node(Node::Type::Expr) {}
FloatNode::FloatNode(double value) : value(value), Node(Node::Type::Float) {}
ForNode::ForNode() : Node(Node::Type::For) {}
FunctionNode::FunctionNode() : Node(Node::Type::Function), moreName("__null__") {}
GroupNode::GroupNode(std::shared_ptr<Node> v) : v(v), Node(Node::Type::Group) {}
IdentifierNode::IdentifierNode(std::string id) : id(id), Node(Node::Type::Identifier) {}
IfNode::IfNode() : Node(Node::Type::If) {}
InDecrementNode::InDecrementNode(std::shared_ptr<Node> body, bool isDecrement, bool isAfter) : body(body), isDecrement(isDecrement), isAfter(isAfter), Node(Node::Type::InDecrement) {}
IndexNode::IndexNode(std::shared_ptr<Node> left, std::shared_ptr<Node> index) : left(left), index(index), Node(Node::Type::Index) {}
InfixNode::InfixNode(std::string _op) : _op(_op), Node(Node::Type::Infix) {}
IntegerNode::IntegerNode(long long value) : value(value), Node(Node::Type::Integer) {}
NewNode::NewNode(std::string classname) : classname(classname), Node(Node::Type::New) {}
NullNode::NullNode() : Node(Node::Type::Null) {}
PrefixNode::PrefixNode(std::string _op) : _op(_op), Node(Node::Type::Prefix) {}
ProgramNode::ProgramNode(std::shared_ptr<Node> mainScope) : mainScope(mainScope), Node(Node::Type::Program) {}
RemoveNode::RemoveNode(std::string toRemove) : toRemove(toRemove), Node(Node::Type::Remove) {}
ReturnNode::ReturnNode(std::shared_ptr<Node> obj) : obj(obj), Node(Node::Type::Return) {}
ScopeNode::ScopeNode() : Node(Node::Type::Scope) {}
StringNode::StringNode(std::string value) : value(unescape(value)), Node(Node::Type::String) {}
TernaryNode::TernaryNode(std::shared_ptr<Node> _cond) : _cond(_cond), Node(Node::Type::Ternary) {}
WhileNode::WhileNode(bool isDoWhile) : isDoWhile(isDoWhile), Node(Node::Type::While) {}

inline void concatv(std::vector<std::shared_ptr<Asm>>& from, std::vector<std::shared_ptr<Asm>> v) {
    from.insert(from.end(), v.begin(), v.end());
}

std::vector<std::shared_ptr<Asm>> ArrayNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::MKARR));
    for (auto i : elements) {
        concatv(res, i->to_asm(args));
        res.push_back(std::make_shared<Asm>(Asm::Type::JARR));
    }
    return res;
}

std::vector<std::shared_ptr<Asm>> AssignNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, left->to_asm(args));
    if (_op != "=") {
        auto calc_node = std::make_shared<InfixNode>(_op.substr(0, _op.length() - 1));
        calc_node->left = left;
        calc_node->right = right;
        concatv(res, calc_node->to_asm(args));
    }
    else {
        concatv(res, right->to_asm(args));
    }
    res.push_back(std::make_shared<Asm>(Asm::Type::MOV));
    return res;
}

std::vector<std::shared_ptr<Asm>> BooleanNode::to_asm(ToAsmArgs args) const {
    if (value) {
        return { std::make_shared<Asm>(Asm::Type::TRUEV) };
    }
    else {
        return { std::make_shared<Asm>(Asm::Type::FALSEV) };
    }
}

std::vector<std::shared_ptr<Asm>> BreakContinueNode::to_asm(ToAsmArgs args) const {
    if (!args.isInRepeat) {
        throw CompilerError("Cannot use break/continue out of a repeat (for/while/dowhile)");
    }
    if (isContinue) {
        return { std::make_shared<IntegerAsm>(Asm::Type::JMP, args.next_pt) };
    }
    else {
        return { std::make_shared<IntegerAsm>(Asm::Type::JMP, args.end_pt) };
    }
}

std::vector<std::shared_ptr<Asm>> CallNode::to_asm(ToAsmArgs _args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::PCQ));
    size_t current = 0;
    for (size_t i = 0; i < args.size(); i++) {
        concatv(res, args[i]->to_asm(_args));
        if (current != expands.size() && expands[current] == i) {
            current++;
            res.push_back(std::make_shared<Asm>(Asm::Type::EARG));
        }
        else {
            res.push_back(std::make_shared<Asm>(Asm::Type::ARG));
        }
    }
    concatv(res, to_run->to_asm(_args));
    res.push_back(std::make_shared<Asm>(Asm::Type::CALL));
    res.push_back(std::make_shared<Asm>(Asm::Type::CLARG));
    return res;
}

std::vector<std::shared_ptr<Asm>> CForNode::to_asm(ToAsmArgs args) const {
    auto _args = args;
    size_t _Begin = (*args.flagcount)++;
    size_t _Next = (*args.flagcount)++;
    size_t _End = (*args.flagcount)++;
    _args.isInRepeat = true;
    _args.next_pt = _Next;
    _args.end_pt = _End;
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::BR));
    concatv(res, _init->to_asm(args));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Begin));
    concatv(res, _cond->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::IF));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JF, _End));
    concatv(res, _body->to_asm(_args));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Next));
    concatv(res, _next->to_asm(_args));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JMP, _Begin));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
    res.push_back(std::make_shared<Asm>(Asm::Type::ER));
    return res;
}

std::vector<std::shared_ptr<Asm>> ClassNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    if (_ext != ":") {
        res.push_back(std::make_shared<StringAsm>(Asm::Type::ATTR, _ext));
    }
    res.push_back(std::make_shared<Asm>(Asm::Type::CLASS));
    for (auto i : inner) {
        concatv(res, i->to_asm(args));
    }
    res.push_back(std::make_shared<StringAsm>(Asm::Type::ECLASS, _name));
    return res;
}

std::vector<std::shared_ptr<Asm>> ClassMember::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, inner->to_asm(args));
    if (ac == PUBLIC) {
        res.push_back(std::make_shared<Asm>(Asm::Type::PUB));
    }
    else {
        res.push_back(std::make_shared<Asm>(Asm::Type::PRIV));
    }
    if (isFinal) {
        res.push_back(std::make_shared<Asm>(Asm::Type::FINAL));
    }
    switch (type) {
    case METHOD:
        res.push_back(std::make_shared<StringAsm>(Asm::Type::METHOD, name));
        break;
    case VALUE:
        res.push_back(std::make_shared<StringAsm>(Asm::Type::VALUE, name));
        break;
    case STATIC:
        res.push_back(std::make_shared<StringAsm>(Asm::Type::STATIC, name));
        break;
    default:
        throw CompilerError("Unknown Class Member Type");
    }
    return res;
}

std::vector<std::shared_ptr<Asm>> ConstructorNode::to_asm(ToAsmArgs _args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::BFDC));
    for (auto[k, v] : typechecks) {
        res.push_back(std::make_shared<SIAsm>(Asm::Type::TREQ, k, v));
    }
    for (auto i : args) {
        res.push_back(std::make_shared<StringAsm>(Asm::Type::RECV, i));
    }
    concatv(res, inner->to_asm(_args));
    res.push_back(std::make_shared<Asm>(Asm::Type::NULLV));
    res.push_back(std::make_shared<Asm>(Asm::Type::SRVL));
    res.push_back(std::make_shared<Asm>(Asm::Type::RET));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::EFDC, argcount));
    return res;
}

std::vector<std::shared_ptr<Asm>> CreationNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    for (auto i : creations) {
        concatv(res, i.second->to_asm(args));
        if (isGlobal) res.push_back(std::make_shared<Asm>(Asm::Type::GLOBAL));
        if (isConst) res.push_back(std::make_shared<Asm>(Asm::Type::CONSTV));
        if (allowOverwrite) {
            res.push_back(std::make_shared<StringAsm>(Asm::Type::LET, i.first));
        }
        else {
            res.push_back(std::make_shared<StringAsm>(Asm::Type::VAR, i.first));
        }
        res.push_back(std::make_shared<Asm>(Asm::Type::CF));
    }
    return res;
}

std::vector<std::shared_ptr<Asm>> DecorateNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, inner->to_asm(args));
    concatv(res, decorator->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::ADC));
    return res;
}

std::vector<std::shared_ptr<Asm>> DestructorNode::to_asm(ToAsmArgs _args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::BFDD));
    concatv(res, inner->to_asm(_args));
    res.push_back(std::make_shared<Asm>(Asm::Type::NULLV));
    res.push_back(std::make_shared<Asm>(Asm::Type::SRVL));
    res.push_back(std::make_shared<Asm>(Asm::Type::RET));
    res.push_back(std::make_shared<Asm>(Asm::Type::EFDD));
    return res;
}

std::vector<std::shared_ptr<Asm>> EnumerateNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::ENUM));
    for (auto i : items) {
        res.push_back(std::make_shared<StringAsm>(Asm::Type::EENTRY, i));
    }
    res.push_back(std::make_shared<StringAsm>(Asm::Type::EENUM, _name));
    return res;
}

std::vector<std::shared_ptr<Asm>> ErrorNode::to_asm(ToAsmArgs args) const {
    return { std::make_shared<Asm>(Asm::Type::ERR) };
}

std::vector<std::shared_ptr<Asm>> ExprNode::to_asm(ToAsmArgs args) const {
    return inner->to_asm(args);
}

std::vector<std::shared_ptr<Asm>> FloatNode::to_asm(ToAsmArgs args) const {
    return { std::make_shared<FloatAsm>(Asm::Type::CFL, value) };
}

std::vector<std::shared_ptr<Asm>> ForNode::to_asm(ToAsmArgs args) const {
    auto _args = args;
    size_t _Begin = (*args.flagcount)++;
    size_t _Next = (*args.flagcount)++;
    size_t _End = (*args.flagcount)++;
    _args.isInRepeat = true;
    _args.next_pt = _Next;
    _args.end_pt = _End;
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, _elem->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::A2I));
    if (_var->type != Type::Identifier) {
        throw CompilerError("Common For Statement variable should be an identifer expression");
    }
    std::string _name = std::dynamic_pointer_cast<IdentifierNode>(_var)->id;
    res.push_back(std::make_shared<Asm>(Asm::Type::BR));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Begin));
    res.push_back(std::make_shared<Asm>(Asm::Type::IEND));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JT, _End));
    res.push_back(std::make_shared<Asm>(Asm::Type::IGET));
    res.push_back(std::make_shared<StringAsm>(Asm::Type::LET, _name));
    concatv(res, _body->to_asm(_args));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Next));
    res.push_back(std::make_shared<Asm>(Asm::Type::IINC));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JMP, _Begin));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
    res.push_back(std::make_shared<Asm>(Asm::Type::ER));
    res.push_back(std::make_shared<Asm>(Asm::Type::PNUL));
    return res;
}

std::vector<std::shared_ptr<Asm>> FunctionNode::to_asm(ToAsmArgs _args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::BFD));
    for (auto[k, v] : typechecks) {
        res.push_back(std::make_shared<SIAsm>(Asm::Type::TREQ, k, v));
    }
    for (auto i : args) {
        res.push_back(std::make_shared<StringAsm>(Asm::Type::RECV, i));
    }
    if (hasMore()) {
        res.push_back(std::make_shared<StringAsm>(Asm::Type::ERECV, moreName));
    }
    concatv(res, inner->to_asm(_args));
    res.push_back(std::make_shared<Asm>(Asm::Type::NULLV));
    res.push_back(std::make_shared<Asm>(Asm::Type::SRVL));
    res.push_back(std::make_shared<Asm>(Asm::Type::RET));
    res.push_back(std::make_shared<Asm>(Asm::Type::EFD));
    return res;
}

std::vector<std::shared_ptr<Asm>> GroupNode::to_asm(ToAsmArgs args) const {
    return v->to_asm(args);
}

std::vector<std::shared_ptr<Asm>> IdentifierNode::to_asm(ToAsmArgs args) const {
    return { std::make_shared<StringAsm>(Asm::Type::PUSH, id) };
}

std::vector<std::shared_ptr<Asm>> IfNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, _cond->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::IF));
    size_t _End = (*args.flagcount)++;
    if (_else) {
        size_t _Else = (*args.flagcount)++;
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JF, _Else));
        concatv(res, _then->to_asm(args));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JMP, _End));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Else));
        concatv(res, _else->to_asm(args));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
    }
    else {
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JF, _End));
        concatv(res, _then->to_asm(args));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
    }
    return res;
}

std::vector<std::shared_ptr<Asm>> InDecrementNode::to_asm(ToAsmArgs args) const  {
    if (isDecrement) {
        if (isAfter) {
            return { std::make_shared<Asm>(Asm::Type::ADEC) };
        }
        return { std::make_shared<Asm>(Asm::Type::BDEC) };
    }
    if (isAfter) {
        return { std::make_shared<Asm>(Asm::Type::AINC) };
    }
    return { std::make_shared<Asm>(Asm::Type::BINC) };
}

std::vector<std::shared_ptr<Asm>> IndexNode::to_asm(ToAsmArgs args) const  {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, left->to_asm(args));
    concatv(res, index->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::INDEX));
    return res;
}

std::vector<std::shared_ptr<Asm>> InfixNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    if (_op == "||") {
        size_t _End = (*args.flagcount)++;
        concatv(res, left->to_asm(args));
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JTK, _End));
        res.push_back(std::make_shared<Asm>(Asm::Type::PNUL));
        concatv(res, right->to_asm(args));
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
        return res;
    }
    if (_op == "&&") {
        size_t _End = (*args.flagcount)++;
        concatv(res, left->to_asm(args));
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JFK, _End));
        res.push_back(std::make_shared<Asm>(Asm::Type::PNUL));
        concatv(res, right->to_asm(args));
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
        return res;
    }
    if (_op == "." || _op == "::") {
        if (right->type != Type::Identifier) {
            throw CompilerError("Undefined grammar: object.[non-identifier] or object::[non-identifier]");
        }
        auto id = std::dynamic_pointer_cast<IdentifierNode>(right)->id;
        concatv(res, left->to_asm(args));
        if (_op == ".") {
            res.push_back(std::make_shared<StringAsm>(Asm::Type::ACCESS, id));
        }
        else {
            res.push_back(std::make_shared<StringAsm>(Asm::Type::FACS, id));
        }
        return res;
    }
    concatv(res, left->to_asm(args));
    concatv(res, right->to_asm(args));
    Asm::Type type;
    if (_op == "+") type = Asm::Type::ADD;
    else if (_op == "-") type = Asm::Type::SUB;
    else if (_op == "*") type = Asm::Type::MUL;
    else if (_op == "/") type = Asm::Type::DIV;
    else if (_op == "%") type = Asm::Type::MOD;
    else if (_op == "&") type = Asm::Type::BAND;
    else if (_op == "|") type = Asm::Type::BOR;
    else if (_op == "^") type = Asm::Type::BXOR;
    else if (_op == ">>") type = Asm::Type::SHR;
    else if (_op == "<<") type = Asm::Type::SHL;
    else if (_op == "==") type = Asm::Type::CEQ;
    else if (_op == "!=") type = Asm::Type::CNEQ;
    else if (_op == ">=") type = Asm::Type::CGE;
    else if (_op == "<=") type = Asm::Type::CLE;
    else if (_op == ">") type = Asm::Type::CG;
    else if (_op == "<") type = Asm::Type::CL;
    else throw CompilerError("Unknown Infix: " + _op);
    res.push_back(std::make_shared<Asm>(type));
    return res;
}

std::vector<std::shared_ptr<Asm>> IntegerNode::to_asm(ToAsmArgs args) const {
    return { std::make_shared<IntegerAsm>(Asm::Type::CINT, value) };
}

std::vector<std::shared_ptr<Asm>> NewNode::to_asm(ToAsmArgs _args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::PCQ));
    size_t current = 0;
    for (size_t i = 0; i < args.size(); i++) {
        concatv(res, args[i]->to_asm(_args));
        if (current != expands.size() && expands[current] == i) {
            current++;
            res.push_back(std::make_shared<Asm>(Asm::Type::EARG));
        }
        else {
            res.push_back(std::make_shared<Asm>(Asm::Type::ARG));
        }
    }
    res.push_back(std::make_shared<StringAsm>(Asm::Type::NEW, classname));
    res.push_back(std::make_shared<Asm>(Asm::Type::CLARG));
    return res;
}

std::vector<std::shared_ptr<Asm>> NullNode::to_asm(ToAsmArgs args) const  {
    return { std::make_shared<Asm>(Asm::Type::NULLV) };
}

std::vector<std::shared_ptr<Asm>> PrefixNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, right->to_asm(args));
    if (_op == "!") {
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
    }
    Asm::Type type;
    if (_op == "~") type = Asm::Type::BNOT;
    else if (_op == "-") type = Asm::Type::NEG;
    else if (_op == "+") type = Asm::Type::POS;
    else type = Asm::Type::NOT;
    res.push_back(std::make_shared<Asm>(type));
    return res;
}

std::vector<std::shared_ptr<Asm>> ProgramNode::to_asm(ToAsmArgs args) const {
    return mainScope->to_asm(args);
}

std::vector<std::shared_ptr<Asm>> RemoveNode::to_asm(ToAsmArgs args) const {
    return { std::make_shared<StringAsm>(Asm::Type::DEL, toRemove) };
}

std::vector<std::shared_ptr<Asm>> ReturnNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    concatv(res, obj->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::SRVL));
    res.push_back(std::make_shared<Asm>(Asm::Type::RET));
    return res;
}

std::vector<std::shared_ptr<Asm>> ScopeNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    res.push_back(std::make_shared<Asm>(Asm::Type::BR));
    for (auto i : statements) {
        concatv(res, i->to_asm(args));
        if (i->type == Type::Expr) {
            res.push_back(std::make_shared<Asm>(Asm::Type::PNUL));
        }
    }
    res.push_back(std::make_shared<Asm>(Asm::Type::ER));
    return res;
}

std::vector<std::shared_ptr<Asm>> StringNode::to_asm(ToAsmArgs args) const {
    return { std::make_shared<StringAsm>(Asm::Type::CSTR, value) };
}

std::vector<std::shared_ptr<Asm>> TernaryNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    size_t _Else = (*args.flagcount)++;
    size_t _End = (*args.flagcount)++;
    concatv(res, _cond->to_asm(args));
    res.push_back(std::make_shared<Asm>(Asm::Type::IF));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JF, _Else));
    concatv(res, _if->to_asm(args));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JMP, _End));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Else));
    concatv(res, _else->to_asm(args));
    res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
    return res;
}

std::vector<std::shared_ptr<Asm>> WhileNode::to_asm(ToAsmArgs args) const {
    std::vector<std::shared_ptr<Asm>> res;
    auto _args = args;
    size_t _Begin = (*args.flagcount)++;
    size_t _Next = (*args.flagcount)++;
    size_t _End = (*args.flagcount)++;
    _args.end_pt = _End;
    _args.next_pt = _Next;
    if (isDoWhile) {
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Begin));
        concatv(res, _body->to_asm(_args));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Next));
        concatv(res, _cond->to_asm(_args));
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JT, _Begin));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
        return res;
    }
    else {
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Begin));
        concatv(res, _cond->to_asm(_args));
        res.push_back(std::make_shared<Asm>(Asm::Type::IF));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JF, _End));
        concatv(res, _body->to_asm(_args));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _Next));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::JMP, _Begin));
        res.push_back(std::make_shared<IntegerAsm>(Asm::Type::FLAG, _End));
        return res;
    }
}