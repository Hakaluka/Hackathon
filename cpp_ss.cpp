
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;
#include <queue>

// Define the screenshot capture function
void captureScreenshot(const char* filename) {
    // Get the desktop DC
    HDC hDesktop = GetDC(NULL);

    // Get the desktop dimensions
    int width = GetDeviceCaps(hDesktop, HORZRES);
    int height = GetDeviceCaps(hDesktop, VERTRES);

    // Create a bitmap to store the screenshot
    HDC hMemDC = CreateCompatibleDC(hDesktop);
    HBITMAP hBitmap = CreateCompatibleBitmap(hDesktop, width, height);
    SelectObject(hMemDC, hBitmap);

    // Capture the screenshot
    StretchBlt(hMemDC, 0, 0, width, height, hDesktop, 0, 0, width, height, SRCCOPY);

    // Save the screenshot to file
    stringstream ss;
    ss << "screenshot/" << filename << ".bmp";
    string filenameStr = ss.str();
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // Negative height for top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
    int bufferSize = bmi.bmiHeader.biSizeImage = width * height * 3;
    char* pixels = new char[bufferSize];
    GetDIBits(hMemDC, hBitmap, 0, height, pixels, &bmi, DIB_RGB_COLORS);
    ofstream file(filenameStr, ios::binary);
    file.write((char*)&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
    file.write((char*)pixels, bufferSize);
    delete[] pixels;

    // Clean up
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDesktop);
}
//forming a queue of 1200 screenshots
queue<string> screenshotQueue;


int main() {
    int frameCount = 0;
    while (true) {
        // Capture screenshot
         stringstream ss;
        ss << "screenshot_" <<  setw(6) <<  setfill('0') << frameCount++;
        captureScreenshot(ss.str().c_str());
        screenshotQueue.push(ss.str() + ".bmp");
        if (screenshotQueue.size() > 2400) screenshotQueue.pop();
        // if (frameCount >= 1200) frameCount=0;
        // Wait for next frame (adjust the delay as needed)
        Sleep(0.01); // approximately 60 FPS
        if (GetAsyncKeyState(VK_MENU) < 0 && GetAsyncKeyState('Q') < 0) break;
        cout << "Queue: ";
        while (!screenshotQueue.empty()) {
            cout << screenshotQueue.front() << " ";
            screenshotQueue.pop();
        }
        cout << endl;
    }
    // Exit loop when 'q' is pressed
    
    return 0;
}

