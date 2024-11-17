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

#include "parser/parser.hpp"
#include "parser_error.hpp"

constexpr bool doImport = true;

std::string noNext() {
    throw ParserError("Read end-of-file, but not ended");
}

void Parser::parse_token() {
    _prev = _current;
    _current = lexer.parseNext();
    if (_current->type == Token::Type::LParan || _current->type == Token::Type::LBrace || _current->type == Token::Type::LBracket) stacking++;
    if (_current->type == Token::Type::RParan || _current->type == Token::Type::RBrace || _current->type == Token::Type::RBracket) stacking--;
    if (_current->type == Token::Type::End && stacking) {
        auto bprev = _prev;
        lexer = Lexer(_getNext(), source);
        parse_token(); 
        _prev = bprev;
    }
    // std::cout << "Got " << _current->value << "\n";
}

Parser::Parser(const std::string code, const std::string src, std::function<std::string()> _getNext) : lexer(code, src), _getNext(_getNext) {
    stacking = 0;
    source = src;
    parse_token();
}

bool Parser::pricmp(OperatorPriority a, OperatorPriority b) {
    if (a > b) {
        return false;
    }
    if (a < b) {
        return true;
    }
    return a == OperatorPriority::Assign || a == OperatorPriority::Pow;
}

std::shared_ptr<Node> Parser::lookupIn(Token::Type type, std::shared_ptr<Node> left) {
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
    case Token::Type::ForceExtand:
    case Token::Type::Pow:
    case Token::Type::FullEqual:
    case Token::Type::NotFullEqual:
    case Token::Type::More:
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
        return nullptr;
    }
}

bool Parser::shouldEnd() {
    switch (_current->type) {
    case Token::Type::End:
    case Token::Type::Semicolon:
    case Token::Type::RBrace:
    case Token::Type::RBracket:
    case Token::Type::RParan:
    case Token::Type::As:
        return true;
    default:
        return false;
    }
}

std::shared_ptr<Node> Parser::lookupPre(Token::Type type) {
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
    case Token::Type::Func:
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
    case Token::Type::Decorate:
        return parse_decorate();
    case Token::Type::New:
        return parse_class_new();
    default:
        throw ParserError("Unknown prefix operator", &lexer);
    }
}

Parser::OperatorPriority Parser::getpri(Token::Type type) {
    auto it = priorityTable.find(type);
    if (it == priorityTable.end()) {
        return OperatorPriority::Lowest;
    }
    return it->second;
}

std::shared_ptr<Node> Parser::parse_program() {
    auto scope = std::make_shared<ScopeNode>();
    while (_current->type != Token::Type::End) {
        if (_current->type == Token::Type::Import && doImport) {
            parse_import_now(scope);
            continue;
        }
        scope->statements.push_back(parse_statement());
    }
    parse_token();
    return std::make_shared<ProgramNode>(scope);
}

std::shared_ptr<Node> Parser::parse_statement() {
    switch (_current->type) {
    case Token::Type::LBrace:
        return parse_scope();
    case Token::Type::FunctionDef:
        return parse_function_creation();
    case Token::Type::Class:
        return parse_class_creation();
    case Token::Type::Const:
    case Token::Type::Let:
    case Token::Type::Var:
    case Token::Type::Global:
        return parse_creation();
    case Token::Type::If:
        return parse_if();
    case Token::Type::Return:
        return parse_return();
    case Token::Type::For:
        return parse_for();
    case Token::Type::While:
    case Token::Type::Dowhile:
        return parse_while();
    case Token::Type::Enumerate:
        return parse_enumerate_creation();
    case Token::Type::Delete:
        return parse_remove();
    case Token::Type::Break:
    case Token::Type::Continue:
        return parse_break_continue();
    default:
        return parse_expr();
    }
}

std::shared_ptr<Node> Parser::parse_break_continue() {
    if (_current->type != Token::Type::Break && _current->type != Token::Type::Continue) {
        throw ParserError("BreakContinue statement should contain a break token or a continue token");
    }
    auto res = std::make_shared<BreakContinueNode>(_current->type == Token::Type::Continue);
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return res;
}

