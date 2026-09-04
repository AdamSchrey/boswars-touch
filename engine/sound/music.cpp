//     ____                _       __
//    / __ )____  _____   | |     / /___ ___________
//   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
//  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
// /_____/\____/____/     |__/|__/\__,_/_/  /____/
//
//       A futuristic real-time strategy game.
//          This file is part of Bos Wars.
//
/**@name music.cpp - Background music support */
//
//      (c) Copyright 2002-2008 by Lutz Sammer, Nehal Mistry, and Jimmy Salmon
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
#include <stdio.h>
#include "SDL.h"
#include "sound_server.h"
#include "script.h"


bool CallbackMusic;                       /// flag true callback ccl if stops


/**
**  Check if music is finished and play the next song
*/
void CheckMusicFinished(bool force)
{
	/* IsMusicPlaying() relies on a variable which is changed in another
	 * thread. In theory, that could cause race conditions. The test
	 * checks if the music music stopped. It cannot start at the same
	 * time as starting the music always happens in this thread. */
	if (IsMusicPlaying())
		return;

	if (SoundEnabled() && IsMusicEnabled() && CallbackMusic) {
		// printf("Music stopped\n");
		lua_pushstring(Lua, "MusicStopped");
		lua_gettable(Lua, LUA_GLOBALSINDEX);
		if (!lua_isfunction(Lua, -1)) {
			fprintf(stderr, "No MusicStopped function in Lua\n");
			StopMusic();
		} else {
			LuaCall(0, 1);
		}
	}
}

/**
**  Init music
*/
void InitMusic(void)
{
}

//@}
