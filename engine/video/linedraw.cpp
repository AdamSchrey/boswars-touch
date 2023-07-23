//     ____                _       __
//    / __ )____  _____   | |     / /___ ___________
//   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
//  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
// /_____/\____/____/     |__/|__/\__,_/_/  /____/
//
//       A futuristic real-time strategy game.
//          This file is part of Bos Wars.
//
/**@name linedraw.cpp - The general linedraw functions. */
//
//      (c) Copyright 2000-2008 by Lutz Sammer, Stephan Rasenberg,
//                                 Jimmy Salmon, and Nehal Mistry
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
#include "video.h"
#include "intern_video.h"


/**
** Bitmask, denoting a postion left/right/above/below clip rectangle
** (mainly used by VideoDrawLineClip)
*/
#define ClipCodeInside 0 /// Clipping inside rectangle
#define ClipCodeAbove  1 /// Clipping above rectangle
#define ClipCodeBelow  2 /// Clipping below rectangle
#define ClipCodeLeft   4 /// Clipping left rectangle
#define ClipCodeRight  8 /// Clipping right rectangle


namespace linedraw_sdl {

static inline void set_renderer_color(Uint32 color, int alpha)
{
	SDL_SetRenderDrawColor(TheRenderer, (color >> 16) & 0xff, (color >> 8) & 0xff,
		(color >> 0) & 0xff, alpha);
}

void VideoDrawPixel(Uint32 color, int x, int y)
{
	set_renderer_color(color, 255);
	SDL_RenderDrawPoint(TheRenderer, x, y);
}

void VideoDrawTransPixel(Uint32 color, int x, int y, unsigned char alpha)
{
	set_renderer_color(color, alpha);
	SDL_RenderDrawPoint(TheRenderer, x, y);
}

/**
**  Draw a clipped pixel
*/
static void DrawPixelClip(Uint32 color, int x, int y)
{
	if (x >= ClipX1 && y >= ClipY1 && x <= ClipX2 && y <= ClipY2) {
		VideoDrawPixel(color, x, y);
	}
}

/**
**  Draw a transparent clipped pixel
*/
static void VideoDrawTransPixelClip(Uint32 color, int x, int y, unsigned char alpha)
{
	if (x >= ClipX1 && y >= ClipY1 && x <= ClipX2 && y <= ClipY2) {
		VideoDrawTransPixel(color, x, y, alpha);
	}
}

/**
**  Draw a transparent clipped pixel
*/
void DrawTransPixelClip(Uint32 color, int x, int y, unsigned char alpha)
{
	VideoDrawTransPixel(color, x, y, alpha);
}

/**
**  Draw a vertical line
*/
void DrawVLine(Uint32 color, int x, int y, int height)
{
	set_renderer_color(color, 255);
	SDL_RenderDrawLine(TheRenderer, x, y, x, y + height);
}

/**
**  Draw a transparent vertical line
*/
void DrawTransVLine(Uint32 color, int x, int y,
	int height, unsigned char alpha)
{
	set_renderer_color(color, alpha);
	SDL_RenderDrawLine(TheRenderer, x, y, x, y + height);
}

/**
**  Draw a vertical line clipped
*/
void DrawVLineClip(Uint32 color, int x, int y, int height)
{
	int w = 1;
	CLIP_RECTANGLE(x, y, w, height);
	DrawVLine(color, x, y, height);
}

/**
**  Draw a transparent vertical line clipped
*/
void DrawTransVLineClip(Uint32 color, int x, int y,
	int height, unsigned char alpha)
{
	int w = 1;
	CLIP_RECTANGLE(x, y, w, height);
	set_renderer_color(color, alpha);
	SDL_RenderDrawLine(TheRenderer, x, y, x, y + height);
}

/**
**  Draw a horizontal line
*/
void DrawHLine(Uint32 color, int x, int y, int width)
{
	set_renderer_color(color, 255);
	SDL_RenderDrawLine(TheRenderer, x, y, x + width, y);
}

/**
**  Draw a horizontal line clipped
*/
void DrawHLineClip(Uint32 color, int x, int y, int width)
{
	int h = 1;
	CLIP_RECTANGLE(x, y, width, h);
	set_renderer_color(color, 255);
	SDL_RenderDrawLine(TheRenderer, x, y, x + width, y);
}

/**
**  Draw a transparent horizontal line
*/
void DrawTransHLine(Uint32 color, int x, int y,
	int width, unsigned char alpha)
{
	set_renderer_color(color, alpha);
	SDL_RenderDrawLine(TheRenderer, x, y, x + width, y);
}

/**
**  Draw a transparent horizontal line clipped
*/
void DrawTransHLineClip(Uint32 color, int x, int y,
	int width, unsigned char alpha)
{
	int h=1;
	CLIP_RECTANGLE(x, y, width, h);
	set_renderer_color(color, alpha);
	SDL_RenderDrawLine(TheRenderer, x, y, x + width, y);
}

/**
**  Draw a line
*/
void DrawLine(Uint32 color, int sx, int sy, int dx, int dy)
{
	SDL_SetRenderDrawColor(TheRenderer, (color >> 16) & 0xff, (color >> 8) & 0xff,
		(color >> 0) & 0xff, 255);
	SDL_RenderDrawLine(TheRenderer, sx, sy, dx, dy);
}

/**
**  Draw a line clipped
*/
void DrawLineClip(Uint32 color, int sx, int sy, int dx, int dy)
{
	int w = dx - sx;
	int h = dy - sy;
	CLIP_RECTANGLE(sx, sy, w, h);
	SDL_SetRenderDrawColor(TheRenderer, (color >> 16) & 0xff, (color >> 8) & 0xff,
		(color >> 0) & 0xff, 255);
	SDL_RenderDrawLine(TheRenderer, sx, sy, sx+w, sy+h);
}

/**
**  Draw a transparent line
*/
void DrawTransLine(Uint32 color, int sx, int sy,
	int dx, int dy, unsigned char alpha)
{
	// FIXME: trans
	DrawLine(color, sx, sy, dx, dy);
}

/**
**  Draw a transparent line clipped
*/
void DrawTransLineClip(Uint32 color, int sx, int sy,
	int dx, int dy, unsigned char alpha)
{
	// FIXME: trans
	DrawLineClip(color, sx, sy, dx, dy);
}

static void DrawSdlRect(int x, int y, int w, int h)
{
	SDL_Rect rect = {x, y, w, h};

	SDL_RenderDrawRect(TheRenderer, &rect);
}

/**
**  Draw a rectangle
*/
void DrawRectangle(Uint32 color, int x, int y, int w, int h)
{
	SDL_SetRenderDrawColor(TheRenderer, (color >> 16) & 0xff, (color >> 8) & 0xff,
		(color >> 0) & 0xff, 255);
	DrawSdlRect(x, y, w, h);
}

/**
**  Draw a rectangle clipped
*/
void DrawRectangleClip(Uint32 color, int x, int y, int w, int h)
{
	CLIP_RECTANGLE(x, y, w, h);
	DrawRectangle(color, x, y, w, h);
}

/**
**  Draw a transparent rectangle
*/
void DrawTransRectangle(Uint32 color, int x, int y,
	int w, int h, unsigned char alpha)
{
	SDL_SetRenderDrawColor(TheRenderer, (color >> 8) & 0xff, (color >> 0) & 0xff,
		(color >> 16) & 0xff, alpha);
	DrawSdlRect(x, y, w, h);
}

/**
**  Draw a transparent rectangle clipped.
**
**  @param color  color
**  @param x      x coordinate on the screen
**  @param y      y coordinate on the screen
**  @param h      height of rectangle (0=don't draw).
**  @param w      width of rectangle (0=don't draw).
**  @param alpha  alpha value of pixels.
*/
void DrawTransRectangleClip(Uint32 color, int x, int y,
	int w, int h, unsigned char alpha)
{
	CLIP_RECTANGLE(x, y, w, h);
	DrawTransRectangle(color, x, y, w, h, alpha);
}

/**
**  Draw a filled rectangle
*/
void FillRectangle(Uint32 color, int x, int y, int w, int h)
{
	SDL_Rect drect = {
		static_cast<Sint16>(x),
		static_cast<Sint16>(y),
		static_cast<Uint16>(w),
		static_cast<Uint16>(h)
	};
	SDL_FillRect(TheScreen, &drect, color);
	set_renderer_color(color, 255);
	SDL_RenderFillRect(TheRenderer, &drect);
}

/**
**  Draw a filled rectangle clipped
*/
void FillRectangleClip(Uint32 color, int x, int y,
	int w, int h)
{
	CLIP_RECTANGLE(x, y, w, h);

	FillRectangle(color, x, y, w, h);
}

/**
**  Draw a filled transparent rectangle
*/
void FillTransRectangle(Uint32 color, int x, int y,
	int w, int h, unsigned char alpha)
{
	SDL_Rect rect = {x, y, w, h};

	SDL_SetRenderDrawBlendMode(TheRenderer, SDL_BLENDMODE_BLEND);
	set_renderer_color(color, alpha);

	SDL_RenderFillRect(TheRenderer, &rect);
}

/**
**  Draw a filled transparent rectangle clipped
*/
void FillTransRectangleClip(Uint32 color, int x, int y,
	int w, int h, unsigned char alpha)
{
	CLIP_RECTANGLE(x, y, w, h);
	FillTransRectangle(color, x, y, w, h, alpha);
}

/**
**  Draw a circle
*/
void DrawCircle(Uint32 color, int x, int y, int r)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;

