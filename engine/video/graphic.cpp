//     ____                _       __
//    / __ )____  _____   | |     / /___ ___________
//   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
//  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
// /_____/\____/____/     |__/|__/\__,_/_/  /____/
//
//       A futuristic real-time strategy game.
//          This file is part of Bos Wars.
//
/**@name graphic.cpp - The general graphic functions. */
//
//      (c) Copyright 1999-2023 by Lutz Sammer, Nehal Mistry, Jimmy Salmon
//             and François Beerten.
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; only version 2 of the License.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//      02111-1307, USA.
//

//@{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stratagus.h"
#include <string>
#include <map>
#include <list>
#include "video.h"
#include "player.h"
#include "intern_video.h"
#include "iocompat.h"
#include "iolib.h"


static enum {
	ModuleNotInitialized,
	ModuleOK,
	ModuleDeinitialized
} GraphicsState;

static int HashCount;

static std::map<std::string, CGraphic *> GraphicHash;

static std::list<CGraphic *> Graphics;

struct CGraphicsModule {
	CGraphicsModule() {
		GraphicsState = ModuleOK;
	}
	~CGraphicsModule() {
		GraphicsState = ModuleDeinitialized;
	}
	static void AssertOK() {
#ifdef DEBUG
		switch (GraphicsState) {
		case ModuleNotInitialized:
			fprintf(stderr, "The graphics module hasn't yet been initialized\n");
			// Do not call ExitFatal, because that calls exit,
			// which might attempt to call destructors again.
			abort();
		case ModuleOK:
			break;
		case ModuleDeinitialized:
			fprintf(stderr, "The graphics module has already been deinitialized\n");
			abort();
		default:
			abort();
		}
#endif	// DEBUG
	}
};

static CGraphicsModule GraphicsModule;


/**
**  Video draw the graphic clipped.
**
**  @param x   X screen position
**  @param y   Y screen position
*/
void CGraphic::DrawClip(int x, int y) const
{
	int oldx = x;
	int oldy = y;
	int w = Width;
	int h = Height;
	CLIP_RECTANGLE(x, y, w, h);
	DrawSub(x - oldx, y - oldy, w, h, x, y);
}

/**
**  Video draw part of graphic.
**
**  @param gx  X offset into object
**  @param gy  Y offset into object
**  @param w   width to display
**  @param h   height to display
**  @param x   X screen position
**  @param y   Y screen position
*/
void CGraphic::DrawSub(int gx, int gy, int w, int h, int x, int y) const
{
	SDL_Rect srect = {
		static_cast<Sint16>(gx),
		static_cast<Sint16>(gy),
		static_cast<Uint16>(w),
		static_cast<Uint16>(h)
	};
	SDL_Rect drect = {
		static_cast<Sint16>(x),
		static_cast<Sint16>(y),
		static_cast<Uint16>(w),
		static_cast<Uint16>(h)
	};

	SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	if (texture)
		SDL_RenderCopy(TheRenderer, texture, &srect, &drect);
}

/**
**  Video draw part of graphic clipped.
**
**  @param gx  X offset into object
**  @param gy  Y offset into object
**  @param w   width to display
**  @param h   height to display
**  @param x   X screen position
**  @param y   Y screen position
*/
void CGraphic::DrawSubClip(int gx, int gy, int w, int h, int x, int y) const
{
	int oldx = x;
	int oldy = y;
	CLIP_RECTANGLE(x, y, w, h);
	DrawSub(gx + x - oldx, gy + y - oldy, w, h, x, y);
}

