
#include <math.h>
#include "toolkit.h"

#include <math.h>
#include "toolkit.h"


typedef enum
{
	file,
	dynamic
} TexType;

struct texpair
{
	TexType ttype;

	const char* mFilename;
	unsigned char* mRawBitMap;
	GLuint mFramebufferName;


	GLuint mHandle;
	int mClamp;
	unsigned int* mip;

	int mxSize;        // Grid size (n x n)
	int mySize;        // Grid size (n x n)
};

texpair* gTextureStore=NULL;
int gTextureStoreSize=0;

static void load_blank_texture(texpair& Tex);

//static void do_loadtexture(const char * aFilename, int clamp = 1)
static void do_loadtexture(texpair& Tex)
{
	int i,j;

	// Load texture using stb
	int n;

	if(!Tex.mRawBitMap)
	{
		Tex.mRawBitMap=stbi_load(Tex.mFilename,&Tex.mxSize,&Tex.mySize,&n,4);

		if(Tex.mRawBitMap==NULL)
			return;

		unsigned int* src=(unsigned int*)Tex.mRawBitMap;

		// mark all pixels with alpha = 0 to black
		for(i=0; i<Tex.mySize; i++)
		{
			for(j=0; j<Tex.mxSize; j++)
			{
				if((src[i*Tex.mxSize+j]&0xff000000)==0)
					src[i*Tex.mxSize+j]=0;
			}
		}
	}

	// Tell OpenGL to read the texture
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,Tex.mxSize,Tex.mySize,0,GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid*)Tex.mRawBitMap);
#if do_mip_stuff
	if(!Tex.mip)
	{
		Tex.mip=new unsigned int[Tex.mxSize*Tex.mySize*5];
		memset(Tex.mip,0,Tex.mxSize*Tex.mySize*4);

		// precalculate summed area tables
		// it's a box filter, which isn't very good, but at least it's fast =)
		int ra=0,ga=0,ba=0,aa=0;
		int i,j,c;
		unsigned int* rbuf=Tex.mip+(Tex.mxSize*Tex.mySize*1);
		unsigned int* gbuf=Tex.mip+(Tex.mxSize*Tex.mySize*2);
		unsigned int* bbuf=Tex.mip+(Tex.mxSize*Tex.mySize*3);
		unsigned int* abuf=Tex.mip+(Tex.mxSize*Tex.mySize*4);

		unsigned long* src=(unsigned long*)Tex.RawBitMap;

		for(j=0,c=0; j<Tex.mySize; j++)
		{
			ra=ga=ba=aa=0;
			for(i=0; i<Tex.mxSize; i++,c++)
			{
				ra+=(src[c]>>0)&0xff;
				ga+=(src[c]>>8)&0xff;
				ba+=(src[c]>>16)&0xff;
				aa+=(src[c]>>24)&0xff;
				if(j==0)
				{
					rbuf[c]=ra;
					gbuf[c]=ga;
					bbuf[c]=ba;
					abuf[c]=aa;
				}
				else
				{
					rbuf[c]=ra+rbuf[c-Tex.mxSize];
					gbuf[c]=ga+gbuf[c-Tex.mxSize];
					bbuf[c]=ba+bbuf[c-Tex.mxSize];
					abuf[c]=aa+abuf[c-Tex.mxSize];
				}
			}
		}
		int w=Tex.mxSize;
		int h=Tex.mySize;

		int l;
		l=0;

		while(w>1||h>1)
		{
			l++;
			w/=2;
			h/=2;
			if(w==0) w=1;
			if(h==0) h=1;

			int dw=Tex.mxSize/w;
			int dh=Tex.mxSize/h;

			for(j=0,c=0; j<h; j++)
			{
				for(i=0; i<w; i++,c++)
				{
					int x1=i*dw;
					int y1=j*dh;
					int x2=x1+dw-1;
					int y2=y1+dh-1;
					int div=(x2-x1)*(y2-y1);
					y1*=Tex.mxSize;
					y2*=Tex.mxSize;
					int r=rbuf[y2+x2]-rbuf[y1+x2]-rbuf[y2+x1]+rbuf[y1+x1];
					int g=gbuf[y2+x2]-gbuf[y1+x2]-gbuf[y2+x1]+gbuf[y1+x1];
					int b=bbuf[y2+x2]-bbuf[y1+x2]-bbuf[y2+x1]+bbuf[y1+x1];
					int a=abuf[y2+x2]-abuf[y1+x2]-abuf[y2+x1]+abuf[y1+x1];

					r/=div;
					g/=div;
					b/=div;
					a/=div;

					if(a==0)
						Tex.mip[c]=0;
					else
						Tex.mip[c]=((r&0xff)<<0)|
						((g&0xff)<<8)|
						((b&0xff)<<16)|
						((a&0xff)<<24);
				}
			}
		}
	}

	if(Tex.mip)
	{
		glTexImage2D(GL_TEXTURE_2D,l,GL_RGBA,Tex.mxSize,Tex.mySize,0,GL_RGBA,GL_UNSIGNED_BYTE,(GLvoid*)Tex.mip);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); // Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	}
	else
#endif
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
	}

	// and cleanup.
