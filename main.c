#include "img_processing.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    int selection;
    int img;
    int stop=1;
    printf("program starts:  \n");
    while (stop)
    {
        printf("please select: 1.img_zooming  2.exit \n");
        scanf("%d",&selection);
        switch (selection)
        {
            case 1:
                img=img_zooming();
                break;
            case 2:
                stop=0;
                break;
            default:
                printf("again \n");
                break;
        }

    }
    printf("program ends!\n");
    return 0;

}