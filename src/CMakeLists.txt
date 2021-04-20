add_executable(ouija
    main.cpp
    board.cpp
)

target_compile_features(ouija PRIVATE cxx_std_20)

find_package(Curses REQUIRED)
target_include_directories(ouija
    PUBLIC
        .
        ../extern/ncursespp/include
    PRIVATE
        SYSTEM ${CURSES_INCLUDE_DIRS})
target_link_libraries(ouija PRIVATE ncursespp ${CURSES_LIBRARIES} pthread dl)
