#include "json_parser.hpp"
#include <fstream>
#include <cctype>
#include <cstdlib>

namespace simple_json {

void Parser::skip_ws() {
    while (pos < src.size()) {
        char c = src[pos];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') pos++;
        else break;
    }
}

char Parser::peek() {
    skip_ws();
    return (pos < src.size()) ? src[pos] : '\0';
}

char Parser::get() {
    skip_ws();
    return (pos < src.size()) ? src[pos++] : '\0';
}

bool Parser::match(char expected) {
    if (peek() == expected) {
        get();
        return true;
    }
    return false;
}

Value Parser::parse_str() {
    if (get() != '"') throw std::runtime_error("JSON: Ожидалась кавычка");
    std::string s;
    while (pos < src.size()) {
        char c = src[pos++];
        if (c == '"') return Value::from_str(s);
        if (c == '\\' && pos < src.size()) {
            char esc = src[pos++];
            if (esc == 'n') s += '\n';
            else if (esc == 't') s += '\t';
            else s += esc;
        } else {
            s += c;
        }
    }
    throw std::runtime_error("JSON: Незакрытая строка");
}

Value Parser::parse_num() {
    size_t start = pos;
    if (src[pos] == '-') pos++;
    while (pos < src.size() && (std::isdigit(src[pos]) || src[pos] == '.')) pos++;
    char* endptr = nullptr;
    std::string num_str = src.substr(start, pos - start);
    double val = std::strtod(num_str.c_str(), &endptr);
    return Value::from_num(val);
}

Value Parser::parse_lit() {
    std::string token;
    while (pos < src.size() && std::isalpha(src[pos])) token += src[pos++];
    if (token == "true") return Value::from_bool(true);
    if (token == "false") return Value::from_bool(false);
    if (token == "null") return Value();
    throw std::runtime_error("JSON: Неизвестный литерал " + token);
}

Value Parser::parse_arr() {
    if (get() != '[') throw std::runtime_error("JSON: Ожидался символ '['");
    Value v; v.type = Type::Array;
    if (match(']')) return v;
    while (true) {
        v.arr.push_back(parse_val());
        if (match(']')) break;
        if (!match(',')) throw std::runtime_error("JSON: Ожидалась запятая в массиве");
    }
    return v;
}

Value Parser::parse_obj() {
    if (get() != '{') throw std::runtime_error("JSON: Ожидался символ '{'");
    Value v; v.type = Type::Object;
    if (match('}')) return v;
    while (true) {
        if (peek() != '"') throw std::runtime_error("JSON: Ключ должен быть строкой");
        std::string key = parse_str().as_string();
        if (!match(':')) throw std::runtime_error("JSON: Ожидалось ':' после ключа");
        v.obj[key] = parse_val();
        if (match('}')) break;
        if (!match(',')) throw std::runtime_error("JSON: Ожидалась запятая в объекте");
    }
    return v;
}

Value Parser::parse_val() {
    char c = peek();
    if (c == '{') return parse_obj();
    if (c == '[') return parse_arr();
    if (c == '"') return parse_str();
    if (c == '-' || std::isdigit(c)) return parse_num();
    if (std::isalpha(c)) return parse_lit();
    throw std::runtime_error("JSON: Неожиданный символ: " + std::string(1, c));
}

Value Parser::parse(const std::string& text) {
    Parser p(text);
    Value root = p.parse_val();
    return root;
}

Value Parser::parse_file(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) throw std::runtime_error("Не удалось открыть файл: " + path);
    std::string text((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return parse(text);
}

} // namespace simple_json
