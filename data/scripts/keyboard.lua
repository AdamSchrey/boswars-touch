--     ____                _       __
--    / __ )____  _____   | |     / /___ ___________
--   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
--  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
-- /_____/\____/____/     |__/|__/\__,_/_/  /____/
--
--       A futuristic real-time strategy game.
--          This file is part of Bos Wars.
--
--      keyboard.lua - On-screen keyboard for text input fields and chat.
--
--      (c) Copyright 2026 by Adam Schrey
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

-- On-screen keyboard for touch devices.
--
-- It supports two modes:
--   * TextField mode: types into a guichan TextField widget that has the
--     focus.  Used by every menu text field via addTextInputField().
--   * Chat mode: types into the engine's in-game input buffer (the same
--     buffer that Enter-to-chat uses) via the InputBegin/InputKey bindings.
--
-- Keys provided:
--   * Letters A-Z (upper/lower case, toggled with a shift key that also
--     updates the visible captions).
--   * Digits 0-9 (for IP addresses and server names).
--   * Space, backspace, ".", "-", "_", ":" and "/" (for host names/URLs).
--   * Enter ("⏎") to commit chat messages.
-- No other special characters are included.
--
-- The keyboard is styled like the other menu buttons: transparent blue
-- base color (dark) with a yellow foreground (clear).

-- Layout constants
OSKKeyWidth = 24
OSKKeyHeight = 24
OSKKeyGap = 2

-- Letter rows of the keyboard (lowercase by default).
OSKRows = {
  {"a","b","c","d","e","f","g","h","i","j"},
  {"k","l","m","n","o","p","q","r","s","t"},
  {"u","v","w","x","y","z"},
}
-- Digit row and extra characters.
OSKDigits = {"0","1","2","3","4","5","6","7","8","9"}
OSKExtraKeys = {".", "-", "_", ":", "/"}

-- State of the on-screen keyboard.
OSKTargetField = nil          -- the currently focused TextField (TextField mode)
OSKChatMode = false           -- true when typing into the chat input buffer
OSKUpperCase = false
OSKLetterButtons = {}         -- letter buttons, refreshed on shift toggle

-- Return the caption for a letter key, honoring the current case state.
local function oskLetterCaption(letter)
  if OSKUpperCase then
    return string.upper(letter)
  end
  return letter
end

-- Type a single character into the current target.
function OSKTypeChar(ch)
  if OSKChatMode then
    -- The engine input accepts characters >= ' ' via their byte value.
    InputKey(string.byte(ch))
  elseif OSKTargetField ~= nil then
    OSKTargetField:setText(OSKTargetField:getText() .. ch)
    OSKTargetField:requestFocus()
  end
end

-- Remove the last character from the current target.
function OSKBackspace()
  if OSKChatMode then
    InputKey(string.byte("\b"))  -- backspace
  elseif OSKTargetField ~= nil then
    local text = OSKTargetField:getText()
    -- All allowed input characters are ASCII, so removing the last byte
    -- is correct for our input set.
    local stripped = string.gsub(text, ".$", "")
    OSKTargetField:setText(stripped)
    OSKTargetField:requestFocus()
  end
end

-- Commit the current input (Enter).  Only meaningful in chat mode; in
-- TextField mode this is a no-op.
function OSKEnter()
  if OSKChatMode then
    InputKey(string.byte("\r"))  -- return / enter
  end
end

-- Toggle between lower and upper case letters and refresh the visible
-- captions of all letter buttons.
function OSKToggleShift()
  OSKUpperCase = not OSKUpperCase
  for _, btn in ipairs(OSKLetterButtons) do
    local lower = btn._oskLetter
    if lower ~= nil then
      btn:setCaption(oskLetterCaption(lower))
    end
  end
end

-- Mark the given text field as the focus target and leave chat mode.
-- Called whenever a TextField gains focus (e.g. by tapping it).
function OSKSetFocus(textField)
  OSKTargetField = textField
  OSKChatMode = false
end

-- Enter chat mode: switch the keyboard target to the engine input buffer.
function OSKEnterChatMode()
  OSKChatMode = true
  OSKTargetField = nil
  InputBegin()
end

