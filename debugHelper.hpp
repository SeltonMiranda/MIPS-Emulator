/*
 * The purpose of this header is to have a bunch of functions to
 * help for debugging tasks, anytime later I'll create a proper debugger
 */

#pragma once

template<typename T>
auto printVector(const T& vec) -> void {
    int i = 0;
    std::cout << "---------------------------------------------------\n";
    for (const auto& value : vec) {
        std::cout << std::format("value {} : {}\n", i++, value);
    }
    std::cout << "---------------------------------------------------\n";
}

template<typename T>
auto printHashMap(const T& map) -> void {
    int i = 0;
    std::cout << "---------------------------------------------------\n";
    for (const auto& [key, value] : map) {
        std::cout << std::format("pair {}, key {}, value {}\n", i++, key, value);
    }
    std::cout << "---------------------------------------------------\n";
}