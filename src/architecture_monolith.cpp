#include "architecture.hpp"
#include "logger.hpp"
#include <iostream>
#include <iomanip>

class MonolithArchitecture : public IArchitecture {
public:
    std::string getName() const override { return "Монолитная (Monolith)"; }

    void printTradeoffs() const override {
        std::cout << "\nПреимущества:\n"
                  << "  - Простота разработки, тестирования и развертывания (единый исполняемый файл).\n"
                  << "  - Максимальная производительность: прямые вызовы функций в памяти без IPC/сети.\n"
                  << "  - Единая согласованная база данных и транзакционность.\n\n"
                  << "Недостатки:\n"
                  << "  - Высокая сложность горизонтального масштабирования отдельных узлов.\n"
                  << "  - Сильная связность компонентов: ошибка в модуле способна уронить всю систему.\n"
                  << "  - Замедление релизных циклов при росте размера команды.\n";
    }

    bool addTask(const std::string& title, const std::string& desc, const std::string& user) override {
        TaskRepository repo;
        bool ok = repo.add(title, desc, user);
        if (ok) Logger::info("[Монолит] Задача '" + title + "' успешно добавлена в единое хранилище.");
        return ok;
    }

    bool deleteTask(int id) override {
        TaskRepository repo;
        bool ok = repo.remove(id);
        if (ok) Logger::info("[Монолит] Задача ID " + std::to_string(id) + " удалена.");
        else Logger::warn("[Монолит] Ошибка: Задача ID " + std::to_string(id) + " не найдена.");
        return ok;
    }

    bool listTasks() override {
        TaskRepository repo;
        const auto& list = repo.getAll();
        std::cout << "\n--- Список задач (Монолитная архитектура) ---\n";
        if (list.empty()) {
            std::cout << "(Список задач пуст)\n";
            return true;
        }
        for (const auto& t : list) {
            std::cout << "[" << (t.completed ? "X" : " ") << "] #" << t.id 
                      << " " << t.title << " (" << t.assigned_user << "): " << t.description << "\n";
        }
        return true;
    }

    bool completeTask(int id) override {
        TaskRepository repo;
        bool ok = repo.markCompleted(id);
        if (ok) Logger::info("[Монолит] Задача ID " + std::to_string(id) + " отмечена как выполненная.");
        else Logger::warn("[Монолит] Ошибка: Задача ID " + std::to_string(id) + " не найдена.");
        return ok;
    }
};

std::unique_ptr<IArchitecture> createMonolithArchitecture() {
    return std::make_unique<MonolithArchitecture>();
}
