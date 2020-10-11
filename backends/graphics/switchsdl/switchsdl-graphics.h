/* ResidualVM - A 3D game interpreter
 *
 * ResidualVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BACKENDS_GRAPHICS_SWITCHSDL_GRAPHICS_H
#define BACKENDS_GRAPHICS_SWITCHSDL_GRAPHICS_H

#include "backends/graphics/openglsdl/openglsdl-graphics.h"

/**
 * Nintendo Switch based graphics manager
 *
 * Used when rendering games with Nintendo Switch
 */
class SwitchSdlGraphicsManager : public OpenGLSdlGraphicsManager {
public:
	SwitchSdlGraphicsManager(SdlEventSource *sdlEventSource, SdlWindow *window);
	virtual ~SwitchSdlGraphicsManager();

	virtual void setupScreen(uint gameWidth, uint gameHeight, bool fullscreen, bool accel3d) override;

	virtual void updateScreen();

protected:
	void drawCursor(Graphics::Surface *surface);
	void closeCursor();

	OpenGL::TiledSurface *_cursorScreen;
	Graphics::Surface *_cursor;
	Graphics::Surface *_cursorBlank;
	const byte *_cursorPalette;
	bool _cursorVisible;

	void setCursorPalette(const byte *colors, uint start, uint num) override;
	void setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, bool dontScale = false, const Graphics::PixelFormat *format = nullptr) override;
	bool showMouse(bool visible) override;

	Common::Point _cursorPosition;
	virtual bool notifyMousePosition(Common::Point &mouse) override;
};

#endif
