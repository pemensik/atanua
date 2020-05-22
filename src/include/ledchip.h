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
in a product, an acknowledgment in the product docu

GLuint load_blank_texture(const char* aFilename,int Clamp,int w,int h,unsigned long* src)
{

    GLenum a=glGetError();

    if(a!=GL_NO_ERROR)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"A","B",NULL);
    }

    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,src);

    a=glGetError();

    if(a!=GL_NO_ERROR)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"A","C",NULL);
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
    //    do_loadtexture(aFilename,clamp);


    return texname;
}
mentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/
#ifndef LEDCHIP_H
#define LEDCHIP_H

class SlidingAverage;

class LEDChip : public Chip
{
    int mColor;     // Color for the LED. 24bit value
    Pin mInputPin;  // Chip's input pin
    int mBaseTexture;
    int mFlareTexture;
    int mInverse;
	SlidingAverage *mAvg;

public:
    LEDChip(int aColor, int aInverse);
	~LEDChip();

    virtual void render(int aChipId);
    virtual void update(float aTick);
	virtual int isUselessInBox() { return 1; }
};

#endif
