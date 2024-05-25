#pragma once

#include "object/_all.hpp"
#include "ast/_all.hpp"
#include "parser/parser.hpp"

std::shared_ptr<Node> Parser::make_error() {
    parse_token();
    return std::make_shared<ErrorNode>();
}

std::shared_ptr<Node> Parser::parse_program() {
    auto region = std::make_shared<RegionNode>();
    while (_current->type != Token::Type::End) {
        if (_current->type == Token::Type::Import && doImport) {
            parse_import_now(region);
            continue;
        }
        region->statements.push_back(parse_statement());
    }
    parse_token();
    return std::make_shared<ProgramNode>(region);
}

std::shared_ptr<Node> Parser::parse_statement() {
    switch (_current->type) {
    case Token::Type::LBrace:
        return parse_region();
    case Token::Type::Class:
        return parse_class_creation();
    case Token::Type::Const:
    case Token::Type::Let:
    case Token::Type::Private:
    case Token::Type::Public:
    case Token::Type::Var:
    case Token::Type::Global:
        return std::make_shared<StatementNode>(parse_creation());
    case Token::Type::Constructor:
    case Token::Type::Destructor:
        return parse_function_creation();
    case Token::Type::If:
        return parse_if();
    case Token::Type::Import:
        return std::make_shared<StatementNode>(parse_import());
    case Token::Type::Return:
        return std::make_shared<StatementNode>(parse_return());
    case Token::Type::For:
        return parse_for();
    case Token::Type::While:
    case Token::Type::Dowhile:
        return parse_while();
    case Token::Type::Enumerate:
        return parse_enumerate_creation();
    case Token::Type::Delete:
        return std::make_shared<StatementNode>(parse_remove());
    default:
        return std::make_shared<StatementNode>(parse_expr());
    }
}

std::shared_ptr<Node> Parser::parse_region() {
    if (_current->type != Token::Type::LBrace) {
        format_error("Regions must start with a left brace.");
        return make_error();
    }
    parse_token();
    auto region = std::make_shared<RegionNode>();
    while (!shouldEnd()) {
        if (_current->type == Token::Type::Import && doImport) {
            parse_import_now(region);
            continue;
        }
        region->statements.push_back(parse_statement());
    }
    if (_current->type != Token::Type::RBrace) {
        end_not_correct_error();
    }
    parse_token();
    return region;
}

std::shared_ptr<Node> Parser::parse_expr() {
    return std::make_shared<ExprNode>(parse_expr_level(OperatorPriority::Lowest));
}

std::shared_ptr<Node> Parser::parse_expr_level(OperatorPriority pri) {
    auto _node = lookupPre(_current->type);
    if (_node->type == Node::Type::Error) {
        return _node;
    }
    while (!shouldEnd() && pricmp(pri, getpri(_current->type))) {
        _node = lookupIn(_current->type, _node);
        if (_node->type == Node::Type::Error) {
            return _node;
        }
    }
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _node;
}

std::shared_ptr<Node> Parser::parse_if() {
    if (_current->type != Token::Type::If) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    auto _node = std::make_shared<IfNode>();
    _node->_cond = parse_expr();
    _node->_then = parse_statement();
    if (_current->type == Token::Type::Else) {
        parse_token();
        _node->_else = parse_statement();
    }
}

std::shared_ptr<Node> Parser::parse_for() {
    if (_current->type != Token::Type::For) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    auto _node = std::make_shared<ForNode>();
    _node->_var = parse_identifier();
    if (_current->type != Token::Type::LParan) {
        format_error("For-Repeat must be written in the format of \"for var(range) statement\".");
    }
    parse_token();
    _node->_elem = parse_expr();
    if (_current->type != Token::Type::RParan) {
        format_error("For-Repeat must be written in the format of \"for var(range) statement\".");
    }
    parse_token();
    _node->_body = parse_statement();
}

std::shared_ptr<Node> Parser::parse_while() {
    if (_current->type != Token::Type::While && _current->type != Token::Type::Dowhile) {
        unhandled_compiler_error();
        return make_error();
    }
    bool isDoWhile = (_current->type == Token::Type::Dowhile);
    parse_token();
    auto _node = std::make_shared<WhileNode>();
    _node->isDoWhile = isDoWhile;
    _node->_cond = parse_expr();
    _node->_body = parse_statement();
}

