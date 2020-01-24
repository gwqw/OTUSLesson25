cmake_minimum_required(VERSION 3.2)

# build number for travis
if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(join_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(join_server VERSION 0.0.1)
endif()

set(USE_TEST ON)

# boost dependensies
find_package(Boost COMPONENTS unit_test_framework system REQUIRED)
find_package(Threads REQUIRED)

# source
set(SOURCE
        join_server.cpp join_server.h
        database.cpp database.h
        command.cpp command.h
        str_utils.cpp str_utils.h
        command_parser.cpp command_parser.h
        command_handler.cpp command_handler.h
        thread_pool.cpp thread_pool.h
)
set(EXE_SOURCE main.cpp ${SOURCE})
set(TEST_SOURCE test_course.cpp ${SOURCE})

# targets and libraries
set(EXE_NAME join_server)
set(TEST_NAME test_join_server)
add_executable(${EXE_NAME} ${EXE_SOURCE})
add_executable(${TEST_NAME} ${TEST_SOURCE})

# compiler options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMP_OPTIONS "-Wall;-Wextra;-Wshadow;-Wnon-virtual-dtor;-Wold-style-cast;-Wcast-align;-Wunused;-Woverloaded-virtual;-Wconversion;-Wsign-conversion;-Wduplicated-cond")
  if(CMAKE_BUILD_TYPE MATCHES Debug)
      set(CMP_OPTIONS ${CMP_OPTIONS}";-D_GLIBCXX_DEBUG;-D_GLIBCXX_DEBUG_PEDANTIC")
      message("debug")
  endif()
else()
  set(CMP_OPTIONS "/W4")
endif()

# target properties
set_target_properties(${EXE_NAME} ${TEST_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS ${CMP_OPTIONS}
)

# add boost headers for test
if(UNIX)
    set_target_properties(${TEST_NAME} PROPERTIES
        COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    )
endif()
target_include_directories(${EXE_NAME}
        PRIVATE ${Boost_INCLUDE_DIR}
)
target_include_directories(${TEST_NAME}
        PRIVATE ${Boost_INCLUDE_DIR}
)

# target linking
target_link_libraries(${EXE_NAME}
        ${Boost_LIBRARIES}
)
target_link_libraries(${TEST_NAME}
    ${Boost_LIBRARIES}
)
target_link_libraries(${EXE_NAME} Threads::Threads)
target_link_libraries(${TEST_NAME} Threads::Threads)

# installation
install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT gwrass@mail.com)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

# tests
enable_testing()
add_test(${TEST_NAME} ${TEST_NAME})

