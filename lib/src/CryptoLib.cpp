/*++

Copyright (c) SubSpace Library.

Module:

    CryptoLib.cpp

--*/

#include "CryptoLib.h"

// SubSpaceCrypt implementation
SubSpaceCrypt::SubSpaceCrypt(int32 key) : mKey(key)
{
}

SubSpaceCrypt::~SubSpaceCrypt()
{
}

void SubSpaceCrypt::Encrypt(bstring & data)
{
    // Stub implementation - simple XOR
    for(size_t i = 0; i < data.length(); ++i)
    {
        data[i] ^= (mKey >> (i % 4 * 8)) & 0xFF;
    }
}

void SubSpaceCrypt::Decrypt(bstring & data)
{
    // Same as encrypt for XOR
    Encrypt(data);
}

// ContinuumCrypt implementation
ContinuumCrypt::ContinuumCrypt(const bstring & key) : mKey(key)
{
}

ContinuumCrypt::~ContinuumCrypt()
{
}

void ContinuumCrypt::Encrypt(bstring & data)
{
    // Stub implementation - simple XOR with key
    if(mKey.length() == 0)
        return;
    
    for(size_t i = 0; i < data.length(); ++i)
    {
        data[i] ^= mKey[i % mKey.length()];
    }
}

void ContinuumCrypt::Decrypt(bstring & data)
{
    // Same as encrypt for XOR
    Encrypt(data);
}
