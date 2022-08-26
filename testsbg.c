///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Filename:      testsbg.c
// Description:   Test file for sbg library.
//
// Additional Notes:
// ------------------
// None.
//
// Development History:
//  Date         Author        Description of Change
// ------       --------      -----------------------
// 2022-08-26   Sky Hoffert   Initial release.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "sbg.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    printf("Hello, testsbg.\n");

    sbg s;
    int retval;

    retval = sbg_init(&s);

    if (retval != SBG_OK)
    {
        printf("ERR. Couldn't init sbg variable. Exiting.\n");
        return 100;
    }

    sbg_pt a, b;
    a.x = 0;
    a.y = 0;
    b.x = 100;
    b.y = 100;
    sbg_draw_line(&s, &a, &b);
    
    char c = getc(stdin);

    while (c != 'q')
    {
        if (c == 'd')
        {
            sprintf(s.msg, "TE\r\n");
        }

        c = getc(stdin);
    }

    retval = sbg_term(&s);

    if (retval != SBG_OK)
    {
        printf("ERR. Couldn't init sbg variable. Exiting.\n");
        return 100;
    }

    printf("Exiting testsbg.\n");

    return 0;
}