std::shared_ptr<Node> Parser::parse_scope() {
    if (_current->type != Token::Type::LBrace) {
        throw ParserError("A scope should begin with a left brace");
    }
    parse_token();
    auto scope = std::make_shared<ScopeNode>();
    while (!shouldEnd()) {
        if (_current->type == Token::Type::Import) {
            parse_import_now(scope);
            continue;
        }
        scope->statements.push_back(parse_statement());
    }
    if (_current->type != Token::Type::RBrace) {
        throw ParserError("A scope should end with a right brace");
    }
    parse_token();
    return scope;
}

std::shared_ptr<Node> Parser::parse_expr() {
    auto res = std::make_shared<ExprNode>(parse_expr_level(OperatorPriority::Lowest));
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return res;
}

std::shared_ptr<Node> Parser::parse_expr_level(OperatorPriority pri) {
    auto _node = lookupPre(_current->type);
    if (_node->type == Node::Type::Error) {
        return _node;
    }
    while (!shouldEnd() && pricmp(pri, getpri(_current->type))) {
        auto tmp = lookupIn(_current->type, _node);
        if (tmp == nullptr) return _node;
        _node = tmp;
    }
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _node;
}

std::shared_ptr<Node> Parser::parse_if() {
    if (_current->type != Token::Type::If) {
        throw ParserError("An if statement should begin with 'if'");
    }
    parse_token();
    auto _node = std::make_shared<IfNode>();
    if (_current->type != Token::Type::LParan) {
        throw ParserError("If statement should have a paran");
    }
    parse_token();
    _node->_cond = parse_expr();
    if (_current->type != Token::Type::RParan) {
        throw ParserError("If statement should have a paran");
    }
    parse_token();
    _node->_then = parse_statement();
    if (_current->type == Token::Type::Else) {
        parse_token();
        _node->_else = parse_statement();
    }
    return _node;
}

std::shared_ptr<Node> Parser::parse_for() {
    if (_current->type != Token::Type::For) {
        throw ParserError("A for statement should begin with a for token", &lexer);
    }
    parse_token();
    if (_current->type == Token::Type::Identifier) {
        auto _node = std::make_shared<ForNode>();
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("An identifier expression should include an identifier token", &lexer);
        }
        _node->_var = _current->value;
        parse_token();
        if (_current->type != Token::Type::LParan) {
            throw ParserError("For format error", &lexer);
        }
        parse_token();
        _node->_elem = parse_expr();
        if (_current->type != Token::Type::RParan) {
            throw ParserError("For format error", &lexer);
        }
        parse_token();
        _node->_body = parse_statement();
        return _node;
    }
    else if (_current->type == Token::Type::LParan) {
        parse_token();
        auto _node = std::make_shared<CForNode>();
        _node->_init = parse_statement();
        _node->_cond = parse_expr();
        _node->_next = parse_statement();
        if (_current->type != Token::Type::RParan) {
            throw ParserError("For format error", &lexer);
        }
        _node->_body = parse_statement();
        return _node;
    }
    else {
        throw ParserError("Unknown for type", &lexer);
    }
}

std::shared_ptr<Node> Parser::parse_while() {
    if (_current->type != Token::Type::While && _current->type != Token::Type::Dowhile) {
        throw ParserError("A while statement should begin with a while token or a do-while token", &lexer);
    }
    bool isDoWhile = (_current->type == Token::Type::Dowhile);
    parse_token();
    auto _node = std::make_shared<WhileNode>(isDoWhile);
    _node->_cond = parse_expr();
    _node->_body = parse_statement();
    return _node;
}

