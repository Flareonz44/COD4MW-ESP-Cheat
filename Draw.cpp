#include "stdafx.h"
#include "Draw.h"
const D3DCOLOR TEAM_COLOR = D3DCOLOR_ARGB(255, 255, 255, 255);
const D3DCOLOR ENEMY_COLOR = D3DCOLOR_ARGB(255, 255, 0, 0);
const D3DCOLOR MENU_TITLE_COLOR = D3DCOLOR_ARGB(255, 211, 20, 255);
const D3DCOLOR MENU_MAIN_COLOR = D3DCOLOR_ARGB(255, 177, 23, 211);
const D3DCOLOR MENU_ON_COLOR = D3DCOLOR_ARGB(255, 35, 234, 68);
const D3DCOLOR MENU_OFF_COLOR = D3DCOLOR_ARGB(255, 234, 35, 35);
const D3DCOLOR RETICLE_COLOR = D3DCOLOR_ARGB(255, 255, 255, 0);
const D3DCOLOR ENTITY_COLOR = D3DCOLOR_ARGB(255, 42, 243, 247);

const D3DCOLOR ARAB_COLOR = D3DCOLOR_ARGB(255, 229, 680, 78);
const D3DCOLOR USMC_COLOR = D3DCOLOR_ARGB(255, 52, 239, 221);
const D3DCOLOR OPFORCE_COLOR = D3DCOLOR_ARGB(255, 224, 211, 33);
const D3DCOLOR SAS_COLOR = D3DCOLOR_ARGB(255, 120, 72, 242);
const D3DCOLOR SPETZNAS_COLOR = D3DCOLOR_ARGB(255, 92, 164, 242);
D3DCOLOR usageColor = ENTITY_COLOR;

float d2r(float d) {
	return (d / 180.0) * ((float)D3DX_PI);
}

float r2d(float r) {
	return (r / (float)D3DX_PI) * 180;
}

Vector4 worldToScreen(Vector3 playerPos, Vector3 entPos, float viewMatrix[16], int wWidth, int wHeigth) {
	Vector3 vecEnt = { entPos.x - playerPos.x, entPos.y - playerPos.y, entPos.z - playerPos.z };
	float distToObj = sqrt(vecEnt.x * vecEnt.x + vecEnt.y * vecEnt.y + vecEnt.z * vecEnt.z);

	Vector4 clip;

	clip.x = entPos.x * viewMatrix[0] + entPos.y * viewMatrix[1] + entPos.z * viewMatrix[2] + viewMatrix[3];
	clip.y = entPos.x * viewMatrix[4] + entPos.y * viewMatrix[5] + entPos.z * viewMatrix[6] + viewMatrix[7];
	clip.z = entPos.x * viewMatrix[8] + entPos.y * viewMatrix[9] + entPos.z * viewMatrix[10] + viewMatrix[11];
	clip.w = entPos.x * viewMatrix[12] + entPos.y * viewMatrix[13] + entPos.z * viewMatrix[14] + viewMatrix[15];

	if (clip.w < 0.1) {
		return{ 0, 0, 0, 0 };
	}

	Vector3 NDC;

	NDC.x = clip.x / clip.w;
	NDC.y = clip.y / clip.w;
	NDC.z = clip.z / clip.w;

	float x = (wWidth / 2 * NDC.x) + (NDC.x + wWidth / 2);
	float y = -(wHeigth / 2 * NDC.y) + (NDC.y + wHeigth / 2);

	return{ x, y, distToObj, 1 };
}

DDraw::DDraw() {}

DDraw::DDraw(HWND overlayHwnd, HWND targetHwnd, int width, int height) {
	this->width = width;
	this->height = height;
	this->targetHwnd = targetHwnd;
	this->overlayHwnd = overlayHwnd;
	init(overlayHwnd);
}

int DDraw::init(HWND hWnd) {
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &object))) {
		exit(1);
	}

	ZeroMemory(&params, sizeof(params));

	params.BackBufferHeight = height;
	params.BackBufferWidth = width;
	params.Windowed = TRUE;
	params.hDeviceWindow = hWnd;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.EnableAutoDepthStencil = TRUE;
	params.AutoDepthStencilFormat = D3DFMT_D16;

	object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &params, 0, &device);

	D3DXCreateFontA(device, 16, 0, FW_SEMIBOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 3, DEFAULT_PITCH, "Segoe UI", &font);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	return 0;
}

