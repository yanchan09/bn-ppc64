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
#include <fmt/core.h>

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
			if (inst & 0x2)
				dst = SEXT16(inst & 0xfffc);
			else
				dst = addr + SEXT16(inst & 0xfffc);
			if (inst & 0x1) {
				result.AddBranch(CallDestination, dst);
			} else {
				result.AddBranch(TrueBranch, dst);
				result.AddBranch(FalseBranch, addr+4);
			}
			return true;
		case 18:
			/* b/ba/bl/bla */
			if (inst & 0x2)
				dst = SEXT26(inst & 0x3fffffc);
			else
				dst = addr + SEXT26(inst & 0x3fffffc);
			if (inst & 0x1)
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
		return disasm.DecodeInstruction(data, addr);
	}

	virtual bool GetInstructionLowLevelIL(const uint8_t *data, uint64_t addr, size_t &len, LowLevelILFunction &il) override {
		len = 4;
		PpcLifter lift(&il, this);
		return lift.LiftInstruction(data, addr);
	}

	virtual std::string GetRegisterName(uint32_t reg) override {
		if (reg == 32) {
			return "ctr";
		} else {
			char buf[16];
			snprintf(buf, sizeof(buf), "r%d", reg);
			std::string s = buf;
			return s;
		}
	}

	virtual BNRegisterInfo GetRegisterInfo(uint32_t reg) override {
		BNRegisterInfo i = {0};
		i.fullWidthRegister = reg;
		i.size = 8;
		i.extend = NoExtend;
		return i;
	}

	virtual std::vector<uint32_t> GetAllRegisters() override {
		std::vector<uint32_t> v(33);
		for (int i = 0; i < 33; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual std::vector<uint32_t> GetFullWidthRegisters() override {
		std::vector<uint32_t> v(33);
		for (int i = 0; i < 33; i++) {
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
		std::vector<uint32_t> v(FLAG__LAST);
		for (int i = 0; i < FLAG__LAST; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual std::vector<uint32_t> GetAllFlagWriteTypes() override {
		std::vector<uint32_t> v(FLAG_WRITE__MAX);
		for (int i = 0; i < FLAG_WRITE__MAX; i++) {
			v[i] = i;
		}
		return v;
	}

	virtual BNFlagRole GetFlagRole(uint32_t flag, uint32_t semClass = 0) override {
		switch (flag) {
			case FLAG_CR0_LT: return NegativeSignFlagRole;
			case FLAG_CR0_GT: return PositiveSignFlagRole;
			case FLAG_CR0_EQ: return ZeroFlagRole;
			case FLAG_CR0_SO: return SpecialFlagRole;
			// todo: CR1 flags
			case FLAG_XER_SO: return SpecialFlagRole;
			case FLAG_XER_OV: return OverflowFlagRole;
			case FLAG_XER_CA: return CarryFlagRole;
			default: return SpecialFlagRole;
		}
	}

	virtual std::string GetFlagName(uint32_t flag) override {
		char buf[16];
		switch (flag) {
			case FLAG_CR0_LT: return "cr0.lt";
			case FLAG_CR0_GT: return "cr0.gt";
			case FLAG_CR0_EQ: return "cr0.eq";
			case FLAG_CR0_SO: return "cr0.so";
			case FLAG_CR1_FX: return "cr1.fx";
			case FLAG_CR1_FEX: return "cr1.fex";
			case FLAG_CR1_VX: return "cr1.vx";
			case FLAG_CR1_OX: return "cr1.ox";
			case FLAG_XER_SO: return "xer.so";
			case FLAG_XER_OV: return "xer.ov";
			case FLAG_XER_CA: return "xer.ca";
			default:
				if (flag < 64) {
					// CR register
					return fmt::format("cr{}.{}", flag/4, flag%4);
				} else {
					return fmt::format("unknown.{}", flag);
				}
		}
	}

	virtual std::string GetFlagWriteTypeName(uint32_t flag) override {
		std::string str;
		if (flag & FLAG_WRITE_CR0) {
			str.append("cr0");
		}
		if (flag & FLAG_WRITE_CA) {
			if (str.size()) str.append(".");
			str.append("ca");
		}
		return str;
	}

	virtual std::vector<uint32_t> GetFlagsWrittenByFlagWriteType(uint32_t flag) override {
		std::vector<uint32_t> vec;
		if (flag & FLAG_WRITE_CR0) {
			vec.insert(vec.end(), {FLAG_CR0_LT, FLAG_CR0_GT, FLAG_CR0_EQ, FLAG_CR0_SO});
		}
		if (flag & FLAG_WRITE_CA) {
			vec.insert(vec.end(), {FLAG_XER_CA});
		}
		return vec;
	}

	virtual ExprId GetFlagWriteLowLevelIL(BNLowLevelILOperation op, size_t size, uint32_t flagWriteType, uint32_t flag, BNRegisterOrConstant *operands, size_t operandCount, LowLevelILFunction &il) override {
		if (flag == FLAG_CR0_SO || flag == FLAG_XER_SO) {
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
