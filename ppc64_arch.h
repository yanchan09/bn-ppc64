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

#include <binaryninjaapi.h>

#include "disasm.h"
#include "il.h"
#include "intrinsics.h"

using namespace BinaryNinja;

#define SEXT16(i) (((i)&0x8000) ? (0xffffffffffff0000 | (i)) : (i))
#define SEXT26(i) (((i)&0x2000000) ? (0xfffffffffc000000 | (i)) : (i))
#define SEXT32(i) (((i)&0x80000000) ? (0xffffffff00000000 | (i)) : (i))

class Ppc64Architecture: public Architecture {
public:
	Ppc64Architecture(const std::string &name) : Architecture(name) {}

	virtual BNEndianness GetEndianness() const override {
		return BigEndian;
	}

	virtual size_t GetAddressSize() const override {
		return 8;
	}

	virtual size_t GetMaxInstructionLength() const override {
		return 4;
	}

	virtual bool GetInstructionInfo(const uint8_t *data, uint64_t addr, size_t maxLen, InstructionInfo &result) override {
		if (maxLen < 4)
			return false;

		result.length = 4;
		uint32_t inst = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
		uint32_t op = inst >> 26;
		uint64_t dst;
		switch(op) {
		case 16:
			/* bc/bca/bcl/bcla */
			if (op & 0x2)
				dst = SEXT16(inst & 0xfffc);
			else
				dst = addr + SEXT16(inst & 0xfffc);
			if (op & 0x1) {
				result.AddBranch(CallDestination, dst);
			} else {
				result.AddBranch(TrueBranch, dst);
				result.AddBranch(FalseBranch, addr+4);
			}
			return true;
		case 18:
			/* b/ba/bl/bla */
			if (op & 0x2)
				dst = SEXT26(inst & 0x3fffffc);
			else
				dst = addr + SEXT26(inst & 0x3fffffc);
			if (op & 0x1)
				result.AddBranch(CallDestination, dst);
			else
				result.AddBranch(UnconditionalBranch, dst);
			return true;
		}
		return true;
	}

	virtual bool GetInstructionText(const uint8_t *data, uint64_t addr, size_t &len, std::vector<InstructionTextToken> &result) override {
		len = 4;
		PpcDisassembler disasm(&result);
		return disasm.DecodeInstruction(data);
	}

	virtual bool GetInstructionLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len, LowLevelILFunction &il) override {
		len = 4;
		PpcLifter lift(&il, this);
		return lift.LiftInstruction(data, addr);
	}

	virtual std::string GetRegisterName(uint32_t reg) override {
		char buf[16];
		snprintf(buf, sizeof(buf), "r%d", reg);
		std::string s = buf;
		return s;
	}

	virtual BNRegisterInfo GetRegisterInfo(uint32_t reg) override {
		BNRegisterInfo i = {0};
		i.offset = 8 * reg;
		i.size = 8;
		return i;
	}

	virtual std::vector<uint32_t> GetAllRegisters() override {
		std::vector<uint32_t> v(32);
		for (int i = 0; i < 32; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual std::vector<uint32_t> GetFullWidthRegisters() override {
		std::vector<uint32_t> v(32);
		for (int i = 0; i < 32; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual std::string GetIntrinsicName(uint32_t i) override {
		Intrinsic in = static_cast<Intrinsic>(i);
		switch (in) {
			case Intrinsic::dcbt: return "dcbt";
			case Intrinsic::dcbtst: return "dcbtst";
			case Intrinsic::icbi: return "icbi";
			case Intrinsic::isync: return "isync";
			case Intrinsic::mfspr: return "mfspr";
			case Intrinsic::mtspr: return "mtspr";
			case Intrinsic::slbie: return "slbie";
			case Intrinsic::slbmte: return "slbmte";
			case Intrinsic::tlbiel: return "tlbiel";
			case Intrinsic::tlbie: return "tlbie";
			case Intrinsic::tlbia: return "tlbia";
			case Intrinsic::tlbsync: return "tlbsync";
			case Intrinsic::sync: return "sync";
			case Intrinsic::eieio: return "eieio";
			default: return "";
		}
	}

	virtual std::vector<uint32_t> GetAllIntrinsics() override {
		uint32_t max = static_cast<uint32_t>(Intrinsic::ENUM_LAST);
		std::vector<uint32_t> v(max);
		for (int i = 0; i < max; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual std::vector<uint32_t> GetAllFlags() override {
		uint32_t max = static_cast<uint32_t>(Flag::_LAST);
		std::vector<uint32_t> v(max);
		for (int i = 0; i < max; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual std::vector<uint32_t> GetAllFlagWriteTypes() override {
		uint32_t max = static_cast<uint32_t>(FlagWriteType::_LAST);
		std::vector<uint32_t> v(max);
		for (int i = 0; i < max; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual BNFlagRole GetFlagRole(uint32_t flag, uint32_t semClass = 0) override {
		Flag fl = static_cast<Flag>(flag);
		switch (fl) {
			case Flag::CR0_LT: return NegativeSignFlagRole;
			case Flag::CR0_GT: return PositiveSignFlagRole;
			case Flag::CR0_EQ: return ZeroFlagRole;
			case Flag::CR0_SO: return SpecialFlagRole;
			case Flag::XER_SO: return SpecialFlagRole;
			case Flag::XER_OV: return OverflowFlagRole;
			case Flag::XER_CA: return CarryFlagRole;
			default: return SpecialFlagRole;
		}
	}

	virtual std::string GetFlagName(uint32_t flag) override {
		Flag fl = static_cast<Flag>(flag);
		switch (fl) {
			case Flag::CR0_LT: return "cr0.lt";
			case Flag::CR0_GT: return "cr0.gt";
			case Flag::CR0_EQ: return "cr0.eq";
			case Flag::CR0_SO: return "cr0.so";
			case Flag::XER_SO: return "xer.so";
			case Flag::XER_OV: return "xer.ov";
			case Flag::XER_CA: return "xer.ca";
			default: return "";
		}
	}

	virtual std::string GetFlagWriteTypeName(uint32_t flag) override {
		FlagWriteType fl = static_cast<FlagWriteType>(flag);
		switch (fl) {
			case FlagWriteType::CR0: return "cr0";
			default: return "";
		}
	}

	virtual ExprId GetFlagWriteLowLevelIL(BNLowLevelILOperation op, size_t size, uint32_t flagWriteType, uint32_t flag, BNRegisterOrConstant *operands, size_t operandCount, LowLevelILFunction &il) override {
		if (flag == static_cast<uint32_t>(Flag::CR0_SO) || flag == static_cast<uint32_t>(Flag::XER_SO)) {
			ExprId ei = GetDefaultFlagWriteLowLevelIL(op, size, OverflowFlagRole, operands, operandCount, il);
			// assume ei is something like compute_flag()
			// We shall replace that with current_flag_value | compute_flag()
			return il.Or(0, il.Flag(flag), ei);
		} else {
			BNFlagRole role = GetFlagRole(flag);
			return GetDefaultFlagWriteLowLevelIL(op, size, role, operands, operandCount, il);
		}
	}
};
