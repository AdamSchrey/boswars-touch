//     ____                _       __
//    / __ )____  _____   | |     / /___ ___________
//   / __  / __ \/ ___/   | | /| / / __ `/ ___/ ___/
//  / /_/ / /_/ (__  )    | |/ |/ / /_/ / /  (__  )
// /_____/\____/____/     |__/|__/\__,_/_/  /____/
//
//       A futuristic real-time strategy game.
//          This file is part of Bos Wars.
//
//      ingame_overlay.cpp - Touch overlay shown during a game: camera
//                            move buttons, a chat toggle button and an
//                            on-screen keyboard for chat input.
//
//      (c) Copyright 2026 by Adam Schrey
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
#include "interface.h"

#include "guichan.h"
#include "widgets.h"
#include "video.h"
#include "script.h"

// SDL key codes are needed for chat keys (Enter/Backspace) that the
// on-screen keyboard feeds via InputKey().
#include "SDL.h"

#include <string>
#include <vector>

// The same colors used by the Lua menu button helper (addButton in
// guichan.lua) so the touch overlay matches the in-game menu buttons:
//   dark     = Color(38, 38, 78, 128)
//   clear    = Color(200, 200, 128)
//   disabled = Color(112, 112, 112, 128)
static const gcn::Color OskDark(38, 38, 78, 128);
static const gcn::Color OskClear(200, 200, 128);
static const gcn::Color OskDisabled(112, 112, 112, 128);

// Apply the menu button style to a ButtonWidget.
static void ApplyMenuButtonStyle(ButtonWidget *b)
{
	b->setBaseColor(OskDark);
	b->setBackgroundColor(OskDark);
	b->setDisabledColor(OskDisabled);
}

/*----------------------------------------------------------------------------
--  Camera scroll button
----------------------------------------------------------------------------*/

class CameraScrollButton : public ButtonWidget
{
public:
	CameraScrollButton(const std::string &caption, int scrollMask) :
		ButtonWidget(caption), ScrollMask(scrollMask)
	{
	}

	// While the button is held down, set the scroll state so that the
	// main loop's DoScrollArea() continuously moves the camera.  When
	// released, the scroll stops.  A short tap still scrolls for one
	// frame, which gives the single-step behavior.
	virtual void mousePress(int x, int y, int button)
	{
		ButtonWidget::mousePress(x, y, button);
		if (GameRunning) {
			KeyScrollState |= ScrollMask;
		}
	}

	virtual void mouseRelease(int x, int y, int button)
	{
		ButtonWidget::mouseRelease(x, y, button);
		if (GameRunning) {
			KeyScrollState &= ~ScrollMask;
		}
	}

private:
	int ScrollMask;
};

/*----------------------------------------------------------------------------
--  On-screen keyboard buttons for chat
----------------------------------------------------------------------------*/

// Key for a character button: feed the byte value to the engine input.
class CharKeyListener : public gcn::ActionListener
{
public:
	CharKeyListener(const std::string &ch) : Char(ch) {}
	virtual void action(const std::string &)
	{
		if (Char.size() == 1) {
			InputKey((int)(unsigned char)Char[0]);
		}
	}
private:
	std::string Char;
};

// Backspace key.
class BackspaceListener : public gcn::ActionListener
{
public:
	virtual void action(const std::string &)
	{
		InputKey((int)SDLK_BACKSPACE);
	}
};

// Enter key: commit the chat message.
class EnterListener : public gcn::ActionListener
{
public:
	virtual void action(const std::string &)
	{
		InputKey((int)SDLK_RETURN);
	}
};

// Space key.
class SpaceListener : public gcn::ActionListener
{
public:
	virtual void action(const std::string &)
	{
		InputKey((int)' ');
	}
};

/*----------------------------------------------------------------------------
--  Chat toggle button + on-screen keyboard container
----------------------------------------------------------------------------*/

// Build a single styled key button of the given size and add it to parent
// at (x, y).  Returns the created button.
static ButtonWidget *MakeKey(gcn::Container *parent, const std::string &caption,
	int w, int h, int x, int y, gcn::ActionListener *listener)
{
	ButtonWidget *b = new ButtonWidget(caption);
	b->setSize(w, h);
	ApplyMenuButtonStyle(b);
	if (listener) {
		b->addActionListener(listener);
	}
	parent->add(b, x, y);
	return b;
}

