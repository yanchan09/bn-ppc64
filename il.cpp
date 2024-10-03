/* 
 * Copyright (C) 2024 yanchan09
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "il.h"
#include "intrinsics.h"

#include <lowlevelilinstruction.h>

using namespace BinaryNinja;

#include "decode_macros.h"

/* Implementation */

#define DECODE_MAIN PpcLifter::LiftInstruction
#define DECODE_GROUP(g) PpcLifter::lift##g
#define EMIT_IL
#include "decode.inc.cpp"
#undef EMIT_IL
#undef DECODE_MAIN
#undef DECODE_GROUP
