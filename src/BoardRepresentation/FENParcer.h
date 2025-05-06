#pragma once
#include "BoardUtils.h"

Flags ParseFEN(const String fen, u64f (&bitboards)[13]);

String SaveFEN(const Flags flags, u64f (&bitboards)[13]);