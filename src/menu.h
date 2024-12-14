#pragma once
#include "constants.h"

namespace Menu {
	void toggleMenu();
	void init();
	void startRenderer();
	void render();
	void endRender();
	BOOL __stdcall newSwapBuffers( HDC hdc );
}