///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// Filename:      sbg.h
// Description:   Header file for sbg (s Browser GUI) library.
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

#ifndef SBG_H
#define SBG_H

#include <pthread.h>

#define SBG_OK      0
#define SBG_ERR   100

#define SBG_PORT 8001

typedef struct sbg {
    int width;
    int height;

    pthread_t _wsthread;
    int _quit_flag;

    int _servfd;
    int _sockfd;

    char msg[1024];
} sbg;

typedef struct sbg_pt {
    float x;
    float y;
} sbg_pt;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Function:    sbg_init
// Description: Initialize a given sbg variable.
// @param s: sbg struct to initialize.
// @return int: error code (0/SBG_OK for success).
//
int sbg_init(sbg* s);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Function:    sbg_term
// Description: Terminate a given sbg variable.
// @param s: sbg struct to terminate.
// @return int: error code (0/SBG_OK for success).
//
int sbg_term(sbg* s);

void sbg_send(const char* msg);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Function:    sbg_draw_line
// Description: Draw a line between two points.
// @param s: sbg struct.
// @param a: point a of line.
// @param b: point b of line.
//
void sbg_draw_line(const sbg* s, const sbg_pt* a, const sbg_pt* b);

#endif
