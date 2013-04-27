/* File: wizard2.c */

/* Purpose: Wizard commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#include "script.h"
#include "wild.h"

/* Hack */
/* Note: these declarations in maid_grf.c were static and should be again when I'm done. */

extern int map_cx;
extern int map_cy;
extern int display_map_info(int x, int y, char *c, byte *a, char *tc, byte *ta);
extern void resize_big_map(void);
extern void display_banner(wild_done_type *w_ptr);

static void display_law_map(int *cx, int *cy)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, j, x, y;

	byte feat;

	byte ta;
	char tc;

	byte tta;
	char ttc;

	byte tp;

	bool road;

	u16b w_type, w_info, twn;

	byte **ma;
	char **mc;

	byte **mp;

	byte **mta;
	char **mtc;

	int hgt, wid, yrat, xrat, xfactor, yfactor;

	place_type *pl_ptr;


	/* Hack - disable bigtile mode */
	if (use_bigtile)
	{
		Term_bigregion(-1, -1, -1);
	}

	/* Get size */
	Term_get_size(&wid, &hgt);
	hgt -= 2;
	wid -= 2;

	/* Paranoia */
	if ((hgt < 3) || (wid < 3))
	{
		/*
		 * Need to place the player...
		 * This is wrong, but the map is too small anyway.
		 */
		(*cy) = ROW_MAP;
		(*cx) = COL_MAP;
		return;
	}

	/* Allocate the maps */
	C_MAKE(ma, (hgt + 2), byte *);
	C_MAKE(mc, (hgt + 2), char *);
	C_MAKE(mp, (hgt + 2), byte *);

	C_MAKE(mta, (hgt + 2), byte *);
	C_MAKE(mtc, (hgt + 2), char *);

	/* Allocate and wipe each line map */
	for (i = 0; i < (hgt + 2); i++)
	{
		/* Allocate one row each array */
		C_MAKE(ma[i], (wid + 2), byte);
		C_MAKE(mc[i], (wid + 2), char);
		C_MAKE(mp[i], (wid + 2), byte);

		C_MAKE(mta[i], (wid + 2), byte);
		C_MAKE(mtc[i], (wid + 2), char);
	}

	/* Clear the chars and attributes */
	for (y = 0; y < hgt + 2; ++y)
	{
		for (x = 0; x < wid + 2; ++x)
		{
			/* Nothing here */
			ma[y][x] = TERM_WHITE;
			mc[y][x] = ' ';

			mta[y][x] = TERM_WHITE;
			mtc[y][x] = ' ';

			/* No priority */
			mp[y][x] = 0;
		}
	}

	if (!p_ptr->depth)
	{
		/* Plot wilderness */

		/* work out coords of player in wilderness */
		x = px / 16 + *cx;
		y = py / 16 + *cy;

		/* recenter */
		x = x - wid / 2;
		if (x + wid >= max_wild) x = max_wild - wid - 1;
		if (x < 0) x = 0;

		y = y - hgt / 2;
		if (y + hgt >= max_wild) y = max_wild - hgt - 1;
		if (y < 0) y = 0;

		/* Player location in wilderness */
		(*cy) += py / 16 - y + ROW_MAP;
		(*cx) += px / 16 - x + 1;

		/* Fill in the map */
		for (i = 0; i < wid; ++i)
		{
			for (j = 0; j < hgt; ++j)
			{
				/* Only draw blocks inside map */
				if (((x + i + 1) >= max_wild)
					|| ((y + j + 1) >= max_wild)) continue;

				/* Only draw blocks that have been seen */
				/* Not while debugging.
				if (!(wild[j + y][i + x].done.info & WILD_INFO_SEEN)) continue;

				*/

				w_type = wild[j + y][i + x].done.wild;
				w_info = wild[j + y][i + x].done.info;

				if (w_type < WILD_SEA)
				{
					/* Normal terrain */
					feat = wild_gen_data[w_type].feat;

					/* Allow roads to be drawn */
					road = TRUE;
				}
				else
				{
					feat = FEAT_DEEP_WATER;

					/* No roads please */
					road = FALSE;
				}

				/* Add in effect of other specials */
				if (w_info & (WILD_INFO_WATER))
				{
					feat = FEAT_DEEP_WATER;
				}
				else if (w_info & (WILD_INFO_ACID))
				{
					feat = FEAT_DEEP_ACID;
				}
				else if (w_info & (WILD_INFO_LAVA))
				{
					feat = FEAT_DEEP_LAVA;
				}

				/* This is a nasty hack */

				/* Add in effects of roads */
				if ((w_info & (WILD_INFO_ROAD)) && road)
				{
					ma[j + 1][i + 1] = TERM_UMBER;
					mc[j + 1][i + 1] = '+';
					feat = FEAT_NONE;
				}
				else if ((w_info & (WILD_INFO_TRACK)) && road)
				{
					ma[j + 1][i + 1] = TERM_L_UMBER;
					mc[j + 1][i + 1] = '+';
					feat = FEAT_NONE;
				}

				/* Hack - draw places */
				/* Eventually will get attr,char from place data structure. */

				twn = wild[j + y][i + x].done.place;

				/* If there is a place... */
				if (twn)
				{
					pl_ptr = &place[twn];

					switch (place[twn].type)
					{
						case PL_QUEST_PIT:
						{
							/* Hack make a char / attr from depth */
							wild_type *w_ptr = &wild[pl_ptr->y][pl_ptr->x];

							int depth = (w_ptr->done.mon_gen + 9) / 10;

							if (depth > 9) depth = 9;

							/* Quests are white on this map */
							ma[j + 1][i + 1] = TERM_WHITE;
							mc[j + 1][i + 1] = '0' + depth;
							/* feat = FEAT_NONE; */

							break;
						}

						case PL_DUNGEON:
						{
							/* Hack make a char / attr from depth */
							int depth = (pl_ptr->dungeon->min_level + 9) / 10;

							if (depth > 9) depth = 9;

							/* Dungeons are blue */
							ma[j + 1][i + 1] = TERM_L_BLUE;
							mc[j + 1][i + 1] = '0' + depth;
							feat = FEAT_NONE;

							break;
						}

						default:
						{
							/* Towns are white */
							ma[j + 1][i + 1] = TERM_WHITE;
							mc[j + 1][i + 1] = pl_ptr->name[0];
							/* feat = FEAT_NONE; */

							break;
						}
					}
				}

				/* Finally show position of player */
				if ((i + x == px / 16) && (j + y == py / 16))
				{
					ma[j + 1][i + 1] = TERM_WHITE;
					mc[j + 1][i + 1] = '@';
					feat = FEAT_NONE;
				}

				if (feat)
				{
					byte law = wild[y+j][x+i].done.mon_gen;
					/* Get attr / char pair for wilderness block type */

					if (law <= 9) {
						ma[j+1][i+1] = TERM_L_RED;
						mc[j+1][i+1] = '0'+(law);
					}
					else if (law <= 19) {
						ma[j+1][i+1] = TERM_ORANGE;
						mc[j+1][i+1] = '0'+(law-10);
					}
					else if (law <= 29) {
						ma[j+1][i+1] = TERM_YELLOW;
						mc[j+1][i+1] = '0'+(law-20);
					}
					else if (law <= 39) {
						ma[j+1][i+1] = TERM_L_GREEN;
						mc[j+1][i+1] = '0'+(law-30);
					}
					else if (law <= 49) {
						ma[j+1][i+1] = TERM_GREEN;
						mc[j+1][i+1] = '0'+(law-40);
					}
					else if (law <= 59) {
						ma[j+1][i+1] = TERM_BLUE;
						mc[j+1][i+1] = '0'+(law-50);
					} else {
						ma[j+1][i+1] = TERM_VIOLET;
						mc[i+1][j+1] = '0'+(law-60);
					}

					mta[j + 1][i + 1] = ma[j + 1][i + 1];
					mtc[j + 1][i + 1] = mc[j + 1][i + 1];
				}
			}
		}
	}
	else
	{
		yrat = p_ptr->max_hgt - p_ptr->min_hgt;
		xrat = p_ptr->max_wid - p_ptr->min_wid;

		/* Get scaling factors */
		yfactor = ((yrat / hgt < 4) && (yrat > hgt)) ? 10 : 1;
		xfactor = ((xrat / wid < 4) && (xrat > wid)) ? 10 : 1;

		yrat = (yrat * yfactor + hgt - 1) / hgt;
		xrat = (xrat * xfactor + wid - 1) / wid;

		/* Player location in dungeon */
		(*cy) = py * yfactor / yrat + ROW_MAP;
		(*cx) = px * xfactor / xrat + 1;

		/* Fill in the map of dungeon */
		for (i = p_ptr->min_wid; i < p_ptr->max_wid; ++i)
		{
			for (j = p_ptr->min_hgt; j < p_ptr->max_hgt; ++j)
			{
				/* Location */
				x = i * xfactor / xrat + 1;
				y = j * yfactor / yrat + 1;

				/* Get priority and symbol */
				tp = display_map_info(i, j, &tc, &ta, &ttc, &tta);

				/* Save "best" */
				if (mp[y][x] < tp)
				{
					/* Save the char */
					mc[y][x] = tc;

					/* Save the attr */
					ma[y][x] = ta;

					/* Save the transparency graphic */
					mtc[y][x] = ttc;
					mta[y][x] = tta;

					/* Save priority */
					mp[y][x] = tp;
				}
			}
		}
	}

	/* Corners */
	i = wid + 1;
	j = hgt + 1;

	/* Draw the corners */
	mc[0][0] = '+';
	mc[0][i] = '+';
	mc[j][0] = '+';
	mc[j][i] = '+';

	/* Draw the horizontal edges */
	for (i = 1; i <= wid; i++)
	{
		mc[0][i] = '-';
		mc[j][i] = '-';
	}

	/* Draw the vertical edges */
	for (j = 1; j <= hgt; j++)
	{
		mc[j][0] = '|';
		mc[j][i] = '|';
	}

	/* Display each map line in order */
	for (j = 0; j < hgt + 2; ++j)
	{
		/* Display the line */
		for (i = 0; i < wid + 2; ++i)
		{
			ta = ma[j][i];
			tc = mc[j][i];

			tta = mta[j][i];
			ttc = mtc[j][i];

			/* Hack -- Queue it */
			Term_queue_char(i, j, ta, tc, tta, ttc);
		}
	}

	/* Free each line map */
	for (i = 0; i < (hgt + 2); i++)
	{
		/* Free one row each array */
		FREE(ma[i]);
		FREE(mc[i]);
		FREE(mta[i]);
		FREE(mtc[i]);
		FREE(mp[i]);
	}

	/* Free the maps */
	FREE(ma);
	FREE(mc);
	FREE(mta);
	FREE(mtc);
	FREE(mp);
}


