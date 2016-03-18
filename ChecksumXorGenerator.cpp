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

// Modified only to change type names

#include "stdafx.h"
#include "ChecksumXorGenerator.h"

static const DWORD kMaxCacheSize = 100;

ChecksumXorGenerator::ChecksumXorGenerator()
{
    cacheBegin_ = 0;
}

void ChecksumXorGenerator::init(DWORD seed)
{
    memset(xorTable_, 0, sizeof(xorTable_));
    value0_ = seed;
    value1_ = seed;
    value2_ = seed;
    initTables();

    cache_.clear();
    cacheBegin_ = 2; // there is no sequence 0 or 1
}

DWORD ChecksumXorGenerator::get(DWORD sequence)
{
    if(cacheBegin_ == 0)
    {
        // not initialized
        return 0;
    }

    if(sequence < cacheBegin_)
    {
        // sequence already purged
        return 0;
    }

    if(sequence >= cacheBegin_ + kMaxCacheSize)
    {
        // sequence too far in future
        return 0;
    }

    while(sequence >= cacheBegin_ + cache_.size())
    {
        cache_.push_back(generate());
    }

    return cache_[sequence - cacheBegin_];
}

void ChecksumXorGenerator::purge(DWORD sequence)
{
    if(cacheBegin_ == 0)
    {
        // not initialized
        return;
    }

    if(sequence >= cacheBegin_ + cache_.size())
    {
        // get not successful for sequence yet
        return;
    }

    while(sequence >= cacheBegin_)
    {
        cache_.pop_front();
        cacheBegin_++;
    }
}

DWORD ChecksumXorGenerator::generate()
{
    DWORD value = xorTable_[counter_];

    if(counter_ > 0)
    {
        counter_--;
    }
    else
    {
        scramble();
        counter_ = 255;
    }

    return value;
}

void ChecksumXorGenerator::initTables()
{
    DWORD xorvals[8];

    for(int i = 0; i < 8; i++)
    {
        xorvals[i] = 0x9E3779B9;
    }

    for(int i = 0; i < 4; i++)
    {
        initMix(xorvals);
    }

    for(int i = 0; i < 256; i += 8)
    {
        for(int j = 0; j < 8; j++)
        {
            xorvals[j] += xorTable_[i + j];
        }

        initMix(xorvals);

        for(int j = 0; j < 8; j++)
        {
            unkTable_[i + j] = xorvals[j];
        }
    }

    for(int i = 0; i < 256; i += 8)
    {
        for(int j = 0; j < 8; j++)
        {
            xorvals[j] += unkTable_[i + j];
        }

        initMix(xorvals);

        for(int j = 0; j < 8; j++)
        {
            unkTable_[i + j] = xorvals[j];
        }
    }

    scramble();
    counter_ = 255;
}

void ChecksumXorGenerator::initMix(DWORD* xorvals)
{
#define ROUND(base, shift) \
    xorvals[base] ^= xorvals[(base + 1) & 7] shift; \
    xorvals[(base + 3) & 7] += xorvals[base]; \
    xorvals[(base + 1) & 7] += xorvals[(base + 2) & 7];

    ROUND(0, << 0x0B);
    ROUND(1, >> 0x02);
    ROUND(2, << 0x08);
    ROUND(3, >> 0x10);
    ROUND(4, << 0x0A);
    ROUND(5, >> 0x04);
    ROUND(6, << 0x08);
    ROUND(7, >> 0x09);

#undef ROUND
}

void ChecksumXorGenerator::scramble()
{
    DWORD* local_unk = unkTable_;
    DWORD* local_xor = xorTable_;
    DWORD key0 = value0_;
    value2_++;
    DWORD key2 = value1_ + value2_;
    DWORD* lc_unk0 = local_unk;
    DWORD* lc_unk200 = lc_unk0 + 128;
    DWORD* lc_unk0_stop_point = lc_unk200;
    DWORD var_18;
    DWORD var_1c;

    while(lc_unk0 < lc_unk0_stop_point)
    {
        scrambleRound(key0 << 0x0D, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
        scrambleRound(key0 >> 0x06, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
        scrambleRound(key0 << 0x02, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
        scrambleRound(key0 >> 0x10, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
    }

    lc_unk200 = local_unk;

    while(lc_unk200 < lc_unk0_stop_point)
    {
        scrambleRound(key0 << 0x0D, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
        scrambleRound(key0 >> 0x06, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
        scrambleRound(key0 << 0x02, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
        scrambleRound(key0 >> 0x10, &key0, &key2, &local_unk, &lc_unk0, &lc_unk200, &local_xor, &var_18, &var_1c);
    }

    value1_ = key2;
    value0_ = key0;
}

void ChecksumXorGenerator::scrambleRound(
        DWORD shiftedVal,
        DWORD* key0_ptr,
        DWORD* key2_ptr,
        DWORD** localunk_ptr,
        DWORD** lc_unk0_ptr,
        DWORD** lc_unk200_ptr,
        DWORD** localxor_ptr,
        DWORD* var_18_ptr,
        DWORD* var_1c_ptr)
{
    *var_18_ptr = **lc_unk0_ptr;
    *key0_ptr = (*key0_ptr ^ shiftedVal) + **lc_unk200_ptr;
    *lc_unk200_ptr = *lc_unk200_ptr + 1;
    DWORD res = Crazy_XOR_01(*localunk_ptr, *var_18_ptr);
    *var_1c_ptr = res + *key0_ptr + *key2_ptr;
    **lc_unk0_ptr = *var_1c_ptr;
    *lc_unk0_ptr = *lc_unk0_ptr + 1;
    res = Crazy_XOR_01(*localunk_ptr, *var_1c_ptr >> 8);
    *key2_ptr = res + *var_18_ptr;
    **localxor_ptr = *key2_ptr;
    *localxor_ptr = *localxor_ptr + 1;
}

DWORD ChecksumXorGenerator::Crazy_XOR_01(const DWORD* data, DWORD index)
{
    return *(const DWORD*)((const BYTE*)data + (index & 0x3FC));
}
