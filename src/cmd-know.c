/*
 * File: cmd-know.c
 * Purpose: Knowledge screen stuff.
 *
 * Copyright (c) 2000-2007 Eytan Zweig, Andrew Doull, Pete Mack.
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */
#include "angband.h"
#include "object/tvalsval.h"
#include "ui.h"
#include "ui-menu.h"


/* Flag value for missing array entry */
#define MISSING -17


typedef struct
{
	int maxnum;          /* Maximum possible item count for this class */
	bool easy_know;      /* Items don't need to be IDed to recognize membership */

	const char *(*name)(int gid);               /* Name of this group */

	/* Compare, in group and display order (optional if already sorted) */
	int (*gcomp)(const void *, const void *);   /* Compares gids of two oids */
	int (*group)(int oid);                      /* Returns gid for an oid */

	/* Summary function for the "object" information. */
	void (*summary)(int gid, const int *object_list, int n, int top, s16b row, s16b col);

} group_funcs;

typedef struct
{
	/* Displays an entry at specified location, including kill-count and graphics */
	void (*display_member)(s16b col, s16b row, bool cursor, int oid);

	void (*lore)(int oid);       /* Displays lore for an oid */


	/* Required only for objects with modifiable display attributes */
	/* Unknown 'flavors' return flavor attributes */
	char *(*xchar)(int oid);     /* Get character attr for OID (by address) */
	byte *(*xattr)(int oid);     /* Get color attr for OID (by address) */

	const char *(*xtra_prompt)(int oid);  /* Returns optional extra prompt */
	void (*xtra_act)(char ch, int oid);   /* Handles optional extra actions */

	bool is_visual;                       /* Does this kind have visual editing? */

} member_funcs;


/* Helper class for generating joins */
typedef struct join
{
		int oid;
		int gid;
} join_t;

/* A default group-by */
static join_t *default_join;
#if 0
static int default_join_cmp(const void *a, const void *b)
{
		join_t *ja = &default_join[*(int*)a];
		join_t *jb = &default_join[*(int*)b];
		int c = ja->gid - jb->gid;
		if (c) return c;
		return ja->oid - jb->oid;
}
#endif
static int default_group(int oid) { return default_join[oid].gid; }


static int *obj_group_order;

/*
 * Description of each monster group.
 */
static struct
{
	cptr chars;
	cptr name;
} monster_group[] =
{
	{ (cptr)-1,	"Uniques" },
	{ "A",		"Angels" },
	{ "a",		"Ants" },
	{ "b",		"Bats" },
	{ "B",		"Birds" },
	{ "C",		"Canines" },
	{ "c",		"Centipedes" },
	{ "uU",		"Demons" },
	{ "dD",		"Dragons" },
	{ "vE",		"Elementals/Vortices" },
	{ "e",		"Eyes/Beholders" },
	{ "f",		"Felines" },
	{ "G",		"Ghosts" },
	{ "OP",		"Giants/Ogres" },
	{ "g",		"Golems" },
	{ "H",		"Harpies/Hybrids" },
	{ "h",		"Hominids (Elves, Dwarves)" },
	{ "M",		"Hydras" },
	{ "i",		"Icky Things" },
	{ "lFI",	"Insects" },
	{ "j",		"Jellies" },
	{ "K",		"Killer Beetles" },
	{ "k",		"Kobolds" },
	{ "L",		"Lichs" },
	{ "tp",		"Men" },
	{ ".$?!_",	"Mimics" },
	{ "m",		"Molds" },
	{ ",",		"Mushroom Patches" },
	{ "n",		"Nagas" },
	{ "o",		"Orcs" },
	{ "q",		"Quadrupeds" },
	{ "Q",		"Quylthulgs" },
	{ "R",		"Reptiles/Amphibians" },
	{ "r",		"Rodents" },
	{ "S",		"Scorpions/Spiders" },
	{ "s",		"Skeletons/Drujs" },
	{ "J",		"Snakes" },
	{ "T",		"Trolls" },
	{ "V",		"Vampires" },
	{ "W",		"Wights/Wraiths" },
	{ "w",		"Worms/Worm Masses" },
	{ "X",		"Xorns/Xarens" },
	{ "y",		"Yeeks" },
	{ "Y",		"Yeti" },
	{ "Z",		"Zephyr Hounds" },
	{ "z",		"Zombies" },
	{ NULL,		NULL }
};

/*
 * Description of each feature group.
 */
const char *feature_group_text[] = 
{
	"Floors",
	"Traps",
	"Doors",
	"Stairs",
	"Walls",
	"Streamers",
	"Obstructions",
	"Stores",
	"Other",
	NULL
};



/* Useful method declarations */
static void display_visual_list(s16b col, s16b row, s16b height, s16b width,
				byte attr_top, byte char_left);

static bool visual_mode_command(ui_event_data ke, bool *visual_list_ptr, 
				s16b height, s16b width, 
				byte *attr_top_ptr, byte *char_left_ptr, 
				byte *cur_attr_ptr, byte *cur_char_ptr,
				s16b col, s16b row, int *delay);

static void place_visual_list_cursor(s16b col, s16b row, byte a,
				byte c, byte attr_top, byte char_left);

/*
 * Clipboard variables for copy&paste in visual mode
 */
static byte attr_idx = 0;
static byte char_idx = 0;

/*
 * Return a specific ordering for the features
 */
static int feat_order(int feat)
{
	feature_type *f_ptr = &f_info[feat];

	switch (f_ptr->d_char)
	{
		case '.': 				return 0;
		case '^': 				return 1;
		case '\'': case '+': 	return 2;
		case '<': case '>':		return 3;
		case '#':				return 4;
		case '*': case '%' :	return 5;
		case ';': case ':' :	return 6;

		default:
		{
			return 8;
		}
	}
}


/* Emit a 'graphical' symbol and a padding character if appropriate */
static void big_pad(s16b col, s16b row, byte a, byte c)
{
	Term_putch(col, row, a, c);
	if (!use_bigtile) return;

	if (a & 0x80)
		Term_putch(col + 1, row, 255, -1);
	else
		Term_putch(col + 1, row, 1, ' ');
}

/* Return the actual width of a symbol */
static s16b actual_width(s16b width)
{
#ifdef UNANGBAND
	if (use_trptile) width *= 3;
	else if (use_dbltile) width *= 2;
#endif

	if (use_bigtile) width *= 2;

	return width;
}

/* Return the actual height of a symbol */
static s16b actual_height(s16b height)
{
#ifdef UNANGBAND
	if (use_trptile) height = height * 3 / 2;
	else if (use_dbltile) height *= 2;
#endif

	if (use_bigtile) height *= 2;

	return height;
}


/* From an actual width, return the logical width */
static s16b logical_width(s16b width)
{
	s16b divider = 1;

#ifdef UNANGBAND
	if (use_trptile) divider = 3;
	else if (use_dbltile) divider = 2;
#endif

	if (use_bigtile) divider *= 2;

	return width / divider;
}