std::shared_ptr<Node> Parser::parse_function() {
    if (_current->type != Token::Type::Func && _current->type != Token::Type::Lambda) {
        throw ParserError("A function expression should begin with a function token or a lambda token", &lexer);
    }
    auto _obj = std::make_shared<FunctionNode>();
    bool isLambda = (_current->type == Token::Type::Lambda);
    parse_token();

    if (_current->type != Token::Type::LParan) {
        throw ParserError("A function should have an argument list", &lexer);
    }
    while (!shouldEnd() && _current->type != Token::Type::RParan) {
        parse_token();
        if (_current->type == Token::Type::RParan) break;
        std::string _p;
        if (_current->type == Token::Type::More) {
            parse_token();
            if (_current->type != Token::Type::Identifier) {
                throw ParserError("A expand argument should have a name");
            }
            _obj->moreName = _current->value;
            parse_token();
            if (_current->type != Token::Type::RParan) {
                if (_current->type != Token::Type::Comma) {
                    throw ParserError("Arguments should be splited by comma");
                }
                else {
                    throw ParserError("Expanding argument should be the last one");
                }
            }
            break;
        }
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("An argument should be an identifier");
        }
        _p = _current->value;
        parse_token();
        if (_current->type == Token::Type::As) {
            parse_token();
            if (_current->type != Token::Type::Identifier) {
                throw ParserError("Typechecks should be identifiers");
            }
            _obj->typechecks.insert({_obj->args.size(), _current->value});
            parse_token();
        }
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
            throw ParserError("Arguments should be splited by comma");
        }
        _obj->args.push_back(_p);
    }
    parse_token();

    if (isLambda) {
        if (_current->type != Token::Type::Arrow) {
            throw ParserError("Lambdas should have an arrow");
        }
        parse_token();
        auto expr = parse_expr();
        auto ret = std::make_shared<ReturnNode>(expr);
        auto scope = std::make_shared<ScopeNode>();
        scope->statements.push_back(ret);
        _obj->inner = scope;
        return _obj;
    }
    _obj->inner = parse_scope();
    return _obj;
}

std::shared_ptr<Node> Parser::parse_ternary(std::shared_ptr<Node> cond) {
    if (_current->type != Token::Type::Ternary) {
        throw ParserError("A ternary should look like 'cond ? if : else'", &lexer);
    }
    parse_token();
    auto _node = std::make_shared<TernaryNode>(cond);
    _node->_if = parse_expr_level(OperatorPriority::Assign);
    if (_current->type != Token::Type::As) {
       throw ParserError("A ternary should have an 'as' token (':')", &lexer);
    }
    parse_token();
    _node->_else = parse_expr_level(OperatorPriority::Assign);
    return _node;
}

std::shared_ptr<Node> Parser::parse_class_creation() {
    if (_current->type != Token::Type::Class) {
        throw ParserError("A class statement should begin with a class token", &lexer);
    }
    parse_token();
    if (_current->type != Token::Type::Identifier) {
        throw ParserError("A class should have a name", &lexer);
    }
    auto _classNode = std::make_shared<ClassNode>();
    _classNode->_name = _current->value;
    parse_token();
    if (_current->type == Token::Type::As) {
        parse_token();
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("The class to extend must be an identifier token", &lexer);
        }
        _classNode->_ext = _current->value;
        parse_token();
    }
    if (_current->type != Token::Type::LBrace) {
        throw ParserError("A class should have a scope", &lexer);
    }
    parse_token();
    while (!shouldEnd()) {
        _classNode->inner.push_back(parse_class_statement());
    }
    if (_current->type != Token::Type::RBrace) {
        throw ParserError("The scope of class should end with a right brace", &lexer);
    }
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _classNode;
}

std::shared_ptr<Node> Parser::parse_enumerate_creation() {
    if (_current->type != Token::Type::Enumerate) {
        throw ParserError("An enumerate statement should begin with an enumerate token", &lexer);
    }
    parse_token();
    std::string _name;
    if (_current->type != Token::Type::Identifier) {
        throw ParserError("An enumerate should have a name", &lexer);
    }
    _name = _current->value;
    parse_token();
    if (_current->type != Token::Type::LBrace) {
        throw ParserError("An enumerate should have an item list", &lexer);
    }
    auto _enumNode = std::make_shared<EnumerateNode>();
    _enumNode->_name = _name;
    while (!shouldEnd() && _current->type != Token::Type::RBrace) {
        parse_token();
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("Each item in an enumerate should be an identifier token", &lexer);
        }
        _enumNode->items.push_back(_current->value);
        parse_token();
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RBrace) {
            throw ParserError("Items in an enumerate should be splited by comma and end with a right brace", &lexer);
        }
    }
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _enumNode;
}

