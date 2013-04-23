/* File: z-term.h */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

#ifndef INCLUDED_Z_TERM_H
#define INCLUDED_Z_TERM_H

#include "h-basic.h"



/*
 * A term_win is a "window" for a Term
 *
 * - Cursor Useless/Visible codes
 * - Cursor Location (see "Useless")
 *
 * - Array[h] -- Access to the attribute array
 * - Array[h] -- Access to the character array
 *
 * - Array[h*w] -- Attribute array
 * - Array[h*w] -- Character array
 *
 * Note that the attr/char pair at (x,y) is a[y][x]/c[y][x]
 * and that the row of attr/chars at (0,y) is a[y]/c[y]
 */

typedef struct term_win term_win;

struct term_win
{
	bool cu, cv;
	byte cx, cy;

	byte **a;
	char **c;

	byte *va;
	char *vc;

	byte **ta;
	char **tc;

	byte *vta;
	char *vtc;
};



/*
 * An actual "term" structure
 *
 * - Extra "user" info (used by application)
 *
 * - Extra "data" info (used by implementation)
 *
 *
 * - Flag "user_flag"
 *   An extra "user" flag (used by application)
 *
 *
 * - Flag "data_flag"
 *   An extra "data" flag (used by implementation)
 *
 *
 * - Flag "active_flag"
 *   This "term" is "active"
 *
 * - Flag "mapped_flag"
 *   This "term" is "mapped"
 *
 * - Flag "total_erase"
 *   This "term" should be fully erased
 *
 * - Flag "fixed_shape"
 *   This "term" is not allowed to resize
 *
 * - Flag "icky_corner"
 *   This "term" has an "icky" corner grid
 *
 * - Flag "soft_cursor"
 *   This "term" uses a "software" cursor
 *
 * - Flag "always_pict"
 *   Use the "Term_pict()" routine for all text
 *
 * - Flag "higher_pict"
 *   Use the "Term_pict()" routine for special text
 *
 * - Flag "always_text"
 *   Use the "Term_text()" routine for invisible text
 *
 * - Flag "unused_flag"
 *   Reserved for future use
 *
 * - Flag "never_bored"
 *   Never call the "TERM_XTRA_BORED" action
 *
 * - Flag "never_frosh"
 *   Never call the "TERM_XTRA_FROSH" action
 *
 *
 * - Value "attr_blank"
 *   Use this "attr" value for "blank" grids
 *
 * - Value "char_blank"
 *   Use this "char" value for "blank" grids
 *
 *
 * - Ignore this pointer
 *
 * - Keypress Queue -- various data
 *
 * - Keypress Queue -- pending keys
 *
 *
 * - Window Width (max 255)
 * - Window Height (max 255)
 *
 * - Minimum modified row
 * - Maximum modified row
 *
 * - Minimum modified column (per row)
 * - Maximum modified column (per row)
 *
 *
 * - Displayed screen image
 * - Requested screen image
 *
 * - Temporary screen image
 *
 *
 *
 * - Hook for init-ing the term
 * - Hook for nuke-ing the term
 *
 * - Hook for user actions
 *
 * - Hook for extra actions
 *
 * - Hook for placing the cursor
 *
 * - Hook for drawing some blank spaces
 *
 * - Hook for drawing a string of chars using an attr
 *
 * - Hook for drawing a sequence of special attr/char pairs
 */

typedef struct term term;

struct term
{
	vptr user;

	vptr data;

	bool user_flag;

	bool data_flag;

	bool active_flag;
	bool mapped_flag;
	bool total_erase;
	bool fixed_shape;
	bool icky_corner;
	bool soft_cursor;
	bool always_pict;
	bool higher_pict;
	bool always_text;
	bool unused_flag;
	bool never_bored;
	bool never_frosh;

	byte attr_blank;
	char char_blank;

	char *key_queue;

	u16b key_head;
	u16b key_tail;
	u16b key_xtra;
	u16b key_size;

	byte wid;
	byte hgt;

	byte y1;
	byte y2;

	byte *x1;
	byte *x2;

	term_win *old;
	term_win *scr;

	term_win *tmp;

	void (*init_hook)(term *t);
	void (*nuke_hook)(term *t);

	errr (*user_hook)(int n);

	errr (*xtra_hook)(int n, int v);

	errr (*curs_hook)(int x, int y);

	errr (*wipe_hook)(int x, int y, int n);

	errr (*text_hook)(int x, int y, int n, byte a, cptr s);