/* From an actual height, return the logical height */
static s16b logical_height(s16b height)
{
	s16b divider = 1;

#ifdef UNANGBAND
	if (use_trptile)
	{
		height *= 2;
		divider = 3;
	}
	else if (use_dbltile) divider = 2;
#endif

	if (use_bigtile) divider *= 2;

	return height / divider;
}


static void display_group_member(menu_type *menu, int oid,
						bool cursor, s16b row, s16b col, s16b wid)
{
	const member_funcs *o_funcs = menu->menu_data;
	byte attr = curs_attrs[CURS_KNOWN][cursor == oid];

	(void)wid;

	/* Print the interesting part */
	o_funcs->display_member(col, row, cursor, oid);

#if 0 /* Debugging code */
	c_put_str(attr, format("%d", oid), row, 60);
#endif

	/* Do visual mode */
	if (o_funcs->is_visual && o_funcs->xattr)
	{
		byte c = *o_funcs->xchar(oid);
		byte a = *o_funcs->xattr(oid);

		c_put_str(attr, format((c & 0x80) ? "%02x/%02x" : "%02x/%d", a, c), row, 60);
	}
}

static const char *recall_prompt(int oid)
{
	(void)oid;
	return ", 'r' to recall";
}

#define swap(a, b) (swapspace = (void*)(a)), ((a) = (b)), ((b) = swapspace)

/*
 * Interactive group by. 
 * Recognises inscriptions, graphical symbols, lore
 */
static void display_knowledge(const char *title, int *obj_list, int o_count,
				group_funcs g_funcs, member_funcs o_funcs,
				const char *otherfields)
{
	/* maximum number of groups to display */
	int max_group = g_funcs.maxnum < o_count ? g_funcs.maxnum : o_count ;

	/* This could (should?) be (void **) */
	int *g_list;
	s16b *g_offset;

	char **g_names;

	s16b g_name_len = 8;  /* group name length, minumum is 8 */

	s16b grp_cnt = 0; /* total number groups */

	s16b g_cur = 0, grp_old = -1; /* group list positions */
	s16b o_cur = 0;					/* object list positions */
	s16b g_o_count = 0;				 /* object count for group */
	int oid = -1;  				/* object identifiers */

	region title_area = { 0, 0, 0, 4 };
	region group_region = { 0, 6, MISSING, -2 };
	region object_region = { MISSING, 6, 0, -2 };

	/* display state variables */
	bool visual_list = FALSE;
	byte attr_top = 0;
	byte char_left = 0;

	int delay = 0;

	menu_type group_menu;
	menu_type object_menu;
	menu_iter object_iter;

	/* Panel state */
	/* These are swapped in parallel whenever the actively browsing " */
	/* changes */
	s16b *active_cursor = &g_cur, *inactive_cursor = &o_cur;
	menu_type *active_menu = &group_menu, *inactive_menu = &object_menu;
	int panel = 0;

	void *swapspace;
	bool do_swap = FALSE;

	bool flag = FALSE;
	bool redraw = TRUE;

	s16b browser_rows;
	s16b wid, hgt;
	s16b i;
	int prev_g = -1;

	int omode = OPT(rogue_like_commands);


	/* Get size */
	Term_get_size(&wid, &hgt);
	browser_rows = hgt - 8;

	/* Disable the roguelike commands for the duration */
	OPT(rogue_like_commands) = FALSE;

	/* Do the group by. ang_sort only works on (void **) */
	/* Maybe should make this a precondition? */
	if (g_funcs.gcomp)
		qsort(obj_list, o_count, sizeof(*obj_list), g_funcs.gcomp);


	/* Sort everything into group order */
	g_list = C_ZNEW(max_group + 1, int);
	g_offset = C_ZNEW(max_group + 1, s16b);

	for (i = 0; i < o_count; i++)
	{
		if (prev_g != g_funcs.group(obj_list[i]))
		{
			prev_g = g_funcs.group(obj_list[i]);
			g_offset[grp_cnt] = i;
			g_list[grp_cnt++] = prev_g;
		}
	}

	g_offset[grp_cnt] = (s16b) o_count;
	g_list[grp_cnt] = -1;

	/* The compact set of group names, in display order */
	g_names = C_ZNEW(grp_cnt, char *);

	for (i = 0; i < grp_cnt; i++)
	{
		s16b len;
		g_names[i] = (char *)g_funcs.name(g_list[i]);
		len = (s16b) strlen(g_names[i]);
		if (len > g_name_len) g_name_len = len;
	}

	/* Reasonable max group name len */
	if (g_name_len >= 20) g_name_len = 20;

	object_region.col = g_name_len + 3;
	group_region.width = g_name_len;


	/* Leave room for the group summary information */
	if (g_funcs.summary) object_region.page_rows = -3;


	/* Set up the two menus */
	WIPE(&group_menu, menu_type);
	group_menu.count = grp_cnt;
	group_menu.cmd_keys = "\n\r6\x8C";  /* Ignore these as menu commands */
	group_menu.menu_data = g_names;

	WIPE(&object_menu, menu_type);
	object_menu.menu_data = &o_funcs;
	WIPE(&object_iter, object_iter);
	object_iter.display_row = display_group_member;

	o_funcs.is_visual = FALSE;

	menu_init(&group_menu, MN_SKIN_SCROLL, find_menu_iter(MN_ITER_STRINGS), &group_region);
	menu_init(&object_menu, MN_SKIN_SCROLL, &object_iter, &object_region);


	/* This is the event loop for a multi-region panel */
	/* Panels are -- text panels, two menus, and visual browser */
	/* with "pop-up menu" for lore */
	while ((!flag) && (grp_cnt))
	{
		ui_event_data ke, ke0;

		if (redraw)
		{
			/* Print the title bits */
			region_erase(&title_area);
			prt(format("Knowledge - %s", title), 2, 0);
			prt("Group", 4, 0);
			prt("Name", 4, g_name_len + 3);

			if (otherfields)
				prt(otherfields, 4, 55);


			/* Print dividers: horizontal and vertical */
			for (i = 0; i < 79; i++)
				Term_putch(i, 5, TERM_WHITE, '=');

			for (i = 0; i < browser_rows; i++)
				Term_putch(g_name_len + 1, 6 + i, TERM_WHITE, '|');


			/* Reset redraw flag */
			redraw = FALSE;
		}

		if (g_cur != grp_old)
		{
			grp_old = g_cur;
			o_cur = 0;
			g_o_count = g_offset[g_cur+1] - g_offset[g_cur];
			menu_set_filter(&object_menu, obj_list + g_offset[g_cur], g_o_count);
			group_menu.cursor = g_cur;
			object_menu.cursor = 0;
		}

		/* HACK ... */
		if (!visual_list)
		{
			/* ... The object menu may be browsing the entire group... */
			o_funcs.is_visual = FALSE;
			menu_set_filter(&object_menu, obj_list + g_offset[g_cur], g_o_count);
			object_menu.cursor = o_cur;
		}
		else
		{
			/* ... or just a single element in the group. */
			o_funcs.is_visual = TRUE;
			menu_set_filter(&object_menu, obj_list + o_cur + g_offset[g_cur], 1);
			object_menu.cursor = 0;
		}

		oid = obj_list[g_offset[g_cur]+o_cur];

		/* Print prompt */
		{
			const char *pedit = (!o_funcs.xattr) ? "" :
					(!(attr_idx|char_idx) ? ", 'c' to copy" : ", 'c', 'p' to paste");
			const char *xtra = o_funcs.xtra_prompt ? o_funcs.xtra_prompt(oid) : "";
			const char *pvs = "";

			if (visual_list) pvs = ", ENTER to accept";
			else if (o_funcs.xattr) pvs = ", 'v' for visuals";



			prt(format("<dir>%s%s%s, ESC", pvs, pedit, xtra), hgt - 1, 0);
		}

		if (do_swap)
		{
			do_swap = FALSE;
			swap(active_menu, inactive_menu);
			swap(active_cursor, inactive_cursor);
			panel = 1 - panel;
		}

		if (g_funcs.summary && !visual_list)
		{
			g_funcs.summary(g_cur, obj_list, g_o_count, g_offset[g_cur],
			                object_menu.boundary.row + object_menu.boundary.page_rows,
			                object_region.col);
		}

		menu_refresh(inactive_menu);
		menu_refresh(active_menu);

		handle_stuff();

		if (visual_list)
		{
			display_visual_list(g_name_len + 3, 7, browser_rows-1,
			                             wid - (g_name_len + 3), attr_top, char_left);
			place_visual_list_cursor(g_name_len + 3, 7, *o_funcs.xattr(oid), 
										*o_funcs.xchar(oid), attr_top, char_left);
		}

		if (delay)
		{
			/* Force screen update */
			Term_fresh();

			/* Delay */
			Term_xtra(TERM_XTRA_DELAY, delay);

			delay = 0;
		}

		ke = inkey_ex();

		/* Do visual mode command if needed */
		if (o_funcs.xattr && o_funcs.xchar &&
					visual_mode_command(ke, &visual_list,
					browser_rows-1, wid - (g_name_len + 3),
					&attr_top, &char_left,
					o_funcs.xattr(oid), (byte*)
					o_funcs.xchar(oid),
					g_name_len + 3, 7, &delay))
		{
			continue;
		}

		if (ke.type == EVT_MOUSE)
		{
			/* Change active panels */
			if (region_inside(&inactive_menu->boundary, &ke))
			{
				swap(active_menu, inactive_menu);
				swap(active_cursor, inactive_cursor);
				panel = 1-panel;
			}
		}

		ke0 = run_event_loop(&active_menu->target, &ke);
		if (ke0.type != EVT_AGAIN) ke = ke0;

		switch (ke.type)
		{
			case EVT_KBRD:
			{
				break;
			}

			case ESCAPE:
			{
				flag = TRUE;
				continue;
			}

			case EVT_SELECT:
			{
				if (panel == 1 && oid >= 0 && o_cur == active_menu->cursor)
				{
					o_funcs.lore(oid);
					redraw = TRUE;
				}
			}

			case EVT_MOVE:
			{
				*active_cursor = active_menu->cursor;
				continue;
			}

			case EVT_BACK:
			{
				if (panel == 1)
					do_swap = TRUE;
			}

			/* XXX Handle EVT_RESIZE */

			default:
			{
				continue;
			}
		}

		switch (ke.key)
		{
			case ESCAPE:
			{
				flag = TRUE;
				break;
			}

			case 'R':
			case 'r':
			{
				/* Recall on screen */
				if (oid >= 0)
					o_funcs.lore(oid);

				redraw = TRUE;
				break;
			}

			/* Jump down a page */
			case '3':
			{
				*active_cursor += browser_rows;

				if (g_cur >= grp_cnt) g_cur = grp_cnt - 1;
				else if (o_cur >= g_o_count) o_cur = g_o_count - 1;

				break;
			}

			/* Jump up a page */
			case '9':
			{
				*active_cursor -= browser_rows;

				if (*active_cursor < 0) *active_cursor = 0;

				break;
			}

			default:
			{
				int d = target_dir(ke.key);

				/* Handle key-driven motion between panels */
				if (ddx[d] && ((ddx[d] < 0) == (panel == 1)))
				{
					/* Silly hack -- diagonal arithmetic */
					*inactive_cursor += ddy[d];
					if (*inactive_cursor < 0) *inactive_cursor = 0;
					else if (g_cur >= grp_cnt) g_cur = grp_cnt - 1;
					else if (o_cur >= g_o_count) o_cur = g_o_count - 1;
					do_swap = TRUE;
				}
				else if (o_funcs.xtra_act)
				{
					o_funcs.xtra_act(ke.key, oid);
				}

				break;
			}
		}
	}

	/* Restore roguelike option */
	OPT(rogue_like_commands) = (omode != 0);

	/* Prompt */
	if (!grp_cnt)
		prt(format("No %s known.", title), 15, 0);

	FREE(g_names);
	FREE(g_offset);
	FREE(g_list);
}

