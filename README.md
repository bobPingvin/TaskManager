# Task Manager — Учебный проект ТРПО (ИСП-43)

Консольное приложение для моделирования системы управления задачами (To-Do List) с поддержкой трёх архитектурных паттернов: **монолитного**, **клиент-серверного** и **микросервисного**.

Разработано специально для **ALT Linux** без использования сторонних библиотек (только стандартная библиотека C++17 и POSIX API).

---

## 1. Структура проекта

```text
~/task-manager/
├── CMakeLists.txt              # Сборка проекта, сервисов и плагинов
├── README.md                   # Документация проекта
├── include/                    # Заголовочные файлы (.hpp)
├── src/                        # Исходный код ядра и архитектур
│   ├── main.cpp                # Точка входа и CLI-парсер
│   ├── json_parser.cpp         # Собственный рекурсивный JSON-парсер
│   ├── task_repository.cpp     # Хранилище задач
│   ├── architecture_monolith.cpp
│   ├── architecture_client_server.cpp
│   ├── architecture_microservices.cpp
│   ├── use_case_generator.cpp  # Генератор UML Use Case диаграмм
│   └── plugin_loader.cpp       # Загрузчик .so через dlopen/dlsym
├── services/                   # Отдельные микросервисы (fork + execvp)
│   ├── task_service.cpp        # Сервис задач
│   ├── user_service.cpp        # Сервис пользователей
│   └── notify_service.cpp      # Сервис уведомлений
├── plugins/                    # Динамические библиотеки правил (.so)
│   ├── rule_scrum.cpp          # Плагин гибкой разработки
│   └── rule_waterfall.cpp      # Плагин каскадной модели (ГОСТ 19)
├── data/                       # Данные и конфигурации
│   ├── tasks.json              # Файл задач
│   └── services.json           # Конфигурация сервисов
├── tests/
│   └── run_tests.sh            # Скрипт сборки и тестирования
├── ipc/                        # Именованные каналы (FIFO) для обмена
├── logs/                       # Логи работы системы и оповещений
└── output/
    └── use-case-diagram.md     # Экспортированная диаграмма прецедентов
```

---

## 2. Сборка и автоматический запуск тестов

Для полной компиляции программы, фоновых микросервисов, разделяемых библиотек (`.so`) и прогона всех проверок запустите:

```bash
chmod +x tests/run_tests.sh
./tests/run_tests.sh
```

---

## 3. Примеры использования CLI

### 3.1. Монолитная архитектура
Вся бизнес-логика и хранилище работают напрямую в одном процессе в оперативной памяти:
```bash
# Просмотр задач
./task-manager --architecture monolith --action list

# Добавление новой задачи
./task-manager --architecture monolith --action add --title "Купить учебник" --desc "По программированию"

# Отметка о выполнении задачи (ID=1)
./task-manager --architecture monolith --action complete --id 1

# Удаление задачи (ID=2)
./task-manager --architecture monolith --action delete --id 2
```

### 3.2. Клиент-серверная архитектура
Эмуляция сетевого взаимодействия через слой DTO/API-контроллера с централизованной базой:
```bash
./task-manager --architecture client-server --action list
./task-manager --architecture client-server --action add --title "Сдать лабораторную" --desc "До дедлайна"
```

### 3.3. Микросервисная архитектура (POSIX fork + execvp + mkfifo)
Запуск независимых процессов сервисов с передачей сообщений через именованные каналы FIFO:
```bash
./task-manager --architecture microservices --start data/services.json --action add --title "Тикет в Jira" --desc "Создан через IPC канал"
```

---

## 4. Динамические плагины (.so)

Система поддерживает расширение логики выбора архитектуры через библиотеки, загружаемые во время работы с помощью `dlopen()` и `dlsym()`:
```bash
./task-manager --architecture monolith --plugins
```

Экспортируемая функция плагинов:
```cpp
extern "C" ArchitectureRule* create_rule();
```

---

## 5. Артефакты работы
- **Диаграмма вариантов использования:** автоматически сохраняется в `output/use-case-diagram.md` при каждом запуске.
- **Логи приложения:** пишутся в `logs/task_manager.log`.
- **Логи уведомлений микросервисов:** пишутся в `logs/alerts.log`.