	for (px = 0; px <= py + 1; ++px) {
		VideoDrawPixel(color, x + px, y + py);
		VideoDrawPixel(color, x + px, y - py);
		VideoDrawPixel(color, x - px, y + py);
		VideoDrawPixel(color, x - px, y - py);

		VideoDrawPixel(color, x + py, y + px);
		VideoDrawPixel(color, x + py, y - px);
		VideoDrawPixel(color, x - py, y + px);
		VideoDrawPixel(color, x - py, y - px);

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
		}
	}

}

/**
**  Draw a transparent circle
*/
void DrawTransCircle(Uint32 color, int x, int y,
	int r, unsigned char alpha)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;


	for (px = 0; px <= py + 1; ++px) {
		VideoDrawTransPixel(color, x + px, y + py, alpha);
		VideoDrawTransPixel(color, x + px, y - py, alpha);
		VideoDrawTransPixel(color, x - px, y + py, alpha);
		VideoDrawTransPixel(color, x - px, y - py, alpha);

		VideoDrawTransPixel(color, x + py, y + px, alpha);
		VideoDrawTransPixel(color, x + py, y - px, alpha);
		VideoDrawTransPixel(color, x - py, y + px, alpha);
		VideoDrawTransPixel(color, x - py, y - px, alpha);

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
		}
	}

}

