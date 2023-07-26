//     ____                _       __
//    / __ )____  _____   | |     / /___ ___________
//   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
//  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
// /_____/\____/____/     |__/|__/\__,_/_/  /____/
//
//       A futuristic real-time strategy game.
//          This file is part of Bos Wars.
//
/**@name sdl.cpp - SDL video support. */
//
//      (c) Copyright 1999-2010 by Lutz Sammer, Jimmy Salmon, Nehal Mistry
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

#include "stratagus.h"

#ifdef DEBUG
#include <signal.h>
#endif

#include <map>
#include <string>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#ifndef _MSC_VER
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef USE_WIN32
#include "net_lowlevel.h"
#endif

#include "video.h"
#include "font.h"
#include "interface.h"
#include "network.h"
#include "ui.h"
#include "sound_server.h"
#include "sound.h"
#include "interface.h"
#include "minimap.h"
#include "widgets.h"
#include "editor.h"


SDL_Window *TheWindow; /// Internal screen
SDL_Renderer *TheRenderer = NULL; /// Internal screen
SDL_Texture *TheTexture; /// Internal screen
SDL_Surface *TheScreen; /// Internal screen

static SDL_Rect Rects[100];
static int NumRects;

static std::map<int, std::string> Key2Str;
static std::map<std::string, int> Str2Key;

static int FrameTicks;     /// Frame length in ms
static int FrameRemainder; /// Frame remainder 0.1 ms
static int FrameFraction;  /// Frame fractional term

const EventCallback *Callbacks;

#ifdef DEBUG
bool DumpAllSdlEvents;               /// Show all events received from SDL
#endif

/*----------------------------------------------------------------------------
--  Sync
----------------------------------------------------------------------------*/

/**
**  Initialise video sync.
**  Calculate the length of video frame and any simulation skips.
**
**  @see VideoSyncSpeed @see SkipFrames @see FrameTicks @see FrameRemainder
*/
void SetVideoSync(void)
{
	int ms;

	if (VideoSyncSpeed) {
		ms = (1000 * 1000 / CYCLES_PER_SECOND) / VideoSyncSpeed;
	} else {
		ms = INT_MAX;
	}
	SkipFrames = ms / 400;
	while (SkipFrames && ms / SkipFrames < 200) {
		--SkipFrames;
	}
	ms /= SkipFrames + 1;

	FrameTicks = ms / 10;
	FrameRemainder = ms % 10;
	DebugPrint("frames %d - %d.%dms\n" _C_ SkipFrames _C_ ms / 10 _C_ ms % 10);
}

/*----------------------------------------------------------------------------
--  Video
----------------------------------------------------------------------------*/

#if defined(DEBUG) && !defined(USE_WIN32)
static void CleanExit(int signum)
{
	// Clean SDL
	SDL_Quit();
	// Reestablish normal behaviour for next abort call
	signal(SIGABRT, SIG_DFL);
	// Generates a core dump
	abort();
}
#endif

