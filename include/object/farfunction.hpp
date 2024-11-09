#pragma once

#include "object/executable.hpp"
#include "asm/asm.hpp"
#include "env/environment.hpp"

class FarFunction : public Executable {
private:
    std::vector<std::shared_ptr<Asm>> code;
    std::shared_ptr<Environment> env;
public:
    FarFunction(std::vector<std::shared_ptr<Asm>> code, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> args) override;
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};