#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <string.h>
#include <gdiplus.h>
#include "Gdiplus.h" // Gdiplusflat.h
#include <sstream>
#include <direct.h>

//Visual Studio shortcut for adding library:
#pragma comment(lib, "Gdiplus.lib")
// GpStatus WINGDIPAPI GdipCreateBitmapFromHBITMAP(HBITMAP hbm,
//                         HPALETTE hpal,
//                         GpBitmap** pBitmap);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

void screenshot(POINT a, POINT b, int framecount)
{
    int w = b.x - a.x;
    int h = b.y - a.y;

    if (w <= 0) return;
    if (h <= 0) return;

    HDC     hScreen = GetDC(HWND_DESKTOP);
    HDC     hDc = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDc, hBitmap);
    BitBlt(hDc, 0, 0, w, h, hScreen, a.x, a.y, SRCCOPY);

    Gdiplus::Bitmap bitmap(hBitmap, NULL);
    CLSID clsid;

    GetEncoderClsid(L"image/png", &clsid);

    //GDI+ expects Unicode filenames
    std::wstringstream filename;
    _mkdir("screenshot");
    filename << L"screenshot\\screenshot_" << framecount << L".png";

    

    bitmap.Save((filename.str().c_str()), &clsid, NULL);

    SelectObject(hDc, old_obj);
    DeleteDC(hDc);
    ReleaseDC(HWND_DESKTOP, hScreen);
    DeleteObject(hBitmap);
}

int main() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    RECT rc;
    GetClientRect(GetDesktopWindow(), &rc);
    POINT a{ 0, 0 };
    POINT b{ 1920, 1080 };

    int frameCount = 0;


    while (true) {
        std::wstringstream filename;

        
            screenshot(a, b, frameCount++);  // Capture and save the screenshot
            if (frameCount >= 2400) {
                std::string s = ("screenshot\\screenshot_" + std::to_string(frameCount - 2400) + ".png");
                 remove(s.c_str());
            }

            Sleep(0.016);  // Sleep to avoid overwhelming the system
            if (GetAsyncKeyState(VK_MENU) < 0 && GetAsyncKeyState('Q') < 0) break;
            
        

        // if (GetAsyncKeyState(VK_MENU) < 0 && GetAsyncKeyState('Q') < 0) break;
    }
    system("py video_converter.py");
   
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}