/**
**  Initialize SDLKey to string map
*/
static void InitKey2Str()
{
	Str2Key[_("esc")] = SDLK_ESCAPE;

	if (!Key2Str.empty()) {
		return;
	}

	int i;
	char str[20];

	Key2Str[SDLK_BACKSPACE] = "backspace";
	Key2Str[SDLK_TAB] = "tab";
	Key2Str[SDLK_CLEAR] = "clear";
	Key2Str[SDLK_RETURN] = "return";
	Key2Str[SDLK_PAUSE] = "pause";
	Key2Str[SDLK_ESCAPE] = "escape";
	Key2Str[SDLK_SPACE] = " ";
	Key2Str[SDLK_EXCLAIM] = "!";
	Key2Str[SDLK_QUOTEDBL] = "\"";
	Key2Str[SDLK_HASH] = "#";
	Key2Str[SDLK_DOLLAR] = "$";
	Key2Str[SDLK_AMPERSAND] = "&";
	Key2Str[SDLK_QUOTE] = "'";
	Key2Str[SDLK_LEFTPAREN] = "(";
	Key2Str[SDLK_RIGHTPAREN] = ")";
	Key2Str[SDLK_ASTERISK] = "*";
	Key2Str[SDLK_PLUS] = "+";
	Key2Str[SDLK_COMMA] = ",";
	Key2Str[SDLK_MINUS] = "-";
	Key2Str[SDLK_PERIOD] = ".";
	Key2Str[SDLK_SLASH] = "/";

	str[1] = '\0';
	for (i = SDLK_0; i <= SDLK_9; ++i) {
		str[0] = i;
		Key2Str[i] = str;
	}

	Key2Str[SDLK_COLON] = ":";
	Key2Str[SDLK_SEMICOLON] = ";";
	Key2Str[SDLK_LESS] = "<";
	Key2Str[SDLK_EQUALS] = "=";
	Key2Str[SDLK_GREATER] = ">";
	Key2Str[SDLK_QUESTION] = "?";
	Key2Str[SDLK_AT] = "@";
	Key2Str[SDLK_LEFTBRACKET] = "[";
	Key2Str[SDLK_BACKSLASH] = "\\";
	Key2Str[SDLK_RIGHTBRACKET] = "]";
	Key2Str[SDLK_BACKQUOTE] = "`";

	str[1] = '\0';
	for (i = SDLK_a; i <= SDLK_z; ++i) {
		str[0] = i;
		Key2Str[i] = str;
	}

	Key2Str[SDLK_DELETE] = "delete";

	for (i = SDLK_KP_0; i <= SDLK_KP_9; ++i) {
		sprintf_s(str, sizeof(str), "kp_%d", i - SDLK_KP_0);
		Key2Str[i] = str;
	}

	Key2Str[SDLK_KP_PERIOD] = "kp_period";
	Key2Str[SDLK_KP_DIVIDE] = "kp_divide";
	Key2Str[SDLK_KP_MULTIPLY] = "kp_multiply";
	Key2Str[SDLK_KP_MINUS] = "kp_minus";
	Key2Str[SDLK_KP_PLUS] = "kp_plus";
	Key2Str[SDLK_KP_ENTER] = "kp_enter";
	Key2Str[SDLK_KP_EQUALS] = "kp_equals";
	Key2Str[SDLK_UP] = "up";
	Key2Str[SDLK_DOWN] = "down";
	Key2Str[SDLK_RIGHT] = "right";
	Key2Str[SDLK_LEFT] = "left";
	Key2Str[SDLK_INSERT] = "insert";
	Key2Str[SDLK_HOME] = "home";
	Key2Str[SDLK_END] = "end";
	Key2Str[SDLK_PAGEUP] = "pageup";
	Key2Str[SDLK_PAGEDOWN] = "pagedown";

	for (i = SDLK_F1; i <= SDLK_F15; ++i) {
		sprintf_s(str, sizeof(str), "f%d", i - SDLK_F1 + 1);
		Key2Str[i] = str;
		sprintf_s(str, sizeof(str), "F%d", i - SDLK_F1 + 1);
		Str2Key[str] = i;
	}

	Key2Str[SDLK_HELP] = "help";
	Key2Str[SDLK_PRINTSCREEN] = "print";
	Key2Str[SDLK_SYSREQ] = "sysreq";
	Key2Str[SDLK_PAUSE] = "break";
	Key2Str[SDLK_MENU] = "menu";
	Key2Str[SDLK_POWER] = "power";
	Key2Str[SDLK_UNDO] = "undo";
}