int DDraw::render() {
	if (device == nullptr) {
		return 1;
	}
	device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
	device->BeginScene();
	if (targetHwnd == GetForegroundWindow()) {//targetHwnd == GetForegroundWindow()
		// MENU
		if (!showMenu){
			drawText((char*)"DeepEye Cheats", width/2, 5 + (3 * 16), MENU_TITLE_COLOR);
			drawText((char*)"[INS] > Menu", width / 2, 5 + (4 * 16), MENU_MAIN_COLOR);
		}
		else {
			drawText((char*)"- - MENU - -", width / 2, 5 + (3 * 16), MENU_TITLE_COLOR);
			drawText((char*)"[INS] Display menu", width / 2, 5 + (4 * 16), MENU_MAIN_COLOR);
			drawText((char*)"[9] Close cheat", width / 2, 5 + (5 * 16), MENU_MAIN_COLOR);
			if (showReticle) {
				drawText((char*)"[1] Reticle", width / 2, 5 + (6 * 16), MENU_ON_COLOR);
			}
			else {
				drawText((char*)"[1] Reticle", width / 2, 5 + (6 * 16), MENU_OFF_COLOR);
			}
			if (showLines) {
				drawText((char*)"[2] Lines", width / 2, 5 + (7 * 16), MENU_ON_COLOR);
			}
			else {
				drawText((char*)"[2] Lines", width / 2, 5 + (7 * 16), MENU_OFF_COLOR);
			}
			if (showReference) {
				drawText((char*)"[3] Color Ref", width / 2, 5 + (8 * 16), MENU_ON_COLOR);
			}
			else {
				drawText((char*)"[3] Color Ref", width / 2, 5 + (8 * 16), MENU_OFF_COLOR);
			}
			if (showDist) {
				drawText((char*)"[4] Distance", width / 2, 5 + (9 * 16), MENU_ON_COLOR);
			}
			else {
				drawText((char*)"[4] Distance", width / 2, 5 + (9 * 16), MENU_OFF_COLOR);
			}
		}
		// RETICLE
		if (showReticle) {
			drawLine(width/2 - 3, height/2, width/2 + 4, height/2, RETICLE_COLOR);
			drawLine(width/2, height/2 - 3, width/2, height/2 + 4, RETICLE_COLOR);
		}
		if (showReference) {
			drawText((char*)" - Reference - ", width * .9, 5 + (3 * 16), MENU_TITLE_COLOR);
			drawText((char*)"Arab", width * .9, 5 + (4 * 16), ARAB_COLOR);
			drawText((char*)"USMC", width * .9, 5 + (5 * 16), USMC_COLOR);
			drawText((char*)"OpForce", width * .9, 5 + (6 * 16), OPFORCE_COLOR);
			drawText((char*)"SAS", width * .9, 5 + (7 * 16), SAS_COLOR);
			drawText((char*)"Spetznas", width * .9, 5 + (8 * 16), SPETZNAS_COLOR);
		}
		for (int i = 0; i < loadedEnts.size(); i++) {
			if (loadedEnts[i].isAlive) {
				int addh = 62;
				if (strstr((char*)loadedEnts[i].status, (char*)"crouch")) {
					addh = 45;
				}
				else if (strstr((char*)loadedEnts[i].status, (char*)"prone")) {
					addh = 15;
				}
				Vector4 c1 = worldToScreen({ loadDat.playerX, loadDat.playerY, loadDat.playerZ }, { loadedEnts[i].posX - 10, loadedEnts[i].posY + 10, loadedEnts[i].posZ }, loadDat.viewMatrix, width, height);
				Vector4 c2 = worldToScreen({ 0, 0, 0 }, { loadedEnts[i].posX + 10, loadedEnts[i].posY + 10, loadedEnts[i].posZ }, loadDat.viewMatrix, width, height);
				Vector4 c3 = worldToScreen({ loadDat.playerX, loadDat.playerY, loadDat.playerZ }, { loadedEnts[i].posX + 10, loadedEnts[i].posY - 10, loadedEnts[i].posZ }, loadDat.viewMatrix, width, height);
				Vector4 c4 = worldToScreen({ 0, 0, 0 }, { loadedEnts[i].posX - 10, loadedEnts[i].posY - 10, loadedEnts[i].posZ }, loadDat.viewMatrix, width, height);
				Vector4 c5 = worldToScreen({ 0, 0, 0 }, { loadedEnts[i].posX - 10, loadedEnts[i].posY + 10, loadedEnts[i].posZ + addh }, loadDat.viewMatrix, width, height);
				Vector4 c6 = worldToScreen({ 0, 0, 0 }, { loadedEnts[i].posX + 10, loadedEnts[i].posY + 10, loadedEnts[i].posZ + addh }, loadDat.viewMatrix, width, height);
				Vector4 c7 = worldToScreen({ 0, 0, 0 }, { loadedEnts[i].posX + 10, loadedEnts[i].posY - 10, loadedEnts[i].posZ + addh }, loadDat.viewMatrix, width, height);
				Vector4 c8 = worldToScreen({ 0, 0, 0 }, { loadedEnts[i].posX - 10, loadedEnts[i].posY - 10, loadedEnts[i].posZ + addh }, loadDat.viewMatrix, width, height);
				/*
				arab 1
				usmc 2
				opforce 3
				sas 4
				spetznas 5
				*/
				if (loadedEnts[i].team == 1) {
					usageColor = ARAB_COLOR;
				}
				else if (loadedEnts[i].team == 2) {
					usageColor = USMC_COLOR;
				}
				else if (loadedEnts[i].team == 3) {
					usageColor = OPFORCE_COLOR;
				}
				else if (loadedEnts[i].team == 4) {
					usageColor = SAS_COLOR;
				}
				else if (loadedEnts[i].team == 5) {
					usageColor = SPETZNAS_COLOR;
				}
				else {
					usageColor = ENTITY_COLOR;
				}
				if (c1.w && c2.w && c3.w && c4.w && c5.w && c6.w && c7.w && c8.w) {
					drawLine(c1.x, c1.y, c2.x, c2.y, usageColor);
					drawLine(c2.x, c2.y, c3.x, c3.y, usageColor);
					drawLine(c3.x, c3.y, c4.x, c4.y, usageColor);
					drawLine(c4.x, c4.y, c1.x, c1.y, usageColor);
					//connect
					drawLine(c1.x, c1.y, c5.x, c5.y, usageColor);
					drawLine(c2.x, c2.y, c6.x, c6.y, usageColor);
					drawLine(c3.x, c3.y, c7.x, c7.y, usageColor);
					drawLine(c4.x, c4.y, c8.x, c8.y, usageColor);
					//top
					drawLine(c5.x, c5.y, c6.x, c6.y, usageColor);
					drawLine(c6.x, c6.y, c7.x, c7.y, usageColor);
					drawLine(c7.x, c7.y, c8.x, c8.y, usageColor);
					drawLine(c8.x, c8.y, c5.x, c5.y, usageColor);
					if (showLines) {
						drawLine(width / 2, height, (c1.x + c3.x) / 2.0, (c1.y + c3.y) / 2.0, usageColor);
					}
					if (showDist) {
						if (loadDat.playerX == 0.0 && loadDat.playerY == -0.0 && loadDat.playerZ == 0.0) {
							drawText((char*)"...", (c5.x + c7.x) / 2.0, ((c5.y + c7.y) / 2.0) - 16, usageColor);
						}
						else {
							char dista[40];
							snprintf(dista, sizeof(dista), "%u", (int)(((c1.z + c3.z) / 2) / 50));
							drawText((char*)strcat(dista, "m"), (c5.x + c7.x) / 2.0, ((c5.y + c7.y) / 2.0) - 16, usageColor);
						}
					}
				}
			}
		}
	}
	device->EndScene();
	device->PresentEx(0, 0, 0, 0, 0);
	return 0;
}