static void do_cmd_view_law_map(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int cy, cx;

	void (*hook) (void);

	/* No law map in vanilla town mode or in the dungeon. */
	if (p_ptr->depth || vanilla_town) return;

	/* Remember what the resize hook was */
	hook = angband_term[0]->resize_hook;

	/* Hack - change the redraw hook so bigscreen works */
	angband_term[0]->resize_hook = resize_big_map;

	/* Note */
	prtf(0, 0, "Please wait...");

	/* Flush */
	Term_fresh();

	/* Clear the screen */
	Term_clear();

	{
		/* Offset from player */
		int x, y;

		/* Direction */
		int d;

		/* Input character */
		char c;

		wild_done_type *w_ptr;

		/* No offset yet */
		x = 0;
		y = 0;

		/* In the wilderness - Display the map + move it around */

		while (TRUE)
		{
			/* Reset offset of map */
			cx = x;
			cy = y;

			/* Match offset for the resize */
			map_cx = cx;
			map_cy = cy;

			display_law_map(&cx, &cy);

			/* Get wilderness square */
			w_ptr = &wild[y + py / WILD_BLOCK_SIZE][x + px / WILD_BLOCK_SIZE].done;

			/* Get the banners on the screen */
			display_banner(w_ptr);

			/* Show the cursor */
			Term_gotoxy(cx, cy);

			/* Draw it */
			Term_fresh();

			/* Get a response */
			c = inkey();

			/* Allow a redraw */
			if (c == KTRL('R'))
			{
				/* Do the redraw */
				do_cmd_redraw();

				continue;
			}

			/* Done if not a direction */
			d = get_keymap_dir(c);

			if (!d) break;

			x += ddx[d];
			y += ddy[d];

			/* Bounds checking */
			if (x + px / WILD_BLOCK_SIZE < 0)
			{
				x = -px / WILD_BLOCK_SIZE;
			}
			if (y + py / WILD_BLOCK_SIZE < 0)
			{
				y = -py / WILD_BLOCK_SIZE;
			}
			if (x + px / WILD_BLOCK_SIZE > max_wild - 2)
			{
				x = max_wild - px / WILD_BLOCK_SIZE - 2;
			}
			if (y + py / WILD_BLOCK_SIZE > max_wild - 2)
			{
				y = max_wild - py / WILD_BLOCK_SIZE - 2;
			}
		}
	}

	/* Hack - change the redraw hook so bigscreen works */
	angband_term[0]->resize_hook = hook;

	/* The size may have changed during the scores display */
	angband_term[0]->resize_hook();

	/* Hack - Flush it */
	Term_fresh();
}


/*
 * Hack -- Rerate Hitpoints
 */
void do_cmd_rerate(void)
{
	int min_value, max_value, i, j, percent;

	min_value = (PY_MAX_LEVEL * 3 * (p_ptr->rp.hitdie - 1)) / 8;
	min_value += PY_MAX_LEVEL;

	max_value = (PY_MAX_LEVEL * 5 * (p_ptr->rp.hitdie - 1)) / 8;
	max_value += PY_MAX_LEVEL;

	p_ptr->player_hp[0] = p_ptr->rp.hitdie;

	/* Rerate */
	while (1)
	{
		/* Collect values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			/* Add in racial hit dice */
			j = randint1(rp_ptr->r_mhp);
			p_ptr->player_hp[i] = p_ptr->player_hp[i - 1] + j;

			/* If class hit dice is non zero - add it on */
			if (cp_ptr->c_mhp)
			{
				p_ptr->player_hp[i] += randint1(cp_ptr->c_mhp);
			}
		}

		/* Legal values */
		if ((p_ptr->player_hp[PY_MAX_LEVEL - 1] >= min_value) &&
			(p_ptr->player_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}

	percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
					(2 * p_ptr->rp.hitdie +
					 ((PY_MAX_LEVEL - 1) * (p_ptr->rp.hitdie + 1))));


	/* Update and redraw hitpoints */
	p_ptr->update |= (PU_HP);
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Handle stuff */
	handle_stuff();

	/* Message */
	msgf("Current Life Rating is %d/100.", percent);
}