/**
**  Initialize the video part for SDL.
*/
void InitVideoSdl(void)
{
	Uint32 flags;

	if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
		if (SDL_Init(
				SDL_INIT_AUDIO | SDL_INIT_VIDEO |
				SDL_INIT_TIMER) < 0 ) {
			fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
			exit(1);
		}

		// Clean up on exit

		atexit(SDL_Quit);

		// If debug is enabled, Stratagus disable SDL Parachute.
		// So we need gracefully handle segfaults and aborts.
#if defined(DEBUG) && !defined(USE_WIN32)
		signal(SIGSEGV, CleanExit);
		signal(SIGABRT, CleanExit);
#endif
	}

	// Initialize the display

	flags = 0;
	// Sam said: better for windows.
	/* SDL_HWSURFACE|SDL_HWPALETTE | */
	if (Video.FullScreen) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	if (!Video.Width || !Video.Height) {
		Video.Width = 640;
		Video.Height = 480;
	}

	TheWindow = SDL_CreateWindow("Bos Wars", SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		Video.Width, Video.Height, flags);
	if (TheWindow == NULL) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
			Video.Width, Video.Height, Video.Depth, SDL_GetError());
		exit(1);
	}

	if (!TheRenderer) {
		TheRenderer = SDL_CreateRenderer(TheWindow, -1, 0);
		SDL_RenderClear(TheRenderer);
	}
	SDL_RenderSetLogicalSize(TheRenderer, Video.Width, Video.Height);
	SDL_SetRenderDrawColor(TheRenderer, 0, 0, 0, 255);
	TheScreen = SDL_CreateRGBSurface(0, Video.Width, Video.Height, 32,
										0x00FF0000,
										0x0000FF00,
										0x000000FF,
										0);
	TheTexture = SDL_CreateTexture(TheRenderer,
									  SDL_PIXELFORMAT_ARGB8888,
									  SDL_TEXTUREACCESS_STREAMING,
									  Video.Width, Video.Height);

	Video.FullScreen = (SDL_GetWindowFlags(TheWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 1 : 0;
	Video.Depth = TheScreen->format->BitsPerPixel;

	// Turn cursor off, we use our own.
	SDL_ShowCursor(0);

	InitKey2Str();

	ColorBlack = Video.MapRGB(0, 0, 0);
	ColorDarkGreen = Video.MapRGB(48, 100, 4);
	ColorDarkBlue = Video.MapRGB(0, 0, 96);
	ColorBlue = Video.MapRGB(0, 0, 252);
	ColorCyan = Video.MapRGB(0, 160, 208);
	ColorOrange = Video.MapRGB(248, 140, 20);
	ColorWhite = Video.MapRGB(252, 248, 240);
	ColorGray = Video.MapRGB(128, 128, 128);
	ColorRed = Video.MapRGB(252, 0, 0);
	ColorGreen = Video.MapRGB(0, 252, 0);
	ColorYellow = Video.MapRGB(252, 252, 0);

	UI.MouseWarpX = UI.MouseWarpY = -1;
}

/**
**  Check if a resolution is valid
**
**  @param w  Width
**  @param h  Height
*/
int VideoValidResolution(int w, int h)
{
	return 1;
}

/**
**  Invalidate some area
**
**  @param x  screen pixel X position.
**  @param y  screen pixel Y position.
**  @param w  width of rectangle in pixels.
**  @param h  height of rectangle in pixels.
*/
void InvalidateArea(int x, int y, int w, int h)
{
	Assert(NumRects != sizeof(Rects) / sizeof(*Rects));
	Assert(x >= 0 && y >= 0 && x + w <= Video.Width && y + h <= Video.Height);
	Rects[NumRects].x = x;
	Rects[NumRects].y = y;
	Rects[NumRects].w = w;
	Rects[NumRects].h = h;
	++NumRects;
}

/**
**  Invalidate whole window
*/
void Invalidate(void)
{
	Rects[0].x = 0;
	Rects[0].y = 0;
	Rects[0].w = Video.Width;
	Rects[0].h = Video.Height;
	NumRects = 1;
}


#ifdef DEBUG

static void DumpSdlActiveEvent(const SDL_WindowEvent &active, const char *type)
{
	printf("SDL_WindowEvent {"
	       " type=%s, event=%d }\n",
	       type, active.event);
}

static void DumpSdlKeyboardEvent(const SDL_KeyboardEvent &key, const char *type)
{
	printf("SDL_KeyboardEvent {"
	       " type=%s, state=%d, keysym={"
	       " scancode=%d, sym=%d, mod=%d, }}\n",
	       type,
	       static_cast<int>(key.state),
	       static_cast<int>(key.keysym.scancode),
	       static_cast<int>(key.keysym.sym),
	       static_cast<int>(key.keysym.mod));
}

static void DumpSdlMouseMotionEvent(const SDL_MouseMotionEvent &motion, const char *type)
{
	printf("SDL_MouseMotionEvent {"
	       " type=%s, which=%d, state=%d,"
	       " x=%d, y=%d, xrel=%d, yrel=%d }\n",
	       type,
	       static_cast<int>(motion.which),
	       static_cast<int>(motion.state),
	       static_cast<int>(motion.x),
	       static_cast<int>(motion.y),
	       static_cast<int>(motion.xrel),
	       static_cast<int>(motion.yrel));
}

static void DumpSdlMouseButtonEvent(const SDL_MouseButtonEvent &button, const char *type)
{
	printf("SDL_MouseButtonEvent {"
	       " type=%s, which=%d, button=%d, state=%d, x=%d, y=%d }\n",
	       type,
	       static_cast<int>(button.which),
	       static_cast<int>(button.button),
	       static_cast<int>(button.state),
	       static_cast<int>(button.x),
	       static_cast<int>(button.y));
}

static void DumpSdlJoyAxisEvent(const SDL_JoyAxisEvent &jaxis, const char *type)
{
	printf("SDL_JoyAxisEvent {"
	       " type=%s, which=%d, axis=%d, value=%d }\n",
	       type,
	       static_cast<int>(jaxis.which),
	       static_cast<int>(jaxis.axis),
	       static_cast<int>(jaxis.value));
}

static void DumpSdlJoyBallEvent(const SDL_JoyBallEvent &jball, const char *type)
{
	printf("SDL_JoyBallEvent {"
	       " type=%s, which=%d, ball=%d, xrel=%d, yrel=%d }\n",
	       type,
	       static_cast<int>(jball.which),
	       static_cast<int>(jball.ball),
	       static_cast<int>(jball.xrel),
	       static_cast<int>(jball.yrel));
}

static void DumpSdlJoyHatEvent(const SDL_JoyHatEvent &jhat, const char *type)
{
	printf("SDL_JoyHatEvent {"
	       " type=%s, which=%d, button=%d, state=%d }\n",
	       type,
	       static_cast<int>(jhat.which),
	       static_cast<int>(jhat.hat),
	       static_cast<int>(jhat.value));
}

static void DumpSdlJoyButtonEvent(const SDL_JoyButtonEvent &jbutton, const char *type)
{
	printf("SDL_JoyButtonEvent {"
	       " type=%s, which=%d, button=%d, state=%d }\n",
	       type,
	       static_cast<int>(jbutton.which),
	       static_cast<int>(jbutton.button),
	       static_cast<int>(jbutton.state));
}

static void DumpSdlQuitEvent(const SDL_QuitEvent &, const char *type)
{
	printf("SDL_QuitEvent {"
	       " type=%s }\n",
	       type);
}

static void DumpSdlEvent(const SDL_Event *event)
{
	switch (event->type) {
	case SDL_WINDOWEVENT:
		DumpSdlActiveEvent(event->window, "SDL_ACTIVEEVENT");
		break;

	case SDL_KEYDOWN:
		DumpSdlKeyboardEvent(event->key, "SDL_KEYDOWN");
		break;

	case SDL_KEYUP:
		DumpSdlKeyboardEvent(event->key, "SDL_KEYUP");
		break;

	case SDL_MOUSEMOTION:
		DumpSdlMouseMotionEvent(event->motion, "SDL_MOUSEMOTION");
		break;

	case SDL_MOUSEBUTTONDOWN:
		DumpSdlMouseButtonEvent(event->button, "SDL_MOUSEBUTTONDOWN");
		break;

	case SDL_MOUSEBUTTONUP:
		DumpSdlMouseButtonEvent(event->button, "SDL_MOUSEBUTTONUP");
		break;

	case SDL_JOYAXISMOTION:
		DumpSdlJoyAxisEvent(event->jaxis, "SDL_JOYAXISMOTION");
		break;

	case SDL_JOYBALLMOTION:
		DumpSdlJoyBallEvent(event->jball, "SDL_JOYBALLMOTION");
		break;

	case SDL_JOYHATMOTION:
		DumpSdlJoyHatEvent(event->jhat, "SDL_JOYHATMOTION");
		break;

	case SDL_JOYBUTTONDOWN:
		DumpSdlJoyButtonEvent(event->jbutton, "SDL_JOYBUTTONDOWN");
		break;

	case SDL_JOYBUTTONUP:
		DumpSdlJoyButtonEvent(event->jbutton, "SDL_JOYBUTTONUP");
		break;

	case SDL_QUIT:
		DumpSdlQuitEvent(event->quit, "SDL_QUIT");
		break;

	default:
		printf("SDL_Event { type=%d, ... }\n",
		       static_cast<int>(event->type));
		break;

	}
	fflush(stdout);
}

#endif // DEBUG

/**
**  Handle interactive input event.
**
**  @param callbacks  Callback structure for events.
**  @param event      SDL event structure pointer.
*/
static void SdlDoEvent(const EventCallback *callbacks, const SDL_Event *event)
{
#ifdef DEBUG
	if (DumpAllSdlEvents) {
		DumpSdlEvent(event);
	}
#endif

	switch (event->type) {
		case SDL_MOUSEBUTTONDOWN:
			InputMouseButtonPress(callbacks, SDL_GetTicks(),
				event->button.button);
			break;

		case SDL_MOUSEBUTTONUP:
			InputMouseButtonRelease(callbacks, SDL_GetTicks(),
				event->button.button);
			break;

			// FIXME: check if this is only useful for the cursor
			// FIXME: if this is the case we don't need this.
		case SDL_MOUSEMOTION:
			InputMouseMove(callbacks, SDL_GetTicks(),
				event->motion.x, event->motion.y);
			// FIXME: Same bug fix from X11
			if ((UI.MouseWarpX != -1 || UI.MouseWarpY != -1) &&
					(event->motion.x != UI.MouseWarpX ||
						event->motion.y != UI.MouseWarpY)) {
				int xw = UI.MouseWarpX;
				int yw = UI.MouseWarpY;
				UI.MouseWarpX = -1;
				UI.MouseWarpY = -1;
				SDL_WarpMouseInWindow(TheWindow, xw, yw);
			}
			break;

		case SDL_WINDOWEVENT:
		    switch (event->window.event) {
				case SDL_WINDOWEVENT_ENTER:
				case SDL_WINDOWEVENT_LEAVE:
				{
					static bool InMainWindow = true;
					static bool DoTogglePause = false;

					if (InMainWindow && (event->window.event == SDL_WINDOWEVENT_LEAVE)) {
						InputMouseExit(callbacks, SDL_GetTicks());
						if (!GamePaused) {
							DoTogglePause = true;
							UiTogglePause();
						}
					} else if (!InMainWindow && (event->window.event == SDL_WINDOWEVENT_ENTER)) {
						if (GamePaused && DoTogglePause) {
							DoTogglePause = false;
							UiTogglePause();
						}
					}
					InMainWindow = event->window.event == SDL_WINDOWEVENT_ENTER;
				}
				break;
			}
			break;

		case SDL_KEYDOWN:
			InputKeyButtonPress(callbacks, SDL_GetTicks(),
				event->key.keysym.sym, 0);
			break;

		case SDL_KEYUP:
			InputKeyButtonRelease(callbacks, SDL_GetTicks(),
				event->key.keysym.sym, 0);
			break;

		case SDL_TEXTINPUT:
			InputKeyButtonPress(callbacks, SDL_GetTicks(),
				event->text.text[0], event->text.text[0]);
			break;

		case SDL_QUIT:
			Exit(0);
			break;
	}

	if (callbacks == GetCallbacks()) {
		handleInput(event);
	}
}

/**
**  Set the current callbacks
*/
void SetCallbacks(const EventCallback *callbacks)
{
	Callbacks = callbacks;
}

/**
**  Get the current callbacks
*/
const EventCallback *GetCallbacks()
{
	return Callbacks;
}

/**
**  Wait for interactive input event for one frame.
**
**  Handles system events, joystick, keyboard, mouse.
**  Handles the network messages.
**  Handles the sound queue.
**
**  All events available are fetched. Sound and network only if available.
**  Returns if the time for one frame is over.
*/
void WaitEventsOneFrame()
{
	struct timeval tv;
	fd_set rfds;
	fd_set wfds;
	Socket maxfd;
	int i;
	int s;
	SDL_Event event[1];
	Uint32 ticks;
	int interrupts;

	++FrameCounter;

	ticks = SDL_GetTicks();
	if (ticks > NextFrameTicks) { // We are too slow :(
		++SlowFrameCounter;
	}

	InputMouseTimeout(GetCallbacks(), ticks);
	InputKeyTimeout(GetCallbacks(), ticks);
	CursorAnimate(ticks);

	interrupts = 0;

	for (;;) {
		//
		// Time of frame over? This makes the CPU happy. :(
		//
		ticks = SDL_GetTicks();
		if (!interrupts && ticks < NextFrameTicks) {
			SDL_Delay(NextFrameTicks - ticks);
			ticks = SDL_GetTicks();
		}
		while (ticks >= NextFrameTicks) {
			++interrupts;
			FrameFraction += FrameRemainder;
			if (FrameFraction > 10) {
				FrameFraction -= 10;
				++NextFrameTicks;
			}
			NextFrameTicks += FrameTicks;
		}

		//
		// Prepare select
		//
		maxfd = 0;
		tv.tv_sec = tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);

		//
		// Network
		//
		if (IsNetworkGame()) {
			if (NetworkFildes > maxfd) {
				maxfd = NetworkFildes;
			}
			FD_SET(NetworkFildes, &rfds);
		}

#if 0
		s = select(maxfd + 1, &rfds, &wfds, NULL,
			(i = SDL_PollEvent(event)) ? &tv : NULL);
#else
		// QUICK HACK to fix the event/timer problem
		// The timer code didn't interrupt the select call.
		// Perhaps I could send a signal to the process
		// Not very nice, but this is the problem if you use other libraries
		// The event handling of SDL is wrong designed = polling only.
		// There is hope on SDL 1.3 which will have this fixed.

		s = select(maxfd + 1, &rfds, &wfds, NULL, &tv);
		i = SDL_PollEvent(event);
#endif

		if (i) { // Handle SDL event
			SdlDoEvent(GetCallbacks(), event);
		}

		if (s > 0) {
			//
			// Network
			//
			if (IsNetworkGame() && FD_ISSET(NetworkFildes, &rfds) ) {
				GetCallbacks()->NetworkEvent();
			}
		}

		//
		// No more input and time for frame over: return
		//
		if (!i && s <= 0 && interrupts) {
			break;
		}
	}
	handleInput(NULL);

	if (!SkipGameCycle--) {
		SkipGameCycle = SkipFrames;
	}

	if (GameRunning || Editor.Running || PatchEditorRunning) {
		Video.ClearScreen();
	}
}


