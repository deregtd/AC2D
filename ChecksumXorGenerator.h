/*
 * Bael'Zharon's Respite
 * Copyright (C) 2014 Daniel Skorupski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef BZR_CHECKSUMXORGENERATOR_H
#define BZR_CHECKSUMXORGENERATOR_H

#include <deque>

class ChecksumXorGenerator
{
public:
    ChecksumXorGenerator();

    void init(DWORD seed);
    DWORD get(DWORD sequence);
    void purge(DWORD sequence);

private:
    DWORD generate();
    void initTables();
    static void initMix(DWORD* xorvals);
    void scramble();
    static void scrambleRound(DWORD shiftedVal, DWORD* key0_ptr, DWORD* key2_ptr, DWORD** localunk_ptr, DWORD** lc_unk0_ptr, DWORD** lc_unk200_ptr, DWORD** localxor_ptr, DWORD* var_18_ptr, DWORD* var_1c_ptr);
    static DWORD Crazy_XOR_01(const DWORD* data, DWORD index);

    DWORD counter_;
    DWORD xorTable_[256];
    DWORD unkTable_[256];
    DWORD value0_;
    DWORD value1_;
    DWORD value2_;

    DWORD cacheBegin_;
    std::deque<DWORD> cache_;
};

#endif