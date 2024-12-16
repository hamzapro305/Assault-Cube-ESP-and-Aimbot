#pragma once
#include "settings.h"
#include "imgui.h"

namespace Settings {
	extern int windowWidth = 1024;
	extern int windowHeight = 768;
	namespace ESP {
		extern bool enabled = true;
		extern bool drawTeam = false;
		extern ImColor* teamColor = new ImColor(0, 0, 255, 255);
		extern ImColor* enemyColor = new ImColor( 255 , 0 , 0, 255 );
	}
	namespace Aimbot {
		extern bool enabled = true;
		extern bool smoothing = true;
		extern float smoothingAmount = 1.0f;
		extern bool checkInFov = true;
		extern float fov = 180;
		extern bool drawFovCircle = true;
	}
}