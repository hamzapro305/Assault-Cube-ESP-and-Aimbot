#pragma once
#include "settings.h"
#include "imgui.h"

namespace Settings {
	namespace ESP {
		extern bool enabled = true;
		extern bool drawTeam = false;
		extern ImColor* teamColor = new ImColor(0, 0, 255, 255);
		extern ImColor* enemyColor = new ImColor( 255 , 0 , 0, 255 );
	}
	namespace Aimbot {

	}
}