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
#include "composite.h"
#include "slidingaverage.h"

Composite::Composite(CType aSize)
{
 	mSize = aSize;
	mPins=4;

//	numberwang=0;


	if(mSize==i480)
	{
		mxSize=640;
		mySize=525;
	}
	else
	{
		mxSize=768;
		mySize=625;
	}

	set(0, 0, mxSize/16, mySize/16, NULL);

	mInputPin = new Pin[mPins];

	int i;
	for(i=0; i<mPins; i++)
	{
		mPin.push_back(&mInputPin[i]);
	}

	mPinDescription = new char*[mPins];

	float ypos=0.2;

	mInputPin[0].set(0,ypos,this,"Composite");
	mInputPin[1].set(0,2+ypos,this,"VSync");
	mInputPin[2].set(0,4+ypos,this,"HSync");
	mInputPin[3].set(0,6+ypos,this,"Clk");

	mBaseTexture=create_blank_texture("composite_base",1,mxSize,mySize);
	
	for(i=0; i<mPins; i++)
	{
		mInputPin[i].mReadOnly=1;
	}
	mScanX=0;
	mScanY=0;
	mInVSync=false;
	mInHSync=false;

	mLastClkState=NETSTATE_INVALID;

	mMaxX=100;
	mMaxY=100;


}

Composite::~Composite()
{ 
	delete[] mInputPin;	
	int i;


	if(mInputPin)
	{
		delete[] mInputPin;
		mInputPin=0;
	}

	for(i=0; i<mPins; i++)
	{
		delete[] mPinDescription[i];
	}
	mPins=0;

	if(mPinDescription)
	{
		delete[] mPinDescription;
		mPinDescription=0;
	}
}

void Composite::render(int aChipId)
{
	unsigned long* RawBitMap;

	if(FindTextBitmap("composite_base",(unsigned char **)&RawBitMap))
	{
		glBindTexture(GL_TEXTURE_2D,mBaseTexture);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1,1,1,1);
		glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0,0);
		glVertex2f(mX+0.25,mY);
//		glTexCoord2f(0,1);
		glTexCoord2f(0,(float)mMaxY/(float)mySize);
		glVertex2f(mX+0.25,mY+mH);
//		glTexCoord2f(1,0);
		glTexCoord2f((float)mMaxX/(float)mxSize,0);
		glVertex2f(mX+0.25+mW-0.5,mY);
//		glTexCoord2f(1,1);
		glTexCoord2f((float)mMaxX/(float)mxSize,(float)mMaxY/(float)mySize);
		glVertex2f(mX+0.25+mW-0.5,mY+mH);
		glEnd();
		glDisable(GL_TEXTURE_2D);


//		glBlendFunc(GL_ONE,GL_ONE);
//		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
}   

//if((mInputPinA.mNet->mState==NETSTATE_HIGH||mInputPinA.mNet->mState==NETSTATE_NC)&&
//	(mInputPinB.mNet->mState==NETSTATE_HIGH||mInputPinB.mNet->mState==NETSTATE_NC))

void Composite::update(float aTick)
{
	//    int x, y, c;
	unsigned long* RawBitMap,pVal;
	netstates	ClkState;


	if(mInputPin[0].mNet&&mInputPin[1].mNet&&mInputPin[2].mNet&&mInputPin[3].mNet)
	{
		ClkState=mInputPin[3].mNet->mState;

		if(ClkState!=mLastClkState)
		{
			mLastClkState=ClkState;

			if(ClkState==NETSTATE_LOW)
			{

				netstates HPin=mInputPin[2].mNet->mState;
				netstates VPin=mInputPin[1].mNet->mState;
				netstates DPin=mInputPin[0].mNet->mState;

				if(
					HPin==NETSTATE_NC||HPin==NETSTATE_INVALID||
					VPin==NETSTATE_NC||VPin==NETSTATE_INVALID||
					DPin==NETSTATE_NC||DPin==NETSTATE_INVALID)
				{
				}
				else
				{
					if(HPin==NETSTATE_HIGH&&!mInHSync)
					{
						mInHSync=true;
						mScanX=0;
						mScanY++;
						if(mScanY>mMaxY)
						{
							mMaxY=mScanY;
						}
					}
					else if(HPin==NETSTATE_HIGH&&mInHSync)
					{
//						mScanX=0;
						//				mScanX++;
					}
					else if(HPin==NETSTATE_LOW&&mInHSync)
					{
						mInHSync=false;
					}
					else if(VPin==NETSTATE_LOW&&mInVSync)
					{
						mInVSync=false;
					}
					else
					{
						//				mScanX++;
					}

					if(VPin==NETSTATE_HIGH&&!mInVSync)
					{
						mScanX=0;
						mScanY=0;
						mInVSync=true;
					}
					else if(VPin==NETSTATE_LOW&&mInVSync)
					{
						mInVSync=false;
						mScanX=0;
					}

					if(!mInVSync&&!mInHSync)
					{
						if(DPin==NETSTATE_HIGH)
						{
							pVal=0xFFFFFF;
						}
						else
						{
							pVal=0x000000;
						}
					}
					else
					{
						pVal=0x000000;
					}


					if(FindTextBitmap("composite_base",(unsigned char**)&RawBitMap))
					{
						/*
								for(y=0,c=0; y<mySize; y++)
								{
									for(x=0; x<mxSize; x++)
									{
										RawBitMap[y*mxSize+x]=0xFF000000;

										if((y/10)%2)
										{
											RawBitMap[y*mxSize+x]|=0x00FF0000;
										}
										else
										{
											if((x/10)%2)
											{
												RawBitMap[y*mxSize+x]|=0x000000FF;
											}
											else
											{
												RawBitMap[y*mxSize+x]|=0x000000FF|(((numberwang)<<8)&0xFF00);
											}
										}
									}
								}
								*/

								//				RawBitMap[mScanY*mxSize+mScanX]=0xFF000000|pVal;
						if(mInVSync)
						{
							RawBitMap[mScanY*mxSize+mScanX]=0xFF0000FF;
						}
						else if(mInHSync)
						{
							RawBitMap[mScanY*mxSize+mScanX]=0xFF00FF00;
						}
						else
						{
							if(DPin==NETSTATE_HIGH)
							{
								RawBitMap[mScanY*mxSize+mScanX]=0xFFFFFFFF;
							}
							else
							{
								RawBitMap[mScanY*mxSize+mScanX]=0xFF000000;
							}
						}
					}

					mDirty=1;

					reload_textures();

					mScanX++;
					if(mScanX>mMaxX)
					{
						mMaxX=mScanX;
					}
				}
			}
		}
	}
}    
