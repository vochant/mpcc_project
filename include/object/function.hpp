#pragma once

#include "object/executable.hpp"
#include "env/environment.hpp"

class Function : public Executable {
private:
    size_t position;
    std::shared_ptr<Environment> env;
public:
    Function(size_t position, std::shared_ptr<Environment> env);
    std::shared_ptr<Object> call(std::vector<std::shared_ptr<Object>> args) override;
    std::shared_ptr<Object> make_copy() override;
    std::string toString() override;
};