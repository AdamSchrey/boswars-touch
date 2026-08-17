--     ____                _       __
--    / __ )____  _____   | |     / /___ ___________
--   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
--  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
-- /_____/\____/____/     |__/|__/\__,_/_/  /____/
--
--       A futuristic real-time strategy game.
--          This file is part of Bos Wars.
--
--      keyboard.lua - On-screen keyboard for menu text fields.
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

-- On-screen keyboard for menu text fields (touch devices).
--
-- It types into a guichan TextField widget that has the focus, inserting
-- each character at the caret position (not always at the end).
--
-- Layout (top to bottom), matching the in-game chat keyboard:
--   row 1: digits 0-9
--   row 2: ".", "-", "_", ":", "/" (left aligned) ... "<-" (3 keys gap, 2 wide)
--   row 3-5: letters in QWERTY order
--   row 6: CAPS (3 wide) + Space (3 wide) + 1 key gap + Enter (wide)
--
-- Keys provided:
--   * Letters a-z (upper/lower case, toggled with CAPS).
--   * Digits 0-9 (for IP addresses and server names).
--   * Space, backspace, ".", "-", "_", ":" and "/" (for host names/URLs).
--
-- The keyboard is styled like the other menu buttons: transparent blue
-- base color (dark) with the disabled color.

-- Layout constants
OSKKeyWidth = 39
OSKKeyHeight = 39
OSKKeyGap = 3

-- Letter rows in QWERTY layout (lowercase by default).
OSKQwertyRows = {
  {"q","w","e","r","t","y","u","i","o","p"},
  {"a","s","d","f","g","h","j","k","l"},
  {"z","x","c","v","b","n","m"},
}
OSKDigits = {"0","1","2","3","4","5","6","7","8","9"}
OSKExtraKeys = {".", "-", "_", ":", "/"}

-- State of the on-screen keyboard.
OSKTargetField = nil          -- the currently focused TextField
OSKUpperCase = false
OSKLetterButtons = {}         -- letter buttons, refreshed on CAPS toggle
OSKLetters = {}               -- lowercase letters, parallel to buttons

-- Return the caption for a letter key, honoring the current case state.
local function oskLetterCaption(letter)
  if OSKUpperCase then
    return string.upper(letter)
  end
  return letter
end

-- Insert a single character into the target field at the caret position.
function OSKTypeChar(ch)
  if OSKTargetField == nil then
    return
  end
  local text = OSKTargetField:getText()
  local caret = OSKTargetField:getCaretPosition()
  -- Caret is a byte index into the UTF-8 text.  All our characters are
  -- ASCII, so inserting at the byte index is correct.
  local before = string.sub(text, 1, caret)
  local after = string.sub(text, caret + 1)
  OSKTargetField:setText(before .. ch .. after)
  OSKTargetField:setCaretPosition(caret + string.len(ch))
  OSKTargetField:requestFocus()
end

-- Remove the character before the caret (backspace).
function OSKBackspace()
  if OSKTargetField == nil then
    return
  end
  local text = OSKTargetField:getText()
  local caret = OSKTargetField:getCaretPosition()
  if caret > 0 then
    -- All our characters are ASCII, so the previous byte is one char.
    local before = string.sub(text, 1, caret - 1)
    local after = string.sub(text, caret + 1)
    OSKTargetField:setText(before .. after)
    OSKTargetField:setCaretPosition(caret - 1)
    OSKTargetField:requestFocus()
  end
end

-- Toggle between lower and upper case letters and refresh the visible
-- captions of all letter buttons and the CAPS button itself.
OSKCapsButton = nil
function OSKToggleCaps()
  OSKUpperCase = not OSKUpperCase
  if OSKCapsButton ~= nil then
    OSKCapsButton:setCaption(OSKUpperCase and "caps" or "CAPS")
  end
  for i, btn in ipairs(OSKLetterButtons) do
    local lower = OSKLetters[i]
    if lower ~= nil then
      btn:setCaption(oskLetterCaption(lower))
    end
  end
end

-- Mark the given text field as the focus target.
-- Called whenever a TextField gains focus (e.g. by tapping it).
function OSKSetFocus(textField)
  OSKTargetField = textField
end

-- Build a single styled key button.
local function oskKeyButton(caption, callback)
  local b = ButtonWidget(caption)
  b:setActionCallback(callback)
  b:setSize(OSKKeyWidth, OSKKeyHeight)
  b:setBackgroundColor(dark)
  b:setBaseColor(dark)
  b:setDisabledColor(disabled)
  return b
end

-- Place a wide key spanning several columns and add it to a parent.
local function oskWideKey(parent, caption, width, callback, x, y)
  local b = ButtonWidget(caption)
  b:setActionCallback(callback)
  b:setSize(width, OSKKeyHeight)
  b:setBackgroundColor(dark)
  b:setBaseColor(dark)
  b:setDisabledColor(disabled)
  parent:add(b, x, y)
  return b
