CMAKE_MINIMUM_REQUIRED( VERSION 3.25 )
PROJECT( SQLCC )

SET(CMAKE_CXX_STANDARD 23)
# SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/external/sqlite3")

ADD_LIBRARY(sqlite3 STATIC ${PROJECT_SOURCE_DIR}/external/sqlite3/sqlite3.c)

TARGET_INCLUDE_DIRECTORIES(sqlite3 PUBLIC ${PROJECT_SOURCE_DIR}/external/sqlite3)

INCLUDE(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
SET(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

ENABLE_TESTING()

ADD_EXECUTABLE( test_df
    ${PROJECT_SOURCE_DIR}/test/test_df.cc 
)

ADD_EXECUTABLE( test_sql
  	${PROJECT_SOURCE_DIR}/test/test_sql.cc
    ${PROJECT_SOURCE_DIR}/src/sql.cc
    ${PROJECT_SOURCE_DIR}/src/sqlite.cc
)

TARGET_LINK_LIBRARIES( test_df PRIVATE GTest::gtest_main )

TARGET_LINK_LIBRARIES( test_sql PRIVATE GTest::gtest_main )
TARGET_LINK_LIBRARIES( test_sql PRIVATE sqlite3 )

INCLUDE(GoogleTest)
GTEST_DISCOVER_TESTS(test_df)
GTEST_DISCOVER_TESTS(test_sql)