#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

#include <array>
#include <vector>
#include <cmath>
#include <string>
#include <random>

typedef std::array<int,2> golworldsize;
typedef std::vector<byte> golworld;
typedef unsigned char uchar;
BITMAPINFO bmpInfo;

golworldsize worldsize;
golworld world;
golworld nworld;
static uchar* image;

void mkimg(golworld);
void paintscreen(HWND);
void first(PSTR);
void next();

LRESULT CALLBACK WndProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
    TCHAR tcStr[128];

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CREATE:
        return 0;
    case WM_PAINT:
        paintscreen(hwnd);
        return 0;
    }
    return DefWindowProc(hwnd,msg,wp,lp);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,PSTR lpCmdLine,int nCmdShow) {
       std::cout << "Game of Life" << std::endl << std::endl;
    first(lpCmdLine);

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
    winc.cbClsExtra    = winc.cbWndExtra    = 0;
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

void first(PSTR fname) {
    std::cout << "loading file: " << fname << std::endl;
    std::ifstream ifs(fname);
    std::string str;
    int margin[4] = {0,0,0,0};
    { // margin
        getline(ifs,str);
        int rcnt = 0;
        int nlen = 0;
        int nstart = 0;
        for (int i=0;i<str.length();i++) {
            if (str.substr(i,1)==",") {
                if (rcnt<4) {
                    try{margin[rcnt] = std::stoi(str.substr(nstart,nlen));}
                    catch(const std::invalid_argument& e){std::cerr << "invalid argument" << std::endl;}
                    catch(const std::out_of_range& e){std::cerr << "Out of range" <<std::endl;}
                }
                rcnt++;nstart = i+1;nlen = 0;
            }
            else {nlen++;}
        }
        if (rcnt<4) {
            try{margin[rcnt] = std::stoi(str.substr(nstart,nlen));}
            catch(const std::invalid_argument& e){std::cerr << "invalid argument" << std::endl;}
            catch(const std::out_of_range& e){std::cerr << "Out of range" <<std::endl;}
        }
        std::cout << "margin: " << margin[0] << " " << margin[1] << " " << margin[2] << " " << margin[3] << std::endl;
    }
    world = {};
    int ycnt = 0;
    int width = 0;
    int lwidth;
    {
        getline(ifs,str);
        width = str.length();
        lwidth = margin[1]+width+margin[3];
        for (int i=0;i<margin[0]*lwidth;i++) {world.insert(world.end(),0);}

        for (int i=0;i<margin[1];i++) {world.insert(world.end(),0);}
        for (int xcnt=0;xcnt<str.length();xcnt++) {world.insert(world.end(),!(str.substr(xcnt,1)=="0"));}
        for (int i=0;i<margin[3];i++) {world.insert(world.end(),0);}
        ycnt++;
    }
    while (getline(ifs,str)) {
        if (width!=str.length()) {
            std::cerr << "The World Edges are not aligned" <<std::endl;
        }
        for (int i=0;i<margin[1];i++) {world.insert(world.end(),0);}
        for (int xcnt=0;xcnt<str.length();xcnt++) {world.insert(world.end(),!(str.substr(xcnt,1)=="0"));}
        for (int i=0;i<margin[3];i++) {world.insert(world.end(),0);}
        ycnt++;
    }
    for (int i=0;i<margin[2]*lwidth;i++) {world.insert(world.end(),0);}
    worldsize = {lwidth,margin[0]+ycnt+margin[2]};
    std::cout << "world-size: " << worldsize[0] << "x" << worldsize[1] << std::endl;
    nworld = golworld(worldsize[0]*worldsize[1]);
    image = new uchar [worldsize[0]*worldsize[1]*4];
    return;
}
void next() {
    for (int iy = 1; iy < worldsize[1]-1; iy++) {
        for (int ix = 1; ix < worldsize[0]-1; ix++) {
            int ar = 0;
            int ii = iy*worldsize[0]+ix;
            for (int by=-1;by<=1;by++) {
                for (int bx=-1;bx<=1;bx++) {
                    ar+=world[(iy+by)*worldsize[0]+ix+bx];
                }
            }
            ar-=world[ii];
            nworld[ii] = 0;
            if(ar==3) {
                nworld[ii] = 1;
            } else if(ar==2) {
                nworld[ii] = world[ii];
            }
        }
    }
    for (int i=0;i<worldsize[0]*worldsize[1];i++) {
        // make image
        uchar bright = nworld[worldsize[0]*worldsize[1]-i]*255;
        image[i*4+0] = bright; // B
        image[i*4+1] = bright; // G
        image[i*4+2] = bright; // R
        // copy array
        world[i] = nworld[i];
    }
    return;
}