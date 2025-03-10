Test task to implement a Trading Engine

---

### Project Structure

```
trading-engine/
├── CMakeLists.txt
├── src/
│   ├── CMakeLists.txt
│   ├── main.cpp
│   ├── trading_engine.cpp
│   └── trading_engine.hpp
└── tests/
    ├── CMakeLists.txt
    └── test_main.cpp
    └── tests.cpp
```

- **`src/`**: Contains application's source code.
- **`tests/`**: Contains test source files
---
### Building and Running the Application

1. **Configure and Build the Project:**

    Open a terminal in the project's root directory and execute:

    ```bash
   cmake -B build
   cmake --build build
    ```


2. **Run the Application:**

   After building, run the main application with:

    ```bash
    chmod +x build/src/trading_engine_main
    ./build/src/trading_engine_main
    ```

3. **Run the Tests:**

   After building, run the tests using:

   ```bash
   ctest --test-dir build
   ```

   This command will execute the tests registered with CTest and display the results.
