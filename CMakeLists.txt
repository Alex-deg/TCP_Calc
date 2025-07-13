cmake_minimum_required(VERSION 3.10)
project(TCP_CALCULATOR)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка для сервера
add_executable(server
    Server/server_main.cpp
    Server/server.cpp
    Server/server.h
)

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR})

# Настройка для клиента
add_executable(client
    Client/client_main.cpp
    Client/client.cpp
    Client/client.h
)

target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR})

# # Общие флаги компиляции для обоих проектов
# if(UNIX)
#     target_compile_options(server PRIVATE -Wall -Wextra -pedantic)
#     target_compile_options(client PRIVATE -Wall -Wextra -pedantic)
    
#     # Для Linux добавляем необходимые библиотеки
#     find_package(Threads REQUIRED)
#     target_link_libraries(server PRIVATE Threads::Threads)
#     target_link_libraries(client PRIVATE Threads::Threads)
# endif()