#ifdef ALLOW_WIZARD

/*
 * Create the artifact of the specified number -- DAN
 *
 */
static void wiz_create_named_art(int a_idx)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	/* Create the artifact */
	create_named_art(a_idx, px, py);

	/* All done */
	msgf("Allocated.");
}


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_mango(int arg)
{
	int i = arg;
	/* Debug */

	for (i = 0; i < place_count; i++) 
	{
		if (place[i].type != PL_TOWN_MINI) continue;
		
		msgf ("Place %i: %s", i, place[i].name);
	}
}


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_ben(int arg)
{
	(void)arg;

	do_cmd_view_law_map();

	/* Make entire wilderness known */
	{
		int i, j;
		for (i = 0; i < WILD_SIZE; i++) {
			for (j=0; j < WILD_SIZE; j++) {
				wild[i][j].done.info |= WILD_INFO_SEEN;
			}
		}
	}

	/* Make all quests known */
	{
		int i;
		for (i = 0; i < z_info->q_max; i++)
		{
			if (quest[i].type) {
				quest[i].flags |= QUEST_FLAG_KNOWN;
				quest[i].status = QUEST_STATUS_TAKEN;
			}
		}
	}

}



#ifdef MONSTER_HORDES

/* Summon a horde of monsters */
static void do_cmd_summon_horde(void)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int wy = py, wx = px;
	int attempts = 1000;
	cave_type *c_ptr;


	while (--attempts)
	{
		scatter(&wx, &wy, px, py, 3);

		/* paranoia */
		if (!in_bounds2(wx, wy)) continue;

		c_ptr = area(wx, wy);
		if (cave_naked_grid(c_ptr)) break;

		/* Not under the player */
		if ((wy == py) && (wx == px)) break;
	}

	(void)alloc_horde(wx, wy);
}

#endif /* MONSTER_HORDES */


#ifdef USE_64B
typedef u64b ufix40_24;	/* Fixed point: 40 bits integer 24 bits fractional */

static ufix40_24 pow4(ufix40_24 n)
{
	ufix40_24 pow2 = (n * n) >> 24;
	return (pow2 * pow2) >> 24;
}


static void get_obj_dist(int min_level, int obj_num, u32b rarity[MAX_DEPTH])
{
	int i;
	long value1, total;
	alloc_entry *table = alloc_kind_table;
	ufix40_24 p;

	int level;

	for (i = 0; i < MAX_DEPTH; i++)
		rarity[i] = 0;

	for (level = 0; level < MAX_DEPTH * 2; level++)
	{
		/* Reset total */
		total = 0L;

		/* Process probabilities */
		for (i = 0; i < alloc_kind_size; i++)
		{
			/* Objects are sorted by depth */
			if (table[i].level > level) break;

			/* What John West rejects, makes John West the best. */
			if (table[i].level < min_level) continue;

			/* Total */
			total += table[i].prob2;
		}

		/* No legal objects */
		if (total <= 0) continue;

		value1 = 0;
		p = 0;

		/* Find the object */
		for (i = 0; i < alloc_kind_size; i++)
		{
			/* Objects are sorted by depth */
			if (table[i].level > level) break;

			/* What John West rejects, makes John West the best. */
			if (table[i].level < min_level) continue;

			if (table[i].index == obj_num)
			{
				p += pow4((u64b) 0x1000000L * (value1 + table[i].prob2) /
						  total) - pow4((u64b) 0x1000000L * value1 / total);
			}

			/* Increment */
			value1 += table[i].prob2;
		}

		/* Add base probability */
		if (level < MAX_DEPTH)
			rarity[level] += (u32b)(p * (GREAT_OBJ - 1) / GREAT_OBJ);

		/* Add the probability for out-of-depth objects */
		for (i = 1; i <= MAX_DEPTH; i++)
		{
			if (level - MAX_DEPTH / i >= 0 && level - MAX_DEPTH / i < MAX_DEPTH)
			{
				rarity[level - MAX_DEPTH / i] +=
					(u32b)((p / MAX_DEPTH) / GREAT_OBJ);
			}
		}
	}

	/* Scale down the final result */
	for (i = 0; i < MAX_DEPTH; i++)
		rarity[i] /= 0x100;
}

#endif /* USE_64B */

/*
 * Output a rarity graph for a type of object.
 *
 * Use a monte-carlo method to calculate the probabilities.
 */
#ifndef USE_64B
static void prt_alloc(const object_type *o_ptr, int col, int row, u32b monte)
#else /* !USE_64B */
static void prt_alloc(const object_type *o_ptr, int col, int row)
#endif /* USE_64B */
{
	u32b i, j;
	u32b maxd = 1, maxr = 1, maxt = 1;
	u32b rarity[MAX_DEPTH];
	u32b total[MAX_DEPTH];
	u32b display[20];
	cptr c = CLR_WHITE;
	cptr r = "+--common--+";
	u16b kind = o_ptr->k_idx;
	u16b home = k_info[kind].level;

	/* Wipe the tables */
	(void)C_WIPE(rarity, MAX_DEPTH, u32b);
	(void)C_WIPE(total, MAX_DEPTH, u32b);
	(void)C_WIPE(display, 20, u32b);

	message_flush();
	prtf(0, 0, "Calculating probability distribution - please wait.");

	/* Refresh */
	Term_fresh();

#ifndef USE_64B

	/* Scan all entries */
	for (i = 0; i < MAX_DEPTH; i++)
	{
		for (j = 0; j < monte; j++)
		{
			if (get_obj_num(i, 0) == kind) rarity[i]++;
		}
		total[i] = monte;
	}

#else /* !USE_64B */

	/* Calculate */
	get_obj_dist(0, kind, rarity);

	for (i = 0; i < MAX_DEPTH; i++)
		total[i] = 0x10000;

#endif /* USE_64B */

	/* Find maxima */
	for (i = 0; i < MAX_DEPTH; i++)
	{
		if (rarity[i] > maxr) maxr = rarity[i];
		if (total[i] > maxt) maxt = total[i];
	}

	/* Simulate a log graph */
	if (maxt / maxr > 32)
	{
		c = CLR_L_WHITE;
		r = "+-uncommon-+";
	}
	if (maxt / maxr > 1024)
	{
		c = CLR_SLATE;
		r = "+---rare---+";
	}
	if (maxt / maxr > 32768L)
	{
		c = CLR_L_DARK;
		r = "+--unique--+";
	}

	/* Calculate probabilities for each range */
	for (i = 0; i < 20; i++)
	{
		/* Shift the values into view */
		for (j = i * MAX_DEPTH / 20; j < (i + 1) * MAX_DEPTH / 20; j++)
		{
			display[i] += rarity[j] * maxt * 10 / total[j];
		}

		/* Correct proportions */
		display[i] /= maxr;

		/* Track maximum */
		if (display[i] > maxd) maxd = display[i];
	}

	/* Normalize */
	for (i = 0; i < 20; i++)
	{
		display[i] = display[i] * 10 / maxd;
	}

	/* Graph the rarities */
	for (i = 0; i < 20; i++)
	{
		Term_putch(col, row + i + 1, TERM_WHITE, '|');

		/* Note the level */
		if ((i * MAX_DEPTH / 20 <= home) && (home < (i + 1) * MAX_DEPTH / 20))
		{
			prtf(col + 1, row + i + 1, CLR_RED "%.*s", display[i], "**********");
		}
		else
		{
			prtf(col + 1, row + i + 1, "%s%.*s", c, display[i], "**********");
		}
	}

	/* Make it look nice */
	prtf(col, row, r);

	Term_putch(col, row + 2, TERM_WHITE, '6');

	Term_putch(col, row + 8, TERM_WHITE, 'A');
	Term_putch(col, row + 9, TERM_WHITE, 'L');
	Term_putch(col, row + 10, TERM_WHITE, 'L');
	Term_putch(col, row + 11, TERM_WHITE, 'O');
	Term_putch(col, row + 12, TERM_WHITE, 'C');

	prtf(col, row + 21, "+");
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (!p_ptr->target_who) return;

	/* Teleport to the target */
	teleport_player_to(p_ptr->target_col, p_ptr->target_row);
}


