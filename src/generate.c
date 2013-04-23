#define GENERATE_C
/* File: generate.c */

/* Purpose: Dungeon generation */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#define SAFE_MAX_ATTEMPTS 5000

/*
 * Note that Level generation is *not* an important bottleneck,
 * though it can be annoyingly slow on older machines...  Thus
 * we emphasize "simplicity" and "correctness" over "speed".
 *
 * This entire file is only needed for generating levels.
 * This may allow smart compilers to only load it when needed.
 *
 * Consider the "v_info.txt" file for vault generation.
 *
 * In this file, we use the "special" granite and perma-wall sub-types,
 * where "basic" is normal, "inner" is inside a room, "outer" is the
 * outer wall of a room, and "solid" is the outer wall of the dungeon
 * or any walls that may not be pierced by corridors.  Thus the only
 * wall type that may be pierced by a corridor is the "outer granite"
 * type.  The "basic granite" type yields the "actual" corridors.
 *
 * Note that we use the special "solid" granite wall type to prevent
 * multiple corridors from piercing a wall in two adjacent locations,
 * which would be messy, and we use the special "outer" granite wall
 * to indicate which walls "surround" rooms, and may thus be "pierced"
 * by corridors entering or leaving the room.
 *
 * Note that a tunnel which attempts to leave a room near the "edge"
 * of the dungeon in a direction toward that edge will cause "silly"
 * wall piercings, but will have no permanently incorrect effects,
 * as long as the tunnel can *eventually* exit from another side.
 * And note that the wall may not come back into the room by the
 * hole it left through, so it must bend to the left or right and
 * then optionally re-enter the room (at least 2 grids away).  This
 * is not a problem since every room that is large enough to block
 * the passage of tunnels is also large enough to allow the tunnel
 * to pierce the room itself several times.
 *
 * Note that no two corridors may enter a room through adjacent grids,
 * they must either share an entryway or else use entryways at least
 * two grids apart.  This prevents "large" (or "silly") doorways.
 *
 * To create rooms in the dungeon, we first divide the dungeon up
 * into "blocks" of 11x11 grids each, and require that all rooms
 * occupy a rectangular group of blocks.  As long as each room type
 * reserves a sufficient number of blocks, the room building routines
 * will not need to check bounds.  Note that most of the normal rooms
 * actually only use 23x11 grids, and so reserve 33x11 grids.
 *
 * Note that the use of 11x11 blocks (instead of the old 33x11 blocks)
 * allows more variability in the horizontal placement of rooms, and
 * at the same time has the disadvantage that some rooms (two thirds
 * of the normal rooms) may be "split" by panel boundaries.  This can
 * induce a situation where a player is in a room and part of the room
 * is off the screen.  It may be annoying enough to go back to 33x11
 * blocks to prevent this visual situation.
 *
 * Note that the dungeon generation routines are much different (2.7.5)
 * and perhaps "DUN_ROOMS" should be less than 50.
 *
 * XXX XXX XXX Note that it is possible to create a room which is only
 * connected to itself, because the "tunnel generation" code allows a
 * tunnel to leave a room, wander around, and then re-enter the room.
 *
 * XXX XXX XXX Note that it is possible to create a set of rooms which
 * are only connected to other rooms in that set, since there is nothing
 * explicit in the code to prevent this from happening.  But this is less
 * likely than the "isolated room" problem, because each room attempts to
 * connect to another room, in a giant cycle, thus requiring at least two
 * bizarre occurances to create an isolated section of the dungeon.
 *
 * Note that (2.7.9) monster pits have been split into monster "nests"
 * and monster "pits".  The "nests" have a collection of monsters of a
 * given type strewn randomly around the room (jelly, animal, or undead),
 * while the "pits" have a collection of monsters of a given type placed
 * around the room in an organized manner (orc, troll, giant, dragon, or
 * demon).  Note that both "nests" and "pits" are now "level dependant",
 * and both make 16 "expensive" calls to the "get_mon_num()" function.
 *
 * Note that the cave grid flags changed in a rather drastic manner
 * for Angband 2.8.0 (and 2.7.9+), in particular, dungeon terrain
 * features, such as doors and stairs and traps and rubble and walls,
 * are all handled as a set of 64 possible "terrain features", and
 * not as "fake" objects (440-479) as in pre-2.8.0 versions.
 *
 * The 64 new "dungeon features" will also be used for "visual display"
 * but we must be careful not to allow, for example, the user to display
 * hidden traps in a different way from floors, or secret doors in a way
 * different from granite walls, or even permanent granite in a different
 * way from granite.  XXX XXX XXX
 */


/*
 * Dungeon generation values
 */
#define DUN_ROOMS 50 /* Number of rooms to attempt */
#define DUN_UNUSUAL 194 /* Level/chance of unusual room (was 200) */
#define DUN_DEST    18  /* 1/chance of having a destroyed level */
#define SMALL_LEVEL 3   /* 1/chance of smaller size (3)*/
#define EMPTY_LEVEL 15  /* 1/chance of being 'empty' (15)*/
#define DARK_EMPTY  5   /* 1/chance of arena level NOT being lit (2)*/

/*
 * Dungeon tunnel generation values
 */
#define DUN_TUN_RND 10 /* Chance of random direction */
#define DUN_TUN_CHG 30 /* Chance of changing direction */
#define DUN_TUN_CON 15 /* Chance of extra tunneling */
#define DUN_TUN_PEN 25 /* Chance of doors at room entrances */
#define DUN_TUN_JCT 90 /* Chance of doors at tunnel junctions */

/*
 * Dungeon streamer generation values
 */
#define DUN_STR_DEN 5 /* Density of streamers */
#define DUN_STR_RNG 2 /* Width of streamers */
#define DUN_STR_MAG 3 /* Number of magma streamers */
#define DUN_STR_MC 90 /* 1/chance of treasure per magma */
#define DUN_STR_QUA 2 /* Number of quartz streamers */
#define DUN_STR_QC 40 /* 1/chance of treasure per quartz */

/*
 * Dungeon treausre allocation values
 */
#define DUN_AMT_ROOM 9 /* Amount of objects for rooms */
#define DUN_AMT_ITEM 3 /* Amount of objects for rooms/corridors */
#define DUN_AMT_GOLD 3 /* Amount of treasure for rooms/corridors */

/*
 * Hack -- Dungeon allocation "places"
 */
#define ALLOC_SET_CORR 1 /* Hallway */
#define ALLOC_SET_ROOM 2 /* Room */
#define ALLOC_SET_BOTH 3 /* Anywhere */

/*
 * Hack -- Dungeon allocation "types"
 */
#define ALLOC_TYP_RUBBLE 1 /* Rubble */
#define ALLOC_TYP_TRAP 3 /* Trap */
#define ALLOC_TYP_GOLD 4 /* Gold */
#define ALLOC_TYP_OBJECT 5 /* Object */



/*
 * The "size" of a "generation block" in grids
 */
#define BLOCK_HGT 11
#define BLOCK_WID 11

/*
 * Maximum numbers of rooms along each axis (currently 6x6)
 */
#define MAX_ROOMS_ROW (MAX_HGT / BLOCK_HGT)
#define MAX_ROOMS_COL (MAX_WID / BLOCK_WID)


/*
 * Bounds on some arrays used in the "dun_data" structure.
 * These bounds are checked, though usually this is a formality.
 */
#define CENT_MAX 100
#define DOOR_MAX 200
#define WALL_MAX 500
#define TUNN_MAX 900


/*
 * Maximal number of room types
 */
#define ROOM_MAX 10

static bool new_player_spot(void);

/*
 * Return TRUE if (x,y) contains feature v and no monsters.
 */
static bool PURE is_feat_spot(int y, int x, vptr v)
{
	int feat = *((byte*)(v));

	return (cave_feat[y][x] == feat && !cave_m_idx[y][x]);
}

static bool place_player(void)
{
	/* If the player teleported (or recalled) in, place them randomly */
	if (p_ptr->came_from == START_RANDOM)
	{
		return new_player_spot();
	}
	else if(p_ptr->came_from == START_WALK)
	{

		/* Don't need to alter px & py,
			but need to make sure player does not
			start on a tree or water */
		if((cave_feat[p_ptr->py][p_ptr->px] == FEAT_TREE) ||
			(cave_feat[p_ptr->py][p_ptr->px] == FEAT_BUSH) ||
			(cave_feat[p_ptr->py][p_ptr->px] == FEAT_WATER))
		{
			cave_feat[p_ptr->py][p_ptr->px] = FEAT_FLOOR;
		}
		player_place(p_ptr->py,p_ptr->px);
	}
	/* If the player should start from a feature, move him there. */
	else
	{
		int x, y;

		/* Find a spot. */
		if (!rand_location(&y, &x, is_feat_spot, &p_ptr->came_from)) return FALSE;

		/* Set the player's location there. */
		player_place(y,x);
	}
	return TRUE;
}


/*
 * Simple structure to hold a map location
 */


typedef struct coord coord;

struct coord
{
	byte y;
	byte x;
};


/*
 * Room type information
 */

typedef struct room_data room_data;

struct room_data
{
	/* Required size in blocks */
	s16b dy1, dy2, dx1, dx2;

	/* Hack -- minimum level */
	s16b level;
};


/*
 * Structure to hold all "dungeon generation" data
 */

typedef struct dun_data dun_data;

struct dun_data
{
	/* Array of centers of rooms */
	int cent_n;
	coord cent[CENT_MAX];

	/* Array of possible door locations */
	int door_n;
	coord door[DOOR_MAX];

	/* Array of wall piercing locations */
	int wall_n;
	coord wall[WALL_MAX];

	/* Array of tunnel grids */
	int tunn_n;
	coord tunn[TUNN_MAX];

	/* Number of blocks along each axis */
	int row_rooms;
	int col_rooms;

	/* Array of which blocks are used */
	bool room_map[MAX_ROOMS_ROW][MAX_ROOMS_COL];

	/* Hack -- there is a pit/nest on this level */
	bool crowded;
};


/*
 * Dungeon generation data -- see "cave_gen()"
 */
static dun_data *dun;

/* Notice if the game is allowed to give a special feeling. */
static bool good_item_flag;

void generate_spirit_name(spirit_type *s_ptr)
{
	if (s_ptr->pact) return;

	if (s_ptr->sphere == SPIRIT_NATURE)
	{
		create_random_name(elf_syllables, s_ptr->name);
	}
	else
	{
		create_random_name(hobbit_syllables, s_ptr->name);
	}
}

/* Re-randomise spirit names after town creation */
void generate_spirit_names(void)
{
	int i;
	for (i=0;i<MAX_SPIRITS;i++)
	{
		generate_spirit_name(spirits+i);
	}
}

/*
 * Express cave_empty_bold() as a function to allow pointers to it.
 */
bool PURE cave_empty_bold_p(int y, int  x)
{
	return cave_empty_bold(y, x);
}

/*
 * Array of room types (assumes 11x11 blocks)
 */
static room_data room_info[ROOM_MAX] =
{
	{  0, 0,  0, 0,  0 }, /* 0 = Nothing */
	{  0, 0, -1, 1,  1 }, /* 1 = Simple (33x11) */
	{  0, 0, -1, 1,  1 }, /* 2 = Overlapping (33x11) */
	{  0, 0, -1, 1,  3 }, /* 3 = Crossed (33x11) */
	{  0, 0, -1, 1,  3 }, /* 4 = Large (33x11) */
	{  0, 0, -1, 1,  5 }, /* 5 = Monster nest (33x11) */
	{  0, 0, -1, 1,  5 }, /* 6 = Monster pit (33x11) */
	{  0, 1, -1, 1,  5 }, /* 7 = Lesser vault (33x22) */
	{ -1, 2, -2, 3, 10 }, /* 8 = Greater vault (66x44) */
	{  0, 1, -1, 1,  1 }  /* 9 = Quest rooms  (Heino Vander Sanden) */
};
/*
 * place a friendly monster next to the player
 * Note: The monster is assumed to already exist.
 */
static void replace_friend(int m_idx)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	int ny, nx, d, i, min;
	int dis = 2;
	bool            look = TRUE;

	monster_type    *m_ptr = &m_list[m_idx];

	/* Be quite generous... */
	int attempts = 5000;

	/* Paranoia */
	if (!m_ptr->r_idx) return;

	/* Minimum distance */
	min = dis / 2;

	/* Look until done */
	while ((look) && --attempts)
	{
		/* Verify max distance */
		if (dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (1)
			{
				ny = rand_spread(py, dis);
				nx = rand_spread(px, dis);
				d = distance(py, px, ny, nx);
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds_fully(ny, nx)) continue;

			/* Require "empty" floor space */
			if (!cave_empty_bold(ny, nx)) continue;

			/* Hack -- no teleport onto glyph of warding */
			if (cave_feat[ny][nx] == FEAT_GLYPH) continue;
			if (cave_feat[ny][nx] == FEAT_MINOR_GLYPH) continue;

			/* ...nor onto the Pattern */
			if ((cave_feat[ny][nx] >= FEAT_PATTERN_START) &&
				(cave_feat[ny][nx] <= FEAT_PATTERN_XTRA2)) continue;

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;

		/* Decrease the minimum distance */
		min = min / 2;
	}

	if (attempts < 1)
	{
		msg_format("You get separated from %v.", monster_desc_f2, m_ptr, 0x80);
		return;
	}
	/* Update the new location */
	cave_m_idx[ny][nx] = m_idx;

	/* Move the monster */
	m_ptr->fy = ny;
	m_ptr->fx = nx;

	/* Update the monster (new location) */
	update_mon(m_idx, TRUE);

}

/* Replace all friends on new level */

static void replace_all_friends(void)
{
	int i;
	for(i=0;i<m_max;i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip non-pets */
		if (!(m_ptr->smart & SM_ALLY)) continue;

		/* Place next to player */
		replace_friend(i);

	}
}


/*
 * Helper for plasma generation.
 */
static void perturb_point_mid(int x1, int x2, int x3, int x4,
				int xmid, int ymid, int rough, int depth_max)
{
	/*
	 * Average the four corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough*2 + 1;
	int tmp = randint(tmp2) - (rough+1);

	int avg = ((x1 + x2 + x3 + x4) / 4) + tmp;

	/* Normalize */
	if (avg < 0) avg = 0;
	if (avg > depth_max) avg = depth_max;

	/* Set the new value. */
	cave_feat[ymid][xmid] = avg;
}


static void perturb_point_end(int x1, int x2, int x3,
				int xmid, int ymid, int rough, int depth_max)
{
	/*
	 * Average the three corners & perturb it a bit.
	 * tmp is a random int +/- rough
	 */
	int tmp2 = rough*2 + 2;
	int tmp = randint(tmp2);

	int avg;

	if (rand_int(100) < 50)
		avg = ((x1 + x2 + x3) / 3) + tmp;
	else
		avg = ((x1 + x2 + x3) / 3) - tmp;

	/* Normalize */
	if (avg < 0) avg = 0;
	if (avg > depth_max) avg = depth_max;

	/* Set the new value. */
	cave_feat[ymid][xmid] = avg;
}


/*
 * A generic function to generate the plasma fractal.
 * Note that it uses ``cave_feat'' as temporary storage.
 * The values in ``cave_feat'' after this function
 * are NOT actual features; They are raw heights which
 * need to be converted to features.
 */
static void plasma_recursive(int x1, int y1, int x2, int y2,
				int depth_max, int rough)
{
	/* Find middle */
	int xmid = (x2-x1)/2 + x1;
	int ymid = (y2-y1)/2 + y1;

	/* Are we done? */
	if (x1+1 == x2)
	{
		return;
	}

	perturb_point_mid(cave_feat[y1][x1], cave_feat[y2][x1], cave_feat[y1][x2],
		cave_feat[y2][x2], xmid, ymid, rough, depth_max);

	perturb_point_end(cave_feat[y1][x1], cave_feat[y1][x2],cave_feat[ymid][xmid],
		xmid, y1, rough, depth_max);

	perturb_point_end(cave_feat[y1][x2], cave_feat[y2][x2],cave_feat[ymid][xmid],
		x2, ymid, rough, depth_max);

	perturb_point_end(cave_feat[y2][x2], cave_feat[y2][x1],cave_feat[ymid][xmid],
		xmid, y2, rough, depth_max);

	perturb_point_end(cave_feat[y2][x1], cave_feat[y1][x1],cave_feat[ymid][xmid],
		x1, ymid, rough, depth_max);


	/* Recurse the four quadrants */
	plasma_recursive(x1, y1, xmid, ymid, depth_max, rough);
	plasma_recursive(xmid, y1, x2, ymid, depth_max, rough);
	plasma_recursive(x1, ymid, xmid, y2, depth_max, rough);
	plasma_recursive(xmid, ymid, x2, y2, depth_max, rough);
}


