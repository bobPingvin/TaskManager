#include "architecture.hpp"
#include "logger.hpp"
#include <iostream>

class ClientServerArchitecture : public IArchitecture {
public:
    std::string getName() const override { return "Клиент-серверная (Client-Server)"; }

    void printTradeoffs() const override {
        std::cout << "\nПреимущества:\n"
                  << "  - Разделение ответственности: тонкий клиент (UI/CLI) и централизованный сервер логики/БД.\n"
                  << "  - Возможность независимого обновления клиентских приложений.\n"
                  << "  - Централизованный аудит, безопасность и контроль доступа.\n\n"
                  << "Недостатки:\n"
                  << "  - Сетевые накладные расходы и сериализация запросов (API latency).\n"
                  << "  - Сервер является единой точкой отказа (Single Point of Failure).\n"
                  << "  - Необходимость версионирования протоколов взаимодействия.\n";
    }

    bool addTask(const std::string& title, const std::string& desc, const std::string& user) override {
        Logger::info("[Клиент] Формирование запроса POST /api/tasks...");
        Logger::info("[Сервер] Аутентификация клиента, валидация DTO, запись в БД...");
        TaskRepository repo;
        bool ok = repo.add(title, desc, user);
        if (ok) Logger::info("[Сервер -> Клиент] 201 Created. Задача зарегистрирована.");
        return ok;
    }

    bool deleteTask(int id) override {
        Logger::info("[Клиент] Отправка запроса DELETE /api/tasks/" + std::to_string(id));
        TaskRepository repo;
        bool ok = repo.remove(id);
        if (ok) Logger::info("[Сервер -> Клиент] 200 OK. Задача успешно удалена из БД.");
        else Logger::warn("[Сервер -> Клиент] 404 Not Found. Идентификатор задачи не существует.");
        return ok;
    }

    bool listTasks() override {
        Logger::info("[Клиент] GET /api/tasks -> Запрос списка...");
        TaskRepository repo;
        const auto& list = repo.getAll();
        std::cout << "\n--- Список задач (Клиент-серверный ответ API) ---\n";
        if (list.empty()) {
            std::cout << "(Данные отсутствуют на сервере)\n";
            return true;
        }
        for (const auto& t : list) {
            std::cout << "[HTTP 200] ID: " << t.id 
                      << " | Статус: " << (t.completed ? "COMPLETED" : "PENDING")
                      << " | Исполнитель: " << t.assigned_user
                      << " | Заголовок: " << t.title << "\n";
        }
        return true;
    }

    bool completeTask(int id) override {
        Logger::info("[Клиент] PATCH /api/tasks/" + std::to_string(id) + "/complete");
        TaskRepository repo;
        bool ok = repo.markCompleted(id);
        if (ok) Logger::info("[Сервер] Статус обновлен. Оповещение подписчиков сокетов.");
        else Logger::warn("[Сервер] 404 Not Found. Невозможно обновить несуществующую задачу.");
        return ok;
    }
};

std::unique_ptr<IArchitecture> createClientServerArchitecture() {
    return std::make_unique<ClientServerArchitecture>();
}
