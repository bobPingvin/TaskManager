#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include "logger.hpp"
#include "architecture.hpp"
#include "use_case_generator.hpp"
#include "plugin_loader.hpp"

std::unique_ptr<IArchitecture> createMonolithArchitecture();
std::unique_ptr<IArchitecture> createClientServerArchitecture();
std::unique_ptr<IArchitecture> createMicroservicesArchitecture();

static void printHelp() {
    std::cout << "Использование: task-manager [ОПЦИИ]\n\n"
              << "Опции:\n"
              << "  --architecture <monolith|client-server|microservices>\n"
              << "      Выбор архитектурного режима.\n"
              << "  --action <add|delete|list|complete>\n"
              << "      Операция над задачами.\n"
              << "  --title <строка>   Заголовок задачи (для add)\n"
              << "  --desc <строка>    Описание задачи (для add)\n"
              << "  --id <число>       ID задачи (для delete, complete)\n"
              << "  --start <файл>     Запуск микросервисов из JSON конфига\n"
              << "  --plugins          Проверить рекомендации загруженных .so плагинов\n"
              << "  --help             Показать справку\n";
}

int main(int argc, char* argv[]) {
    Logger::init("logs/task_manager.log");

    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::string archStr = "monolith";
    std::string action = "list";
    std::string title = "Тестовая задача";
    std::string desc = "Описание задачи";
    std::string user = "student";
    std::string startConfigFile;
    int targetId = -1;
    bool checkPlugins = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--architecture" && i + 1 < argc) archStr = argv[++i];
        else if (arg == "--action" && i + 1 < argc) action = argv[++i];
        else if (arg == "--title" && i + 1 < argc) title = argv[++i];
        else if (arg == "--desc" && i + 1 < argc) desc = argv[++i];
        else if (arg == "--id" && i + 1 < argc) targetId = std::stoi(argv[++i]);
        else if (arg == "--start" && i + 1 < argc) startConfigFile = argv[++i];
        else if (arg == "--plugins") checkPlugins = true;
        else if (arg == "--help") { printHelp(); return 0; }
    }

    std::cout << "\n=== Task Manager (ТРПО ИСП-43) ===\n";

    ArchType archType = ArchType::Monolith;
    std::unique_ptr<IArchitecture> arch;

    if (archStr == "monolith") {
        archType = ArchType::Monolith;
        arch = createMonolithArchitecture();
    } else if (archStr == "client-server" || archStr == "client_server") {
        archType = ArchType::ClientServer;
        arch = createClientServerArchitecture();
    } else if (archStr == "microservices") {
        archType = ArchType::Microservices;
        arch = createMicroservicesArchitecture();
    } else {
        std::cerr << "Ошибка: Неизвестная архитектура: " << archStr << "\n";
        return 1;
    }

    std::cout << "Выбранная архитектура: " << arch->getName() << "\n";
    arch->printTradeoffs();

    // Генерация и экспорт Use Case диаграммы
    std::cout << "\n" << UseCaseGenerator::generateText(archType) << "\n";
    UseCaseGenerator::exportMarkdown(archType, "output/use-case-diagram.md");

    // Запуск микросервисов, если указан конфиг
    if (!startConfigFile.empty() && archType == ArchType::Microservices) {
        arch->startServices(startConfigFile);
    }

    // Выполнение команд над задачами
    if (action == "add") {
        arch->addTask(title, desc, user);
    } else if (action == "delete") {
        if (targetId <= 0) {
            std::cerr << "Ошибка: Укажите корректный --id для удаления\n";
        } else {
            arch->deleteTask(targetId);
        }
    } else if (action == "complete") {
        if (targetId <= 0) {
            std::cerr << "Ошибка: Укажите корректный --id для отметки\n";
        } else {
            arch->completeTask(targetId);
        }
    } else if (action == "list") {
        arch->listTasks();
    }

    // Проверка работы динамических плагинов
    if (checkPlugins) {
        std::cout << "\n--- Загрузка пользовательских правил (Плагины) ---\n";
        PluginLoader loader;
        loader.loadPlugins("plugins");
        for (const auto& r : loader.getRules()) {
            std::cout << "\n[Правило]: " << r->name() << "\n"
                      << "Описание: " << r->description() << "\n"
                      << "Оценка: " << r->evaluate(5, false, true) << "\n";
        }
    }

    return 0;
}