std::shared_ptr<Node> Parser::parse_function_creation() {
    Token::Type funcType = _current->type;
    parse_token();
    if (funcType == Token::Type::Constructor) {
        auto _obj = std::make_shared<ConstructorNode>();
        if (_current->type != Token::Type::LParan) {
            throw ParserError("Constructors should have an argument list", &lexer);
        }
        while (!shouldEnd() && _current->type != Token::Type::RParan) {
            parse_token();
            if (_current->type == Token::Type::RParan) break;
            std::string _p;
            if (_current->type == Token::Type::More) {
                throw ParserError("Constructors cannot use argument expanding", &lexer);
            }
            if (_current->type != Token::Type::Identifier) {
                throw ParserError("Arguments should have a name", &lexer);
            }
            _p = _current->value;
            parse_token();
            if (_current->type == Token::Type::As) {
                parse_token();
                if (_current->type != Token::Type::Identifier) {
                    throw ParserError("Typechecks should be identifiers");
                }
                _obj->typechecks.insert({_obj->args.size(), _current->value});
                parse_token();
            }
            if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
                throw ParserError("Arguments should be splited by comma and end with a right paran", &lexer);
            }
            _obj->args.push_back(_p);
        }
        parse_token();
        _obj->inner = parse_scope();
        _obj->argcount = _obj->args.size();
        return _obj;
    }
    else if (funcType == Token::Type::Destructor) {
        auto _obj = std::make_shared<DestructorNode>();
        _obj->inner = parse_scope();
        return _obj;
    }
    else if (funcType == Token::Type::FunctionDef || funcType == Token::Type::Method) {
        auto _obj = std::make_shared<FunctionNode>();
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("Functions should have a name", &lexer);
        }
        std::string _name = _current->value;
        parse_token();
        if (_current->type != Token::Type::LParan) {
            throw ParserError("A function should have an argument list", &lexer);
        }
        while (!shouldEnd() && _current->type != Token::Type::RParan) {
            parse_token();
            if (_current->type == Token::Type::RParan) break;
            std::string _p;
            if (_current->type == Token::Type::More) {
                parse_token();
                if (_current->type != Token::Type::Identifier) {
                    throw ParserError("A expand argument should have a name");
                }
                _obj->moreName = _current->value;
                parse_token();
                if (_current->type == Token::Type::As) {
                    parse_token();
                    if (_current->type != Token::Type::Identifier) {
                        throw ParserError("Typechecks should be identifiers");
                    }
                    _obj->typechecks.insert({_obj->args.size(), _current->value});
                    parse_token();
                }
                if (_current->type != Token::Type::RParan) {
                    if (_current->type != Token::Type::Comma) {
                        throw ParserError("Arguments should be splited by comma");
                    }
                    else {
                        throw ParserError("Expanding argument should be the last one");
                    }
                }
                break;
            }
            if (_current->type != Token::Type::Identifier) {
                throw ParserError("An argument should be an identifier");
            }
            _p = _current->value;
            parse_token();
            if (_current->type == Token::Type::As) {
                parse_token();
                if (_current->type != Token::Type::Identifier) {
                    throw ParserError("Typechecks should be identifiers");
                }
                _obj->typechecks.insert({_obj->args.size(), _current->value});
                parse_token();
            }
            if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
                throw ParserError("Arguments should be splited by comma");
            }
            _obj->args.push_back(_p);
        }
        parse_token();
        _obj->inner = parse_scope();
        auto cr = std::make_shared<CreationNode>(false, false, false);
        cr->creations.push_back({_name, _obj});
        return cr;
    }
    else {
        throw ParserError("Unknown function definition head", &lexer);
    }
}

std::shared_ptr<Node> Parser::parse_creation() {
    bool isConst = (_current->type == Token::Type::Const);
    if (isConst) {
        parse_token();
    }
    std::shared_ptr<CreationNode> _node;
    if (_current->type == Token::Type::Let) {
        _node = std::make_shared<CreationNode>(false, true, isConst);
        parse_token();
    }
    else if (_current->type == Token::Type::Var || _current->type == Token::Type::Public) {
        _node = std::make_shared<CreationNode>(false, false, isConst);
        parse_token();
    }
    else if (_current->type == Token::Type::Global) {
        _node = std::make_shared<CreationNode>(true, false, isConst);
        parse_token();
    }
    else if (isConst) {
        _node = std::make_shared<CreationNode>(false, false, true);
    }
    else {
        throw ParserError("Unknown creation header", &lexer);;
    }
    while (true) {
        std::shared_ptr<Node> _obj = nullptr;
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("The variable to create must be an identifier token", &lexer);
        }
        std::string _name = _current->value;
        parse_token();
        if (_current->type == Token::Type::Assign) {
            parse_token();
            _obj = parse_expr();
        }
        _node->creations.push_back(std::make_pair(_name, _obj));
        if (_current->type != Token::Type::Comma) {
            break;
        }
        parse_token();
    }
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _node;
}

