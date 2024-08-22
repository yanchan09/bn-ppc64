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

class PpcDisassembler {
private:
	std::vector<InstructionTextToken> *result;

	void emitDForm(uint32_t inst, const std::string &name);
	void emitDFormDisplaced(uint32_t inst, const std::string &name);
	void emitMDForm(uint32_t inst, const std::string &name);
	void emitMDSForm(uint32_t inst, const std::string &name);

	bool decode31(uint32_t inst);
	bool decode30(uint32_t inst);
public:
	PpcDisassembler(std::vector<InstructionTextToken> *result) {
		this->result = result;
	}

	bool DecodeInstruction(const uint8_t *data);
};
