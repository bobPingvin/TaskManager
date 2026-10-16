#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    std::string alert = (argc > 1) ? argv[1] : "Новое событие в системе управления задачами";
    std::ofstream f("logs/alerts.log", std::ios::app);
    if (f.is_open()) {
        f << "[ALERT] " << alert << std::endl;
    }
    std::cout << "[NotifyService] Оповещение отправлено: " << alert << "\n";
    return 0;
}
