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

enum class Flag : uint32_t {
	CR0_LT,
	CR0_GT,
	CR0_EQ,
	CR0_SO,
	XER_SO,
	XER_OV,
	XER_CA,
	_LAST
};

enum class FlagWriteType : uint32_t {
	CR0,
	_LAST
};

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
