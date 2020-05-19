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
#include "atanua.h"
#include "atanua_internal.h"
#include "chip6116.h"
#include "fileutils.h"

Chip6116::Chip6116(int aType)
{
    mType = aType;
    const char * title;
    switch (mType)
    {
    case 16:
        title = "2048-word by 8-bit\n"
                "SRAM\n\n"
                "Click chip to give it\n"
                "keyboard focus, then\n"
                "press L to load\n"
                "contents.";
        mMemSize = 2048;
        break;
    }

    mMemory = new unsigned char[mMemSize];

    set(0,0,6.8,3.4,title);
    float xpos = 0.13 + 0.5475;
    mPin.push_back(&mAddressPin[8]);
    mAddressPin[8].set(xpos, -0.25, this, "Pin 23:A8"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[9]);
    mAddressPin[9].set(xpos, -0.25, this, "Pin 22:A9"); xpos += 0.5475;
    mPin.push_back(&mCtrlPin[2]);
    mCtrlPin[2].set(xpos, -0.25, this, "Pin 21:!WE"); xpos += 0.5475;
    mPin.push_back(&mCtrlPin[1]);
    mCtrlPin[1].set(xpos, -0.25, this, "Pin 20:!OE"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[10]);
    mAddressPin[10].set(xpos, -0.25, this, "Pin 19:A10"); xpos += 0.5475;
    mPin.push_back(&mCtrlPin[0]);
    mCtrlPin[0].set(xpos, -0.25, this, "Pin 18:!CS"); xpos += 0.5475;
    mPin.push_back(&mIOPin[7]);
    mIOPin[7].set(xpos, -0.25, this, "Pin 17:IO7"); xpos += 0.5475;
    mPin.push_back(&mIOPin[6]);
    mIOPin[6].set(xpos, -0.25, this, "Pin 16:IO6"); xpos += 0.5475;
    mPin.push_back(&mIOPin[5]);
    mIOPin[5].set(xpos, -0.25, this, "Pin 15:IO5"); xpos += 0.5475;
    mPin.push_back(&mIOPin[4]);
    mIOPin[4].set(xpos, -0.25, this, "Pin 14:IO4"); xpos += 0.5475;
    mPin.push_back(&mIOPin[3]);
    mIOPin[3].set(xpos, -0.25, this, "Pin 13:IO3"); xpos += 0.5475;

    xpos = 0.15;

    mPin.push_back(&mAddressPin[7]);
    mAddressPin[7].set(xpos, 3.1, this, "Pin 1:A7"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[6]);
    mAddressPin[6].set(xpos, 3.1, this, "Pin 2:A6"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[5]);
    mAddressPin[5].set(xpos, 3.1, this, "Pin 3:A5"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[4]);
    mAddressPin[4].set(xpos, 3.1, this, "Pin 4:A4"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[3]);
    mAddressPin[3].set(xpos, 3.1, this, "Pin 5:A3"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[2]);
    mAddressPin[2].set(xpos, 3.1, this, "Pin 6:A2"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[1]);
    mAddressPin[1].set(xpos, 3.1, this, "Pin 7:A1"); xpos += 0.5475;
    mPin.push_back(&mAddressPin[0]);
    mAddressPin[0].set(xpos, 3.1, this, "Pin 8:A0"); xpos += 0.5475;
    mPin.push_back(&mIOPin[0]);
    mIOPin[0].set(xpos, 3.1, this, "Pin 9:IO0"); xpos += 0.5475;
    mPin.push_back(&mIOPin[1]);
    mIOPin[1].set(xpos, 3.1, this, "Pin 10:IO1"); xpos += 0.5475;
    mPin.push_back(&mIOPin[2]);
    mIOPin[2].set(xpos, 3.1, this, "Pin 11:IO2"); xpos += 0.5475;

    mTexture = load_texture("data/chip_24pin.png");
}

Chip6116::~Chip6116()
{
    delete[] mMemory;
    mMemory = NULL;
}