/*
 * The default table in terrain level generation.
 */

static int terrain_table[6][9] =
{
	{
			FEAT_WATER,
			FEAT_WATER,
			FEAT_WATER,

			FEAT_WATER,
			FEAT_WATER,
			FEAT_WATER,

			FEAT_WATER,
			FEAT_WATER,
			FEAT_WATER,

  		      /*0, 0, 0,
			0, 0, 0,
			0, 0, 0,*/
	},
	{
			FEAT_WATER,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_BUSH,
			FEAT_BUSH,
			FEAT_TREE,
	},
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_BUSH,
			FEAT_BUSH,
			FEAT_TREE,
	},
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_BUSH,

			FEAT_BUSH,
			FEAT_TREE,
			FEAT_TREE,
	},
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_FLOOR,
			FEAT_BUSH,
			FEAT_BUSH,

			FEAT_TREE,
			FEAT_TREE,
			FEAT_TREE,
	},
	{
			FEAT_FLOOR,
			FEAT_FLOOR,
			FEAT_FLOOR,

			FEAT_BUSH,
			FEAT_BUSH,
			FEAT_TREE,

			FEAT_TREE,
			FEAT_TREE,
			FEAT_TREE,
	},
};


bool PURE daytime_p(void)
{
	return ((turn % (10L * TOWN_DAWN)) < ((10L * TOWN_DAWN) / 2));
}


/*
 * Don't create stairs if forbidden by the DF_NIGHTTIME flag.
 */
static bool PURE hide_terrain_stairs(void)
{
	int d = wild_grid[p_ptr->wildy][p_ptr->wildx].dungeon;

	/* No stairs to hide. */
	if (d >= MAX_CAVES) return FALSE;

	/* No day time ban on stairs. */
	if (~dun_defs[d].flags & DF_NIGHTTIME) return FALSE;

	/* It is night. */
	if (!daytime_p()) return FALSE;

	/* Always show the stairs we just used. */
	if (p_ptr->came_from == START_DOWN_STAIRS) return FALSE;

	/* Cover the stairs up. */
	return TRUE;
}

/*
 * Handle daylight and add some random monsters.
 */
static void surface_gen_final(int monsters)
{
	bool day = daytime_p();
	int y, x;

	/* Daylight makes things glow. */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			/* Perma-Lite */
			if (day) cave_info[y][x] |= (CAVE_GLOW);

			/* Memorize */
			if ((day && view_perma_grids) || (cave_info[y][x] & CAVE_MARK))
				mark_spot(y, x);
		}
	}

	/* Make some residents. */
	while (monsters--) alloc_monster(3, dun_depth, TRUE);
}

/*
 * Generate a terrain level using ``plasma'' fractals.
 *
 */