/*
 * Display visuals.
 */
static void display_visual_list(s16b col, s16b row, s16b height, s16b width, byte attr_top, byte char_left)
{
	s16b i, j;

	/* Clear the display lines */
	for (i = 0; i < height; i++)
			Term_erase(col, row + i, width);

	width = logical_width(width);

	/* Display lines until done */
	for (i = 0; i < height; i++)
	{
		/* Display columns until done */
		for (j = 0; j < width; j++)
		{
			byte a;
			unsigned char c;
			s16b x = col + actual_width(j);
			s16b y = row + actual_width(i);
			int ia, ic;

			ia = attr_top + i;
			ic = char_left + j;

			a = (byte)ia;
			c = (unsigned char)ic;

			/* Display symbol */
			big_pad(x, y, a, c);
		}
	}
}


/*
 * Place the cursor at the collect position for visual mode
 */
static void place_visual_list_cursor(s16b col, s16b row, byte a, byte c, byte attr_top, byte char_left)
{
	s16b i = a - attr_top;
	s16b j = c - char_left;

	s16b x = col + actual_width(j);
	s16b y = row + actual_height(i);

	/* Place the cursor */
	Term_gotoxy(x, y);
}


/*
 *  Do visual mode command -- Change symbols
 */
static bool visual_mode_command(ui_event_data ke, bool *visual_list_ptr, 
				s16b height, s16b width, 
				byte *attr_top_ptr, byte *char_left_ptr, 
				byte *cur_attr_ptr, byte *cur_char_ptr,
				s16b col, s16b row, int *delay)
{
	static byte attr_old = 0;
	static char char_old = 0;

	/* These are the distance we want to maintain between the
	 * cursor and borders.
	 */
	int frame_left = logical_width(10);
	int frame_right = logical_width(10);
	int frame_top = logical_height(4);
	int frame_bottom = logical_height(4);

	switch (ke.key)
	{
		case ESCAPE:
		{
			if (*visual_list_ptr)
			{
				/* Cancel change */
				*cur_attr_ptr = attr_old;
				*cur_char_ptr = char_old;
				*visual_list_ptr = FALSE;

				return TRUE;
			}

			break;
		}

		case '\n':
		case '\r':
		{
			if (*visual_list_ptr)
			{
				/* Accept change */
				*visual_list_ptr = FALSE;
				return TRUE;
			}

			break;
		}

		case 'V':
		case 'v':
		{
			if (!*visual_list_ptr)
			{
				*visual_list_ptr = TRUE;

				*attr_top_ptr = (byte)MAX(0, (int)*cur_attr_ptr - frame_top);
				*char_left_ptr = (char)MAX(0, (int)*cur_char_ptr - frame_left);

				attr_old = *cur_attr_ptr;
				char_old = *cur_char_ptr;
			}
			else
			{
				/* Cancel change */
				*cur_attr_ptr = attr_old;
				*cur_char_ptr = char_old;
				*visual_list_ptr = FALSE;
			}

			return TRUE;
		}

		case 'C':
		case 'c':
		{
			/* Set the visual */
			attr_idx = *cur_attr_ptr;
			char_idx = *cur_char_ptr;

			return TRUE;
		}

		case 'P':
		case 'p':
		{
			if (attr_idx)
			{
				/* Set the char */
				*cur_attr_ptr = attr_idx;
				*attr_top_ptr = (byte)MAX(0, (int)*cur_attr_ptr - frame_top);
			}

			if (char_idx)
			{
				/* Set the char */
				*cur_char_ptr = char_idx;
				*char_left_ptr = (char)MAX(0, (int)*cur_char_ptr - frame_left);
			}

			return TRUE;
		}

		default:
		{
			if (*visual_list_ptr)
			{
				s16b eff_width = actual_width(width);
				s16b eff_height = actual_height(height);
				int d = target_dir(ke.key);
				byte a = *cur_attr_ptr;
				byte c = *cur_char_ptr;

				/* Get mouse movement */
				if (ke.key == '\xff')
				{
					s16b my = ke.mousey - row;
					s16b mx = ke.mousex - col;

					my = logical_height(my);
					mx = logical_width(mx);

					if ((my >= 0) && (my < eff_height) && (mx >= 0) && (mx < eff_width)
						&& ((ke.index) || (a != *attr_top_ptr + my)
							|| (c != *char_left_ptr + mx)))
					{
						/* Set the visual */
						ISBYTE(my);
						*cur_attr_ptr = a = *attr_top_ptr + (byte) my;
						ISBYTE(mx);
						*cur_char_ptr = c = *char_left_ptr + (byte) mx;

						/* Move the frame */
						if (*char_left_ptr > MAX(0, (int)c - frame_left))
							(*char_left_ptr)--;
						if (*char_left_ptr + eff_width <= MIN(255, (int)c + frame_right))
							(*char_left_ptr)++;
						if (*attr_top_ptr > MAX(0, (int)a - frame_top))
							(*attr_top_ptr)--;
						if (*attr_top_ptr + eff_height <= MIN(255, (int)a + frame_bottom))
							(*attr_top_ptr)++;

						/* Delay */
						*delay = 100;

						/* Accept change */
						if (ke.index) *visual_list_ptr = FALSE;

						return TRUE;
					}

					/* Cancel change */
					else if (ke.index)
					{
						*cur_attr_ptr = attr_old;
						*cur_char_ptr = char_old;
						*visual_list_ptr = FALSE;

						return TRUE;
					}
				}
				else
				{
					/* Restrict direction */
					if ((a == 0) && (ddy[d] < 0)) d = 0;
					if ((c == 0) && (ddx[d] < 0)) d = 0;
					if ((a == 255) && (ddy[d] > 0)) d = 0;
					if ((c == 255) && (ddx[d] > 0)) d = 0;

					ISBYTE(a+ddy[d]);
					a += (byte) ddy[d];
					ISBYTE(c+ddy[d]);
					c += (byte) ddx[d];

					/* Set the visual */
					*cur_attr_ptr = a;
					*cur_char_ptr = c;

					/* Move the frame */
					if ((ddx[d] < 0) && *char_left_ptr > MAX(0, (int)c - frame_left))
						(*char_left_ptr)--;
					if ((ddx[d] > 0) && *char_left_ptr + eff_width <=
														MIN(255, (int)c + frame_right))
					(*char_left_ptr)++;

					if ((ddy[d] < 0) && *attr_top_ptr > MAX(0, (int)a - frame_top))
						(*attr_top_ptr)--;
					if ((ddy[d] > 0) && *attr_top_ptr + eff_height <=
													MIN(255, (int)a + frame_bottom))
						(*attr_top_ptr)++;

					/* We need to always eat the input even if it is clipped,
					 * otherwise it will be interpreted as a change object
					 * selection command with messy results.
					 */
					return TRUE;
				}
			}
		}
	}

	/* Visual mode command is not used */
	return FALSE;
}