-- Build a single styled key button.
local function oskKeyButton(caption, callback)
  local b = ButtonWidget(caption)
  b:setActionCallback(callback)
  b:setSize(OSKKeyWidth, OSKKeyHeight)
  b:setBackgroundColor(dark)
  b:setBaseColor(dark)
  b:setForegroundColor(clear)
  b:setDisabledColor(disabled)
  return b
end

-- Place a wide key spanning several columns and add it to the menu.
local function oskWideKey(menu, caption, width, callback, x, y)
  local b = ButtonWidget(caption)
  b:setActionCallback(callback)
  b:setSize(width, OSKKeyHeight)
  b:setBackgroundColor(dark)
  b:setBaseColor(dark)
  b:setForegroundColor(clear)
  b:setDisabledColor(disabled)
  menu:add(b, x, y)
  return b
end

-- Compute the position of the keyboard, preferring right of the field,
-- then left, then below-centered.  Returns kbX, kbY.
local function oskComputePosition(fieldX, fieldY, fieldW, kbWidth, kbHeight)
  local kbX = fieldX + fieldW + 8
  if kbX + kbWidth > Video.Width then
    kbX = fieldX - kbWidth - 8
  end
  local kbY
  if kbX < 0 then
    kbX = math.max(0, math.floor((Video.Width - kbWidth) / 2))
    kbY = fieldY + OSKKeyHeight + 8
  else
    kbY = fieldY
  end
  if kbY + kbHeight > Video.Height then
    kbY = math.max(0, Video.Height - kbHeight - 8)
  end
  return kbX, kbY
end

-- Build the on-screen keyboard anchored to a TextField and add it to the
-- menu.  Called automatically by menu:addTextInputField().
function AddOnScreenKeyboard(menu, textField, fieldX, fieldY, fieldW)
  if textField == nil then
    return
  end

  local step = OSKKeyWidth + OSKKeyGap
  local stepY = OSKKeyHeight + OSKKeyGap

  -- Total keyboard dimensions: 10 columns, 6 rows
  -- (3 letter rows + digit row + extra row + bottom control row).
  local kbWidth = 10 * step - OSKKeyGap
  local kbHeight = 6 * stepY - OSKKeyGap

  local kbX, kbY = oskComputePosition(fieldX, fieldY, fieldW, kbWidth, kbHeight)

  OSKTargetField = textField
  OSKChatMode = false
  OSKLetterButtons = {}

  -- Letter rows
  for row = 1, #OSKRows do
    local letters = OSKRows[row]
    for col = 1, #letters do
      local letter = letters[col]
      local b = oskKeyButton(letter,
        function()
          local ch = letter
          if OSKUpperCase then
            ch = string.upper(letter)
          end
          OSKTypeChar(ch)
        end)
      b._oskLetter = letter
      table.insert(OSKLetterButtons, b)
      menu:add(b, kbX + (col - 1) * step, kbY + (row - 1) * stepY)
    end
  end

  -- Digit row (4th row)
  for col = 1, #OSKDigits do
    local d = OSKDigits[col]
    local b = oskKeyButton(d, function() OSKTypeChar(d) end)
    menu:add(b, kbX + (col - 1) * step, kbY + 3 * stepY)
  end

  -- Extra characters row (5th row)
  for col = 1, #OSKExtraKeys do
    local k = OSKExtraKeys[col]
    local b = oskKeyButton(k, function() OSKTypeChar(k) end)
    menu:add(b, kbX + (col - 1) * step, kbY + 4 * stepY)
  end

  -- Bottom control row: shift, space (wide), backspace, enter.
  local bottomY = kbY + 5 * stepY
  local shiftBtn = oskKeyButton("^", function() OSKToggleShift() end)
  menu:add(shiftBtn, kbX, bottomY)

  -- Space, wide, centered.
  local spaceWidth = OSKKeyWidth * 3 + OSKKeyGap * 2
  local spaceBtn = oskWideKey(menu, "Space", spaceWidth,
    function() OSKTypeChar(" ") end,
    kbX + 1 * step, bottomY)

  local backspaceBtn = oskWideKey(menu, "<-", OSKKeyWidth + 16,
    function() OSKBackspace() end,
    kbX + 5 * step, bottomY)

  -- Enter ("⏎"), wide.
  local enterWidth = OSKKeyWidth * 3 + OSKKeyGap * 2
  local enterBtn = oskWideKey(menu, "⏎", enterWidth,
    function() OSKEnter() end,
    kbX + 7 * step, bottomY)
end