/*
 * Aux function for "do_cmd_wiz_change()".	-RAK-
 */
static void do_cmd_wiz_change_aux(void)
{
	int i;
	int tmp_int;
	long tmp_long;
	char tmp_val[160];

	/* Query the stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Default */
		strnfmt(tmp_val, 160, "%d", p_ptr->stat[i].max);

		/* Query */
		if (!get_string(tmp_val, 4, "%s (30-400): ", stat_names[i])) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > stat_cap(i)) tmp_int = stat_cap(i);
		else if (tmp_int < 30) tmp_int = 30;

		/* Save it */
		p_ptr->stat[i].cur = p_ptr->stat[i].max = tmp_int;
	}


	/* Default */
	strnfmt(tmp_val, 160, "%ld", (long)(p_ptr->au));

	/* Query */
	if (!get_string(tmp_val, 9, "Gold: ")) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->au = tmp_long;


	/* Default */
	strnfmt(tmp_val, 160, "%ld", (long)(p_ptr->max_exp));

	/* Query */
	if (!get_string(tmp_val, 10, "Experience: ")) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->max_exp = tmp_long;
	p_ptr->exp = tmp_long;

	/* Update */
	check_experience();
}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(void)
{
	/* Interact */
	do_cmd_wiz_change_aux();

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Create a feature near the player.
 */
static void do_cmd_wiz_feature(int feat)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int y, x, d = 3, attempts = 30;

	cave_type *c_ptr;

	while (1)
	{
		/* Find a location */
		y = rand_spread(py, d);
		x = rand_spread(px, d);

		/* Reject illegal grids */
		if (!in_boundsp(x, y)) continue;

		/* Reject the player */
		if ((y == py) && (x == px)) continue;

		attempts--;

		if (!attempts)
		{
			d++;
			attempts = 8 * d;
		}

		/* Access grid */
		c_ptr = area(x, y);

		/* Try to place a new feature */
		if (c_ptr->feat == feat) continue;

		/* Okay */
		break;
	}

	/* Nuke objects */
	delete_object_list(&c_ptr->o_idx);

	/* Nuke monsters */
	delete_monster_idx(c_ptr->m_idx);

	/* Place the feature */
	cave_set_feat(x, y, feat);

	/* Change knowledge of grid */
	parea(x, y)->feat = feat;
}


/*
 * Learn the whole wilderness map
 */
static void learn_map(void)
{
	int i, j;

	for (i = 0; i < max_wild; i++)
	{
		for (j = 0; j < max_wild; j++)
		{
			wild[j][i].done.info |= WILD_INFO_SEEN;
		}
	}
}


/*
 * Wizard routines for creating objects		-RAK-
 * And for manipulating them!                   -Bernd-
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * The following functions are meant to play with objects:
 * Create, modify, roll for them (for statistic purposes) and more.
 * The original functions were by RAK.
 * The function to show an item's debug information was written
 * by David Reeve Sward <sward+@CMU.EDU>.
 *                             Bernd (wiebelt@mathematik.hu-berlin.de)
 *
 * Here are the low-level functions
 * - wiz_display_item()
 *     display an item's debug-info
 * - wiz_create_itemtype()
 *     specify tval and sval (type and subtype of object)
 * - wiz_tweak_item()
 *     specify pval, +AC, +tohit, +todam
 *     Note that the wizard can leave this function anytime,
 *     thus accepting the default-values for the remaining values.
 *     pval comes first now, since it is most important.
 * - wiz_reroll_item()
 *     apply some magic to the item or turn it into an artifact.
 * - wiz_roll_item()
 *     Get some statistics about the rarity of an item:
 *     We create a lot of fake items and see if they are of the
 *     same type (tval and sval), then we compare pval and +AC.
 *     If the fake-item is better or equal it is counted.
 *     Note that cursed items that are better or equal (absolute values)
 *     are counted, too.
 *     HINT: This is *very* useful for balancing the game!
 * - wiz_quantity_item()
 *     change the quantity of an item, but be sane about it.
 *
 * And now the high-level functions
 * - do_cmd_wiz_play()
 *     play with an existing object
 * - wiz_create_item()
 *     create a new object
 *
 * Note -- You do not have to specify "pval" and other item-properties
 * directly. Just apply magic until you are satisfied with the item.
 *
 * Note -- For some items (such as wands, staffs, some rings, etc), you
 * must apply magic, or you will get "broken" or "uncharged" objects.
 *
 * Note -- Redefining artifacts via "do_cmd_wiz_play()" may destroy
 * the artifact.  Be careful.
 *
 * Hack -- this function will allow you to create multiple artifacts.
 * This "feature" may induce crashes or other nasty effects.
 */

/*
 * Just display an item's properties (debug-info)
 * Originally by David Reeve Sward <sward+@CMU.EDU>
 * Verbose item flags by -Bernd-
 */
static void wiz_display_item(const object_type *o_ptr)
{
	int j = 13;
	byte hack_info = o_ptr->info;

	/* Hack - we will reset the object to exactly like it was */
	object_type *q_ptr = (object_type *)o_ptr;

	/* Hack the visibility by (see object_desc_store) */
	q_ptr->info |= (OB_STOREB);

	/* Clear the screen */
    clear_region(13 - 2, 1, 23);

	/* Describe fully */
	prtf(j, 2, "%v", OBJECT_STORE_FMT(q_ptr, TRUE, 3));

	/* Undo visibility hack */
	q_ptr->info = hack_info;

	prtf(j, 4, "kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
			   o_ptr->k_idx, get_object_level(o_ptr),
			   o_ptr->tval, o_ptr->sval);

	prtf(j, 5, "number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
			   o_ptr->number, o_ptr->weight,
			   o_ptr->ac, o_ptr->dd, o_ptr->ds);

	prtf(j, 6, "pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
			   o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d);

	prtf(j, 7, "a_idx = %-4d  cost = %ld",
			   o_ptr->a_idx, (long)object_value_real(o_ptr));

	prtf(j, 8, "info = %04x  timeout = %-d",
			   o_ptr->info, o_ptr->timeout);

	prtf(j, 9, "desc = %s", item_activation(o_ptr));

	prtf(j, 10, "+------------FLAGS1------------+\n"
	    		"AFFECT........SLAY........BRAND.\n"
	    		"              cvae      xsqpaefc\n"
	    		"siwdcc  ssidsahanvudotgddhuoclio\n"
	    		"tnieoh strnipttmiinmrrnrrraiierl\n"
	    		"rtsxna.plcfgdkcpmldncltggpksdced\n"
                "%v", binary_fmt, o_ptr->flags[0]);

	prtf(j, 17, "+------------FLAGS2------------+\n"
	    		"SUST...IMMUN..RESIST............\n"
	    		"       paefctrpsaefcpfldbc sn   \n"
	    		"siwdcc oclioheatcliooeialoshtncd\n"
	    		"tnieoh iierlrfraierliatrnnnrhehi\n"
	    		"rtsxna.sdcedwlatdcedsrekdfddrxss\n"
                "%v", binary_fmt, o_ptr->flags[1]);

	prtf(j + 32, 10,"+------------FLAGS3------------+\n"
			"SH  NO tehsif itdrmsIGNRadtabchp\n"
			"fe  tm yzdhnelneieihaefccrpgluvr\n"
			"il  ea cktmativlgggocliotnorercm\n"
			"re  lg rnyorhtiesehtierlvxrvssuc\n"
			"ec  ec swpdtresptntsdcedtpttsers\n"
                    "%v", binary_fmt, o_ptr->flags[2]);

	prtf(j + 32, 17,"+------------FLAGS4-------------\n"
			"sww     IMSH p pt reHURT..  CURS\n"
			"add     ldac alao exaefcld  as h\n"
			"vsw     iacomtusuptpclioia  utee\n"
			"1hl     trilurcscsrlierltr  taaa\n"
			"0tk     ekddtnkwhinodcedek  ottl\n"
		    "%v", binary_fmt, o_ptr->flags[3]);

}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	int tval;
	cptr desc;
}
tval_desc;

