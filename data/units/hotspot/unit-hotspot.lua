--       _________ __                 __                               
--      /   _____//  |_____________ _/  |______     ____  __ __  ______
--      \_____  \\   __\_  __ \__  \\   __\__  \   / ___\|  |  \/  ___/
--      /        \|  |  |  | \// __ \|  |  / __ \_/ /_/  >  |  /\___ \ 
--     /_______  /|__|  |__|  (____  /__| (____  /\___  /|____//____  >
--             \/                  \/          \//_____/            \/ 
--  ______________________                           ______________________
--			  T H E   W A R   B E G I N S
--	   Stratagus - A free fantasy real time strategy game engine
--
--	unit-hotspot.lua	-	Define the hot spot unit.
--
--	(c) Copyright 2007 by Francois Beerten.
--
--      This program is free software; you can redistribute it and/or modify
--      it under the terms of the GNU General Public License as published by
--      the Free Software Foundation; either version 2 of the License, or
--      (at your option) any later version.
--  
--      This program is distributed in the hope that it will be useful,
--      but WITHOUT ANY WARRANTY; without even the implied warranty of
--      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--      GNU General Public License for more details.
--  
--      You should have received a copy of the GNU General Public License
--      along with this program; if not, write to the Free Software
--      Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
--
--	$Id: unit-tree02.lua 8696 2007-04-07 18:14:35Z feb $

DefineAnimations("animations-hotspot", {
    Still = {"frame 0", "wait 10", "frame 1", "wait 10", "frame 2", "wait 10",
        "frame 3", "wait 10", "frame 4", "wait 10", "frame 5", "wait 10", 
        "frame 6", "wait 10", "frame 7", "wait 10", "frame 8", "wait 10", 
        "frame 9", "wait 10", "frame 10", "wait 10", "frame 11", "wait 10", 
        "frame 12", "wait 10", "frame 13", "wait 10", "frame 14", "wait 10", 
        "frame 15", "wait 10",
        "frame 14", "wait 10", "frame 13", "wait 10", "frame 12", "wait 10", "frame 11", "wait 10",
        "frame 10", "wait 10", "frame 9", "wait 10", "frame 8", "wait 10", "frame 7", "wait 10", "frame 6", "wait 10",
		"frame 5", "wait 10", "frame 4", "wait 10", "frame 3", "wait 10", "frame 2", "wait 10", "frame 1", "wait 10",
	},
    Death = {"unbreakable begin", "frame 0", "unbreakable end", "wait 1", },
})

DefineIcon({
        Name = "icon-hotspot",
        Size = {46, 38},
        Frame = 0,
        File = "units/hotspot/hotspot_i.png"})

DefineConstruction("construction-hotspot", {
        Constructions = {
                {Percent = 0, File = "main", Frame = 0},
        }
})

DefineUnitType("unit-hotspot", {
    Name = "Hot Spot",
    Image = {"file", "units/hotspot/hotspot.png", "size", {64, 64}},
    Offset = {0, 0},
    Animations = "animations-hotspot",
    Icon = "icon-hotspot",
    Construction = "construction-hotspot",
    HitPoints = 1,
    Indestructible = 1,
    DrawLevel = 2,
    TileSize = {2, 2},
    BoxSize = {64, 64},
    NeutralMinimapColor = {250, 29, 209},
    SightRange = 0,
    Armor = 0,
    BasicDamage = 0,
    PiercingDamage = 0,
    Missile = "missile-none",
    Priority = 0,
    AnnoyComputerFactor = 0,
    Points = 0,
    ExplodeWhenKilled = "missile-64x64-explosion",
    Corpse = "unit-destroyed-2x2-place",
    Type = "land",
    Building = true,
    VisibleUnderFog = true,
    Neutral = true,
    NumDirections = 1,
    Sounds = {}
})

DefineAllow("unit-hotspot", AllowAll)






