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

#define DFORM_RS(i) ((i>>21)&0x1f)
#define DFORM_RT(i) ((i>>21)&0x1f)
#define DFORM_RA(i) ((i>>16)&0x1f)
#define DFORM_UI(i) (i&0xffff)
#define DFORM_SI(i) ((i&0x8000) ? (-((i&0xffff)^0xffff)) : (i&0xffff))
#define DFORM_D(i) (i&0xffff)

#define MFORM_RS(i) ((i>>21)&0x1f)
#define MFORM_RA(i) ((i>>16)&0x1f)
#define MFORM_RB(i) ((i>>11)&0x1f)
#define MFORM_sh(i) ((i>>11)&0x1f)
#define MFORM_mb(i) ((i>>5)&0x3f)
#define MFORM_me(i) ((i>>1)&0x3f)
#define MFORM_Rc(i) (i&1)

#define MDFORM_RS(i) ((i>>21)&0x1f)
#define MDFORM_RA(i) ((i>>16)&0x1f)
#define MDFORM_sh(i) (((i&0xf800) >> 10) | ((i&0x2) >> 1))
#define MDFORM_mb(i) ((i>>5)&0x3f)
#define MDFORM_me(i) ((i>>5)&0x3f)
#define MDFORM_Rc(i) (i&1)

#define MDSFORM_RS(i) ((i>>21)&0x1f)
#define MDSFORM_RA(i) ((i>>16)&0x1f)
#define MDSFORM_RB(i) ((i>>11)&0x1f)
#define MDSFORM_mb(i) ((i>>5)&0x3f)
#define MDSFORM_me(i) ((i>>5)&0x3f)
#define MDSFORM_Rc(i) (i&1)

void PpcDisassembler::emitDForm(uint32_t inst, const std::string &name) {
	char buf[16];
	result->emplace_back(InstructionToken, name);
	result->emplace_back(TextToken, " ");
	snprintf(buf, sizeof(buf), "r%d", DFORM_RS(inst));
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "r%d", DFORM_RA(inst));
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "0x%x", DFORM_UI(inst));
	result->emplace_back(IntegerToken, buf, DFORM_UI(inst), 2);
}

void PpcDisassembler::emitDFormDisplaced(uint32_t inst, const std::string &name) {
	char buf[16];
	result->emplace_back(InstructionToken, name);
	result->emplace_back(TextToken, " ");
	snprintf(buf, sizeof(buf), "r%d", DFORM_RS(inst));
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "0x%x", DFORM_D(inst));
	result->emplace_back(IntegerToken, buf, DFORM_D(inst), 2);
	result->emplace_back(TextToken, "(");
	snprintf(buf, sizeof(buf), "r%d", DFORM_RA(inst));
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ")");
}

void PpcDisassembler::emitMDForm(uint32_t inst, const std::string &name) {
	char buf[16];
	result->emplace_back(InstructionToken, name);
	result->emplace_back(TextToken, " ");
	snprintf(buf, sizeof(buf), "r%d", (inst >> 21) & 0b11111);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "r%d", (inst >> 16) & 0b11111);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "r%d", (inst >> 11) & 0b11111);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "0x%x", (inst >> 5) & 0b111111);
	result->emplace_back(IntegerToken, buf, (inst >> 5) & 0b111111, 1);
}

void PpcDisassembler::emitMDSForm(uint32_t inst, const std::string &name) {
	char buf[16];
	result->emplace_back(InstructionToken, name);
	result->emplace_back(TextToken, " ");
	snprintf(buf, sizeof(buf), "r%d", (inst >> 21) & 0b11111);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "r%d", (inst >> 16) & 0b11111);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "r%d", (inst >> 11) & 0b11111);
	result->emplace_back(RegisterToken, buf);
	result->emplace_back(TextToken, ", ");
	snprintf(buf, sizeof(buf), "0x%x", (inst >> 5) & 0b111111);
	result->emplace_back(IntegerToken, buf, (inst >> 5) & 0b111111, 1);
}

