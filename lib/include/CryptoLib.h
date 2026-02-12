/*++

Copyright (c) SubSpace Library.

Module:

    CryptoLib.h

Description:

    Cryptography library for SubSpace applications.

--*/

#ifndef _CRYPTOLIB_H_
#define _CRYPTOLIB_H_

#include "base.h"
#include "NetLib.h"

class ICryptosystem
{
public:
    virtual ~ICryptosystem() {}
    virtual void Encrypt(bstring & data) = 0;
    virtual void Decrypt(bstring & data) = 0;
};

class SubSpaceCrypt : public ICryptosystem
{
public:
    SubSpaceCrypt(int32 key);
    virtual ~SubSpaceCrypt();
    
    virtual void Encrypt(bstring & data);
    virtual void Decrypt(bstring & data);
    
private:
    int32 mKey;
};

class ContinuumCrypt : public ICryptosystem
{
public:
    ContinuumCrypt(const bstring & key);
    virtual ~ContinuumCrypt();
    
    virtual void Encrypt(bstring & data);
    virtual void Decrypt(bstring & data);
    
private:
    bstring mKey;
};

#endif // _CRYPTOLIB_H_
