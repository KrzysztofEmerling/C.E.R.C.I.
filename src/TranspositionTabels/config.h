#include <cstddef>

#pragma once

#define TT_EVAL_SIZE_MB 16

constexpr std::size_t MB_TO_BYTES(std::size_t mb)
{
    return mb * 1024 * 1024;
}