/* The following sections implement "subclasses" of the
 * abstract classes represented by member_funcs and group_funcs
 */

/* =================== MONSTERS ==================================== */
/* Many-to-many grouping - use default auxiliary join */

/*
 * Display a monster
 */
static void display_monster(s16b col, s16b row, bool cursor, int oid)
{
	/* HACK Get the race index. (Should be a wrapper function) */
	int r_idx = default_join[oid].oid;

	/* Access the race */
	monster_race *r_ptr = &r_info[r_idx];
	monster_lore *l_ptr = &l_list[r_idx];

	/* Choose colors */
	byte attr = curs_attrs[CURS_KNOWN][(int)cursor];
	byte a = r_ptr->x_attr;
	byte c = r_ptr->x_char;

	/* Display the name */
	c_prt(attr, r_name + r_ptr->name, row, col);

#ifdef UNANGBAND
	if (use_dbltile || use_trptile)
		return;
#endif

	/* Display symbol */
	big_pad(66, row, a, c);

	/* Display kills */
	if (r_ptr->flags[0] & (RF0_UNIQUE))
		put_str(format("%s", (r_ptr->max_num == 0)?  " dead" : "alive"), row, 70);
	else
		put_str(format("%5d", l_ptr->pkills), row, 70);
}


static int m_cmp_race(const void *a, const void *b)
{
	const monster_race *r_a = &r_info[default_join[*(const int *)a].oid];
	const monster_race *r_b = &r_info[default_join[*(const int *)b].oid];
	int gid = default_join[*(const int *)a].gid;

	/* Group by */
	int c = gid - default_join[*(const int *)b].gid;
	if (c) return c;

	/* Order results */
	c = r_a->d_char - r_b->d_char;
	if (c && gid != 0)
	{
		/* UNIQUE group is ordered by level & name only */
		/* Others by order they appear in the group symbols */
		return strchr(monster_group[gid].chars, r_a->d_char)
			- strchr(monster_group[gid].chars, r_b->d_char);
	}
	c = r_a->level - r_b->level;
	if (c) return c;

	return strcmp(r_name + r_a->name, r_name + r_b->name);
}

