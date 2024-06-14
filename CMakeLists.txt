cmake_minimum_required(VERSION 3.8)
project(test)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 头文件
include_directories(CMAKE_<LANG>_COMPILER_EXTERNAL_TOOLCHAIN)

# 列出所有需要包含的源文件
set(SOURCES 
    main.cpp
    # LR1.cpp
    Lexer/Delimiters.txt
    Lexer/KeyWord.txt

    utils/GrammarParaser.cpp
    utils/grammar
    utils/AnalysisTable.json
    test.txt
)

# 查找jsoncpp库
find_package(jsoncpp CONFIG REQUIRED)

# 将这些源文件添加到您的可执行文件或库中
add_executable(test ${SOURCES})
# add_executable(test ext.cpp)

# 链接jsoncpp库
target_link_libraries(test PRIVATE jsoncpp_lib)