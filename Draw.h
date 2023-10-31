#pragma once
#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <Windows.h>
#include "D3D9Helper.h"
#include "dstructs.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")


class DDraw {
private:
	IDirect3D9Ex* object = NULL;
	IDirect3DDevice9Ex* device = NULL;
	D3DPRESENT_PARAMETERS params;
	ID3DXFont* font = 0;
	ID3DXLine* line = nullptr;
	HWND targetHwnd, overlayHwnd;
	int width, height;
	int init(HWND hWnd);
	int drawText(LPCSTR string, int x, int y, D3DCOLOR color);
	std::vector<entityData> loadedEnts;
	gameData loadDat;
public:
	DDraw();
	DDraw(HWND overlayHwnd, HWND targetHwnd, int width, int height);
	int drawRectangle(float x, float y, float width, float height, D3DCOLOR color);
	int drawFilledRectangle(float x, float y, float width, float height, D3DCOLOR color);
	int drawLine(float x1, float y1, float x2, float y2, D3DCOLOR color);
	int render();
	int passEntities(std::vector<entityData> entList);
	int passData(gameData gDat);
	bool showMenu = false;
	bool showDist = true;
	bool showTeam = true;
	bool showLines = true;
	bool showReticle = true;
	bool showName = true;
	bool showReference = true;
};