std::shared_ptr<Node> Parser::parse_number() {
    std::string _val = _current->value;
    parse_token();
    if (_prev->type == Token::Type::Integer) {
        if (_val.at(0) == '0' && _val.length() > 1) {
            if (_val.at(1) == 'x') {
                if (_val.length() == 2) {
                    throw ParserError("Invalid hex number", &lexer);
                }
                return std::make_shared<IntegerNode>(std::stoll(_val.substr(2), nullptr, 16));
            }
            else if (_val.at(1) == 'b') {
                if (_val.length() == 2) {
                    throw ParserError("Invalid bin number", &lexer);
                }
                return std::make_shared<IntegerNode>(std::stoll(_val.substr(2), nullptr, 2));
            }
            else {
                return std::make_shared<IntegerNode>(std::stoll(_val.substr(1), nullptr, 8));
            }
        }
        else {
            return std::make_shared<IntegerNode>(std::stoll(_val));
        }
    }
    else if (_prev->type == Token::Type::Float) {
        return std::make_shared<FloatNode>(std::stod(_val));
    }
    else {
        throw ParserError("Invalid number type", &lexer);
    }
}

std::shared_ptr<Node> Parser::parse_string() {
    if (_current->type != Token::Type::String) {
        throw ParserError("String expression must be a string token", &lexer);
    }
    parse_token();
    return std::make_shared<StringNode>(_prev->value);
}

std::shared_ptr<Node> Parser::parse_boolean() {
    if (_current->type != Token::Type::True && _current->type != Token::Type::False) {
        throw ParserError("Boolean must be true or false", &lexer);
    }
    parse_token();
    return std::make_shared<BooleanNode>(_prev->type == Token::Type::True);
}

std::shared_ptr<Node> Parser::parse_array() {
    if (_current->type != Token::Type::LBracket) {
        throw ParserError("Array expressions should begin with a left bracket", &lexer);
    }
    auto _arr = std::make_shared<ArrayNode>();
    while (_current->type != Token::Type::RBracket) {
        parse_token();
        if (_current->type == Token::Type::RBracket) break;
        _arr->elements.push_back(parse_expr());
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RBracket) {
            throw ParserError("Items of an array must be splited by comma", &lexer);
        }
    }
    parse_token();
    return _arr;
}

std::shared_ptr<Node> Parser::parse_return() {
    if (_current->type != Token::Type::Return) {
        throw ParserError("Return expressions must begin with a return token", &lexer);
    }
    parse_token();
    if (shouldEnd()) {
        auto node = std::make_shared<ReturnNode>(std::make_shared<NullNode>());
        if (_current->type == Token::Type::Semicolon) {
            parse_token();
        }
        return node;
    }
    bool isr;
    if (isr = (_current->type == Token::Type::BitwiseAnd)) {
        parse_token();
    }
    auto node = std::make_shared<ReturnNode>(parse_expr());
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    node->isReference = isr;
    return node;
}

std::shared_ptr<Node> Parser::parse_infix(std::shared_ptr<Node> left) {
    auto _node = std::make_shared<InfixNode>(_current->value);
    OperatorPriority pri = getpri(_current->type);
    if (pri == OperatorPriority::Lowest) {
        throw ParserError("Unknown Infix", &lexer);
    }
    parse_token();
    _node->left = left;
    _node->right = parse_expr_level(pri);
    return _node;
}

std::shared_ptr<Node> Parser::parse_prefix() {
    auto _node = std::make_shared<PrefixNode>(_current->value);
    if (_current->type != Token::Type::Plus && _current->type != Token::Type::Minus && _current->type != Token::Type::BitwiseNot && _current->type != Token::Type::LogicalNot) {
        throw ParserError("Unknown Prefix", &lexer);
    }
    parse_token();
    _node->right = parse_expr_level(OperatorPriority::Single);
    return _node;
}

