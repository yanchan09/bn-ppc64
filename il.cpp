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
#define MDFORM_sh(i) (((i&0xf800) >> 11) | ((i&0x2) << 4))
//#define MDFORM_mb(i) ((i>>5)&0x3f)
#define MDFORM_mb(i) (((i&0x7c0)>>6)|(i&0x20))
#define MDFORM_me(i) ((i>>5)&0x3f)
#define MDFORM_Rc(i) (i&1)

#define MDSFORM_RS(i) ((i>>21)&0x1f)
#define MDSFORM_RA(i) ((i>>16)&0x1f)
#define MDSFORM_RB(i) ((i>>11)&0x1f)
#define MDSFORM_mb(i) ((i>>5)&0x3f)
#define MDSFORM_me(i) ((i>>5)&0x3f)
#define MDSFORM_XO(i) ((i>>1)&0x0f)
#define MDSFORM_Rc(i) (i&1)

#define XFORM_L(i) ((i>>21)&0x1)
#define XFORM_RS(i) ((i>>21)&0x1f)
#define XFORM_RA(i) ((i>>16)&0x1f)
#define XFORM_RB(i) ((i>>11)&0x1f)

#define XFXFORM_RS(i) ((i>>21)&0x1f)
#define XFXFORM_SPR(i) (((i&0x1f0000)>>16)|((i&0xf800)>>6))

#define XOFORM_RT(i) ((i>>21)&0x1f)
#define XOFORM_RA(i) ((i>>16)&0x1f)
#define XOFORM_RB(i) ((i>>11)&0x1f)

#define SEXT16(i) (((i)&0x8000) ? (0xffffffffffff0000 | (i)) : (i))
#define SEXT26(i) (((i)&0x2000000) ? (0xfffffffffc000000 | (i)) : (i))
#define SEXT32(i) (((i)&0x80000000) ? (0xffffffff00000000 | (i)) : (i))

bool PpcLifter::LiftInstruction(const uint8_t *data, uint64_t addr) {
	uint32_t inst = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
	uint32_t op = inst >> 26;
	char buf[16];
	ExprId ei0, ei1;
	ExprId ea;
	ExprId cond;
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
	/* tdi */
	il->AddInstruction(il->Unimplemented());
	return true;
case 3:
	/* twi */
	il->AddInstruction(il->Unimplemented());
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
	/* mulli */
	il->AddInstruction(il->Unimplemented());
	return true;
case 8:
	/* subfic */
	il->AddInstruction(il->Unimplemented());
	return true;
case 9:
	/* dozi */
	il->AddInstruction(il->Unimplemented());
	return true;
case 10:
	/* cmpli */
	il->AddInstruction(il->Unimplemented());
	return true;
case 11:
	/* cmpi */
	il->AddInstruction(il->Unimplemented());
	return true;
case 12:
	/* addic */
	il->AddInstruction(il->Unimplemented());
	return true;
case 13:
	/* addic. */
	il->AddInstruction(il->Unimplemented());
	return true;
case 14:
	/* addi */
	if (DFORM_RA(inst) == 0) {
		ei0 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
	} else {
		ei0 = il->Const(regWidth, SEXT16(DFORM_UI(inst)));
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei0, ei1);
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
	}
	return true;
case 15:
	/* addis */
	if (DFORM_RA(inst) == 0) {
		ei0 = il->Const(regWidth, SEXT32(DFORM_UI(inst) << 16));
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
	} else {
		ei0 = il->Const(regWidth, SEXT32(DFORM_UI(inst) << 16));
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei0, ei1);
		ei0 = il->SetRegister(regWidth, DFORM_RS(inst), ei0);
		il->AddInstruction(ei0);
	}
	return true;
	case 16:
		/* bc/bca/bcl/bcla */
		if (op & 0x2)
			dst = SEXT16(inst & 0xfffc);
		else
			dst = addr + SEXT16(inst & 0xfffc);
		cond = il->Const(1, 1);
		label1 = il->GetLabelForAddress(arch, dst);
		label2 = il->GetLabelForAddress(arch, addr+4);
		if (label1 && label2)
			il->AddInstruction(il->If(cond, *label1, *label2));
		return true;
	case 17:
		/* sc */
		il->AddInstruction(il->Unimplemented());
		return true;
	case 18:
		/* b/ba/bl/bla */
		if (op & 0x2)
			dst = SEXT26(inst & 0x3fffffc);
		else
			dst = addr + SEXT26(inst & 0x3fffffc);
		if (op & 0x1)
			il->AddInstruction(il->Call(il->ConstPointer(8, dst)));
		else
			il->AddInstruction(il->Jump(il->ConstPointer(8, dst)));
		return true;
