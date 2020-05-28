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
#include "namedpin.h"
#include "box.h"

list<NamedPin*> NamedPin::mNamedPins;


NamedPin::~NamedPin()
{
	mNamedPins.remove(this);
}

NamedPin::NamedPin(float aSize) : Label(aSize)
{
	mPin.push_back(&mInputPin);
	mInputPin.set(0.25,(mH-0.5)/2,this,"Named pin");

	// hack: if size is negative, this is not related to the
	// box we're loading.
	if(aSize>=0)
	{
		if(gBoxBeingLoaded!=NULL)
		{
			gBoxBeingLoaded->getBoxStitchingInformation()->mNamedPinsInside.push_back(this);
		}
	}

	mInputPin.mReadOnly=1;
	mNamedPins.push_back(this);
}

void NamedPin::render(int aChipId)
{
	drawrect(mInputPin.mX+mX-0.1,mInputPin.mY+mY-0.1,0.7,0.7,0xffff0000);
	mX+=0.75;
	Label::render(aChipId);
	mX-=0.75;

}


void NamedPin::tool()
{
	if(mData!=mPinName)
	{
		for(list<NamedPin*>::iterator it=mNamedPins.begin(); it!=mNamedPins.end(); ++it)
		{
			if(*it!=this)
			{
				if((*it)->mPinName==mPinName)
				{
					delete_wire(&mInputPin,&(*it)->mInputPin,LineType::NAMEDPINWIRE,false);
				}
			}
		}
		mPinName=mData;
		for(list<NamedPin*>::iterator it=mNamedPins.begin(); it!=mNamedPins.end(); ++it)
		{
			if(*it!=this)
			{
				if((*it)->mPinName==mPinName)
				{
					add_wire(&mInputPin,&(*it)->mInputPin,LineType::NAMEDPINWIRE,false);
				}
			}
		}
	}
}

void NamedPin::Magic()
{
	for(list<NamedPin*>::iterator it=mNamedPins.begin(); it!=mNamedPins.end(); ++it)
	{
		(*it)->tool();
	}
}