int DDraw::passEntities(std::vector<entityData> entList) {
	this->loadedEnts = entList;
	return 0;
}

int DDraw::passData(gameData gDat) {
	this->loadDat = gDat;
	return 0;
}

int DDraw::drawText(LPCSTR String, int x, int y, D3DCOLOR color) {
	RECT rect;
	rect.top = y;
	rect.left = x;
	rect.bottom = y + 1;
	rect.right = x + 1;
	font->DrawTextA(0, String, strlen(String), &rect, DT_NOCLIP | DT_CENTER, color);
	return 0;
}

int DDraw::drawRectangle(float x, float y, float width, float height, D3DCOLOR color) {
	D3DXVECTOR2 Rect[5];
	Rect[0] = D3DXVECTOR2(x, y);
	Rect[1] = D3DXVECTOR2(x + width, y);
	Rect[2] = D3DXVECTOR2(x + width, y + height);
	Rect[3] = D3DXVECTOR2(x, y + height);
	Rect[4] = D3DXVECTOR2(x, y);
	if (!line) {
		D3DXCreateLine(device, &line);
	}
	line->SetWidth(1);
	line->Draw(Rect, 5, color);
	return 0;
}

int DDraw::drawFilledRectangle(float x, float y, float width, float height, D3DCOLOR color) {
	D3DXVECTOR2 vertices[2] = { D3DXVECTOR2(x + width / 2, y),D3DXVECTOR2(x + width / 2, y + height) };
	if (!line) {
		D3DXCreateLine(device, &line);
	}
	line->SetWidth(width);
	line->Draw(vertices, 2, color);
	return 0;
}

int DDraw::drawLine(float x1, float y1, float x2, float y2, D3DCOLOR color) {
	D3DXVECTOR2 vert[2];
	vert[0] = D3DXVECTOR2(x1, y1);
	vert[1] = D3DXVECTOR2(x2, y2);
	if (!line) {
		D3DXCreateLine(device, &line);
	}
	line->SetWidth(1);
	line->Draw(vert, 2, color);
	return 0;
}