static cptr terrain_gen(void)
{
	int x, y,main_feat,door_feat,do_posts,dummy;
	int table_size = sizeof(terrain_table[0]) / sizeof(int);
	int roughness = 1; /* The roughness of the level. */
	int terrain[3][3]; /* The terrain around the current area */
	int wildx = p_ptr->wildx; 
	int wildy= p_ptr->wildy;

	x = wildx;
	y = wildy;
	/* Initialize the terrain array */
	terrain[0][0] = wild_grid[y - 1][x - 1].terrain;
	terrain[0][1] = wild_grid[y - 1][x].terrain;
	terrain[0][2] = wild_grid[y - 1][x + 1].terrain;
	terrain[1][0] = wild_grid[y][x - 1].terrain;
	terrain[1][1] = wild_grid[y][x].terrain;
	terrain[1][2] = wild_grid[y][x + 1].terrain;
	terrain[2][0] = wild_grid[y + 1][x - 1].terrain;
	terrain[2][1] = wild_grid[y + 1][x].terrain;
	terrain[2][2] = wild_grid[y + 1][x + 1].terrain;

	/* Create level background */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave_feat[y][x] = table_size / 2;
		}
	}


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant wilderness layout */
	Rand_value = wild_grid[wildy][wildx].seed;

	/* Initialize the four corners
	 *
	 * We calculate the medium height of the adjacent
	 * terrains for every corner.
	 */
	cave_feat[1][1] = randint(table_size);
	cave_feat[cur_hgt-2][1] = randint(table_size);
	cave_feat[1][cur_wid-2] = randint(table_size);
	cave_feat[cur_hgt-2][cur_wid-2] = randint(table_size);


	/* x1, y1, x2, y2, num_depths, roughness */
	plasma_recursive(1, 1, cur_wid-2, cur_hgt-2, table_size-1, roughness);

	/* Translate returned values to 'proper' FEAT_* codes */
	for (y = 1; y < cur_hgt-1; y++)
	{
		for (x = 1; x < cur_wid-1; x++)
		{
			/* Hack - have the terrain slightly dependant on neighbouring tiles. */ 
			/* cave[y][x].feat = terrain_table[terrain[1][1]][cave[y][x].feat];*/ 
			cave_feat[y][x] = terrain_table[terrain[one_in(3)?y*3/cur_hgt:1][one_in(3)?x*3/cur_wid:1]][cave_feat[y][x]]; 
		}
	}


	/* Special boundary walls -- Top */
	main_feat=FEAT_WILD_BORDER;
	door_feat=FEAT_WILD_BORDER;
	do_posts=0;
	if(wild_grid[wildy-1][wildx].terrain == 0)
	{
		main_feat=FEAT_WATER_BORDER;
		door_feat=FEAT_WATER_BORDER;
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_UP)
	{
		door_feat = FEAT_PATH_BORDER;
		do_posts=2;
	}
	if(is_town_p(wildy-1,wildx))
	{
		main_feat=FEAT_PERM_SOLID;
		door_feat=FEAT_WILD_BORDER;
		do_posts=1;
	}

	for (x = 0; x < cur_wid; x++)
	{
		y = 0;
		cave_feat[y][x] = main_feat;
		if (main_feat != FEAT_WILD_BORDER) cave_info[y][x] |= (CAVE_MARK | CAVE_GLOW);
	}
	cave_feat[0][cur_wid/2] = door_feat;
	if(do_posts==1)
	{
		cave_feat[1][cur_wid/2-1] = FEAT_PERM_SOLID;
		cave_feat[2][cur_wid/2-1] = FEAT_FLOOR;
		cave_info[1][cur_wid/2-1] |= (CAVE_MARK | CAVE_GLOW);
		cave_feat[1][cur_wid/2] = FEAT_OPEN;
		cave_feat[2][cur_wid/2] = FEAT_FLOOR;
		cave_feat[1][cur_wid/2+1] = FEAT_PERM_SOLID;
		cave_feat[2][cur_wid/2+1] = FEAT_FLOOR;
		cave_info[1][cur_wid/2+1] |= (CAVE_MARK | CAVE_GLOW);
	}
	else if (do_posts==2)
	{
		cave_feat[0][cur_wid/2-1]=FEAT_PATH_BORDER;
		cave_feat[0][cur_wid/2+1]=FEAT_PATH_BORDER;
		cave_feat[1][cur_wid/2-1]=FEAT_PATH;
		cave_feat[1][cur_wid/2]=FEAT_PATH;
		cave_feat[1][cur_wid/2+1]=FEAT_PATH;
	}
	/* Special boundary walls -- Bottom */
	main_feat=FEAT_WILD_BORDER;
	door_feat=FEAT_WILD_BORDER;
	do_posts=0;
	if(wild_grid[wildy+1][wildx].terrain == 0)
	{
		main_feat=FEAT_WATER_BORDER;
		door_feat=FEAT_WATER_BORDER;
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_DOWN)
	{
		door_feat = FEAT_PATH_BORDER;
		do_posts=2;
	}
	if(is_town_p(wildy+1,wildx))
	{
		main_feat=FEAT_PERM_SOLID;
		door_feat=FEAT_WILD_BORDER;
		do_posts=1;
	}

	for (x = 0; x < cur_wid; x++)
	{
		y = cur_hgt-1;
		cave_feat[y][x] = main_feat;
		if (main_feat != FEAT_WILD_BORDER) cave_info[y][x] |= (CAVE_MARK | CAVE_GLOW);
	}
	cave_feat[cur_hgt-1][cur_wid/2]=door_feat;
	if(do_posts == 1)
	{
		cave_feat[cur_hgt-2][cur_wid/2-1] = FEAT_PERM_SOLID;
		cave_feat[cur_hgt-3][cur_wid/2-1] = FEAT_FLOOR;
		cave_info[cur_hgt-2][cur_wid/2-1] |= (CAVE_MARK | CAVE_GLOW);
		cave_feat[cur_hgt-2][cur_wid/2] = FEAT_OPEN;
		cave_feat[cur_hgt-3][cur_wid/2] = FEAT_FLOOR;
		cave_feat[cur_hgt-2][cur_wid/2+1] = FEAT_PERM_SOLID;
		cave_feat[cur_hgt-3][cur_wid/2+1] = FEAT_FLOOR;
		cave_info[cur_hgt-2][cur_wid/2+1] |= (CAVE_MARK | CAVE_GLOW);
	}
	else if(do_posts == 2)
	{
		cave_feat[cur_hgt-1][cur_wid/2-1]=FEAT_PATH_BORDER;
		cave_feat[cur_hgt-1][cur_wid/2+1]=FEAT_PATH_BORDER;
		cave_feat[cur_hgt-2][cur_wid/2-1]=FEAT_PATH;
		cave_feat[cur_hgt-2][cur_wid/2]=FEAT_PATH;
		cave_feat[cur_hgt-2][cur_wid/2+1]=FEAT_PATH;
	}

	/* Special boundary walls -- Left */
	main_feat=FEAT_WILD_BORDER;
	door_feat=FEAT_WILD_BORDER;
	do_posts=0;
	if(wild_grid[wildy][wildx-1].terrain == 0)
	{
		main_feat=FEAT_WATER_BORDER;
		door_feat=FEAT_WATER_BORDER;
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_LEFT)
	{
		door_feat=FEAT_PATH_BORDER;
		do_posts=2;
	}
	if(is_town_p(wildy, wildx-1))
	{
		main_feat=FEAT_PERM_SOLID;
		door_feat=FEAT_WILD_BORDER;
		do_posts=1;
	}

	for (y = 0; y < cur_hgt; y++)
	{
		x = 0;
		cave_feat[y][x] = main_feat;
		if (main_feat != FEAT_WILD_BORDER) cave_info[y][x] |= (CAVE_MARK | CAVE_GLOW);
	}
	cave_feat[cur_hgt/2][0]=door_feat;
	if(do_posts==1)
	{
		cave_feat[cur_hgt/2-1][1] = FEAT_PERM_SOLID;
		cave_feat[cur_hgt/2-1][2] = FEAT_FLOOR;
		cave_info[cur_hgt/2-1][1] |= (CAVE_MARK | CAVE_GLOW);
		cave_feat[cur_hgt/2][1] = FEAT_OPEN;
		cave_feat[cur_hgt/2][2] = FEAT_FLOOR;
		cave_feat[cur_hgt/2+1][1] = FEAT_PERM_SOLID;
		cave_feat[cur_hgt/2+1][2] = FEAT_FLOOR;
		cave_info[cur_hgt/2+1][1] |= (CAVE_MARK | CAVE_GLOW);
	}
	else if(do_posts==2)
	{
		cave_feat[cur_hgt/2-1][0]=FEAT_PATH_BORDER;
		cave_feat[cur_hgt/2+1][0]=FEAT_PATH_BORDER;
		cave_feat[cur_hgt/2-1][1]=FEAT_PATH;
		cave_feat[cur_hgt/2][1]=FEAT_PATH;
		cave_feat[cur_hgt/2+1][1]=FEAT_PATH;
	}


	/* Special boundary walls -- Right */
	main_feat=FEAT_WILD_BORDER;
	door_feat=FEAT_WILD_BORDER;
	do_posts=0;
	if(wild_grid[wildy][wildx+1].terrain == 0)
	{
		main_feat=FEAT_WATER_BORDER;
		door_feat=FEAT_WATER_BORDER;
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_RIGHT)
	{
		door_feat=FEAT_PATH_BORDER;
		do_posts=2;
	}
	if(is_town_p(wildy, wildx+1))
	{
		main_feat=FEAT_PERM_SOLID;
		door_feat=FEAT_WILD_BORDER;
		do_posts=1;
	}

	for (y = 0; y < cur_hgt; y++)
	{
		x = cur_wid-1;
		cave_feat[y][x] = main_feat;
		if (main_feat != FEAT_WILD_BORDER) cave_info[y][x] |= (CAVE_MARK | CAVE_GLOW);
	}
	cave_feat[cur_hgt/2][cur_wid-1]=door_feat;
	if(do_posts==1)
	{
		cave_feat[cur_hgt/2-1][cur_wid-2] = FEAT_PERM_SOLID;
		cave_feat[cur_hgt/2-1][cur_wid-3] = FEAT_FLOOR;
		cave_info[cur_hgt/2-1][cur_wid-2] |= (CAVE_MARK | CAVE_GLOW);
		cave_feat[cur_hgt/2][cur_wid-2] = FEAT_OPEN;
		cave_feat[cur_hgt/2][cur_wid-3] = FEAT_FLOOR;
		cave_feat[cur_hgt/2+1][cur_wid-2] = FEAT_PERM_SOLID;
		cave_feat[cur_hgt/2+1][cur_wid-3] = FEAT_FLOOR;
		cave_info[cur_hgt/2+1][cur_wid-2] |= (CAVE_MARK | CAVE_GLOW);
	}
	else if(do_posts==2)
	{
		cave_feat[cur_hgt/2-1][cur_wid-1]=FEAT_PATH_BORDER;
		cave_feat[cur_hgt/2+1][cur_wid-1]=FEAT_PATH_BORDER;
		cave_feat[cur_hgt/2-1][cur_wid-2]=FEAT_PATH;
		cave_feat[cur_hgt/2][cur_wid-2]=FEAT_PATH;
		cave_feat[cur_hgt/2+1][cur_wid-2]=FEAT_PATH;
	}

	/* Corner Posts */
	if (is_town_p(wildy-1, wildx-1) || is_town_p(wildy-1, wildx) ||
		is_town_p(wildy, wildx-1))
	{
		cave_feat[0][0]=FEAT_PERM_SOLID;
		cave_info[0][0] |= (CAVE_MARK | CAVE_GLOW);
	}

	if (is_town_p(wildy-1, wildx+1) || is_town_p(wildy-1, wildx) ||
		is_town_p(wildy, wildx+1))
	{
		cave_feat[0][cur_wid-1]=FEAT_PERM_SOLID;
		cave_info[0][cur_wid-1] |= (CAVE_MARK | CAVE_GLOW);
	}

	if (is_town_p(wildy+1, wildx-1) || is_town_p(wildy+1, wildx) ||
		is_town_p(wildy, wildx-1))
	{
		cave_feat[cur_hgt-1][0]=FEAT_PERM_SOLID;
		cave_info[cur_hgt-1][0] |= (CAVE_MARK | CAVE_GLOW);
	}

	if (is_town_p(wildy+1, wildx+1) || is_town_p(wildy+1, wildx) ||
		is_town_p(wildy, wildx+1))
	{
		cave_feat[cur_hgt-1][cur_wid-1]=FEAT_PERM_SOLID;
		cave_info[cur_hgt-1][cur_wid-1] |= (CAVE_MARK | CAVE_GLOW);
	}

	/* Ensure water in sea corners */
	if((wild_grid[wildy-1][wildx-1].terrain == 0) ||
		(wild_grid[wildy-1][wildx].terrain == 0) ||
		(wild_grid[wildy][wildx-1].terrain == 0))
	{
		cave_feat[0][0]=FEAT_WATER_BORDER;
		cave_info[0][0] |= (CAVE_MARK | CAVE_GLOW);
	}

	if((wild_grid[wildy-1][wildx+1].terrain == 0) ||
		(wild_grid[wildy-1][wildx].terrain == 0) ||
		(wild_grid[wildy][wildx+1].terrain == 0))
	{
		cave_feat[0][cur_wid-1]=FEAT_WATER_BORDER;
		cave_info[0][cur_wid-1] |= (CAVE_MARK | CAVE_GLOW);
	}

	if((wild_grid[wildy+1][wildx-1].terrain == 0) ||
		(wild_grid[wildy+1][wildx].terrain == 0) ||
		(wild_grid[wildy][wildx-1].terrain == 0))
	{
		cave_feat[cur_hgt-1][0]=FEAT_WATER_BORDER;
		cave_info[cur_hgt-1][0] |= (CAVE_MARK | CAVE_GLOW);
	}

	/* Place Roads */
	if(wild_grid[wildy][wildx].road_map & ROAD_UP)
	{
		x=0;
		for(y=2;y<cur_hgt/2+2;y++)
		{
			x=x+rand_range(-1,1);
			if (x>(cur_hgt/2+2-y)) x=((cur_hgt/2+2)-y);
			if (x< -(cur_hgt/2+2-y)) x=-((cur_hgt/2+2)-y);
			cave_feat[y][cur_wid/2-1+x]=FEAT_PATH;
			cave_feat[y][cur_wid/2+x]=FEAT_PATH;
			cave_feat[y][cur_wid/2+1+x]=FEAT_PATH;
		}
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_DOWN)
	{
		x=0;
		for(y=cur_hgt-3;y>cur_hgt/2-2;y--)
		{
			x=x+rand_range(-1,1);
			if (x>(y-(cur_hgt/2-2))) x=y-((cur_hgt/2-2));
			if (x< -(y-(cur_hgt/2-2))) x=-(y-(cur_hgt/2-2));
			cave_feat[y][cur_wid/2-1+x]=FEAT_PATH;
			cave_feat[y][cur_wid/2+x]=FEAT_PATH;
			cave_feat[y][cur_wid/2+1+x]=FEAT_PATH;
		}
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_LEFT)
	{
		y=0;
		for(x=2;x<cur_wid/2+2;x++)
		{
			y=y+rand_range(-1,1);
			if (y>(cur_wid/2+2-x)) y=((cur_wid/2+2)-x);
			if (y< -(cur_wid/2+2-x)) y=-((cur_wid/2+2)-x);
			cave_feat[cur_hgt/2-1+y][x]=FEAT_PATH;
			cave_feat[cur_hgt/2+y][x]=FEAT_PATH;
			cave_feat[cur_hgt/2+1+y][x]=FEAT_PATH;
		}
	}
	if(wild_grid[wildy][wildx].road_map & ROAD_RIGHT)
	{
		y=0;
		for(x=cur_wid-3;x>cur_wid/2-2;x--)
		{
			y=y+rand_range(-1,1);
			if (y>(x-(cur_wid/2-2))) y=x-((cur_wid/2-2));
			if (y< -(x-(cur_wid/2-2))) y=-(x-(cur_wid/2-2));
			cave_feat[cur_hgt/2-1+y][x]=FEAT_PATH;
			cave_feat[cur_hgt/2+y][x]=FEAT_PATH;
			cave_feat[cur_hgt/2+1+y][x]=FEAT_PATH;
		}
	}

	if((wild_grid[wildy+1][wildx+1].terrain == 0) ||
		(wild_grid[wildy+1][wildx].terrain == 0) ||
		(wild_grid[wildy][wildx+1].terrain == 0))
	{
		cave_feat[cur_hgt-1][cur_wid-1]=FEAT_WATER_BORDER;
		cave_info[cur_hgt-1][cur_wid-1] |= (CAVE_MARK | CAVE_GLOW);
	}

	if(wild_grid[wildy][wildx].dungeon < MAX_CAVES)
	{
		dun_type *d_ptr = dun_defs+wild_grid[wildy][wildx].dungeon;
		if (!d_ptr->max_level)
		{
			/* No dungeon. */
		}
		else if (d_ptr->flags & DF_TOWER)
		{
			s16b i;
			/* Place the tower */

			/* Pick a location within the central area */
			y = rand_range(20,cur_hgt-5);
			x = rand_range(10,cur_wid-10);

			/* Build a tower */
			for(i=-2;i<3;i++)
			{
				cave_feat[y][x+i] = FEAT_PERM_BUILDING;
				cave_info[y][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-4;i<5;i++)
			{
				cave_feat[y-1][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-1][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-5;i<6;i++)
			{
				cave_feat[y-2][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-2][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-6;i<7;i++)
			{
				cave_feat[y-3][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-3][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-6;i<7;i++)
			{
				cave_feat[y-4][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-4][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-7;i<8;i++)
			{
				cave_feat[y-5][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-5][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-7;i<8;i++)
			{
				cave_feat[y-6][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-6][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-7;i<8;i++)
			{
				cave_feat[y-7][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-7][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-7;i<8;i++)
			{
				cave_feat[y-8][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-8][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-7;i<8;i++)
			{
				cave_feat[y-9][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-9][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-6;i<7;i++)
			{
				cave_feat[y-10][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-10][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-6;i<7;i++)
			{
				cave_feat[y-11][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-11][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-5;i<6;i++)
			{
				cave_feat[y-12][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-12][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-4;i<5;i++)
			{
				cave_feat[y-13][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-13][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}
			for(i=-2;i<4;i++)
			{
				cave_feat[y-14][x+i] = FEAT_PERM_BUILDING;
				cave_info[y-14][x+i] |= (CAVE_MARK | CAVE_GLOW);
			}

			if (!hide_terrain_stairs())
			{
				/* Access the stair grid */
				cave_feat[y][x]=FEAT_LESS;
				cave_info[y][x] |= (CAVE_MARK | CAVE_GLOW);
			}
			/* Clear a space around the entrance */
			for(i=-3;i<4;i++)
			{
				if((cave_feat[y+1][x+i] == FEAT_TREE) || (cave_feat[y+1][x+i] == FEAT_BUSH))
				{
					cave_feat[y+1][x+i] = FEAT_FLOOR;
				}
			}
			for(i=-2;i<3;i++)
			{
				if((cave_feat[y+2][x+i] == FEAT_TREE) || (cave_feat[y+2][x+i] == FEAT_BUSH))
				{
					cave_feat[y+2][x+i] = FEAT_FLOOR;
				}
			}

		}
		else
		{
			/* Place the stairs */
			dummy = 0;
			while (dummy < SAFE_MAX_ATTEMPTS)
			{
				dummy++;

				/* Pick a location within the central area */
				y = rand_range(5,cur_hgt-5);
				x = rand_range(5,cur_wid-5);

				/* Require a "naked" floor grid */
				if (cave_naked_bold(y, x)) break;
			}

			if (dummy >= SAFE_MAX_ATTEMPTS)
			{
				if (cheat_room)
				{
					msg_print("Warning! Could not place stairs!");
				}
			}


			/* Access the stair grid */
			cave_feat[y-2][x] = FEAT_FLOOR;
			cave_feat[y-1][x-1] = FEAT_FLOOR;
			cave_feat[y-1][x] = FEAT_FLOOR;
			cave_feat[y-1][x+1] = FEAT_FLOOR;
			cave_feat[y][x-2] = FEAT_FLOOR;
			cave_feat[y][x-1] = FEAT_FLOOR;
			if (hide_terrain_stairs())
			{
				/* Someone leaves a rock over the door. */
				cave_feat[y][x]=FEAT_PERM_BUILDING;
			}
			else
			{
				cave_feat[y][x] = FEAT_MORE;
			}
			cave_feat[y][x+1] = FEAT_FLOOR;
			cave_feat[y][x+2] = FEAT_FLOOR;
			cave_feat[y+1][x-1] = FEAT_FLOOR;
			cave_feat[y+1][x] = FEAT_FLOOR;
			cave_feat[y+1][x+1] = FEAT_FLOOR;
			cave_feat[y+2][x] = FEAT_FLOOR;
		}
	}


	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;

	/* Put the player on the map, or fail. */
	if (!place_player())
		return "could not place player";

	/* Handle daylight and add monsters. */
	surface_gen_final(MIN_M_ALLOC_LEVEL);

	return SUCCESS;
}



/*
 * Always picks a correct direction
 */
static void correct_dir(int *rdir, int *cdir, int y1, int x1, int y2, int x2)
{
	/* Extract vertical and horizontal directions */
	*rdir = (y1 == y2) ? 0 : (y1 < y2) ? 1 : -1;
	*cdir = (x1 == x2) ? 0 : (x1 < x2) ? 1 : -1;

	/* Never move diagonally */
	if (*rdir && *cdir)
	{
		if (rand_int(100) < 50)
		{
			*rdir = 0;
		}
		else
		{
			*cdir = 0;
		}
	}
}


/*
 * Pick a random direction
 */
static void rand_dir(int *rdir, int *cdir)
{
	/* Pick a random direction */
	int i = rand_int(4);

	/* Extract the dy/dx components */
	*rdir = ddy_ddd[i];
	*cdir = ddx_ddd[i];
}


/*
 * Returns random co-ordinates for player/monster/object
 */
static bool new_player_spot(void)
{
	int y, x;
	int max_attempts = 5000;

	/* Place the player */
	while (max_attempts--)
	{
		/* Pick a legal spot */
		y = rand_range(1, cur_hgt - 2);
		x = rand_range(1, cur_wid - 2);

		/* Must be a "naked" floor grid */
		if (!cave_naked_bold(y, x)) continue;

		/* Refuse to start on anti-teleport grids */
		if (cave_info[y][x] & (CAVE_ICKY)) continue;

		/* Done */
		break;

	}

	if (max_attempts < 1) /* Should be -1, actually if we failed... */
		return FALSE;


	/* Place the player */
	player_place(y, x);

	return TRUE;
}



/*
 * Count the number of walls adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds_fully(y, x)"
 *
 * We count only granite walls and permanent walls.
 */
static int next_to_walls(int y, int x)
{
	int k = 0;

	if (cave_feat[y+1][x] >= FEAT_WALL_EXTRA) k++;
	if (cave_feat[y-1][x] >= FEAT_WALL_EXTRA) k++;
	if (cave_feat[y][x+1] >= FEAT_WALL_EXTRA) k++;
	if (cave_feat[y][x-1] >= FEAT_WALL_EXTRA) k++;

	return (k);
}



/*
 * Convert existing terrain type to rubble
 */
static void place_rubble(int y, int x)
{
	/* Create rubble */
	cave_feat[y][x] = FEAT_RUBBLE;
}



/*
 * Convert existing terrain type to "up stairs"
 */
static void place_up_stairs(int y, int x)
{
	/* Create up stairs */
	cave_feat[y][x] = FEAT_LESS;
}


/*
 * Convert existing terrain type to "down stairs"
 */
static void place_down_stairs(int y, int x)
{
	/* Create down stairs */
	cave_feat[y][x] = FEAT_MORE;
}





/*
 * Place an up/down staircase at given location
 */
static void place_random_stairs(int y, int x)
{
	/* Paranoia */
	if (!cave_clean_bold(y, x)) return;

	/* Choose a staircase */
	if (dun_level <= 0)
	{
		/* No stairs outside */
		return;
	}
	else if (is_quest() || (dun_level == dun_defs[p_ptr->cur_dungeon].max_level))
	{
		if(dun_defs[p_ptr->cur_dungeon].flags & DF_TOWER)
		{
			place_down_stairs(y, x);
		}
		else
		{
			place_up_stairs(y, x);
		}
	}
	else if (rand_int(100) < 50)
	{
		place_down_stairs(y, x);
	}
	else
	{
		place_up_stairs(y, x);
	}
}


/*
 * Place a locked door at the given location
 */
static void place_locked_door(int y, int x)
{
	/* Create locked door */
	cave_feat[y][x] = FEAT_DOOR_HEAD + randint(7);
}


/*
 * Place a secret door at the given location
 */
static void place_secret_door(int y, int x)
{
	/* Create secret door */
	cave_feat[y][x] = FEAT_SECRET;
}


/*
 * Place a random type of door at the given location
 */
static void place_random_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(1000);

	/* Open doors (300/1000) */
	if (tmp < 300)
	{
		/* Create open door */
		cave_feat[y][x] = FEAT_OPEN;
	}

	/* Broken doors (100/1000) */
	else if (tmp < 400)
	{
		/* Create broken door */
		cave_feat[y][x] = FEAT_BROKEN;
	}

	/* Secret doors (200/1000) */
	else if (tmp < 600)
	{
		/* Create secret door */
		cave_feat[y][x] = FEAT_SECRET;
	}

	/* Closed doors (300/1000) */
	else if (tmp < 900)
	{
		/* Create closed door */
		cave_feat[y][x] = FEAT_DOOR_HEAD + 0x00;
	}

	/* Locked doors (99/1000) */
	else if (tmp < 999)
	{
		/* Create locked door */
		cave_feat[y][x] = FEAT_DOOR_HEAD + randint(7);
	}

	/* Stuck doors (1/1000) */
	else
	{
		/* Create jammed door */
		cave_feat[y][x] = FEAT_DOOR_HEAD + 0x08 + rand_int(8);
	}
}

/*
 * Place a random type of door at the given location to replace a secret door
 */
void replace_secret_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(400);

	/* Closed doors (300/400) */
	if (tmp < 300)
	{
		/* Create closed door */
		cave_set_feat(y,x, FEAT_DOOR_HEAD);
	}

	/* Locked doors (99/400) */
	else if (tmp < 999)
	{
		/* Create locked door */
		cave_set_feat(y,x, FEAT_DOOR_HEAD + randint(7));
	}

	/* Stuck doors (1/400) */
	else
	{
		/* Create jammed door */
		cave_set_feat(y,x, FEAT_DOOR_HEAD + 0x08 + rand_int(8));
	}
}


/*
 * Places some staircases near walls
 */
static void alloc_stairs(int feat, int num, int walls)
{
	int y, x, i, j, flag;


	/* Place "num" stairs */
	for (i = 0; i < num; i++)
	{
		/* Place some stairs */
		for (flag = FALSE; !flag; )
		{
			/* Try several times, then decrease "walls" */
			for (j = 0; !flag && j <= 3000; j++)
			{
				/* Pick a random grid */
				y = rand_int(DUNGEON_HGT);
				x = rand_int(DUNGEON_WID);

				/* Require "naked" floor grid */
				if (!cave_naked_bold(y, x)) continue;

				/* Require a certain number of adjacent walls */
				if (next_to_walls(y, x) < walls) continue;

				/* Town -- must go down */
				if (dun_level <= 0)
				{
					/* Clear previous contents, add down stairs */
					cave_feat[y][x] = FEAT_MORE;
				}

				/* Quest -- must go up */
				else if (is_quest() ||
					(dun_level == dun_defs[p_ptr->cur_dungeon].max_level))
				{
					if (dun_defs[p_ptr->cur_dungeon].flags & DF_TOWER)
					{
						/* Clear previous contents, add down stairs */
						cave_feat[y][x] = FEAT_MORE;
					}
					else
					{
						/* Clear previous contents, add up stairs */
						cave_feat[y][x] = FEAT_LESS;
					}
				}

				/* Requested type */
				else
				{
					/* Clear previous contents, add stairs */
					cave_feat[y][x] = feat;
				}

				/* All done */
				flag = TRUE;
			}

			/* Require fewer walls */
			if (walls) walls--;
		}
	}
}

/*
 * Allocates some objects (using "place" and "type")
 */
static void alloc_object(int set, int typ, int num)
{
	int y, x, k;
	int dummy = 0;

	/* Place some objects */
	for (k = 0; k < num; k++)
	{
		/* Pick a "legal" spot */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			bool room;

			dummy++;

			/* Location */
			y = rand_int(cur_hgt);
			x = rand_int(cur_wid);

			/* Require "naked" floor grid */
			if (!cave_naked_bold(y, x)) continue;

			/* Check for "room" */
			room = (cave_info[y][x] & (CAVE_ROOM)) ? TRUE : FALSE;

			/* Require corridor? */
			if ((set == ALLOC_SET_CORR) && room) continue;

			/* Require room? */
			if ((set == ALLOC_SET_ROOM) && !room) continue;

			/* Accept it */
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place object!");
			}
			return;
		}


		/* Place something */
		switch (typ)
		{
			case ALLOC_TYP_RUBBLE:
			{
				place_rubble(y, x);
				break;
			}

			case ALLOC_TYP_TRAP:
			{
				place_trap(y, x);
				break;
			}

			case ALLOC_TYP_GOLD:
			{
				place_gold(y, x, FOUND_FLOOR, 0);
				break;
			}

			case ALLOC_TYP_OBJECT:
			{
				place_object(y, x, FALSE, FALSE, FOUND_FLOOR, 0);
				break;
			}
		}
	}
}



/*
 * Places "streamers" of rock through dungeon
 *
 * Note that their are actually six different terrain features used
 * to represent streamers.  Three each of magma and quartz, one for
 * basic vein, one with hidden gold, and one with known gold.  The
 * hidden gold types are currently unused.
 */
static void build_streamer(int feat, int chance)
{
	int i, tx, ty;
	int y, x, dir;


	/* Hack -- Choose starting point */
	y = rand_spread(DUNGEON_HGT / 2, 10);
	x = rand_spread(DUNGEON_WID / 2, 15);

	/* Choose a random compass direction */
	dir = ddd[rand_int(8)];

	/* Place streamer into dungeon */
	while (TRUE)
	{
		/* One grid per density */
		for (i = 0; i < DUN_STR_DEN; i++)
		{
			int d = DUN_STR_RNG;

			/* Pick a nearby grid */
			while (1)
			{
				ty = rand_spread(y, d);
				tx = rand_spread(x, d);
				if (!in_bounds(ty, tx)) continue;
				break;
			}

			/* Only convert "granite" walls */
			if (cave_feat[ty][tx] < FEAT_WALL_EXTRA) continue;
			if (cave_feat[ty][tx] > FEAT_WALL_SOLID) continue;

			/* Clear previous contents, add proper vein type */
			cave_feat[ty][tx] = feat;

			/* Hack -- Add some (known) treasure */
			if (rand_int(chance) == 0) cave_feat[ty][tx] += 0x04;
		}

		/* Advance the streamer */
		y += ddy[dir];
		x += ddx[dir];

		/* Stop at dungeon edge */
		if (!in_bounds(y, x)) break;
	}
}

/*
 * Build a destroyed level
 */
static void destroy_level(void)
{
	int y1, x1, y, x, k, t, n;


	/* Note destroyed levels */
	if (cheat_room) msg_print("Destroyed Level");

	/* Drop a few epi-centers (usually about two) */
	for (n = 0; n < randint(5); n++)
	{
		/* Pick an epi-center */
		x1 = rand_range(5, DUNGEON_WID-1 - 5);
		y1 = rand_range(5, DUNGEON_HGT-1 - 5);

		/* Big area of affect */
		for (y = (y1 - 15); y <= (y1 + 15); y++)
		{
			for (x = (x1 - 15); x <= (x1 + 15); x++)
			{
				/* Skip illegal grids */
				if (!in_bounds_fully(y, x)) continue;

				/* Extract the distance */
				k = distance(y1, x1, y, x);

				/* Stay in the circle of death */
				if (k >= 16) continue;

				/* Delete the monster (if any) */
				delete_monster(y, x);

				/* Destroy valid grids */
				if (cave_valid_bold(y, x))
				{
					/* Delete objects */
					delete_object(y, x);

					/* Wall (or floor) type */
					t = rand_int(200);

					/* Granite */
					if (t < 20)
					{
						/* Create granite wall */
						cave_feat[y][x] = FEAT_WALL_EXTRA;
					}

					/* Quartz */
					else if (t < 70)
					{
						/* Create quartz vein */
						cave_feat[y][x] = FEAT_QUARTZ;
					}

					/* Magma */
					else if (t < 100)
					{
						/* Create magma vein */
						cave_feat[y][x] = FEAT_MAGMA;
					}

					/* Floor */
					else
					{
						/* Create floor */
						cave_feat[y][x] = FEAT_FLOOR;
					}

					/* No longer part of a room or vault */
					cave_info[y][x] &= ~(CAVE_ROOM | CAVE_ICKY);

					/* No longer illuminated or known */
					cave_info[y][x] &= ~(CAVE_MARK | CAVE_GLOW);
				}
			}
		}
	}
}



/*
 * Create up to "num" objects near the given coordinates
 * Only really called by some of the "special room" routines.
 */
static void vault_objects(int y, int x, int num)
{
	int dummy = 0;
	int i = 0, j = y, k = x;


	/* Attempt to place 'num' objects */
	for (; num > 0; --num)
	{
		/* Try up to 11 spots looking for empty space */
		for (i = 0; i < 11; ++i)
		{
			/* Pick a random location */
			while (dummy < SAFE_MAX_ATTEMPTS)
			{
				j = rand_spread(y, 2);
				k = rand_spread(x, 3);
				dummy++;
				if (!in_bounds(j, k)) continue;
				break;
			}


			if (dummy >= SAFE_MAX_ATTEMPTS)
			{
				if (cheat_room)
				{
					msg_print("Warning! Could not place vault object!");
				}
			}


			/* Require "clean" floor space */
			if (!cave_clean_bold(j, k)) continue;

			/* Place an item */
			if (rand_int(100) < 75)
			{
				place_object(j, k, FALSE, FALSE, FOUND_FLOOR, 0);
			}

			/* Place gold */
			else
			{
				place_gold(j, k, FOUND_FLOOR, 0);
			}

			/* Placement accomplished */
			break;
		}
	}
}


/*
 * Place a trap with a given displacement of point
 */
static void vault_trap_aux(int y, int x, int yd, int xd)
{
	int count = 0, y1 = y, x1 = x;
	int dummy = 0;

	/* Place traps */
	for (count = 0; count <= 5; count++)
	{
		/* Get a location */
		while (dummy < SAFE_MAX_ATTEMPTS)
		{
			y1 = rand_spread(y, yd);
			x1 = rand_spread(x, xd);
			dummy++;
			if (!in_bounds(y1, x1)) continue;
			break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place vault trap!");
			}
		}


		/* Require "naked" floor grids */
		if (!cave_naked_bold(y1, x1)) continue;

		/* Place the trap */
		place_trap(y1, x1);

		/* Done */
		break;
	}
}


/*
 * Place some traps with a given displacement of given location
 */
static void vault_traps(int y, int x, int yd, int xd, int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		vault_trap_aux(y, x, yd, xd);
	}
}


/*
 * Hack -- Place some sleeping monsters near the given location
 */
static void vault_monsters(int y1, int x1, int num)
{
	int y, x;

	/* Try to summon "num" monsters "near" the given location */
	while (num-- && scatter(&y, &x, y1, x1, 1, cave_empty_bold_p))
	{
		/* Place the monster at the chosen location (allow groups) */
		place_monster(y, x, dun_depth+2, TRUE, TRUE);
	}
}




/*
 * Room building routines.
 *
 * Six basic room types:
 *   1 -- normal
 *   2 -- overlapping
 *   3 -- cross shaped
 *   4 -- large room with features
 *   5 -- monster nests
 *   6 -- monster pits
 *   7 -- simple vaults
 *   8 -- greater vaults
 */


/*
 * Type 1 -- normal rectangular rooms
 */
static void build_type1(int yval, int xval)
{
	int y, x, y2, x2;
	int y1, x1;

	bool light;


	/* Choose lite or dark */
	light = (p_ptr->depth <= randint(25));


	/* Pick a room size */
	y1 = yval - randint(4);
	y2 = yval + randint(3);
	x1 = xval - randint(11);
	x2 = xval + randint(11);


	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
			if (light) cave_info[y][x] |= (CAVE_GLOW);
		}
	}

	/* Walls around the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2+1] = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2+1][x] = FEAT_WALL_OUTER;
	}


	/* Hack -- Occasional pillar room */
	if (rand_int(20) == 0)
	{
		for (y = y1; y <= y2; y += 2)
		{
			for (x = x1; x <= x2; x += 2)
			{
				cave_feat[y][x] = FEAT_WALL_INNER;
			}
		}
	}

	/* Hack -- Occasional ragged-edge room */
	else if (rand_int(50) == 0)
	{
		for (y = y1 + 2; y <= y2 - 2; y += 2)
		{
			cave_feat[y][x1] = FEAT_WALL_INNER;
			cave_feat[y][x2] = FEAT_WALL_INNER;
		}
		for (x = x1 + 2; x <= x2 - 2; x += 2)
		{
			cave_feat[y1][x] = FEAT_WALL_INNER;
			cave_feat[y2][x] = FEAT_WALL_INNER;
		}
	}
}


/*
 * Type 2 -- Overlapping rectangular rooms
 */
static void build_type2(int yval, int xval)
{
	int y, x;
	int y1a, x1a, y2a, x2a;
	int y1b, x1b, y2b, x2b;

	bool light;



	/* Choose lite or dark */
	light = (p_ptr->depth <= randint(25));


	/* Determine extents of the first room */
	y1a = yval - randint(4);
	y2a = yval + randint(3);
	x1a = xval - randint(11);
	x2a = xval + randint(10);

	/* Determine extents of the second room */
	y1b = yval - randint(3);
	y2b = yval + randint(4);
	x1b = xval - randint(10);
	x2b = xval + randint(11);


	/* Place a full floor for room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		for (x = x1a - 1; x <= x2a + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
			if (light) cave_info[y][x] |= (CAVE_GLOW);
		}
	}

	/* Place a full floor for room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		for (x = x1b - 1; x <= x2b + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
			if (light) cave_info[y][x] |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		cave_feat[y][x1a-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2a+1] = FEAT_WALL_OUTER;
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		cave_feat[y1a-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2a+1][x] = FEAT_WALL_OUTER;
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		cave_feat[y][x1b-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2b+1] = FEAT_WALL_OUTER;
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		cave_feat[y1b-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2b+1][x] = FEAT_WALL_OUTER;
	}



	/* Replace the floor for room "a" */
	for (y = y1a; y <= y2a; y++)
	{
		for (x = x1a; x <= x2a; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
		}
	}

	/* Replace the floor for room "b" */
	for (y = y1b; y <= y2b; y++)
	{
		for (x = x1b; x <= x2b; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
		}
	}
}



/*
 * Type 3 -- Cross shaped rooms
 *
 * Builds a room at a row, column coordinate
 *
 * Room "a" runs north/south, and Room "b" runs east/east
 * So the "central pillar" runs from x1a,y1b to x2a,y2b.
 *
 * Note that currently, the "center" is always 3x3, but I think that
 * the code below will work (with "bounds checking") for 5x5, or even
 * for unsymetric values like 4x3 or 5x3 or 3x4 or 3x5, or even larger.
 */
static void build_type3(int yval, int xval)
{
	int y, x, dy, dx, wy, wx;
	int y1a, x1a, y2a, x2a;
	int y1b, x1b, y2b, x2b;

	bool light;



	/* Choose lite or dark */
	light = (p_ptr->depth <= randint(25));


	/* For now, always 3x3 */
	wx = wy = 1;

	/* Pick max vertical size (at most 4) */
	dy = rand_range(3, 4);

	/* Pick max horizontal size (at most 15) */
	dx = rand_range(3, 11);


	/* Determine extents of the north/south room */
	y1a = yval - dy;
	y2a = yval + dy;
	x1a = xval - wx;
	x2a = xval + wx;

	/* Determine extents of the east/west room */
	y1b = yval - wy;
	y2b = yval + wy;
	x1b = xval - dx;
	x2b = xval + dx;


	/* Place a full floor for room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		for (x = x1a - 1; x <= x2a + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
			if (light) cave_info[y][x] |= (CAVE_GLOW);
		}
	}

	/* Place a full floor for room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		for (x = x1b - 1; x <= x2b + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
			if (light) cave_info[y][x] |= (CAVE_GLOW);
		}
	}


	/* Place the walls around room "a" */
	for (y = y1a - 1; y <= y2a + 1; y++)
	{
		cave_feat[y][x1a-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2a+1] = FEAT_WALL_OUTER;
	}
	for (x = x1a - 1; x <= x2a + 1; x++)
	{
		cave_feat[y1a-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2a+1][x] = FEAT_WALL_OUTER;
	}

	/* Place the walls around room "b" */
	for (y = y1b - 1; y <= y2b + 1; y++)
	{
		cave_feat[y][x1b-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2b+1] = FEAT_WALL_OUTER;
	}
	for (x = x1b - 1; x <= x2b + 1; x++)
	{
		cave_feat[y1b-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2b+1][x] = FEAT_WALL_OUTER;
	}


	/* Replace the floor for room "a" */
	for (y = y1a; y <= y2a; y++)
	{
		for (x = x1a; x <= x2a; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
		}
	}

	/* Replace the floor for room "b" */
	for (y = y1b; y <= y2b; y++)
	{
		for (x = x1b; x <= x2b; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
		}
	}



	/* Special features (3/4) */
	switch (rand_int(4))
	{
		/* Large solid middle pillar */
		case 1:
		{
			for (y = y1b; y <= y2b; y++)
			{
				for (x = x1a; x <= x2a; x++)
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
			}
			break;
		}

		/* Inner treasure vault */
		case 2:
		{
			/* Build the vault */
			for (y = y1b; y <= y2b; y++)
			{
				cave_feat[y][x1a] = FEAT_WALL_INNER;
				cave_feat[y][x2a] = FEAT_WALL_INNER;
			}
			for (x = x1a; x <= x2a; x++)
			{
				cave_feat[y1b][x] = FEAT_WALL_INNER;
				cave_feat[y2b][x] = FEAT_WALL_INNER;
			}

			/* Place a secret door on the inner room */
			switch (rand_int(4))
			{
				case 0: place_secret_door(y1b, xval); break;
				case 1: place_secret_door(y2b, xval); break;
				case 2: place_secret_door(yval, x1a); break;
				case 3: place_secret_door(yval, x2a); break;
			}

			/* Place a treasure in the vault */
			place_object(yval, xval, FALSE, FALSE, FOUND_FLOOR, 0);

			/* Let's guard the treasure well */
			vault_monsters(yval, xval, rand_int(2) + 3);

			/* Traps naturally */
			vault_traps(yval, xval, 4, 4, rand_int(3) + 2);

			break;
		}

		/* Something else */
		case 3:
		{
			/* Occasionally pinch the center shut */
			if (rand_int(3) == 0)
			{
				/* Pinch the east/west sides */
				for (y = y1b; y <= y2b; y++)
				{
					if (y == yval) continue;
					cave_feat[y][x1a - 1] = FEAT_WALL_INNER;
					cave_feat[y][x2a + 1] = FEAT_WALL_INNER;
				}

				/* Pinch the north/south sides */
				for (x = x1a; x <= x2a; x++)
				{
					if (x == xval) continue;
					cave_feat[y1b - 1][x] = FEAT_WALL_INNER;
					cave_feat[y2b + 1][x] = FEAT_WALL_INNER;
				}

				/* Sometimes shut using secret doors */
				if (rand_int(3) == 0)
				{
					place_secret_door(yval, x1a - 1);
					place_secret_door(yval, x2a + 1);
					place_secret_door(y1b - 1, xval);
					place_secret_door(y2b + 1, xval);
				}
			}

			/* Occasionally put a "plus" in the center */
			else if (rand_int(3) == 0)
			{
				cave_feat[yval][xval] = FEAT_WALL_INNER;
				cave_feat[y1b][xval] = FEAT_WALL_INNER;
				cave_feat[y2b][xval] = FEAT_WALL_INNER;
				cave_feat[yval][x1a] = FEAT_WALL_INNER;
				cave_feat[yval][x2a] = FEAT_WALL_INNER;
			}

			/* Occasionally put a pillar in the center */
			else if (rand_int(3) == 0)
			{
				cave_feat[yval][xval] = FEAT_WALL_INNER;
			}

			break;
		}
	}
}


/*
 * Type 4 -- Large room with inner features
 *
 * Possible sub-types:
 *	1 - Just an inner room with one door
 *	2 - An inner room within an inner room
 *	3 - An inner room with pillar(s)
 *	4 - Inner room has a maze
 *	5 - A set of four inner rooms
 */
static void build_type4(int yval, int xval)
{
	int y, x, y1, x1;
	int y2, x2, tmp;

	bool light;



	/* Choose lite or dark */
	light = (p_ptr->depth <= randint(25));


	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;


	/* Place a full floor under the room */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
			if (light) cave_info[y][x] |= (CAVE_GLOW);
		}
	}

	/* Outer Walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2+1] = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2+1][x] = FEAT_WALL_OUTER;
	}


	/* The inner room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_INNER;
		cave_feat[y][x2+1] = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_INNER;
		cave_feat[y2+1][x] = FEAT_WALL_INNER;
	}


	/* Inner room variations */
	switch (randint(5))
	{
		/* Just an inner room with a monster */
		case 1:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Place a monster in the room */
		vault_monsters(yval, xval, 1);

		break;


		/* Treasure Vault (with a door) */
		case 2:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Place another inner room */
		for (y = yval - 1; y <= yval + 1; y++)
		{
			for (x = xval -  1; x <= xval + 1; x++)
			{
				if ((x == xval) && (y == yval)) continue;
				cave_feat[y][x] = FEAT_WALL_INNER;
			}
		}

		/* Place a locked door on the inner room */
		switch (randint(4))
		{
			case 1: place_locked_door(yval - 1, xval); break;
			case 2: place_locked_door(yval + 1, xval); break;
			case 3: place_locked_door(yval, xval - 1); break;
			case 4: place_locked_door(yval, xval + 1); break;
		}

		/* Monsters to guard the "treasure" */
		vault_monsters(yval, xval, randint(3) + 2);

		/* Object (80%) */
		if (rand_int(100) < 80)
		{
			place_object(yval, xval, FALSE, FALSE, FOUND_FLOOR, 0);
		}

		/* Stairs (20%) */
		else
		{
			place_random_stairs(yval, xval);
		}

		/* Traps to protect the treasure */
		vault_traps(yval, xval, 4, 10, 2 + randint(3));

		break;


		/* Inner pillar(s). */
		case 3:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Large Inner Pillar */
		for (y = yval - 1; y <= yval + 1; y++)
		{
			for (x = xval - 1; x <= xval + 1; x++)
			{
				cave_feat[y][x] = FEAT_WALL_INNER;
			}
		}

		/* Occasionally, two more Large Inner Pillars */
		if (rand_int(2) == 0)
		{
			tmp = randint(2);
			for (y = yval - 1; y <= yval + 1; y++)
			{
				for (x = xval - 5 - tmp; x <= xval - 3 - tmp; x++)
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
				for (x = xval + 3 + tmp; x <= xval + 5 + tmp; x++)
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
			}
		}

		/* Occasionally, some Inner rooms */
		if (rand_int(3) == 0)
		{
			/* Long horizontal walls */
			for (x = xval - 5; x <= xval + 5; x++)
			{
				cave_feat[yval-1][x] = FEAT_WALL_INNER;
				cave_feat[yval+1][x] = FEAT_WALL_INNER;
			}

			/* Close off the left/right edges */
			cave_feat[yval][xval-5] = FEAT_WALL_INNER;
			cave_feat[yval][xval+5] = FEAT_WALL_INNER;

			/* Secret doors (random top/bottom) */
			place_secret_door(yval - 3 + (randint(2) * 2), xval - 3);
			place_secret_door(yval - 3 + (randint(2) * 2), xval + 3);

			/* Monsters */
			vault_monsters(yval, xval - 2, randint(2));
			vault_monsters(yval, xval + 2, randint(2));

			/* Objects */
			if (one_in(3))
				place_object(yval, xval - 2, FALSE, FALSE, FOUND_FLOOR, 0);
			if (one_in(3))
				place_object(yval, xval + 2, FALSE, FALSE, FOUND_FLOOR, 0);
		}

		break;


		/* Maze inside. */
		case 4:

		/* Place a secret door */
		switch (randint(4))
		{
			case 1: place_secret_door(y1 - 1, xval); break;
			case 2: place_secret_door(y2 + 1, xval); break;
			case 3: place_secret_door(yval, x1 - 1); break;
			case 4: place_secret_door(yval, x2 + 1); break;
		}

		/* Maze (really a checkerboard) */
		for (y = y1; y <= y2; y++)
		{
			for (x = x1; x <= x2; x++)
			{
				if (0x1 & (x + y))
				{
					cave_feat[y][x] = FEAT_WALL_INNER;
				}
			}
		}

		/* Monsters just love mazes. */
		vault_monsters(yval, xval - 5, randint(3));
		vault_monsters(yval, xval + 5, randint(3));

		/* Traps make them entertaining. */
		vault_traps(yval, xval - 3, 2, 8, randint(3));
		vault_traps(yval, xval + 3, 2, 8, randint(3));

		/* Mazes should have some treasure too. */
		vault_objects(yval, xval, 3);

		break;


		/* Four small rooms. */
		case 5:

		/* Inner "cross" */
		for (y = y1; y <= y2; y++)
		{
			cave_feat[y][xval] = FEAT_WALL_INNER;
		}
		for (x = x1; x <= x2; x++)
		{
			cave_feat[yval][x] = FEAT_WALL_INNER;
		}

		/* Doors into the rooms */
		if (rand_int(100) < 50)
		{
			int i = randint(10);
			place_secret_door(y1 - 1, xval - i);
			place_secret_door(y1 - 1, xval + i);
			place_secret_door(y2 + 1, xval - i);
			place_secret_door(y2 + 1, xval + i);
		}
		else
		{
			int i = randint(3);
			place_secret_door(yval + i, x1 - 1);
			place_secret_door(yval - i, x1 - 1);
			place_secret_door(yval + i, x2 + 1);
			place_secret_door(yval - i, x2 + 1);
		}

		/* Treasure, centered at the center of the cross */
		vault_objects(yval, xval, 2 + randint(2));

		/* Gotta have some monsters. */
		vault_monsters(yval + 1, xval - 4, randint(4));
		vault_monsters(yval + 1, xval + 4, randint(4));
		vault_monsters(yval - 1, xval - 4, randint(4));
		vault_monsters(yval - 1, xval + 4, randint(4));

		break;
	}
}


/*
 * The following functions are used to determine if the given monster
 * is appropriate for inclusion in a monster nest or monster pit or
 * the given type.
 *
 * None of the pits/nests are allowed to include "unique" monsters,
 * or monsters which can "multiply".
 *
 * Some of the pits/nests are asked to avoid monsters which can blink
 * away or which are invisible.  This is probably a hack.
 *
 * The old method made direct use of monster "names", which is bad.
 *
 * Note the use of Angband 2.7.9 monster race pictures in various places.
 */


/*
 * Helper function for "monster nest (jelly)"
 */
static bool vault_aux_jelly(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Also decline evil jellies (like death moulds and shoggoths) */
	if (r_ptr->flags3 & (RF3_EVIL)) return (FALSE);

	/* Require icky thing, jelly, mould, or mushroom */
	if (!strchr("ijm,", r_ptr->gfx.dc)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (animal)"
 */
static bool vault_aux_animal(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "animal" flag */
	if (!(r_ptr->flags3 & (RF3_ANIMAL))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (undead)"
 */
static bool vault_aux_undead(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require Undead */
	if (!(r_ptr->flags3 & (RF3_UNDEAD))) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster nest (chapel)"
 */
static bool vault_aux_chapel(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "shaman"*/
	return !!(r_ptr->flags2 & RF2_SHAMAN);
}

/*
 * Helper function for "monster nest (cultist lair)"
 */
static bool vault_aux_cult(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require cultists. */
	return !!(r_ptr->flags2 & RF2_CULTIST);

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (kennel)"
 */
static bool vault_aux_kennel(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require a Zephyr Hound or a dog */
	return ((r_ptr->gfx.dc == 'Z') || (r_ptr->gfx.dc == 'C'));

}

/*
 * Helper function for "monster nest (treasure)"
 */
static bool vault_aux_treasure(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Require "Treasure" */
	if (!((r_ptr->gfx.dc == '!') || (r_ptr->gfx.dc == '|') ||
		(r_ptr->gfx.dc == '$') || (r_ptr->gfx.dc == '?') ||
		(r_ptr->gfx.dc == '=')))
	{
		return (FALSE);
	}

	/* Okay */
	return (TRUE);
}

/*
 * Helper function for "monster nest (clone)"
 */
static bool vault_aux_clone(int template_race, int r_idx)
{
	return (r_idx == template_race);
}


/*
 * Helper function for "monster nest (symbol clone)"
 */
static bool vault_aux_symbol(int template_race, int r_idx)
{
	return ((r_info[r_idx].gfx.dc == (r_info[template_race].gfx.dc))
		&& !(r_info[r_idx].flags1 & RF1_UNIQUE));
}


/*
 * Helper function for "monster pit (orc)"
 */
static bool vault_aux_orc(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Hack -- Require "o" monsters */
	if (!strchr("o", r_ptr->gfx.dc)) return (FALSE);

	/* Okay */
	return (TRUE);
}



/*
 * Helper function for "monster pit (troll)"
 */
static bool vault_aux_troll(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Hack -- Require "T" monsters */
	if (!strchr("T", r_ptr->gfx.dc)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (giant)"
 */
static bool vault_aux_giant(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Hack -- Require "P" monsters */
	if (!strchr("P", r_ptr->gfx.dc)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Hack -- breath type for "vault_aux_dragon()"
 */
static u32b vault_aux_dragon_mask4;


/*
 * Helper function for "monster pit (dragon)"
 */
static bool vault_aux_dragon(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Hack -- Require "d" or "D" monsters */
	if (!strchr("Dd", r_ptr->gfx.dc)) return (FALSE);

	/* Hack -- Require correct "breath attack" */
	if (r_ptr->flags4 != vault_aux_dragon_mask4) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Helper function for "monster pit (demon)"
 */
static bool vault_aux_demon(int UNUSED p, int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Decline unique monsters */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Hack -- Require "U" monsters */
	if (!strchr("U", r_ptr->gfx.dc)) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Type 5 -- Monster nests
 *
 * A monster nest is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type strewn about the room.
 *
 * The monsters are chosen from a set of 64 randomly selected monster
 * races, to allow the nest creation to fail instead of having "holes".
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the nest.
 *
 * Currently, a monster nest is one of
 *   a nest of "jelly" monsters   (Dungeon level 5 and deeper)
 *   a nest of "animal" monsters  (Dungeon level 30 and deeper)
 *   a nest of "undead" monsters  (Dungeon level 50 and deeper)
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which
 * case the nest will be empty, and will not affect the level rating.
 *
 * Note that "monster nests" will never contain "unique" monsters.
 */
static void build_type5(int yval, int xval)
{
	int y, x, y1, x1, y2, x2;

	int tmp, i, p=0;

	s16b what[64];

	cptr name;

	bool empty = FALSE;

	bool (*get_mon_num_hook)(int, int);


	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;


	/* Place the floor area */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
		}
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2+1] = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2+1][x] = FEAT_WALL_OUTER;
	}


	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_INNER;
		cave_feat[y][x2+1] = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_INNER;
		cave_feat[y2+1][x] = FEAT_WALL_INNER;
	}


	/* Place a secret door */
	switch (randint(4))
	{
		case 1: place_secret_door(y1 - 1, xval); break;
		case 2: place_secret_door(y2 + 1, xval); break;
		case 3: place_secret_door(yval, x1 - 1); break;
		case 4: place_secret_door(yval, x2 + 1); break;
	}


	/* Hack -- Choose a nest type */
	tmp = randint(p_ptr->depth);

	/* Monster nest (jelly) */
	if (tmp < 30)
	{
		/* Describe */
		name = "jelly";

		/* Restrict to jelly */
		get_mon_num_hook = vault_aux_jelly;
	}

	/* Monster nest (animal) */
	else if (tmp < 50)
	{
		/* Describe */
		name = "animal";

		/* Restrict to animal */
		get_mon_num_hook = vault_aux_animal;
	}

	/* Monster nest (undead) */
	else
	{
		/* Describe */
		name = "undead";

		/* Restrict to undead */
		get_mon_num_hook = vault_aux_undead;
	}

	/* Prepare special allocation table */
	get_mon_num_prep(get_mon_num_hook, p);


	/* Pick some monster types */
	for (i = 0; i < 64; i++)
	{
		/* Get a (hard) monster type */
		what[i] = get_mon_num(p_ptr->depth + 10);

		/* Notice failure */
		if (!what[i]) empty = TRUE;
	}


	/* Prepare normal allocation table */
	get_mon_num_prep(NULL, 0);


	/* Oops */
	if (empty) return;


	/* Describe */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster nest (%s)", name);
	}


	/* Increase the level rating */
	rating += 10;

	/* (Sometimes) Cause a "special feeling" (for "Monster Nests") */
	if ((p_ptr->depth <= 40) && (randint(p_ptr->depth*p_ptr->depth + 1) < 300))
	{
		good_item_flag = TRUE;
	}


	/* Place some monsters */
	for (y = yval - 2; y <= yval + 2; y++)
	{
		for (x = xval - 9; x <= xval + 9; x++)
		{
			int r_idx = what[rand_int(64)];

			/* Place that "random" monster (no groups) */
			(void)place_monster_aux(y, x, r_idx, FALSE, FALSE, FALSE, FALSE);
		}
	}
}



/*
 * Type 6 -- Monster pits
 *
 * A monster pit is a "big" room, with an "inner" room, containing
 * a "collection" of monsters of a given type organized in the room.
 *
 * Monster types in the pit
 *   orc pit	(Dungeon Level 5 and deeper)
 *   troll pit	(Dungeon Level 20 and deeper)
 *   giant pit	(Dungeon Level 40 and deeper)
 *   dragon pit	(Dungeon Level 60 and deeper)
 *   demon pit	(Dungeon Level 80 and deeper)
 *
 * The inside room in a monster pit appears as shown below, where the
 * actual monsters in each location depend on the type of the pit
 *
 *   #####################
 *   #0000000000000000000#
 *   #0112233455543322110#
 *   #0112233467643322110#
 *   #0112233455543322110#
 *   #0000000000000000000#
 *   #####################
 *
 * Note that the monsters in the pit are now chosen by using "get_mon_num()"
 * to request 16 "appropriate" monsters, sorting them by level, and using
 * the "even" entries in this sorted list for the contents of the pit.
 *
 * Hack -- all of the "dragons" in a "dragon" pit must be the same "color",
 * which is handled by requiring a specific "breath" attack for all of the
 * dragons.  This may include "multi-hued" breath.  Note that "wyrms" may
 * be present in many of the dragon pits, if they have the proper breath.
 *
 * Note the use of the "get_mon_num_prep()" function, and the special
 * "get_mon_num_hook()" restriction function, to prepare the "monster
 * allocation table" in such a way as to optimize the selection of
 * "appropriate" non-unique monsters for the pit.
 *
 * Note that the "get_mon_num()" function may (rarely) fail, in which case
 * the pit will be empty, and will not effect the level rating.
 *
 * Note that "monster pits" will never contain "unique" monsters.
 */
static void build_type6(int yval, int xval)
{
	int tmp, what[16];

	int i, j, y, x, y1, x1, y2, x2, p=0;

	bool (*get_mon_num_hook)(int, int);

	bool empty = FALSE;

	cptr name;


	/* Large room */
	y1 = yval - 4;
	y2 = yval + 4;
	x1 = xval - 11;
	x2 = xval + 11;


	/* Place the floor area */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		for (x = x1 - 1; x <= x2 + 1; x++)
		{
			cave_feat[y][x] = FEAT_FLOOR;
			cave_info[y][x] |= (CAVE_ROOM);
		}
	}

	/* Place the outer walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_OUTER;
		cave_feat[y][x2+1] = FEAT_WALL_OUTER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_OUTER;
		cave_feat[y2+1][x] = FEAT_WALL_OUTER;
	}


	/* Advance to the center room */
	y1 = y1 + 2;
	y2 = y2 - 2;
	x1 = x1 + 2;
	x2 = x2 - 2;

	/* The inner walls */
	for (y = y1 - 1; y <= y2 + 1; y++)
	{
		cave_feat[y][x1-1] = FEAT_WALL_INNER;
		cave_feat[y][x2+1] = FEAT_WALL_INNER;
	}
	for (x = x1 - 1; x <= x2 + 1; x++)
	{
		cave_feat[y1-1][x] = FEAT_WALL_INNER;
		cave_feat[y2+1][x] = FEAT_WALL_INNER;
	}


	/* Place a secret door */
	switch (randint(4))
	{
		case 1: place_secret_door(y1 - 1, xval); break;
		case 2: place_secret_door(y2 + 1, xval); break;
		case 3: place_secret_door(yval, x1 - 1); break;
		case 4: place_secret_door(yval, x2 + 1); break;
	}


	/* Choose a pit type */
	tmp = randint(p_ptr->depth);

	/* Orc pit */
	if (tmp < 20)
	{
		/* Message */
		name = "orc";

		/* Restrict monster selection */
		get_mon_num_hook = vault_aux_orc;
	}

	/* Troll pit */
	else if (tmp < 40)
	{
		/* Message */
		name = "troll";

		/* Restrict monster selection */
		get_mon_num_hook = vault_aux_troll;
	}

	/* Giant pit */
	else if (tmp < 60)
	{
		/* Message */
		name = "giant";

		/* Restrict monster selection */
		get_mon_num_hook = vault_aux_giant;
	}

	/* Dragon pit */
	else if (tmp < 80)
	{
		/* Pick dragon type */
		switch (rand_int(6))
		{
			/* Black */
			case 0:
			{
				/* Message */
				name = "acid dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_ACID;

				/* Done */
				break;
			}

			/* Blue */
			case 1:
			{
				/* Message */
				name = "electric dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_ELEC;

				/* Done */
				break;
			}

			/* Red */
			case 2:
			{
				/* Message */
				name = "fire dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_FIRE;

				/* Done */
				break;
			}

			/* White */
			case 3:
			{
				/* Message */
				name = "cold dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_COLD;

				/* Done */
				break;
			}

			/* Green */
			case 4:
			{
				/* Message */
				name = "poison dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = RF4_BR_POIS;

				/* Done */
				break;
			}

			/* Multi-hued */
			default:
			{
				/* Message */
				name = "multi-hued dragon";

				/* Restrict dragon breath type */
				vault_aux_dragon_mask4 = (RF4_BR_ACID | RF4_BR_ELEC |
				                          RF4_BR_FIRE | RF4_BR_COLD |
				                          RF4_BR_POIS);

				/* Done */
				break;
			}

		}

		/* Restrict monster selection */
		get_mon_num_hook = vault_aux_dragon;
	}

	/* Demon pit */
	else
	{
		/* Message */
		name = "demon";

		/* Restrict monster selection */
		get_mon_num_hook = vault_aux_demon;
	}

	/* Prepare special allocation table */
	get_mon_num_prep(get_mon_num_hook, p);


	/* Pick some monster types */
	for (i = 0; i < 16; i++)
	{
		/* Get a (hard) monster type */
		what[i] = get_mon_num(p_ptr->depth + 10);

		/* Notice failure */
		if (!what[i]) empty = TRUE;
	}


	/* Remove restriction */
	get_mon_num_hook = NULL;

	/* Prepare normal allocation table */
	get_mon_num_prep(NULL, 0);

	/* Oops */
	if (empty) return;


	/* XXX XXX XXX */
	/* Sort the entries */
	for (i = 0; i < 16 - 1; i++)
	{
		/* Sort the entries */
		for (j = 0; j < 16 - 1; j++)
		{
			int i1 = j;
			int i2 = j + 1;

			int p1 = r_info[what[i1]].level;
			int p2 = r_info[what[i2]].level;

			/* Bubble */
			if (p1 > p2)
			{
				int tmp = what[i1];
				what[i1] = what[i2];
				what[i2] = tmp;
			}
		}
	}

	/* Select the entries */
	for (i = 0; i < 8; i++)
	{
		/* Every other entry */
		what[i] = what[i * 2];
	}


	/* Message */
	if (cheat_room)
	{
		/* Room type */
		msg_format("Monster pit (%s)", name);

		/* Contents */
		for (i = 0; i < 8; i++)
		{
			/* Message */
			msg_print(r_name + r_info[what[i]].name);
		}
	}


	/* Increase the level rating */
	rating += 10;

	/* (Sometimes) Cause a "special feeling" (for "Monster Pits") */
	if ((p_ptr->depth <= 40) && (randint(p_ptr->depth*p_ptr->depth + 1) < 300))
	{
		good_item_flag = TRUE;
	}


	/* Top and bottom rows */
	for (x = xval - 9; x <= xval + 9; x++)
	{
		place_monster_aux(yval - 2, x, what[0], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(yval + 2, x, what[0], FALSE, FALSE, FALSE, FALSE);
	}

	/* Middle columns */
	for (y = yval - 1; y <= yval + 1; y++)
	{
		place_monster_aux(y, xval - 9, what[0], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 9, what[0], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 8, what[1], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 8, what[1], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 7, what[1], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 7, what[1], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 6, what[2], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 6, what[2], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 5, what[2], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 5, what[2], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 4, what[3], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 4, what[3], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 3, what[3], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 3, what[3], FALSE, FALSE, FALSE, FALSE);

		place_monster_aux(y, xval - 2, what[4], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(y, xval + 2, what[4], FALSE, FALSE, FALSE, FALSE);
	}

	/* Above/Below the center monster */
	for (x = xval - 1; x <= xval + 1; x++)
	{
		place_monster_aux(yval + 1, x, what[5], FALSE, FALSE, FALSE, FALSE);
		place_monster_aux(yval - 1, x, what[5], FALSE, FALSE, FALSE, FALSE);
	}

	/* Next to the center monster */
	place_monster_aux(yval, xval + 1, what[6], FALSE, FALSE, FALSE, FALSE);
	place_monster_aux(yval, xval - 1, what[6], FALSE, FALSE, FALSE, FALSE);

	/* Center monster */
	place_monster_aux(yval, xval, what[7], FALSE, FALSE, FALSE, FALSE);
}

/*
 * Place an object at a level deeper than its current one (for vaults).
 */
static void place_object_gen(int y, int x, bool good, bool great, int how,
	int idx, int level)
{
	object_level += level;
	place_object(y, x, good, great, how, idx);
	object_level -= level;
}

/*
 * Hack -- fill in "vault" rooms
 */
static void build_vault(int yval, int xval, int vault)
{
	const int ymax = v_info[vault].hgt;
	const int xmax = v_info[vault].wid;
	cptr const data = v_text + v_info[vault].text;

	int dx, dy, x, y;

	cptr t;

	/* Place dungeon features and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* Extract the location */
			x = xval - (xmax / 2) + dx;
			y = yval - (ymax / 2) + dy;

			/* Hack -- skip "non-grids" */
			if (*t == ' ') continue;

			/* Lay down a floor */
			cave_feat[y][x] = FEAT_FLOOR;

			/* Part of a vault */
			cave_info[y][x] |= (CAVE_ROOM | CAVE_ICKY);

			/* Analyze the grid */
			switch (*t)
			{
				/* Granite wall (outer) */
				case '%':
				cave_feat[y][x] = FEAT_WALL_OUTER;
				break;

				/* Granite wall (inner) */
				case '#':
				cave_feat[y][x] = FEAT_WALL_INNER;
				break;

				/* Permanent wall (inner) */
				case 'X':
				cave_feat[y][x] = FEAT_PERM_INNER;
				break;

				/* Treasure/trap */
				case '*':
				if (rand_int(100) < 75)
				{
					place_object(y, x, FALSE, FALSE, FOUND_VAULT, vault);
				}
				else
				{
					place_trap(y, x);
				}
				break;

				/* Secret doors */
				case '+':
				place_secret_door(y, x);
				break;

				/* Trap */
				case '^':
				place_trap(y, x);
				break;
			}
		}
	}


	/* Place dungeon monsters and objects */
	for (t = data, dy = 0; dy < ymax; dy++)
	{
		for (dx = 0; dx < xmax; dx++, t++)
		{
			/* Extract the grid */
			x = xval - (xmax/2) + dx;
			y = yval - (ymax/2) + dy;

			/* Hack -- skip "non-grids" */
			if (*t == ' ') continue;

			/* Analyze the symbol */
			switch (*t)
			{
				/* Monster */
				case '&':
				{
					place_monster(y, x, dun_depth+5, TRUE, TRUE);
					break;
				}

				/* Meaner monster */
				case '@':
				{
					place_monster(y, x, dun_depth+11, TRUE, TRUE);
					break;
				}

				/* Meaner monster, plus treasure */
				case '9':
				{
					place_monster(y, x, dun_depth+9, TRUE, TRUE);
					place_object_gen(y, x, TRUE, FALSE, FOUND_VAULT, vault, 7);
					break;
				}

				/* Nasty monster and treasure */
				case '8':
				{
					place_monster(y, x, dun_depth+40, TRUE, TRUE);
					place_object_gen(y, x, TRUE, TRUE, FOUND_VAULT, vault, 20);
					break;
				}

				/* Monster and/or object */
				case ',':
				{
					if (rand_int(100) < 50)
					{
						place_monster(y, x, dun_depth+3, TRUE, TRUE);
					}
					if (rand_int(100) < 50)
					{
						place_object_gen(y, x, FALSE, FALSE, FOUND_VAULT, vault, 7);
					}
					break;
				}

				case 'p':
				cave_set_feat(y, x, FEAT_PATTERN_START);
				break;

				case 'a':
				cave_set_feat(y, x, FEAT_PATTERN_1);
				break;

				case 'b':
				cave_set_feat(y, x, FEAT_PATTERN_2);
				break;

				case 'c':
				cave_set_feat(y, x, FEAT_PATTERN_3);
				break;

				case 'd':
				cave_set_feat(y, x, FEAT_PATTERN_4);
				break;

				case 'P':
				cave_set_feat(y, x, FEAT_PATTERN_END);
				break;

				case 'B':
				cave_set_feat(y, x, FEAT_PATTERN_XTRA1);
				break;

				case 'A':
				{
					place_object_gen(y, x, TRUE, FALSE, FOUND_VAULT, vault, 12);
				}
				break;

			}
		}
	}
}


static cptr vault_name(vault_type *v_ptr)
{
	return v_name+v_ptr->name;
}

static vault_type *pick_vault(int typ)
{
	int i, n;
	for (i = n = 0; i < z_info->v_max; i++)
	{
		if (v_info[i].typ == typ) n++;
	}

	n = rand_int(n);

	for (i = 0; i < z_info->v_max; i++)
	{
		if (v_info[i].typ == typ && !n--) return v_info+i;
	}

	/* No suitable vaults... */
	if (cheat_room)
	{
		msg_format("Warning! Could not place '%d' vault!", typ);
	}
	return 0;
}

static void build_type7_or_8(int yval, int xval, int typ)
{
#ifdef FORCE_V_IDX
	vault_type *v_ptr = ((typ == 7) ? &v_info[2] : &v_info[rand_range(77,79)]);
#else
	vault_type *v_ptr = pick_vault(typ);
#endif
	cptr type = (typ == 7) ? "Lesser" : "Greater";

	/* No vault chosen. */
	if (!v_ptr) return;

	/* Hack - the game should have already checked this... */
	while (!in_bounds(yval - v_ptr->hgt/2, xval - v_ptr->wid/2) ||
		!in_bounds(yval + (v_ptr->hgt+1)/2, xval + (v_ptr->wid+1)/2))
	{
		v_ptr = pick_vault(typ);
	}

	/* Message */
	if (cheat_room) msg_format("%s Vault %s", type, vault_name(v_ptr));

	/* Boost the rating */
	rating += v_ptr->rat;

	/* (Sometimes) Cause a special feeling */
	if (((dun_depth) <= 50) ||
		(randint(((dun_depth)-40) * ((dun_depth)-40) + 50) < 400))
	{
		good_item_flag = TRUE;
	}

	/* Hack -- Build the vault */
	build_vault(yval, xval, v_ptr - v_info);
}


/*
 * Constructs a tunnel between two points
 *
 * This function must be called BEFORE any streamers are created,
 * since we use the special "granite wall" sub-types to keep track
 * of legal places for corridors to pierce rooms.
 *
 * We use "door_flag" to prevent excessive construction of doors
 * along overlapping corridors.
 *
 * We queue the tunnel grids to prevent door creation along a corridor
 * which intersects itself.
 *
 * We queue the wall piercing grids to prevent a corridor from leaving
 * a room and then coming back in through the same entrance.
 *
 * We "pierce" grids which are "outer" walls of rooms, and when we
 * do so, we change all adjacent "outer" walls of rooms into "solid"
 * walls so that no two corridors may use adjacent grids for exits.
 *
 * The "solid" wall check prevents corridors from "chopping" the
 * corners of rooms off, as well as "silly" door placement, and
 * "excessively wide" room entrances.
 *
 * Useful "feat" values:
 *   FEAT_WALL_EXTRA -- granite walls
 *   FEAT_WALL_INNER -- inner room walls
 *   FEAT_WALL_OUTER -- outer room walls
 *   FEAT_WALL_SOLID -- solid room walls
 *   FEAT_PERM_BUILDING -- shop walls (perma)
 *   FEAT_PERM_INNER -- inner room walls (perma)
 *   FEAT_PERM_OUTER -- outer room walls (perma)
 *   FEAT_PERM_SOLID -- dungeon border (perma)
 */
static void build_tunnel(int row1, int col1, int row2, int col2)
{
	int i, y, x;
	int tmp_row, tmp_col;
	int row_dir, col_dir;
	int start_row, start_col;
	int main_loop_count = 0;

	bool door_flag = FALSE;



	/* Reset the arrays */
	dun->tunn_n = 0;
	dun->wall_n = 0;

	/* Save the starting location */
	start_row = row1;
	start_col = col1;

	/* Start out in the correct direction */
	correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

	/* Keep going until done (or bored) */
	while ((row1 != row2) || (col1 != col2))
	{
		/* Mega-Hack -- Paranoia -- prevent infinite loops */
		if (main_loop_count++ > 2000) break;

		/* Allow bends in the tunnel */
		if (rand_int(100) < DUN_TUN_CHG)
		{
			/* Acquire the correct direction */
			correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

			/* Random direction */
			if (rand_int(100) < DUN_TUN_RND)
			{
				rand_dir(&row_dir, &col_dir);
			}
		}

		/* Get the next location */
		tmp_row = row1 + row_dir;
		tmp_col = col1 + col_dir;


		/* Do not leave the dungeon!!! XXX XXX */
		while (!in_bounds_fully(tmp_row, tmp_col))
		{
			/* Acquire the correct direction */
			correct_dir(&row_dir, &col_dir, row1, col1, row2, col2);

			/* Random direction */
			if (rand_int(100) < DUN_TUN_RND)
			{
				rand_dir(&row_dir, &col_dir);
			}

			/* Get the next location */
			tmp_row = row1 + row_dir;
			tmp_col = col1 + col_dir;
		}


		/* Avoid the edge of the dungeon */
		if (cave_feat[tmp_row][tmp_col] == FEAT_PERM_SOLID) continue;

		/* Avoid the edge of vaults */
		if (cave_feat[tmp_row][tmp_col] == FEAT_PERM_OUTER) continue;

		/* Avoid "solid" granite walls */
		if (cave_feat[tmp_row][tmp_col] == FEAT_WALL_SOLID) continue;

		/* Pierce "outer" walls of rooms */
		if (cave_feat[tmp_row][tmp_col] == FEAT_WALL_OUTER)
		{
			/* Acquire the "next" location */
			y = tmp_row + row_dir;
			x = tmp_col + col_dir;

			/* Hack -- Avoid outer/solid permanent walls */
			if (cave_feat[y][x] == FEAT_PERM_SOLID) continue;
			if (cave_feat[y][x] == FEAT_PERM_OUTER) continue;

			/* Hack -- Avoid outer/solid granite walls */
			if (cave_feat[y][x] == FEAT_WALL_OUTER) continue;
			if (cave_feat[y][x] == FEAT_WALL_SOLID) continue;

			/* Accept this location */
			row1 = tmp_row;
			col1 = tmp_col;

			/* Save the wall location */
			if (dun->wall_n < WALL_MAX)
			{
				dun->wall[dun->wall_n].y = row1;
				dun->wall[dun->wall_n].x = col1;
				dun->wall_n++;
			}

			/* Forbid re-entry near this piercing */
			for (y = row1 - 1; y <= row1 + 1; y++)
			{
				for (x = col1 - 1; x <= col1 + 1; x++)
				{
					/* Convert adjacent "outer" walls as "solid" walls */
					if (cave_feat[y][x] == FEAT_WALL_OUTER)
					{
						/* Change the wall to a "solid" wall */
						cave_feat[y][x] = FEAT_WALL_SOLID;
					}
				}
			}
		}

		/* Travel quickly through rooms */
		else if (cave_info[tmp_row][tmp_col] & (CAVE_ROOM))
		{
			/* Accept the location */
			row1 = tmp_row;
			col1 = tmp_col;
		}

		/* Tunnel through all other walls */
		else if (cave_feat[tmp_row][tmp_col] >= FEAT_WALL_EXTRA)
		{
			/* Accept this location */
			row1 = tmp_row;
			col1 = tmp_col;

			/* Save the tunnel location */
			if (dun->tunn_n < TUNN_MAX)
			{
				dun->tunn[dun->tunn_n].y = row1;
				dun->tunn[dun->tunn_n].x = col1;
				dun->tunn_n++;
			}

			/* Allow door in next grid */
			door_flag = FALSE;
		}

		/* Handle corridor intersections or overlaps */
		else
		{
			/* Accept the location */
			row1 = tmp_row;
			col1 = tmp_col;

			/* Collect legal door locations */
			if (!door_flag)
			{
				/* Save the door location */
				if (dun->door_n < DOOR_MAX)
				{
					dun->door[dun->door_n].y = row1;
					dun->door[dun->door_n].x = col1;
					dun->door_n++;
				}

				/* No door in next grid */
				door_flag = TRUE;
			}

			/* Hack -- allow pre-emptive tunnel termination */
			if (rand_int(100) >= DUN_TUN_CON)
			{
				/* Distance between row1 and start_row */
				tmp_row = row1 - start_row;
				if (tmp_row < 0) tmp_row = (-tmp_row);

				/* Distance between col1 and start_col */
				tmp_col = col1 - start_col;
				if (tmp_col < 0) tmp_col = (-tmp_col);

				/* Terminate the tunnel */
				if ((tmp_row > 10) || (tmp_col > 10)) break;
			}
		}
	}


	/* Turn the tunnel into corridor */
	for (i = 0; i < dun->tunn_n; i++)
	{
		/* Access the grid */
		y = dun->tunn[i].y;
		x = dun->tunn[i].x;

		/* Clear previous contents, add a floor */
		cave_feat[y][x] = FEAT_FLOOR;
	}


	/* Apply the piercings that we found */
	for (i = 0; i < dun->wall_n; i++)
	{
		/* Access the grid */
		y = dun->wall[i].y;
		x = dun->wall[i].x;

		/* Convert to floor grid */
		cave_feat[y][x] = FEAT_FLOOR;

		/* Occasional doorway */
		if (rand_int(100) < DUN_TUN_PEN)
		{
			/* Place a random door */
			place_random_door(y, x);
		}
	}
}




/*
 * Count the number of "corridor" grids adjacent to the given grid.
 *
 * Note -- Assumes "in_bounds_fully(y1, x1)"
 *
 * This routine currently only counts actual "empty floor" grids
 * which are not in rooms.  We might want to also count stairs,
 * open doors, closed doors, etc.  XXX XXX
 */
static int next_to_corr(int y1, int x1)
{
	int i, y, x, k = 0;


	/* Scan adjacent grids */
	for (i = 0; i < 4; i++)
	{
		/* Extract the location */
		y = y1 + ddy_ddd[i];
		x = x1 + ddx_ddd[i];

		/* Skip non floors */
		if (!cave_floor_bold(y, x)) continue;

		/* Skip non "empty floor" grids */
		if (cave_feat[y][x] != FEAT_FLOOR) continue;

		/* Skip grids inside rooms */
		if (cave_info[y][x] & (CAVE_ROOM)) continue;

		/* Count these grids */
		k++;
	}

	/* Return the number of corridors */
	return (k);
}


/*
 * Determine if the given location is "between" two walls,
 * and "next to" two corridor spaces.  XXX XXX XXX
 *
 * Assumes "in_bounds_fully(y,x)"
 */
static bool possible_doorway(int y, int x)
{
	/* Count the adjacent corridors */
	if (next_to_corr(y, x) >= 2)
	{
		/* Check Vertical */
		if ((cave_feat[y-1][x] >= FEAT_MAGMA) &&
		    (cave_feat[y+1][x] >= FEAT_MAGMA))
		{
			return (TRUE);
		}

		/* Check Horizontal */
		if ((cave_feat[y][x-1] >= FEAT_MAGMA) &&
		    (cave_feat[y][x+1] >= FEAT_MAGMA))
		{
			return (TRUE);
		}
	}

	/* No doorway */
	return (FALSE);
}


/*
 * Places door at y, x position if at least 2 walls found
 */
static void try_door(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Ignore walls */
	if (cave_feat[y][x] >= FEAT_MAGMA) return;

	/* Ignore room grids */
	if (cave_info[y][x] & (CAVE_ROOM)) return;

	/* Occasional door (if allowed) */
	if ((rand_int(100) < DUN_TUN_JCT) && possible_doorway(y, x))
	{
		/* Place a door */
		place_random_door(y, x);
	}
}




/*
 * Attempt to build a room of the given type at the given block
 *
 * Note that we restrict the number of "crowded" rooms to reduce
 * the chance of overflowing the monster list during level creation.
 */
static bool room_build(int y0, int x0, int typ)
{
	int y, x, y1, x1, y2, x2;


	/* Restrict level */
	if ((dun_depth) < room_info[typ].level) return (FALSE);

	/* Restrict "crowded" rooms */
	if (dun->crowded && ((typ == 5) || (typ == 6))) return (FALSE);

	/* Extract blocks */
	y1 = y0 + room_info[typ].dy1;
	y2 = y0 + room_info[typ].dy2;
	x1 = x0 + room_info[typ].dx1;
	x2 = x0 + room_info[typ].dx2;

	/* Never run off the screen */
	if ((y1 < 0) || (y2 >= dun->row_rooms)) return (FALSE);
	if ((x1 < 0) || (x2 >= dun->col_rooms)) return (FALSE);

	/* Verify open space */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			if (dun->room_map[y][x]) return (FALSE);
		}
	}

	/* XXX XXX XXX It is *extremely* important that the following */
	/* calculation is *exactly* correct to prevent memory errors */

	/* Acquire the location of the room */
	y = ((y1 + y2 + 1) * BLOCK_HGT) / 2;
	x = ((x1 + x2 + 1) * BLOCK_WID) / 2;

	/* Build a room */
	switch (typ)
	{
		/* Build an appropriate room */
		case 8: case 7: build_type7_or_8(y, x, typ); break;
		case 6: build_type6(y, x); break;
		case 5: build_type5(y, x); break;
		case 4: build_type4(y, x); break;
		case 3: build_type3(y, x); break;
		case 2: build_type2(y, x); break;
		case 1: build_type1(y, x); break;

		/* Paranoia */
		default: return (FALSE);
	}

	/* Save the room location */
	if (dun->cent_n < CENT_MAX)
	{
		dun->cent[dun->cent_n].y = y;
		dun->cent[dun->cent_n].x = x;
		dun->cent_n++;
	}

	/* Reserve some blocks */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			dun->room_map[y][x] = TRUE;
		}
	}

	/* Count "crowded" rooms */
	if ((typ == 5) || (typ == 6)) dun->crowded = TRUE;

	/* Success */
	return (TRUE);
}


/*
 * Generate a new dungeon level. Return an error message if this attempt failed.
 *
 * Note that "dun_body" adds about 4000 bytes of memory to the stack.
 */
static cptr cave_gen(void)
{
	int i, k, y, x, y1, x1;
	int max_vault_ok = 2;
	bool destroyed = FALSE;
	bool empty_level = FALSE;

	dun_data dun_body;

	/* Global data */
	dun = &dun_body;


	if (!(cur_hgt != DUNGEON_HGT)) max_vault_ok--;
	if (!(cur_wid != DUNGEON_WID)) max_vault_ok--;


	if ((randint(EMPTY_LEVEL)==1) && empty_levels)
	{
		empty_level = TRUE;
		if (cheat_room)
			msg_print("Arena level.");
		}

	/* Hack -- Start with basic granite */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			if (empty_level)
				cave_feat[y][x] = FEAT_FLOOR;
			else
				/* Create granite wall */
				cave_feat[y][x] = FEAT_WALL_EXTRA;
		}
	}


	/* Possible "destroyed" level */
	if (((dun_depth) > 10) && (rand_int(DUN_DEST) == 0) && (small_levels))
		destroyed = TRUE;

	if (is_quest())
	{
		/* Hack -- No destroyed "quest" levels */
		destroyed = FALSE;

		/* Quest Level => make quest monster (Heino Vander Sanden) */
		r_info[get_quest_monster()].flags1 |= (RF1_GUARDIAN);
	}

	/* Actual maximum number of rooms on this level */
	dun->row_rooms = cur_hgt / BLOCK_HGT;
	dun->col_rooms = cur_wid / BLOCK_WID;

	/* Initialize the room table */
	for (y = 0; y < dun->row_rooms; y++)
	{
		for (x = 0; x < dun->col_rooms; x++)
		{
			dun->room_map[y][x] = FALSE;
		}
	}


	/* No "crowded" rooms yet */
	dun->crowded = FALSE;


	/* No rooms yet */
	dun->cent_n = 0;

	/* Build some rooms */
	for (i = 0; i < DUN_ROOMS; i++)
	{
		/* Pick a block for the room */
		y = rand_int(dun->row_rooms);
		x = rand_int(dun->col_rooms);

		/* Align dungeon rooms */
		if (dungeon_align)
		{
			/* Slide some rooms right */
			if ((x % 3) == 0) x++;

			/* Slide some rooms left */
			if ((x % 3) == 2) x--;
		}

		/* Destroyed levels are boring */
		if (destroyed)
		{
			/* Attempt a "trivial" room */
			if (room_build(y, x, 1)) continue;

			/* Never mind */
			continue;
		}

		/* Attempt an "unusual" room */
		if (rand_int(DUN_UNUSUAL) < (dun_depth))
		{
			/* Roll for room type */
			k = rand_int(100);

			/* Attempt a very unusual room */ /* test hack */
			if (rand_int(DUN_UNUSUAL) < (dun_depth) || 1)
			{
#ifdef FORCE_V_IDX
				if (room_build(y,x,8)) continue;
#else
				/* Type 8 -- Greater vault (10%) */
				if (k < 10)
				{
					if (max_vault_ok > 1)
					{
						if (room_build(y, x, 8)) continue;
					}
					else
					{
						if (cheat_room) msg_print("Refusing a greater vault.");
					}
				}

				/* Type 7 -- Lesser vault (15%) */
				if (k < 25)
				{
					if (max_vault_ok > 0)
					{
						if (room_build(y, x, 7)) continue;
					}
					else
					{
						if (cheat_room) msg_print("Refusing a lesser vault.");
					}
				}


				/* Type 5 -- Monster nest (15%) */
				if ((k < 40) && room_build(y, x, 5)) continue;

				/* Type 6 -- Monster pit (15%) */
				if ((k < 55) && room_build(y, x, 6)) continue;
#endif

			}

			/* Type 4 -- Large room (25%) */
			if ((k < 25) && room_build(y, x, 4)) continue;

			/* Type 3 -- Cross room (25%) */
			if ((k < 50) && room_build(y, x, 3)) continue;

			/* Type 2 -- Overlapping (50%) */
			if ((k < 100) && room_build(y, x, 2)) continue;
		}

		/* Attempt a trivial room */
		if (room_build(y, x, 1)) continue;
	}


	/* Special boundary walls -- Top */
	for (x = 0; x < DUNGEON_WID; x++)
	{
		y = 0;

		/* Clear previous contents, add "solid" perma-wall */
		cave_feat[y][x] = FEAT_PERM_SOLID;
	}

	/* Special boundary walls -- Bottom */
	for (x = 0; x < DUNGEON_WID; x++)
	{
		y = DUNGEON_HGT-1;

		/* Clear previous contents, add "solid" perma-wall */
		cave_feat[y][x] = FEAT_PERM_SOLID;
	}

	/* Special boundary walls -- Left */
	for (y = 0; y < DUNGEON_HGT; y++)
	{
		x = 0;

		/* Clear previous contents, add "solid" perma-wall */
		cave_feat[y][x] = FEAT_PERM_SOLID;
	}

	/* Special boundary walls -- Right */
	for (y = 0; y < DUNGEON_HGT; y++)
	{
		x = DUNGEON_WID-1;

		/* Clear previous contents, add "solid" perma-wall */
		cave_feat[y][x] = FEAT_PERM_SOLID;
	}

	/* Hack -- Scramble the room order */
	for (i = 0; i < dun->cent_n; i++)
	{
		int pick1 = rand_int(dun->cent_n);
		int pick2 = rand_int(dun->cent_n);
		y1 = dun->cent[pick1].y;
		x1 = dun->cent[pick1].x;
		dun->cent[pick1].y = dun->cent[pick2].y;
		dun->cent[pick1].x = dun->cent[pick2].x;
		dun->cent[pick2].y = y1;
		dun->cent[pick2].x = x1;
	}

	/* Start with no tunnel doors */
	dun->door_n = 0;

	/* Hack -- connect the first room to the last room */
	y = dun->cent[dun->cent_n-1].y;
	x = dun->cent[dun->cent_n-1].x;

	/* Connect all the rooms together */
	for (i = 0; i < dun->cent_n; i++)
	{
		/* Connect the room to the previous room */
		build_tunnel(dun->cent[i].y, dun->cent[i].x, y, x);

		/* Remember the "previous" room */
		y = dun->cent[i].y;
		x = dun->cent[i].x;
	}

	/* Place intersection doors  */
	for (i = 0; i < dun->door_n; i++)
	{
		/* Extract junction location */
		y = dun->door[i].y;
		x = dun->door[i].x;

		/* Try placing doors */
		try_door(y, x - 1);
		try_door(y, x + 1);
		try_door(y - 1, x);
		try_door(y + 1, x);
	}


	/* Hack -- Add some magma streamers */
	for (i = 0; i < DUN_STR_MAG; i++)
	{
		build_streamer(FEAT_MAGMA, DUN_STR_MC);
	}

	/* Hack -- Add some quartz streamers */
	for (i = 0; i < DUN_STR_QUA; i++)
	{
		build_streamer(FEAT_QUARTZ, DUN_STR_QC);
	}


	/* Destroy the level if necessary */
	if (destroyed) destroy_level();


	/* Place 3 or 4 down stairs near some walls */
	alloc_stairs(FEAT_MORE, rand_range(3, 4), 3);

	/* Place 1 or 2 up stairs near some walls */
	alloc_stairs(FEAT_LESS, rand_range(1, 2), 3);

	/* Add an extra stairway somewhere on the level. */
	if (p_ptr->came_from == START_UP_STAIRS || p_ptr->came_from == START_DOWN_STAIRS)
		alloc_stairs(p_ptr->came_from, 1, 0);


	/* Basic "amount" */
	k = ((dun_depth) / 3);
	if (k > 10) k = 10;
	if (k < 2) k = 2;

	/*
	 * Put the quest monster(s) in the dungeon
	 * Heino Vander Sanden
	 */
	if (is_quest())
	{
		quest_type *q_ptr;
		int r_idx;

		r_idx = get_quest_monster();
		q_ptr = get_quest();
		if (q_ptr->cur_num != 0) q_ptr->cur_num = q_ptr->cur_num_known = 0;
		while (r_info[r_idx].cur_num < q_ptr->max_num)
		{
			if (!put_quest_monster(q_ptr->r_idx))
				return "could not place quest monster";
		}

	}

	/* Pick a base number of monsters */
	i = MIN_M_ALLOC_LEVEL;

	/* To make small levels a bit more playable */
	if (cur_hgt < MAX_HGT || cur_wid < MAX_WID)
	{
		int small_tester = i;

		i = (i * cur_hgt) / MAX_HGT;
		i = (i * cur_wid) / MAX_WID;
		i += 1;

		if (i > small_tester) i = small_tester;
		else if (cheat_hear)
		{
			msg_format("Reduced monsters base from %d to %d", small_tester, i);
		}
	}

	i += randint(8);

	/* Put some monsters in the dungeon */
	for (i = i + k; i > 0; i--)
	{
		(void)alloc_monster(0, dun_depth, TRUE);
	}


	/* Place some traps in the dungeon */
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_TRAP, randint(k));

	/* Put some rubble in corridors */
	alloc_object(ALLOC_SET_CORR, ALLOC_TYP_RUBBLE, randint(k));

	/* Put some objects in rooms */
	alloc_object(ALLOC_SET_ROOM, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ROOM, 3));

	/* Put some objects/gold in the dungeon */
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_OBJECT, randnor(DUN_AMT_ITEM, 3));
	alloc_object(ALLOC_SET_BOTH, ALLOC_TYP_GOLD, randnor(DUN_AMT_GOLD, 3));

	if (empty_level && (!one_in(DARK_EMPTY) || !percent(dun_depth)))
	{
		for (y=0; y<DUNGEON_HGT; y++)
		{
			for (x=0; x<DUNGEON_WID; x++)
			{
				/* Perma-lite the grid */
				cave_info[y][x] |= (CAVE_GLOW);
			}
		}
		//wiz_lite();
	}

	/* Ghosts are never used in the first few levels. */
	if (dun_depth < 6)
	{
		i = 0;
	}
	/* Ghosts love to inhabit destroyed levels. */
	else if (destroyed)
	{
		i = 11;
	}
	/* They can exist elsewhere, though. */
	else
	{
		i = 1;
	}

	/* Roll for it. */
	while (i && rand_int(dun_depth/2*3+36) >= dun_depth/2-2) i--;

	/* Attempt to place a ghost, if allowed */
	if (i && place_ghost())
	{
		/* A ghost makes the level special */
		good_item_flag = TRUE;
	}

	/* Put the player on the map, or fail. */
	if (!place_player())
		return "could not place player";

	return NULL;
}



