#include <iostream>

#include <array>
#include <vector>
#include <cmath>
#include <random>

typedef std::array<int,2> golworldsize;
typedef std::vector<char> golworld;
typedef unsigned char uchar;

golworldsize worldsize = {30,30};
golworld world = golworld(worldsize[0]*worldsize[1]);
golworld nworld = golworld(worldsize[0]*worldsize[1]);
int gcnt = 0;

void showimg(golworld w);
void first();
void next();

int main() {
	first();
	showimg(world);
	for (int c=0;c<1000;c++) {
		std::cout << "\033[" << worldsize[1]+1 << "A";
		next();
	}
	return 0;
}

void first() {
    for (int cr=0;cr<worldsize[0]*worldsize[1];cr++) {
        if (rand()%100>50) {
            world[cr] = 1;
        }
        else {
            world[cr] = 0;
        }
    }
    return;
}
void next() {
	gcnt++;
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

			//printf("%d " ,ar);
            if(ar==3) {
                nworld[ii] = 1;
            } else if(ar==2) {
                nworld[ii] = world[ii];
            }
        }
    }
	showimg(nworld);
    for (int i=0;i<worldsize[0]*worldsize[1];i++) { // copy array
        world[i] = nworld[i];
    }
    return;
}
void showimg(golworld w) {
	std::cout << "generation " << gcnt << std::endl; // info
    for (int y=0;y<worldsize[1];y++) {
		for (int x=0;x<worldsize[0];x++) {
			if (w[y*worldsize[0]+x]==0) { // dead
				std::cout << "..";
			}
			else { // alive
				std::cout << "WW";
			}
		}
		std::cout << std::endl;
    }
}
