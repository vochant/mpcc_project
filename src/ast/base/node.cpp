#pragma once

#include "ast/_all.hpp"

void illegal_program_error() {
    err_begin();
    std::cerr << "Unknown program!\n";
    std::cerr << "Is it using unsupported formatting or future versions?";
    err_end();
}

std::shared_ptr<Node> Node::get_node(std::istream& is) {
    Node::Type _sign = Node::Type(BinaryIn::read_byte(is));
    std::shared_ptr<Node> _bas;
    switch(_sign) {
    case Type::Array:
        _bas = std::make_shared<ArrayNode>();
        break;
    case Type::Assign:
        _bas = std::make_shared<AssignNode>("");
        break;
    case Type::Call:
        _bas = std::make_shared<CallNode>(nullptr);
        break;
    case Type::Class:
        _bas = std::make_shared<ClassNode>();
        break;
    case Type::Constant:
        _bas = std::make_shared<ConstantNode>(nullptr);
        break;
    case Type::Creation:
        _bas = std::make_shared<CreationNode>(false, false, false, false);
        break;
    case Type::Enumerate:
        _bas = std::make_shared<EnumerateNode>();
        break;
    case Type::Expr:
        _bas = std::make_shared<ExprNode>(nullptr);
        break;
    case Type::For:
        _bas = std::make_shared<ForNode>();
        break;
    case Type::Function:
        _bas = std::make_shared<FunctionNode>();
        break;
    case Type::Identifier:
        _bas = std::make_shared<IdentifierNode>("");
        break;
    case Type::If:
        _bas = std::make_shared<IfNode>();
        break;
    case Type::Import:
        _bas = std::make_shared<ImportNode>();
        break;
    case Type::InDecrement:
        _bas = std::make_shared<InDecrementNode>(nullptr, false, false);
        break;
    case Type::Index:
        _bas = std::make_shared<IndexNode>(nullptr, nullptr);
        break;
    case Type::Infix:
        _bas = std::make_shared<InfixNode>("");
        break;
    case Type::Prefix:
        _bas = std::make_shared<PrefixNode>("");
        break;
    case Type::Program:
        _bas = std::make_shared<ProgramNode>(nullptr);
        break;
    case Type::Region:
        _bas = std::make_shared<RegionNode>();
        break;
    case Type::Remove:
        _bas = std::make_shared<RemoveNode>("");
        break;
    case Type::Return:
        _bas = std::make_shared<ReturnNode>(nullptr);
        break;
    case Type::Statement:
        _bas = std::make_shared<StatementNode>();
        break;
    case Type::While:
        _bas = std::make_shared<WhileNode>(false);
        break;
	case Type::Group:
		_bas = std::make_shared<GroupNode>(nullptr);
		break;
    default:
        _bas = std::make_shared<ErrorNode>();
    }
    _bas->readFrom(is);
    return _bas;
}