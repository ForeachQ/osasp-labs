#include <Windows.h>
#include <commctrl.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <Winuser.h>
#include <math.h>
#include <thread>
#include <windows.h> 
#include <objidl.h>
#include <gdiplus.h>

#define BACKGRD Color(255, 255, 255)

using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

RECT rc;
int recH;
int recW;
POINT figure{
	100, 100
};

int constX = 0;
int constY = 0;
double speed = 35;
bool animation = false;

BOOL moveFigure = FALSE;
bool sprait = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void Paint(HWND hWnd, HDC hdc);
void Animation(HWND hWnd);

int CALLBACK wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = NULL;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"MyAppClass";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;


	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	if (hwnd = CreateWindow(wc.lpszClassName, L"Заголовок!", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 250, 100, 1000, 600,
		nullptr, nullptr, wc.hInstance, nullptr);
		hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		HWND animationButton = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Animation",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			10,         // x position 
			10,         // y position 
			80,        // Button width
			30,        // Button height
			hWnd,     // Parent window
			reinterpret_cast<HMENU>(1),       // menu.
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);      // Pointer not needed.

		HWND switchButton = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Switch",      // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			100,         // x position 
			10,         // y position 
			80,        // Button width
			30,        // Button height
			hWnd,     // Parent window
			reinterpret_cast<HMENU>(2),       // menu.
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);      // Pointer not needed.
		GetClientRect(hWnd, &rc);
		break;
	}

	case WM_SIZE:
		GetClientRect(hWnd, &rc);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 1:
		{
			if (!animation)
			{
				animation = true;
				std::thread newThread(Animation, std::ref(hWnd));
				newThread.detach();
			}

			break;
		}
		case 2:
			sprait = !sprait;
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		default:
			break;
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HDC hMemDC = CreateCompatibleDC(hdc);
		HBITMAP mBM = CreateCompatibleBitmap(hdc, rc.right - rc.left,
			rc.bottom - rc.top);

		HBITMAP oldBM = (HBITMAP)
			SelectObject(hMemDC, mBM);

		Paint(hWnd, hMemDC);

		BitBlt(hdc, rc.left, rc.top,
			rc.right - rc.left, rc.bottom - rc.top, hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, oldBM);
		DeleteObject(mBM);
		DeleteDC(hMemDC);

		EndPaint(hWnd, &ps);

	}
	break;

	case WM_LBUTTONDOWN:
		POINT pointerPos;
		pointerPos.x = LOWORD(lParam);
		pointerPos.y = HIWORD(lParam);

		if (pointerPos.x > figure.x && pointerPos.x < figure.x + recW &&
			pointerPos.y > figure.y && pointerPos.y < figure.y + recH)
		{
			moveFigure = TRUE;
			animation = false;
			constX = pointerPos.x - figure.x;
			constY = pointerPos.y - figure.y;
		}
		break;

	case WM_MOUSEMOVE:
		if (moveFigure)
		{
			figure.x = LOWORD(lParam) - constX > 0 ? LOWORD(lParam) - constX : 0;
			figure.x = LOWORD(lParam) - constX + recW < rc.right ? figure.x :
				rc.right - recW;
			figure.y = HIWORD(lParam) - constY > 0 ? HIWORD(lParam) - constY : 0;
			figure.y = HIWORD(lParam) - constY + recH < rc.bottom ? figure.y :
				rc.bottom - recH;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_LBUTTONUP:
		moveFigure = FALSE;
		break;
	case WM_APP + 1:
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_MOUSEWHEEL:
		animation = false;
		if (wParam & MK_SHIFT) {
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (figure.x + 10 + recW <= rc.right)
					figure.x += 10;
				else
					figure.x = rc.right - recW;
			}
			else if (figure.x - 10 >= 0)
				figure.x -= 10;
			else
				figure.x = 0;
		}
		else {
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
				if (figure.y - 10 >= 0)
					figure.y -= 10;
				else
					figure.y = 0;
			}
			else if (figure.y + recH + 10 <= rc.bottom)
				figure.y += 10;
			else
				figure.y = rc.bottom - recH;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_KEYDOWN:
		animation = false;
		switch (wParam)
		{
		case VK_LEFT:
			if (figure.x - 10 >= 0)
				figure.x -= 10;
			else
				figure.x = 0;
			break;

		case VK_RIGHT:
			if (figure.x + 10 + recW <= rc.right)
				figure.x += 10;
			else
				figure.x = rc.right - recW;

			break;

		case VK_UP:
			if (figure.y - 10 >= 0)
				figure.y -= 10;
			else
				figure.y = 0;

			break;

		case VK_DOWN:
			if (figure.y + recH + 10 <= rc.bottom)
				figure.y += 10;
			else
				figure.y = rc.bottom - recH;

			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(EXIT_SUCCESS);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void Animation(HWND hWnd) {
	while (animation)
	{
		bool firstMove = true;
		double vectorX;
		double vectorY;
		srand(time(0));
		vectorX = rand() % (rc.right + 1);
		srand(vectorX);
		vectorY = rand() % (rc.bottom + 1);

		double x = sqrt(pow(figure.x - vectorX, 2) + pow(figure.y - vectorY, 2)) / (speed / 5);
		double xSpeed = (vectorX - figure.x) / x;
		double ySpeed = (vectorY - figure.y) / x;

		if (figure.x == rc.right - recW && xSpeed > 0) {
			xSpeed = -xSpeed;
		}
		else if (figure.y == rc.bottom - recH && ySpeed > 0) {
			ySpeed = -ySpeed;
		}


		while (animation && (firstMove ||
			(figure.x + xSpeed >= 0 && figure.y + ySpeed >= 0
				&& figure.x + xSpeed + recW <= rc.right && figure.y + ySpeed + recH <= rc.bottom))) {
			firstMove = false;
			figure.x += xSpeed;
			figure.y += ySpeed;
			Sleep(10);
			PostMessage(hWnd, WM_APP + 1, 0, 0);
		}
		if (figure.x + xSpeed < 0) {
			figure.x = 0;
		}
		else if (figure.y + ySpeed < 0) {
			figure.y = 0;
		}
		else if (figure.x + xSpeed + recW > rc.right) {
			figure.x = rc.right - recW;
		}
		else if (figure.y + ySpeed + recH > rc.bottom) {
			figure.y = rc.bottom - recH;
		}
		PostMessage(hWnd, WM_APP + 1, 0, 0);
		Sleep(10);
	}
	return;
}

void Paint(HWND hWnd, HDC hdc) {
	if (!sprait) {
		recH = 100;
		recW = 200;
		if (figure.x + recW > rc.right)
			figure.x = rc.right - recW;
		if (figure.y + recH > rc.bottom)
			figure.y = rc.bottom - recH;
		HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
		HBRUSH brush = CreateSolidBrush(RGB(0, 0, 255));

		HGDIOBJ oldPen = SelectObject(hdc, pen);
		HGDIOBJ oldBrush = SelectObject(hdc, brush);

		HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(hdc, &rc, hbrBkGnd);
		DeleteObject(hbrBkGnd);

		Rectangle(hdc, figure.x, figure.y, figure.x + recW, figure.y + recH);

		SelectObject(hdc, oldPen);
		SelectObject(hdc, oldBrush);

		DeleteObject(pen);
		DeleteObject(brush);
	}
	else {
		ULONG_PTR token;
		GdiplusStartupInput input;
		GdiplusStartup(&token, &input, 0);

		Bitmap bitmap(L"pog.png");
		UINT w = bitmap.GetWidth(), h = bitmap.GetHeight();

		HBITMAP hBmp;

		HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
		FillRect(hdc, &rc, hbrBkGnd);
		DeleteObject(hbrBkGnd);

		bitmap.GetHBITMAP(BACKGRD, &hBmp);
		HDC hDcBitmap = CreateCompatibleDC(hdc);


		HBITMAP hPrevBmp = (HBITMAP)SelectObject(hDcBitmap, hBmp);
		double k = 0.2;
		recH = h * k;
		recW = w * k;

		if (figure.x + recW > rc.right)
			figure.x = rc.right - recW;
		if (figure.y + recH > rc.bottom)
			figure.y = rc.bottom - recH;
		StretchBlt(hdc, figure.x, figure.y, recW, recH, hDcBitmap, 0, 0, w, h, SRCCOPY);

		SelectObject(hDcBitmap, hPrevBmp);
		DeleteObject(hBmp);
		DeleteDC(hDcBitmap);
	}
}
