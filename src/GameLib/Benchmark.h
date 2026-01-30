#pragma once
#include <chrono>
#include <unordered_map>
#include <string>
#include <iostream>

class Benchmark {
public:
    static void start(const std::string& tag);
    static void end(const std::string& tag);

private:
    static std::unordered_map<std::string, std::chrono::steady_clock::time_point> startTimes;
};