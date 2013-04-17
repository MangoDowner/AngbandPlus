/* File: init1.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * Note that if "ALLOW_TEMPLATES" is not defined, then a lot of the code
 * in this file is compiled out, and the game will not run unless valid
 * "binary template files" already exist in "lib/data".  Thus, one can
 * compile Angband with ALLOW_TEMPLATES defined, run once to create the
 * "*.raw" files in "lib/data", and then quit, and recompile without
 * defining ALLOW_TEMPLATES, which will both save 20K and prevent people
 * from changing the ascii template files in potentially dangerous ways.
 *
 * The code could actually be removed and placed into a "stand-alone"
 * program, but that feels a little silly, especially considering some
 * of the platforms that we currently support.
 */


/*
 * Convert a "color letter" into an "actual" color
 * The colors are: dwsorgbuDWvyRGBU, as shown below
 */
int color_char_to_attr(char c)
{
	switch (c)
	{
		case 'd': return (TERM_DARK);
		case 'w': return (TERM_WHITE);
		case 's': return (TERM_SLATE);
		case 'o': return (TERM_ORANGE);
		case 'r': return (TERM_RED);
		case 'g': return (TERM_GREEN);
		case 'b': return (TERM_BLUE);
		case 'u': return (TERM_UMBER);

		case 'D': return (TERM_L_DARK);
		case 'W': return (TERM_L_WHITE);
		case 'v': return (TERM_VIOLET);
		case 'y': return (TERM_YELLOW);
		case 'R': return (TERM_L_RED);
		case 'G': return (TERM_L_GREEN);
		case 'B': return (TERM_L_BLUE);
		case 'U': return (TERM_L_UMBER);
	}

	return (-1);
}


#ifdef ALLOW_TEMPLATES


/*
 * Hack -- error tracking
 */
extern s16b error_idx;
extern s16b error_line;



/*** Helper arrays for parsing ascii template files ***/

/*
 * Monster Blow Methods
 */
static cptr r_info_blow_method[] =
{
	"",
	"HIT",
	"TOUCH",
	"PUNCH",
	"KICK",
	"CLAW",
	"BITE",
	"STING",
	"XXX1",
	"BUTT",
	"CRUSH",
	"ENGULF",
	"XXX2",
	"CRAWL",
	"DROOL",
	"SPIT",
	"XXX3",
	"GAZE",
	"WAIL",
	"SPORE",
	"XXX4",
	"BEG",
	"INSULT",
	"MOAN",
	"XXX5",
	NULL
};


/*
 * Monster Blow Effects
 */
static cptr r_info_blow_effect[] =
{
	"",
	"HURT",
	"POISON",
	"UN_BONUS",
	"UN_POWER",
	"EAT_GOLD",
	"EAT_ITEM",
	"EAT_FOOD",
	"EAT_LITE",
	"ACID",
	"ELEC",
	"FIRE",
	"COLD",
	"BLIND",
	"CONFUSE",
	"TERRIFY",
	"PARALYZE",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_ALL",
	"SHATTER",
	"EXP_10",
	"EXP_20",
	"EXP_40",
	"EXP_80",
	NULL
};


/*
 * Monster race flags
 */
static cptr r_info_flags1[] =
{
	"UNIQUE",
	"QUESTOR",
	"MALE",
	"FEMALE",
	"CHAR_CLEAR",
	"CHAR_MULTI",
	"ATTR_CLEAR",
	"ATTR_MULTI",
	"FORCE_DEPTH",
	"FORCE_MAXHP",
	"FORCE_SLEEP",
	"FORCE_EXTRA",
	"FRIEND",
	"FRIENDS",
	"ESCORT",
	"ESCORTS",
	"NEVER_BLOW",
	"NEVER_MOVE",
	"RAND_25",
	"RAND_50",
	"ONLY_GOLD",
	"ONLY_ITEM",
	"DROP_60",
	"DROP_90",
	"DROP_1D2",
	"DROP_2D2",
	"DROP_3D2",
	"DROP_4D2",
	"DROP_GOOD",
	"DROP_GREAT",
	"DROP_USEFUL",
	"DROP_CHOSEN"
};

/*
 * Monster race flags
 */
static cptr r_info_flags2[] =
{
	"STUPID",
	"SMART",
	"XXX1X2",
	"XXX2X2",
	"INVISIBLE",
	"COLD_BLOOD",
	"EMPTY_MIND",
	"WEIRD_MIND",
	"MULTIPLY",
	"REGENERATE",
	"XXX3X2",
	"XXX4X2",
	"POWERFUL",
	"XXX5X2",
	"XXX7X2",
	"XXX6X2",
	"OPEN_DOOR",
	"BASH_DOOR",
	"PASS_WALL",
	"KILL_WALL",
	"MOVE_BODY",
	"KILL_BODY",
	"TAKE_ITEM",
	"KILL_ITEM",
	"BRAIN_1",
	"BRAIN_2",
	"BRAIN_3",
	"BRAIN_4",
	"BRAIN_5",
	"BRAIN_6",
	"BRAIN_7",
	"BRAIN_8"
};

/*
 * Monster race flags
 */
static cptr r_info_flags3[] =
{
	"ORC",
	"TROLL",
	"GIANT",
	"DRAGON",
	"DEMON",
	"UNDEAD",
	"EVIL",
	"ANIMAL",
	"XXX1X3",
	"XXX2X3",
	"XXX3X3",
	"XXX4X3",
	"HURT_LITE",
	"HURT_ROCK",
	"HURT_FIRE",
	"HURT_COLD",
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"IM_POIS",
	"XXX5X3",
	"RES_NETH",
	"RES_WATE",
	"RES_PLAS",
	"RES_NEXU",
	"RES_DISE",
	"XXX6X3",
	"NO_FEAR",
	"NO_STUN",
	"NO_CONF",
	"NO_SLEEP"
};

