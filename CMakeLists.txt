cmake_minimum_required(VERSION 3.20)
project(cross-pkg)

set(CMAKE_CXX_STANDARD 20)

add_executable(cross-pkg src/main.cpp
        src/messages.cpp
        src/messages.hpp
        src/commands/search.cpp
        src/commands/search.hpp
        src/commands/update.cpp
        src/commands/update.hpp
        src/commands/version.cpp
        src/commands/version.hpp
        src/commands/clear.cpp
        src/commands/clear.hpp
        src/commands/build.cpp
        src/commands/build.hpp
        src/commands/list.cpp
        src/commands/list.hpp
        src/colors.hpp
        src/cmd.cpp
        src/cmd.hpp
        src/commands/help.cpp
        src/commands/help.hpp
        src/defines.hpp
        src/split.cpp
        src/split.hpp
        src/commands/download.cpp
        src/commands/download.hpp
        src/commands/install.cpp
        src/commands/install.hpp
        src/commands/remove.cpp
        src/commands/remove.hpp
)

install(TARGETS cross-pkg DESTINATION bin)