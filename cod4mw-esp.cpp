#include "stdafx.h"
#include "cod4mw-esp.h"
#define DEBUG_MODE false


HINSTANCE hInst;
LPCWSTR overlayTitle = (LPCWSTR)"DeepEye COD4X Cheat Overlay";
HWND overlayHwnd, targetHwnd;
int width, height;
DDraw ddraw;
HANDLE targetProc;
std::vector<entityData> loadedEntities;
gameData loadedData;
_olel1DC olel1dc;
_olel4CC olel4cc;
DWORD iw3mp_exe = 0x0;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL CALLBACK FindWindowBySubstr(HWND hwnd, LPARAM substring) {
	const DWORD TITLE_SIZE = 1024;
	TCHAR windowTitle[TITLE_SIZE];

	if (GetWindowText(hwnd, windowTitle, TITLE_SIZE)) {
		if (_tcsstr(windowTitle, LPCTSTR(substring)) != NULL) {
			targetHwnd = hwnd;
			return true;
		}
	}
	return true;
}

DWORD GetModuleBaseAddress(DWORD dwProcID, char* szModuleName) {
	uintptr_t ModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32)) {
			do {
				if (strcmp(ModuleEntry32.szModule, szModuleName) == 0) {
					ModuleBaseAddress = (uintptr_t)ModuleEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return ModuleBaseAddress;
}

void put(LPCSTR message) {
	MessageBoxA(0, message, "Info", 0);
}

gameData getData() {
	DWORD pocid;
	GetWindowThreadProcessId(targetHwnd, &pocid);
	iw3mp_exe = GetModuleBaseAddress(pocid, "iw3mp.exe");
	gameData gdat;
	targetProc = OpenProcess(PROCESS_VM_READ, FALSE, pocid);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ReadProcessMemory(targetProc, (PBYTE*)((iw3mp_exe + 0xC907680) + (0x4 * (4*j+i))), &gdat.viewMatrix[4 * i + j], sizeof(gdat.viewMatrix[0]), 0);
		}
	}
	for (int i = 0; i < 64; i++) {
		entityData enti;
		DWORD stat;
		ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel4cc.start + (0x4CC * i) + olel4cc.model1), &enti.model1, sizeof(enti.model1), 0);
		ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel4cc.start + (0x4CC * i) + olel4cc.model2), &enti.model2, sizeof(enti.model2), 0);
		ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel4cc.start + (0x4CC * i) + olel4cc.status), &stat, sizeof(stat), 0);
		if (!stat && (strstr((char*)enti.model1, (char*)"_mp_") != NULL || strstr((char*)enti.model2, (char*)"_mp_") != NULL)) {
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.posX), &gdat.playerX, sizeof(gdat.playerX), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.posY), &gdat.playerY, sizeof(gdat.playerY), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.posZ), &gdat.playerZ, sizeof(gdat.playerZ), 0);
		}
	}
	return gdat;
}