static char *m_xchar(int oid) { return &r_info[default_join[oid].oid].x_char; }
static byte *m_xattr(int oid) { return &r_info[default_join[oid].oid].x_attr; }
static const char *race_name(int gid) { return monster_group[gid].name; }

static void mon_lore(int oid)
{
	/* Save the screen */
	screen_save();

	/* Describe */
	text_out_hook = text_out_to_screen;

	/* Recall monster */
	roff_top(default_join[oid].oid);
	Term_gotoxy(0, 2);
	describe_monster(default_join[oid].oid, FALSE);

	text_out_c(TERM_L_BLUE, "\n[Press any key to continue]\n");
	(void)anykey();

	/* Load the screen */
	screen_load();
}

static void mon_summary(int gid, const int *object_list, int n, int top, s16b row, s16b col)
{
	int i;
	int kills = 0;

	/* Access the race */
	for (i = 0; i < n; i++)
	{
		int oid = default_join[object_list[i+top]].oid;
		kills += l_list[oid].pkills;
	}

	/* Different display for the first item if we've got uniques to show */
	if (gid == 0 && ((&r_info[default_join[object_list[0]].oid])->flags[0] & (RF0_UNIQUE)))
	{
		c_prt(TERM_L_BLUE, format("%d known uniques, %d slain.", n, kills),
					row, col);
	}
	else
	{
		int tkills = 0;

		for (i = 0; i < z_info->r_max; i++) 
			tkills += l_list[i].pkills;

		c_prt(TERM_L_BLUE, format("Creatures slain: %d/%d (in group/in total)", kills, tkills), row, col);
	}
}

static int count_known_monsters(void)
{
	int m_count = 0;
	int i;
	size_t j;

	for (i = 0; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];
		if (!OPT(cheat_know) && !l_list[i].sights) continue;
		if (!r_ptr->name) continue;

		if (r_ptr->flags[0] & RF0_UNIQUE) m_count++;

		for (j = 1; j < N_ELEMENTS(monster_group) - 1; j++)
		{
			const char *pat = monster_group[j].chars;
			if (strchr(pat, r_ptr->d_char)) m_count++;
		}
	}

	return m_count;
}

/*
 * Display known monsters.
 */
static void do_cmd_knowledge_monsters(void *obj, const char *name)
{
	group_funcs r_funcs = {N_ELEMENTS(monster_group), FALSE, race_name,
							m_cmp_race, default_group, mon_summary};

	member_funcs m_funcs = {display_monster, mon_lore, m_xchar, m_xattr, recall_prompt, 0, 0};

	int *monsters;
	int m_count = 0;
	int i;
	size_t j;

	(void)obj;
	(void)name;

	for (i = 0; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];
		if (!OPT(cheat_know) && !l_list[i].sights) continue;
		if (!r_ptr->name) continue;

		if (r_ptr->flags[0] & RF0_UNIQUE) m_count++;

		for (j = 1; j < N_ELEMENTS(monster_group) - 1; j++)
		{
			const char *pat = monster_group[j].chars;
			if (strchr(pat, r_ptr->d_char)) m_count++;
		}
	}

	default_join = C_ZNEW(m_count, join_t);
	monsters = C_ZNEW(m_count, int);

	m_count = 0;
	for (i = 0; i < z_info->r_max; i++)
	{
		monster_race *r_ptr = &r_info[i];
		if (!OPT(cheat_know) && !l_list[i].sights) continue;
		if (!r_ptr->name) continue;
	
		for (j = 0; j < N_ELEMENTS(monster_group)-1; j++)
		{
			const char *pat = monster_group[j].chars;
			if (j == 0 && !(r_ptr->flags[0] & RF0_UNIQUE)) 
				continue;
			else if (j > 0 && !strchr(pat, r_ptr->d_char))
				continue;

			monsters[m_count] = m_count;
			default_join[m_count].oid = i;
			default_join[m_count++].gid = j;
		}
	}

	display_knowledge("monsters", monsters, m_count, r_funcs, m_funcs,
						"          Sym  Kills");
	FREE(default_join);
	FREE(monsters);
}

/* =================== ARTIFACTS ==================================== */
/* Many-to-one grouping */

static void get_artifact_display_name(char *o_name, size_t namelen, int a_idx)
{
	object_type object_type_body;
	object_type *o_ptr = &object_type_body;

	/* Make fake artifact */
	o_ptr = &object_type_body;
	object_wipe(o_ptr);
	ISBYTE(a_idx);
	make_fake_artifact(o_ptr, (byte) a_idx);

	/* Get its name */
	object_desc(o_name, namelen, o_ptr, TRUE, ODESC_BASE | ODESC_SPOIL);
}

/*
 * Display an artifact label
 */
static void display_artifact(s16b col, s16b row, bool cursor, int oid)
{
	char o_name[80];

	/* Choose a color */
	byte attr = curs_attrs[CURS_KNOWN][(int)cursor];

	get_artifact_display_name(o_name, sizeof o_name, oid);

	/* Display the name */
	c_prt(attr, o_name, row, col);
}

/*
 * Show artifact lore
 */
static void desc_art_fake(int a_idx)
{
	object_type *o_ptr;
	object_type object_type_body;

	/* Get local object */
	o_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(o_ptr);

	/* Make fake artifact */
	ISBYTE(a_idx);
	make_fake_artifact(o_ptr, (byte) a_idx);
	o_ptr->ident |= (IDENT_STORE | IDENT_KNOWN);

	/* Hack -- Handle stuff */
	handle_stuff();

	text_out_hook = text_out_to_screen;
	screen_save();

	Term_gotoxy(0, 0);
	object_info_header(o_ptr);
	if (!object_info(o_ptr, FALSE))
		text_out("\n\nThis item does not seem to possess any special abilities.");

	text_out_c(TERM_L_BLUE, "\n\n[Press any key to continue]\n");
	(void)anykey();
      
	screen_load();
}

static int a_cmp_tval(const void *a, const void *b)
{
	const artifact_type *a_a = &a_info[*(const int *)a];
	const artifact_type *a_b = &a_info[*(const int *)b];

	/*group by */
	int ta = obj_group_order[a_a->tval];
	int tb = obj_group_order[a_b->tval];
	int c = ta - tb;
	if (c) return c;

	/* order by */
	c = a_a->sval - a_b->sval;
	if (c) return c;
	return strcmp(a_name+a_a->name, a_name+a_b->name);
}

static const char *kind_name(int gid) { return object_text_order[gid].name; }
static int art2gid(int oid) { return obj_group_order[a_info[oid].tval]; }

