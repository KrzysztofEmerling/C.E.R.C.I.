#pragma once

#include <chrono>
#include <string>
#include <sstream>
#include <vector>

inline std::string FormatTime(std::chrono::nanoseconds duration)
{
    using namespace std::chrono;

    auto total_ns = duration.count();
    auto hours = total_ns / 3'600'000'000'000;
    total_ns %= 3'600'000'000'000;
    auto minutes = total_ns / 60'000'000'000;
    total_ns %= 60'000'000'000;
    auto seconds = total_ns / 1'000'000'000;
    total_ns %= 1'000'000'000;
    auto milliseconds = total_ns / 1'000'000;
    total_ns %= 1'000'000;
    auto microseconds = total_ns / 1'000;

    std::vector<std::string> parts;

    if (hours > 0)
        parts.push_back(std::to_string(hours) + " hr");
    if (minutes > 0)
        parts.push_back(std::to_string(minutes) + " min");
    if (seconds > 0)
        parts.push_back(std::to_string(seconds) + " sec");
    if (milliseconds > 0)
        parts.push_back(std::to_string(milliseconds) + " ms");
    if (microseconds > 0)
        parts.push_back(std::to_string(microseconds) + " µs");

    if (parts.empty())
        parts.push_back(std::to_string(total_ns) + " ns");

    std::ostringstream oss;
    for (size_t i = 0; i < parts.size() && i < 3; ++i)
    {
        if (i > 0)
            oss << ", ";
        oss << parts[i];
    }

    return oss.str();
}
