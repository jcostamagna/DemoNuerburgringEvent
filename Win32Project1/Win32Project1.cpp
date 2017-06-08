// Win32Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project1.h"
#include <string>



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWndState;
Session* session;
int idSession;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::wstring stringToWstring(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}




int getNextId() {
	PostgreSQL db;
	if (!db.connect()) return 0;

	if (!db.doQuerySelect("SELECT MAX(Id) FROM Session")) { return 0; }
	//std::wstring title = stringToWstring(std::string("Sesion ") + db.getValue(0, 0));
	//LPCWSTR titleWND = title.c_str();
	//MessageBox(NULL, L"Sesion already running!", titleWND, NULL);
	int id = std::stoi(db.getValue(0, 0));
	return ++id;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

    MSG msg;
	idSession = getNextId();
	session = NULL;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {


        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 500, 200, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }
   

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   

   return TRUE;
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hbrBkgnd = CreateSolidBrush(RGB(255, 255, 255));
    switch (message)
    {
	case WM_CREATE:
	{
		CreateWindow(TEXT("button"), TEXT("Start"),
			WS_VISIBLE | WS_CHILD,
			20, 50, 80, 25,
			hWnd, (HMENU)IDC_START, NULL, NULL);
		
		CreateWindow(TEXT("button"), TEXT("Stop"),
			WS_VISIBLE | WS_CHILD,
			20, 80, 80, 25,
			hWnd, (HMENU)IDC_STOP, NULL, NULL);

		hWndState = CreateWindow(TEXT("STATIC"), TEXT("Ready to start!"), WS_VISIBLE | WS_CHILD | SS_LEFT, 120, 100, 200, 50, hWnd, NULL, hInst, NULL);
		

		break;
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				if (session) { stopSession(); }
                DestroyWindow(hWnd);
                break;
			case IDC_START:
			{
				manageSession();
				break;
			}
			case IDC_STOP:
			{
				stopSession();
				break;
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

			EndPaint(hWnd, &ps);
        }
        break;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		//SetBkMode(hdcStatic, TRANSPARENT);
		//SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkColor(hdcStatic, RGB(255, 255, 255));

		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(RGB(0, 0, 0));
		}
		return (INT_PTR)hbrBkgnd;
	}
    case WM_DESTROY:
		if (session) { stopSession(); }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}






LRESULT OnCtlColorStatic(HDC hdc, HWND hCtl)
{
	if (hCtl == hWndState)
	{
		SetTextColor(hdc, RGB(230, 230, 230));
		SetBkColor(hdc, RGB(230, 230, 230));
		return (LRESULT)GetStockObject(HOLLOW_BRUSH);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


void manageSession() {
	if (session) { return sessionRunningError(); }
	
	session = new Session(idSession);
	session->start();
	
	std::wstring descript = stringToWstring(std::string("Session ") + std::to_string(idSession) + std::string(" started fine!"));
	LPCWSTR descriptWND = descript.c_str();

	SetWindowText(hWndState, descriptWND);
}

void sessionRunningError() {
	MessageBox(NULL, L"Session already running!", L"Error", NULL);
}

void stopSession() {
	if (!session) { return notSessionError(); }

	session->end();
	
	std::wstring descript = stringToWstring(std::string("Session ") + std::to_string(idSession) + std::string(" ended fine!"));
	LPCWSTR descriptWND = descript.c_str();

	SetWindowText(hWndState, descriptWND);
	idSession++;
	delete session;
	session = NULL;
}

void notSessionError() {
	MessageBox(NULL, L"There is no session running!", L"Error", NULL);
}