/* Check if the given artifact idx is something we should "Know" about */
static bool artifact_is_known(int a_idx)
{
	int i;

	/* Artifact doesn't exist at all, or not created yet */
	if (!a_info[a_idx].name || a_info[a_idx].cur_num == 0) return FALSE;

	/* Check all objects to see if it exists but hasn't been IDed */
	for (i = 0; i < z_info->o_max; i++)
	{
		int a = o_list[i].name1;

		/* If we haven't actually identified the artifact yet */
		if (a && a == a_idx && !object_known_p(&o_list[i]))
		{
			return FALSE;
		}
	}

        /* Check inventory for the same */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Ignore non-objects */
		if (!o_ptr->k_idx) continue;


		if (o_ptr->name1 && o_ptr->name1 == a_idx && 
		    !object_known_p(o_ptr))
		{
			return FALSE;
		}
	}

	return TRUE;
}
 

/* If 'artifacts' is NULL, it counts the number of known artifacts, otherwise
   it collects the list of known artifacts into 'artifacts' as well. */
static int collect_known_artifacts(int *artifacts, size_t artifacts_len)
{
	int a_count = 0;
	int j;

	if (artifacts)
		assert(artifacts_len >= z_info->a_max);

	for (j = 0; j < z_info->a_max; j++)
	{
		/* Artifact doesn't exist */
		if (!a_info[j].name) continue;

		if (OPT(cheat_xtra) || artifact_is_known(j))
		{
			if (artifacts)
				artifacts[a_count++] = j;
			else
				a_count++;
		}
	}

	return a_count;
}

/*
 * Display known artifacts
 */
static void do_cmd_knowledge_artifacts(void *obj, const char *name)
{
	/* HACK -- should be TV_MAX */
	group_funcs obj_f = {TV_GOLD, FALSE, kind_name, a_cmp_tval, art2gid, 0};
	member_funcs art_f = {display_artifact, desc_art_fake, 0, 0, recall_prompt, 0, 0};

	int *artifacts;
	int a_count = 0;

	(void)obj;
	(void)name;

	artifacts = C_ZNEW(z_info->a_max, int);

	/* Collect valid artifacts */
	a_count = collect_known_artifacts(artifacts, z_info->a_max);

	display_knowledge("artifacts", artifacts, a_count, obj_f, art_f, 0);
	FREE(artifacts);
}

/* =================== EGO ITEMS  ==================================== */
/* Many-to-many grouping (uses default join) */

/* static u16b *e_note(int oid) {return &e_info[default_join[oid].oid].note;} */
static const char *ego_grp_name(int gid) { return object_text_order[gid].name; }

static void display_ego_item(s16b col, s16b row, bool cursor, int oid)
{
	/* HACK: Access the object */
	ego_item_type *e_ptr = &e_info[default_join[oid].oid];

	/* Choose a color */
	byte attr = curs_attrs[0 != (int)e_ptr->everseen][0 != (int)cursor];

	/* Display the name */
	c_prt(attr, e_name + e_ptr->name, row, col);
}

/*
 * Describe fake ego item "lore"
 */
static void desc_ego_fake(int oid)
{
	/* Hack: dereference the join */
	const char *cursed[] = { "permanently cursed", "heavily cursed", "cursed" };
	const char *xtra[] = { "sustain", "higher resistance", "ability" };
	u32b f3, i;

	int e_idx = default_join[oid].oid;
	ego_item_type *e_ptr = &e_info[e_idx];

	object_type dummy;
	WIPE(&dummy, dummy);

	/* Save screen */
	screen_save();

	/* Set text_out hook */
	text_out_hook = text_out_to_screen;

	/* Dump the name */
	c_prt(TERM_L_BLUE, format("%s %s", ego_grp_name(default_group(oid)),
	                                   e_name + e_ptr->name), 0, 0);

	/* Begin recall */
	Term_gotoxy(0, 1);
	text_out("\n");

	if (e_ptr->text)
	{
		s16b x, y;
		text_out(e_text + e_ptr->text);
		Term_locate(&x, &y);
		Term_gotoxy(0, y+1);
	}

	/* List ego flags */
	ISBYTE(e_idx);
	dummy.name2 = (byte) e_idx;
	object_info_spoil(&dummy);

	if (e_ptr->xtra)
		text_out(format("It provides one random %s.", xtra[e_ptr->xtra - 1]));

	for (i = 0, f3 = TR3_PERMA_CURSE; i < 3 ; f3 >>= 1, i++)
	{
		if (e_ptr->flags[3] & f3)
		{
			text_out_c(TERM_RED, format("It is %s.", cursed[i]));
			break;
		}
	}

	text_out_c(TERM_L_BLUE, "\n\n[Press any key to continue]\n");
	(void)anykey();

	screen_load();
}

/* TODO? Currently ego items will order by e_idx */
static int e_cmp_tval(const void *a, const void *b)
{
	const ego_item_type *ea = &e_info[default_join[*(const int *)a].oid];
	const ego_item_type *eb = &e_info[default_join[*(const int *)b].oid];

	/* Group by */
	int c = default_join[*(const int *)a].gid - default_join[*(const int *)b].gid;
	if (c) return c;

	/* Order by */
	return strcmp(e_name + ea->name, e_name + eb->name);
}

/*
 * Display known ego_items
 */
static void do_cmd_knowledge_ego_items(void *obj, const char *name)
{
	group_funcs obj_f =
		{TV_GOLD, FALSE, ego_grp_name, e_cmp_tval, default_group, 0};

	member_funcs ego_f = {display_ego_item, desc_ego_fake, 0, 0, recall_prompt, 0, 0};

	int *egoitems;
	int e_count = 0;
	int i, j;

	(void)obj;
	(void)name;

	/* HACK: currently no more than 3 tvals for one ego type */
	egoitems = C_ZNEW(z_info->e_max * EGO_TVALS_MAX, int);
	default_join = C_ZNEW(z_info->e_max * EGO_TVALS_MAX, join_t);

	for (i = 0; i < z_info->e_max; i++)
	{
		if (e_info[i].everseen || OPT(cheat_xtra))
		{
			for (j = 0; j < EGO_TVALS_MAX && e_info[i].tval[j]; j++)
			{
				int gid = obj_group_order[e_info[i].tval[j]];

				/* Ignore duplicate gids */
				if (j > 0 && gid == default_join[e_count - 1].gid) continue;

				egoitems[e_count] = e_count;
				default_join[e_count].oid = i;
				default_join[e_count++].gid = gid; 
			}
		}
	}

	display_knowledge("ego items", egoitems, e_count, obj_f, ego_f, "");

	FREE(default_join);
	FREE(egoitems);
}

/* =================== ORDINARY OBJECTS  ==================================== */
/* Many-to-one grouping */

/*
 * Looks up an artifact idx given an object_kind *that's already known
 * to be an artifact*.  Behaviour is distinctly unfriendly if passed
 * flavours which don't correspond to an artifact.
 */
static int get_artifact_from_kind(object_kind *k_ptr)
{
	int i;

	assert(k_ptr->flags[3] & TR3_INSTA_ART);

	/* Look for the corresponding artifact */
	for (i = 0; i < z_info->a_max; i++)
	{
		if (k_ptr->tval == a_info[i].tval &&
		    k_ptr->sval == a_info[i].sval)
		{
			break;
		}
	}

        assert(i < z_info->a_max);
	return i;
}

