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

bool DECODE_MAIN(const uint8_t *data, uint64_t addr) {
	uint32_t inst = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	uint32_t op = inst >> 26;
	char buf[16];
	ExprId ei0, ei1;
	ExprId ea;
	ExprId cond, ctrValue;
	BNLowLevelILLabel *label1, *label2;
	uint64_t dst;
	size_t regWidth = 8;
	switch (op) {
case 0:
	/* Illegal/Reserved */
	return false;
case 1:
	/* Reserved */
	return false;
case 2:
#if   defined(EMIT_ASM)
	Op("tdi");
#elif defined(EMIT_IL)
	/* tdi */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 3:
#if   defined(EMIT_ASM)
	Op("twi");
#elif defined(EMIT_IL)
	/* twi */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 4:
	/* Reserved */
	return false;
case 5:
	/* Reserved */
	return false;
case 6:
	/* Reserved */
	return false;
case 7:
#if   defined(EMIT_ASM)
	Op("mulli");
	Reg(DFORM_RT(inst));
	Reg(DFORM_RA(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* mulli */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 8:
#if   defined(EMIT_ASM)
	Op("subfic");
	Reg(DFORM_RT(inst));
	Reg(DFORM_RA(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* subfic */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 9:
#if   defined(EMIT_ASM)
	Op("dozi");
#elif defined(EMIT_IL)
	/* dozi */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 10:
#if   defined(EMIT_ASM)
	Op("cmpli");
	Imm(DFORM_BF(inst));
	Imm(DFORM_L(inst));
	Reg(DFORM_RA(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* cmpli */
	ei0 = il->Register(DFORM_L(inst) ? regWidth : 4, DFORM_RA(inst));
	ei1 = il->Const(regWidth, DFORM_UI(inst));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+0, il->CompareUnsignedLessThan(regWidth, ei0, ei1)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+1, il->CompareUnsignedGreaterThan(regWidth, ei0, ei1)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+2, il->CompareEqual(regWidth, ei0, ei1)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+3, il->Flag(FLAG_XER_SO)));
#endif
	return true;
case 11:
#if   defined(EMIT_ASM)
	Op("cmpi");
	Imm(DFORM_BF(inst));
	Imm(DFORM_L(inst));
	Reg(DFORM_RA(inst));
	Imm(DFORM_SI(inst));
#elif defined(EMIT_IL)
	if (DFORM_L(inst)) {
		ei0 = il->Register(regWidth, DFORM_RA(inst));
	} else {
		ei0 = il->SignExtend(regWidth, il->Register(4, DFORM_RA(inst)));
	}
	ei1 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+0, il->CompareSignedLessThan(regWidth, ei0, ei1)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+1, il->CompareSignedGreaterThan(regWidth, ei0, ei1)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+2, il->CompareEqual(regWidth, ei0, ei1)));
	il->AddInstruction(il->SetFlag(DFORM_BF(inst)*4+3, il->Flag(FLAG_XER_SO)));
#endif
	return true;
case 12:
#if   defined(EMIT_ASM)
	Op("addic");
	Reg(DFORM_RT(inst));
	Reg(DFORM_RA(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* addic */
	ei0 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei0, ei1, FLAG_WRITE_CA);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 13:
#if   defined(EMIT_ASM)
	Op("addic.");
	Reg(DFORM_RT(inst));
	Reg(DFORM_RA(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* addic. */
	ei0 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei0, ei1, FLAG_WRITE_CR0 | FLAG_WRITE_CA);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 14:
	if (DFORM_RA(inst) == 0) {
		/* li */
#if   defined(EMIT_ASM)
		Op("li");
		Reg(DFORM_RS(inst));
		Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
		ei0 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
#endif
	} else {
		/* addi */
#if   defined(EMIT_ASM)
		Op("addi");
		Reg(DFORM_RS(inst));
		Reg(DFORM_RA(inst));
		Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
		ei0 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei0, ei1);
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
#endif
	}
	return true;
case 15:
	if (DFORM_RA(inst) == 0) {
		/* lis */
#if   defined(EMIT_ASM)
		Op("li");
		Reg(DFORM_RS(inst));
		Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
		ei0 = il->Const(regWidth, SEXT32(DFORM_UI(inst) << 16));
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
#endif
	} else {
		/* addis */
#if   defined(EMIT_ASM)
		Op("addis");
		Reg(DFORM_RS(inst));
		Reg(DFORM_RA(inst));
		Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
		ei0 = il->Const(regWidth, SEXT32(DFORM_UI(inst) << 16));
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei0, ei1);
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
#endif
	}
	return true;
	case 16:
#if   defined(EMIT_ASM)
		{
			std::string branch_mnemonics[] = {"bc", "bcl", "bca", "bcla"};
			Op(branch_mnemonics[op&0x3]);
			Imm(BFORM_BO(inst));
			Imm(BFORM_BI(inst));
			Imm(BFORM_BD(inst));
		}
#elif defined(EMIT_IL)
		/* bc/bca/bcl/bcla */
		if (inst & 0x2)
			dst = SEXT16(BFORM_BD(inst));
		else
			dst = addr + SEXT16(BFORM_BD(inst));
		if ((BFORM_BO(inst) & 0b10100) == 0b10100) {
			// Branch always
			if (inst & 0x1)
				il->AddInstruction(il->Call(il->ConstPointer(8, dst)));
			else
				il->AddInstruction(il->Jump(il->ConstPointer(8, dst)));
			return true;
		}

		if ((BFORM_BO(inst) & 0b00100) == 0) {
			// Decrement CTR
			il->AddInstruction(il->SetRegister(8, PPC_REG_CTR, il->Sub(8, il->Register(8, PPC_REG_CTR), il->Const(8, 1))));
			if (BFORM_BO(inst) & 0b00010)
				cond = il->CompareEqual(8, il->Register(8, PPC_REG_CTR), il->Const(8, 0));
			else
				cond = il->CompareNotEqual(8, il->Register(8, PPC_REG_CTR), il->Const(8, 0));
		}
		if ((BFORM_BO(inst) & 0b10000) == 0) {
			// Check CR_BI
			ei0 = il->Flag(BFORM_BI(inst));
			ei0 = il->CompareEqual(1, ei0, il->Const(1, (BFORM_BO(inst) >> 3) & 1));
			if ((BFORM_BO(inst) & 0b00100) == 0)
				cond = il->And(1, cond, ei0);
			else
				cond = ei0;
		}

		if (inst & 0x1) {
			LowLevelILLabel callLabel;
			label1 = il->GetLabelForAddress(arch, addr+4);
			if (label1) {
				il->AddInstruction(il->If(cond, callLabel, *label2));
				il->MarkLabel(callLabel);
				il->AddInstruction(il->Call(addr+4));
			}
		} else {
			label1 = il->GetLabelForAddress(arch, dst);
			label2 = il->GetLabelForAddress(arch, addr+4);
			if (label1 && label2) {
				il->AddInstruction(il->If(cond, *label1, *label2));
			}
		}
#endif
		return true;
	case 17:
#if   defined(EMIT_ASM)
		Op("sc");
#elif defined(EMIT_IL)
		/* sc */
		il->AddInstruction(il->SystemCall());
#endif
		return true;
	case 18:
#if   defined(EMIT_ASM)
		{
			std::string branch_mnemonics[] = {"b", "bl", "ba", "bla"};
			Op(branch_mnemonics[op&0x3]);
			Imm(IFORM_LI(inst));
		}
#elif defined(EMIT_IL)
		/* b/ba/bl/bla */
		if (inst & 0x2)
			dst = SEXT26(IFORM_LI(inst));
		else
			dst = addr + SEXT26(IFORM_LI(inst));
		if (inst & 0x1)
			il->AddInstruction(il->Call(il->ConstPointer(8, dst)));
		else
			il->AddInstruction(il->Jump(il->ConstPointer(8, dst)));
#endif
		return true;
case 19:
	return DECODE_GROUP(19)(inst);
case 20:
#if   defined(EMIT_ASM)
	Op("rlwimi");
#elif defined(EMIT_IL)
	/* rlwimi */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 21:
#if   defined(EMIT_ASM)
	Op("rlwinm");
#elif defined(EMIT_IL)
	/* rlwinm */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 22:
#if   defined(EMIT_ASM)
	Op("rlmi");
#elif defined(EMIT_IL)
	/* rlmi */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 23:
#if   defined(EMIT_ASM)
	Op("rlwnm");
#elif defined(EMIT_IL)
	/* rlwnm */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 24:
	if (DFORM_RA(inst) == 0 && DFORM_RS(inst) == 0 && DFORM_UI(inst) == 0) {
		/* nop */
#if   defined(EMIT_ASM)
		Op("nop");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Nop());
#endif
	} else {
		/* ori */
#if   defined(EMIT_ASM)
		Op("ori");
		Reg(DFORM_RA(inst));
		Reg(DFORM_RS(inst));
		Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
		ei1 = il->Const(regWidth, DFORM_UI(inst));
		ei0 = il->Register(regWidth, DFORM_RS(inst));
		ei0 = il->Or(regWidth, ei0, ei1);
		ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
		il->AddInstruction(ei0);
#endif
	}
	return true;
case 25:
#if   defined(EMIT_ASM)
	Op("oris");
	Reg(DFORM_RA(inst));
	Reg(DFORM_RS(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* oris */
	ei1 = il->Const(regWidth, DFORM_UI(inst) << 16);
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Or(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 26:
#if   defined(EMIT_ASM)
	Op("xori");
	Reg(DFORM_RA(inst));
	Reg(DFORM_RS(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* xori */
	ei1 = il->Const(regWidth, DFORM_UI(inst));
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Xor(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 27:
#if   defined(EMIT_ASM)
	Op("xoris");
	Reg(DFORM_RA(inst));
	Reg(DFORM_RS(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* xoris */
	ei1 = il->Const(regWidth, DFORM_UI(inst) << 16);
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Xor(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 28:
#if   defined(EMIT_ASM)
	Op("andi.");
	Reg(DFORM_RA(inst));
	Reg(DFORM_RS(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* andi. */
	ei1 = il->Const(regWidth, DFORM_UI(inst));
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->And(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0, FLAG_WRITE_CR0);
	il->AddInstruction(ei0);
#endif
	return true;
case 29:
#if   defined(EMIT_ASM)
	Op("andis.");
	Reg(DFORM_RA(inst));
	Reg(DFORM_RS(inst));
	Imm(DFORM_UI(inst));
#elif defined(EMIT_IL)
	/* andis. */
	ei1 = il->Const(regWidth, DFORM_UI(inst) << 16);
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->And(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0, FLAG_WRITE_CR0);
	il->AddInstruction(ei0);
#endif
	return true;
case 30:
	return DECODE_GROUP(30)(inst);
case 31:
	return DECODE_GROUP(31)(inst);
case 32:
#if   defined(EMIT_ASM)
	Op("lwz");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* lwz */
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	if (DFORM_RA(inst) != 0) {
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei1, ei0);
	}
	ei0 = il->Load(4, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 33:
	/* lwzu */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
#if   defined(EMIT_ASM)
	Op("lwzu");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(4, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 34:
#if   defined(EMIT_ASM)
	Op("lbz");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* lbz */
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	if (DFORM_RA(inst) != 0) {
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei1, ei0);
	}
	ei0 = il->Load(1, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 35:
	/* lbzu */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
#if   defined(EMIT_ASM)
	Op("lbzu");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(1, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 36:
#if   defined(EMIT_ASM)
	Op("stw");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* stw */
	if (DFORM_RA(inst) == 0) {
		ea = il->Const(2, SEXT16(DFORM_D(inst)));
	} else {
		ea = il->Add(regWidth,
			il->Register(regWidth, DFORM_RA(inst)),
			il->Const(regWidth, SEXT16(DFORM_D(inst)))
		);
	}
	il->AddInstruction(
		il->Store(4, ea, il->Register(regWidth, DFORM_RS(inst)))
	);
#endif
	return true;
case 37:
	/* stwu */
	if (DFORM_RA(inst) == 0)
		return false;
#if   defined(EMIT_ASM)
	Op("stwu");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ea = il->Add(regWidth,
		il->Register(regWidth, DFORM_RA(inst)),
		il->Const(regWidth, SEXT16(DFORM_D(inst)))
	);
	il->AddInstruction(
		il->Store(4, ea, il->Register(regWidth, DFORM_RS(inst)))
	);
	il->AddInstruction(
		il->SetRegister(regWidth, DFORM_RA(inst), ea)
	);
#endif
	return true;
case 38:
#if   defined(EMIT_ASM)
	Op("stb");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* stb */
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	if (DFORM_RA(inst) != 0) {
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei1, ei0);
	}
	ei1 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Store(1, ei0, ei1);
	il->AddInstruction(ei0);
#endif
	return true;
case 39:
	/* stbu */
	if (DFORM_RA(inst) == 0)
		return false;
#if   defined(EMIT_ASM)
	Op("stbu");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ea = il->Add(regWidth,
		il->Register(regWidth, DFORM_RA(inst)),
		il->Const(regWidth, SEXT16(DFORM_D(inst)))
	);
	il->AddInstruction(
		il->Store(1, ea, il->Register(regWidth, DFORM_RS(inst)))
	);
	il->AddInstruction(
		il->SetRegister(regWidth, DFORM_RA(inst), ea)
	);
#endif
	return true;
case 40:
#if   defined(EMIT_ASM)
	Op("lhz");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* lhz */
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	if (DFORM_RA(inst) != 0) {
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei1, ei0);
	}
	ei0 = il->Load(2, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 41:
	/* lhzu */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
#if   defined(EMIT_ASM)
	Op("lhzu");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(2, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 42:
#if   defined(EMIT_ASM)
	Op("lha");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* lha */
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	if (DFORM_RA(inst) != 0) {
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei1, ei0);
	}
	ei0 = il->Load(2, ei0);
	ei0 = il->SignExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 43:
	/* lhau */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
#if   defined(EMIT_ASM)
	Op("lhau");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(2, ei0);
	ei0 = il->SignExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
#endif
	return true;
case 44:
#if   defined(EMIT_ASM)
	Op("sth");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	/* sth */
	if (DFORM_RA(inst) == 0) {
		ea = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	} else {
		ea = il->Add(regWidth,
			il->Register(regWidth, DFORM_RA(inst)),
			il->Const(regWidth, SEXT16(DFORM_D(inst)))
		);
	}
	il->AddInstruction(
		il->Store(2, ea, il->Register(regWidth, DFORM_RS(inst)))
	);
#endif
	return true;
case 45:
	/* sthu */
	if (DFORM_RA(inst) == 0)
		return false;
#if   defined(EMIT_ASM)
	Op("sthu");
	Reg(DFORM_RS(inst));
	Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
	ea = il->Add(regWidth,
		il->Register(regWidth, DFORM_RA(inst)),
		il->Const(regWidth, SEXT16(DFORM_D(inst)))
	);
	il->AddInstruction(
		il->Store(2, ea, il->Register(regWidth, DFORM_RS(inst)))
	);
	il->AddInstruction(
		il->SetRegister(regWidth, DFORM_RA(inst), ea)
	);
#endif
	return true;
case 46:
#if   defined(EMIT_ASM)
	Op("lmw");
#elif defined(EMIT_IL)
	/* lmw */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 47:
#if   defined(EMIT_ASM)
	Op("stmw");
#elif defined(EMIT_IL)
	/* stmw */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 48:
#if   defined(EMIT_ASM)
	Op("lfs");
#elif defined(EMIT_IL)
	/* lfs */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 49:
#if   defined(EMIT_ASM)
	Op("lfsu");
#elif defined(EMIT_IL)
	/* lfsu */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 50:
#if   defined(EMIT_ASM)
	Op("lfd");
#elif defined(EMIT_IL)
	/* lfd */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 51:
#if   defined(EMIT_ASM)
	Op("lfdu");
#elif defined(EMIT_IL)
	/* lfdu */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 52:
#if   defined(EMIT_ASM)
	Op("stfs");
#elif defined(EMIT_IL)
	/* stfs */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 53:
#if   defined(EMIT_ASM)
	Op("stfsu");
#elif defined(EMIT_IL)
	/* stfsu */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 54:
#if   defined(EMIT_ASM)
	Op("stfd");
#elif defined(EMIT_IL)
	/* stfd */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 55:
#if   defined(EMIT_ASM)
	Op("stfdu");
#elif defined(EMIT_IL)
	/* stfdu */
	il->AddInstruction(il->Unimplemented());
#endif
	return true;
case 56:
	/* Reserved */
	return false;
case 57:
	/* Reserved */
	return false;
case 58:
	return DECODE_GROUP(58)(inst);
case 59:
	return DECODE_GROUP(59)(inst);
case 60:
	/* Reserved */
	return false;
case 61:
	/* Reserved */
	return false;
case 62:
	return DECODE_GROUP(62)(inst);
case 63:
	return DECODE_GROUP(63)(inst);
default:
	return false;
}
}

bool DECODE_GROUP(19)(uint32_t inst) {
	uint32_t op = (inst >> 1) & 0b1111111111;
	switch (op) {
	case 150:
#if   defined(EMIT_ASM)
		Op("isync");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::isync), {}));
#endif
		return true;
	default:
#if   defined(EMIT_ASM)
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
		//return false;
	}
}

bool DECODE_GROUP(30)(uint32_t inst) {
	uint32_t op = MDSFORM_XO(inst);
	ExprId r, m, mInv;
#define MASK_64 0xffffffffffffffff 
	switch (op) {
	case 0:
	case 1:
#if   defined(EMIT_ASM)
		Op("rldicl");
		Reg(MDFORM_RA(inst));
		Reg(MDFORM_RS(inst));
		Imm(MDFORM_sh(inst));
		Imm(MDFORM_mb(inst));
#elif defined(EMIT_IL)
		if (MDFORM_sh(inst) == 64-MDFORM_mb(inst)) {
			// Simplification: srdi
			il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst),
				il->LogicalShiftRight(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_mb(inst)))
			));
		} else {
			/* rldicl */
			// r <- ROTL64((RS), sh)
			r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
			// m <- MASK(mb, 63)
			m = il->Const(8, MASK_64 >> MDFORM_mb(inst));
			// RA <- r & m
			il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
		}
#endif
		return true;
	case 2:
	case 3:
#if   defined(EMIT_ASM)
		Op("rldicr");
		Reg(MDFORM_RA(inst));
		Reg(MDFORM_RS(inst));
		Imm(MDFORM_sh(inst));
		Imm(MDFORM_mb(inst));
#elif defined(EMIT_IL)
		/* rldicr */
		// r <- ROTL64((RS), sh)
		r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
		// m <- MASK(0, e)
		m = il->Const(8, MASK_64 << (64 - MDFORM_mb(inst)));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
#endif
		return true;
	case 4:
	case 5:
#if   defined(EMIT_ASM)
		Op("rldic");
		Reg(MDFORM_RA(inst));
		Reg(MDFORM_RS(inst));
		Imm(MDFORM_sh(inst));
		Imm(MDFORM_mb(inst));
#elif defined(EMIT_IL)
		/* rldic */
		// r <- ROTL64((RS), sh)
		r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
		// m <- MASK(mb, ~sh)
		m = il->Const(8, (MASK_64 >> MDFORM_mb(inst)) & (MASK_64 << MDFORM_sh(inst)));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
#endif
		return true;
	case 6:
	case 7:
#if   defined(EMIT_ASM)
		Op("rldimi");
		Reg(MDFORM_RA(inst));
		Reg(MDFORM_RS(inst));
		Imm(MDFORM_sh(inst));
		Imm(MDFORM_mb(inst));
#elif defined(EMIT_IL)
		/* rldimi */
		// r <- ROTL64((RS), sh)
		r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
		// m <- MASK(mb, ~sh)
		m = il->Const(8, (MASK_64 >> MDFORM_mb(inst)) & (MASK_64 << MDFORM_sh(inst)));
		mInv = il->Const(8, ~((MASK_64 >> MDFORM_mb(inst)) & (MASK_64 << MDFORM_sh(inst))));
		// RA <- (r&m) | ((RA)&~m)
		il->AddInstruction(
			il->SetRegister(8, MDFORM_RA(inst), il->Or(8, il->And(8, r, m), il->And(8, il->Register(8, MDFORM_RA(inst)), mInv)))
		);
#endif
		return true;
	case 8:
#if   defined(EMIT_ASM)
		Op("rldcl");
		Reg(MDFORM_RA(inst));
		Reg(MDFORM_RS(inst));
		Reg(MDFORM_RB(inst));
		Imm(MDFORM_mb(inst));
#elif defined(EMIT_IL)
		/* rldcl */
		// r <- ROTL64((RS), (RB)_58:63)
		r = il->RotateLeft(8,
			il->Register(8, MDFORM_RS(inst)),
			il->And(1, il->Register(1, MDFORM_RB(inst)), il->Const(1, 0b111111))
		);
		// m <- MASK(mb, 63)
		m = il->Const(8, MASK_64 >> MDFORM_mb(inst));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
#endif
		return true;
	case 9:
#if   defined(EMIT_ASM)
		Op("rldcr");
		Reg(MDFORM_RA(inst));
		Reg(MDFORM_RS(inst));
		Reg(MDFORM_RB(inst));
		Imm(MDFORM_mb(inst));
#elif defined(EMIT_IL)
		/* rldcr */
		// r <- ROTL64((RS), (RB)_58:63)
		r = il->RotateLeft(8,
			il->Register(8, MDFORM_RS(inst)),
			il->And(1, il->Register(1, MDFORM_sh(inst)), il->Const(1, 0b111111))
		);
		// m <- MASK(0, e)
		m = il->Const(8, MASK_64 << (64 - MDFORM_mb(inst)));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
#endif
		return true;
	default:
		/* Reserved */
		return false;
	}
#undef MASK_64
}

bool DECODE_GROUP(31)(uint32_t inst) {	
	uint32_t op = (inst >> 1) & 0b1111111111;
	char buf[16];
	switch (op) {
	case 0:
#if   defined(EMIT_ASM)
		Op("cmp");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 4:
#if   defined(EMIT_ASM)
		Op("tw");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 8:
	case 520:
#if   defined(EMIT_ASM)
		Op("subfc[o][.]");
#elif defined(EMIT_IL)
		// TODO: OE and Rc flags
		// TODO: CA flag
		il->AddInstruction(il->SetRegister(8, XOFORM_RT(inst), il->Sub(8,
				il->Register(8, XOFORM_RB(inst)),
				il->Register(8, XOFORM_RA(inst))
		)));
#endif
		return true;
	case 9:
	case 521:
#if   defined(EMIT_ASM)
		Op("mulhdu");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 10:
	case 522:
#if   defined(EMIT_ASM)
		Op("addc[o][.]");
#elif defined(EMIT_IL)
		// TODO: OE and Rc flags
		// TODO: CA flag
		il->AddInstruction(il->SetRegister(8, XOFORM_RT(inst), il->Add(8,
				il->Register(8, XOFORM_RB(inst)),
				il->Register(8, XOFORM_RA(inst))
		)));
#endif
		return true;
	case 11:
	case 523:
#if   defined(EMIT_ASM)
		Op("mulhwu");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 19:
#if   defined(EMIT_ASM)
		Op("mfcr");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 20:
#if   defined(EMIT_ASM)
		Op("lwarx");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 21:
#if   defined(EMIT_ASM)
		Op("ldx");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 23:
#if   defined(EMIT_ASM)
		Op("lwzx");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 24:
#if   defined(EMIT_ASM)
		Op("slw[.]");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 26:
#if   defined(EMIT_ASM)
		Op("cntlzw[.]")
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 27:
#if   defined(EMIT_ASM)
		Op("sld[.]");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 28:
#if   defined(EMIT_ASM)
		Op("and[.]");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 32:
#if   defined(EMIT_ASM)
		Op("cmpl");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 40:
	case 552:
#if   defined(EMIT_ASM)
		Op("subf[o][.]");
#elif defined(EMIT_IL)
		// TODO: OE and Rc flags
		il->AddInstruction(il->SetRegister(8, XOFORM_RT(inst), il->Sub(8,
				il->Register(8, XOFORM_RB(inst)),
				il->Register(8, XOFORM_RA(inst))
		)));
#endif
		return true;
	case 53:
#if   defined(EMIT_ASM)
		Op("ldux");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 54:
#if   defined(EMIT_ASM)
		Op("dcbst");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 55:
#if   defined(EMIT_ASM)
		Op("lwzux");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 58:
#if   defined(EMIT_ASM)
		Op("cntlzd[.]");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 60:
#if   defined(EMIT_ASM)
		Op("andc[.]");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 68:
#if   defined(EMIT_ASM)
		Op("td");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Unimplemented());
#endif
		return true;
	case 246:
#if   defined(EMIT_ASM)
		Op("dcbtst");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::dcbtst), {}));
#endif
		return true;
	case 266:
	case 778:
#if   defined(EMIT_ASM)
#elif defined(EMIT_IL)
		// TODO: OE and Rc flags
		il->AddInstruction(
			il->SetRegister(8, XOFORM_RT(inst), il->Add(8,
				il->Register(8, XOFORM_RA(inst)),
				il->Register(8, XOFORM_RB(inst))
			))
		);
#endif
		return true;
	case 274:
#if   defined(EMIT_ASM)
		Op("tlbiel");
		Reg(XFORM_RB(inst));
		Imm(XFORM_L(inst));
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbiel), {
			il->Register(8, XFORM_RB(inst)),
			il->Const(1, XFORM_L(inst)),
		}));
#endif
		return true;
	case 278:
		// TODO: decode operands
#if   defined(EMIT_ASM)
		Op("dcbt");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::dcbt), {}));
