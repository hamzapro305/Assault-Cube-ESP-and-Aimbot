#include "esp.h"
#include "constants.h"
#include <stdio.h>
#include "geom.h"
#include "settings.h"
#include "imgui.h"
#include <iostream>
#include <ctime>

const float FOV = 90;
const vec3 centerScreenPos = { 1024 / 2 , 768 / 2 , 0};
float currAimTime = 0;
clock_t lastAimTime = clock();
Player* currTarget = nullptr;

#define min(a, b) a < b ? a : b
void normalizeAngle(vec3& angle) {
	if (angle.x > 360) angle.x -= 360;
	if (angle.x < 0) angle.x += 360;

	if (angle.y > 90) angle.y -= 90;
	if (angle.y < -90) angle.y += 90;
}
bool isInFOV(Player* owner, vec3 looking) {

	vec3 angle = CalcAngle(owner->pos, looking);
	vec3 playerAngle{ owner->yaw + 180, owner->pitch, 0 };
	normalizeAngle(playerAngle);

	vec3 angleDiff = playerAngle - angle;

	if (angleDiff.x > 180) angleDiff.x -= 360;
	if (angleDiff.x < -180) angleDiff.x += 360;
	if (angleDiff.y > 180) angleDiff.y -= 360;
	if (angleDiff.y < -180) angleDiff.y += 360;

	return (fabs(angleDiff.x) <= FOV / 2 && fabs(angleDiff.y) <= FOV / 2);
}
bool isValidTarget(Player* player) {
	if (
		player->health > 100 ||
		player->health <= 0 || 
		!isInFOV(localPlayerPtr, player->o)
		) return false;
	return true;
}

bool isInFovW2S(vec3& screenLoc) {
	if ( abs(centerScreenPos.Distance(screenLoc)) < Settings::Aimbot::fov) {
		return true;
	}
	return false;
}

void smoothingAngle(vec3& from, vec3& to, float percent) {
	vec3 delta = to - from;
	normalizeAngle( delta );
	if ( delta.x > 180 ) delta.x -= 360;
	if ( delta.x < 180 ) delta.x += 360;
	if ( delta.y > 45 ) delta.y -= 45;
	if ( delta.y < -45 ) delta.y += 45;

	from.x += delta.x * percent;
	from.y += delta.y * percent;
	normalizeAngle( from );
}


Player* ESP::getNearestPlayer() {
	Player* nearestPlayer = nullptr;
	float nearestDistance = 9999999.0f;
	for (int i = 1; i < noPlayers + 1; i++) {
		Player* player = entityList->players[i];
		if (!isValidTarget(player)) continue;
		float distance = localPlayerPtr->pos.Distance(player->pos);
		if (distance < nearestDistance) {
			nearestDistance = distance;
			nearestPlayer = player;
		}
	}
	return nearestPlayer;
}

Player* getNearestEntityW2S() {
	Player* nearestPlayer = nullptr;
	float nearestDistance = 9999999.0f;
	float distance = 0;
	for ( int i = 1; i <= noPlayers; i++ ) {
		Player* player = entityList->players[ i ];
		if ( player->health <= 0 || player->health > 100 || player->team == localPlayerPtr->team ) continue;

		vec3 headPos = { player->o.x, player->o.y, player->o.z };
		vec3 headScreenPos = WorldToScreen( headPos );

		if ( headScreenPos.z < 0 ) {
			continue;
		}

		if ( Settings::Aimbot::checkInFov && !isInFovW2S( headScreenPos ) ) {
			continue;
		}
		headScreenPos.z = 0;
		distance = abs( centerScreenPos.Distance( headScreenPos ) );
		if ( distance < nearestDistance ) {
			nearestDistance = distance;
			nearestPlayer = player;
		}
	}
	return nearestPlayer;
}

