#pragma once

class ImColor;

namespace Settings {
	extern int windowWidth;
	extern int windowHeight;
	namespace ESP {
		extern bool enabled;
		extern bool drawTeam;
		extern ImColor* teamColor;
		extern ImColor* enemyColor;
	}
	namespace Aimbot {
		extern bool enabled;
		extern bool smoothing;
		extern float smoothingAmount;
		extern bool checkInFov;
		extern float fov;
		extern bool drawFovCircle;
	}
}