/*
 * A list of tvals and their textual names
 */
static const tval_desc tvals[] =
{
	{TV_SWORD, "Sword"},
	{TV_POLEARM, "Polearm"},
	{TV_HAFTED, "Hafted Weapon"},
	{TV_BOW, "Bow"},
	{TV_ARROW, "Arrows"},
	{TV_BOLT, "Bolts"},
	{TV_SHOT, "Shots"},
	{TV_SHIELD, "Shield"},
	{TV_CROWN, "Crown"},
	{TV_HELM, "Helm"},
	{TV_GLOVES, "Gloves"},
	{TV_BOOTS, "Boots"},
	{TV_CLOAK, "Cloak"},
	{TV_DRAG_ARMOR, "Dragon Scale Mail"},
	{TV_HARD_ARMOR, "Hard Armor"},
	{TV_SOFT_ARMOR, "Soft Armor"},
	{TV_RING, "Ring"},
	{TV_AMULET, "Amulet"},
	{TV_LITE, "Lite"},
	{TV_POTION, "Potion"},
	{TV_SCROLL, "Scroll"},
	{TV_WAND, "Wand"},
	{TV_STAFF, "Staff"},
	{TV_ROD, "Rod"},
	{TV_LIFE_BOOK, "Life Spellbook"},
	{TV_SORCERY_BOOK, "Sorcery Spellbook"},
	{TV_NATURE_BOOK, "Nature Spellbook"},
	{TV_CHAOS_BOOK, "Chaos Spellbook"},
	{TV_DEATH_BOOK, "Death Spellbook"},
	{TV_CONJ_BOOK, "Conjuration Spellbook"},
	{TV_ARCANE_BOOK, "Arcane Spellbook"},
	{TV_SPIKE, "Spikes"},
	{TV_DIGGING, "Digger"},
	{TV_CHEST, "Chest"},
	{TV_FIGURINE, "Magical Figurine"},
	{TV_STATUE, "Statue"},
	{TV_FOOD, "Food"},
	{TV_FLASK, "Flask"},
	{TV_JUNK, "Junk"},
	{TV_SKELETON, "Skeleton"},
	{0, NULL}
};


/*
 * Strip an "object name" into a buffer
 */
