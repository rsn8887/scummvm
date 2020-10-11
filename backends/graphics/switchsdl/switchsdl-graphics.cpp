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

#include "common/scummsys.h"

#if defined(SDL_BACKEND) && defined(NINTENDO_SWITCH)

#include "backends/graphics/switchsdl/switchsdl-graphics.h"

#include "backends/events/sdl/sdl-events.h"
#include "common/config-manager.h"
#include "engines/engine.h"
#include "graphics/opengl/framebuffer.h"
#include "graphics/opengl/surfacerenderer.h"
#include "graphics/opengl/tiledsurface.h"

SwitchSdlGraphicsManager::SwitchSdlGraphicsManager(SdlEventSource *sdlEventSource, SdlWindow *window)
	:
	OpenGLSdlGraphicsManager(sdlEventSource, window, capabilities),
	_cursor(nullptr),
	_cursorBlank(nullptr),
	_cursorScreen(nullptr),
	_cursorVisible(false),
	_cursorPosition(0, 0) {
}

SwitchSdlGraphicsManager::~SwitchSdlGraphicsManager() {
	closeCursor();
}

void SwitchSdlGraphicsManager::setupScreen(uint gameWidth, uint gameHeight, bool fullscreen, bool accel3d) {
	closeCursor();

	// Nintendo Switch requirement
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	OpenGLSdlGraphicsManager::setupScreen(gameWidth, gameHeight, fullscreen, accel3d);

	_cursorScreen = new OpenGL::TiledSurface(_overlayScreen->getWidth(), _overlayScreen->getHeight(), _overlayFormat);
}

void SwitchSdlGraphicsManager::drawCursor(Graphics::Surface *surface) {
	int w = _cursorScreen->getWidth() - _cursorPosition.x;
	int h = _cursorScreen->getHeight() - _cursorPosition.y;
	w = CLIP<int>(w, w, surface->w);
	h = CLIP<int>(h, h, surface->h);

	_cursorScreen->copyRectToSurface(surface->getPixels(), surface->pitch, _cursorPosition.x, _cursorPosition.y, w, h);
}

void SwitchSdlGraphicsManager::closeCursor() {
	if (_cursor) {
		_cursor->free();
		delete _cursor;
		_cursor = nullptr;
	}

	if (_cursorBlank) {
		_cursorBlank->free();
		delete _cursorBlank;
		_cursorBlank = nullptr;
	}

	if (_cursorScreen) {
		delete _cursorScreen;
		_cursorScreen = nullptr;
	}
}

void SwitchSdlGraphicsManager::updateScreen() {
	if (_frameBuffer) {
		_frameBuffer->detach();
		glViewport(0, 0, _overlayScreen->getWidth(), _overlayScreen->getHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		_surfaceRenderer->prepareState();
		drawSideTextures();
		_surfaceRenderer->render(_frameBuffer, _gameRect);
		_surfaceRenderer->restorePreviousState();
	}

	if (_overlayVisible) {
		_overlayScreen->update();

		if (_overlayBackground) {
			_overlayBackground->update();
		}

		drawOverlay();
	}

	if (_cursor && _cursorVisible) {
		_surfaceRenderer->prepareState();
		_surfaceRenderer->setFlipY(true);
		drawCursor(_cursor);
		_cursorScreen->update();
		_cursorScreen->draw(_surfaceRenderer);
		_surfaceRenderer->restorePreviousState();
		drawCursor(_cursorBlank);
	}

	SDL_GL_SwapWindow(_window->getSDLWindow());

	if (_frameBuffer) {
		_frameBuffer->attach();
	}
}

void SwitchSdlGraphicsManager::setCursorPalette(const byte *colors, uint start, uint num) {
	_cursorPalette = colors;
}

void SwitchSdlGraphicsManager::setMouseCursor(const void *buf, uint w, uint h, int hotspot_x, int hotspot_y, uint32 keycolor, bool dontScale, const Graphics::PixelFormat *format) {
	if (!w || !h) {
		return;
	}

	if (!_cursor) {
		_cursor = new Graphics::Surface();
		_cursor->create(w, h, _overlayFormat);

		for (uint y = 0; y < h; y++) {
			const byte *srcRow = (const byte *)buf + y * w;
			byte *dstRow = (byte *)_cursor->getBasePtr(0, y);

			for (uint x = 0; x < w; x++) {
				byte index = *srcRow++;

				if (index != keycolor) {
					byte r = _cursorPalette[index * 3 + 0];
					byte g = _cursorPalette[index * 3 + 1];
					byte b = _cursorPalette[index * 3 + 2];

					uint32 color = _overlayFormat.RGBToColor(r, g, b);
					*((uint32 *)dstRow) = color;
				}

				dstRow += _overlayFormat.bytesPerPixel;
			}
		}

		_cursorBlank = new Graphics::Surface();
		_cursorBlank->create(w, h, _overlayFormat);
		_cursorBlank->fillRect(Common::Rect(w, h), 0);
	}
}

bool SwitchSdlGraphicsManager::showMouse(bool visible) {
	_cursorVisible = visible;
	return true;
}

bool SwitchSdlGraphicsManager::notifyMousePosition(Common::Point &mouse) {
	transformMouseCoordinates(mouse);

	_cursorPosition.x = mouse.x;
	_cursorPosition.y = mouse.y;

	return true;
}

#endif