/*
 * Display the objects in a group.
 */
static void display_object(s16b col, s16b row, bool cursor, int oid)
{
	s16b k_idx = INT2S16B(oid);

	object_kind *k_ptr = &k_info[k_idx];
	const char *inscrip = get_autoinscription(INT2S16B(oid));

	char o_name[80];

	/* Choose a color */
	bool aware = (!k_ptr->flavor || k_ptr->aware);
	byte attr = curs_attrs[(int)aware][(int)cursor];

	/* Find graphics bits -- versions of the object_char and object_attr defines */
	/* TODO Check this works */
	bool use_flavour = (k_ptr->flavor) && 
			!(aware && k_ptr->tval == TV_SCROLL) && 
			!(aware && k_ptr->tval == TV_SPELL);

	byte a = use_flavour ? flavor_info[k_ptr->flavor].x_attr : k_ptr->x_attr;
	byte c = use_flavour ? flavor_info[k_ptr->flavor].x_char : k_ptr->x_char;

	/* Display known artifacts differently */
	if ((k_ptr->flags[3] & TR3_INSTA_ART) && artifact_is_known(get_artifact_from_kind(k_ptr)))
	{
		get_artifact_display_name(o_name, sizeof(o_name), get_artifact_from_kind(k_ptr));
	}
	else
	{
 		object_kind_name(o_name, sizeof(o_name), k_idx, OPT(cheat_know));
	}

	/* If the type is "tried", display that */
	if (k_ptr->tried && !aware)
		my_strcat(o_name, " {tried}", sizeof(o_name));

	/* Display the name */
	c_prt(attr, o_name, row, col);

	/* Show autoinscription if around */
	if (aware && inscrip)
		c_put_str(TERM_YELLOW, inscrip, row, 55);

#ifdef UNANGBAND
	/* Hack - don't use if double tile */
	if (use_dbltile || use_trptile)
		return;
#endif

	/* Display symbol */
	big_pad(76, row, a, c);
}

/*
 * Describe fake object
 */
static void desc_obj_fake(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];
	object_type object_type_body;
	object_type *o_ptr = &object_type_body;

	/* Check for known artifacts, display them as artifacts */
	if ((k_ptr->flags[3] & TR3_INSTA_ART) && artifact_is_known(get_artifact_from_kind(k_ptr)))
	{
		desc_art_fake(get_artifact_from_kind(k_ptr));
		return;
	}

	/* Wipe the object */
	object_wipe(o_ptr);

	/* Create the artifact */
	object_prep(o_ptr, INT2S16B(k_idx));

	/* Hack -- its in the store */
	if (k_info[k_idx].aware) o_ptr->ident |= (IDENT_STORE);

	/* It's fully know */
	if (!k_info[k_idx].flavor) object_known(o_ptr);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Describe */
	text_out_hook = text_out_to_screen;
	screen_save();

	Term_gotoxy(0,0);
	object_info_header(o_ptr);
	if (!object_info(o_ptr, FALSE))
		text_out("\n\nThis item does not seem to possess any special abilities.");

	text_out_c(TERM_L_BLUE, "\n\n[Press any key to continue]\n");
	(void)anykey();

	screen_load();
}

static int o_cmp_tval(const void *a, const void *b)
{
	const object_kind *k_a = &k_info[*(const int *)a];
	const object_kind *k_b = &k_info[*(const int *)b];

	/* Group by */
	int ta = obj_group_order[k_a->tval];
	int tb = obj_group_order[k_b->tval];
	int c = ta - tb;
	if (c) return c;

	/* Order by */
	c = k_a->aware - k_b->aware;
	if (c) return -c; /* aware has low sort weight */

	switch (k_a->tval)
	{
		case TV_LITE:
		case TV_BOOK: /* TODO Book conversion */
		case TV_DRAG_ARMOR:
			/* leave sorted by sval */
			break;

		default:
			if (k_a->aware)
				return strcmp(k_name + k_a->name, k_name + k_b->name);

			/* Then in tried order */
			c = k_a->tried - k_b->tried;
			if (c) return -c;

			return strcmp(flavor_text + flavor_info[k_a->flavor].text,
			              flavor_text + flavor_info[k_b->flavor].text);
	}

	return k_a->sval - k_b->sval;
}

static int obj2gid(int oid) { return obj_group_order[k_info[oid].tval]; }

static char *o_xchar(int oid)
{
	object_kind *k_ptr = &k_info[oid];

	if (!k_ptr->flavor || k_ptr->aware)
		return &k_ptr->x_char;
	else
		return &flavor_info[k_ptr->flavor].x_char;
}

static byte *o_xattr(int oid)
{
	object_kind *k_ptr = &k_info[oid];

	if (!k_ptr->flavor || k_ptr->aware)
		return &k_ptr->x_attr;
	else
		return &flavor_info[k_ptr->flavor].x_attr;
}

/*
 * Display special prompt for object inscription.
 */
static const char *o_xtra_prompt(int oid)
{
	object_kind *k_ptr = &k_info[oid];
	s16b idx = (s16b) get_autoinscription_index(INT2S16B(oid)); /* TODO should function return int? */

	const char *no_insc = ", 'r' to recall, '{'";
	const char *with_insc = ", 'r' to recall, '{', '}'";


	/* Forget it if we've never seen the thing */
	if (k_ptr->flavor && !k_ptr->aware)
		return "";

	/* If it's already inscribed */
	if (idx != -1)
		return with_insc;

	return no_insc;
}

/*
 * Special key actions for object inscription.
 */
static void o_xtra_act(char ch, int oid)
{
	object_kind *k_ptr = &k_info[oid];
	s16b idx = (s16b) get_autoinscription_index(INT2S16B(oid));

	/* Forget it if we've never seen the thing */
	if (k_ptr->flavor && !k_ptr->aware)
		return;

	/* Uninscribe */
	if (ch == '}')
	{
		if (idx != -1) remove_autoinscription(INT2S16B(oid));
		return;
	}

	/* Inscribe */
	else if (ch == '{')
	{
		char note_text[80] = "";

		/* Avoid the prompt getting in the way */
		screen_save();

		/* Prompt */
		prt("Inscribe with: ", 0, 0);

		/* Default note */
		if (idx != -1)
			strnfmt(note_text, sizeof(note_text), "%s", get_autoinscription(INT2S16B(oid)));

		/* Get an inscription */
		if (askfor_aux(note_text, sizeof(note_text), NULL))
		{
			/* Remove old inscription if existent */
			if (idx != -1)
				remove_autoinscription(INT2S16B(oid));

			/* Add the autoinscription */
			add_autoinscription(INT2S16B(oid), note_text);

			/* Notice stuff (later) */
			p_ptr->notice |= (PN_AUTOINSCRIBE);
			p_ptr->redraw |= (PR_INVEN | PR_EQUIP);
		}

		/* Reload the screen */
		screen_load();
	}
}



/*
 * Display known objects
 */
