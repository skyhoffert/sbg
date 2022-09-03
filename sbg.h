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
// 2022-08-27   Sky Hoffert   Updated API for working websocket/sbg, implemented drawline.
// 2022-09-02   Sky Hoffert   Added set background color and draw circle.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SBG_H
#define SBG_H

#include <pthread.h>

#define SBG_OK      0
#define SBG_ERR   100

#define SBG_PORT 8001

#define SBG_SZ 1024

typedef struct sbg {
    int width;
    int height;

    pthread_t _wsthread;
    int _quit_flag;

    int _servfd;
    int _sockfd;
    int _verified;

    char msg[SBG_SZ];
    pthread_mutex_t msg_mtx;
} sbg;

typedef struct sbg_pt {
    float x;
    float y;
} sbg_pt;

typedef struct sbg_line {
    sbg_pt a;
    sbg_pt b;
    int color;
    float width;
} sbg_line;

typedef struct sbg_circle {
    sbg_pt c;
    float radius;
    int fill_color;
    int line_color;
    float line_width;
} sbg_circle;

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

///////////////////////////////////////////////////////////////////////////////////////////////////
// Function:    sbg_set_bg_color
// Description: Set the background color to a given value.
//
void sbg_set_bg_color(sbg* s, int color);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Function:    sbg_draw_line
// Description: Draw a given line.
//
void sbg_draw_line(sbg* s, const sbg_line* l);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Function:    sbg_draw_circle
// Description: Draw a circle based on given sbg_circle struct.
//
void sbg_draw_circle(sbg* s, const sbg_circle* c);

#endif
