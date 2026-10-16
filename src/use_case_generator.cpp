#include "use_case_generator.hpp"
#include "logger.hpp"
#include <fstream>
#include <sstream>

std::string UseCaseGenerator::generateText(ArchType type) {
    std::ostringstream ss;
    ss << "=========================================================\n"
       << "  ДИАГРАММА ВАРИАНТОВ ИСПОЛЬЗОВАНИЯ (USE CASE DIAGRAM)\n"
       << "=========================================================\n\n";

    if (type == ArchType::Monolith) {
        ss << "  [ Актёр: Пользователь (User) ]\n"
           << "                 │\n"
           << "                 ├── (UC-1: Добавить задачу)\n"
           << "                 ├── (UC-2: Удалить задачу)\n"
           << "                 ├── (UC-3: Просмотреть список задач)\n"
           << "                 └── (UC-4: Отметить выполнение задачи)\n"
           << "                                 │\n"
           << "                 ┌───────────────▼────────────────┐\n"
           << "                 │     Система: Монолит To-Do     │\n"
           << "                 │ (Вся бизнес-логика и хранилище)│\n"
           << "                 └────────────────────────────────┘\n";
    } else if (type == ArchType::ClientServer) {
        ss << "  [ Актёр: Пользователь ] ──> [ CLI / Тонкий клиент ]\n"
           << "                                      │\n"
           << "                             ( Сетевой REST API / IPC )\n"
           << "                                      │\n"
           << "                                      ▼\n"
           << "                   ┌──────────────────────────────────────┐\n"
           << "                   │ Сервер: Аутентификация, Сервисный    │\n"
           << "                   │ слой, Валидация и Центральная БД     │\n"
           << "                   └──────────────────────────────────────┘\n";
    } else {
        ss << "  [ Актёр: Пользователь ]\n"
           << "             │\n"
           << "             ▼\n"
           << "  [ API Gateway / Оркестратор ]\n"
           << "        │              │               │\n"
           << "   (FIFO / IPC)   (FIFO / IPC)    (FIFO / IPC)\n"
           << "        ▼              ▼               ▼\n"
           << "  (Task Service) (User Service) (Notify Service)\n"
           << "        │              │               │\n"
           << "  [ tasks.json ] [ users.json ]  [ logs/alerts.log ]\n";
    }
    return ss.str();
}

bool UseCaseGenerator::exportMarkdown(ArchType type, const std::string& outPath) {
    std::ofstream f(outPath);
    if (!f.is_open()) return false;

    f << "# Use Case Diagram: Task Manager System\n\n";
    f << "```text\n";
    f << generateText(type);
    f << "```\n\n";

    f << "## Спецификация актеров и прецедентов\n";
    f << "- **Основной актёр:** Пользователь (User / QA / Developer).\n";
    f << "- **Прецеденты:**\n";
    f << "  1. **Add Task:** Создание новой задачи с валидацией входных данных.\n";
    f << "  2. **Delete Task:** Удаление задачи по уникальному ID.\n";
    f << "  3. **View Tasks:** Просмотр списка задач с фильтрацией и статусом.\n";
    f << "  4. **Mark Completed:** Изменение жизненного цикла задачи на 'Завершено'.\n";

    Logger::info("Диаграмма сохранена в файл: " + outPath);
    return true;
}
