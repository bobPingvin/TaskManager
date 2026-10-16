#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>

namespace simple_json {

enum class Type { Null, Boolean, Number, String, Array, Object };

class Value {
public:
    Type type = Type::Null;
    bool b_val = false;
    double n_val = 0.0;
    std::string s_val;
    std::vector<Value> arr;
    std::map<std::string, Value> obj;

    Value() = default;
    static Value from_bool(bool b) { Value v; v.type = Type::Boolean; v.b_val = b; return v; }
    static Value from_num(double n) { Value v; v.type = Type::Number; v.n_val = n; return v; }
    static Value from_str(const std::string& s) { Value v; v.type = Type::String; v.s_val = s; return v; }

    bool as_bool() const {
        if (type != Type::Boolean) throw std::runtime_error("JSON: Ожидался boolean");
        return b_val;
    }
    double as_number() const {
        if (type != Type::Number) throw std::runtime_error("JSON: Ожидалось число");
        return n_val;
    }
    int as_int() const { return static_cast<int>(as_number()); }
    std::string as_string() const {
        if (type != Type::String) throw std::runtime_error("JSON: Ожидалась строка");
        return s_val;
    }
    const std::vector<Value>& as_array() const {
        if (type != Type::Array) throw std::runtime_error("JSON: Ожидался массив");
        return arr;
    }
    const std::map<std::string, Value>& as_object() const {
        if (type != Type::Object) throw std::runtime_error("JSON: Ожидался объект");
        return obj;
    }
    bool has(const std::string& key) const {
        return type == Type::Object && obj.find(key) != obj.end();
    }
    const Value& operator[](const std::string& key) const {
        if (type != Type::Object) throw std::runtime_error("JSON: Доступ по ключу не для объекта: " + key);
        auto it = obj.find(key);
        if (it == obj.end()) throw std::runtime_error("JSON: Ключ не найден: " + key);
        return it->second;
    }
};

class Parser {
public:
    static Value parse(const std::string& text);
    static Value parse_file(const std::string& path);

private:
    std::string src;
    size_t pos = 0;

    explicit Parser(std::string s) : src(std::move(s)) {}
    void skip_ws();
    char peek();
    char get();
    bool match(char expected);

    Value parse_val();
    Value parse_obj();
    Value parse_arr();
    Value parse_str();
    Value parse_num();
    Value parse_lit();
};

} // namespace simple_json