bool PpcDisassembler::DecodeInstruction(const uint8_t *data) {
	uint32_t inst = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	uint32_t op = inst >> 26;
	char buf[16];
	switch (op) {
	case 18:
		result->emplace_back(InstructionToken, "branch?");
		break;
	case 16:
		result->emplace_back(InstructionToken, "branch_cond?");
		break;
	case 19:
		result->emplace_back(InstructionToken, "ud19");
		break;
	case 17:
		result->emplace_back(InstructionToken, "sc");
		break;
	case 34:
		if (DFORM_RA(inst) == 0) {
			result->emplace_back(InstructionToken, "lbz");
			result->emplace_back(TextToken, " ");
			snprintf(buf, sizeof(buf), "r%d", DFORM_RS(inst));
			result->emplace_back(RegisterToken, buf);
			result->emplace_back(TextToken, ", ");
			snprintf(buf, sizeof(buf), "0x%x", DFORM_D(inst));
			result->emplace_back(IntegerToken, buf, DFORM_D(inst), 2);
			result->emplace_back(TextToken, "(");
			result->emplace_back(IntegerToken, "0", 0, 2);
			result->emplace_back(TextToken, ")");
		} else {
			emitDFormDisplaced(inst, "lbz");
		}
		break;
	case 35:
		if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
			return false;
		emitDFormDisplaced(inst, "lbzu");
		break;
	case 40:
		emitDFormDisplaced(inst, "lhz");
		break;
	case 41:
		emitDFormDisplaced(inst, "lhzu");
		break;
	case 42:
		emitDFormDisplaced(inst, "lha");
		break;
	case 43:
		emitDFormDisplaced(inst, "lhau");
		break;
	case 32:
		emitDFormDisplaced(inst, "lwz");
		break;
	case 33:
		emitDFormDisplaced(inst, "lwzu");
		break;
	case 38:
		emitDFormDisplaced(inst, "stb");
		break;
	case 31:
		return decode31(inst);
	case 39:
		emitDFormDisplaced(inst, "stbu");
		break;
	case 44:
		emitDFormDisplaced(inst, "sth");
		break;
	case 45:
		emitDFormDisplaced(inst, "sthu");
		break;
	case 36:
		emitDFormDisplaced(inst, "stw");
		break;
	case 37:
		emitDFormDisplaced(inst, "stwu");
		break;
	case 14:
		if (DFORM_RA(inst) == 0) {
			result->emplace_back(InstructionToken, "li");
			result->emplace_back(TextToken, " ");
			snprintf(buf, sizeof(buf), "r%d", DFORM_RS(inst));
			result->emplace_back(RegisterToken, buf);
			result->emplace_back(TextToken, ", ");
			snprintf(buf, sizeof(buf), "0x%x", inst & 0xffff);
			result->emplace_back(IntegerToken, buf, inst&0xffff, 2);
		} else {
			emitDForm(inst, "addi");
		}
		break;
	case 15:
		if (DFORM_RA(inst) == 0) {
			result->emplace_back(InstructionToken, "lis");
			result->emplace_back(TextToken, " ");
			snprintf(buf, sizeof(buf), "r%d", DFORM_RS(inst));
			result->emplace_back(RegisterToken, buf);
			result->emplace_back(TextToken, ", ");
			snprintf(buf, sizeof(buf), "0x%x", inst & 0xffff);
			result->emplace_back(IntegerToken, buf, inst&0xffff, 2);
		} else {
			emitDForm(inst, "addis");
		}
		break;
	case 12:
		emitDForm(inst, "addic");
		break;
	case 13:
		emitDForm(inst, "addic.");
		break;
	case 8:
		emitDForm(inst, "subfic");
		break;
	case 7:
		emitDForm(inst, "mulli");
		break;
	case 11:
		result->emplace_back(TextToken, "cmpi");
		break;
	case 10:
		result->emplace_back(InstructionToken, "cmpli");
		break;
	case 2:
		emitDForm(inst, "tdi");
		break;
	case 3:
		emitDForm(inst, "twi");
		break;
	case 28:
		emitDForm(inst, "andi.");
		break;
	case 29:
		emitDForm(inst, "andis.");
		break;
	case 24:
		emitDForm(inst, "ori");
		break;
	case 25:
		emitDForm(inst, "oris");
		break;
	case 26:
		emitDForm(inst, "xori");
		break;
	case 27:
		emitDForm(inst, "xoris");
		break;
	case 30:
		return decode30(inst);
	default:
		result->emplace_back(TextToken, "ud");
		break;
		//return false;
	}
	return true;
}