void Chip6116::render(int aChipId)
{
    const char * label;
    switch (mType)
    {
    case 16:
        label = "6116";
        break;
    }
    drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
    fn.drawstring(label,mX+2.5,mY+1.35,0x5fffffff,0.75);
    
    if (gUIState.kbditem == aChipId)
    {
        if (gUIState.keyentered == SDLK_l)
        {
            FILE * f = openfiledialog("Load EEPROM Contents");
            if (f)
            {
                fread(mMemory,mMemSize,1,f);
                fclose(f);
            }
        }
    }
}

void Chip6116::update(float aTick) 
{
    int i;
//    for (i = 0; i < 8; i++)
//        if (mAddressPin[i].mNet)
//        mIOPin[i].setState((mAddressPin[i].mNet->mState != NETSTATE_LOW)?PINSTATE_WRITE_HIGH:PINSTATE_WRITE_LOW);

    // 1. if enable pins are bad, go bad
    
    if (mCtrlPin[CS].mNet == NULL ||
        mCtrlPin[OE].mNet == NULL ||
        mCtrlPin[WE].mNet == NULL ||
        mCtrlPin[CS].mNet->mState == NETSTATE_INVALID ||
        mCtrlPin[OE].mNet->mState == NETSTATE_INVALID ||
        mCtrlPin[WE].mNet->mState == NETSTATE_INVALID
        )
    {
        for(i=0; i<8; i++)
        {
            mIOPin[i].setState(gConfig.mPropagateInvalidState);
        }
    }
    else
    {
        // 2. if enable pins are down, go NC

        if(mCtrlPin[CS].mNet->mState!=NETSTATE_LOW ||
            mCtrlPin[OE].mNet->mState==mCtrlPin[WE].mNet->mState)
        {
            for(i=0; i<8; i++)
            {
                mIOPin[i].setState(PINSTATE_HIGHZ);
            }
        }
        else
        {
            // 3. if input pins are bad, go bad

            int valid=1;
            int range=10;
            if(mType>8) range++;
            if(mType>16) range++;
            for(i=0; i<range; i++)
            {
                if(mAddressPin[i].mNet==NULL||mAddressPin[i].mNet->mState==NETSTATE_INVALID)
                {
                    valid=0;
                }
            }

            if(!valid)
            {
                for(i=0; i<8; i++)
                {
                    mIOPin[i].setState(gConfig.mPropagateInvalidState);
                }
            }
            else
            {
                // 4. do the thing

                int address=0;

                for(i=0; i<range; i++)
                {
                    address|=(mAddressPin[i].mNet->mState!=NETSTATE_LOW)?(1<<i):0;
                }


                mIOPin[i].setState(PINSTATE_HIGHZ);

                if(mCtrlPin[OE].mNet->mState==NETSTATE_LOW)
                {
                    int value=mMemory[address];

                    for(i=0; i<8; i++)
                    {
                        mIOPin[i].setState((value&(1<<i))?PINSTATE_WRITE_HIGH:PINSTATE_WRITE_LOW);
                    }
                }
                else if(mCtrlPin[WE].mNet->mState==NETSTATE_LOW)
                {
                    int value=0;
                    for(i=0; i<8; i++)
                    {
                        mIOPin[i].setState(PINSTATE_READ);
                        if(mIOPin[i].mNet->mState==NETSTATE_HIGH)
                        {
                            value|=0x01<<i;
                        }
                        else if(mIOPin[i].mNet->mState==NETSTATE_INVALID)
                        {
                        }
                    }
                    mMemory[address]=value;
                }
            }
        }
    }
}    


void Chip6116::serialize(File *f)
{
    int l;
    l = mMemSize - 1;
    while (l > 0 && mMemory[l] == 0) l--;
    l++;
    f->writeint(l);
    int i;
    for (i = 0; i < l; i++)
        f->writebyte(mMemory[i]);
}

void Chip6116::deserialize(File *f)
{
    memset(mMemory,0,mMemSize);
    int l = f->readint();
    int i;
    for (i = 0; i < l; i++)
        mMemory[i] = f->readbyte();
}

void Chip6116::clone(Chip *aOther)
{
    Chip6116 *d = (Chip6116 *)aOther;
    // Copy of the memory should be enough..
    memcpy(d->mMemory, mMemory, mMemSize);
}