#endif
		return true;
	case 306:
#if   defined(EMIT_ASM)
		Op("tlbie");
		Reg(XFORM_RB(inst));
		Imm(XFORM_L(inst));
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbie), {
			il->Register(8, XFORM_RB(inst)),
			il->Const(1, XFORM_L(inst)),
		}));
#endif
		return true;
	case 339:
#if   defined(EMIT_ASM)
		switch (XFXFORM_SPR(inst)) {
		case 1:
			Op("mfxer");
			Reg(XFXFORM_RS(inst));
			break;
		case 8:
			Op("mflr");
			Reg(XFXFORM_RS(inst));
			break;
		case 9:
			Op("mfctr");
			Reg(XFXFORM_RS(inst));
			break;
		default:
			Op("mfspr");
			Reg(XFXFORM_RS(inst));
			Imm(XFXFORM_SPR(inst));
			break;
		}
#elif defined(EMIT_IL)
		switch (XFXFORM_SPR(inst)) {
		/*case 1:
			Op("mfxer");
			Reg(XFXFORM_RS(inst));
			break;
		case 8:
			Op("mflr");
			Reg(XFXFORM_RS(inst));
			break;*/
		case 9:
			il->AddInstruction(il->SetRegister(8, XFXFORM_RS(inst), il->Register(8, PPC_REG_CTR)));
			break;
		default:
			il->AddInstruction(il->Intrinsic({
				RegisterOrFlag::Register(XFXFORM_RS(inst))
			}, static_cast<uint32_t>(Intrinsic::mfspr), {
				il->Const(2, XFXFORM_SPR(inst)),
			}));
			break;
		}
