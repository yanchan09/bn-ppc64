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

using namespace BinaryNinja;

class PpcLifter {
private:
	LowLevelILFunction *il;
	Architecture *arch;

	bool lift19(uint32_t inst);
	bool lift30(uint32_t inst);
	bool lift31(uint32_t inst);
	bool lift58(uint32_t inst);
	bool lift59(uint32_t inst);
	bool lift62(uint32_t inst);
	bool lift63(uint32_t inst);
public:
	PpcLifter(LowLevelILFunction *il, Architecture *arch) {
		this->il = il;
		this->arch = arch;
	}

	bool LiftInstruction(const uint8_t *data, uint64_t addr);
};