void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *k_ptr = &k_info[k_idx];

	cptr str = (k_name + k_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
		if (*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}

/*
 * Global variables so the sub-menus can access
 * the required information
 */
static int create_item_kidx = 0;
static int create_item_tval = 0;

/*
 * Select the item to use
 */
static bool wiz_create_itemtype_aux2(int num)
{
	int i;

	/* Look up the item to use */
	for (i = 0; i < z_info->k_max; i++)
	{
		object_kind *k_ptr = &k_info[i];

		if (k_ptr->tval == create_item_tval)
		{
			/* Are we there yet? */
			if (!num)
			{
				create_item_kidx = i;
				return (TRUE);
			}

			/* Count down the objects to go */
			num--;
		}
	}

	/* Paranoia */
	return (FALSE);
}


/*
 * Specify the sval for the object to create.
 */
static bool wiz_create_itemtype_aux1(int tval_entry)
{
	int i, num = 0;

	int tval = tvals[tval_entry].tval;

	char buf[1024];
	char prompt[80];

	menu_type *item_menu;

	bool result;

	/* Count number of options */
	for (i = 0; i < z_info->k_max; i++)
	{
		object_kind *k_ptr = &k_info[i];

		if (k_ptr->tval == tval) num++;
	}

	/* Create menu array */
	C_MAKE(item_menu, num + 1, menu_type);

	/* Collect all the objects and their descriptions */
	num = 0;
	for (i = 0; i < z_info->k_max; i++)
	{
		object_kind *k_ptr = &k_info[i];

		if (k_ptr->tval == tval)
		{
			/* Acquire the "name" of object "i" */
			strip_name(buf, i);

			/* Create the menu entry */
			item_menu[num].text = string_make(buf);
			item_menu[num].help = NULL;
			item_menu[num].action = wiz_create_itemtype_aux2;
			item_menu[num].flags = MN_ACTIVE;

			num++;
		}
	}

	/* Save tval so we can access it in aux2 */
	create_item_tval = tval;

	/* Create the prompt */
	strnfmt(prompt, 80, "What Kind of %s? ", tvals[tval_entry].desc);
	result = display_menu(item_menu, -1, FALSE, NULL, prompt);

	/* Free the option strings */
	for (i = 0; i <= num; i++)
	{
		string_free(item_menu[i].text);
	}

	/* Free the array */
	FREE(item_menu);

	return (result);
}


/*
 * Specify tval and sval (type and subtype of object) originally
 *
 * This function returns the k_idx of an object type, or zero if failed
 */
static int wiz_create_itemtype(void)
{
	int i, num;

	menu_type *item_menu;

	/* Count number of options */
	num = 0;
	while(tvals[num].tval) num++;

	/* Create menu array */
	C_MAKE(item_menu, num + 1, menu_type);

	/* Collect all the tvals and their descriptions */
	for (i = 0; i < num; i++)
	{
		item_menu[i].text = tvals[i].desc;
		item_menu[i].help = NULL;
		item_menu[i].action = wiz_create_itemtype_aux1;
		item_menu[i].flags = MN_ACTIVE | MN_CLEAR;
	}

	/* Hack - we know that item_menu[num].text is NULL due to C_MAKE */

	/* Clear item to make */
	create_item_kidx = 0;

	display_menu(item_menu, -1, FALSE, NULL, "Get what type of object? ");

	/* Free the array */
	FREE(item_menu);

	return (create_item_kidx);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(object_type *o_ptr)
{
	char tmp_val[80];


	strnfmt(tmp_val, 80, "%d", o_ptr->pval);
	if (!get_string(tmp_val, 6, "Enter new 'pval' setting: ")) return;
	o_ptr->pval = atoi(tmp_val);
	wiz_display_item(o_ptr);

	strnfmt(tmp_val, 80, "%d", o_ptr->to_a);
	if (!get_string(tmp_val, 6, "Enter new 'to_a' setting: ")) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	strnfmt(tmp_val, 80, "%d", o_ptr->to_h);
	if (!get_string(tmp_val, 6, "Enter new 'to_h' setting: ")) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	strnfmt(tmp_val, 80, "%d", o_ptr->to_d);
	if (!get_string(tmp_val, 6, "Enter new 'to_d' setting: ")) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);

	/* XXX XXX XXX Very dangerous... */
	strnfmt(tmp_val, 80, "%d", (int)o_ptr->a_idx);
	if (!get_string(tmp_val, 6, "Enter new 'a_idx' setting: ")) return;
	o_ptr->a_idx = atoi(tmp_val);
	wiz_display_item(o_ptr);

	/* Apply trigger */
	apply_object_trigger(TRIGGER_ALTER, o_ptr, "");
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
static object_type *wiz_reroll_item(object_type *o_ptr)
{
	char ch;

	/* Hack -- leave normal artifacts alone */
	if (FLAG(o_ptr, TR_INSTA_ART) && o_ptr->a_idx) return (o_ptr);

	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(o_ptr);

		/* Ask wizard what to do. */
		if (!get_com
			("[a]ccept, [w]orthless, [n]ormal, [e]xcellent, [s]pecial? ", &ch))
		{
			/* Preserve wizard-generated artifacts */
			if (FLAG(o_ptr, TR_INSTA_ART) && o_ptr->a_idx)
			{
				a_info[o_ptr->a_idx].cur_num = 0;
				o_ptr->a_idx = 0;
			}

			/* Done */
			return (NULL);
		}

		/* Create/change it! */
		if (ch == 'A' || ch == 'a') break;

		/* Preserve wizard-generated artifacts */
		if (FLAG(o_ptr, TR_INSTA_ART) && o_ptr->a_idx)
		{
			a_info[o_ptr->a_idx].cur_num = 0;
			o_ptr->a_idx = 0;

			/* Remove the artifact flag */
			o_ptr->flags[2] &= ~(TR2_INSTA_ART);
		}

		switch (ch)
		{
			case 'w':  case 'W':
			{
				/* Apply bad magic */
				o_ptr = object_prep(o_ptr->k_idx);
				apply_magic(o_ptr, p_ptr->depth, 0, OC_FORCE_BAD);
				break;
			}
			case 'n':  case 'N':
			{
				/* Apply normal magic */
				o_ptr = object_prep(o_ptr->k_idx);
				apply_magic(o_ptr, p_ptr->depth, 0, OC_NORMAL);
				break;
			}
			case 'e':  case 'E':
			{
				/* Apply great magic */
				o_ptr = object_prep(o_ptr->k_idx);
				apply_magic(o_ptr, p_ptr->depth, 30, OC_FORCE_GOOD);
				break;
			}
			case 's':  case 'S':
			{
				/* Apply special magic */
				o_ptr = object_prep(o_ptr->k_idx);

				/* Make a random artifact */
				(void)create_artifact(o_ptr, p_ptr->depth, FALSE);
				break;
			}
		}
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	p_ptr->window |= (PW_SPELL | PW_PLAYER);

	/* Notice changes */
	notice_item();

	/* Success */
	return (o_ptr);
}


/*
 * Redraw the rarity graph with a different number of rolls
 * per level.  This changes the sqrt(n) poisson error.
 * (Otherwise really rare items don't get very good graphs.)
 */
static void wiz_statistics(object_type *o_ptr)
{
#ifndef USE_64B

	u32b test_roll = 100000;

	char tmp_val[80];

	strnfmt(tmp_val, 80, "%ld", (long)test_roll);
	if (get_string(tmp_val, 11, "Enter number of items to roll: "))
	{
		test_roll = atol(tmp_val);
	}
	test_roll = MAX(0, test_roll);

	/* Display the rarity graph */
	prt_alloc(o_ptr, 0, 2, test_roll);

#else /* !USE_64B */

	/* Display the rarity graph */
	prt_alloc(o_ptr, 0, 2);

#endif /* USE_64B */

}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(object_type *o_ptr)
{
	int tmp_int, tmp_qnt;

	char tmp_val[100];


	/* Never duplicate artifacts */
	if (FLAG(o_ptr, TR_INSTA_ART)) return;

	/* Store old quantity. -LM- */
	tmp_qnt = o_ptr->number;

	/* Default */
	strnfmt(tmp_val, 100, "%d", (int)o_ptr->number);

	/* Query */
	if (get_string(tmp_val, 3, "Quantity: "))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;

		/* Accept modifications */
		o_ptr->number = tmp_int;

		/* Hack -- rod pvals must change if the number in the stack does. -LM- */
		if (o_ptr->tval == TV_ROD)
			o_ptr->pval = o_ptr->pval * o_ptr->number / tmp_qnt;

		/* Notice weight changes */
		p_ptr->update |= PU_WEIGHT;
	}
}



/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
static void do_cmd_wiz_play(void)
{
	object_type *q_ptr;

	object_type *o_ptr;

	char ch;

	cptr q, s;

	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";

	q_ptr = get_item(q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR));

	/* Not a valid item */
	if (!q_ptr) return;

	/* Save the screen */
	screen_save();

	/* Duplicate object */
	o_ptr = object_dup(q_ptr);

	/* Display the item */
	wiz_display_item(o_ptr);

	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(o_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [r]eroll [t]weak [q]uantity [s]tatistics? ", &ch))
		{
			/* Ignore changes */
			msgf("Changes ignored.");

			/* Done */
			break;
		}

		/* Accept changes */
		if (ch == 'A' || ch == 'a')
		{
			/* Message */
			msgf("Changes accepted.");

			/* Swap the objects */
			swap_objects(q_ptr, o_ptr);

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_SPELL | PW_PLAYER);

			/* Notice changes */
			notice_item();

			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(o_ptr);
		}

		if (ch == 'r' || ch == 'r')
		{
			o_ptr = wiz_reroll_item(o_ptr);

			/* Failure - get old item */
			if (!o_ptr)
			{
				/* Restore old item */
				o_ptr = object_dup(q_ptr);
			}
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(o_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			wiz_quantity_item(o_ptr);
		}

		if (ch == 'l' || ch == 'L')
		{
			int i;
			for (i = 0; i < MAX_TRIGGER; i++)
			{
				if (o_ptr->trigger[i])
					msgf("%i - '%s'. ", i, quark_str(
							o_ptr->trigger[i]));
			}
		}
	}

	/* Restore the screen */
	screen_load();
}


/*
 * Wizard routine for creating objects		-RAK-
 * Heavily modified to allow magification and artifactification  -Bernd-
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items.
 */
