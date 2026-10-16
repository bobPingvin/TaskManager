#pragma once
#include <string>

class ArchitectureRule {
public:
    virtual ~ArchitectureRule() = default;
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual std::string evaluate(int teamSize, bool requiresHighLoad, bool needFastMVP) const = 0;
};

extern "C" ArchitectureRule* create_rule();