std::shared_ptr<Node> Parser::parse_remove() {
    if (_current->type != Token::Type::Delete) {
        throw ParserError("Remove expressions should begin with a delete token", &lexer);
    }
    parse_token();
    if (_current->type != Token::Type::Identifier) {
        throw ParserError("Nothing to remove", &lexer);
    }
    auto _node = std::make_shared<RemoveNode>(_current->value);
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _node;
}

std::shared_ptr<Node> Parser::import_file(std::string filePath) {
    std::string _input_src = "", _input_line;
    std::ifstream is(filePath);
    if (!is.good()) {
        throw ParserError("Import: File not found: " + filePath, &lexer);
    }
    while (!is.eof()) {
        std::getline(is, _input_line);
        _input_src += "\n" + _input_line;
    }
    Parser parser(_input_src);
    return parser.parse_program();
}

void Parser::parse_import_now(std::shared_ptr<ScopeNode> defr) {
    if (_current->type != Token::Type::Import) {
        throw ParserError("Import expressions must begin with a import token", &lexer);
    }
    parse_token();
    if (_current->type != Token::Type::String) {
        throw ParserError("Static import cannot use dynamic file path", &lexer);
    }
	auto fileName = unescape(_current->value);
	if (implist.count(fileName)) {
		return;
	}
    auto _innerProg = import_file(fileName);
	implist.insert(fileName);
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    if (_innerProg->type != Node::Type::Program) {
        throw ParserError("Import result is not a program", &lexer);
    }
    auto _cast = std::dynamic_pointer_cast<ProgramNode>(_innerProg);
    if (_cast->mainScope->type != Node::Type::Scope) {
        throw ParserError("Import result did not contain a scope", &lexer);
    }
    defr->mergeWith(std::dynamic_pointer_cast<ScopeNode>(_cast->mainScope));
}

std::shared_ptr<Node> Parser::parse_assign(std::shared_ptr<Node> left) {
    auto _node = std::make_shared<AssignNode>(_current->value);
    if (getpri(_current->type) != OperatorPriority::Assign) {
        throw ParserError("Invalid assign operator", &lexer);
    }
    parse_token();
    _node->left = left;
    _node->right = parse_expr_level(OperatorPriority::Assign);
    return _node;
}

std::shared_ptr<Node> Parser::parse_group() {
    if (_current->type != Token::Type::LParan) {
      throw ParserError("Groups should begin with a left paran", &lexer);
    }
    parse_token();
    auto _res =  parse_expr();
    if (_current->type != Token::Type::RParan) {
        throw ParserError("Groups should end with a right paran", &lexer);
    }
    parse_token();
    return std::make_shared<GroupNode>(_res);
}

std::shared_ptr<Node> Parser::parse_call(std::shared_ptr<Node> left) {
    auto _node = std::make_shared<CallNode>(left);
    if (_current->type != Token::Type::LParan) {
        throw ParserError("Call expressions should have an argument list", &lexer);
    }
    while (_current->type != Token::Type::RParan) {
        parse_token();
        if (_current->type == Token::Type::RParan) break;
        if (_current->type == Token::Type::More) {
            parse_token();
            _node->expands.push_back(_node->args.size());
        }
        _node->args.push_back(parse_expr());
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
            throw ParserError("Arguments should be splited by comma", &lexer);
        }
    }
    parse_token();
    return _node;
}

std::shared_ptr<Node> Parser::parse_index(std::shared_ptr<Node> left) {
    if (_current->type != Token::Type::LBracket) {
        throw ParserError("Index expressions should begin with a left bracket", &lexer);
    }
    parse_token();
    auto _node = std::make_shared<IndexNode>(left, parse_expr());
    if (_current->type != Token::Type::RBracket) {
        throw ParserError("Index expressions should end with a right bracket", &lexer);
    }
    parse_token();
    return _node;
}