#endif
		return true;
	case 402:
#if   defined(EMIT_ASM)
		Op("slbmte");
		Reg(XFORM_RS(inst));
		Reg(XFORM_RB(inst));
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::slbmte), {
			il->Register(8, XFORM_RS(inst)),
			il->Register(8, XFORM_RB(inst)),
		}));
#endif
		return true;
	case 434:
#if   defined(EMIT_ASM)
		Op("slbie");
		Reg(XFORM_RB(inst));
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::slbie), {
			il->Register(8, XFORM_RB(inst)),
		}));
#endif
		return true;
	case 444:
		if (XFORM_RA(inst) == XFORM_RB(inst)) {
			/* mr */
#if   defined(EMIT_ASM)
			Op("mr");
			Reg(XFORM_RA(inst));
			Reg(XFORM_RS(inst));
#elif defined(EMIT_IL)
			il->AddInstruction(il->SetRegister(8, XFORM_RA(inst), il->Register(8, XFORM_RS(inst))));
#endif
		} else {
			/* or / or. */
#if   defined(EMIT_ASM)
			Op("or");
			Reg(XFORM_RA(inst));
			Reg(XFORM_RS(inst));
			Reg(XFORM_RB(inst));
#elif defined(EMIT_IL)
			// TODO: CR0
			il->AddInstruction(il->SetRegister(8, XFORM_RA(inst), il->Or(8,
				il->Register(8, XFORM_RS(inst)),
				il->Register(8, XFORM_RB(inst))
			)));
#endif
		}
		return true;
	case 467:
