#include <cstddef>

#pragma once

#define TT_MOVEGEN_SIZE_MB 128
#define TT_EVAL_SIZE_MB 32

constexpr std::size_t MB_TO_BYTES(std::size_t mb)
{
    return mb * 1024 * 1024;
}