/**
**  Draw a circle clipped
*/
void DrawCircleClip(Uint32 color, int x, int y, int r)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;


	for (px = 0; px <= py + 1; ++px) {
		DrawPixelClip(color, x + px, y + py);
		DrawPixelClip(color, x + px, y - py);
		DrawPixelClip(color, x - px, y + py);
		DrawPixelClip(color, x - px, y - py);

		DrawPixelClip(color, x + py, y + px);
		DrawPixelClip(color, x + py, y - px);
		DrawPixelClip(color, x - py, y + px);
		DrawPixelClip(color, x - py, y - px);

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
		}
	}

}

/**
**  Draw a transparent circle clipped
*/
void DrawTransCircleClip(Uint32 color, int x, int y,
	int r, unsigned char alpha)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;

	for (px = 0; px <= py + 1; ++px) {
		VideoDrawTransPixelClip(color, x + px, y + py, alpha);
		VideoDrawTransPixelClip(color, x + px, y - py, alpha);
		VideoDrawTransPixelClip(color, x - px, y + py, alpha);
		VideoDrawTransPixelClip(color, x - px, y - py, alpha);

		VideoDrawTransPixelClip(color, x + py, y + px, alpha);
		VideoDrawTransPixelClip(color, x + py, y - px, alpha);
		VideoDrawTransPixelClip(color, x - py, y + px, alpha);
		VideoDrawTransPixelClip(color, x - py, y - px, alpha);

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
		}
	}
}

/**
**  Draw a filled circle
*/
void FillCircle(Uint32 color, int x, int y, int r)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;

	for (px = 0; px <= py; ++px) {

		// Fill up the middle half of the circle
		DrawVLine(color, x + px, y, py + 1);
		DrawVLine(color, x + px, y - py, py);
		if (px) {
			DrawVLine(color, x - px, y, py + 1);
			DrawVLine(color, x - px, y - py, py);
		}

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
			// Fill up the left/right half of the circle
			if (py >= px) {
				DrawVLine(color, x + py + 1, y, px + 1);
				DrawVLine(color, x + py + 1, y - px, px);
				DrawVLine(color, x - py - 1, y, px + 1);
				DrawVLine(color, x - py - 1, y - px,  px);
			}
		}
	}
}

