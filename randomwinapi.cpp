#include <windows.h>
#include <iostream>

#include <array>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <iterator>

typedef std::array<int,2> golworldsize;
typedef std::vector<byte> golworld;
typedef unsigned char uchar;
BITMAPINFO bmpInfo;

golworldsize worldsize = {1000,1000};
golworld world = golworld(worldsize[0]*worldsize[1]);
golworld nworld = golworld(worldsize[0]*worldsize[1]);
static uchar* image = new uchar [worldsize[0]*worldsize[1]*4];


std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
std::chrono::system_clock::time_point end = std::chrono::system_clock::now();

void mkimg(golworld w);
void paintscreen(HWND);
void first();
void next();

LRESULT CALLBACK WndProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
	TCHAR tcStr[128];

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		//std::cout << "\033[2K\033[1G" << "keydown " << wp << std::endl;
		bool a;
		switch (wp)
		{
			default:
			break;
		}
		return 0;
	case WM_CREATE:
   		std::cout << "Game of Life" << std::endl;
        first();
		return 0;
	case WM_PAINT:
		paintscreen(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd,msg,wp,lp);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow) {
	HWND hwnd;
	MSG msg;
	WNDCLASS winc;
	bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth=worldsize[0];
	bmpInfo.bmiHeader.biHeight=worldsize[1];
	bmpInfo.bmiHeader.biPlanes=1;
	bmpInfo.bmiHeader.biBitCount=32;
	bmpInfo.bmiHeader.biCompression=BI_RGB;

	winc.style = CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc = WndProc;
	winc.cbClsExtra	= winc.cbWndExtra	= 0;
	winc.hInstance = hInstance;
	winc.hIcon = LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor = LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName = NULL;
	winc.lpszClassName = TEXT("app");

	

	if (!RegisterClass(&winc)) return -1;

	hwnd = CreateWindow(
		TEXT("app") , TEXT("Game of Life") ,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
		CW_USEDEFAULT , CW_USEDEFAULT ,
		worldsize[0]+100 , worldsize[1]+100 , // window size
		NULL , NULL , hInstance , NULL
	);

	if (hwnd == NULL) return -1;
	while(GetMessage(&msg,NULL,0,0)) {DispatchMessage(&msg);}
	return msg.wParam;
}

void paintscreen(HWND hwnd) {
	next();
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	hdc=BeginPaint(hwnd,&ps);
	{ // 3d
		SetDIBitsToDevice(hdc,(width-worldsize[0])/2,(height-worldsize[1])/2,worldsize[0],worldsize[1],0,0,0,worldsize[1],image,&bmpInfo,DIB_RGB_COLORS);
	}
	EndPaint(hwnd,&ps);
	InvalidateRect(hwnd,NULL,FALSE);
	return;
}

void first() {
    for (int cr=0;cr<worldsize[0]*worldsize[1];cr++) {
        if (rand()%100>70) {world[cr] = 1;}
    }
    return;
}
void next() {
    for (int i=0;i<worldsize[0]*worldsize[1];i++) { // fill 0
        nworld[i] = 0;
    }
    for (int iy = 1; iy < worldsize[1]-1; iy++) {
        for (int ix = 1; ix < worldsize[0]-1; ix++) {
            int ar = 0;
            int ii = (iy*worldsize[0]+ix);
            for (int by=-1;by<=1;by++) {
                for (int bx=-1;bx<=1;bx++) {
                    int bi = ((iy+by)*worldsize[0]+ix+bx);
                    if (!(by==0&bx==0)) {
                        if (world[bi]==1) {
                            ar++;
                        }
                    }
                }
            }
            if(ar==3) {
                nworld[ii] = 1;
            } else if(ar==2) {
                nworld[ii] = world[ii];
            }
        }
    }
    for (int i=0;i<worldsize[0]*worldsize[1];i++) {
		// make image
		uchar bright = nworld[i]*255;
		image[i*4+0] = bright; // B
		image[i*4+1] = bright; // G
		image[i*4+2] = bright; // R
		// copy array
        world[i] = nworld[i];
    }
    return;
}