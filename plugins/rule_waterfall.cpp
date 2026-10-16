#include "architecture_rule.hpp"

class WaterfallRule : public ArchitectureRule {
public:
    std::string name() const override {
        return "Каскадное правило (Waterfall/ГОСТ 19)";
    }
    std::string description() const override {
        return "Анализирует требования к жесткой регламентации и стабильности документации.";
    }
    std::string evaluate(int teamSize, bool /*requiresHighLoad*/, bool /*needFastMVP*/) const override {
        if (teamSize > 15) {
            return "Рекомендация: Клиент-серверная архитектура с формализованным API контрактом.";
        }
        return "Рекомендация: Монолитная система с полным комплектом документации ЕСПД (ГОСТ 19).";
    }
};

extern "C" ArchitectureRule* create_rule() {
    return new WaterfallRule();
}