// Build the on-screen keyboard for chat and add it to the container.
// Initially hidden; visibility is toggled by the chat button.  A pointer
// to the keyboard container is stored so it can be shown/hidden.
static gcn::Container *MakeChatKeyboard(gcn::Container *parent)
{
	const int kw = 26;
	const int kh = 26;
	const int gap = 2;
	const int step = kw + gap;
	const int stepY = kh + gap;

	gcn::Container *kb = new gcn::Container();
	kb->setOpaque(false);
	// 10 columns, 6 rows (3 letters + digits + symbols + controls).
	kb->setDimension(gcn::Rectangle(0, 0, 10 * step - gap, 6 * stepY - gap));
	kb->setVisible(false);

	// Letter rows.
	static const char *letters[3][10] = {
		{"a","b","c","d","e","f","g","h","i","j"},
		{"k","l","m","n","o","p","q","r","s","t"},
		{"u","v","w","x","y","z",""}
	};
	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 10 && letters[row][col][0]; ++col) {
			std::string ch = letters[row][col];
			MakeKey(kb, ch, kw, kh, col * step, row * stepY,
				new CharKeyListener(ch));
		}
	}
	// Digit row.
	const char *digits[10] = {"0","1","2","3","4","5","6","7","8","9"};
	for (int col = 0; col < 10; ++col) {
		std::string ch = digits[col];
		MakeKey(kb, ch, kw, kh, col * step, 3 * stepY,
			new CharKeyListener(ch));
	}
	// Symbol row.
	const char *syms[5] = {".","-","_",":","/"};
	for (int col = 0; col < 5; ++col) {
		std::string ch = syms[col];
		MakeKey(kb, ch, kw, kh, col * step, 4 * stepY,
			new CharKeyListener(ch));
	}

	// Control row: backspace, space (wide), enter (wide).
	int bottomY = 5 * stepY;
	MakeKey(kb, "<-", kw + 12, kh, 0, bottomY, new BackspaceListener());
	MakeKey(kb, "Space", kw * 3 + gap * 2, kh, step * 1, bottomY,
		new SpaceListener());
	MakeKey(kb, "\xE2\x8F\x8E", kw * 3 + gap * 2, kh, step * 7, bottomY,
		new EnterListener());

	parent->add(kb, 2, 2);
	return kb;
}

class ChatToggleListener : public gcn::ActionListener
{
public:
	ChatToggleListener(gcn::Container *keyboard) : Keyboard(keyboard) {}
	virtual void action(const std::string &)
	{
		if (Keyboard == NULL) {
			return;
		}
		bool show = !Keyboard->isVisible();
		Keyboard->setVisible(show);
		if (show) {
			// Enter chat/input mode so typed characters reach the input.
			UiBeginInput();
		}
	}
private:
	gcn::Container *Keyboard;
};

/*----------------------------------------------------------------------------
--  Overlay creation
----------------------------------------------------------------------------*/

static const int OverlayBtnSize = 32;
static const int OverlayGap = 2;

// Create the touch overlay on the in-game GUI container.
// The overlay is owned by the container (added as a child), so the caller
// does not need to free it.
void CreateIngameTouchOverlay(gcn::Container *container)
{
	if (container == NULL) {
		return;
	}

	// On-screen keyboard, initially hidden, top-left.
	gcn::Container *keyboard = MakeChatKeyboard(container);

	// Camera + chat buttons at the bottom-left, just above the bottom bar.
	const int rowY = Video.Height - OverlayBtnSize - 2;
	int x = 2;

	CameraScrollButton *leftBtn = new CameraScrollButton("\xE2\x86\x90", ScrollLeft);
	leftBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(leftBtn);
	container->add(leftBtn, x, rowY);
	x += OverlayBtnSize + OverlayGap;

	CameraScrollButton *downBtn = new CameraScrollButton("\xE2\x86\x93", ScrollDown);
	downBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(downBtn);
	container->add(downBtn, x, rowY);
	x += OverlayBtnSize + OverlayGap;

	CameraScrollButton *upBtn = new CameraScrollButton("\xE2\x86\x91", ScrollUp);
	upBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(upBtn);
	container->add(upBtn, x, rowY);
	x += OverlayBtnSize + OverlayGap;

	CameraScrollButton *rightBtn = new CameraScrollButton("\xE2\x86\x92", ScrollRight);
	rightBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(rightBtn);
	container->add(rightBtn, x, rowY);
	x += OverlayBtnSize + OverlayGap * 4;

	ButtonWidget *chatBtn = new ButtonWidget("\xE2\x9C\x89");
	chatBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(chatBtn);
	chatBtn->addActionListener(new ChatToggleListener(keyboard));
	container->add(chatBtn, x, rowY);
}

//@}
