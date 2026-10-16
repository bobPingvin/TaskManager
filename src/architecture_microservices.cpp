#include "architecture.hpp"
#include "logger.hpp"
#include "json_parser.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>

class MicroservicesArchitecture : public IArchitecture {
public:
    std::string getName() const override { return "Микросервисная (Microservices: POSIX fork+exec+FIFO)"; }

    void printTradeoffs() const override {
        std::cout << "\nПреимущества:\n"
                  << "  - Изоляция сбоев: падение одного микросервиса не ломает всю экосистему.\n"
                  << "  - Независимое масштабирование критических компонентов (Task vs Notify vs User).\n"
                  << "  - Свобода выбора технологического стека и автономность команд разработки.\n\n"
                  << "Недостатки:\n"
                  << "  - Высокая сложность сопровождения, отладки и мониторинга распределённых вызовов.\n"
                  << "  - Накладные расходы межпроцессного взаимодействия (IPC / FIFO / Network).\n"
                  << "  - Проблема согласованности распределённых данных (Eventual Consistency).\n";
    }

    bool startServices(const std::string& servicesConfig) override {
        Logger::info("[Оркестратор] Запуск микросервисов из конфига: " + servicesConfig);
        try {
            auto cfg = simple_json::Parser::parse_file(servicesConfig);
            if (!cfg.has("services")) {
                Logger::error("Некорректный конфиг services.json");
                return false;
            }
            std::cout << "\nЗарегистрированные микросервисы в кластере:\n";
            for (const auto& s : cfg["services"].as_array()) {
                std::cout << "  - Сервис: " << s["name"].as_string() 
                          << " | Исполняемый файл: " << s["executable"].as_string()
                          << " | Канал FIFO: " << s["fifo"].as_string() << "\n";
            }
        } catch (const std::exception& e) {
            Logger::error("Ошибка чтения конфигурации микросервисов: " + std::string(e.what()));
            return false;
        }
        return executeIpcFlow("ECHO", "Инициализация и проверка каналов сервисов");
    }

    bool addTask(const std::string& title, const std::string& desc, const std::string& user) override {
        return executeIpcFlow("ADD", title + "|" + desc + "|" + user);
    }

    bool deleteTask(int id) override {
        return executeIpcFlow("DELETE", std::to_string(id));
    }

    bool listTasks() override {
        return executeIpcFlow("LIST", "");
    }

    bool completeTask(int id) override {
        return executeIpcFlow("COMPLETE", std::to_string(id));
    }

private:
    bool executeIpcFlow(const std::string& cmd, const std::string& payload) {
        mkdir("ipc", 0777);
        std::string reqFifo = "ipc/task_req.fifo";
        std::string respFifo = "ipc/task_resp.fifo";

        mkfifo(reqFifo.c_str(), 0666);
        mkfifo(respFifo.c_str(), 0666);

        pid_t pid = fork();
        if (pid < 0) {
            Logger::error("Не удалось выполнить fork() сервиса");
            return false;
        }

        if (pid == 0) {
            char* args[] = { (char*)"bin/task_service", (char*)reqFifo.c_str(), (char*)respFifo.c_str(), nullptr };
            execvp(args[0], args);
            char* fallback[] = { (char*)"./task_service", (char*)reqFifo.c_str(), (char*)respFifo.c_str(), nullptr };
            execvp(fallback[0], fallback);
            _exit(127);
        }

        int wfd = open(reqFifo.c_str(), O_WRONLY);
        if (wfd >= 0) {
            std::string msg = cmd + ":" + payload + "\n";
            write(wfd, msg.c_str(), msg.size());
            close(wfd);
        }

        int rfd = open(respFifo.c_str(), O_RDONLY);
        std::string response;
        if (rfd >= 0) {
            char buf[512];
            ssize_t n = read(rfd, buf, sizeof(buf) - 1);
            if (n > 0) {
                buf[n] = '\0';
                response = buf;
            }
            close(rfd);
        }

        int status = 0;
        waitpid(pid, &status, 0);

        if (!response.empty()) {
            Logger::info("[IPC Ответ от TaskService]: " + response);
            return response.rfind("OK", 0) == 0;
        }
        return false;
    }
};

std::unique_ptr<IArchitecture> createMicroservicesArchitecture() {
    return std::make_unique<MicroservicesArchitecture>();
}