/**
**  Video draw part of graphic with alpha.
**
**  @param gx     X offset into object
**  @param gy     Y offset into object
**  @param w      width to display
**  @param h      height to display
**  @param x      X screen position
**  @param y      Y screen position
**  @param alpha  Alpha
*/
void CGraphic::DrawSubTrans(int gx, int gy, int w, int h, int x, int y,
	unsigned char alpha) const
{
	Uint8 oldalpha = 255;
	SDL_Rect srect = {
		static_cast<Sint16>(gx),
		static_cast<Sint16>(gy),
		static_cast<Uint16>(w),
		static_cast<Uint16>(h)
	};
	SDL_Rect drect = {
		static_cast<Sint16>(x),
		static_cast<Sint16>(y),
		static_cast<Uint16>(w),
		static_cast<Uint16>(h)
	};

	SDL_GetSurfaceAlphaMod(Surface, &oldalpha);
	SDL_SetSurfaceAlphaMod(Surface, alpha);
	SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	SDL_SetSurfaceAlphaMod(Surface, oldalpha);
	if (texture) {
		SDL_SetTextureAlphaMod(texture, alpha);
		SDL_RenderCopy(TheRenderer, texture, &srect, &drect);
	}
}

/**
**  Video draw part of graphic with alpha and clipped.
**
**  @param gx     X offset into object
**  @param gy     Y offset into object
**  @param w      width to display
**  @param h      height to display
**  @param x      X screen position
**  @param y      Y screen position
**  @param alpha  Alpha
*/
void CGraphic::DrawSubClipTrans(int gx, int gy, int w, int h, int x, int y,
	unsigned char alpha) const
{
	int oldx = x;
	int oldy = y;
	CLIP_RECTANGLE(x, y, w, h);
	DrawSubTrans(gx + x - oldx, gy + y - oldy, w, h, x, y, alpha);
}

