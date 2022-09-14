#include "040pixel.h" //don't know if I need this
#include <math.h>


void triRenderWithAleft(
        double a0, double a1, double b0, double b1, double c0, double c1, 
        double r, double g, double b) {
    if (a1 < c1){ // c is above a

        if (a0 != c0){ //render left half
            for (int x0 = (int) ceil(a0); x0 <= floor(c0); x0++){
                int bottom = (int) ceil(a1 + (b1-a1) / (b0 - a0) * (x0 - a0));
                int top = (int) floor(a1 + (c1-a1) / (c0 - a0) * (x0 - a0));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, r, g, b);
                
            }
        }

        if (b0 != c0){
            for (int x0 = (int) floor(c0) + 1; x0 <= floor(b0); x0++){
                int bottom = (int) ceil(a1 + (b1-a1) / (b0 - a0) * (x0 - a0));
                int top = (int) floor(b1 + (c1-b1) / (c0 - b0) * (x0 - b0));

                for (int x1 = bottom; x1 <= top; x1++)
                    pixSetRGB(x0, x1, r, g, b);
            }
        }


    }
    // else
    //     break;
}

void triRender(
        double a0, double a1, double b0, double b1, double c0, double c1, 
        double r, double g, double b) {
    if (a0 <= b0 && a0 <= c0)
        triRenderWithAleft(a0, a1, b0, b1, c0, c1, r, g, b);
    else if (b0 <= a0 && b0 <= c0)
        triRenderWithAleft(b0, b1, c0, c1, a0, a1, r, g, b);
    else
        triRenderWithAleft(c0, c1, a0, a1, b0, b1, r, g, b);
	
}

