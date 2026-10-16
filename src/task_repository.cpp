#include "task_repository.hpp"
#include "logger.hpp"
#include <fstream>
#include <algorithm>

TaskRepository::TaskRepository(std::string filePath) : filePath_(std::move(filePath)) {
    load();
}

bool TaskRepository::load() {
    tasks_.clear();
    nextId_ = 1;
    try {
        auto root = simple_json::Parser::parse_file(filePath_);
        if (root.type != simple_json::Type::Array) return false;
        for (const auto& item : root.as_array()) {
            Task t;
            if (item.has("id")) t.id = item["id"].as_int();
            if (item.has("title")) t.title = item["title"].as_string();
            if (item.has("description")) t.description = item["description"].as_string();
            if (item.has("completed")) t.completed = item["completed"].as_bool();
            if (item.has("assigned_user")) t.assigned_user = item["assigned_user"].as_string();
            tasks_.push_back(t);
            if (t.id >= nextId_) nextId_ = t.id + 1;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool TaskRepository::save() {
    std::ofstream f(filePath_);
    if (!f.is_open()) return false;
    f << "[\n";
    for (size_t i = 0; i < tasks_.size(); ++i) {
        const auto& t = tasks_[i];
        f << "  {\n";
        f << "    \"id\": " << t.id << ",\n";
        f << "    \"title\": \"" << t.title << "\",\n";
        f << "    \"description\": \"" << t.description << "\",\n";
        f << "    \"completed\": " << (t.completed ? "true" : "false") << ",\n";
        f << "    \"assigned_user\": \"" << t.assigned_user << "\"\n";
        f << "  }" << (i + 1 < tasks_.size() ? "," : "") << "\n";
    }
    f << "]\n";
    return true;
}

bool TaskRepository::add(const std::string& title, const std::string& desc, const std::string& user) {
    if (title.empty()) return false;
    Task t{nextId_++, title, desc, false, user.empty() ? "user" : user};
    tasks_.push_back(t);
    return save();
}

bool TaskRepository::remove(int id) {
    auto it = std::remove_if(tasks_.begin(), tasks_.end(), [id](const Task& t) { return t.id == id; });
    if (it == tasks_.end()) return false;
    tasks_.erase(it, tasks_.end());
    return save();
}

bool TaskRepository::markCompleted(int id) {
    for (auto& t : tasks_) {
        if (t.id == id) {
            t.completed = true;
            return save();
        }
    }
    return false;
}

const Task* TaskRepository::findById(int id) const {
    for (const auto& t : tasks_) {
        if (t.id == id) return &t;
    }
    return nullptr;
}