/**
**  Draw a filled transparent circle
*/
void FillTransCircle(Uint32 color, int x, int y,
	int r, unsigned char alpha)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;

	for (px = 0; px <= py; ++px) {

		// Fill up the middle half of the circle
		DrawTransVLine(color, x + px, y, py + 1, alpha);
		DrawTransVLine(color, x + px, y - py, py, alpha);
		if (px) {
			DrawTransVLine(color, x - px, y, py + 1, alpha);
			DrawTransVLine(color, x - px, y - py, py, alpha);
		}

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
			// Fill up the left/right half of the circle
			if (py >= px) {
				DrawTransVLine(color, x + py + 1, y, px + 1, alpha);
				DrawTransVLine(color, x + py + 1, y - px, px, alpha);
				DrawTransVLine(color, x - py - 1, y, px + 1, alpha);
				DrawTransVLine(color, x - py - 1, y - px,  px, alpha);
			}
		}
	}
}

/**
**  Draw a filled circle clipped
*/
void FillCircleClip(Uint32 color, int x, int y, int r)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;

	for (px = 0; px <= py; ++px) {

		// Fill up the middle half of the circle
		DrawVLineClip(color, x + px, y, py + 1);
		DrawVLineClip(color, x + px, y - py, py);
		if (px) {
			DrawVLineClip(color, x - px, y, py + 1);
			DrawVLineClip(color, x - px, y - py, py);
		}

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
			// Fill up the left/right half of the circle
			if (py >= px) {
				DrawVLineClip(color, x + py + 1, y, px + 1);
				DrawVLineClip(color, x + py + 1, y - px, px);
				DrawVLineClip(color, x - py - 1, y, px + 1);
				DrawVLineClip(color, x - py - 1, y - px,  px);
			}
		}
	}
}

/**
**  Draw a filled transparent circle clipped
*/
void FillTransCircleClip(Uint32 color, int x, int y,
	int r, unsigned char alpha)
{
	int p;
	int px;
	int py;

	p = 1 - r;
	py = r;

	for (px = 0; px <= py; ++px) {

		// Fill up the middle half of the circle
		DrawTransVLineClip(color, x + px, y, py + 1, alpha);
		DrawTransVLineClip(color, x + px, y - py, py, alpha);
		if (px) {
			DrawTransVLineClip(color, x - px, y, py + 1, alpha);
			DrawTransVLineClip(color, x - px, y - py, py, alpha);
		}

		if (p < 0) {
			p += 2 * px + 3;
		} else {
			p += 2 * (px - py) + 5;
			py -= 1;
			// Fill up the left/right half of the circle
			if (py >= px) {
				DrawTransVLineClip(color, x + py + 1, y, px + 1, alpha);
				DrawTransVLineClip(color, x + py + 1, y - px, px, alpha);
				DrawTransVLineClip(color, x - py - 1, y, px + 1, alpha);
				DrawTransVLineClip(color, x - py - 1, y - px,  px, alpha);
			}
		}
	}
}

/**
**  Initialize line draw
*/
void InitLineDraw(void)
{
}

}

void CVideo::DrawPixelClip(Uint32 color, int x, int y)
{
	linedraw_sdl::DrawPixelClip(color, x, y);
}
void CVideo::DrawTransPixelClip(Uint32 color, int x, int y, unsigned char alpha)
{
	linedraw_sdl::DrawTransPixelClip(color, x, y, alpha);
}

void CVideo::DrawVLine(Uint32 color, int x, int y, int height)
{
	linedraw_sdl::DrawVLine(color, x, y, height);
}
void CVideo::DrawTransVLine(Uint32 color, int x, int y, int height, unsigned char alpha)
{
	linedraw_sdl::DrawTransVLine(color, x, y, height, alpha);
}
void CVideo::DrawVLineClip(Uint32 color, int x, int y, int height)
{
	linedraw_sdl::DrawVLineClip(color, x, y, height);
}
void CVideo::DrawTransVLineClip(Uint32 color, int x, int y, int height, unsigned char alpha)
{
	linedraw_sdl::DrawTransVLineClip(color, x, y, height, alpha);
}