#if   defined(EMIT_ASM)
		switch (XFXFORM_SPR(inst)) {
		case 1:
			Op("mtxer");
			Reg(XFXFORM_RS(inst));
			break;
		case 8:
			Op("mtlr");
			Reg(XFXFORM_RS(inst));
			break;
		case 9:
			Op("mtctr");
			Reg(XFXFORM_RS(inst));
			break;
		default:
			Op("mtspr");
			Imm(XFXFORM_SPR(inst));
			Reg(XFXFORM_RS(inst));
			break;
		}
#elif defined(EMIT_IL)
		switch (XFXFORM_SPR(inst)) {
		/*case 1:
			Op("mtxer");
			Reg(XFXFORM_RS(inst));
			break;
		case 8:
			Op("mtlr");
			Reg(XFXFORM_RS(inst));
			break;*/
		case 9:
			il->AddInstruction(il->SetRegister(8, PPC_REG_CTR, il->Register(8, XFXFORM_RS(inst))));
			break;
		default:
			il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::mtspr), {
				il->Const(2, XFXFORM_SPR(inst)),
				il->Register(8, XFXFORM_RS(inst)),
			}));
			break;
		}
#endif
		return true;
	case 370:
#if   defined(EMIT_ASM)
		Op("tlbia");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbia), {}));