static void wiz_create_item(void)
{
	int k_idx;

	/* Save the screen */
	screen_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	/* Restore the screen */
	screen_load();

	if (!k_idx) return;

	/* Place the object */
	place_specific_object(p_ptr->px, p_ptr->py, p_ptr->depth, k_idx);

	/* All done */
	msgf("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(void)
{
	/* Remove curses */
	(void)remove_all_curse();

	/* Restore stats */
	(void)res_stat(A_STR);
	(void)res_stat(A_INT);
	(void)res_stat(A_WIS);
	(void)res_stat(A_CON);
	(void)res_stat(A_DEX);
	(void)res_stat(A_CHR);

	/* Restore the level */
	(void)restore_level();

	/* Heal the player */
	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;

	/* Restore mana */
	p_ptr->csp = p_ptr->msp;
	p_ptr->csp_frac = 0;

	/* Cure stuff */
	(void)clear_blind();
	(void)clear_confused();
	(void)clear_poisoned();
	(void)clear_afraid();
	(void)clear_paralyzed();
	(void)clear_image();
	(void)clear_stun();
	(void)clear_cut();
	(void)clear_slow();

	/* No longer hungry */
	(void)set_food(PY_FOOD_MAX - 1);

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Go to any level
 */
static void do_cmd_wiz_jump(void)
{
	int min_depth, max_depth;
	dun_type *d_ptr = dungeon();

	/* In the wilderness and no dungeon? */
	if (!check_down_wild()) return;

	max_depth = (dungeon_abyss ? 127 : d_ptr->max_level);
	min_depth = d_ptr->min_level;

	/* Ask for level */
	if (p_ptr->cmd.arg <= 0)
	{
		char tmp_val[160];

		/* Default */
		strnfmt(tmp_val, 160, "%d", p_ptr->depth);

		/* Does this dungeon start right at the surface */
		if (min_depth == 1)
		{
			/* Ask for a level */
			if (!get_string(tmp_val, 11, "Jump to level (0-%d): ",
							max_depth)) return;
		}
		/* Ignore the depths between the surface and the start */
		else
		{
			/* Ask for a level */
			if (!get_string(tmp_val, 11, "Jump to level (0, %d-%d): ",
							min_depth, max_depth)) return;
		}

		/* Extract request */
		p_ptr->cmd.arg = atoi(tmp_val);
	}

	/* Paranoia */
	if (p_ptr->cmd.arg < 0) p_ptr->cmd.arg = 0;

	/* Paranoia */
	if (p_ptr->cmd.arg > 0 && p_ptr->cmd.arg < min_depth)
		p_ptr->cmd.arg = min_depth;

	/* Paranoia */
	if (p_ptr->cmd.arg > max_depth) p_ptr->cmd.arg = max_depth;

	/* Accept request */
	msgf("You jump to dungeon level %d.", p_ptr->cmd.arg);

	/* Change level */
	p_ptr->depth = p_ptr->cmd.arg;

	/* Change the recall_depth of the dungeon */
	d_ptr->recall_depth = MAX(d_ptr->recall_depth, p_ptr->depth);

	/* Leaving */
	p_ptr->state.leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type *q_ptr;

	/* Scan every object */
	for (i = 1; i < z_info->k_max; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= p_ptr->cmd.arg)
		{
			/* Prepare object */
			q_ptr = object_prep(i);

			/* Awareness */
			object_aware(q_ptr);
		}
	}
}


/*
 * Summon some creatures
 */
static void do_cmd_wiz_summon(int num)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i;

	for (i = 0; i < num; i++)
	{
		(void)summon_specific(0, px, py, p_ptr->depth, 0, TRUE, FALSE, FALSE);
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named(int r_idx, bool slp)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, x, y;

	cave_type *c_ptr;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= z_info->r_max) return;

	/* Try 10 times */
	for (i = 0; i < 10; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&x, &y, px, py, d);

		/* paranoia */
		if (!in_bounds2(x, y)) continue;

		/* Require empty grids */
		c_ptr = area(x, y);
		if (!cave_empty_grid(c_ptr)) continue;

		/* Not on player */
		if ((x == px) && (y == py)) continue;

		/* Place it (allow groups) */
		if (place_monster_aux(x, y, r_idx, slp, TRUE, FALSE, FALSE, TRUE)) break;
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(int r_idx, bool slp)
{
	(void)summon_named_creature(p_ptr->px, p_ptr->py, r_idx, slp, TRUE, TRUE);
}


/*
 * Hack -- Delete all nearby monsters
 */
static void do_cmd_wiz_zap(void)
{
	int i;

	/* Genocide everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Delete nearby monsters */
		if (m_ptr->cdis <= MAX_SIGHT) delete_monster_idx(i);
	}

	/* Update some things */
	p_ptr->update |= (PU_MON_LITE);
}


/*
 * Hack -- Delete all monsters
 */
static void do_cmd_wiz_zap_all(void)
{
	int i;

	/* Genocide everyone */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Delete this monster */
		delete_monster_idx(i);
	}

	/* Update some things */
	p_ptr->update |= (PU_MON_LITE);
}

void do_cmd_wiz_town_teleport(void)
{
	place_type *pl_ptr2;
	int i, j;

	/* Make sure the player is outside. */
	if (p_ptr->depth)
	{
		msgf("You cannot do this from a dungeon.");
		return;
	}


	/* Make sure something bad doesn't happen */
	if (p_ptr->cmd.arg >= place_count)
	{
		msgf("Place %i out of bounds, defaulting to %i.", p_ptr->cmd.arg, place_count-1);
		p_ptr->cmd.arg = place_count - 1;
	}
	if (p_ptr->cmd.arg <= 1) p_ptr->cmd.arg = 1;

	/* Get the right place */
	pl_ptr2 = &place[p_ptr->cmd.arg - 1];

	/* Move the player */
	if (pl_ptr2->type == PL_TOWN_OLD || pl_ptr2->type == PL_TOWN_FRACT)
	{
		store_type *st_ptr;

		/* Hack: Must make sure we "know" the area first. */
		p_ptr->px = pl_ptr2->x * 16;
		p_ptr->py = pl_ptr2->y * 16;
		p_ptr->wilderness_x = p_ptr->px;
		p_ptr->wilderness_y = p_ptr->py;
		move_wild();


		for (j = 0; j < 100; j++)
		{
			i = randint0(pl_ptr2->numstores);
			st_ptr = &pl_ptr2->store[i];
			if (st_ptr->owner_name != 0 && st_ptr->type != BUILD_BLANK && st_ptr->type != BUILD_NONE && (st_ptr->x != 0 || st_ptr->y != 0))
				break;
		}

		p_ptr->px = pl_ptr2->x * 16 + st_ptr->x;
		p_ptr->py = pl_ptr2->y * 16 + st_ptr->y;
	}
	/* For quests and dungeons, this will take you to an open spot already */
	else
	{
		p_ptr->px = pl_ptr2->x * 16;
		p_ptr->py = pl_ptr2->y * 16;
	}

	/* Say something */
	msgf("You vanish and reappear somewhere else.");

	p_ptr->wilderness_x = p_ptr->px;
	p_ptr->wilderness_y = p_ptr->py;

	/* Notice player location */
	Term_move_player();

	/* Remove all monster lights */
	lite_n = 0;

	/* Notice the move */
	move_wild();

	/* Check for new panel (redraw map) */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_DISTANCE);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}



static void do_cmd_wiz_towns(void)
{
	/* Count towns */
	int towns = 0;
	int stairs = 0;
	int stores = 0;
	int i, j;

	for (i = 0; i < MAX_CITY_BUILD; i++)
		wild_build[i].num = 0;

	for (i = 0; i < place_count; i++)
	{
		place_type *pl_ptr = &place[i];
		if (pl_ptr->type == PL_TOWN_OLD || pl_ptr->type == PL_TOWN_FRACT) towns++;
		for (j = 0; j < pl_ptr->numstores; j++)
		{
			store_type *st_ptr = &pl_ptr->store[j];
			if (st_ptr->type == BUILD_STAIRS) stairs++;
			else if (st_ptr->type != BUILD_NONE && st_ptr->type != BUILD_BLANK) stores++;
			/* count the stores, in case it's requested */
			wild_build[st_ptr->type].num++;
		}
	}
	msgf("%i towns, %i stores, %i stairs", towns, stores, stairs);

	/* Make entire wilderness known */
	{
		int i, j;
		for (i = 0; i < WILD_SIZE; i++) {
			for (j=0; j < WILD_SIZE; j++) {
				wild[i][j].done.info |= WILD_INFO_SEEN;
			}
		}
	}

	if (get_check("Give complete store count? "))
	{
		for (i = 0; i < MAX_CITY_BUILD; i++)
		{
			msgf("%s: %i", building_name(i), wild_build[i].num);
		}
	}
	if (get_check("Show wilderness difficulty map? "))
		do_cmd_view_law_map();
}

