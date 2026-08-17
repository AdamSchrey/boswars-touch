--     ____                _       __
--    / __ )____  _____   | |     / /___ ___________
--   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
--  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
-- /_____/\____/____/     |__/|__/\__,_/_/  /____/
--
--       A futuristic real-time strategy game.
--          This file is part of Bos Wars.
--
--      keyboard.lua - On-screen keyboard for text input fields.
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

-- On-screen keyboard shown next to text input fields on touch devices.
--
-- Provides all alphabet letters (upper and lower case, toggled with a
-- shift key), backspace and the characters ".", "-" and "_".  No space
-- or other special characters are included by design.
-- Styled like the other menu buttons: transparent blue base color
-- (dark) with a yellow foreground (clear).

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
-- Extra characters on the bottom row.
OSKExtraKeys = {".", "-", "_"}

-- State of the on-screen keyboard: the text field it edits and whether
-- letters are currently upper case.
OSKTargetField = nil
OSKUpperCase = false
-- References to the letter buttons so their captions can be refreshed
-- when the shift state changes.
OSKLetterButtons = {}

-- Append a character to the target text field, keeping the focus.
function OSKTypeChar(ch)
  if OSKTargetField == nil then
    return
  end
  OSKTargetField:setText(OSKTargetField:getText() .. ch)
  OSKTargetField:requestFocus()
end

-- Remove the last character from the target text field.
function OSKBackspace()
  if OSKTargetField == nil then
    return
  end
  local text = OSKTargetField:getText()
  -- The allowed input characters are all ASCII, so removing the last
  -- byte is correct for our input set.
  local stripped = string.gsub(text, ".$", "")
  OSKTargetField:setText(stripped)
  OSKTargetField:requestFocus()
end

-- Toggle between lower and upper case letters and refresh the visible
-- captions of all letter buttons.
function OSKToggleShift()
  OSKUpperCase = not OSKUpperCase
  local caption = OSKUpperCase
  for _, btn in ipairs(OSKLetterButtons) do
    local lower = btn._oskLetter
    if lower ~= nil then
      if OSKUpperCase then
        btn:setCaption(string.upper(lower))
      else
        btn:setCaption(lower)
      end
    end
  end
end

-- Helper to build a single styled key button.
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

-- Build the on-screen keyboard anchored to the given text field and
-- add it to the menu.  The keyboard is placed to the right of the
-- field; if it would not fit on the screen it is moved to the left of
-- the field, and if that still does not fit it is placed below the
-- field.
--
-- The target field is updated so key presses affect the most recently
-- created text field.  This keeps the keyboard working in every menu
-- that uses menu:addTextInputField().
function AddOnScreenKeyboard(menu, textField, fieldX, fieldY, fieldW)
  if textField == nil then
    return
  end

  local step = OSKKeyWidth + OSKKeyGap
  local stepY = OSKKeyHeight + OSKKeyGap

  -- Total keyboard dimensions: 10 columns (longest letter row) and
  -- 4 rows (3 letter rows + bottom row with shift/extra/backspace).
  local kbWidth = 10 * step - OSKKeyGap
  local kbHeight = 4 * stepY - OSKKeyGap

  -- Prefer placing the keyboard to the right of the field.
  local kbX = fieldX + fieldW + 8
  if kbX + kbWidth > Video.Width then
    -- Not enough room on the right: try to the left of the field.
    kbX = fieldX - kbWidth - 8
  end
  if kbX < 0 then
    -- Still no room: place it below the field, horizontally centered.
    kbX = math.max(0, math.floor((Video.Width - kbWidth) / 2))
    kbY = fieldY + OSKKeyHeight + 8
  else
    kbY = fieldY
  end
  -- Keep it fully on screen vertically.
  if kbY + kbHeight > Video.Height then
    kbY = math.max(0, Video.Height - kbHeight - 8)
  end

  -- Remember which text field the keyboard targets.
  OSKTargetField = textField
  OSKLetterButtons = {}

  local row, col
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

  -- Bottom row: shift toggle, extra characters and a wider backspace.
  local bottomRowY = kbY + 3 * stepY

  local shiftBtn = oskKeyButton("^", function() OSKToggleShift() end)
  menu:add(shiftBtn, kbX, bottomRowY)

  local colIndex = 2
  for _, key in ipairs(OSKExtraKeys) do
    local b = oskKeyButton(key,
      function()
        OSKTypeChar(key)
      end)
    menu:add(b, kbX + (colIndex - 1) * step, bottomRowY)
    colIndex = colIndex + 1
  end

  -- Backspace, slightly wider so it is easy to hit.
  local bsWidth = OSKKeyWidth + 16
  local bs = ButtonWidget("<-")
  bs:setActionCallback(function() OSKBackspace() end)
  bs:setSize(bsWidth, OSKKeyHeight)
  bs:setBackgroundColor(dark)
  bs:setBaseColor(dark)
  bs:setForegroundColor(clear)
  bs:setDisabledColor(disabled)
  menu:add(bs, kbX + (colIndex - 1) * step, bottomRowY)
end
