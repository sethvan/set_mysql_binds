cmake_minimum_required(VERSION 3.16)

project( set_mysql_binds VERSION 1.0.0 LANGUAGES "CXX" )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)

add_library( set_mysql_binds SHARED 
src/utilities.cpp
src/getDBTables.cpp
src/createDBTableBinds.cpp
)

find_library(MYSQLCLIENT_LIBRARY NAMES mysqlclient)

if(MYSQLCLIENT_LIBRARY)
  target_link_libraries(set_mysql_binds PRIVATE ${MYSQLCLIENT_LIBRARY})
else()
  message(FATAL_ERROR "mysqlclient library not found")
endif()

target_include_directories( set_mysql_binds PUBLIC include )
target_compile_features( set_mysql_binds PRIVATE cxx_std_20)
target_link_options( set_mysql_binds PRIVATE -fsanitize=address)
target_compile_options( set_mysql_binds PRIVATE
    -Wall -Wextra -Werror -Wl,-z,defs -fwrapv -O3
)

