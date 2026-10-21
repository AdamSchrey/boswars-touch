--     ____                _       __
--    / __ )____  _____   | |     / /___ ___________
--   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
--  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
-- /_____/\____/____/     |__/|__/\__,_/_/  /____/
--
--       A futuristic real-time strategy game.
--          This file is part of Bos Wars.
--
--      campaigns.lua - The main UI lua script.
--
--      (c) Copyright 2006-2010 by François Beerten
--
--      This program is free software; you can redistribute it and/or modify
--      it under the terms of the GNU General Public License as published by
--      the Free Software Foundation; only version 2 of the License.
--
--      This program is distributed in the hope that it will be useful,
--      but WITHOUT ANY WARRANTY; without even the implied warranty of
--      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--      GNU General Public License for more details.
--
--      You should have received a copy of the GNU General Public License
--      along with this program; if not, write to the Free Software
--      Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
--      02111-1307, USA.
--


function RunBriefingMenu(objectivestext, briefingtext, briefingsound, background)
  local menu
  local b
  local channel = nil

  if (objectivestext == nil) then
    objectivestext = default_objectives
  end
  SetObjectives(objectivestext)

  local objText = ""
  for i,f in ipairs(objectivestext) do
    objText = objText .. f .. "\n"
  end

  menu = BosMenu(_("Briefing"), background)
  local text = briefingtext ..
      "\n\n" ..
      _("Objectives:") ..
      "         \n" ..
      objText
  local t= MultiLineLabel(text)
  t:setFont(Fonts["large"])
  t:setAlignment(MultiLineLabel.LEFT)
  t:setVerticalAlignment(MultiLineLabel.CENTER)
  t:setLineWidth(400)
  t:adjustSize()
  t:setHeight(Video.Height * 12 / 20)
  t:setBackgroundColor(dark)
  menu:add(t, Video.Width / 2 - 200, Video.Height / 20 * 3)

  menu:addButton(_("~!Start"), Video.Width / 2 - 100, Video.Height - 100,
    function()
      if (channel ~= nil) then StopChannel(channel) end
      menu:stop()
    end)
  menu:addButton(_("Cancel (~<Esc~>)"), Video.Width / 2 - 100, Video.Height - 65,
    function()
      if (channel ~= nil) then StopChannel(channel) end
      menu:stop(1)
    end)

  if (briefingsound ~= nil) then
    channel = PlaySoundFile(briefingsound, function() channel = nil end)
  end

  return menu:run()
end

function AddCampaignMessage(when, text)
  AddTrigger(
    function() return (GameCycle() >= when) end,
    function() return AddMessage(text) end
  )
end

function AddCampaignFinalAssault(when, text)
  AddTrigger(
    function() return (GameCycle() >= when) end,
    function()
      AddMessage(text)
      CreateUnit("unit-vault", 1, {60, 8})
      CreateUnit("unit-tank", 1, {59, 12})
      CreateUnit("unit-tank", 1, {61, 12})
      CreateUnit("unit-tank", 1, {59, 14})
      CreateUnit("unit-tank", 1, {60, 14})
      AiForce(9, {"unit-tank", 4, "unit-rtank", 3})
      AiAttackWithForce(9)
    end
  )
end


function CreateMapStep(map, objectivestext, briefingtext, briefingsound, briefingbackground)
  function RunCampaignMap()
    if (RunBriefingMenu(objectivestext, briefingtext, briefingsound, briefingbackground) ~= 0) then
      GameResult = GameQuitToMenu
      return
    end
    Load(map) -- Needed to force the load of the presentation
    RunMap(map, objectivestext)
  end
  return RunCampaignMap
end


function RunCampaign(campaign)
  Load(campaign)
  currentCampaign = campaign
  -- Campaign results must never be recorded as standalone map wins.
  SkirmishMap = nil
  -- Campaigns have no difficulty selector; they play at the map
  -- default.  Store it in GameDifficulty so a campaign saved game
  -- that is loaded after a restart restores the same AI pacing.
  GameDifficulty = GameSettings.Difficulty or 3
  if position == nil then
    position = 1
  end
  while position <= table.getn(campaign_steps) do
    -- The ending step is not a playable level: show it but never
    -- count it as campaign progress (the List of Campaigns shows
    -- [won/total] where total excludes the ending step).
    if (campaign_steps[position] == DisplayEnding) then
      campaign_steps[position]()
      position = position + 1
    else
      campaign_steps[position]()
      if GameResult == GameVictory then
        -- Record the campaign progress (highest level won) so the
        -- List of Campaigns can show how far the player got.
        local progress = preferences.CampaignProgress or {}
        local campaignName = string.gsub(string.gsub(campaign, "^campaigns/", ""),
                                        "/campaign.lua$", "")
        if ((progress[campaignName] or 0) < position) then
          progress[campaignName] = position
          preferences.CampaignProgress = progress
          SavePreferences()
        end
        position = position + 1
      elseif GameResult == GameDefeat then
        position = position
      else
        currentCampaign = nil
        return
      end
    end
  end
  currentCampaign = nil
end


function RunCampaignsMenu(s)
  local menu
  local b

  menu = BosMenu(_("List of Campaigns"))

  ResetMapOptions()
  -- List the campaigns with their progress ("[2/5]" = levels won /
  -- total levels).  The progress is stored in the preferences as the
  -- highest level number won; loading the campaign.lua to count the
  -- levels is safe because it only builds the campaign_steps table.
  local function listCampaigns(path)
    local result = {}
    for i, dir in ipairs(ListDirsInDirectory(path)) do
      if (string.find(dir, "^%a")) then
        local name = string.gsub(dir, "/$", "")
        local total = 0
        local loaded = pcall(Load, path .. name .. "/campaign.lua")
        if (loaded and campaign_steps ~= nil) then
          total = table.getn(campaign_steps)
          -- The ending step is not a playable level.
          if (total > 0 and campaign_steps[total] == DisplayEnding) then
            total = total - 1
          end
        end
        campaign_steps = nil
        local progress = preferences.CampaignProgress and
          preferences.CampaignProgress[name]
        local label = name
        if (total > 0) then
          -- Clamp to the total: older versions could record the
          -- non-playable ending step as progress (e.g. "[6/5]").
          if (progress ~= nil and progress > total) then
            progress = total
          end
          label = label .. " [" .. (progress or 0) .. "/" .. total .. "]"
        elseif (progress) then
          label = label .. " [" .. progress .. "]"
        end
        table.insert(result, label)
      end
    end
    return result
  end
  local browser = menu:addBrowser("campaigns/", listCampaigns,
                                 Video.Width / 2 - 150, 100, 300, 200)
  function startgamebutton(s)
    DebugPrint("Starting campaign")
    position = nil
    local selected = string.gsub(browser:getSelectedItem(), " ?%[.*%]$", "")
    RunCampaign("campaigns/" .. selected .. "/campaign.lua")
    menu:stop()
  end
  menu:addButton(_("Main Menu (~<Esc~>)"), Video.Width / 2 - 250, Video.Height - 100,
                 function() menu:stop() end)
  menu:addButton(_("~!Start"), Video.Width / 2 + 50 ,  Video.Height - 100,
                 startgamebutton)

  menu:run()
end