std::shared_ptr<Node> Parser::parse_function() {
    if (_current->type != Token::Type::Function && _current->type != Token::Type::Lambda) {
        unhandled_compiler_error();
        return make_error();
    }
    auto _obj = std::make_shared<FunctionNode>();
    _obj->isLambda = (_current->type == Token::Type::Lambda);

    parse_token();
    if (_current->type != Token::Type::LParan) {
        format_error("Functions must have an arguments list.");
        return make_error();
    }
    while (!shouldEnd() && _current->type != Token::Type::RParan) {
        parse_token();
        std::string _p;
        if (_current->type == Token::Type::More) {
            parse_token();
            if (_current->type != Token::Type::Identifier) {
                format_error("More-Argument must have a name.");
                return make_error();
            }
            _p = _current->value;
            parse_token();
            if (_current->type != Token::Type::RParan) {
                if (_current->type != Token::Type::Comma) {
                    format_error("Please use ',' to split arguments.");
                }
                else {
                    format_error("More-Argument must be the last argument.");
                }
                return make_error();
            }
            break;
        }
        if (_current->type != Token::Type::Identifier) {
            format_error("Arguments must have a name.");
            return make_error();
        }
        _p = _current->value;
        parse_token();
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
            format_error("Please use ',' to split arguments.");
            return make_error();
        }
        _obj->args.push_back(_p);
    }
    parse_token();
    _obj->inner = parse_region();
    return _obj;
}

std::shared_ptr<Node> Parser::parse_ternary(std::shared_ptr<Node> cond) {
    if (_current->type != Token::Type::Ternary) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    auto _node = std::make_shared<TernaryNode>(cond);
    _node->_if = parse_expr_level(OperatorPriority::Assign);
    if (_current->type != Token::Type::As) {
        format_error("Ternary Expressions must be written like \"cond ? yes : no\".");
        return make_error();
    }
    parse_token();
    _node->_else = parse_expr_level(OperatorPriority::Assign);
    return _node;
}

std::shared_ptr<Node> Parser::parse_class_creation() {
    if (_current->type != Token::Type::Class) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    if (_current->type != Token::Type::Identifier) {
        format_error("Classes must have a name.");
        return make_error();
    }
    std::string _name = parse_identifier()->toString();
    auto _classNode = std::make_shared<ClassNode>();
    if (_current->type == Token::Type::As) {
        parse_token();
        if (_current->type != Token::Type::Identifier) {
            format_error("Classes must inherit something.");
            return make_error();
        }
        _classNode->_ext = parse_identifier()->toString();
    }
    _classNode->inner = parse_statement();
    auto _creationNode = std::make_shared<CreationNode>(false, false, false, true);
    _creationNode->creations.push_back(std::make_pair(_name, _classNode));
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _classNode;
}

std::shared_ptr<Node> Parser::parse_enumerate_creation() {
    if (_current->type != Token::Type::Enumerate) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    std::string _name;
    if (_current->type != Token::Type::Identifier) {
        format_error("Enumerates must have a name.");
        return make_error();
    }
    _name = parse_identifier()->toString();
    if (_current->type != Token::Type::LBrace) {
        format_error("Enumerates must have a value list.");
        return make_error();
    }
    auto _enumNode = std::make_shared<EnumerateNode>();
    while (!shouldEnd() && _current->type != Token::Type::RBrace) {
        parse_token();
        if (_current->type != Token::Type::Identifier) {
            format_error("Enumerate values must have a name.");
            return make_error();
        }
        _enumNode->items.push_back(_current->value);
        parse_token();
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RBrace) {
            format_error("You should use ',' to split enumerate values.");
            return make_error();
        }
    }
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    auto _creationNode = std::make_shared<CreationNode>(false, false, false, true);
    _creationNode->creations.push_back(std::make_pair(_name, _enumNode));
}

