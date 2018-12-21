#include <windows.h>
//#include <Windowsx.h>
#include <tlhelp32.h>

LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
HANDLE OpenProcessByName(LPCTSTR Name, DWORD dwAccess);
DWORD SearchProcessByName(LPCTSTR Name);
ATOM RegisterApplicationWinClass();
//HWND MakeWindow(HWND parent);
HWND MakeMainWindow();
void MessageProcess();
bool IsApplicationActve(LPCTSTR Name);
DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout);



HINSTANCE hin;

LPCTSTR windowClassName = "uc-window-class";

LPCTSTR mainTitle = "Main Title";
LPCTSTR otherName = "Second Title";

HWND WinHandler, MainHandler;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hin = hInstance;
	if (RegisterApplicationWinClass() == 0) return -1;
	MainHandler = MakeMainWindow();
	//WinHandler = MakeWindow(MainHandler);

	//ShowWindow(MainHandler, nShowCmd);
	//UpdateWindow(MainHandler);

	//ShowWindow(WinHandler, nShowCmd);
	//UpdateWindow(WinHandler);

	MessageProcess();
	return 0;
}

void MessageProcess()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

ATOM RegisterApplicationWinClass()
{
	WNDCLASSEX winapp;
	winapp.cbSize = sizeof(WNDCLASSEX);
	winapp.hInstance = hin;
	winapp.lpszClassName = windowClassName;
	winapp.lpfnWndProc = WinProc;
	winapp.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winapp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winapp.hIconSm = winapp.hIcon;
	winapp.hCursor = LoadCursor(NULL, IDC_ARROW);
	winapp.lpszMenuName = NULL;
	winapp.cbClsExtra = 0;
	winapp.cbWndExtra = 0;
	winapp.hbrBackground = (HBRUSH)COLOR_WINDOW;
	return RegisterClassEx(&winapp);
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_ENDSESSION:
			SetWindowText(hwnd, "WM_ENDSESSION");
			break;

		case WM_QUERYENDSESSION:
			SetWindowText(hwnd, "Check for uTorrent");
			// wait for utorrent close!!!

			/*MessageBox(
				hwnd, IsApplicationActve("uTorrent.exe") ? "Correct Handle" : "Invalid Handle", "Process Check", 0
			);*/

			ShowWindow(MainHandler, SW_SHOWNORMAL);
			UpdateWindow(MainHandler);

			if (IsApplicationActve("uTorrent.exe")) {
				DWORD pID = SearchProcessByName("uTorrent.exe");
				TerminateApp(pID, 0);
				SetWindowText(hwnd, "Waiting for uTorrent...");
				Sleep(1000);
			}

			while (IsApplicationActve("uTorrent.exe")) {
				Sleep(1000);
			}
			
			SetWindowText(hwnd, "Waiting for uTorrent - Success!");
			/*MessageBox(
				hwnd, "Exit Complete!", "Process Check", 0
			);*/

			//Sleep(5000);

			return 1;
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_CREATE:
			return 1;
			break;

		case WM_LBUTTONUP:
			//GET_X_LPARAM(lparam);
			//SetWindowText(hwnd, windowClassName);
			//SetWindowText(MainHandler, otherName);

			MessageBox(
				hwnd, IsApplicationActve("uTorrent.exe") ? "Correct Handle" : "Invalid Handle", "Process Check", 0
			);

			if (IsApplicationActve("uTorrent.exe")) {
				DWORD pID = SearchProcessByName("uTorrent.exe");
				TerminateApp(pID, 0);
				Sleep(1000);
			}

			break;

		case WM_LBUTTONDOWN:
			//SetWindowText(hwnd, otherName);
			//SetWindowText(MainHandler, windowClassName);
			break;

		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

/*HWND MakeWindow(HWND parent)
{
	return CreateWindowEx(0, windowClassName, mainTitle,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		parent, 0, hin, 0);
}*/

HWND MakeMainWindow()
{
	//HWND hHiddenWnd = CreateWindowEx(0, windowClassName, "", WS_POPUP, 0, 0, 0, 0, 0, 0, hin, 0);

	return CreateWindowEx(WS_EX_TOOLWINDOW, windowClassName, otherName,
		WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,
		
		/*GetSystemMetrics(SM_CXSCREEN) / 2,
		GetSystemMetrics(SM_CYSCREEN) / 2,
		0, 0, */

		//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		-100, -100, 20, 20,
		
		0, 0, hin, 0);
}

DWORD SearchProcessByName(LPCTSTR Name)
{
	DWORD dwAccess = PROCESS_VM_READ;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe;
		ZeroMemory(&pe, sizeof(PROCESSENTRY32));
		pe.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnap, &pe);
		do
		{
			if (!lstrcmpi(pe.szExeFile, Name))
			{
				return pe.th32ProcessID;
			}
		} while (Process32Next(hSnap, &pe));

	}
	return 0;
}

HANDLE OpenProcessByName(LPCTSTR Name, DWORD dwAccess)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe;
		ZeroMemory(&pe, sizeof(PROCESSENTRY32));
		pe.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnap, &pe);
		do
		{
			if (!lstrcmpi(pe.szExeFile, Name))
			{
				return OpenProcess(dwAccess, 0, pe.th32ProcessID);
			}
		} while (Process32Next(hSnap, &pe));

	}
	return INVALID_HANDLE_VALUE;
}

bool IsApplicationActve(LPCTSTR Name)
{
	HANDLE hFire = OpenProcessByName(Name, PROCESS_VM_READ);
	if (hFire != INVALID_HANDLE_VALUE) {
		CloseHandle(hFire);
		return true;
	}
	else
		return false;

}

#define TA_FAILED 0
#define TA_SUCCESS_CLEAN 1
#define TA_SUCCESS_KILL 2
#define TA_SUCCESS_16 3

BOOL CALLBACK TerminateAppEnum(HWND hwnd, LPARAM lParam)
{
	DWORD dwID;

	GetWindowThreadProcessId(hwnd, &dwID);

	if (dwID == (DWORD)lParam)
	{
		PostMessage(hwnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

DWORD WINAPI TerminateApp(DWORD dwPID, DWORD dwTimeout)
{
	HANDLE   hProc;
	DWORD   dwRet = 0;

	// If we can't open the process with PROCESS_TERMINATE rights,
	// then we give up immediately.
	hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE,
		dwPID);

	if (hProc == NULL)
	{
		return TA_FAILED;
	}

	// TerminateAppEnum() posts WM_CLOSE to all windows whose PID
	// matches your process's.
	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM)dwPID);

	if (dwTimeout > 0) {
		// Wait on the handle. If it signals, great. If it times out,
		// then you kill it.
		if (WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0)
			dwRet = (TerminateProcess(hProc, 0) ? TA_SUCCESS_KILL : TA_FAILED);
		else
			dwRet = TA_SUCCESS_CLEAN;
	}

	CloseHandle(hProc);

	return dwRet;
}