/**
**  Draw graphic object unclipped.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CGraphic::DrawFrame(unsigned frame, int x, int y) const
{
	DrawSub((frame % (Surface->w / Width)) * Width,
		(frame / (Surface->w / Width)) * Height,
		Width, Height, x, y);
}

/**
**  Draw graphic object clipped.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CGraphic::DrawFrameClip(unsigned frame, int x, int y) const
{
	DrawSubClip((frame % (Surface->w / Width)) * Width,
		(frame / (Surface->w / Width)) * Height,
		Width, Height, x, y);
}

void CGraphic::DrawFrameTrans(unsigned frame, int x, int y, int alpha) const
{
	DrawSubTrans((frame % (Surface->w / Width)) * Width,
		(frame / (Surface->w / Width)) * Height,
		Width, Height, x, y, alpha);
}

void CGraphic::DrawFrameClipTrans(unsigned frame, int x, int y, int alpha) const
{
	DrawSubClipTrans((frame % (Surface->w / Width)) * Width,
		(frame / (Surface->w / Width)) * Height,
		Width, Height, x, y, alpha);
}

/**
**  Draw graphic object clipped and with player colors.
**
**  @param player  player number
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CPlayerColorGraphic::DrawPlayerColorFrameClip(int player, unsigned frame,
	int x, int y)
{
	GraphicPlayerPixels(&Players[player], this);
	DrawFrameClip(frame, x, y);
}



/**
**  Draw graphic object unclipped and flipped in X direction.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CGraphic::DrawFrameX(unsigned frame, int x, int y) const
{
	SDL_Rect srect;
	SDL_Rect drect;

	srect.x = (Surface->w - (frame % (Surface->w /
		Width)) * Width) - Width;
	srect.y = (frame / (Surface->w / Width)) * Height;
	srect.w = Width;
	srect.h = Height;

	drect.x = x;
	drect.y = y;
	drect.w = Width;
	drect.h = Height;

	SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	SDL_RenderCopyEx(TheRenderer, texture, &srect, &drect, 0, NULL,
			SDL_FLIP_HORIZONTAL);
}

/**
**  Draw graphic object clipped and flipped in X direction.
**
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CGraphic::DrawFrameClipX(unsigned frame, int x, int y) const
{
	SDL_Rect srect;
	SDL_Rect drect;
	int oldx;
	int oldy;

	srect.x = (Surface->w - (frame % (Surface->w /
			Width)) * Width) - Width;
	srect.y = (frame / (Surface->w / Width)) * Height;
	srect.w = Width;
	srect.h = Height;

	oldx = x;
	oldy = y;
	CLIP_RECTANGLE(x, y, srect.w, srect.h);
	srect.x += x - oldx;
	srect.y += y - oldy;

	drect.x = x;
	drect.y = y;
	drect.w = srect.w;
	drect.h = srect.h;

	SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	SDL_RenderCopyEx(TheRenderer, texture, &srect, &drect, 0, NULL,
			SDL_FLIP_HORIZONTAL);
}

void CGraphic::DrawFrameTransX(unsigned frame, int x, int y, int alpha) const
{
	SDL_Rect srect;
	SDL_Rect drect;
	Uint8 oldalpha;

	srect.x = (Surface->w - (frame % (Surface->w /
		Width)) * Width) - Width;
	srect.y = (frame / (Surface->w / Width)) * Height;
	srect.w = Width;
	srect.h = Height;

	drect.x = x;
	drect.y = y;

	SDL_GetSurfaceAlphaMod(Surface, &oldalpha);
	SDL_SetSurfaceAlphaMod(Surface, alpha);
	SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	SDL_SetSurfaceAlphaMod(Surface, oldalpha);
	SDL_SetTextureAlphaMod(texture, alpha);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopyEx(TheRenderer, texture, &srect, &drect, 0, NULL,
			SDL_FLIP_HORIZONTAL);
}

void CGraphic::DrawFrameClipTransX(unsigned frame, int x, int y, int alpha) const
{
	SDL_Rect srect;
	SDL_Rect drect;
	int oldx;
	int oldy;
	Uint8 oldalpha;

	srect.x = (Surface->w - (frame % (Surface->w /
		Width)) * Width) - Width;
	srect.y = (frame / (Surface->w / Width)) * Height;
	srect.w = Width;
	srect.h = Height;

	oldx = x;
	oldy = y;
	CLIP_RECTANGLE(x, y, srect.w, srect.h);
	srect.x += x - oldx;
	srect.y += y - oldy;

	drect.x = x;
	drect.y = y;

	SDL_GetSurfaceAlphaMod(Surface, &oldalpha);
	SDL_SetSurfaceAlphaMod(Surface, alpha);
	SDL_BlitSurface(Surface, &srect, TheScreen, &drect);
	SDL_SetSurfaceAlphaMod(Surface, oldalpha);
	SDL_SetTextureAlphaMod(texture, alpha);
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_RenderCopyEx(TheRenderer, texture, &srect, &drect, 0, NULL,
			SDL_FLIP_HORIZONTAL);
}

/**
**  Draw graphic object clipped, flipped, and with player colors.
**
**  @param player  player number
**  @param frame   number of frame (object index)
**  @param x       x coordinate on the screen
**  @param y       y coordinate on the screen
*/
void CPlayerColorGraphic::DrawPlayerColorFrameClipX(int player, unsigned frame,
	int x, int y)
{
	GraphicPlayerPixels(&Players[player], this);
	DrawFrameClipX(frame, x, y);
}

/*----------------------------------------------------------------------------
--  Global functions
----------------------------------------------------------------------------*/