std::shared_ptr<Node> Parser::parse_function_creation() {
    if (_current->type != Token::Type::Constructor && _current->type != Token::Type::Destructor) {
        unhandled_compiler_error();
        return make_error();
    }
    Token::Type funcType = _current->type;
    auto _obj = std::make_shared<FunctionNode>();
    _obj->isLambda = false;
    std::string _name;
    parse_token();
    if (funcType == Token::Type::Constructor) {
        if (_current->type != Token::Type::LParan) {
            format_error("Constructors must have an argument list.");
            return make_error();
        }
        while (!shouldEnd() && _current->type != Token::Type::RParan) {
            parse_token();
            std::string _p;
            if (_current->type == Token::Type::More) {
                format_error("Cannot use More-Argument in constructors.");
                return make_error();
            }
            if (_current->type != Token::Type::Identifier) {
                format_error("Arguments must have a name.");
                return make_error();
            }
            _p = _current->value;
            parse_token();
            if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
                format_error("You should use ',' to split arguments.");
                return make_error();
            }
            _obj->args.push_back(_p);
        }
        parse_token();
    }
    if (funcType == Token::Type::Destructor) {
        _name = "__spec_destructor";
    }
    else {
        _name = "__spec_constructor";
    }

    _obj->inner = parse_region();
    auto _creationNode = std::make_shared<CreationNode>(false, false, false, true);
    _creationNode->creations.push_back(std::make_pair(_name, _obj));
    return _creationNode;
}

std::shared_ptr<Node> Parser::parse_creation() {
    bool isConst = (_current->type == Token::Type::Const);
    if (isConst) {
        parse_token();
    }
    std::shared_ptr<CreationNode> _node;
    if (_current->type == Token::Type::Let) {
        _node = std::make_shared<CreationNode>(false, false, true, isConst);
        parse_token();
    }
    else if (_current->type == Token::Type::Var || _current->type == Token::Type::Public) {
        _node = std::make_shared<CreationNode>(false, false, false, isConst);
        parse_token();
    }
    else if (_current->type == Token::Type::Private) {
        _node = std::make_shared<CreationNode>(false, true, false, isConst);
        parse_token();
    }
    else if (_current->type == Token::Type::Global) {
        _node = std::make_shared<CreationNode>(true, false, false, isConst);
        parse_token();
    }
    else if (isConst) {
        _node = std::make_shared<CreationNode>(false, false, false, true);
    }
    else {
        unhandled_compiler_error();
        return make_error();
    }
    while (true) {
        std::shared_ptr<Node> _obj = nullptr;
        if (_current->type != Token::Type::Identifier) {
            format_error("Variables must have a name.");
            return make_error();
        }
        std::string _name = parse_identifier()->toString();
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
    std::shared_ptr<Object> _obj;
    std::string _val = _current->value;
    if (_current->type == Token::Type::Integer) {
        if (_val.at(0) == '0' && _val.length() > 1) {
            if (_val.at(1) == 'x') {
                if (_val.length() == 2) {
                    invalid_number_error();
                    return make_error();
                }
                _obj = std::make_shared<Integer>(std::stoll(_val.substr(2), nullptr, 16));
            }
            else if (_val.at(1) == 'b') {
                if (_val.length() == 2) {
                    invalid_number_error();
                    return make_error();
                }
                _obj = std::make_shared<Integer>(std::stoll(_val.substr(2), nullptr, 2));
            }
            else {
                _obj = std::make_shared<Integer>(std::stoll(_val.substr(1), nullptr, 8));
            }
        }
        else {
            _obj = std::make_shared<Integer>(_val);
        }
    }
    else if (_current->type == Token::Type::Float) {
        _obj = std::make_shared<Float>(std::stod(_val));
    }
    else {
        invalid_number_error();
        return make_error();
    }
    parse_token();
    return std::make_shared<ConstantNode>(_obj);
}

std::shared_ptr<Node> Parser::parse_string() {
    if (_current->type != Token::Type::String) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    return std::make_shared<ConstantNode>(std::make_shared<String>(_prev->value));
}

std::shared_ptr<Node> Parser::parse_boolean() {
    if (_current->type != Token::Type::True && _current->type != Token::Type::False) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    return std::make_shared<ConstantNode>(std::make_shared<Boolean>(_prev->type == Token::Type::True));
}

std::shared_ptr<Node> Parser::parse_array() {
    if (_current->type != Token::Type::LBracket) {
        unhandled_compiler_error();
        return make_error();
    }
    auto _arr = std::make_shared<ArrayNode>();
    while (_current->type != Token::Type::RBracket) {
        parse_token();
        _arr->elements.push_back(parse_expr());
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RBracket) {
            format_error("You should use ',' to split items in an array.");
            return make_error();
        }
    }
    parse_token();
    return _arr;
}