void CVideo::DrawHLine(Uint32 color, int x, int y, int width)
{
	linedraw_sdl::DrawHLine(color, x, y, width);
}
void CVideo::DrawTransHLine(Uint32 color, int x, int y, int width, unsigned char alpha)
{
	linedraw_sdl::DrawTransHLine(color, x, y, width, alpha);
}
void CVideo::DrawHLineClip(Uint32 color, int x, int y, int width)
{
	linedraw_sdl::DrawHLineClip(color, x, y, width);
}
void CVideo::DrawTransHLineClip(Uint32 color, int x, int y, int width, unsigned char alpha)
{
	linedraw_sdl::DrawTransHLineClip(color, x, y, width, alpha);
}

void CVideo::DrawLine(Uint32 color, int sx, int sy, int dx, int dy)
{
	linedraw_sdl::DrawLine(color, sx, sy, dx, dy);
}
void CVideo::DrawTransLine(Uint32 color, int sx, int sy, int dx, int dy, unsigned char alpha)
{
	linedraw_sdl::DrawTransLine(color, sx, sy, dx, dy, alpha);
}
void CVideo::DrawLineClip(Uint32 color, int sx, int sy, int dx, int dy)
{
	linedraw_sdl::DrawLineClip(color, sx, sy, dx, dy);
}
void CVideo::DrawTransLineClip(Uint32 color, int sx, int sy, int dx, int dy, unsigned char alpha)
{
	linedraw_sdl::DrawTransLineClip(color, sx, sy, dx, dy, alpha);
}

void CVideo::DrawRectangle(Uint32 color, int x, int y, int w, int h)
{
	linedraw_sdl::DrawRectangle(color, x, y, w, h);
}
void CVideo::DrawTransRectangle(Uint32 color, int x, int y, int w, int h, unsigned char alpha)
{
	linedraw_sdl::DrawTransRectangle(color, x, y, w, h, alpha);
}
void CVideo::DrawRectangleClip(Uint32 color, int x, int y, int w, int h)
{
	linedraw_sdl::DrawRectangleClip(color, x, y, w, h);
}
void CVideo::DrawTransRectangleClip(Uint32 color, int x, int y, int w, int h, unsigned char alpha)
{
	linedraw_sdl::DrawTransRectangleClip(color, x, y, w, h, alpha);
}

void CVideo::FillRectangle(Uint32 color, int x, int y, int w, int h)
{
	linedraw_sdl::FillRectangle(color, x, y, w, h);
}
void CVideo::FillTransRectangle(Uint32 color, int x, int y, int w, int h, unsigned char alpha)
{
	linedraw_sdl::FillTransRectangle(color, x, y, w, h, alpha);
}
void CVideo::FillRectangleClip(Uint32 color, int x, int y, int w, int h)
{
	linedraw_sdl::FillRectangleClip(color, x, y, w, h);
}
void CVideo::FillTransRectangleClip(Uint32 color, int x, int y, int w, int h, unsigned char alpha)
{
	linedraw_sdl::FillTransRectangleClip(color, x, y, w, h, alpha);
}

void CVideo::DrawCircle(Uint32 color, int x, int y, int r)
{
	linedraw_sdl::DrawCircle(color, x, y, r);
}
void CVideo::DrawTransCircle(Uint32 color, int x, int y, int r, unsigned char alpha)
{
	linedraw_sdl::DrawTransCircle(color, x, y, r, alpha);
}
void CVideo::DrawCircleClip(Uint32 color, int x, int y, int r)
{
	linedraw_sdl::DrawCircleClip(color, x, y, r);
}
void CVideo::DrawTransCircleClip(Uint32 color, int x, int y, int r, unsigned char alpha)
{
	linedraw_sdl::DrawTransCircleClip(color, x, y, r, alpha);
}

void CVideo::FillCircle(Uint32 color, int x, int y, int r)
{
	linedraw_sdl::FillCircle(color, x, y, r);
}
void CVideo::FillTransCircle(Uint32 color, int x, int y, int r, unsigned char alpha)
{
	linedraw_sdl::FillTransCircle(color, x, y, r, alpha);
}
void CVideo::FillCircleClip(Uint32 color, int x, int y, int r)
{
	linedraw_sdl::FillCircleClip(color, x, y, r);
}
void CVideo::FillTransCircleClip(Uint32 color, int x, int y, int r, unsigned char alpha)
{
	linedraw_sdl::FillTransCircleClip(color, x, y, r, alpha);
}

void InitLineDraw(void)
{
	linedraw_sdl::InitLineDraw();
}

//@}
