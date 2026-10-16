#pragma once
#include <string>
#include <vector>
#include "json_parser.hpp"

struct Task {
    int id = 0;
    std::string title;
    std::string description;
    bool completed = false;
    std::string assigned_user;
};

class TaskRepository {
public:
    explicit TaskRepository(std::string filePath = "data/tasks.json");
    bool load();
    bool save();

    const std::vector<Task>& getAll() const { return tasks_; }
    bool add(const std::string& title, const std::string& desc, const std::string& user = "student");
    bool remove(int id);
    bool markCompleted(int id);
    const Task* findById(int id) const;

private:
    std::string filePath_;
    std::vector<Task> tasks_;
    int nextId_ = 1;
};