/*
 * Monster race flags
 */
static cptr r_info_flags4[] =
{
	"SHRIEK",
	"XXX2X4",
	"XXX3X4",
	"XXX4X4",
	"ARROW_1",
	"ARROW_2",
	"ARROW_3",
	"ARROW_4",
	"BR_ACID",
	"BR_ELEC",
	"BR_FIRE",
	"BR_COLD",
	"BR_POIS",
	"BR_NETH",
	"BR_LITE",
	"BR_DARK",
	"BR_CONF",
	"BR_SOUN",
	"BR_CHAO",
	"BR_DISE",
	"BR_NEXU",
	"BR_TIME",
	"BR_INER",
	"BR_GRAV",
	"BR_SHAR",
	"BR_PLAS",
	"BR_WALL",
	"BR_MANA",
	"XXX5X4",
	"XXX6X4",
	"XXX7X4",
	"XXX8X4"
};

/*
 * Monster race flags
 */
static cptr r_info_flags5[] =
{
	"BA_ACID",
	"BA_ELEC",
	"BA_FIRE",
	"BA_COLD",
	"BA_POIS",
	"BA_NETH",
	"BA_WATE",
	"BA_MANA",
	"BA_DARK",
	"DRAIN_MANA",
	"MIND_BLAST",
	"BRAIN_SMASH",
	"CAUSE_1",
	"CAUSE_2",
	"CAUSE_3",
	"CAUSE_4",
	"BO_ACID",
	"BO_ELEC",
	"BO_FIRE",
	"BO_COLD",
	"BO_POIS",
	"BO_NETH",
	"BO_WATE",
	"BO_MANA",
	"BO_PLAS",
	"BO_ICEE",
	"MISSILE",
	"SCARE",
	"BLIND",
	"CONF",
	"SLOW",
	"HOLD"
};

/*
 * Monster race flags
 */
static cptr r_info_flags6[] =
{
	"HASTE",
	"XXX1X6",
	"HEAL",
	"XXX2X6",
	"BLINK",
	"TPORT",
	"XXX3X6",
	"XXX4X6",
	"TELE_TO",
	"TELE_AWAY",
	"TELE_LEVEL",
	"XXX5",
	"DARKNESS",
	"TRAPS",
	"FORGET",
	"XXX6X6",
	"S_KIN",
	"S_HI_DEMON",
	"S_MONSTER",
	"S_MONSTERS",
	"S_ANT",
	"S_SPIDER",
	"S_HOUND",
	"S_HYDRA",
	"S_ANGEL",
	"S_DEMON",
	"S_UNDEAD",
	"S_DRAGON",
	"S_HI_UNDEAD",
	"S_HI_DRAGON",
	"S_WRAITH",
	"S_UNIQUE"
};


/*
 * Object flags
 * ~ added poison brand -- neko
 */
static cptr k_info_flags1[] =
{
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHR",
	"XXX1",
	"XXX2",
	"STEALTH",
	"SEARCH",
	"INFRA",
	"TUNNEL",
	"SPEED",
	"BLOWS",
	"SHOTS",
	"MIGHT",
	"SLAY_ANIMAL",
	"SLAY_EVIL",
	"SLAY_UNDEAD",
	"SLAY_DEMON",
	"SLAY_ORC",
	"SLAY_TROLL",
	"SLAY_GIANT",
	"SLAY_DRAGON",
	"KILL_DRAGON",
	"AMMUNITION",		/* ~ missile weapons -- neko */
	"XXX6",
	"BRAND_POIS",		/* ~ Poison brand -- neko */
	"BRAND_ACID",
	"BRAND_ELEC",
	"BRAND_FIRE",
	"BRAND_COLD"
};

/*
 * Object flags
 */
static cptr k_info_flags2[] =
{
	"SUST_STR",
	"SUST_INT",
	"SUST_WIS",
	"SUST_DEX",
	"SUST_CON",
	"SUST_CHR",
	"XXX1",
	"XXX2",
	"XXX3",
	"XXX4",
	"XXX5",
	"XXX6",
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"RES_ACID",
	"RES_ELEC",
	"RES_FIRE",
	"RES_COLD",
	"RES_POIS",
	"RES_FEAR",
	"RES_LITE",
	"RES_DARK",
	"RES_BLIND",
	"RES_CONFU",
	"RES_SOUND",
	"RES_SHARD",
	"RES_NEXUS",
	"RES_NETHR",
	"RES_CHAOS",
	"RES_DISEN"
};

/*
 * Object flags
 */
static cptr k_info_flags3[] =
{
	"SLOW_DIGEST",
	"FEATHER",
	"LITE",
	"REGEN",
	"TELEPATHY",
	"SEE_INVIS",
	"FREE_ACT",
	"HOLD_LIFE",
	"IMPACT",
	"TELEPORT",
	"AGGRAVATE",
	"DRAIN_EXP",
	"CONFER_FIRE",
	"CONFER_COLD",
	"CONFER_ACID",
	"CONFER_ELEC",
	"CONFER_POIS",
	"CONFER_EVIL",
	"IGNORE_ACID",
	"IGNORE_ELEC",
	"IGNORE_FIRE",
	"IGNORE_COLD",
	"BLESSED",
	"ACTIVATE",
	"INSTA_ART",
	"EASY_KNOW",
	"HIDE_TYPE",
	"SHOW_MODS",
	"CONFER_DRGN",
	"LIGHT_CURSE",
	"HEAVY_CURSE",
	"PERMA_CURSE"
};