	errr (*pict_hook)(int x, int y, int n,
		const byte *ap, const char *cp, const byte *tap, const char *tcp);

	void (*resize_hook)(void);
};







/**** Available Constants ****/


/*
 * Definitions for the "actions" of "Term_xtra()"
 *
 * These values may be used as the first parameter of "Term_xtra()",
 * with the second parameter depending on the "action" itself.  Many
 * of the actions shown below are optional on at least one platform.
 *
 * The "TERM_XTRA_EVENT" action uses "v" to "wait" for an event
 * The "TERM_XTRA_SHAPE" action uses "v" to "show" the cursor
 * The "TERM_XTRA_FROSH" action uses "v" for the index of the row
 * The "TERM_XTRA_SOUND" action uses "v" for the index of a sound
 * The "TERM_XTRA_ALIVE" action uses "v" to "activate" (or "close")
 * The "TERM_XTRA_LEVEL" action uses "v" to "resume" (or "suspend")
 * The "TERM_XTRA_DELAY" action uses "v" as a "millisecond" value
 *
 * The other actions do not need a "v" code, so "zero" is used.
 */
#define TERM_XTRA_EVENT 1 /* Process some pending events */
#define TERM_XTRA_FLUSH 2 /* Flush all pending events */
#define TERM_XTRA_CLEAR 3 /* Clear the entire window */
#define TERM_XTRA_SHAPE 4 /* Set cursor shape (optional) */
#define TERM_XTRA_FROSH 5 /* Flush one row (optional) */
#define TERM_XTRA_FRESH 6 /* Flush all rows (optional) */
#define TERM_XTRA_NOISE 7 /* Make a noise (optional) */
#define TERM_XTRA_SOUND 8 /* Make a sound (optional) */
#define TERM_XTRA_BORED 9 /* Handle stuff when bored (optional) */
#define TERM_XTRA_REACT 10 /* React to global changes (optional) */
#define TERM_XTRA_ALIVE 11 /* Change the "hard" level (optional) */
#define TERM_XTRA_LEVEL 12 /* Change the "soft" level (optional) */
#define TERM_XTRA_DELAY 13 /* Delay some milliseconds (optional) */

/**** Available Functions ****/

extern void Term_queue_char(int x, int y, byte a, char c, byte ta, char tc);
extern void Term_queue_chars(int x, int y, int n, byte a, cptr s);


extern errr Term_exchange(void);

/*** Color constants ***/


/*
 * Angband "attributes" (with symbols, and base (R,G,B) codes)
 *
 * The "(R,G,B)" codes are given in "fourths" of the "maximal" value,
 * and should "gamma corrected" on most (non-Macintosh) machines.
 */
#define TERM_DARK               0       /* 'd' */       /* 0,0,0 */
#define TERM_WHITE              1       /* 'w' */       /* 4,4,4 */
#define TERM_SLATE              2       /* 's' */       /* 2,2,2 */
#define TERM_ORANGE             3       /* 'o' */       /* 4,2,0 */
#define TERM_RED                4       /* 'r' */       /* 3,0,0 */
#define TERM_GREEN              5       /* 'g' */       /* 0,2,1 */
#define TERM_BLUE               6       /* 'b' */       /* 0,0,4 */
#define TERM_UMBER              7       /* 'u' */       /* 2,1,0 */
#define TERM_L_DARK             8       /* 'D' */       /* 1,1,1 */
#define TERM_L_WHITE    9       /* 'W' */       /* 3,3,3 */
#define TERM_VIOLET             10      /* 'v' */       /* 4,0,4 */
#define TERM_YELLOW             11      /* 'y' */       /* 4,4,0 */
#define TERM_L_RED              12      /* 'R' */       /* 4,0,0 */
#define TERM_L_GREEN    13      /* 'G' */       /* 0,4,0 */
#define TERM_L_BLUE             14      /* 'B' */       /* 0,4,4 */
#define TERM_L_UMBER    15      /* 'U' */       /* 3,2,1 */

#define SUCCESS 0 /* No error */

/* Errors from Term_keypress(), etc.. */
#define TERM_ERROR_BAD_INPUT -501
#define TERM_ERROR_OUT_OF_MEMORY -502
#define TERM_ERROR_TRIVIAL_REQUEST -503
#define TERM_ERROR_OUT_OF_BOUNDS -504
#define TERM_ERROR_NO_OUTPUT -505
#define TERM_ERROR_FORBIDDEN -506

/* Distinguish errors which prevent action from those which don't. */
#define WARN(X) -(X)

#endif