case 19:
	return lift19(inst);
case 20:
	/* rlwimi */
	il->AddInstruction(il->Unimplemented());
	return true;
case 21:
	/* rlwinm */
	il->AddInstruction(il->Unimplemented());
	return true;
case 22:
	/* rlmi */
	il->AddInstruction(il->Unimplemented());
	return true;
case 23:
	/* rlwnm */
	il->AddInstruction(il->Unimplemented());
	return true;
case 24:
	/* ori */
	if (DFORM_RA(inst) == 0 && DFORM_RS(inst) == 0 && DFORM_UI(inst) == 0) {
		// Preferred NOP encoding
		ei0 = il->Nop();
		il->AddInstruction(ei0);
	} else {
		ei1 = il->Const(regWidth, DFORM_UI(inst));
		ei0 = il->Register(regWidth, DFORM_RS(inst));
		ei0 = il->Or(regWidth, ei0, ei1);
		ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
		il->AddInstruction(ei0);
	}
	return true;
case 25:
	/* oris */
	ei1 = il->Const(regWidth, DFORM_UI(inst) << 16);
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Or(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 26:
	/* xori */
	ei1 = il->Const(regWidth, DFORM_UI(inst));
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Xor(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 27:
	/* xoris */
	ei1 = il->Const(regWidth, DFORM_UI(inst) << 16);
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Xor(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 28:
	/* andi. */
	// TODO: modify CR0
	ei1 = il->Const(regWidth, DFORM_UI(inst));
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->And(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 29:
	/* andis. */
	// TODO: modify CR0
	ei1 = il->Const(regWidth, DFORM_UI(inst) << 16);
	ei0 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->And(regWidth, ei0, ei1);
	ei0 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 30:
	return lift30(inst);
case 31:
	return lift31(inst);
case 32:
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
	return true;
case 33:
	/* lwzu */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(4, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 34:
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
	return true;
case 35:
	/* lbzu */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(1, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 36:
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
	return true;
case 37:
	/* stwu */
	if (DFORM_RA(inst) == 0)
		return false;
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
	return true;
case 38:
	/* stb */
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	if (DFORM_RA(inst) != 0) {
		ei1 = il->Register(regWidth, DFORM_RA(inst));
		ei0 = il->Add(regWidth, ei1, ei0);
	}
	ei1 = il->Register(regWidth, DFORM_RS(inst));
	ei0 = il->Store(1, ei0, ei1);
	il->AddInstruction(ei0);
	return true;
case 39:
	/* stbu */
	if (DFORM_RA(inst) == 0)
		return false;
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
	return true;
case 40:
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
	return true;
case 41:
	/* lhzu */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(2, ei0);
	ei0 = il->ZeroExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 42:
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
	return true;
case 43:
	/* lhau */
	if (DFORM_RA(inst) == 0 || DFORM_RA(inst) == DFORM_RT(inst))
		return false;
	ei0 = il->Const(regWidth, SEXT16(DFORM_D(inst)));
	ei1 = il->Register(regWidth, DFORM_RA(inst));
	ei0 = il->Add(regWidth, ei1, ei0);
	ei1 = il->SetRegister(regWidth, DFORM_RA(inst), ei0);
	il->AddInstruction(ei1);
	ei0 = il->Load(2, ei0);
	ei0 = il->SignExtend(regWidth, ei0);
	ei0 = il->SetRegister(regWidth, DFORM_RT(inst), ei0);
	il->AddInstruction(ei0);
	return true;
case 44:
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
	return true;
case 45:
	/* sthu */
	if (DFORM_RA(inst) == 0)
		return false;
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
	return true;
case 46:
	/* lmw */
	il->AddInstruction(il->Unimplemented());
	return true;
case 47:
	/* stmw */
	il->AddInstruction(il->Unimplemented());
	return true;
case 48:
	/* lfs */
	il->AddInstruction(il->Unimplemented());
	return true;
case 49:
	/* lfsu */
	il->AddInstruction(il->Unimplemented());
	return true;
case 50:
	/* lfd */
	il->AddInstruction(il->Unimplemented());
	return true;
case 51:
	/* lfdu */
	il->AddInstruction(il->Unimplemented());
	return true;
case 52:
	/* stfs */
	il->AddInstruction(il->Unimplemented());
	return true;
case 53:
	/* stfsu */
	il->AddInstruction(il->Unimplemented());
	return true;
case 54:
	/* stfd */
	il->AddInstruction(il->Unimplemented());
	return true;
case 55:
	/* stfdu */
	il->AddInstruction(il->Unimplemented());
	return true;
case 56:
	/* Reserved */
	return false;
case 57:
	/* Reserved */
	return false;
case 58:
	return lift58(inst);
case 59:
	return lift59(inst);
case 60:
	/* Reserved */
	return false;
case 61:
	/* Reserved */
	return false;
case 62:
	return lift62(inst);
case 63:
	return lift63(inst);
default:
	return false;
}
}


bool PpcLifter::lift19(uint32_t inst) {
	uint32_t op = (inst >> 1) & 0b1111111111;
	switch (op) {
	case 150:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::isync), {}));
		return true;
	default:
		il->AddInstruction(il->Unimplemented());
		return true;
		//return false;
	}
}

bool PpcLifter::lift30(uint32_t inst) {
	uint32_t op = MDSFORM_XO(inst);
	ExprId r, m, mInv;
#define MASK_64 0xffffffffffffffff 
	switch (op) {
	case 0:
	case 1:
		/* rldicl */
		// r <- ROTL64((RS), sh)
		r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
		// m <- MASK(mb, 63)
		m = il->Const(8, MASK_64 >> MDFORM_mb(inst));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
		return true;
	case 2:
	case 3:
		/* rldicr */
		// r <- ROTL64((RS), sh)
		r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
		// m <- MASK(0, e)
		m = il->Const(8, MASK_64 << (64 - MDFORM_mb(inst)));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
		return true;
	case 4:
	case 5:
		/* rldic */
		// r <- ROTL64((RS), sh)
		r = il->RotateLeft(8, il->Register(8, MDFORM_RS(inst)), il->Const(1, MDFORM_sh(inst)));
		// m <- MASK(mb, ~sh)
		m = il->Const(8, (MASK_64 >> MDFORM_mb(inst)) & (MASK_64 << MDFORM_sh(inst)));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
		return true;
	case 6:
	case 7:
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
		return true;
	case 8:
		/* rldcl */
		// r <- ROTL64((RS), (RB)_58:63)
		r = il->RotateLeft(8,
			il->Register(8, MDFORM_RS(inst)),
			il->And(1, il->Register(1, MDFORM_sh(inst)), il->Const(1, 0b111111))
		);
		// m <- MASK(mb, 63)
		m = il->Const(8, MASK_64 >> MDFORM_mb(inst));
		// RA <- r & m
		il->AddInstruction(il->SetRegister(8, MDFORM_RA(inst), il->And(8, r, m)));
		return true;
	case 9:
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
		return true;
	default:
		/* Reserved */
		return false;
	}
#undef MASK_64
}

bool PpcLifter::lift31(uint32_t inst) {	
	uint32_t op = (inst >> 1) & 0b1111111111;
	char buf[16];
	switch (op) {
	case 8:
	case 520:
		/* subfc */
		// TODO: OE and Rc flags
		// TODO: CA flag
		il->AddInstruction(il->SetRegister(8, XOFORM_RT(inst), il->Sub(8,
				il->Register(8, XOFORM_RB(inst)),
				il->Register(8, XOFORM_RA(inst))
		)));
		return true;
	case 10:
	case 522:
		/* addc */
		// TODO: OE and Rc flags
		// TODO: CA flag
		il->AddInstruction(il->SetRegister(8, XOFORM_RT(inst), il->Add(8,
				il->Register(8, XOFORM_RB(inst)),
				il->Register(8, XOFORM_RA(inst))
		)));
		return true;
	case 40:
	case 552:
		// TODO: OE and Rc flags
		il->AddInstruction(il->SetRegister(8, XOFORM_RT(inst), il->Sub(8,
				il->Register(8, XOFORM_RB(inst)),
				il->Register(8, XOFORM_RA(inst))
		)));
		return true;
	case 246:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::dcbtst), {}));
		return true;
	case 266:
	case 778:
		// TODO: OE and Rc flags
		il->AddInstruction(
			il->SetRegister(8, XOFORM_RT(inst), il->Add(8,
				il->Register(8, XOFORM_RA(inst)),
				il->Register(8, XOFORM_RB(inst))
			))
		);
		return true;
	case 274:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbiel), {
			il->Register(8, XFORM_RB(inst)),
			il->Const(1, XFORM_L(inst)),
		}));
		return true;
	case 278:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::dcbt), {}));
		return true;
	case 306:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbie), {
			il->Register(8, XFORM_RB(inst)),
			il->Const(1, XFORM_L(inst)),
		}));
		return true;
	case 339:
		il->AddInstruction(il->Intrinsic({
			RegisterOrFlag::Register(XFXFORM_RS(inst))
		}, static_cast<uint32_t>(Intrinsic::mfspr), {
			il->Const(2, XFXFORM_SPR(inst)),
		}));
		return true;
	case 402:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::slbmte), {
			il->Register(8, XFORM_RS(inst)),
			il->Register(8, XFORM_RB(inst)),
		}));
		return true;
	case 434:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::slbie), {
			il->Register(8, XFORM_RB(inst)),
		}));
		return true;
	case 444:
		if (XFORM_RA(inst) == XFORM_RB(inst)) {
			/* mr */
			il->AddInstruction(il->SetRegister(8, XFORM_RA(inst), il->Register(8, XFORM_RS(inst))));
		} else {
			/* or / or. */
			il->AddInstruction(il->SetRegister(8, XFORM_RA(inst), il->Or(8,
				il->Register(8, XFORM_RS(inst)),
				il->Register(8, XFORM_RB(inst))
			)));
		}
		return true;
	case 467:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::mtspr), {
			il->Const(2, XFXFORM_SPR(inst)),
			il->Register(8, XFXFORM_RS(inst)),
		}));
		return true;
	case 370:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbia), {}));
		return true;
	case 566:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::tlbsync), {}));
		return true;
	case 598:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::sync), {}));
		return true;
	case 854:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::eieio), {}));
		return true;
	case 982:
		il->AddInstruction(il->Intrinsic({}, static_cast<uint32_t>(Intrinsic::icbi), {}));
		return true;
	default:
		il->AddInstruction(il->Unimplemented());
		return true;
		//return false;
	}
}

bool PpcLifter::lift58(uint32_t inst) {
	uint32_t op = inst & 0b11;
	switch (op) {
	case 0:
		/* ld */
		il->AddInstruction(il->Unimplemented());
		return true;
	case 1:
		/* ldu */
		il->AddInstruction(il->Unimplemented());
		return true;
	case 2:
		/* lwa */
		il->AddInstruction(il->Unimplemented());
		return true;
	case 3:
		/* Reserved */
		return false;
	default:
		/* unreachable */
		return false;
	}
}

bool PpcLifter::lift59(uint32_t inst) { return false; }

bool PpcLifter::lift62(uint32_t inst) {
	uint32_t op = inst & 0b11;
	size_t regWidth = 8;
	ExprId ea;
	switch (op) {
	case 0:
		/* std */
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
		return true;
	case 1:
		/* stdu */
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

bool PpcLifter::lift63(uint32_t inst) { return false; }
