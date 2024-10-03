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

#define PPC_REG_CTR 32

#define DFORM_RS(i) ((i>>21)&0x1f)
#define DFORM_RT(i) ((i>>21)&0x1f)
#define DFORM_RA(i) ((i>>16)&0x1f)
#define DFORM_UI(i) (i&0xffff)
#define DFORM_SI(i) ((i&0x8000) ? (-((i&0xffff)^0xffff)) : (i&0xffff))
#define DFORM_D(i) (i&0xffff)
#define DFORM_BF(i) ((i>>23)&0x7)
#define DFORM_L(i) ((i>>21)&0x1)

#define DSFORM_RT(i) ((i>>21)&0x1f)
#define DSFORM_RA(i) ((i>>16)&0x1f)
#define DSFORM_DS(i) (i & 0xfffc)

#define IFORM_LI(i) (i & 0x3fffffc)

#define BFORM_BO(i) ((i>>21)&0x1f)
#define BFORM_BI(i) ((i>>16)&0x1f)
#define BFORM_BD(i) (i & 0xfffc)

#define MFORM_RS(i) ((i>>21)&0x1f)
#define MFORM_RA(i) ((i>>16)&0x1f)
#define MFORM_RB(i) ((i>>11)&0x1f)
#define MFORM_sh(i) ((i>>11)&0x1f)
#define MFORM_mb(i) ((i>>5)&0x3f)
#define MFORM_me(i) ((i>>1)&0x3f)
#define MFORM_Rc(i) (i&1)

#define MDFORM_RS(i) ((i>>21)&0x1f)
#define MDFORM_RA(i) ((i>>16)&0x1f)
#define MDFORM_RB(i) ((i>>11)&0x1f)
#define MDFORM_sh(i) (((i&0xf800) >> 11) | ((i&0x2) << 4))
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

