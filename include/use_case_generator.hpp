#pragma once
#include <string>
#include "architecture.hpp"

class UseCaseGenerator {
public:
    static std::string generateText(ArchType type);
    static bool exportMarkdown(ArchType type, const std::string& outPath);
};
