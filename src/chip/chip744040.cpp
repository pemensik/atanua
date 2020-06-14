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
#include "chip744040.h"

Chip744040::Chip744040()
{
	set(0,0,4.54,2.25,"Asynchronous 12-bit\nBinary Counter");
	float xpos = 0.15 + 0.54;

#define DEF_PIN(x,y,d) mPin.push_back(&x); x.set(xpos, y, this, d); xpos += 0.54;

	DEF_PIN(mOutputPin[10], 0, "Pin 15:QK");
	DEF_PIN(mOutputPin[9],  0, "Pin 14:QJ");
	DEF_PIN(mOutputPin[7],  0, "Pin 13:QH");
	DEF_PIN(mOutputPin[8],  0, "Pin 12:QI");
	DEF_PIN(mClrPin,        0, "Pin 11:CLR");
	DEF_PIN(mClkPin,        0, "Pin 10:!CLK");   
	DEF_PIN(mOutputPin[0],  0, "Pin 9:QA");
	xpos = 0.15;
	DEF_PIN(mOutputPin[11], 1.75, "Pin 1:QL");
	DEF_PIN(mOutputPin[5],  1.75, "Pin 2:QF");
	DEF_PIN(mOutputPin[4],  1.75, "Pin 3:QE");
	DEF_PIN(mOutputPin[6],  1.75, "Pin 4:QG");
	DEF_PIN(mOutputPin[3],  1.75, "Pin 5:QD");
	DEF_PIN(mOutputPin[2],  1.75, "Pin 6:QC");
	DEF_PIN(mOutputPin[1],  1.75, "Pin 7:QB");

	mTexture = load_file_texture("data/chip_16pin.png");

	mOldClk = 0;
	mValue = 0;
}

void Chip744040::render(int aChipId)
{
	drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
	fn.drawstring("74LS4040",mX+0.7,mY+0.7,0x5fffffff,0.75);
}

void Chip744040::update(float aTick) 
{
#define CHECKVALID(pin) (pin.mNet == NULL || pin.mNet->mState == NETSTATE_INVALID)
	int i;
	if (CHECKVALID(mClrPin) ||
		CHECKVALID(mClkPin) ||
		(CHECKVALID(mOutputPin[0]) &&
		CHECKVALID(mOutputPin[1]) &&
		CHECKVALID(mOutputPin[2]) &&
		CHECKVALID(mOutputPin[3]) &&
		CHECKVALID(mOutputPin[4]) &&
		CHECKVALID(mOutputPin[5]) &&
		CHECKVALID(mOutputPin[6]) &&
		CHECKVALID(mOutputPin[7]) &&
		CHECKVALID(mOutputPin[8]) &&
		CHECKVALID(mOutputPin[9]) &&
		CHECKVALID(mOutputPin[10]) &&
		CHECKVALID(mOutputPin[11])))
	{
		for (i = 0; i < 12; i++)
			mOutputPin[i].setState(gConfig.mPropagateInvalidState);
		return;
	}

	if (mClkPin.mNet->mState != mOldClk)
	{
		if(mClkPin.mNet->mState==NETSTATE_LOW)
		{
			mValue++;
		}
		mOldClk=mClkPin.mNet->mState;
	}

	if(mClrPin.mNet->mState==NETSTATE_HIGH)
	{
		mValue=0;
	}

	for (i = 0; i < 12; i++)
	{
		if (mValue & (1 << i))
		{
			mOutputPin[i].setState(PINSTATE_WRITE_HIGH);
		}
		else
		{
			mOutputPin[i].setState(PINSTATE_WRITE_LOW);
		}
	}
}    
