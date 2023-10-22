# Registry Helper

This is a simple project that acts as a wrapper around the Registry Win32 API calls to interact with registry keys.

## Example Usage

```cpp
#include <reg_helper/reg_helper.hpp>

int main(int argc, const char **argv)
{
    try
    {
        const auto internet_options = reg_helper<KEY_QUERY_VALUE | KEY_SET_VALUE>(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings");
        auto proxy_enabled = internet_options.get_value<int>("ProxyEnable");

        std::cout << "Is Proxy Enabled: " << (*proxy_enabled == 1 ? "yes" : "no") << '\n';

        if (*proxy_enabled)
        {
            std::cout << "Proxy Address: " << *internet_options.get_value<std::string>("ProxyServer") << '\n';
        }

        auto succ = internet_options.set_value<int>("ProxyEnable", !*proxy_enabled);
        std::cout << "Toggling ProxyEnable " << (succ ? "successfully" : "failed") <<  '\n';
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
```

## FetchContent CMake include

```cmake
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20 GLOBAL)

project(my_app LANGUAGES CXX VERSION 0.0.1)

include(FetchContent)
FetchContent_Declare(
    reg_helper
    GIT_REPOSITORY https://github.com/Yimura/reg_helper.git
    GIT_TAG master
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(reg_helper)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} reg_helper)
```