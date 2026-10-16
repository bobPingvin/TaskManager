#include <iostream>
#include <fstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "task_repository.hpp"

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;
    std::string inFifo = argv[1];
    std::string outFifo = argv[2];

    int rfd = open(inFifo.c_str(), O_RDONLY);
    if (rfd < 0) return 1;

    char buf[512];
    ssize_t n = read(rfd, buf, sizeof(buf) - 1);
    close(rfd);

    if (n <= 0) return 0;
    buf[n] = '\0';
    std::string msg(buf);
    if (!msg.empty() && msg.back() == '\n') msg.pop_back();

    size_t colon = msg.find(':');
    std::string cmd = (colon != std::string::npos) ? msg.substr(0, colon) : msg;
    std::string payload = (colon != std::string::npos) ? msg.substr(colon + 1) : "";

    TaskRepository repo("data/tasks.json");
    std::string reply = "ERR: Неизвестная команда";

    if (cmd == "ECHO") {
        reply = "OK: Сервис задач активен и готов к обработке";
    } else if (cmd == "ADD") {
        size_t p1 = payload.find('|');
        size_t p2 = (p1 != std::string::npos) ? payload.find('|', p1 + 1) : std::string::npos;
        std::string title = (p1 != std::string::npos) ? payload.substr(0, p1) : payload;
        std::string desc = (p2 != std::string::npos) ? payload.substr(p1 + 1, p2 - p1 - 1) : "Без описания";
        std::string user = (p2 != std::string::npos) ? payload.substr(p2 + 1) : "student";
        if (repo.add(title, desc, user)) reply = "OK: Задача добавлена через Microservice";
        else reply = "ERR: Ошибка валидации задачи";
    } else if (cmd == "DELETE") {
        int id = std::stoi(payload);
        if (repo.remove(id)) reply = "OK: Задача удалена";
        else reply = "ERR: ID не найден";
    } else if (cmd == "COMPLETE") {
        int id = std::stoi(payload);
        if (repo.markCompleted(id)) reply = "OK: Задача выполнена";
        else reply = "ERR: ID не найден";
    } else if (cmd == "LIST") {
        reply = "OK: Всего задач: " + std::to_string(repo.getAll().size());
    }

    int wfd = open(outFifo.c_str(), O_WRONLY);
    if (wfd >= 0) {
        write(wfd, reply.c_str(), reply.size());
        close(wfd);
    }
    return 0;
}
