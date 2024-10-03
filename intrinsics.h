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

#pragma once

#define FLAG_CR0_LT	0
#define FLAG_CR0_GT	1
#define FLAG_CR0_EQ	2
#define FLAG_CR0_SO	3

#define FLAG_CR1_FX	4
#define FLAG_CR1_FEX	5
#define FLAG_CR1_VX	6
#define FLAG_CR1_OX	7

// Flag values 8-63 are rest of the CR register

#define FLAG_XER_SO	64
#define FLAG_XER_OV	65
#define FLAG_XER_CA	66
#define FLAG__LAST	67

#define FLAG_WRITE_CR0	1 << 0
#define FLAG_WRITE_CA	1 << 1
#define FLAG_WRITE__MAX	1 << 2

enum class Intrinsic : uint32_t {
	dcbt,
	dcbtst,
	icbi,
	isync,
	mfspr,
	mtspr,
	slbie,
	slbmte,
	tlbiel,
	tlbie,
	tlbia,
	tlbsync,
	sync,
	eieio,
	ENUM_LAST
};
