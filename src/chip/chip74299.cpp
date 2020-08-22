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

//Mods by JB

#include "atanua.h"
#include "chip74299.h"

Chip74299::Chip74299()
{
	set(0,0,5.7,2.8,"8-Bit Universal Shift Register; Three-State");
	float xpos=0.15+0.54;
	mPin.push_back(&mS[1]);
	mS[1].set(xpos,0.15,this,"Pin 19:S1"); xpos+=0.54;
	mPin.push_back(&mDSL);
	mDSL.set(xpos,0.15,this,"Pin 18:DSL"); xpos+=0.54;
	mPin.push_back(&mQ7);
	mQ7.set(xpos,0.15,this,"Pin 17:Q7"); xpos+=0.54;
	mPin.push_back(&mIO[7]);
	mIO[7].set(xpos,0.15,this,"Pin 16:IO7"); xpos+=0.54;
	mPin.push_back(&mIO[5]);
	mIO[5].set(xpos,0.15,this,"Pin 15:IO5"); xpos+=0.54;
	mPin.push_back(&mIO[3]);
	mIO[3].set(xpos,0.15,this,"Pin 14:IO3"); xpos+=0.54;
	mPin.push_back(&mIO[1]);
	mIO[1].set(xpos,0.15,this,"Pin 13:IO1"); xpos+=0.54;
	mPin.push_back(&mCP);
	mCP.set(xpos,0.15,this,"Pin 12:CP"); xpos+=0.54;
	mPin.push_back(&mDSR);
	mDSR.set(xpos,0.15,this,"Pin 11:DSR"); xpos+=0.54;

	xpos=0.15;
	mPin.push_back(&mS[0]);
	mS[0].set(xpos,2.1,this,"Pin 1:S0"); xpos+=0.54;
	mPin.push_back(&mOE[0]);
	mOE[0].set(xpos,2.1,this,"Pin 2:!OE1"); xpos+=0.54;
	mPin.push_back(&mOE[1]);
	mOE[1].set(xpos,2.1,this,"Pin 3:!OE2"); xpos+=0.54;
	mPin.push_back(&mIO[6]);
	mIO[6].set(xpos,2.1,this,"Pin 4:IO6"); xpos+=0.54;
	mPin.push_back(&mIO[4]);
	mIO[4].set(xpos,2.1,this,"Pin 5:IO4"); xpos+=0.54;
	mPin.push_back(&mIO[2]);
	mIO[2].set(xpos,2.1,this,"Pin 6:IO2"); xpos+=0.54;
	mPin.push_back(&mIO[0]);
	mIO[0].set(xpos,2.1,this,"Pin 7:IO0"); xpos+=0.54;
	mPin.push_back(&mQ0);
	mQ0.set(xpos,2.1,this,"Pin 8:Q0"); xpos+=0.54;
	mPin.push_back(&mMR);
	mMR.set(xpos,2.1,this,"Pin 9:!MR"); xpos+=0.54;

	mTexture=load_file_texture("data/chip_20pin.png");
	mReg=0;
}



void Chip74299::render(int aChipId)
{
	drawtexturedrect(mTexture,mX,mY,mW,mH,0xffffffff);
	fn.drawstring("74LS299",mX+0.6,mY+1,0x5fffffff,0.75);
}

void Chip74299::update(float aTick)
{
	int	i;
	bool	S0=false ,S1=false;

	if(mS[0].getnetState()==NETSTATE_HIGH)
	{
		S0=true;
	}

	if(mS[1].getnetState()==NETSTATE_HIGH)
	{
		S1=true;
	}

	if(mMR.mNet==NULL||
		mCP.mNet==NULL||
		mOE[0].mNet==NULL||
		mOE[1].mNet==NULL||
		mS[0].mNet==NULL||
		mS[1].mNet==NULL||
		mDSR.mNet==NULL||
		mDSL.mNet->mState==NETSTATE_INVALID)
	{
		for(i=0; i<8; i++)
		{
			mIO[i].setState(gConfig.mPropagateInvalidState);
		}
		mQ0.setState(gConfig.mPropagateInvalidState);
		mQ7.setState(gConfig.mPropagateInvalidState);
	}
	else if(mMR.mNet->mState==NETSTATE_LOW)
	{
		for(i=0; i<8; i++)
		{
			mIO[i].setState(PINSTATE_WRITE_LOW);
		}
		mReg=0;
		mQ0.setState(PINSTATE_WRITE_LOW);
		mQ7.setState(PINSTATE_WRITE_LOW);
	}
	else if(mCP.mNet->mState!=mOldClock)
	{
		if(mCP.mNet->mState==NETSTATE_HIGH)
		{
			if(!S0&&!S1)
			{
			}
			else if(!S0&&S1)
			{ //Shift Left
				mReg>>=1;
				if(mDSL.getnetState()==NETSTATE_HIGH)
				{
					mReg|=0x80;
				}
			}
			else if(S0&&!S1)
			{ // Shift Right
				mReg<<=1;
				if(mDSR.getnetState()==NETSTATE_HIGH)
				{
					mReg|=0x01;
				}
			}
			else
			{
				mReg=0;
				for(i=0; i<8; i++)
				{
					mIO[i].setState(PINSTATE_READ);
					if(mIO[i].getnetState()==NETSTATE_HIGH)
					{
						mReg|=(0x01<<i);
					}
				}
			}
		}
		mOldClock=mCP.mNet->mState;
	}

	if(mOE[0].getnetState()==NETSTATE_LOW && mOE[1].getnetState()==NETSTATE_LOW)
	{
		for(i=0; i<8; i++)
		{
			mIO[i].setState((mReg&(0x01<<i))?PINSTATE_READ_OR_WRITE_HIGH:PINSTATE_READ_OR_WRITE_LOW);
		}
		mQ0.setState(mIO[0].getState());
		mQ7.setState(mIO[7].getState());

	}
	else
	{
		for(i=0; i<8; i++)
		{
			mIO[i].setState(PINSTATE_HIGHZ);
		}
		mQ0.setState(PINSTATE_HIGHZ);
		mQ7.setState(PINSTATE_HIGHZ);

	}
}
