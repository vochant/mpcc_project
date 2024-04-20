#pragma once

#include "ast/region.hpp"
#include "ast/error.hpp"
#include "program/vm_options.hpp"

class ProgramNode : public Node {
public:
    std::shared_ptr<Node> mainRegion;
    ProgramNode(std::shared_ptr<Node> mainRegion) : mainRegion(mainRegion), Node(Node::Type::Program) {}
public:
    std::string toString() const override {
        return "Program" + mainRegion->toString();
    }

    void storeInto(std::ostream& os) const override {
        BinaryOut::write_byte(os, char(Node::Type::Program));
        BinaryOut::write_string(os, "MP.SAVE");
        BinaryOut::write_data<int>(os, VERSION_NUMBER);
        mainRegion->storeInto(os);
    }

    void readFrom(std::istream& is) override {
        std::string _sign = BinaryIn::read_string(is);
        if (_sign != "MP.SAVE") {
            illegal_program_error();
            mainRegion = std::make_shared<ErrorNode>();
            return;
        }
        int _vnum = BinaryIn::read_data<int>(is);
        if (_vnum > VERSION_NUMBER || _vnum < 0) {
            illegal_program_error();
            mainRegion = std::make_shared<ErrorNode>();
            return;
        }
        mainRegion = get_node(is);
    }
};