/**
**  Make a new graphic object.
**
**  @param file  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CGraphic *CGraphic::New(const std::string &file, int w, int h)
{
	if (file.empty()) {
		return new CGraphic;
	}

	CGraphicsModule::AssertOK();

	CGraphic *g = GraphicHash[file];
	if (!g) {
		g = new CGraphic;
		if (!g) {
			fprintf(stderr, "Out of memory\n");
			ExitFatal(-1);
		}
		// FIXME: use a constructor for this
		g->File = file;
		g->HashFile = g->File;
		g->Width = w;
		g->Height = h;
		GraphicHash[g->HashFile] = g;
	} else {
		++g->Refs;
		Assert((w == 0 || g->Width == w) && (g->Height == h || h == 0));
	}

	return g;
}

/**
**  Make a new player color graphic object.
**
**  @param file  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CPlayerColorGraphic *CPlayerColorGraphic::New(const std::string &file, int w, int h)
{
	if (file.empty()) {
		return new CPlayerColorGraphic;
	}

	CGraphicsModule::AssertOK();

	CPlayerColorGraphic *g = dynamic_cast<CPlayerColorGraphic *>(GraphicHash[file]);
	if (!g) {
		g = new CPlayerColorGraphic;
		if (!g) {
			fprintf(stderr, "Out of memory\n");
			ExitFatal(-1);
		}
		// FIXME: use a constructor for this
		g->File = file;
		g->HashFile = g->File;
		g->Width = w;
		g->Height = h;
		GraphicHash[g->HashFile] = g;
	} else {
		++g->Refs;
		Assert((w == 0 || g->Width == w) && (g->Height == h || h == 0));
	}

	return g;
}

/**
**  Make a new graphic object.  Don't reuse a graphic from the hash table.
**
**  @param file  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CGraphic *CGraphic::ForceNew(const std::string &file, int w, int h)
{
	CGraphicsModule::AssertOK();
	CGraphic *g = new CGraphic;
	if (!g) {
		fprintf(stderr, "Out of memory\n");
		ExitFatal(-1);
	}
	g->File = file;
	int bufSize = file.size() + 32;
	char *hashfile = new char[bufSize];
	sprintf_s(hashfile, bufSize, "%s%d", file.c_str(), HashCount++);
	g->HashFile = hashfile;
	delete[] hashfile;
	g->Width = w;
	g->Height = h;
	GraphicHash[g->HashFile] = g;

	return g;
}

/**
**  Clone a graphic
*/
CGraphic *CGraphic::Clone() const
{
	CGraphic *g = CGraphic::ForceNew(this->File, this->Width, this->Height);

	if (this->IsLoaded()) {
		g->Load();
	}

	return g;
}

/**
**  Make a new player color graphic object.  Don't reuse a graphic from the
**  hash table.
**
**  @param file  Filename
**  @param w     Width of a frame (optional)
**  @param h     Height of a frame (optional)
**
**  @return      New graphic object
*/
CPlayerColorGraphic *CPlayerColorGraphic::ForceNew(const std::string &file, int w, int h)
{
	CGraphicsModule::AssertOK();
	CPlayerColorGraphic *g = new CPlayerColorGraphic;
	if (!g) {
		fprintf(stderr, "Out of memory\n");
		ExitFatal(-1);
	}
	g->File = file;
	size_t bufSize = file.size() + 32;
	char *hashfile = new char[bufSize];
	sprintf_s(hashfile, bufSize, "%s%d", file.c_str(), HashCount++);
	g->HashFile = hashfile;
	delete[] hashfile;
	g->Width = w;
	g->Height = h;
	GraphicHash[g->HashFile] = g;

	return g;
}

/**
**  Load the size of a graphic, but not the actual pixels.
**  Also, unlike Load(), this does not abort the whole process
**  if the file cannot be loaded.
**
**  @param[out] w     width of the graphic, in pixels
**  @param[out] h     height of the graphic, in pixels
**
**  @return true on success, or false on error.
*/
bool CGraphic::LoadGraphicSize(int *w, int *h)
{
	if (GraphicWidth == 0 && GraphicHeight == 0) {
		if (LoadGraphicPNG(this, true) == -1) {
			return false;
		}
	}

	Assert(GraphicWidth != 0 && GraphicHeight != 0);
	*w = GraphicWidth;
	*h = GraphicHeight;
	return true;
}


