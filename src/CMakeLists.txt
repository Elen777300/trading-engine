
add_library(trading_engine
        trading_engine.cpp
        trading_engine.hpp
)
target_include_directories(trading_engine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(trading_engine_main
        main.cpp
)

# Link the trading_engine library to the main executable
target_link_libraries(trading_engine_main PRIVATE trading_engine)