/* ~ new flag array --neko */
static cptr k_info_flags4[] =
{
    "INH_DRAIN_EXP",
    "INH_MIGHT",
    "INH_SPEED",
    "DEL_FREE_ACT",
    "XXX5",
    "XXX6",
    "XXX7",
    "XXX8",
    "XXX9",
    "XXX10",
    "XXX11",
    "XXX12",
    "XXX13",
    "XXX14",
    "XXX15",
    "XXX16",
    "XXX17",
    "XXX18",
    "XXX19",
    "XXX20",
    "XXX21",
    "NO_WIELDING",
    "NO_SHOOTING",
    "NO_FINGERS",
    "NO_NECK",
    "NO_LIGHT",
    "NO_BODY",
    "NO_CLOAK",
    "NO_ARM",
    "NO_HEAD",
    "NO_HANDS",
    "NO_FEET"
};
    


/*
 * Activation type
 */
static cptr a_info_act[] =
{
	"ILLUMINATION",
	"MAGIC_MAP",
	"CLAIRVOYANCE",
	"PROT_EVIL",
	"DISP_EVIL",
	"HEAL1",
	"HEAL2",
	"CURE_WOUNDS",
	"HASTE1",
	"HASTE2",
	"FIRE1",
	"FIRE2",
	"FIRE3",
	"FROST1",
	"FROST2",
	"FROST3",
	"FROST4",
	"FROST5",
	"ACID1",
	"RECHARGE1",
	"SLEEP",
	"LIGHTNING_BOLT",
	"ELEC2",
	"GENOCIDE",
	"MASS_GENOCIDE",
	"IDENTIFY",
	"DRAIN_LIFE1",
	"DRAIN_LIFE2",
	"BIZZARE",
	"STAR_BALL",
	"RAGE_BLESS_RESIST",
	"PHASE",
	"TRAP_DOOR_DEST",
	"DETECT",
	"RESIST",
	"TELEPORT",
	"RESTORE_LIFE",
	"MISSILE",
	"ARROW",
	"REM_FEAR_POIS",
	"STINKING_CLOUD",
	"STONE_TO_MUD",
	"TELE_AWAY",
	"WOR",
	"CONFUSE",
	"PROBE",
	"FIREBRAND"
};



/*** Initialize from ascii template files ***/


/*
 * Initialize the "z_info" structure, by parsing an ascii "template" file
 */
errr init_z_info_txt(FILE *fp, char *buf)
{
	/* Not ready yet */
	bool okay = FALSE;


	/* Hack - just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != z_head->v_major) ||
			    (v2 != z_head->v_minor) ||
			    (v3 != z_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Hack - Verify 'M:x:' format */
		if (buf[0] != 'M') return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
		if (!buf[2]) return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
		if (buf[3] != ':') return (PARSE_ERROR_UNDEFINED_DIRECTIVE);


		/* Process 'F' for "Maximum f_info[] index" */
		if (buf[2] == 'F')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->f_max = max;

			/* Next... */
			continue;
		}


		/* Process 'K' for "Maximum k_info[] index" */
		if (buf[2] == 'K')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->k_max = max;

			/* Next... */
			continue;
		}


		/* Process 'A' for "Maximum a_info[] index" */
		if (buf[2] == 'A')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->a_max = max;

			/* Next... */
			continue;
		}


		/* Process 'E' for "Maximum e_info[] index" */
		if (buf[2] == 'E')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->e_max = max;

			/* Next... */
			continue;
		}


		/* Process 'R' for "Maximum r_info[] index" */
		if (buf[2] == 'R')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->r_max = max;

			/* Next... */
			continue;
		}


		/* Process 'V' for "Maximum v_info[] index" */
		if (buf[2] == 'V')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->v_max = max;

			/* Next... */
			continue;
		}


		/* Process 'P' for "Maximum p_info[] index" */
		if (buf[2] == 'P')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->p_max = max;

			/* Next... */
			continue;
		}


		/* Process 'H' for "Maximum h_info[] index" */
		if (buf[2] == 'H')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->h_max = max;

			/* Next... */
			continue;
		}


		/* Process 'B' for "Maximum b_info[] subindex" */
		if (buf[2] == 'B')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->b_max = max;

			/* Next... */
			continue;
		}


		/* Process 'O' for "Maximum o_list[] index" */
		if (buf[2] == 'O')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->o_max = max;

			/* Next... */
			continue;
		}


		/* Process 'M' for "Maximum m_list[] index" */
		if (buf[2] == 'M')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->m_max = max;

			/* Next... */
			continue;
		}


		/* Process 'N' for "Fake name size" */
		if (buf[2] == 'N')
		{
			long max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%ld", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->fake_name_size = max;

			/* Next... */
			continue;
		}


		/* Process 'T' for "Fake text size" */
		if (buf[2] == 'T')
		{
			long max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%ld", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->fake_text_size = max;

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


/*
 * Initialize the "v_info" array, by parsing an ascii "template" file
 */
errr init_v_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	vault_type *v_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	v_head->name_size = 0;
	v_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf, "V:%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != v_head->v_major) ||
			    (v2 != v_head->v_minor) ||
			    (v3 != v_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= v_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			v_ptr = &v_info[i];

			/* Hack -- Verify space */
			if (v_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!v_ptr->name) v_ptr->name = ++v_head->name_size;

			/* Append chars to the name */
			strcpy(v_name + v_head->name_size, s);

			/* Advance the index */
			v_head->name_size += strlen(s);

			/* Next... */
			continue;
		}

		/* There better be a current v_ptr */
		if (!v_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (v_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!v_ptr->text) v_ptr->text = ++v_head->text_size;

			/* Append chars to the name */
			strcpy(v_text + v_head->text_size, s);

			/* Advance the index */
			v_head->text_size += strlen(s);

			/* Next... */
			continue;
		}


		/* Process 'X' for "Extra info" (one line only) */
		if (buf[0] == 'X')
		{
			int typ, rat, hgt, wid;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			                &typ, &rat, &hgt, &wid)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			v_ptr->typ = typ;
			v_ptr->rat = rat;
			v_ptr->hgt = hgt;
			v_ptr->wid = wid;

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++v_head->name_size;
	++v_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}