/**
**  Load a graphic
*/
void CGraphic::Load()
{
	if (Surface) {
		return;
	}

	CGraphicsModule::AssertOK();

	// TODO: More formats?
	if (LoadGraphicPNG(this, false) == -1) {
		fprintf(stderr, "Can't load the graphic `%s'\n", File.c_str());
		ExitFatal(-1);
	}

	if (!Width) {
		Width = GraphicWidth;
	}
	if (!Height) {
		Height = GraphicHeight;
	}

	Assert(Width <= GraphicWidth && Height <= GraphicHeight);

	if ((GraphicWidth / Width) * Width != GraphicWidth ||
			(GraphicHeight / Height) * Height != GraphicHeight) {
		fprintf(stderr, "Invalid graphic (width, height) %s\n", File.c_str());
		fprintf(stderr, "Expected: (%d,%d)  Found: (%d,%d)\n",
			Width, Height, GraphicWidth, GraphicHeight);
		ExitFatal(-1);
	}

	NumFrames = GraphicWidth / Width * GraphicHeight / Height;

	texture = SDL_CreateTextureFromSurface(TheRenderer, Surface);
}

/**
**  Free a SDL surface
**
**  @param surface  SDL surface to free
*/
static void FreeSurface(SDL_Surface **surface)
{
	if (!*surface) {
		return;
	}

	unsigned char *pixels = NULL;

	if ((*surface)->flags & SDL_PREALLOC) {
		pixels = (unsigned char *)(*surface)->pixels;
	}

	SDL_FreeSurface(*surface);
	delete[] pixels;
	*surface = NULL;
}

/**
**  Free a graphic
**
**  @param g  Pointer to the graphic
*/
void CGraphic::Free(CGraphic *g)
{
	if (!g) {
		return;
	}

	Assert(g->Refs);

	--g->Refs;
	if (!g->Refs) {
		SDL_DestroyTexture(g->texture);
		FreeSurface(&g->Surface);

		if (!g->HashFile.empty()) {
			if (GraphicsState == ModuleOK) {
				GraphicHash.erase(g->HashFile);
			}
		}
		delete g;
	}
}

/**
**  Resize a graphic
**
**  @param w  New width of graphic.
**  @param h  New height of graphic.
*/
void CGraphic::Resize(int w, int h)
{
	int i;
	int j;
	unsigned char *data;
	unsigned char *pixels;
	int x;
	int bpp;

	Assert(Surface); // can't resize before it's been loaded

	if (GraphicWidth == w && GraphicHeight == h) {
		return;
	}

	// Resizing the same image multiple times looks horrible
	// If the image has already been resized then get a clean copy first
	if (Resized) {
		FreeSurface(&Surface);

		this->Width = this->Height = 0;
		this->Load();

		Resized = false;
		if (GraphicWidth == w && GraphicHeight == h) {
			return;
		}
	}

	Resized = true;
	Uint32 ckey;
	bool useckey = !SDL_GetColorKey(Surface, &ckey);

	bpp = Surface->format->BytesPerPixel;
	if (bpp == 1) {
		SDL_Color pal[256];

		SDL_LockSurface(Surface);

		pixels = (unsigned char *)Surface->pixels;
		data = new unsigned char[w * h];
		x = 0;

		for (i = 0; i < h; ++i) {
			for (j = 0; j < w; ++j) {
				data[x] = pixels[(i * Height / h) * Surface->pitch + j * Width / w];
				++x;
			}
		}

		SDL_UnlockSurface(Surface);
		memcpy(pal, Surface->format->palette->colors, sizeof(SDL_Color) * 256);
		SDL_FreeSurface(Surface);

		Surface = SDL_CreateRGBSurfaceFrom(data, w, h, 8, w, 0, 0, 0, 0);
		SDL_SetPaletteColors(Surface->format->palette, pal, 0, 256);
		if (useckey) {
			SDL_SetColorKey(Surface, SDL_TRUE, ckey);
		}
	} else {
		int ix, iy;
		float fx, fy, fz;
		unsigned char *p1, *p2, *p3, *p4;

		SDL_LockSurface(Surface);

		pixels = (unsigned char *)Surface->pixels;
		data = new unsigned char[w * h * bpp];
		x = 0;

		for (i = 0; i < h; ++i) {
			fy = (float)i * Height / h;
			iy = (int)fy;
			fy -= iy;
			for (j = 0; j < w; ++j) {
				fx = (float)j * Width / w;
				ix = (int)fx;
				fx -= ix;
				fz = (fx + fy) / 2;

				p1 = &pixels[iy * Surface->pitch + ix * bpp];
				p2 = (iy != Surface->h - 1) ?
					&pixels[(iy + 1) * Surface->pitch + ix * bpp] :
					p1;
				p3 = (ix != Surface->w - 1) ?
					&pixels[iy * Surface->pitch + (ix + 1) * bpp] :
					p1;
				p4 = (iy != Surface->h - 1 && ix != Surface->w - 1) ?
					&pixels[(iy + 1) * Surface->pitch + (ix + 1) * bpp] :
					p1;

				data[x * bpp + 0] = static_cast<unsigned char>(
					(p1[0] * (1 - fy) + p2[0] * fy +
					p1[0] * (1 - fx) + p3[0] * fx +
					p1[0] * (1 - fz) + p4[0] * fz) / 3.0 + .5);
				data[x * bpp + 1] = static_cast<unsigned char>(
					(p1[1] * (1 - fy) + p2[1] * fy +
					p1[1] * (1 - fx) + p3[1] * fx +
					p1[1] * (1 - fz) + p4[1] * fz) / 3.0 + .5);
				data[x * bpp + 2] = static_cast<unsigned char>(
					(p1[2] * (1 - fy) + p2[2] * fy +
					p1[2] * (1 - fx) + p3[2] * fx +
					p1[2] * (1 - fz) + p4[2] * fz) / 3.0 + .5);
				if (bpp == 4) {
					data[x * bpp + 3] = static_cast<unsigned char>(
						(p1[3] * (1 - fy) + p2[3] * fy +
						p1[3] * (1 - fx) + p3[3] * fx +
						p1[3] * (1 - fz) + p4[3] * fz) / 3.0 + .5);
				}
				++x;
			}
		}

		int Rmask = Surface->format->Rmask;
		int Gmask = Surface->format->Gmask;
		int Bmask = Surface->format->Bmask;
		int Amask = Surface->format->Amask;

		SDL_UnlockSurface(Surface);
		SDL_FreeSurface(Surface);

		Surface = SDL_CreateRGBSurfaceFrom(data, w, h, 8 * bpp, w * bpp,
			Rmask, Gmask, Bmask, Amask);
	}

	Width = GraphicWidth = w;
	Height = GraphicHeight = h;
}

