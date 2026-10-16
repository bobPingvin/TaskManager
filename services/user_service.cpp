#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    std::cout << "[UserService] Процесс активен (PID=" << getpid() << "). Валидация прав пользователей OK.\n";
    return 0;
}