std::shared_ptr<Node> Parser::parse_return() {
    if (_current->type != Token::Type::Return) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return std::make_shared<ReturnNode>(parse_expr());
}

std::shared_ptr<Node> Parser::parse_infix(std::shared_ptr<Node> left) {
    auto _node = std::make_shared<InfixNode>(_current->value);
    OperatorPriority pri = getpri(_current->type);
    if (pri == OperatorPriority::Lowest) {
        unknown_infix_error();
        return make_error();
    }
    parse_token();
    _node->left = left;
    _node->right = parse_expr_level(pri);
    return _node;
}

std::shared_ptr<Node> Parser::parse_prefix() {
    auto _node = std::make_shared<PrefixNode>(_current->value);
    if (_current->type != Token::Type::Plus && _current->type != Token::Type::Minus && _current->type != Token::Type::BitwiseNot && _current->type != Token::Type::LogicalNot) {
        unknown_prefix_error();
        return make_error();
    }
    parse_token();
    _node->right = parse_expr_level(OperatorPriority::Single);
    return _node;
}

std::shared_ptr<Node> Parser::parse_remove() {
    if (_current->type != Token::Type::Delete) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    if (_current->type != Token::Type::Identifier) {
        format_error("Remove statement must remove something.");
        return make_error();
    }
    auto _node = std::make_shared<RemoveNode>(parse_identifier()->toString());
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _node;
}

std::shared_ptr<Node> Parser::import_file(std::string filePath) {
    std::ifstream is(filePath.substr(filePath.length() - 6) == ".precf" ? filePath : (filePath + ".precf"), std::ios::in | std::ios::binary);
    if (is.good()) {
        auto _res = std::make_shared<ProgramNode>(nullptr);
        _res->readFrom(is);
        return _res;
    }
    else {
        std::string _input_src = "", _input_line;
        std::ifstream is(filePath);
        if (!is.good()) {
            import_error();
            return make_error();
        }
        while (!is.eof()) {
            std::getline(is, _input_line);
            _input_src += "\n" + _input_line;
        }
        Parser parser(_input_src);
        return parser.parse_program();
    }
}

std::shared_ptr<Node> Parser::parse_import() {
    if (_current->type != Token::Type::Import) {
        unhandled_compiler_error();
        return make_error();
    }
    auto _node = std::make_shared<ImportNode>();
    parse_token();
    if (_current->type != Token::Type::String) {
        format_error("Import statements need a fixed path.");
        return make_error();
    }
    _node->item = String::unescape(_current->value);
    parse_token();
    if (_current->type == Token::Type::Semicolon) {
        parse_token();
    }
    return _node;
}

void Parser::parse_import_now(std::shared_ptr<RegionNode> defr) {
    if (_current->type != Token::Type::Import) {
        unhandled_compiler_error();
        return;
    }
    parse_token();
    if (_current->type != Token::Type::String) {
        format_error("Import statement need a fixed path.");
        return;
    }
	auto fileName = String::unescape(_current->value);
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
        import_error();
        return;
    }
    auto _cast = std::dynamic_pointer_cast<ProgramNode>(_innerProg);
    if (_cast->mainRegion->type != Node::Type::Region) {
        import_error();
        return;
    }
    defr->mergeWith(std::dynamic_pointer_cast<RegionNode>(_cast->mainRegion));
}

std::shared_ptr<Node> Parser::parse_assign(std::shared_ptr<Node> left) {
    auto _node = std::make_shared<AssignNode>(_current->value);
    if (getpri(_current->type) != OperatorPriority::Assign) {
        unknown_assign_error();
        return make_error();
    }
    parse_token();
    _node->left = left;
    _node->right = parse_expr_level(OperatorPriority::Assign);
    return _node;
}