#ifdef ALLOW_SPOILERS

/*
 * External function
 */
extern void do_cmd_spoilers(void);

#endif /* ALLOW_SPOILERS */



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(void);


/*
 * Ask for and parse a "debug command"
 * The "cmd.arg" may have been set.
 */
void do_cmd_debug(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int x, y;
	char cmd;


	/* Get a "debug command" */
	(void)get_com("Debug Command: ", &cmd);

	/* Analyze the command */
	switch (cmd)
	{
		case ESCAPE:
		case ' ':
		case '\n':
		case '\r':
		{
			/* Nothing */
			break;
		}

#ifdef ALLOW_SPOILERS
		case '"':
		{
			/* Hack -- Generate Spoilers */
			do_cmd_spoilers();
			break;
		}
#endif /* ALLOW_SPOILERS */

		case '?':
		{
			/* Hack -- Help */
			(void)show_file("wizard.txt", NULL, 0, 0);
			break;
		}

		case 'a':
		{
			/* Cure all maladies */
			do_cmd_wiz_cure_all();
			break;
		}

		case 'A':
		{
			/* Know alignment */
			msgf("Your alignment is %d.", p_ptr->align);
			break;
		}

		case 'b':
		{
			/* Teleport to target */
			do_cmd_wiz_bamf();
			break;
		}

		case 'c':
		{
			/* Create any object */
			wiz_create_item();
			break;
		}

		case 'C':
		{
			/* Create a named artifact */
			wiz_create_named_art(p_ptr->cmd.arg);
			break;
		}

		case 'd':
		{
			/* Detect everything */
			(void)detect_all();
			break;
		}

		case 'e':
		{
			/* Edit character */
			do_cmd_wiz_change();
			break;
		}

		case 'f':
		{
			/* View item info */
			(void)identify_fully();
			break;
		}

		case 'F':
		{
			/* Create feature */
			if (p_ptr->cmd.arg > 0) do_cmd_wiz_feature(p_ptr->cmd.arg);
			break;
		}

		case 'g':
		{
			/* Good Objects */
			if (p_ptr->cmd.arg <= 0) p_ptr->cmd.arg = 1;
			acquirement(px, py, p_ptr->cmd.arg, FALSE, TRUE);
			break;
		}

		case 'h':
		{
			/* Hitpoint rerating */
			do_cmd_rerate();
			break;
		}

#ifdef MONSTER_HORDES
		case 'H':
		{
			do_cmd_summon_horde();
			break;
		}
#endif /* MONSTER_HORDES */

		case 'i':
		{
			/* Identify */
			(void)ident_spell();
			break;
		}

		case 'I':
		{
			/* Fields Integrity */
			(void)test_field_data_integrity();
			break;
		}

		case 'j':
		{
			/* Go up or down in the dungeon */
			do_cmd_wiz_jump();
			break;
		}

		case 'J':
		{
			/* Test compression code */
			/* test_compress_module(); */
			break;
		}

		case 'k':
		{
			/* Self-Knowledge */
			self_knowledge();
			break;
		}

		case 'K':
		{
			/* Debug lua stack depth */
			debug_lua_stack();
			break;
		}

		case 'l':
		{
			/* Learn about objects */
			do_cmd_wiz_learn();
			break;
		}

		case 'L':
		{
			/* Lose Mutation */
			(void)lose_mutation(p_ptr->cmd.arg);
			break;
		}

		case 'm':
		{
			/* Magic Mapping */
			map_area();
			break;
		}

		case 'M':
		{
			/* Gain Mutation */
			(void)gain_mutation(p_ptr->cmd.arg);
			break;
		}

		case 'r':
		{
			/* Specific reward */
			(void)gain_level_reward(p_ptr->cmd.arg);
			break;
		}

		case 'N':
		{
			/* Summon _friendly_ named monster */
			do_cmd_wiz_named_friendly(p_ptr->cmd.arg, TRUE);
			break;
		}

		case 'n':
		{
			/* Summon Named Monster */
			do_cmd_wiz_named(p_ptr->cmd.arg, TRUE);
			break;
		}

		case 'o':
		{
			/* Object playing routines */
			do_cmd_wiz_play();
			break;
		}

		case 'p':
		{
			/* Phase Door */
			teleport_player(10);
			break;
		}

		case 'P':
		{
			/* Polymorph self */
			do_poly_self();
			break;
		}

		case 'u':
		{
			/* Make every dungeon square "known" to test streamers -KMW- */
			for (y = p_ptr->min_hgt; y < p_ptr->max_hgt; y++)
			{
				for (x = p_ptr->min_wid; x < p_ptr->max_wid; x++)
				{
					area(x, y)->info |= (CAVE_GLOW);
					parea(x, y)->feat = area(x, y)->feat;
				}
			}

			wiz_lite();
			break;
		}

		case 's':
		{
			/* Summon Random Monster(s) */
			if (p_ptr->cmd.arg <= 0) p_ptr->cmd.arg = 1;
			do_cmd_wiz_summon(p_ptr->cmd.arg);
			break;
		}

		case 't':
		{
			/* Teleport */
			teleport_player(100);
			break;
		}

		case 'T':
		{
			do_cmd_wiz_towns();
			break;
		}


		case KTRL('T'):
		{
			do_cmd_wiz_town_teleport();
			break;
		}

		case 'v':
		{
			/* Very Good Objects */
			if (p_ptr->cmd.arg <= 0) p_ptr->cmd.arg = 1;
			acquirement(px, py, p_ptr->cmd.arg, TRUE, TRUE);
			break;
		}

		case 'w':
		{
			/* Wizard Light the Level */
			if (p_ptr->depth)
			{
				wiz_lite();
			}
			else
			{
				learn_map();
			}
			break;
		}

#ifdef DEBUG
		case 'W':
		{
			break;
		}

#endif /* DEBUG */
		case 'x':
		{
			/* Increase Experience */
			if (p_ptr->cmd.arg)
			{
				gain_exp(p_ptr->cmd.arg);
			}
			else
			{
				gain_exp(p_ptr->exp + 1);
			}
			break;
		}

		case 'z':
		{
			/* Zap Monsters (Genocide) */
			do_cmd_wiz_zap();
			break;
		}

		case 'Z':
		{
			do_cmd_wiz_zap_all();
			break;
		}

		case '_':
		{
			/* Hack -- whatever I desire */
			do_cmd_wiz_hack_ben(p_ptr->cmd.arg);
			break;
		}

		case '+':
		{
			/* Hack -- whatever I desire */
			do_cmd_wiz_hack_mango(p_ptr->cmd.arg);
			break;
		}

#ifdef DEBUG_SCRIPTS
		case '@':
		{
			/* Execute script */
			do_cmd_script();
			break;
		}
#endif /* DEBUG_SCRIPTS */

		default:
		{
			/* Not a Debug Command */
			msgf("That is not a valid debug command.");
			break;
		}
	}
}


#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif
