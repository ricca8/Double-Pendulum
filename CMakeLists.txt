cmake_minimum_required(VERSION 3.12)
project(RT)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c11 -Wall")
set(SOURCE_FILES main.c)

INCLUDE_DIRECTORIES(  /usr/local/Cellar/allegro/5.2.5.0/include )
LINK_DIRECTORIES(  /usr/local/Cellar/allegro/5.2.5.0/lib )

file(GLOB LIBRARIES "/usr/local/Cellar/allegro/5.2.5.0/lib/*.dylib")
message("LIBRARIES = ${LIBRARIES}")

add_executable(RT
        elaboration.c
        Headers/elaboration.h
        Data.txt
        init.c
        Headers/init.h
        main.c
        graphic.c
        Headers/graphic.h
        keyboard_manager.c
        Headers/keyboard_manager.h
        Headers/macros.h init.c Headers/init.h)



TARGET_LINK_LIBRARIES(RT  ${LIBRARIES})




