cmake_minimum_required(VERSION 3.10)
project(task_manager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

include_directories(include)

# Главное приложение
add_executable(task-manager
    src/main.cpp
    src/json_parser.cpp
    src/task_repository.cpp
    src/architecture_monolith.cpp
    src/architecture_client_server.cpp
    src/architecture_microservices.cpp
    src/use_case_generator.cpp
    src/plugin_loader.cpp
)
target_link_libraries(task-manager ${CMAKE_DL_LIBS})
set_target_properties(task-manager PROPERTIES ENABLE_EXPORTS TRUE)

# Микросервисы
add_executable(task_service
    services/task_service.cpp
    src/task_repository.cpp
    src/json_parser.cpp
)
add_executable(user_service services/user_service.cpp)
add_executable(notify_service services/notify_service.cpp)

# Сборка плагинов (.so)
add_library(rule_scrum SHARED plugins/rule_scrum.cpp)
set_target_properties(rule_scrum PROPERTIES PREFIX "lib" SUFFIX ".so")

add_library(rule_waterfall SHARED plugins/rule_waterfall.cpp)
set_target_properties(rule_waterfall PROPERTIES PREFIX "lib" SUFFIX ".so")
