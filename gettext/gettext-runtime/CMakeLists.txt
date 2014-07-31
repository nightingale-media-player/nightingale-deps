cmake_minimum_required(VERSION 2.6)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(gettext-runtime)

include(ConfigureChecks.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


#add_subdirectory(doc)
add_subdirectory(intl)
#add_subdirectory(intl-java)
#add_subdirectory(intl-csharp)
#add_subdirectory(gnulib-lib)
#add_subdirectory(src)
#add_subdirectory(po)
#add_subdirectory(man)
#add_subdirectory(m4)
#add_subdirectory(tests)