std::vector<entityData> getEntities() {
	DWORD pocid;
	GetWindowThreadProcessId(targetHwnd, &pocid);
	iw3mp_exe = GetModuleBaseAddress(pocid, "iw3mp.exe");
	std::vector<entityData> loent;
	targetProc = OpenProcess(PROCESS_VM_READ, FALSE, pocid);
	for (int i = 0; i < 64; i++) {
		entityData enti;
		DWORD stat;
		ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel4cc.start + (0x4CC * i) + olel4cc.status), &stat, sizeof(stat), 0);
		if (stat) {
			ReadProcessMemory(targetProc, (PBYTE*)(stat), &enti.status, sizeof(enti.status), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel4cc.start + (0x4CC * i) + olel4cc.model1), &enti.model1, sizeof(enti.model1), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel4cc.start + (0x4CC * i) + olel4cc.model2), &enti.model2, sizeof(enti.model2), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.isAlive), &enti.isAlive, sizeof(enti.isAlive), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.posX), &enti.posX, sizeof(enti.posX), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.posY), &enti.posY, sizeof(enti.posY), 0);
			ReadProcessMemory(targetProc, (PBYTE*)(iw3mp_exe + olel1dc.start + (0x1DC * i) + olel1dc.posZ), &enti.posZ, sizeof(enti.posZ), 0);
			/*
			arab 1
			usmc 2
			opforce 3
			sas 4
			spetznas 5
			*/
			if (strstr((char*)enti.model1, (char*)"_arab_") != NULL || strstr((char*)enti.model2, (char*)"_arab_") != NULL) {
				enti.team = 1;
			}
			else if (strstr((char*)enti.model1, (char*)"_usmc_") != NULL || strstr((char*)enti.model2, (char*)"_usmc_") != NULL) {
				enti.team = 2;
			}
			else if (strstr((char*)enti.model1, (char*)"_opforce_") != NULL || strstr((char*)enti.model2, (char*)"_opforce_") != NULL) {
				enti.team = 3;
			}
			else if (strstr((char*)enti.model1, (char*)"_sas_") != NULL || strstr((char*)enti.model2, (char*)"_sas_") != NULL) {
				enti.team = 4;
			}
			else if (strstr((char*)enti.model1, (char*)"_spetznas_") != NULL || strstr((char*)enti.model2, (char*)"_spetznas_") != NULL) {
				enti.team = 5;
			}
			else {
				enti.team = 0;
			}
			loent.push_back(enti);
		}
	}
	return loent;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	MyRegisterClass(hInstance);

	if (DEBUG_MODE) {
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}

	//EnumWindows(FindWindowBySubstr, (LPARAM)targetTitle);
	targetHwnd = FindWindow(0, _T("Call of Duty 4 X"));

	if (targetHwnd) {
		RECT rect;
		GetWindowRect(targetHwnd, &rect);

		TITLEBARINFOEX * ptinfo = (TITLEBARINFOEX *)malloc(sizeof(TITLEBARINFOEX));
		ptinfo->cbSize = sizeof(TITLEBARINFOEX);
		SendMessage(targetHwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)ptinfo);
		int h = ptinfo->rcTitleBar.bottom - ptinfo->rcTitleBar.top;
		free(ptinfo);

		rect.top = rect.top + h;

		width = rect.right - rect.left;
		height = (rect.bottom - rect.top);
	}
	else {
		put("Process not found");
		return FALSE;
	}

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	MSG msg;

	ddraw = DDraw(overlayHwnd, targetHwnd, width, height);

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		RECT rect;
		GetWindowRect(targetHwnd, &rect);

		TITLEBARINFOEX * ptinfo = (TITLEBARINFOEX *)malloc(sizeof(TITLEBARINFOEX));
		ptinfo->cbSize = sizeof(TITLEBARINFOEX);
		SendMessage(targetHwnd, WM_GETTITLEBARINFOEX, 0, (LPARAM)ptinfo);
		int h = ptinfo->rcTitleBar.bottom - ptinfo->rcTitleBar.top;
		free(ptinfo);

		rect.top = rect.top + h;

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		MoveWindow(overlayHwnd, rect.left, rect.top, width, height, true);

		//load entities data
		loadedEntities = getEntities();
		loadedData = getData();
		ddraw.passEntities(loadedEntities);
		ddraw.passData(loadedData);

		if (!IsWindow(targetHwnd)) {
			PostQuitMessage(0);
		}

		if (GetForegroundWindow() == targetHwnd || GetForegroundWindow() == overlayHwnd) {
			if (GetKeyState(VK_INSERT) < 0) {
				ddraw.showMenu = !ddraw.showMenu;
				while (GetKeyState(VK_INSERT) < 0) {}
			}
			if (ddraw.showMenu) {
				if (GetKeyState('9') < 0) {
					PostQuitMessage(0);
				}
				if (GetKeyState('1') < 0) {
					ddraw.showReticle = !ddraw.showReticle;
					while (GetKeyState('1') < 0) {}
				}
				if (GetKeyState('2') < 0) {
					ddraw.showLines = !ddraw.showLines;
					while (GetKeyState('2') < 0) {}
				}
				if (GetKeyState('3') < 0) {
					ddraw.showReference = !ddraw.showReference;
					while (GetKeyState('3') < 0) {}
				}
				if (GetKeyState('4') < 0) {
					ddraw.showDist = !ddraw.showDist;
					while (GetKeyState('4') < 0) {}
				}
			}
		}
	}
	return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
	wcex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wcex.lpszMenuName = overlayTitle;
	wcex.lpszClassName = overlayTitle;
	wcex.hIconSm = 0;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	overlayHwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW, overlayTitle, overlayTitle, WS_POPUP,
		1, 1, width, height, nullptr, nullptr, hInstance, nullptr);

	if (!overlayHwnd) {
		return FALSE;
	}
	SetLayeredWindowAttributes(overlayHwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

	ShowWindow(overlayHwnd, nCmdShow);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_PAINT:
		ddraw.render();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}