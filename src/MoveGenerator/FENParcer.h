#pragma once
#include "BitboardsUtils.h"

Flags ParseFEN(const String fen, u64f (&bitboards)[13]);
// String SaveFEN(const Flags flags, const u64f &bitboards);