#endif
		return true;
	case 566:
#if   defined(EMIT_ASM)
		Op("tlbsync");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbsync), {}));
#endif
		return true;
	case 598:
		// TODO: variants
#if   defined(EMIT_ASM)
		Op("sync");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::sync), {}));
#endif
		return true;
	case 854:
#if   defined(EMIT_ASM)
		Op("eieio");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::eieio), {}));
#endif
		return true;
	case 982:
#if   defined(EMIT_ASM)
		Op("icbi");
#elif defined(EMIT_IL)
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::icbi), {}));
#endif
		return true;
	}
	return false;
}

bool DECODE_GROUP(58)(uint32_t inst) {
	uint32_t op = inst & 0b11;
#ifdef EMIT_IL
	ExprId ea;
#endif
	switch (op) {
	case 0:
		/* ld */
#if   defined(EMIT_ASM)
		Op("ld");
		Reg(DSFORM_RT(inst));
		Disp(DSFORM_RA(inst), DSFORM_DS(inst));
#elif defined(EMIT_IL)
		if (DSFORM_RA(inst) == 0) {
			// if RA = 0 then b <- 0
			// thus: EA <- EXTS(DS || 0b00)
			ea = il->Const(8, SEXT16(DSFORM_DS(inst)));
		} else {
			// else b <- (RA)
			// thus: EA <- (RA) + EXTS(DS || 0b00)
			ea = il->Add(8, il->Register(8, DSFORM_RA(inst)), il->Const(8, SEXT16(DSFORM_DS(inst))));
		}
		// RT <- MEM(EA, 8)
		il->AddInstruction(il->SetRegister(8, DSFORM_RT(inst), il->Load(8, ea)));
#endif
		return true;
	case 1:
		/* ldu */
		if (DSFORM_RA(inst) == 0 || DSFORM_RA(inst) == DSFORM_RT(inst))
			return false;
#if   defined(EMIT_ASM)
		Op("ldu");
		Reg(DSFORM_RT(inst));
		Disp(DSFORM_RA(inst), DSFORM_DS(inst));
#elif defined(EMIT_IL)
		// EA <- (RA) + EXTS(DS || 0b00)
		ea = il->Add(8, il->Register(8, DSFORM_RA(inst)), il->Const(8, SEXT16(DSFORM_DS(inst))));
		// RT <- MEM(EA, 8)
		il->AddInstruction(
			il->SetRegister(8, DSFORM_RT(inst), il->Load(8, ea))
		);
		// RA <- EA
		il->AddInstruction(
			il->SetRegister(8, DSFORM_RA(inst), ea)
		);
#endif
		return true;
	case 2:
		/* lwa */
#if   defined(EMIT_ASM)
		Op("lwa");
		Reg(DSFORM_RT(inst));
		Disp(DSFORM_RA(inst), DSFORM_DS(inst));
#elif defined(EMIT_IL)
		if (DSFORM_RA(inst) == 0) {
			// if RA = 0 then b <- 0
			// thus: EA <- EXTS(DS || 0b00)
			ea = il->Const(8, SEXT16(DSFORM_DS(inst)));
		} else {
			// else b <- (RA)
			// thus: EA <- (RA) + EXTS(DS || 0b00)
			ea = il->Add(8, il->Register(8, DSFORM_RA(inst)), il->Const(8, SEXT16(DSFORM_DS(inst))));
		}
		// RT <- EXTS(MEM(EA, 4))
		il->AddInstruction(
			il->SetRegister(8, DSFORM_RT(inst), il->SignExtend(8, il->Load(4, ea)))
		);
#endif
		return true;
	case 3:
		/* Reserved */
		return false;
	default:
		/* unreachable */
		return false;
	}
}

