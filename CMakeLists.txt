cmake_minimum_required(VERSION 3.16)

project(OmegaChess
        VERSION 0.1.0
        LANGUAGES CXX
)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Автоматическая обработка moc/uic/rcc для Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ----------------------------------------------------------------------
# Qt: по умолчанию Qt6. Если используете Qt5 — см. комментарий ниже.
# ----------------------------------------------------------------------
find_package(Qt6 COMPONENTS Widgets REQUIRED)
# Для Qt5 вместо этого:
# find_package(Qt5 COMPONENTS Widgets REQUIRED)

# ----------------------------------------------------------------------
# Список исходников
# ----------------------------------------------------------------------
set(OMEGA_LOGIC_SOURCES
        logic/Board.cpp
)

set(OMEGA_GUI_SOURCES
        gui/MainWindow.cpp
        gui/BoardView.cpp
)

set(OMEGA_ALL_SOURCES
        main.cpp
        ${OMEGA_LOGIC_SOURCES}
        ${OMEGA_GUI_SOURCES}
        controller/GameController.cpp
        logic/PieceColor.hpp
        logic/PieceKind.hpp
)

# ----------------------------------------------------------------------
# Основной исполняемый файл
# ----------------------------------------------------------------------
add_executable(OmegaChess
        ${OMEGA_ALL_SOURCES}
)

target_include_directories(OmegaChess
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/logic
        ${CMAKE_CURRENT_SOURCE_DIR}/gui
        ${CMAKE_CURRENT_SOURCE_DIR}/controller
)

target_link_libraries(OmegaChess
        PRIVATE
        Qt6::Widgets
)
# Для Qt5:
# target_link_libraries(OmegaChess PRIVATE Qt5::Widgets)

# ----------------------------------------------------------------------
# Тесты логики (tests/logic_tests.cpp)
# ----------------------------------------------------------------------
option(BUILD_LOGIC_TESTS "Собирать логические тесты" ON)

if (BUILD_LOGIC_TESTS)
    add_executable(omega_logic_tests
            tests/logic_tests.cpp
            logic/Board.cpp
    )

    target_include_directories(omega_logic_tests
            PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/logic
            ${CMAKE_CURRENT_SOURCE_DIR}/controller
    )

    # Логика тестов Qt не требует; если потом подключите GameController,
    # можно будет добавить и его .cpp сюда.
endif()

message(STATUS "Проект OmegaChess, версия: ${PROJECT_VERSION}")