bool PpcDisassembler::decode31(uint32_t inst) {
	uint32_t op = (inst >> 1) & 0b1111111111;
	char buf[16];
	switch (op) {
	case 274:
		result->emplace_back(InstructionToken, "tlbiel");
		result->emplace_back(TextToken, " ");
		snprintf(buf, sizeof(buf), "r%d", (inst >> 11) & 0b11111);
		result->emplace_back(RegisterToken, buf);
		result->emplace_back(TextToken, ", ");
		// TODO: Use enum for L (?)
		snprintf(buf, sizeof(buf), "%d", (inst >> 22) & 1);
		result->emplace_back(IntegerToken, buf, (inst >> 22) & 1, 1);
		break;
	case 306:
		result->emplace_back(InstructionToken, "tlbie");
		result->emplace_back(TextToken, " ");
		snprintf(buf, sizeof(buf), "r%d", (inst >> 11) & 0b11111);
		result->emplace_back(RegisterToken, buf);
		result->emplace_back(TextToken, ", ");
		// TODO: Use enum for L (?)
		snprintf(buf, sizeof(buf), "%d", (inst >> 22) & 1);
		result->emplace_back(IntegerToken, buf, (inst >> 22) & 1, 1);
		break;
	case 434:
		result->emplace_back(InstructionToken, "slbie");
		result->emplace_back(TextToken, " ");
		snprintf(buf, sizeof(buf), "r%d", (inst >> 11) & 0b11111);
		result->emplace_back(RegisterToken, buf);
		result->emplace_back(TextToken, ", ");
		// TODO: Use enum for L (?)
		snprintf(buf, sizeof(buf), "%d", (inst >> 22) & 1);
		result->emplace_back(IntegerToken, buf, (inst >> 22) & 1, 1);
		break;
	case 370:
		result->emplace_back(InstructionToken, "tlbia");
		break;
	case 566:
		result->emplace_back(InstructionToken, "tlbsync");
		break;
	case 598:
		result->emplace_back(InstructionToken, "sync");
		result->emplace_back(TextToken, " ");
		// TODO: Use enum for L (?)
		snprintf(buf, sizeof(buf), "%d", (inst >> 22) & 3);
		result->emplace_back(IntegerToken, buf, (inst >> 22) & 3, 1);
		break;
	case 854:
		result->emplace_back(InstructionToken, "eieio");
		break;
	default:
		result->emplace_back(TextToken, "ud31");
		break;
		//return false;
	}
	return true;
}

bool PpcDisassembler::decode30(uint32_t inst) {
	uint32_t op = (inst >> 2) & 0b111;
	char buf[16];
	switch (op) {
	case 0:
		result->emplace_back(InstructionToken, "rldicl");
		break;
	case 1:
		result->emplace_back(InstructionToken, "rldicr");
		break;
	case 2:
		result->emplace_back(InstructionToken, "rldic");
		break;
	case 3:
		result->emplace_back(InstructionToken, "rldimi");
		break;
	case 4:
		if (inst & 0b10) {
			emitMDSForm(inst, "rldcr");
		} else {
			emitMDSForm(inst, "rldcl");
		}
		break;
	default:
		result->emplace_back(InstructionToken, "ud30");
		break;
	}
	return true;
}
