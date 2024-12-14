#include "esp.h"
#include "constants.h"
#include <stdio.h>
#include "geom.h"
#include "settings.h"
#include "imgui.h"
#include <iostream>

const float FOV = 90;

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

Player* ESP::getNearestEntityAngle() {
	vec3 playerAngle{ localPlayerPtr->yaw + 180, localPlayerPtr->pitch, 0 };
	normalizeAngle(playerAngle);

	Player* nearestPlayer = nullptr;
	float smallestAngle = 9999999.0f;

	for (int i = 1; i < noPlayers + 1; i++) {
		Player* player = entityList->players[i];
		if (!isValidTarget(player)) continue;

		vec3 targetAngle = CalcAngle(localPlayerPtr->pos, player->pos);

		vec3 angleDiff = playerAngle - targetAngle;
		normalizeAngle(angleDiff);

		float angleMagnitude = angleDiff.Length();

		if (angleMagnitude < smallestAngle) {
			smallestAngle = angleMagnitude;
			nearestPlayer = player;
		}
	}
	return nearestPlayer;
}

void ESP::aimbot() {
	if (!GetAsyncKeyState(VK_CONTROL)) return;
	Player* target = ESP::getNearestEntityAngle();
	if (!target) return;

	vec3 angle = CalcAngle(localPlayerPtr->pos, target->pos);
	angle.x += 180;
	localPlayerPtr->yaw = angle.x;
	localPlayerPtr->pitch = angle.y;
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

		drawCenteredText(player->name, bottomLeft.x, bottomLeft.y - 20);

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
	}
}