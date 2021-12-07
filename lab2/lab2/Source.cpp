#include <Windows.h>
#include <vector>
#include <algorithm>
#include "StringTableReader.h"

char filePath[256] = R"(D:\Projects\C++\lab2.1\lab2\table.txt)";

using StringTable = std::vector<std::vector<std::string>>;
StringTable inputStrings;
std::vector<size_t> rowsHeight;

size_t winWidth;
size_t winHeight;
RECT wRect;

LOGFONT logFont;

size_t sumOfVectorEntries(const std::vector<size_t>& vect);

void Draw(HDC hdc);
void DrawTable(HDC hdc);
void DrawInputText(HDC hDC);
void DrawLines(HDC hDC);

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
    wc.lpszClassName = "MyAppClass";
    wc.lpszMenuName = nullptr;
    wc.style = CS_VREDRAW | CS_HREDRAW;

    if (!RegisterClassEx(&wc))
        return EXIT_FAILURE;

    if (hwnd = CreateWindow(wc.lpszClassName, "Lab 2", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 250, 100, 1000, 600,
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
    return msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        StringTableReader rd;
        rd.setFilePath(filePath);

        try {
            inputStrings = move(rd.getTable());
            InvalidateRect(hWnd, NULL, TRUE);
        }
        catch (...) {
            MessageBox(hWnd, "Error reading file.", "Error", MB_ICONERROR);
        }
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        Draw(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_SIZE: {
        GetClientRect(hWnd, &wRect);
        winWidth = wRect.right - wRect.left;
        winHeight = wRect.bottom - wRect.top;

        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void DrawInputText(HDC hDC) {
    if (winHeight < 20) return;

    const size_t rows = inputStrings.size();
    const size_t columns = inputStrings[0].size();
    const LONG columnWidth = winWidth / columns;
    rowsHeight.resize(rows);

    size_t fontSize = 1;
    RECT rect = { 0, 0, columnWidth, columnWidth };
    logFont.lfHeight = fontSize;
    HFONT font = CreateFontIndirect(&logFont);

    HFONT oldFont = (HFONT)SelectObject(hDC, font);

    bool fontMaxSize = false;

    while (!fontMaxSize) {

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                rect = { 0, 0, columnWidth, columnWidth };
                DrawText(hDC, inputStrings[i][j].c_str(), -1, &rect, DT_WORDBREAK | DT_LEFT | DT_CALCRECT);
                size_t calculatedWidth = rect.right - rect.left;
                if (calculatedWidth > columnWidth) {
                    fontMaxSize = true;
                    break;
                }
            }
            if (fontMaxSize) break;
        }
        if (fontMaxSize) break;

        fontSize += 1;
        logFont.lfHeight = fontSize;
        DeleteObject(font);
        font = CreateFontIndirect(&logFont);
        SelectObject(hDC, font);
    }

    fontSize -= 1;
    font = (HFONT)SelectObject(hDC, oldFont);
    if (font) DeleteObject(font);
    logFont.lfHeight = fontSize;
    font = CreateFontIndirect(&logFont);
    oldFont = (HFONT)SelectObject(hDC, font);

    std::vector<size_t> heights(columns);

    while (true) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                rect = { 0, 0, columnWidth, columnWidth };
                DrawText(hDC, inputStrings[i][j].c_str(), -1, &rect, DT_WORDBREAK | DT_LEFT | DT_CALCRECT);
                size_t calculated_height = rect.bottom - rect.top;
                heights[j] = calculated_height;
            }
            sort(heights.begin(), heights.end());
            rowsHeight[i] = heights[heights.size() - 1];
        }
        size_t sum = 0;
        for (auto& it : rowsHeight) {
            sum += it;
        }
        if (sum < winHeight) break;

        fontSize -= 1;
        logFont.lfHeight = fontSize;
        DeleteObject(font);
        font = CreateFontIndirect(&logFont);
        SelectObject(hDC, font);
    }

    long additionalHeight = (winHeight - sumOfVectorEntries(rowsHeight)) / rowsHeight.size();

    RECT cell = { 0, 0, columnWidth, 0 };
    for (size_t i = 0; i < rows; ++i) {
        cell.bottom += rowsHeight[i];
        cell.bottom += additionalHeight;
        for (size_t j = 0; j < columns; ++j) {
            DrawText(hDC, inputStrings[i][j].c_str(), -1, &cell, DT_WORDBREAK);
            cell.left += columnWidth;
            cell.right += columnWidth;
        }
        cell.left = 0;
        cell.right = columnWidth;
        cell.top += rowsHeight[i];
        cell.top += additionalHeight;
    }
    font = (HFONT)SelectObject(hDC, oldFont);
    if (font) DeleteObject(font);
}


size_t sumOfVectorEntries(const std::vector<size_t>& inputVector) {
    size_t result = 0;
    for (auto entry : inputVector) result += entry;
    return result;
}


void DrawLines(HDC hDC) {
    size_t columns = inputStrings[0].size();
    size_t rows = inputStrings.size();
    size_t columnWidth = winWidth / columns;
    size_t changingX = columnWidth;
    for (size_t i = 0; i < columns; ++i) {
        MoveToEx(hDC, changingX, 0, NULL);
        LineTo(hDC, changingX, winHeight);
        changingX += columnWidth;
    }

    size_t changingY = 0;
    long additional_height = (winHeight - sumOfVectorEntries(rowsHeight)) / rowsHeight.size();
    for (size_t i = 0; i < rows; ++i) {
        changingY += rowsHeight[i];
        changingY += additional_height;
        MoveToEx(hDC, 0, changingY, NULL);
        LineTo(hDC, winWidth, changingY);
    }
}

void Draw(HDC hdc) {
    // Double bufferization
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP memBM = CreateCompatibleBitmap(hdc, winWidth, winHeight);
    SelectObject(hMemDC, memBM);

    HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hMemDC, &wRect, hbrBkGnd);
    DeleteObject(hbrBkGnd);

    //Draw here
    DrawTable(hMemDC);

    BitBlt(hdc, 0, 0, winWidth, winHeight, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
    DeleteObject(memBM);
}


void DrawTable(HDC hdc) {
    DrawInputText(hdc);
    DrawLines(hdc);
}
