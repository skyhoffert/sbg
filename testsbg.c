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
    
    char c = getc(stdin);

    while (c != 'q')
    {
        if (c == 'd')
        {
            sbg_send(&s, "this is a test message");
        }
        else if (c == 'p')
        {
            sbg_send(&s, "ping");
        }
        else if (c == 'l')
        {
            sbg_line line;
            line.a.x = 0;
            line.a.y = 0;
            line.b.x = 100;
            line.b.y = 100;
            line.color = 0xff0000;
            line.width = 3;
            sbg_draw_line(&s, &line);
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
