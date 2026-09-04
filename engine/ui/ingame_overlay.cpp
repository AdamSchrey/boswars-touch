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
#include "SDL.h"
#include "editor.h"
#include "interface.h"

#include "guichan.h"
#include "widgets.h"
#include "video.h"
#include "script.h"

// SDL key codes are needed for chat keys (Enter/Backspace) that the
// on-screen keyboard feeds via InputKey().

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

// A camera move button that drives the scroll state from its logic() call.
//
// Rather than toggling KeyScrollState on mousePress/mouseRelease (which can
// "stick" if a release event is missed), the button re-evaluates its pressed
// state every frame in logic() and sets/clears the scroll mask accordingly.
// isPressed() is true only while the pointer is down over the button, so the
// camera scrolls exactly while the button is held and stops the moment it is
// released.
class CameraScrollButton : public ButtonWidget
{
public:
	CameraScrollButton(const std::string &caption, int scrollMask) :
		ButtonWidget(caption), ScrollMask(scrollMask)
	{
	}

	virtual void logic()
	{
		ButtonWidget::logic();
		// Work both in-game and in the editor so the camera moves while the
		// button is held and stops on release.
		if ((GameRunning || Editor.Running != EditorNotRunning) &&
				isEnabled() && isPressed()) {
			KeyScrollState |= ScrollMask;
		} else {
			KeyScrollState &= ~ScrollMask;
		}
	}

private:
	int ScrollMask;
};

/*----------------------------------------------------------------------------
--  On-screen keyboard buttons for chat
----------------------------------------------------------------------------*/

// Shared CAPS state for the chat keyboard: when true, letter keys feed
// their upper-case form to the input.
static bool ChatKbUpperCase = false;