std::shared_ptr<Node> Parser::parse_identifier() {
    if (_current->type != Token::Type::Identifier) {
        throw ParserError("An identifier expression should include an identifier token", &lexer);
    }
    auto _node = std::make_shared<IdentifierNode>(_current->value);
    parse_token();
    if (_node->id == "operator" || _node->id == "prefix") {
        auto pri = getpri(_current->type);
        if (pri == OperatorPriority::Lowest) {
            throw ParserError("No such operator: " + _current->value, &lexer);
        }
        _node->id += _current->value;
        if (_current->type == Token::Type::LParan || _current->type == Token::Type::LBracket) {
            parse_token();
            _node->id += _current->value;
        }
        parse_token();
    }
    return _node;
}

std::shared_ptr<Node> Parser::parse_in_decrement_before() {
    if (_current->type != Token::Type::Increment && _current->type != Token::Type::Decrement) {
        throw ParserError("Before-Increment/Decrement expressions should begin with '++' or '--'", &lexer);
    }
    bool _isDecrement = (_current->type == Token::Type::Decrement);
    parse_token();
    return std::make_shared<InDecrementNode>(parse_expr_level(OperatorPriority::Single), _isDecrement, false);
}

std::shared_ptr<Node> Parser::parse_named_constant() {
    if (_current->type == Token::Type::Null) {
        parse_token();
        return std::make_shared<NullNode>();
    }
    throw ParserError("Null expressions should have a null token", &lexer);
}

std::shared_ptr<Node> Parser::parse_in_decrement_after(std::shared_ptr<Node> left) {
    if (_current->type != Token::Type::Increment && _current->type != Token::Type::Decrement) {
        throw ParserError("After-Increment/Decrement expressions should end with '++' or '--'", &lexer);
    }
    bool _isDecrement = (_current->type == Token::Type::Decrement);
    parse_token();
    return std::make_shared<InDecrementNode>(left, _isDecrement, true);
}

std::shared_ptr<Node> Parser::parse_class_statement() {
    if (_current->type == Token::Type::Constructor || _current->type == Token::Type::Destructor) {
        return parse_function_creation();
    }
    ClassMember::AccessControl ac;
    bool isFinal = false;
    if (_current->type == Token::Type::Public) {
        ac = ClassMember::PUBLIC;
    }
    else if (_current->type == Token::Type::Protected) {
        ac = ClassMember::PROTECTED;
    }
    else if (_current->type == Token::Type::Private) {
        ac = ClassMember::PRIVATE;
    }
    else {
        throw ParserError("Unknown Access-Control", &lexer);
    }
    parse_token();
    if (_current->type == Token::Type::Value) {
        parse_token();
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("Values should be identifier tokens", &lexer);
        }
        std::string name = _current->value;
        parse_token();
        std::shared_ptr<Node> value = std::make_shared<NullNode>();
        if (_current->type == Token::Type::Assign) {
            parse_token();
            value = parse_expr();
        }
        auto cm = std::make_shared<ClassMember>(name, value);
        cm->ac = ac;
        cm->ctype = ClassMember::VALUE;
        cm->isFinal = false;
        if (_current->type == Token::Type::Semicolon) {
            parse_token();
        }
        return cm;
    }
    else if (_current->type == Token::Type::Static) {
        parse_token();
        if (_current->type != Token::Type::Identifier) {
            throw ParserError("Values should be identifier tokens", &lexer);
        }
        std::string name = _current->value;
        parse_token();
        if (_current->type != Token::Type::Assign) {
            throw ParserError("Statics should have a value", &lexer);
        }
        parse_token();
        std::shared_ptr<Node> value = parse_expr();
        auto cm = std::make_shared<ClassMember>(name, value);
        cm->ac = ac;
        cm->ctype = ClassMember::STATIC;
        cm->isFinal = false;
        if (_current->type == Token::Type::Semicolon) {
            parse_token();
        }
        return cm;
    }
    if (_current->type == Token::Type::Final) {
        isFinal = true;
        parse_token();
    }
    if (_current->type != Token::Type::Method) {
        throw ParserError("Unknown member type", &lexer);
    }
    auto inner = std::dynamic_pointer_cast<CreationNode>(parse_function_creation());
    auto cm = std::make_shared<ClassMember>(inner->creations[0].first, inner->creations[0].second);
    cm->ac = ac;
    cm->ctype = ClassMember::METHOD;
    cm->isFinal = isFinal;
    return cm;
}