//	stbi_image_free(data);

	if(Tex.mClamp)
	{
		// Set up texture parameters
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	}
	else
	{
		// Set up texture parameters
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}
}


GLuint load_file_texture(const char* aFilename,int clamp)
{
	// First check if we have loaded this texture already
	int i;
	for(i=0; i<gTextureStoreSize; i++)
	{
		if(stricmp(gTextureStore[i].mFilename,aFilename)==0)
			return gTextureStore[i].mHandle;
	}

	// Create OpenGL texture handle and bind it to use

	GLuint texname;
	glGenTextures(1,&texname);
	glBindTexture(GL_TEXTURE_2D,texname);

	//    do_loadtexture(aFilename, clamp);

	gTextureStoreSize++;

	texpair* t=(texpair*)realloc(gTextureStore,sizeof(texpair)*gTextureStoreSize);
	if(t!=NULL)
	{
		gTextureStore=t;

		texpair* Tex=&gTextureStore[gTextureStoreSize-1];

		memset(Tex,0,sizeof(*Tex));

		Tex->ttype=file;

		Tex->mFilename=mystrdup(aFilename);
		Tex->mHandle=texname;
		Tex->mClamp=clamp;
		Tex->mRawBitMap=0;
		Tex->mFramebufferName=-1;

		//        do_loadtexture(aFilename,clamp);
		do_loadtexture(*Tex);
	}

	return texname;
}



//static void load_blank_texture(const char* aFilename,int Clamp,int w,int h,unsigned long* src)
static void load_blank_texture(texpair& Tex)
{

	GLErrorTest();

	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,Tex.mxSize,Tex.mySize,0,GL_RGBA,GL_UNSIGNED_BYTE,Tex.mRawBitMap);

	GLErrorTest();

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering

	GLErrorTest();
}



GLuint load_blank_texture(const char* aFilename,int Clamp,int w,int h)
{
	// First check if we have loaded this texture already
	int i;
	unsigned char* data;

	for(i=0; i<gTextureStoreSize; i++)
	{
		if(stricmp(gTextureStore[i].mFilename,aFilename)==0)
			return gTextureStore[i].mHandle;
	}

	// Create OpenGL texture handle and bind it to use
	gTextureStore[i].mxSize=w;
	gTextureStore[i].mySize=h;

	GLuint FramebufferName=0;

	glGenFramebuffers(1,&FramebufferName);
	GLErrorTest();
	glBindFramebuffer(GL_FRAMEBUFFER,FramebufferName);
	GLErrorTest();

	GLuint texname;
	glGenTextures(1,&texname);
	glBindTexture(GL_TEXTURE_2D,texname);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	unsigned long* src=new unsigned long[w*h];
	data=(unsigned char*)src;

	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			src[y*w+x]=0xFFFFFFFF;
		}
	}


	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_UNSIGNED_BYTE,data);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,texname,0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"glCheckFramebufferStatus","glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE",NULL);
		return false;
	}

	// unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER,0); //Put back to screen
	GLErrorTest();

	gTextureStoreSize++;

	texpair* t=(texpair*)realloc(gTextureStore,sizeof(texpair)*gTextureStoreSize);
	if(t!=NULL)
	{
		gTextureStore=t;

		texpair* Tex=&gTextureStore[gTextureStoreSize-1];

		Tex->ttype=dynamic;

		Tex->mFilename=mystrdup(aFilename);
		Tex->mHandle=texname;
		Tex->mFramebufferName=FramebufferName;
		Tex->mClamp=Clamp;
		Tex->mRawBitMap=(unsigned char*)src;
		Tex->mxSize=w;
		Tex->mySize=h;

		load_blank_texture(*Tex);
	}

	return texname;
}


// First check if we have loaded this texture already
GLuint FindTextBitmap(const char* aFilename,unsigned char** RawBitMap,GLuint& mFramebufferName)
{
	int i;
	for(i=0; i<gTextureStoreSize; i++)
	{
		if(stricmp(gTextureStore[i].mFilename,aFilename)==0)
		{
			*RawBitMap=gTextureStore[i].mRawBitMap;
			mFramebufferName=gTextureStore[i].mFramebufferName;

			return gTextureStore[i].mHandle;
		}
	}
	return 0;
}


GLuint EmptyTextureStore()
{
	int i;
	for(i=0; i<gTextureStoreSize; i++)
	{
		if(gTextureStore[i].mip)
		{
			delete(gTextureStore[i].mip);
		}
		if(gTextureStore[i].mRawBitMap)
		{
			delete(gTextureStore[i].mRawBitMap);
		}
		delete gTextureStore;
	}
	return 0;
}

void reload_textures()
{
	// bind the textures to the same texture names as the last time.
	int i;
	for(i=0; i<gTextureStoreSize; i++)
	{
		GLErrorTest();

		if(gTextureStore[i].ttype==file)
		{
			glBindTexture(GL_TEXTURE_2D,gTextureStore[i].mHandle);
			GLErrorTest();
			do_loadtexture(gTextureStore[i]);
			GLErrorTest();

		}
		else if(gTextureStore[i].mFramebufferName>0)
		{
			load_blank_texture(gTextureStore[i]);
		}
		else
		{
		}
	}
}