// Key for a character button: feed the byte value to the engine input.
class CharKeyListener : public gcn::ActionListener
{
public:
	CharKeyListener(const std::string &ch) : Char(ch) {}
	virtual void action(const std::string &)
	{
		if (Char.size() != 1) {
			return;
		}
		char c = Char[0];
		if (ChatKbUpperCase && c >= 'a' && c <= 'z') {
			c = c - 'a' + 'A';
		}
		InputKey((int)(unsigned char)c);
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

// Enter key: commit the chat message and hide the keyboard afterwards.
class EnterListener : public gcn::ActionListener
{
public:
	EnterListener(gcn::Container *keyboard) : Keyboard(keyboard) {}
	virtual void action(const std::string &)
	{
		InputKey((int)SDLK_RETURN);
		if (Keyboard != NULL) {
			Keyboard->setVisible(false);
		}
	}
private:
	gcn::Container *Keyboard;
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

// CAPS toggle key: switches between lower and upper case letters and
// updates its own caption ("CAPS" / "caps") and all letter captions.
class CapsToggleListener : public gcn::ActionListener
{
public:
	CapsToggleListener(gcn::Button *capsButton,
		const std::vector<gcn::Button *> &letterButtons,
		const std::vector<std::string> &letters) :
		CapsButton(capsButton), LetterButtons(letterButtons), Letters(letters)
	{
	}
	virtual void action(const std::string &)
	{
		UpperCase = !UpperCase;
		ChatKbUpperCase = UpperCase;
		if (CapsButton != NULL) {
			CapsButton->setCaption(UpperCase ? "caps" : "CAPS");
		}
		for (size_t i = 0; i < LetterButtons.size() && i < Letters.size(); ++i) {
			LetterButtons[i]->setCaption(UpperCase
				? UpperString(Letters[i]) : Letters[i]);
		}
	}
private:
	static std::string UpperString(const std::string &s)
	{
		std::string out = s;
		for (size_t i = 0; i < out.size(); ++i) {
			if (out[i] >= 'a' && out[i] <= 'z') {
				out[i] = out[i] - 'a' + 'A';
			}
		}
		return out;
	}

	gcn::Button *CapsButton;
	std::vector<gcn::Button *> LetterButtons;
	std::vector<std::string> Letters;
	bool UpperCase;
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
// Initially hidden; visibility is toggled by the chat button.
//
// Layout (top to bottom):
//   row 0: digits 0-9
//   row 1: ".", "-", "_", ":", "/" (left aligned) ... "<-" (3 keys gap, 2 wide)
//   row 2-4: letters in QWERTY order
//   row 5: CAPS (3 wide) + Space (3 wide) + 1 key gap + Enter (wide)
static gcn::Container *MakeChatKeyboard(gcn::Container *parent)
{
	const int kw = 39;
	const int kh = 39;
	const int gap = 3;
	const int step = kw + gap;
	const int stepY = kh + gap;

	// 6 rows: digits, symbols, 3 letter rows, control row.
	gcn::Container *kb = new gcn::Container();
	kb->setOpaque(false);
	kb->setDimension(gcn::Rectangle(0, 0, 10 * step - gap, 6 * stepY - gap));
	kb->setVisible(false);

	// Row 0: digits 0-9.
	const char *digits[10] = {"0","1","2","3","4","5","6","7","8","9"};
	for (int col = 0; col < 10; ++col) {
		std::string ch = digits[col];
		MakeKey(kb, ch, kw, kh, col * step, 0, new CharKeyListener(ch));
	}

	// Row 1: 5 symbols left-aligned, then "<-" with a 3-key gap, 2 keys wide.
	const char *symsFixed[5] = {".","-","_",":","/"};
	for (int col = 0; col < 5; ++col) {
		std::string ch = symsFixed[col];
		MakeKey(kb, ch, kw, kh, col * step, 1 * stepY,
			new CharKeyListener(ch));
	}
	// "<-" at column 8 (5 symbols + 3 gap), 2 keys wide.
	MakeKey(kb, "<-", kw * 2 + gap, kh, (5 + 3) * step, 1 * stepY,
		new BackspaceListener());

	// Rows 2-4: letters in QWERTY layout.
	// QWERTY top row, then home row, then bottom row.
	static const char *qwerty[3][10] = {
		{"q","w","e","r","t","y","u","i","o","p"},
		{"a","s","d","f","g","h","j","k","l",""},  // 9 keys
		{"z","x","c","v","b","n","m","","",""}     // 7 keys
	};
	std::vector<gcn::Button *> letterButtons;
	std::vector<std::string> letters;
	for (int row = 0; row < 3; ++row) {
		for (int col = 0; col < 10 && qwerty[row][col][0]; ++col) {
			std::string ch = qwerty[row][col];
			ButtonWidget *b = MakeKey(kb, ch, kw, kh,
				col * step, (row + 2) * stepY,
				new CharKeyListener(ch));
			letterButtons.push_back(b);
			letters.push_back(ch);
		}
	}

	// Row 5: CAPS (3 wide) + Space (3 wide) + 1 key gap + Enter (wide).
	int controlY = 5 * stepY;
	int capsWidth = kw * 3 + gap * 2;
	ButtonWidget *capsBtn = MakeKey(kb, "CAPS", capsWidth, kh,
		0, controlY, NULL);

	int spaceWidth = kw * 3 + gap * 2;
	MakeKey(kb, "Space", spaceWidth, kh, 3 * step, controlY,
		new SpaceListener());

	// 1 key gap after Space, then Enter (3 keys wide).
	int enterWidth = kw * 3 + gap * 2;
	MakeKey(kb, "Enter", enterWidth, kh, 7 * step, controlY,
		new EnterListener(kb));

	capsBtn->addActionListener(new CapsToggleListener(capsBtn,
		letterButtons, letters));

	parent->add(kb, 0, 0);
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

// Interactive touch widgets created by this module.  Used by
// IsPointOnGuichanWidget() to prevent clicks on these widgets from
// also reaching the game map.
static std::vector<gcn::Widget *> TouchWidgets;

// Check whether a widget (or any visible descendant) contains the point.
static bool WidgetContainsPoint(gcn::Widget *w, int x, int y)
{
	if (w == NULL || !w->isVisible()) {
		return false;
	}
	int wx, wy;
	w->getAbsolutePosition(wx, wy);
	if (x < wx || y < wy ||
		x >= wx + w->getWidth() || y >= wy + w->getHeight()) {
		return false;
	}
	return true;
}

bool IsPointOnGuichanWidget(int x, int y)
{
	for (size_t i = 0; i < TouchWidgets.size(); ++i) {
		if (WidgetContainsPoint(TouchWidgets[i], x, y)) {
			return true;
		}
	}
	return false;
}

static const int OverlayBtnSize = 48;
static const int OverlayGap = 3;

// Create the touch overlay on the in-game GUI container.
// The overlay is owned by the container (added as a child), so the caller
// does not need to free it.
void CreateIngameTouchOverlay(gcn::Container *container, bool showChat)
{
	if (container == NULL) {
		return;
	}

	// A new game/map recreates the overlay; drop references to the old
	// widgets so IsPointOnGuichanWidget() never checks dangling pointers.
	TouchWidgets.clear();

	// On-screen keyboard, initially hidden, top-left.  Only created in-game
	// (chat makes no sense in the editor).
	gcn::Container *keyboard = NULL;
	if (showChat) {
		keyboard = MakeChatKeyboard(container);
		TouchWidgets.push_back(keyboard);
	}

	// Camera + chat buttons at the bottom-left, above the bottom bar.
	// One button width of margin from the left and bottom screen edges.
	const int margin = OverlayBtnSize;
	const int rowY = Video.Height - OverlayBtnSize - margin;
	int x = margin;

	// Order: up, down, left, right, message.
	CameraScrollButton *upBtn = new CameraScrollButton("^", ScrollUp);
	upBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(upBtn);
	container->add(upBtn, x, rowY);
	TouchWidgets.push_back(upBtn);
	x += OverlayBtnSize + OverlayGap;

	CameraScrollButton *downBtn = new CameraScrollButton("v", ScrollDown);
	downBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(downBtn);
	container->add(downBtn, x, rowY);
	TouchWidgets.push_back(downBtn);
	x += OverlayBtnSize + OverlayGap;

	CameraScrollButton *leftBtn = new CameraScrollButton("<", ScrollLeft);
	leftBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(leftBtn);
	container->add(leftBtn, x, rowY);
	TouchWidgets.push_back(leftBtn);
	x += OverlayBtnSize + OverlayGap;

	CameraScrollButton *rightBtn = new CameraScrollButton(">", ScrollRight);
	rightBtn->setSize(OverlayBtnSize, OverlayBtnSize);
	ApplyMenuButtonStyle(rightBtn);
	container->add(rightBtn, x, rowY);
	TouchWidgets.push_back(rightBtn);
	x += OverlayBtnSize + OverlayGap;

	if (showChat) {
		ButtonWidget *chatBtn = new ButtonWidget("M");
		chatBtn->setSize(OverlayBtnSize, OverlayBtnSize);
		ApplyMenuButtonStyle(chatBtn);
		chatBtn->addActionListener(new ChatToggleListener(keyboard));
		container->add(chatBtn, x, rowY);
		TouchWidgets.push_back(chatBtn);

		// Place the keyboard one button width above the navigation row.
		const int kbX = margin;
		const int kbY = rowY - keyboard->getHeight() - OverlayBtnSize;
		keyboard->setPosition(kbX, kbY);
	}
}

//@}
