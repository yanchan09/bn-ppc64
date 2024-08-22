# Copyright (C) 2024 yanchan09
#
# This program is free software: you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the Free 
# Software Foundation, version 3.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
# more details.
# 
# You should have received a copy of the GNU General Public License along 
# with this program. If not, see <https://www.gnu.org/licenses/>.

project('bn-ppc64', 'cpp')

cmake = import('cmake')

cm_opts = cmake.subproject_options()
cm_opts.add_cmake_defines({'HEADLESS': true})

bna_pro = cmake.subproject('binaryninja-api', options : cm_opts)

shared_library('bn_ppc64', [
  'plugin.cpp', 'disasm.cpp', 'il.cpp'
], dependencies : [
  bna_pro.dependency('binaryninjaapi'),
  bna_pro.dependency('fmt'),
])
