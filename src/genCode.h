#pragma once
#include <cstdint>
#include <iostream>
#include "geom.h"


class Player
{
public:
    char pad_0000[ 4 ]; //0x0000
    Vector3 o; //0x0004
    Vector3 velocity; //0x0010
    Vector3 N00000052; //0x001C
    Vector3 pos; //0x0028
    float yaw; //0x0034
    float pitch; //0x0038
    float roll; //0x003C
    char pad_0040[ 16 ]; //0x0040
    float eyeHeight; //0x0050
    char pad_0054[ 152 ]; //0x0054
    int32_t health; //0x00EC
    int32_t armor; //0x00F0
    char pad_00F4[ 12 ]; //0x00F4
    uint32_t weaponMk77x2; //0x0100
    char pad_0104[ 4 ]; //0x0104
    uint32_t ammoMk77; //0x0108
    uint32_t ammoTmpMAndA; //0x010C
    uint32_t ammoV19; //0x0110
    uint32_t ammoAArd10; //0x0114
    uint32_t ammoAd81; //0x0118
    uint32_t ammoMtp57; //0x011C
    char pad_0120[ 4 ]; //0x0120
    uint32_t ammoMk77x2; //0x0124
    char pad_0128[ 4 ]; //0x0128
    uint32_t timeToShootMk77; //0x012C
    uint32_t timeToShootTmpMAndA; //0x0130
    uint32_t timeToShootV19; //0x0134
    uint32_t timeToShootAArd10; //0x0138
    uint32_t timeToShootAd81; //0x013C
    uint32_t timeToShootMtp57; //0x0140
    uint32_t grenadeCount; //0x0144
    uint32_t timeToShootMk77x2; //0x0148
    bool isDr88HitCooldown; //0x014C
    char pad_014D[ 3 ]; //0x014D
    bool isMk77ShootCooldown; //0x0150
    char pad_0151[ 3 ]; //0x0151
    bool isTmpMAndAShootCooldown; //0x0154
    char pad_0155[ 3 ]; //0x0155
    bool isV19ShootCooldown; //0x0158
    char pad_0159[ 3 ]; //0x0159
    bool isAArd10ShootCooldown; //0x015C
    char pad_015D[ 3 ]; //0x015D
    bool isAd81ShootCooldown; //0x0160
    char pad_0161[ 3 ]; //0x0161
    bool isMtp57ShootCooldown; //0x0164
    char pad_0165[ 7 ]; //0x0165
    bool isMk77x2ShootCooldown; //0x016C
    char pad_016D[ 151 ]; //0x016D
    bool isFullAutoShootingEnabled; //0x0204
	char name[ 104 ]; //0x0205
	char pad_026D[ 159 ]; //0x026D
	int16_t team; //0x030C

	void print() const {
		auto printVector = [](const Vector3& vec) {
			return "(" + std::to_string(vec.x) + " " + std::to_string(vec.y) + " " + std::to_string(vec.z) + ")";
			};

		std::cout << "o: " << printVector(o) 
			<< ", name: " << this->name
			<< ", velocity: " << printVector(velocity)
			<< ", pos: " << printVector(pos)
			<< ", yaw: " << yaw
			<< ", pitch: " << pitch
			<< ", roll: " << roll
			<< ", health: " << health
			<< ", team: " << team
			<< std::endl;
	}
};

class EntityList {
public:
	Player* players[64];
};