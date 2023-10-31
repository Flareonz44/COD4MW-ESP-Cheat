#pragma once
#include <stdint.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <vector>
#include <iostream>
#include <d3dx9math.h>
#include <math.h>
#include <string.h>

struct _olel1DC {
	const DWORD start = 0x44F2D8;
	const DWORD duck = 0x64;
	const DWORD posX = 0x74;
	const DWORD posY = 0x78;
	const DWORD posZ = 0x7C;
	const DWORD isAlive = 0x1C0;
};

struct _olel4CC {
	const DWORD start = 0x439264;
	const DWORD nameTag = 0x18;
	const DWORD team = 0x2C;
	const DWORD status = 0x39C;
	const DWORD model1 = 0x48;
	const DWORD model2 = 0x88;
};

struct entityData {
	char* nameTag[16];
	float posX = NULL;
	float posY = NULL;
	float posZ = NULL;
	float hpZ = NULL;
	int32_t team = NULL;
	bool isAlive = NULL;
	char* status[16];
	int8_t duck = NULL;
	char* model1[32];
	char* model2[32];
};

struct Vector2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;
};

struct Vector4 {
	float x, y, z, w;
};

struct Matrix4x4 {
	float _11, _12, _13, _14;
	float _21, _22, _23, _24;
	float _31, _32, _33, _34;
	float _41, _42, _43, _44;
};

struct gameData {
	float viewMatrix[16];
	char* nameTag[16];
	float playerX = 0.0;
	float playerY = 0.0;
	float playerZ = 0.0;
	int32_t team = NULL;
};