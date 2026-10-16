#include "architecture_rule.hpp"

class ScrumRule : public ArchitectureRule {
public:
    std::string name() const override {
        return "Scrum/Agile Правило гибкой разработки";
    }
    std::string description() const override {
        return "Рекомендует архитектуру для небольших команд с частыми итерациями.";
    }
    std::string evaluate(int teamSize, bool requiresHighLoad, bool needFastMVP) const override {
        if (needFastMVP && teamSize <= 5) {
            return "Рекомендация: Монолитная архитектура (минимальное время до первого релиза MVP).";
        }
        if (requiresHighLoad) {
            return "Рекомендация: Микросервисная архитектура с независимыми командами спринтов.";
        }
        return "Рекомендация: Клиент-серверная архитектура.";
    }
};

extern "C" ArchitectureRule* create_rule() {
    return new ScrumRule();
}