std::shared_ptr<Node> Parser::parse_group() {
    if (_current->type != Token::Type::LParan) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    auto _res =  parse_expr();
    if (_current->type != Token::Type::RParan) {
        format_error("Groups must have an end.");
        return make_error();
    }
    parse_token();
    return _res;
}

std::shared_ptr<Node> Parser::parse_call(std::shared_ptr<Node> left) {
    auto _node = std::make_shared<CallNode>(left);
    if (_current->type != Token::Type::LParan) {
        unhandled_compiler_error();
        return make_error();
    }
    while (_current->type != Token::Type::RParan) {
        parse_token();
        if (_current->type == Token::Type::More) {
            parse_token();
            _node->more = parse_expr();
            if(_current->type != Token::Type::RParan) {
                format_error("More-Argument must be the last argument.");
                return make_error();
            }
            break;
        }
        _node->args.push_back(parse_expr());
        if (_current->type != Token::Type::Comma && _current->type != Token::Type::RParan) {
            format_error("You should use ',' to split arguments.");
            return make_error();
        }
    }
    parse_token();
    return _node;
}

std::shared_ptr<Node> Parser::parse_index(std::shared_ptr<Node> left) {
    if (_current->type != Token::Type::LBracket) {
        unhandled_compiler_error();
        return make_error();
    }
    parse_token();
    auto _node = std::make_shared<IndexNode>(left, parse_expr());
    if (_current->type != Token::Type::RBracket) {
        format_error("Index expression must have an end.");
        return make_error();
    }
    parse_token();
    return _node;
}

std::shared_ptr<Node> Parser::parse_identifier() {
    if (_current->type != Token::Type::Identifier) {
        unhandled_compiler_error();
        return make_error();
    }
    auto _node = std::make_shared<IdentifierNode>(_current->value);
    parse_token();
    if (_node->id == "operator" || _node->id == "prefix") {
        auto pri = getpri(_current->type);
        if (pri == OperatorPriority::Lowest) {
            format_error("No operators or prefixes found.");
            return make_error();
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
        unhandled_compiler_error();
        return make_error();
    }
    bool _isDecrement = (_current->type == Token::Type::Decrement);
    parse_token();
    return std::make_shared<InDecrementNode>(parse_expr_level(OperatorPriority::Single), _isDecrement, false);
}

std::shared_ptr<Node> Parser::parse_named_constant() {
    if (_current->type == Token::Type::Null) {
        return std::make_shared<ConstantNode>(std::make_shared<Null>());
    }
    unhandled_compiler_error();
    return make_error();
}

std::shared_ptr<Node> Parser::parse_in_decrement_after(std::shared_ptr<Node> left) {
    if (_current->type != Token::Type::Increment && _current->type != Token::Type::Decrement) {
        unhandled_compiler_error();
        return make_error();
    }
    bool _isDecrement = (_current->type == Token::Type::Decrement);
    parse_token();
    return std::make_shared<InDecrementNode>(left, _isDecrement, true);
}

void unknown_pre_expr_error() {
    err_begin();
    std::cerr << "Unknown Pre-Expression.";
    err_end();
}

void unknown_in_expr_error() {
    err_begin();
    std::cerr << "Unknown In-Expression.";
    err_end();
}

void unhandled_compiler_error() {
    err_begin();
    std::cerr << "Unhandled error happened when parsing the program.\n";
    std::cerr << "This may be a bug. Report it if you can.";
    err_end();
}

void end_not_correct_error() {
    format_error("Something ended incorrectly.");
} // Moved to format_error

void format_error(std::string msg) {
    err_begin();
    std::cerr << "Format Error: " + msg;
    err_end();
}

void invalid_number_error() {
    err_begin();
    std::cerr << "Invalid type of a number.";
    err_end();
}

void unknown_infix_error() {
    err_begin();
    std::cerr << "Used an unsupported token to be an infix.";
    err_end();
}

void unknown_prefix_error() {
    err_begin();
    std::cerr << "Used an unsupported token to be a prefix.";
    err_end();
}

void unknown_assign_error() {
    err_begin();
    std::cerr << "Used an unsupported token when assign.";
    err_end();
}

void import_error() {
    err_begin();
    std::cerr << "Cannot import a file - check it or its pre-compiled file is valid.";
    err_end();
}