end

-- Build the on-screen keyboard anchored to a TextField and add it to the
-- menu.  Called automatically by menu:addTextInputField().
--
-- The keyboard is placed in a screen-filling transparent container that is
-- added to the menu and raised to the top.  This avoids the menu clipping
-- the keyboard to its own (small) area, which previously left most keys
-- unrendered and unable to receive touch events.
function AddOnScreenKeyboard(menu, textField, fieldX, fieldY, fieldW)
  if textField == nil then
    return
  end

  local step = OSKKeyWidth + OSKKeyGap
  local stepY = OSKKeyHeight + OSKKeyGap

  -- Total keyboard dimensions: 10 columns, 6 rows.
  local kbWidth = 10 * step - OSKKeyGap
  local kbHeight = 6 * stepY - OSKKeyGap

  -- Absolute field position on screen (menu is offset on screen).
  local menuX = menu:getX()
  local menuY = menu:getY()
  local absFieldX = menuX + fieldX
  local absFieldY = menuY + fieldY

  -- Position: prefer right of the field, then left, then centered below.
  local kbX = absFieldX + fieldW + 8
  if kbX + kbWidth > Video.Width then
    kbX = absFieldX - kbWidth - 8
  end
  local kbY
  if kbX < 0 then
    kbX = math.max(0, math.floor((Video.Width - kbWidth) / 2))
    kbY = absFieldY + OSKKeyHeight + 8
  else
    kbY = absFieldY
  end
  if kbY + kbHeight > Video.Height then
    kbY = math.max(0, Video.Height - kbHeight - 8)
  end

  OSKTargetField = textField
  OSKLetterButtons = {}
  OSKLetters = {}
  OSKUpperCase = false

  -- Screen-filling transparent layer so the keyboard is not clipped to
  -- the menu's own (small) rectangle and always receives touch events.
  local layer = Container()
  layer:setOpaque(false)
  layer:setSize(Video.Width, Video.Height)

  -- Dedicated container holding all keys (relative coordinates from 0,0).
  local kb = Container()
  kb:setOpaque(false)
  kb:setSize(kbWidth, kbHeight)

  -- Row 1: digits 0-9.
  for col = 1, #OSKDigits do
    local d = OSKDigits[col]
    local b = oskKeyButton(d, function() OSKTypeChar(d) end)
    kb:add(b, (col - 1) * step, 0 * stepY)
  end

  -- Row 2: 5 symbols left-aligned, then "<-" with a 3-key gap, 2 wide.
  for col = 1, #OSKExtraKeys do
    local k = OSKExtraKeys[col]
    local b = oskKeyButton(k, function() OSKTypeChar(k) end)
    kb:add(b, (col - 1) * step, 1 * stepY)
  end
  oskWideKey(kb, "<-", OSKKeyWidth * 2 + OSKKeyGap,
    function() OSKBackspace() end,
    (5 + 3) * step, 1 * stepY)

  -- Rows 3-5: letters in QWERTY order.
  for row = 1, #OSKQwertyRows do
    local letters = OSKQwertyRows[row]
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
      table.insert(OSKLetterButtons, b)
      table.insert(OSKLetters, letter)
      kb:add(b, (col - 1) * step, (row + 1) * stepY)
    end
  end

  -- Row 6: CAPS (3 wide) + Space (3 wide) + 1 key gap + Enter (wide).
  local controlY = 5 * stepY
  local capsWidth = OSKKeyWidth * 3 + OSKKeyGap * 2
  OSKCapsButton = oskWideKey(kb, "CAPS", capsWidth,
    function() OSKToggleCaps() end,
    0 * step, controlY)

  local spaceWidth = OSKKeyWidth * 3 + OSKKeyGap * 2
  oskWideKey(kb, "Space", spaceWidth,
    function() OSKTypeChar(" ") end,
    3 * step, controlY)

  local enterWidth = OSKKeyWidth * 3 + OSKKeyGap * 2
  oskWideKey(kb, "Enter", enterWidth,
    function() end,
    7 * step, controlY)

  -- Place the keyboard inside the screen-filling layer at the computed
  -- screen coordinates, then add the layer to the menu and raise it above
  -- all other widgets so touch events always reach the keys.  The layer is
  -- added directly to the menu screen (not the visible panel) so it is not
  -- clipped to the panel's small rectangle.
  layer:add(kb, kbX, kbY)
  if menu.addOrig ~= nil then
    menu.addOrig(menu, layer, 0, 0)
  else
    menu:add(layer, 0, 0)
  end
  menu:moveToTop(layer)
end