void do_cmd_knowledge_objects(void *obj, const char *name)
{
	group_funcs kind_f = {TV_GOLD, FALSE, kind_name, o_cmp_tval, obj2gid, 0};
	member_funcs obj_f = {display_object, desc_obj_fake, o_xchar, o_xattr, o_xtra_prompt, o_xtra_act, 0};

	int *objects;
	int o_count = 0;
	int i;

	(void)obj;
	(void)name;

	objects = C_ZNEW(z_info->k_max, int);

	for (i = 0; i < z_info->k_max; i++)
	{
		if ((k_info[i].everseen || k_info[i].flavor || OPT(cheat_xtra)) &&
				!(k_info[i].flags[3] & TR3_INSTA_ART))
		{
			int c = obj_group_order[k_info[i].tval];
			if (c >= 0) objects[o_count++] = i;
		}
	}

	display_knowledge("known objects", objects, o_count, kind_f, obj_f, "Inscribed          Sym");

	FREE(objects);
}

/* =================== TERRAIN FEATURES ==================================== */
/* Many-to-one grouping */

/*
 * Display the features in a group.
 */
static void display_feature(s16b col, s16b row, bool cursor, int oid )
{
	/* Get the feature index */
	int f_idx = oid;

	/* Access the feature */
	feature_type *f_ptr = &f_info[f_idx];

	/* Choose a color */
	byte attr = curs_attrs[CURS_KNOWN][(int)cursor];

	/* Display the name */
	c_prt(attr, f_name + f_ptr->name, row, col);

#ifdef UNANGBAND
	if (use_dbltile || use_trptile) return;
#endif

	/* Display symbol */
	big_pad(68, row, f_ptr->x_attr, f_ptr->x_char);

	/* ILLUMINATION AND DARKNESS GO HERE */

}


static int f_cmp_fkind(const void *a, const void *b)
{
	const feature_type *fa = &f_info[*(const int *)a];
	const feature_type *fb = &f_info[*(const int *)b];

	/* group by */
	int c = feat_order(*(const int *)a) - feat_order(*(const int *)b);
	if (c) return c;

	/* order by feature name */
	return strcmp(f_name + fa->name, f_name + fb->name);
}

static const char *fkind_name(int gid) { return feature_group_text[gid]; }
static byte *f_xattr(int oid) { return &f_info[oid].x_attr; }
static char *f_xchar(int oid) { return &f_info[oid].x_char; }
static void feat_lore(int oid) { (void)oid; /* noop */ }

/*
 * Interact with feature visuals.
 */
static void do_cmd_knowledge_features(void *obj, const char *name)
{
	group_funcs fkind_f = {N_ELEMENTS(feature_group_text), FALSE,
							fkind_name, f_cmp_fkind, feat_order, 0};

	member_funcs feat_f = {display_feature, feat_lore, f_xchar, f_xattr, 0, 0, 0};

	int *features;
	int f_count = 0;
	int i;

	(void)obj;
	(void)name;

	features = C_ZNEW(z_info->f_max, int);

	for (i = 0; i < z_info->f_max; i++)
	{
		/* Ignore non-features and mimics */
		if (f_info[i].name == 0 || f_info[i].mimic != i)
			continue;

		features[f_count++] = i; /* Currently no filter for features */
	}

	display_knowledge("features", features, f_count, fkind_f, feat_f, "           Sym");
	FREE(features);
}


/* =================== END JOIN DEFINITIONS ================================ */




static void do_cmd_self_knowledge(void *obj, const char *name)
{
	(void)obj;
	(void)name;
	
	/* display self knowledge we already know about. */
	self_knowledge(FALSE);
}

static void do_cmd_knowledge_scores(void *obj, const char *name)
{
	(void)obj;
	(void)name;
	show_scores();
}

static void do_cmd_knowledge_history(void *obj, const char *name)
{
	(void)obj;
	(void)name;
	history_display();
}



/*
 * Definition of the "player knowledge" menu.
 */
static menu_item knowledge_actions[] =
{
{ {0, "Display object knowledge",   do_cmd_knowledge_objects,   0}, 'a', 0 },
{ {0, "Display artifact knowledge", do_cmd_knowledge_artifacts, 0}, 'b', 0 },
{ {0, "Display ego item knowledge", do_cmd_knowledge_ego_items, 0}, 'c', 0 },
{ {0, "Display monster knowledge",  do_cmd_knowledge_monsters,  0}, 'd', 0 },
{ {0, "Display feature knowledge",  do_cmd_knowledge_features,  0}, 'e', 0 },
{ {0, "Display self-knowledge",     do_cmd_self_knowledge,      0}, 'f', 0 },
{ {0, "Display hall of fame",       do_cmd_knowledge_scores,    0}, 'g', 0 },
{ {0, "Display character history",  do_cmd_knowledge_history,   0}, 'h', 0 },
};

static menu_type knowledge_menu;






/* Keep macro counts happy. */
static void cleanup_cmds(void)
{
	FREE(obj_group_order);
}

void init_cmd_know(void)
{	
	/* Initialize the menus */
	menu_type *menu = &knowledge_menu;
	WIPE(menu, menu_type);
	menu->title = "Display current knowledge";
	menu->menu_data = knowledge_actions;
	menu->count = N_ELEMENTS(knowledge_actions),
	menu_init(menu, MN_SKIN_SCROLL, find_menu_iter(MN_ITER_ITEMS), &SCREEN_REGION);
	
	/* initialize other static variables */
	if (!obj_group_order)
	{
		int i;
		int gid = -1;
		
		obj_group_order = C_ZNEW(TV_GOLD + 1, int);
		atexit(cleanup_cmds);
		
		/* Allow for missing values */
		for (i = 0; i <= TV_GOLD; i++)
			obj_group_order[i] = -1;
		
		for (i = 0; 0 != object_text_order[i].tval; i++)
		{
			if (object_text_order[i].name) gid = i;
			obj_group_order[object_text_order[i].tval] = gid;
		}
	}
}




/*
 * Display the "player knowledge" menu.
 */
void do_cmd_knowledge(void)
{
	s16b cursor = 0;
	int i;
	ui_event_data c = EVENT_EMPTY;
	region knowledge_region = { 0, 0, -1, 11 };

	/* Grey out menu items that won't display anything */
	if (collect_known_artifacts(NULL, 0) > 0)
		knowledge_actions[1].flags = 0;
	else
		knowledge_actions[1].flags = MN_GREYED;
	
	knowledge_actions[2].flags = MN_GREYED;
	for (i = 0; i < z_info->e_max; i++)
	{
		if (e_info[i].everseen || OPT(cheat_xtra))
		{
			knowledge_actions[2].flags = 0;
			break;
		}
	}
	
	if (count_known_monsters() > 0)
		knowledge_actions[3].flags = 0;
	else
		knowledge_actions[3].flags = MN_GREYED;
	
	screen_save();
	menu_layout(&knowledge_menu, &knowledge_region);
	
	while (c.key != ESCAPE)
	{
		clear_from(0);
		c = menu_select(&knowledge_menu, &cursor, 0);
	}
	
	screen_load();
}
