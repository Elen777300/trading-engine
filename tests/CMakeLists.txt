add_executable(trading_engine_tests
        test_main.cpp
        tests.cpp
)

target_link_libraries(trading_engine_tests PRIVATE trading_engine)
add_test(NAME trading_engine_tests
        COMMAND trading_engine_tests)