std::shared_ptr<Node> Parser::parse_class_new() {
    if (_current->type != Token::Type::New) {
        throw ParserError("New expressions should begin with a new token", &lexer);
    }
    parse_token();
    if (_current->type != Token::Type::Identifier) {
        throw ParserError("Class to new must be an identifier", &lexer);
    }
    std::string name = _current->value;
    auto id = parse_identifier();
    if (_current->type != Token::Type::LParan) {
        auto _res = std::make_shared<NewNode>(name);
        _res->args = {};
        _res->expands = {};
        return _res;
    }
    auto _call = std::dynamic_pointer_cast<CallNode>(parse_call(id));
    auto _res = std::make_shared<NewNode>(name);
    _res->args = _call->args;
    _res->expands = _call->expands;
    return _res;
}

std::shared_ptr<Node> Parser::parse_decorate() {
    if (_current->type != Token::Type::Decorate) {
        throw ParserError("Decorate expressions should begin with a decorate token", &lexer);
    }
    auto _res = std::make_shared<DecorateNode>();
    parse_token();
    _res->decorator = parse_expr();
    _res->inner = parse_expr();
    return _res;
}

const std::map<Token::Type, Parser::OperatorPriority> Parser::priorityTable = {
    {Token::Type::Assign,               OperatorPriority::Assign},
    {Token::Type::AsteriskAssign,       OperatorPriority::Assign},
    {Token::Type::BitwiseAndAssign,     OperatorPriority::Assign},
    {Token::Type::BitwiseLeftAssign,    OperatorPriority::Assign},
    {Token::Type::BitwiseOrAssign,      OperatorPriority::Assign},
    {Token::Type::BitwiseRightAssign,   OperatorPriority::Assign},
    {Token::Type::BitwiseXorAssign,     OperatorPriority::Assign},
    {Token::Type::MinusAssign,          OperatorPriority::Assign},
    {Token::Type::ModulusAssign,        OperatorPriority::Assign},
    {Token::Type::PlusAssign,           OperatorPriority::Assign},
    {Token::Type::SlashAssign,          OperatorPriority::Assign},
    {Token::Type::Ternary,              OperatorPriority::Assign},

    {Token::Type::LogicalOr,            OperatorPriority::LogicalOr},
    {Token::Type::LogicalAnd,           OperatorPriority::LogicalAnd},
    {Token::Type::BitwiseOr,            OperatorPriority::BitwiseOr},
    {Token::Type::BitwiseXor,           OperatorPriority::BitwiseXor},
    {Token::Type::BitwiseAnd,           OperatorPriority::BitwiseAnd},
    
    {Token::Type::Equal,                OperatorPriority::Equals},
    {Token::Type::NotEqual,             OperatorPriority::Equals},
    {Token::Type::FullEqual,            OperatorPriority::Equals},
    {Token::Type::NotFullEqual,         OperatorPriority::Equals},
    {Token::Type::Greater,              OperatorPriority::LessGreater},
    {Token::Type::GreaterEqual,         OperatorPriority::LessGreater},
    {Token::Type::Less,                 OperatorPriority::LessGreater},
    {Token::Type::LessEqual,            OperatorPriority::LessGreater},

    {Token::Type::BitwiseLeft,          OperatorPriority::BitwiseMovement},
    {Token::Type::BitwiseRight,         OperatorPriority::BitwiseMovement},
    {Token::Type::Minus,                OperatorPriority::Sum},
    {Token::Type::Plus,                 OperatorPriority::Sum},
    {Token::Type::Asterisk,             OperatorPriority::Product},
    {Token::Type::Modulus,              OperatorPriority::Product},
    {Token::Type::Slash,                OperatorPriority::Product},
    {Token::Type::Pow,                  OperatorPriority::Pow},

    {Token::Type::LogicalNot,           OperatorPriority::Single},
    {Token::Type::BitwiseNot,           OperatorPriority::Single},
    {Token::Type::LBracket,             OperatorPriority::Suffix},
    {Token::Type::LParan,               OperatorPriority::Suffix},
    {Token::Type::Extand,               OperatorPriority::Suffix},
    {Token::Type::ForceExtand,          OperatorPriority::Suffix},
    {Token::Type::Increment,            OperatorPriority::Suffix},
    {Token::Type::Decrement,            OperatorPriority::Suffix},
    {Token::Type::More,                 OperatorPriority::Range}
};