/**
**  Realize video memory.
*/
void RealizeVideoMemory(void)
{
	if (NumRects) {
		SDL_RenderPresent(TheRenderer);
		NumRects = 0;
	}
	SDL_SetRenderDrawColor(TheRenderer, 0, 0, 0, 255);
	SDL_RenderClear(TheRenderer);
	HideCursor();
}

/**
**  Lock the screen for write access.
*/
void SdlLockScreen(void)
{
	if (SDL_MUSTLOCK(TheScreen)) {
		SDL_LockSurface(TheScreen);
	}
}

/**
**  Unlock the screen for write access.
*/
void SdlUnlockScreen(void)
{
	if (SDL_MUSTLOCK(TheScreen)) {
		SDL_UnlockSurface(TheScreen);
	}
}

/**
**  Convert a SDLKey to a string
*/
const char *SdlKey2Str(int key)
{
	return Key2Str[key].c_str();
}

/**
**  Convert a string to SDLKey
*/
int Str2SdlKey(const char *str)
{
	InitKey2Str();

	std::map<int, std::string>::iterator i;
	for (i = Key2Str.begin(); i != Key2Str.end(); ++i) {
		if (!strcasecmp(str, (*i).second.c_str())) {
			return (*i).first;
		}
	}
	std::map<std::string, int>::iterator i2;
	for (i2 = Str2Key.begin(); i2 != Str2Key.end(); ++i2) {
		if (!strcasecmp(str, (*i2).first.c_str())) {
			return (*i2).second;
		}
	}
	return 0;
}

