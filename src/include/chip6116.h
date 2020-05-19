/*
Atanua Real-Time Logic Simulator
Copyright (c) 2008-2014 Jari Komppa

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#ifndef CHIP6116_H
#define CHIP6116_H

class Chip6116 : public Chip
{
    Pin mAddressPin[11];
    Pin mCtrlPin[3];  //CS=0,OE=1,WE=2
    Pin mIOPin[8];
    int mTexture;
    int mType;
    int mMemSize;
    unsigned char *mMemory;
public:
    Chip6116(int aType); // Ctor
    ~Chip6116(); // Dtor

    virtual void render(int aChipId);
    virtual void update(float aTick);
    virtual void serialize(File *f);
    virtual void deserialize(File *f);
    virtual void clone(Chip *aOther);

    enum { CS=0,OE=1,WE=2 } CPins;
};

#endif
