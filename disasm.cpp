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

#include "disasm.h"

using namespace BinaryNinja;

#include "decode_macros.h"

void PpcDisassembler::Op(const std::string &v) {
	result->emplace_back(InstructionToken, v);
}

void PpcDisassembler::Reg(uint32_t reg) {
	if (result->size() >= 2) result->emplace_back(TextToken, ", ");
	else if (result->size() == 1) result->emplace_back(TextToken, " ");

	char buf[16];
	snprintf(buf, sizeof(buf), "r%d", reg);
	result->emplace_back(RegisterToken, buf);
}
void PpcDisassembler::Imm(uint64_t imm) {
	if (result->size() >= 2) result->emplace_back(TextToken, ", ");
	else if (result->size() == 1) result->emplace_back(TextToken, " ");

	char buf[16];
	snprintf(buf, sizeof(buf), "0x%lx", imm);
	result->emplace_back(IntegerToken, buf, imm);
}
void PpcDisassembler::Disp(uint32_t reg, uint64_t d) {
	if (result->size() >= 2) result->emplace_back(TextToken, ", ");
	else if (result->size() == 1) result->emplace_back(TextToken, " ");

	char buf[16];
	snprintf(buf, sizeof(buf), "%ld", d);
	result->emplace_back(IntegerToken, buf, d);
	result->emplace_back(TextToken, "(");
	snprintf(buf, sizeof(buf), "r%d", reg);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ")");
}

#define DECODE_MAIN PpcDisassembler::DecodeInstruction
#define DECODE_GROUP(g) PpcDisassembler::decode##g
#define EMIT_ASM
#include "decode.inc.cpp"
#undef EMIT_ASM
#undef DECODE_MAIN
#undef DECODE_GROUP