bool DECODE_GROUP(59)(uint32_t inst) { return false; }

bool DECODE_GROUP(62)(uint32_t inst) {
	uint32_t op = inst & 0b11;
	size_t regWidth = 8;
	ExprId ea;
	switch (op) {
	case 0:
		/* std */
#if   defined(EMIT_ASM)
		Op("std");
		Reg(DFORM_RT(inst));
		Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
		if (DFORM_RA(inst) == 0) {
			ea = il->Const(regWidth, SEXT16(DFORM_D(inst)));
		} else {
			ea = il->Add(regWidth,
				il->Register(regWidth, DFORM_RA(inst)),
				il->Const(regWidth, SEXT16(DFORM_D(inst)))
			);
		}
		il->AddInstruction(
			il->Store(8, ea, il->Register(regWidth, DFORM_RS(inst)))
		);
#endif
		return true;
	case 1:
		/* stdu */
#if   defined(EMIT_ASM)
		Op("stdu");
		Reg(DFORM_RT(inst));
		Disp(DFORM_RA(inst), DFORM_D(inst));
#elif defined(EMIT_IL)
		if (DFORM_RA(inst) == 0)
			return false;
		ea = il->Add(regWidth,
			il->Register(regWidth, DFORM_RA(inst)),
			il->Const(regWidth, SEXT16(DFORM_D(inst) & 0xfffc))
		);
		il->AddInstruction(
			il->Store(8, ea, il->Register(regWidth, DFORM_RS(inst)))
		);
		il->AddInstruction(
			il->SetRegister(regWidth, DFORM_RA(inst), ea)
		);
#endif
		return true;
	case 2:
		/* Reserved */
		return false;
	case 3:
		/* Reserved */
		return false;
	default:
		/* unreachable */
		return false;
	}
}

bool DECODE_GROUP(63)(uint32_t inst) { return false; }