/*
 * Initialize the "f_info" array, by parsing an ascii "template" file
 */
errr init_f_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	feature_type *f_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	f_head->name_size = 0;
	f_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != f_head->v_major) ||
			    (v2 != f_head->v_minor) ||
			    (v3 != f_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= f_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			f_ptr = &f_info[i];

			/* Hack -- Verify space */
			if (f_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!f_ptr->name) f_ptr->name = ++f_head->name_size;

			/* Append chars to the name */
			strcpy(f_name + f_head->name_size, s);

			/* Advance the index */
			f_head->name_size += strlen(s);

			/* Default "mimic" */
			f_ptr->mimic = i;

			/* Next... */
			continue;
		}

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (f_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!f_ptr->text) f_ptr->text = ++f_head->text_size;

			/* Append chars to the name */
			strcpy(f_text + f_head->text_size, s);

			/* Advance the index */
			f_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif


		/* Process 'M' for "Mimic" (one line only) */
		if (buf[0] == 'M')
		{
			int mimic;

			/* Scan for the values */
			if (1 != sscanf(buf+2, "%d",
			                &mimic)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			f_ptr->mimic = mimic;

			/* Next... */
			continue;
		}


		/* Process 'G' for "Graphics" (one line only) */
		if (buf[0] == 'G')
		{
			int tmp;

			/* Paranoia */
			if (!buf[2]) return (PARSE_ERROR_GENERIC);
			if (!buf[3]) return (PARSE_ERROR_GENERIC);
			if (!buf[4]) return (PARSE_ERROR_GENERIC);

			/* Extract the attr */
			tmp = color_char_to_attr(buf[4]);

			/* Paranoia */
			if (tmp < 0) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			f_ptr->d_attr = tmp;
			f_ptr->d_char = buf[2];

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++f_head->name_size;
	++f_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}



/*
 * Grab one flag in an object_kind from a textual string
 */
static errr grab_one_kind_flag(object_kind *k_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			k_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			k_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			k_ptr->flags3 |= (1L << i);
			return (0);
		}
	}
    
    /* ~ check flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags4[i]))
		{
			k_ptr->flags4 |= (1L << i);
			return (0);
		}
	}    

	/* Oops */
	msg_format("Unknown object flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}



/*
 * Initialize the "k_info" array, by parsing an ascii "template" file
 */
errr init_k_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	object_kind *k_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	k_head->name_size = 0;
	k_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != k_head->v_major) ||
			    (v2 != k_head->v_minor) ||
			    (v3 != k_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= k_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			k_ptr = &k_info[i];

			/* Hack -- Verify space */
			if (k_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!k_ptr->name) k_ptr->name = ++k_head->name_size;

			/* Append chars to the name */
			strcpy(k_name + k_head->name_size, s);

			/* Advance the index */
			k_head->name_size += strlen(s);

			/* Next... */
			continue;
		}

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (k_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!k_ptr->text) k_ptr->text = ++k_head->text_size;

			/* Append chars to the name */
			strcpy(k_text + k_head->text_size, s);

			/* Advance the index */
			k_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif


		/* Process 'G' for "Graphics" (one line only) */
		if (buf[0] == 'G')
		{
			char sym;
			int tmp;

			/* Paranoia */
			if (!buf[2]) return (PARSE_ERROR_GENERIC);
			if (!buf[3]) return (PARSE_ERROR_GENERIC);
			if (!buf[4]) return (PARSE_ERROR_GENERIC);

			/* Extract the char */
			sym = buf[2];

			/* Extract the attr */
			tmp = color_char_to_attr(buf[4]);

			/* Paranoia */
			if (tmp < 0) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			k_ptr->d_attr = tmp;
			k_ptr->d_char = sym;

			/* Next... */
			continue;
		}

		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int tval, sval, pval;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &tval, &sval, &pval)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			k_ptr->tval = tval;
			k_ptr->sval = sval;
			k_ptr->pval = pval;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int level, extra, wgt;
			long cost;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			                &level, &extra, &wgt, &cost)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			k_ptr->level = level;
			k_ptr->extra = extra;
			k_ptr->weight = wgt;
			k_ptr->cost = cost;

			/* Next... */
			continue;
		}

		/* Process 'A' for "Allocation" (one line only) */
		if (buf[0] == 'A')
		{
			int i;

			/* XXX XXX XXX Simply read each number following a colon */
			for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i)
			{
				/* Default chance */
				k_ptr->chance[i] = 1;

				/* Store the attack damage index */
				k_ptr->locale[i] = atoi(s+1);

				/* Find the slash */
				t = strchr(s+1, '/');

				/* Find the next colon */
				s = strchr(s+1, ':');

				/* If the slash is "nearby", use it */
				if (t && (!s || t < s))
				{
					int chance = atoi(t+1);
					if (chance > 0) k_ptr->chance[i] = chance;
				}
			}

			/* Next... */
			continue;
		}

		/* Hack -- Process 'P' for "power" and such */
		if (buf[0] == 'P')
		{
			int ac, hd1, hd2, th, td, ta;

			/* Scan for the values */
			if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
			                &ac, &hd1, &hd2, &th, &td, &ta)) return (PARSE_ERROR_GENERIC);

			k_ptr->ac = ac;
			k_ptr->dd = hd1;
			k_ptr->ds = hd2;
			k_ptr->to_h = th;
			k_ptr->to_d = td;
			k_ptr->to_a =  ta;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while (*t == ' ' || *t == '|') t++;
				}

				/* Parse this entry */
				if (0 != grab_one_kind_flag(k_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++k_head->name_size;
	++k_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


/*
 * Grab one flag in an artifact_type from a textual string
 */
static errr grab_one_artifact_flag(artifact_type *a_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			a_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			a_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			a_ptr->flags3 |= (1L << i);
			return (0);
		}
	}
    
	/* ~ check flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags4[i]))
		{
			a_ptr->flags4 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown artifact flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_activation(artifact_type *a_ptr, cptr what)
{
	int i;

	/* Scan activations */
	for (i = 0; i < ACT_MAX ; i++)
	{
		if (streq(what, a_info_act[i]))
		{
			a_ptr->activation = i;
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown artifact activation '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}



/*
 * Initialize the "a_info" array, by parsing an ascii "template" file
 */
errr init_a_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	artifact_type *a_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != a_head->v_major) ||
			    (v2 != a_head->v_minor) ||
			    (v3 != a_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= a_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			a_ptr = &a_info[i];

			/* Hack -- Verify space */
			if (a_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!a_ptr->name) a_ptr->name = ++a_head->name_size;

			/* Append chars to the name */
			strcpy(a_name + a_head->name_size, s);

			/* Advance the index */
			a_head->name_size += strlen(s);

			/* Ignore everything */
			a_ptr->flags3 |= (TR3_IGNORE_MASK);

			/* Next... */
			continue;
		}

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (a_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!a_ptr->text) a_ptr->text = ++a_head->text_size;

			/* Append chars to the name */
			strcpy(a_text + a_head->text_size, s);

			/* Advance the index */
			a_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif

		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int tval, sval, pval;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &tval, &sval, &pval)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			a_ptr->tval = tval;
			a_ptr->sval = sval;
			a_ptr->pval = pval;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int level, rarity, wgt;
			long cost;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			                &level, &rarity, &wgt, &cost)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			a_ptr->level = level;
			a_ptr->rarity = rarity;
			a_ptr->weight = wgt;
			a_ptr->cost = cost;

			/* Next... */
			continue;
		}

		/* Process 'P' for "power" and such */
		if (buf[0] == 'P')
		{
			int ac, hd1, hd2, th, td, ta;

			/* Scan for the values */
			if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
			                &ac, &hd1, &hd2, &th, &td, &ta)) return (PARSE_ERROR_GENERIC);

			a_ptr->ac = ac;
			a_ptr->dd = hd1;
			a_ptr->ds = hd2;
			a_ptr->to_h = th;
			a_ptr->to_d = td;
			a_ptr->to_a =  ta;

			/* Next... */
			continue;
		}

		/* Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* Parse this entry */
				if (0 != grab_one_artifact_flag(a_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Process 'A' for "Activation & time" */
		if (buf[0] == 'A')
		{
			int time, rand;

			/* Find the colon before the name */
			s = strchr(buf + 2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the activation */
			grab_one_activation(a_ptr, buf + 2);

			/* Scan for the values */
			if (2 != sscanf(s, "%d:%d",
			                &time, &rand)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			a_ptr->time = time;
			a_ptr->randtime = rand;

			/* Next... */
			continue;
		}

		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++a_head->name_size;
	++a_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


/*
 * Grab one flag in a ego-item_type from a textual string
 */
static bool grab_one_ego_item_flag(ego_item_type *e_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			e_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			e_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			e_ptr->flags3 |= (1L << i);
			return (0);
		}
	}
    
	/* ~ check flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags4[i]))
		{
			e_ptr->flags4 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown ego-item flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}




/*
 * Initialize the "e_info" array, by parsing an ascii "template" file
 */
errr init_e_info_txt(FILE *fp, char *buf)
{
	int i;

	int cur_t = 0;

	char *s, *t;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	ego_item_type *e_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != e_head->v_major) ||
			    (v2 != e_head->v_minor) ||
			    (v3 != e_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= e_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			e_ptr = &e_info[i];

			/* Hack -- Verify space */
			if (e_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!e_ptr->name) e_ptr->name = ++e_head->name_size;

			/* Append chars to the name */
			strcpy(e_name + e_head->name_size, s);

			/* Advance the index */
			e_head->name_size += strlen(s);

			/* Start with the first of the tval indices */
			cur_t = 0;

			/* Next... */
			continue;
		}

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (e_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!e_ptr->text) e_ptr->text = ++e_head->text_size;

			/* Append chars to the name */
			strcpy(e_text + e_head->text_size, s);

			/* Advance the index */
			e_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int level, rarity, pad2;
			long cost;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			                &level, &rarity, &pad2, &cost)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			e_ptr->level = level;
			e_ptr->rarity = rarity;
			/* e_ptr->weight = wgt; */
			e_ptr->cost = cost;

			/* Next... */
			continue;
		}


		/* Process 'X' for "Xtra" (one line only) */
		if (buf[0] == 'X')
		{
			int slot, rating, xtra;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &slot, &rating, &xtra)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			e_ptr->slot = slot;
			e_ptr->rating = rating;
			e_ptr->xtra = xtra;

			/* Next... */
			continue;
		}

		/* Process 'T' for "Types allowed" (up to three lines) */
		if (buf[0] == 'T')
		{
			int tval, sval1, sval2;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &tval, &sval1, &sval2)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			e_ptr->tval[cur_t] = (byte)tval;
			e_ptr->min_sval[cur_t] = (byte)sval1;
			e_ptr->max_sval[cur_t] = (byte)sval2;

			/* increase counter for 'possible tval' index */
			cur_t++;

			/* only three T: lines allowed */
			if (cur_t > 3) return (PARSE_ERROR_GENERIC);

			/* Next... */
			continue;
		}

		/* Hack -- Process 'C' for "creation" */
		if (buf[0] == 'C')
		{
			int th, td, ta, pv;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			                &th, &td, &ta, &pv)) return (PARSE_ERROR_GENERIC);

			e_ptr->max_to_h = th;
			e_ptr->max_to_d = td;
			e_ptr->max_to_a = ta;
			e_ptr->max_pval = pv;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* Parse this entry */
				if (0 != grab_one_ego_item_flag(e_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++e_head->name_size;
	++e_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
static errr grab_one_basic_flag(monster_race *r_ptr, cptr what)
{
	int i;

	/* Scan flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags1[i]))
		{
			r_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags2[i]))
		{
			r_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags3[i]))
		{
			r_ptr->flags3 |= (1L << i);
			return (0);
		}
	}
    
	/* ~ scan flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags4[i]))
		{
			r_ptr->flags4 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_spell_flag(monster_race *r_ptr, cptr what)
{
	int i;

	/* Scan flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags4[i]))
		{
			r_ptr->flags4 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags5 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags5[i]))
		{
			r_ptr->flags5 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags6 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags6[i]))
		{
			r_ptr->flags6 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
	msg_format("Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}




/*
 * Initialize the "r_info" array, by parsing an ascii "template" file
 */
errr init_r_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	monster_race *r_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Start the "fake" stuff */
	r_head->name_size = 0;
	r_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != r_head->v_major) ||
			    (v2 != r_head->v_minor) ||
			    (v3 != r_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= r_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			r_ptr = &r_info[i];

			/* Hack -- Verify space */
			if (r_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!r_ptr->name) r_ptr->name = ++r_head->name_size;

			/* Append chars to the name */
			strcpy(r_name + r_head->name_size, s);

			/* Advance the index */
			r_head->name_size += strlen(s);

			/* Next... */
			continue;
		}

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (r_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!r_ptr->text) r_ptr->text = ++r_head->text_size;

			/* Append chars to the name */
			strcpy(r_text + r_head->text_size, s);

			/* Advance the index */
			r_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

		/* Process 'G' for "Graphics" (one line only) */
		if (buf[0] == 'G')
		{
			char sym;
			int tmp;

			/* Paranoia */
			if (!buf[2]) return (PARSE_ERROR_GENERIC);
			if (!buf[3]) return (PARSE_ERROR_GENERIC);
			if (!buf[4]) return (PARSE_ERROR_GENERIC);

			/* Extract the char */
			sym = buf[2];

			/* Extract the attr */
			tmp = color_char_to_attr(buf[4]);

			/* Paranoia */
			if (tmp < 0) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			r_ptr->d_attr = tmp;
			r_ptr->d_char = sym;

			/* Next... */
			continue;
		}

		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int spd, hp1, hp2, aaf, ac, slp;

			/* Scan for the other values */
			if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
			                &spd, &hp1, &hp2, &aaf, &ac, &slp)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			r_ptr->speed = spd;
			r_ptr->hdice = hp1;
			r_ptr->hside = hp2;
			r_ptr->aaf = aaf;
			r_ptr->ac = ac;
			r_ptr->sleep = slp;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int lev, rar, pad;
			long exp;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			                &lev, &rar, &pad, &exp)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			r_ptr->level = lev;
			r_ptr->rarity = rar;
			r_ptr->extra = pad;
			r_ptr->mexp = exp;

			/* Next... */
			continue;
		}

		/* Process 'B' for "Blows" (up to four lines) */
		if (buf[0] == 'B')
		{
			int n1, n2;

			/* Find the next empty blow slot (if any) */
			for (i = 0; i < 4; i++) if (!r_ptr->blow[i].method) break;

			/* Oops, no more slots */
			if (i == 4) return (PARSE_ERROR_GENERIC);

			/* Analyze the first field */
			for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

			/* Terminate the field (if necessary) */
			if (*t == ':') *t++ = '\0';

			/* Analyze the method */
			for (n1 = 0; r_info_blow_method[n1]; n1++)
			{
				if (streq(s, r_info_blow_method[n1])) break;
			}

			/* Invalid method */
			if (!r_info_blow_method[n1]) return (PARSE_ERROR_GENERIC);

			/* Analyze the second field */
			for (s = t; *t && (*t != ':'); t++) /* loop */;

			/* Terminate the field (if necessary) */
			if (*t == ':') *t++ = '\0';

			/* Analyze effect */
			for (n2 = 0; r_info_blow_effect[n2]; n2++)
			{
				if (streq(s, r_info_blow_effect[n2])) break;
			}

			/* Invalid effect */
			if (!r_info_blow_effect[n2]) return (PARSE_ERROR_GENERIC);

			/* Analyze the third field */
			for (s = t; *t && (*t != 'd'); t++) /* loop */;

			/* Terminate the field (if necessary) */
			if (*t == 'd') *t++ = '\0';

			/* Save the method */
			r_ptr->blow[i].method = n1;

			/* Save the effect */
			r_ptr->blow[i].effect = n2;

			/* Extract the damage dice and sides */
			r_ptr->blow[i].d_dice = atoi(s);
			r_ptr->blow[i].d_side = atoi(t);

			/* Next... */
			continue;
		}

		/* Process 'F' for "Basic Flags" (multiple lines) */
		if (buf[0] == 'F')
		{
			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while (*t == ' ' || *t == '|') t++;
				}

				/* Parse this entry */
				if (0 != grab_one_basic_flag(r_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Process 'S' for "Spell Flags" (multiple lines) */
		if (buf[0] == 'S')
		{
			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* XXX XXX XXX Hack -- Read spell frequency */
				if (1 == sscanf(s, "1_IN_%d", &i))
				{
					/* Extract a "frequency" */
					r_ptr->freq_spell = r_ptr->freq_inate = 100 / i;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

				/* Parse this entry */
				if (0 != grab_one_spell_flag(r_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++r_head->name_size;
	++r_head->text_size;


	/* XXX XXX XXX The ghost is unused */

	/* Mega-Hack -- acquire "ghost" */
	r_ptr = &r_info[z_info->r_max-1];

	/* Get the next index */
	r_ptr->name = r_head->name_size;
	r_ptr->text = r_head->text_size;

	/* Save some space for the ghost info */
	r_head->name_size += 64;
	r_head->text_size += 64;

	/* Hack -- Default name/text for the ghost */
	strcpy(r_name + r_ptr->name, "Nobody, the Undefined Ghost");
	strcpy(r_text + r_ptr->text, "It seems strangely familiar...");

	/* Hack -- set the attr/char info */
	r_ptr->d_attr = r_ptr->x_attr = TERM_WHITE;
	r_ptr->d_char = r_ptr->x_char = 'G';

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->flags1 |= (RF1_UNIQUE);

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->flags1 |= (RF1_NEVER_MOVE | RF1_NEVER_BLOW);

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->hdice = r_ptr->hside = 1;

	/* Hack -- Try to prevent a few "potential" bugs */
	r_ptr->mexp = 1L;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


/*
 * Grab one flag in a player_race from a textual string
 */
static errr grab_one_racial_flag(player_race *pr_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			pr_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			pr_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			pr_ptr->flags3 |= (1L << i);
			return (0);
		}
	}

	/* ~ check flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags4[i]))
		{
			pr_ptr->flags4 |= (1L << i);
			return (0);
		}
	}
    
	/* Oops */
	msg_format("Unknown player flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}



/*
 * Initialize the "p_info" array, by parsing an ascii "template" file
 */
errr init_p_info_txt(FILE *fp, char *buf)
{
	int i, j;

	char *s, *t;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	player_race *pr_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != p_head->v_major) ||
			    (v2 != p_head->v_minor) ||
			    (v3 != p_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (PARSE_ERROR_GENERIC);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= p_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			pr_ptr = &p_info[i];

			/* Hack -- Verify space */
			if (p_head->name_size + strlen(s) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!pr_ptr->name) pr_ptr->name = ++p_head->name_size;

			/* Append chars to the name */
			strcpy(p_name + p_head->name_size, s);

			/* Advance the index */
			p_head->name_size += strlen(s);

			/* Next... */
			continue;
		}

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


		/* Process 'S' for "Stats" (one line only) */
		if (buf[0] == 'S')
		{
			int adj;

			/* Start the string */
			s = buf+1;

			/* For each stat */
			for (j = 0; j < A_MAX; j++)
			{
				/* Find the colon before the subindex */
				s = strchr(s, ':');

				/* Verify that colon */
				if (!s) return (PARSE_ERROR_GENERIC);

				/* Nuke the colon, advance to the subindex */
				*s++ = '\0';

				/* Get the value */
				adj = atoi(s);

				/* Save the value */
				pr_ptr->r_adj[j] = adj;

				/* Next... */
				continue;
			}

			/* Next... */
			continue;
		}

		/* Process 'R' for "Racial Skills" (one line only) */
		if (buf[0] == 'R')
		{
			int dis, dev, sav, stl, srh, fos, thn, thb;

			/* Scan for the values */
			if (8 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d",
			                &dis, &dev, &sav, &stl,
			                &srh, &fos, &thn, &thb)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			pr_ptr->r_dis = dis;
			pr_ptr->r_dev = dev;
			pr_ptr->r_sav = sav;
			pr_ptr->r_stl = stl;
			pr_ptr->r_srh = srh;
			pr_ptr->r_fos = fos;
			pr_ptr->r_thn = thn;
			pr_ptr->r_thb = thb;

			/* Next... */
			continue;
		}

		/* Process 'X' for "Extra Info" (one line only) */
		if (buf[0] == 'X')
		{
			int mhp, exp, infra;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &mhp, &exp, &infra)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			pr_ptr->r_mhp = mhp;
			pr_ptr->r_exp = exp;
			pr_ptr->infra = infra;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'I' for "info" and such */
		if (buf[0] == 'I')
		{
			int hist, b_age, m_age;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &hist, &b_age, &m_age)) return (PARSE_ERROR_GENERIC);

			pr_ptr->hist = hist;
			pr_ptr->b_age = b_age;
			pr_ptr->m_age = m_age;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'I' for "info" and such */
		if (buf[0] == 'I')
		{
			int hist, b_age, m_age;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
			                &hist, &b_age, &m_age)) return (PARSE_ERROR_GENERIC);

			pr_ptr->hist = hist;
			pr_ptr->b_age = b_age;
			pr_ptr->m_age = m_age;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'H' for "Height" */
		if (buf[0] == 'H')
		{
			int m_b_ht, m_m_ht, f_b_ht, f_m_ht;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			                &m_b_ht, &m_m_ht, &f_b_ht, &f_m_ht)) return (PARSE_ERROR_GENERIC);

			pr_ptr->m_b_ht = m_b_ht;
			pr_ptr->m_m_ht = m_m_ht;
			pr_ptr->f_b_ht = f_b_ht;
			pr_ptr->f_m_ht = f_m_ht;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'W' for "Weight" */
		if (buf[0] == 'W')
		{
			int m_b_wt, m_m_wt, f_b_wt, f_m_wt;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			                &m_b_wt, &m_m_wt, &f_b_wt, &f_m_wt)) return (PARSE_ERROR_GENERIC);

			pr_ptr->m_b_wt = m_b_wt;
			pr_ptr->m_m_wt = m_m_wt;
			pr_ptr->f_b_wt = f_b_wt;
			pr_ptr->f_m_wt = f_m_wt;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* Parse this entry */
				if (0 != grab_one_racial_flag(pr_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Hack -- Process 'C' for class choices */
		if (buf[0] == 'C')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* Hack - Parse this entry */
				pr_ptr->choice |= (1 << atoi(s));

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++p_head->name_size;
	++p_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}




/*
 * Initialize the "h_info" array, by parsing an ascii "template" file
 */
errr init_h_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	hist_type *h_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	h_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf, "V:%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != h_head->v_major) ||
			    (v2 != h_head->v_minor) ||
			    (v3 != h_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number" */
		if (buf[0] == 'N')
		{
			int prv, nxt, prc, soc;

			/* Hack - get the index */
			i = error_idx + 1;

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= h_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			h_ptr = &h_info[i];

			/* Scan for the values */
			if (4 != sscanf(buf, "N:%d:%d:%d:%d",
			                &prv, &nxt, &prc, &soc)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			h_ptr->chart = prv;
			h_ptr->next = nxt;
			h_ptr->roll = prc;
			h_ptr->bonus = soc;

			/* Next... */
			continue;
		}

		/* There better be a current h_ptr */
		if (!h_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Get the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (h_head->text_size + strlen(s) + 8 > z_info->fake_text_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the text index */
			if (!h_ptr->text) h_ptr->text = ++h_head->text_size;

			/* Append chars to the name */
			strcpy(h_text + h_head->text_size, s);

			/* Advance the index */
			h_head->text_size += strlen(s);

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "text" size */
	++h_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}




/*
 * Initialize the "b_info" array, by parsing an ascii "template" file
 */
errr init_b_info_txt(FILE *fp, char *buf)
{
	int i, j;

	char *s, *t;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	owner_type *ot_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	b_head->name_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf, "V:%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != b_head->v_major) ||
			    (v2 != b_head->v_minor) ||
			    (v3 != b_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the subindex */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the index */
			i = atoi(buf+2);

			/* Find the colon before the name */
			t = strchr(s, ':');

			/* Verify that colon */
			if (!t) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the name */
			*t++ = '\0';

			/* Paranoia -- require a name */
			if (!*t) return (PARSE_ERROR_GENERIC);

			/* Get the subindex */
			j = atoi(s);

			/* Verify information */
			if (j >= z_info->b_max) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Get the *real* index */
			i = (i * z_info->b_max) + j;

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= b_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			ot_ptr = &b_info[i];

			/* Hack -- Verify space */
			if (b_head->name_size + strlen(t) + 8 > z_info->fake_name_size)
				return (PARSE_ERROR_OUT_OF_MEMORY);

			/* Advance and Save the name index */
			if (!ot_ptr->owner_name) ot_ptr->owner_name = ++b_head->name_size;

			/* Append chars to the name */
			strcpy(b_name + b_head->name_size, t);

			/* Advance the index */
			b_head->name_size += strlen(t);

			/* Next... */
			continue;
		}

		/* There better be a current ot_ptr */
		if (!ot_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);


		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int idx, gld, max, min, hgl, tol;

			/* Scan for the values */
			if (6 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d",
			                &idx, &gld, &max, &min, &hgl, &tol)) return (PARSE_ERROR_GENERIC);

			/* Save the values */
			ot_ptr->owner_race = idx;
			ot_ptr->max_cost = gld;
			ot_ptr->max_inflate = max;
			ot_ptr->min_inflate = min;
			ot_ptr->haggle_per = hgl;
			ot_ptr->insult_max = tol;

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "name" and "text" sizes */
	++b_head->name_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}




/*
 * Initialize the "g_info" array, by parsing an ascii "template" file
 */
errr init_g_info_txt(FILE *fp, char *buf)
{
	int i, j;

	char *s;

	/* Not ready yet */
	bool okay = FALSE;

	/* Current entry */
	byte *g_ptr;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	g_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf, "V:%d.%d.%d", &v1, &v2, &v3)) ||
			    (v1 != g_head->v_major) ||
			    (v2 != g_head->v_minor) ||
			    (v3 != g_head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


		/* Process 'A' for "Adjustments" */
		if (buf[0] == 'A')
		{
			int adj;

			/* Start the string */
			s = buf+1;

			/* Initialize the counter to max races */
			j = z_info->p_max;

			/* Repeat */
			while (j-- > 0)
			{
				/* Hack - get the index */
				i = error_idx + 1;

				/* Verify information */
				if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

				/* Verify information */
				if (i >= g_head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

				/* Save the index */
				error_idx = i;

				/* Point at the "info" */
				g_ptr = &g_info[i];

				/* Find the colon before the subindex */
				s = strchr(s, ':');

				/* Verify that colon */
				if (!s) return (PARSE_ERROR_GENERIC);

				/* Nuke the colon, advance to the subindex */
				*s++ = '\0';

				/* Get the value */
				adj = atoi(s);

				/* Save the value */
				*g_ptr = adj;

				/* Next... */
				continue;
			}

			/* Next... */
			continue;
		}


		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}


	/* Complete the "text" size */
	++g_head->text_size;


	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);


	/* Success */
	return (0);
}


#else	/* ALLOW_TEMPLATES */

#ifdef MACINTOSH
static int i = 0;
#endif

#endif	/* ALLOW_TEMPLATES */
