#pragma once

#include "object/executable.hpp"

class MemberFunction : public Object {
private:
    std::shared_ptr<Executable> exec;
public:
    std::shared_ptr<Executable> apply(std::shared_ptr<Environment> env, long long id);
    std::string toString() override;
    std::shared_ptr<Object> make_copy();
    MemberFunction(std::shared_ptr<Executable> exec);
};