void ESP::aimbot() {
	if ( Settings::Aimbot::drawFovCircle ) {
		ImGui::GetBackgroundDrawList()->AddCircle(
			ImVec2( centerScreenPos.x , centerScreenPos.y ) ,
			Settings::Aimbot::fov ,
			IM_COL32( 255 , 255 , 255 , 255 ) ,
			100
		);
	}
	if ( !Settings::Aimbot::enabled || !GetAsyncKeyState( VK_CONTROL ) ) {
		currAimTime = 0;
		lastAimTime = clock();
		currTarget = nullptr;
		return;
	}
	Player* target = getNearestEntityW2S();
	if (!target) return;


	if ( target != currTarget ) {
		currAimTime = 0;
		lastAimTime = clock();
		currTarget = target;
	}

	clock_t now = clock();
	currAimTime += static_cast< float >( now - lastAimTime ) / CLOCKS_PER_SEC;
	lastAimTime = now;
	float percent = min( currAimTime / Settings::Aimbot::smoothingAmount , 1.0);


	vec3 targetAngle = CalcAngle(localPlayerPtr->pos, target->pos);
	targetAngle.x += 180;

	vec3 currentAngle = { localPlayerPtr->yaw, localPlayerPtr->pitch, 0 };

	if ( Settings::Aimbot::smoothing ) {
		if ( percent >= 1 ) {
			currAimTime = 0;
			percent = 1;
		}
		smoothingAngle( currentAngle , targetAngle , percent );
	}
	else {
		currentAngle = targetAngle;
	}
	// std::cout << "Target Angle: " << targetAngle.x << " " << targetAngle.y << " " << targetAngle.z << std::endl;
	// std::cout << "Current Angle: " << currentAngle.x << " " << currentAngle.y << " " << currentAngle.z << std::endl;

	localPlayerPtr->yaw = currentAngle.x;
	localPlayerPtr->pitch = currentAngle.y;
}

void drawCenteredText( std::string text , float x , float y ) {
	float textWidth = ImGui::CalcTextSize( text.c_str() ).x;
	ImGui::GetBackgroundDrawList()->AddText(
		ImVec2( x - textWidth / 2 , y ) ,
		IM_COL32( 255 , 255 , 255 , 255 ) ,
		text.c_str()
	);
}

void drawScalingBarVertical(float x1, float y1, float x2, float y2, float width, float value, float max, ImColor color) {
	float widthDiff = x2 - x1;
	float heightDiff = y2 - y1;
	float scaleHeight = heightDiff * ( value / max );
	ImGui::GetBackgroundDrawList()->AddRect(
		ImVec2( x1 , y1 ) ,
		ImVec2( x2 , y2 ),
		IM_COL32(1, 1, 1, 255),
		0,
		0,
		width
	);
	ImGui::GetBackgroundDrawList()->AddRectFilled(
		ImVec2( x1 , y1 ) ,
		ImVec2( x2 , y1 + scaleHeight ) ,
		color
	);
}

void ESP::drawEsp() {
	// Check if ESP is enabled
	if ( !Settings::ESP::enabled ) return;

	Player* player = nullptr;
	for ( int i = 1; i <= noPlayers; i++ ) { // Start from 1 (assuming 0 is the local player)
		player = entityList->players[ i ];

		// Validate player object and health range
		if ( !player || player->health <= 0 || player->health > 100 ) continue;

		// Determine if the player is a teammate
		bool teamMate = ( player->team == localPlayerPtr->team );
		if ( teamMate && !Settings::ESP::drawTeam ) continue;

		// Get world positions for the head and feet
		vec3 headPos = { player->o.x, player->o.y, player->o.z };
		vec3 feetPos = { player->o.x, player->o.y, player->o.z - player->eyeHeight };

		// Convert world positions to screen positions
		vec3 headScreenPos = WorldToScreen( headPos );
		vec3 feetScreenPos = WorldToScreen( feetPos );


		// Calculate bounding box dimensions
		float height = abs( headScreenPos.y - feetScreenPos.y );
		float width = height / 4;

		// Define bounding box corners
		ImVec2 topLeft( feetScreenPos.x - width , headScreenPos.y );
		ImVec2 topRight( feetScreenPos.x + width , headScreenPos.y );

		ImVec2 bottomRight( feetScreenPos.x + width , feetScreenPos.y );
		ImVec2 bottomLeft( feetScreenPos.x - width , feetScreenPos.y );


		// Determine ESP color
		ImColor espColor = teamMate ? *Settings::ESP::teamColor : *Settings::ESP::enemyColor;

		// Draw the ESP bounding box
		ImGui::GetBackgroundDrawList()->AddQuad(topLeft, bottomLeft, bottomRight, topRight, espColor );

		drawScalingBarVertical(
			bottomLeft.x - 5 - width / 2 , 
			bottomLeft.y , 
			bottomLeft.x - 5 , 
			topRight.y ,
			1 ,
			player->health ,
			100,
			ImColor( 0 , 255 , 0 , 255 )
		);
		drawCenteredText( player->name , bottomLeft.x , bottomLeft.y - 20 );
	}
}