/*
 * Builds a store at a given (row, column)
 *
 * Note that the solid perma-walls are at x=0/65 and y=0/21
 *
 * As of 2.7.4 (?) the stores are placed in a more "user friendly"
 * configuration, such that the four "center" buildings always
 * have at least four grids between them, to allow easy running,
 * and the store doors tend to face the middle of town.
 *
 * The stores now lie inside boxes from 3-9 and 12-18 vertically,
 * and from 7-17, 21-31, 35-45, 49-59.  Note that there are thus
 * always at least 2 open grids between any disconnected walls.
 */
static void build_store(int n, int yy, int xx)
{
	int y, x, y0, x0, y1, x1, y2, x2, tmp;

	/* Only build stores if they exist */
	if (store[n].type == 99) return;

	/* Find the "center" of the store */
	y0 = yy * 9 + 6;
	x0 = xx * 14 + 12;

	/* Determine the store boundaries */
	y1 = y0 - randint((yy == 0) ? 3 : 2);
	y2 = y0 + randint((yy == 1) ? 3 : 2);
	x1 = x0 - randint(5);
	x2 = x0 + randint(5);

	/* Build an invulnerable rectangular building */
	for (y = y1; y <= y2; y++)
	{
		for (x = x1; x <= x2; x++)
		{
			/* Clear previous contents, add "basic" perma-wall */
			cave_feat[y][x] = FEAT_PERM_EXTRA;

			/* Hack -- The buildings are illuminated and known */
			cave_info[y][x] |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Pick a door direction (S,N,E,W) */
	tmp = 0;
	if ((xx > 0) && (yy == 0)) tmp=0;
	if ((xx < 3) && (yy == 3)) tmp=1;
	if ((xx == 0) && (yy < 3)) tmp=2;
	if ((xx == 3) && (yy > 0)) tmp=3;

	/* Extract a "door location" */
	switch (tmp)
	{
		/* Bottom side */
		case 0:
		{
			y = y2;
			x = rand_range(x1, x2);
			break;
		}

		/* Top side */
		case 1:
		{
			y = y1;
			x = rand_range(x1, x2);
			break;
		}

		/* Right side */
		case 2:
		{
			y = rand_range(y1, y2);
			x = x2;
			break;
		}

		/* Left side */
		default:
		{
			y = rand_range(y1, y2);
			x = x1;
			break;
		}
	}

	/* Clear previous contents, add a store door */
	cave_feat[y][x] = FEAT_SHOP_HEAD + store[n].type;
	store[n].x = x;
	store[n].y = y;
}




/*
 * Generate the "consistent" town features, and place the player
 *
 * Hack -- play with the R.N.G. to always yield the same town
 * layout, including the size and shape of the buildings, the
 * locations of the doorways, and the location of the stairs.
 */
static void town_gen_hack(void)
{
	int y, x, k, n;

	int rooms[MAX_STORES_PER_TOWN];

	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant town layout */
	if (town_defs[p_ptr->cur_town].seed)
	{
		Rand_value = town_defs[p_ptr->cur_town].seed;
	}
	else
	{
		Rand_value = wild_grid[p_ptr->wildy][p_ptr->wildx].seed;
	}


	/* Prepare an Array of "remaining stores", and count them */
	for (n = 0; n < MAX_STORES_PER_TOWN; n++) rooms[n] = n+(MAX_STORES_PER_TOWN * p_ptr->cur_town);

	/* Place four rows of stores */
	for (y = 0; y < 4; y++)
	{
		/* Place four stores per row */
		for (x = 0; x < 4; x++)
		{
			/* Only put buildings round the edge of the square */
			if ((x == 0) || (x == 3) || (y == 0) || (y == 3))
			{
				/* Pick a random unplaced store */
				k = rand_int(n);

				/* Build that store at the proper location */
				build_store(rooms[k], y, x);

				/* Shift the stores down, remove one store */
				rooms[k] = rooms[--n];
			}
		}
	}
	/* Place a few trees... */
	for(n=0; n < rand_range(5,10); n++)
	{
		x = rand_range(17,46);
		y = rand_range(12,29);

		/* Clear previous contents, add tree */
		cave_feat[y][x] = FEAT_TREE;

		/* Memorize the tree */
		cave_info[y][x] |= (CAVE_MARK);
	}
	/* ...and a few bushes */
	for(n=0; n < rand_range(5,10); n++)
	{
		x = rand_range(17,46);
		y = rand_range(12,29);

		/* Clear previous contents, add bush */
		cave_feat[y][x] = FEAT_BUSH;

		/* Memorize the tree */
		cave_info[y][x] |= (CAVE_MARK);
	}


	/* Now place the town gates on all walls*/
	x=cur_wid/2;
	y=1;
	cave_feat[1][x-1] = FEAT_PERM_SOLID;
	cave_info[1][x-1] |= CAVE_MARK;
	cave_feat[0][x] = FEAT_WILD_BORDER;
	cave_info[0][x] |= CAVE_MARK;
	cave_feat[1][x+1] = FEAT_PERM_SOLID;
	cave_info[1][x+1] |= CAVE_MARK;
	cave_feat[y][x] = FEAT_OPEN;
	cave_info[y][x] |= CAVE_MARK;

	x=cur_wid-2;
	y=cur_hgt/2;
	cave_feat[y-1][x] = FEAT_PERM_SOLID;
	cave_info[y-1][x] |= CAVE_MARK;
	cave_feat[y][x+1] = FEAT_WILD_BORDER;
	cave_info[y][x+1] |= CAVE_MARK;
	cave_feat[y+1][x] = FEAT_PERM_SOLID;
	cave_info[y+1][x] |= CAVE_MARK;
	cave_feat[y][x] = FEAT_OPEN;
	cave_info[y][x] |= CAVE_MARK;

	x=cur_wid/2;
	y=cur_hgt-2;
	cave_feat[y][x-1] = FEAT_PERM_SOLID;
	cave_info[y][x-1] |= CAVE_MARK;
	cave_feat[y+1][x] = FEAT_WILD_BORDER;
	cave_info[y+1][x] |= CAVE_MARK;
	cave_feat[y][x+1] = FEAT_PERM_SOLID;
	cave_info[y][x+1] |= CAVE_MARK;
	cave_feat[y][x] = FEAT_OPEN;
	cave_info[y][x] |= CAVE_MARK;

	x=1;
	y=cur_hgt/2;
	cave_feat[y-1][1] = FEAT_PERM_SOLID;
	cave_info[y-1][1] |= CAVE_MARK;
	cave_feat[y][0] = FEAT_WILD_BORDER;
	cave_info[y][0] |= CAVE_MARK;
	cave_feat[y+1][1] = FEAT_PERM_SOLID;
	cave_info[y+1][1] |= CAVE_MARK;
	cave_feat[y][x] = FEAT_OPEN;
	cave_info[y][x] |= CAVE_MARK;

	/* Place the stairs, if any. */
	if (dun_defs[p_ptr->cur_town].max_level)
	{
		int dummy;
		for (dummy = 0; dummy < SAFE_MAX_ATTEMPTS; dummy++)
		{
			dummy++;

			/* Pick a location within the central area */
			y = rand_range(12, 29);
			x = rand_range(17,46);

			/* Require a "naked" floor grid */
			if (cave_naked_bold(y, x)) break;
		}

		if (dummy >= SAFE_MAX_ATTEMPTS)
		{
			if (cheat_room)
			{
				msg_print("Warning! Could not place stairs!");
			}
		}

		/* Clear previous contents, add down stairs */
		cave_feat[y][x] = FEAT_MORE;

		/* Memorize the stairs */
		cave_info[y][x] |= (CAVE_MARK);
	}

	/* Hack -- use the "complex" RNG */
	Rand_quick = FALSE;


	/* Put the player on the map. */
	if (!place_player())
	{
		/* Paranoia - this should not be able to fail. */
		p_ptr->py = p_ptr->px = 1;
	}

	/* Randomise the spirit names for the shops */
	generate_spirit_names();
}




/*
 * Town logic flow for generation of new town
 *
 * We start with a fully wiped cave of normal floors.
 *
 * Note that town_gen_hack() plays games with the R.N.G.
 *
 * This function does NOT do anything about the owners of the stores,
 * nor the contents thereof.  It only handles the physical layout.
 *
 */
static void town_gen(void)
{
	int i, y, x;

	/* Identify the town. */
	p_ptr->cur_town = wild_grid[p_ptr->wildy][p_ptr->wildx].dungeon;

	/* Start with solid walls */
	for (y = 0; y < DUNGEON_HGT; y++)
	{
		for (x = 0; x < DUNGEON_WID; x++)
		{
			/* Create "solid" perma-wall */
			cave_feat[y][x] = FEAT_PERM_SOLID;

			/* Illuminate and memorize the walls */
			cave_info[y][x] |= (CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Then place some floors */
	for (y = 1; y < cur_hgt-1; y++)
	{
		for (x = 1; x < cur_wid-1; x++)
		{
			/* Create empty floor */
			cave_feat[y][x] = FEAT_FLOOR;

			/* Darken and forget the floors */
			cave_info[y][x] &= ~(CAVE_GLOW | CAVE_MARK);
		}
	}

	/* Hack -- Build the buildings/stairs (from memory) */
	town_gen_hack();

	/* Curiously, there are more people about at night. */
	i = (daytime_p()) ? MIN_M_ALLOC_TD : MIN_M_ALLOC_TN;

	/* Handle daylight and add monsters. */
	surface_gen_final(i);
}


/*
 * Generates a random dungeon level -RAK-
 *
 * Hack -- regenerate any "overflow" levels
 *
 * Hack -- allow auto-scumming via a gameplay option.
 */
void generate_cave(void)
{
	int i, y, x, num;
	cptr why;


	/* The dungeon is not ready */
	character_dungeon = FALSE;

	/* Roll for a few things here to prevent them from being affected by
	 * auto_scum */

	/* Surface levels are 2x1 */
	if (dun_level == 0)
	{
		/* Small town */
		cur_hgt = (2*SCREEN_HGT);
		cur_wid = (SCREEN_WID);
	}
	/* Towers are 1x1 */
	else if (dun_defs[p_ptr->cur_dungeon].flags & DF_TOWER)
	{
		cur_hgt = SCREEN_HGT;
		cur_wid = SCREEN_WID;
	}
	/* Sometimes build a small dungeon */
	else if (((randint(SMALL_LEVEL)==1) && small_levels) || dungeon_small)
	{
		cur_hgt = randint(MAX_HGT/SCREEN_HGT) * SCREEN_HGT;
		cur_wid = randint(MAX_WID/SCREEN_WID) * SCREEN_WID;
		if (cheat_room)
		{
			msg_print ("Small dungeon:");
			msg_format("X:%d, Y:%d.", cur_wid, cur_hgt);
		}
	}
	else
	{
		/* Big dungeon */
		cur_hgt = MAX_HGT;
		cur_wid = MAX_WID;
	}



	/* Get the correct dungeon offset */
	if(dun_level != 0)
	{
		dun_offset = dun_defs[p_ptr->cur_dungeon].offset;
		dun_bias = dun_defs[p_ptr->cur_dungeon].bias;
	}
	/* Not too nasty in the wilderness */
	else if (wild_grid[p_ptr->wildy][p_ptr->wildx].dungeon >= MAX_CAVES)
	{
		dun_offset = 2;
		/* We get mainly animals in the wilderness */
		dun_bias = SUMMON_ANIMAL;
	}
	/* Very nasty at Kadath */
	else if (dun_defs[wild_grid[p_ptr->wildy][p_ptr->wildx].dungeon].flags & DF_KADATH)
	{
		dun_offset = 35;
		dun_bias = SUMMON_CTHULOID;
	}
	/* Zero in towns */
	else if (is_town_p(p_ptr->wildy, p_ptr->wildx))
	{
		dun_offset = 0;
		dun_bias = 0;
	}
	/* Nastier near dungeons */
	else
	{
		dun_offset = dun_defs[wild_grid[p_ptr->wildy][p_ptr->wildx].dungeon].offset/2;
		if (dun_offset < 4) dun_offset = 4;
		dun_bias = dun_defs[wild_grid[p_ptr->wildy][p_ptr->wildx].dungeon].bias;
	}

	/* Reset the suitable monster list. */
	get_mon_num_prep(NULL, 0);


	/* Generate */
	for (num = 0, why = ""; why && num < 100; num++)
	{
		/* Nothing special here yet. */
		good_item_flag = FALSE;

		/* Message if "good" test fails noisily. */
		if (why && *why) msg_format("Generation restarted (%s)", why);

		/* Assume good */
		why = NULL;

		/* Wipe the objects */
		wipe_o_list(num || preserve_mode);

		/* Wipe the monsters */
		remove_non_pets();


		/* Start with a blank cave */
		for (y = 0; y < DUNGEON_HGT; y++)
		{
			for (x = 0; x < DUNGEON_WID; x++)
			{
				/* No flags */
				cave_info[y][x] = 0;

				/* No features */
				cave_feat[y][x] = 0;

				/* No objects */
				cave_o_idx[y][x] = 0;

				/* No monsters */
				cave_m_idx[y][x] = 0;

#ifdef MONSTER_FLOW
				/* No flow */
				cave_cost[y][x] = 0;
				cave_when[y][x] = 0;
#endif /* MONSTER_FLOW */

			}
		}

		/* Reset the object generation level */
		object_level = (dun_depth);

		/* Nothing good here yet */
		rating = 0;


		/* Build the town */
		if (dun_level == 0)
		{
			/* If your grid is a town */
			if(is_town_p(p_ptr->wildy, p_ptr->wildx))
			{
				/* Make a town */
				town_gen();
			}
			else
			{
				/* Make a wilderness */
				why = terrain_gen();
			}
		}

		/* Build a real level */
		else
		{
			/* Make a dungeon, and remember any errors. */
			why = cave_gen();
		}


		/* Extract the feeling */
		if (rating > 100) feeling = 2;
		else if (rating > 80) feeling = 3;
		else if (rating > 60) feeling = 4;
		else if (rating > 40) feeling = 5;
		else if (rating > 30) feeling = 6;
		else if (rating > 20) feeling = 7;
		else if (rating > 10) feeling = 8;
		else if (rating > 0) feeling = 9;
		else feeling = 10;

		for (i = 1; !preserve_mode && !good_item_flag && i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];
			if (o_ptr->k_idx && artifact_p(o_ptr)) good_item_flag = TRUE;
		}

		/* Hack -- Have a special feeling sometimes */
		if (!preserve_mode && good_item_flag) feeling = 1;

		/* Hack -- no feeling in the town */
		if (dun_level <= 0) feeling = 0;

		/* Scatter allied monsters around the player. */
		replace_all_friends();

		/* Prevent object over-flow */
		if (o_max >= MAX_O_IDX) why = "too many objects";

		/* Prevent monster over-flow */
		if (m_max >= MAX_M_IDX) why = "too many monsters";

		/* Mega-Hack -- "auto-scum" */
		if (auto_scum)
		{
			/* Require "goodness" */
			if ((feeling > 9) ||
				(((dun_depth) >= 5) && (feeling > 8)) ||
				(((dun_depth) >= 10) && (feeling > 7)) ||
				(((dun_depth) >= 20) && (feeling > 6)) ||
				(((dun_depth) >= 40) && (feeling > 5)))
			{
				/* Give message to cheaters */
				if (cheat_room || cheat_hear ||
					cheat_peek || cheat_xtra)
				{
					/* Message */
					why = "boring level";
				}
				else
				{
					why = "";
				}
			}
		}
	}


	/* The dungeon is ready */
	character_dungeon = TRUE;

	/* Remember when this level was "created" */
		old_turn = turn;
}

