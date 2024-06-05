#pragma once

#include "ast/scope.hpp"
#include "ast/error.hpp"
#include "program/vm_options.hpp"

class ProgramNode : public Node {
public:
    std::shared_ptr<Node> mainScope;
    ProgramNode(std::shared_ptr<Node> mainScope) : mainScope(mainScope), Node(Node::Type::Program) {}
public:
    std::string toString() const override {
        return "Program" + mainScope->toString();
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Program));
        BinaryOut::write_string(os, "MP.SAVE");
        BinaryOut::write_data<int>(os, VERSION_NUMBER);
        mainScope->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        std::string _sign = BinaryIn::read_string(is);
        if (_sign != "MP.SAVE") {
            illegal_program_error();
            mainScope = std::make_shared<ErrorNode>();
            return;
        }
        int _vnum = BinaryIn::read_data<int>(is);
        if (_vnum > VERSION_NUMBER || _vnum < 0) {
            illegal_program_error();
            mainScope = std::make_shared<ErrorNode>();
            return;
        }
        mainScope = get_node(is);
    }
};