/**
**  Check if the mouse is grabbed
*/
bool SdlGetGrabMouse(void)
{
	return SDL_GetWindowGrab(TheWindow);
}

/**
**  Toggle grab mouse.
**
**  @param mode  Wanted mode, 1 grab, -1 not grab, 0 toggle.
*/
void ToggleGrabMouse(int mode)
{
	bool grabbed = SdlGetGrabMouse();

	if (mode <= 0 && grabbed) {
		SDL_SetWindowGrab(TheWindow, SDL_FALSE);
	} else if (mode >= 0 && !grabbed) {
		SDL_SetWindowGrab(TheWindow, SDL_TRUE);
	}
}

/**
**  Toggle full screen mode.
*/
void ToggleFullScreen(void)
{
	Uint32 flags;

	flags = SDL_GetWindowFlags(TheWindow) & SDL_WINDOW_FULLSCREEN_DESKTOP;
#ifdef USE_WIN32
	long framesize;
	SDL_Rect clip;
	Uint32 flags;
	int w;
	int h;
	int bpp;
	unsigned char *pixels = NULL;
	SDL_Color *palette = NULL;
	int ncolors = 0;

	if (!TheScreen) { // don't bother if there's no surface.
		return;
	}

	flags = TheScreen->flags;
	w = TheScreen->w;
	h = TheScreen->h;
	bpp = TheScreen->format->BitsPerPixel;

	if (!SDL_VideoModeOK(w, h, bpp,	flags ^ SDL_FULLSCREEN)) {
		return;
	}

	SDL_GetClipRect(TheScreen, &clip);

	// save the contents of the screen.
	framesize = w * h * TheScreen->format->BytesPerPixel;

	if (!(pixels = new unsigned char[framesize])) { // out of memory
		return;
	}
	SDL_LockSurface(TheScreen);
	memcpy(pixels, TheScreen->pixels, framesize);

	if (TheScreen->format->palette) {
		ncolors = TheScreen->format->palette->ncolors;
		if (!(palette = new SDL_Color[ncolors])) {
			delete[] pixels;
			return;
		}
		memcpy(palette, TheScreen->format->palette->colors,
			ncolors * sizeof(SDL_Color));
	}
	SDL_UnlockSurface(TheScreen);


	TheScreen = SDL_SetVideoMode(w, h, bpp, flags ^ SDL_FULLSCREEN);
	if (!TheScreen) {
		TheScreen = SDL_SetVideoMode(w, h, bpp, flags);
		if (!TheScreen) { // completely screwed.
			delete[] pixels;
			delete[] palette;
			fprintf(stderr, "Toggle to fullscreen, crashed all\n");
			Exit(-1);
		}
	}

	// Windows shows the SDL cursor when starting in fullscreen mode
	// then switching to window mode.  This hides the cursor again.
	SDL_ShowCursor(SDL_ENABLE);
	SDL_ShowCursor(SDL_DISABLE);

	SDL_LockSurface(TheScreen);
	memcpy(TheScreen->pixels, pixels, framesize);
	delete[] pixels;

	if (TheScreen->format->palette) {
		// !!! FIXME : No idea if that flags param is right.
		SDL_SetPalette(TheScreen, SDL_LOGPAL, palette, 0, ncolors);
		delete[] palette;
	}
	SDL_UnlockSurface(TheScreen);

	SDL_SetClipRect(TheScreen, &clip);

	Invalidate(); // Update display
#else // !USE_WIN32
	SDL_SetWindowFullscreen(TheWindow, flags ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif

	Video.FullScreen = (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) ? 0 : 1;
}

//@}
