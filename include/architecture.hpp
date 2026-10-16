#pragma once
#include <string>
#include <vector>
#include "task_repository.hpp"

enum class ArchType { Monolith, ClientServer, Microservices };

class IArchitecture {
public:
    virtual ~IArchitecture() = default;
    virtual std::string getName() const = 0;
    virtual void printTradeoffs() const = 0;
    virtual bool addTask(const std::string& title, const std::string& desc, const std::string& user) = 0;
    virtual bool deleteTask(int id) = 0;
    virtual bool listTasks() = 0;
    virtual bool completeTask(int id) = 0;
    virtual bool startServices(const std::string& /*servicesConfig*/) { return false; }
};
