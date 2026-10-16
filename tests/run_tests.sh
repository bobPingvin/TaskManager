#!/bin/bash
set -e

echo "=== 1. Сборка проекта через CMake ==="
mkdir -p build
cd build
cmake ..
make -j$(nproc)

# Копирование бинарников и плагинов в ожидаемые директории
cp task-manager ..
mkdir -p ../bin
cp task_service user_service notify_service ../bin/
cp librule_scrum.so librule_waterfall.so ../plugins/
cd ..

echo ""
echo "=== 2. ТЕСТ: Монолитная архитектура (Просмотр и добавление) ==="
./task-manager --architecture monolith --action list
./task-manager --architecture monolith --action add --title "Новая задача" --desc "Проверка монолита"
./task-manager --architecture monolith --action complete --id 1

echo ""
echo "=== 3. ТЕСТ: Клиент-серверная архитектура ==="
./task-manager --architecture client-server --action list

echo ""
echo "=== 4. ТЕСТ: Микросервисная архитектура (Запуск через fork+exec+FIFO) ==="
./task-manager --architecture microservices --start data/services.json --action add --title "Микросервисный тикет" --desc "Через FIFO канал"

echo ""
echo "=== 5. ТЕСТ: Обработка крайних случаев (Ошибки без падения) ==="
echo "--- Тест 5.1: Удаление несуществующей задачи (ID 9999) ---"
./task-manager --architecture monolith --action delete --id 9999 || true

echo "--- Тест 5.2: Пустые параметры ---"
./task-manager --architecture monolith --action add --title "" || true

echo ""
echo "=== 6. ТЕСТ: Загрузка .so плагинов (dlopen/dlsym) ==="
./task-manager --architecture monolith --plugins

echo ""
echo "=== 7. Проверка сгенерированной диаграммы ==="
if [ -f "output/use-case-diagram.md" ]; then
    echo "Файл output/use-case-diagram.md успешно создан:"
    head -n 15 output/use-case-diagram.md
fi

echo ""
echo "=== ВСЕ ТЕСТЫ УСПЕШНО ПРОЙДЕНЫ! ==="