/**
**  Check if a pixel is transparent
**
**  @param x  X coordinate
**  @param y  Y coordinate
**
**  @return   True if the pixel is transparent, False otherwise
*/
bool CGraphic::TransparentPixel(int x, int y)
{
	unsigned char *p;
	int bpp;
	bool ret;
	Uint32 colorkey;
	bool has_colorkey = !SDL_GetColorKey(Surface, &colorkey);

	bpp = Surface->format->BytesPerPixel;
	if ((bpp == 1 && !has_colorkey) || bpp == 3) {
		return false;
	}

	ret = 0;
	SDL_LockSurface(Surface);
	p = (unsigned char *)Surface->pixels + y * Surface->pitch + x * bpp;
	if (bpp == 1) {
		if (*p == colorkey) {
			ret = true;
		}
	} else {
		if (p[Surface->format->Ashift >> 3] == 255) {
			ret = true;
		}
	}
	SDL_UnlockSurface(Surface);

	return ret;
}

/**
**  Make shadow sprite
**
**  @todo FIXME: 32bpp
*/
void CGraphic::MakeShadow()
{
	SDL_Color colors[256];

	// Set all colors in the palette to black and use 50% alpha
	memset(colors, 0, sizeof(colors));

	SDL_SetPaletteColors(Surface->format->palette, colors, 0, 256);
	SDL_SetSurfaceAlphaMod(Surface, 128);
}

void FreeGraphics()
{
	CGraphicsModule::AssertOK();
	std::map<std::string, CGraphic *>::iterator i;
	while (!GraphicHash.empty()) {
		i = GraphicHash.begin();
		CGraphic::Free((*i).second);
	}
}

//@}
