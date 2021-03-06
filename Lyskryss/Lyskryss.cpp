// Lyskryss.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lyskryss.h"
#include "Car.h"
#include "Trafficlight.h"
#include "Windows.h"
#include <vector>


#define MAX_LOADSTRING 100


void DrawCars(HDC);
void UpdateCarPositions(std::vector<Car> &, int);
void DrawTrafficLight(HDC, int);
void DrawRoad(HDC);
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_LYSKRYSS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LYSKRYSS));

    MSG msg;

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LYSKRYSS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LYSKRYSS);
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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

	static int ticker = 0;
	static std::vector<Car> carsN;
	static std::vector<Car> carsW;
	
	// Trafficlight info
	static int light_status = 0;
    
	
	switch (message)
    {
	case WM_LBUTTONDOWN: {
		Car* c = new Car(-214, 410, true);
		carsN.push_back(*c);
		// InvalidateRect(hWnd,NULL,false);
		break;
	}
	case WM_RBUTTONDOWN: {
		Car *c = new Car(0, 0, false);
		// InvalidateRect(hWnd, NULL, false);
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
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			
			RECT rect;
			GetWindowRect(hWnd, &rect);

			// Create DC for double-buffering
			HDC hdcMem = CreateCompatibleDC(hdc); // For double-buffering
			HDC hdcTemp = CreateCompatibleDC(hdc); // For BitBlt copying to hdcMem
			
			//Creates a bitmap to use as drawing
			HBITMAP bmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			HBITMAP bm = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(hdcMem, bmMem);
			SelectObject(hdcTemp, bm);
			FloodFill(hdcMem, 0, 0, RGB(255, 255, 255));
			FloodFill(hdcTemp, 0, 0, RGB(255, 255, 255));
			
			// Write Background

			// Write road
			DrawRoad(hdcTemp);
			BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdcTemp, 0, 0, SRCCOPY);

			//Write Trafficlights - size of 56x168
			DrawTrafficLight(hdcTemp, light_status);
			BitBlt(hdcMem, 50, 50, 56,168, hdcTemp, 0, 0, SRCCOPY);
			DrawTrafficLight(hdcTemp, (light_status + 2) % 4);
			BitBlt(hdcMem, 350, 50, 56, 168, hdcTemp, 0, 0, SRCCOPY);

			// Write cars
			//Creates a bitmap to use as drawing
			HBITMAP bmC = CreateCompatibleBitmap(hdcTemp, 214, 80); // Need to make this depending on direction
			SelectObject(hdcTemp, bmC);
			HBITMAP bmCar;
			//Load the bitmap into DC
			bmCar = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
			SelectObject(hdcTemp, bmCar);
			
			//DrawCars(hdcTemp);
			if (!carsN.empty()) {
				for (std::vector<Car>::iterator it = carsN.begin(); it != carsN.end(); ++it) {
					int x = (*it).getxpos();
					BitBlt(hdcMem, x, 400, 214, 80, hdcTemp, 0, 0, SRCCOPY);
				}
			}

			//Cleanup
			DeleteObject(bmC);
			DeleteObject(bmCar);
			// END WRITE CARS

			// Copy hdcMem to hdc - IE write to screen
			BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

			// Cleanup
			DeleteObject(hdcTemp);
			DeleteObject(hdcMem);
			DeleteObject(bm);
			DeleteObject(bmMem);
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_TIMER:
	{
		switch (wParam) {
		case 2:
			// Speed of trafficlight
			if (ticker % 1000 == 0) {
				if (light_status >= 3)
					light_status = 0;
				else
					light_status++;
			}
			//Speed of car
			if (ticker % 10 == 0) {
				UpdateCarPositions(carsN, light_status);
			}
			// Resets ticker if its getting "big"
			if (ticker > 10000)
				ticker = 0;
			else 
				ticker+=10;
			InvalidateRect(hWnd, NULL, false);
			break;
		}
	}
	break;
	case WM_CREATE:
	{
		SetTimer(hWnd, 2, 30, NULL);
	}
	break;
    case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
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

/*!
Thingamajigger*/
/** 
*Draws a car into the HDC provided
*/
void DrawCar(HDC hdcMem, Car car) {
	//Creates a bitmap to use as drawing
	HBITMAP bm = CreateCompatibleBitmap(hdcMem, 214, 80); // Need to make this depending on direction
	// Adds the bitmap to the DC to create a drawing surface
	SelectObject(hdcMem, bm);
	HBITMAP bmCar;

	//Load the bitmap into DC
	if (car.getDir()) {
		bmCar = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		SelectObject(hdcMem, bmCar);
	}
	else {
		bmCar = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		SelectObject(hdcMem, bmCar);

	}

	//Cleanup
	DeleteObject(bm);
	DeleteObject(bmCar);
}

void DrawRoad(HDC hdc) {
	HPEN hPen;
	HBRUSH hBrush;
	HGDIOBJ hOrg;
	// Draw road
	hBrush = CreateSolidBrush(RGB(50, 50, 50)); // Black-grey brush
	hOrg = SelectObject(hdc, hBrush);
	Rectangle(hdc, 0, 300, 1500, 500);
	Rectangle(hdc, 500, 0, 700, 1000);
	SelectObject(hdc, hOrg);
	DeleteObject(hBrush);

	// Draw lines on road
	hPen = CreatePen(PS_SOLID, 4, RGB(255, 255, 0));
	hOrg = SelectObject(hdc, hPen);
	MoveToEx(hdc, 0, 398, 0);
	LineTo(hdc, 1500, 398);
	MoveToEx(hdc, 598, 0, 0);
	LineTo(hdc, 598, 1000);
	SelectObject(hdc, hOrg);
	DeleteObject(hBrush);
}

void UpdateCarPositions(std::vector<Car>& cars, int status) {
	if (!cars.empty()) {
		for (std::vector<Car>::iterator it = cars.begin(); it != cars.end(); ++it) {
			// Add checks for moving past previous cars
			if ((*it).getxpos() > 300 || ((*it).getxpos() < 300 && status == 2)) {
				(*it).move();
			}
			//else if ((*it).getxpos() < 300 && status == 2 ) {


			//}
		}
	}
}

void DrawTrafficLight(HDC hdc , int status) {
	//HPEN hPen;
	HBRUSH hBrush;
	HGDIOBJ hOrg;

	// Diameter of the lights
	static int el_dia = 50;

	// Start drawing Trafficlight 1
	hBrush = CreateSolidBrush(RGB(0, 0, 0)); // Black brush
	hOrg = SelectObject(hdc, hBrush);
	RoundRect(hdc, 0,0, el_dia + 6, (el_dia + 6) * 3, 15, 15);
	// Reset original brush
	SelectObject(hdc, hOrg);
	DeleteObject(hBrush);

	if (status == 0 || status == 1) { // For RED and RED/YELLOW light
		hBrush = CreateSolidBrush(RGB(200, 0, 0)); // red brush
	}
	else {
		hBrush = CreateSolidBrush(RGB(127, 127, 127));
	}
	hOrg = SelectObject(hdc, hBrush);
	Ellipse(hdc, 3, 3, el_dia + 3, el_dia +3);
	// Reset original brush
	SelectObject(hdc, hOrg);
	DeleteObject(hBrush);

	if (status == 1 || status == 3) { // For YELLOW and RED/YELLOW light
		hBrush = CreateSolidBrush(RGB(200, 200, 0)); // Yellow brush
	}
	else {
		hBrush = CreateSolidBrush(RGB(127, 127, 127));
	}
	hOrg = SelectObject(hdc, hBrush);
	Ellipse(hdc, 3, el_dia + 6, el_dia +3, 2 * (el_dia+3));
	// Reset original brush
	SelectObject(hdc, hOrg);
	DeleteObject(hBrush);


	if (status == 2) { // For GREEN light only
		hBrush = CreateSolidBrush(RGB(0, 200, 0)); // Green brush
	}
	else {
		hBrush = CreateSolidBrush(RGB(127, 127, 127));
	}
	hOrg = SelectObject(hdc, hBrush);
	Ellipse(hdc, 3,  2 * (el_dia +6 ), el_dia + 3, 3 * (el_dia + 3));
	// Reset original brush
	SelectObject(hdc, hOrg);
	DeleteObject(hBrush);
}