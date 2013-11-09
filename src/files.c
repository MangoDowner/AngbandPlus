/* File: files.c */

/* Purpose: code dealing with files (and death) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * You may or may not want to use the following "#undef".
 */
/* #undef _POSIX_SAVED_IDS */


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(getuid()) != 0)
	{
#ifdef JP
		quit("setuid(): ���������Ĥ����ޤ���");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(getgid()) != 0)
	{
#ifdef JP
		quit("setgid(): ���������Ĥ����ޤ���");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
		quit("setreuid(): ���������Ĥ����ޤ���");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
		quit("setregid(): ���������Ĥ����ޤ���");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif

# endif

#endif

}


/*
 * Hack -- grab permissions
 */
void safe_setuid_grab(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(player_euid) != 0)
	{
#ifdef JP
		quit("setuid(): ���������Ĥ����ޤ���");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(player_egid) != 0)
	{
#ifdef JP
		quit("setgid(): ���������Ĥ����ޤ���");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
		quit("setreuid(): ���������Ĥ����ޤ���");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
		quit("setregid(): ���������Ĥ����ޤ���");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif /* SAFE_SETUID_POSIX */

# endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Extract the first few "tokens" from a buffer
 *
 * This function uses "colon" and "slash" as the delimeter characters.
 *
 * We never extract more than "num" tokens.  The "last" token may include
 * "delimeter" characters, allowing the buffer to include a "string" token.
 *
 * We save pointers to the tokens in "tokens", and return the number found.
 *
 * Hack -- Attempt to handle the 'c' character formalism
 *
 * Hack -- An empty buffer, or a final delimeter, yields an "empty" token.
 *
 * Hack -- We will always extract at least one token
 */
s16b tokenize(char *buf, s16b num, char **tokens, int mode)
{
	int i = 0;

	char *s = buf;


	/* Process */
	while (i < num - 1)
	{
		char *t;

		/* Scan the string */
		for (t = s; *t; t++)
		{
			/* Found a delimiter */
			if ((*t == ':') || (*t == '/')) break;

			/* Handle single quotes */
			if ((mode & TOKENIZE_CHECKQUOTE) && (*t == '\''))
			{
				/* Advance */
				t++;

				/* Handle backslash */
				if (*t == '\\') t++;

				/* Require a character */
				if (!*t) break;

				/* Advance */
				t++;

				/* Hack -- Require a close quote */
				if (*t != '\'') *t = '\'';
			}

			/* Handle back-slash */
			if (*t == '\\') t++;
		}

		/* Nothing left */
		if (!*t) break;

		/* Nuke and advance */
		*t++ = '\0';

		/* Save the token */
		tokens[i++] = s;

		/* Advance */
		s = t;
	}

	/* Save the token */
	tokens[i++] = s;

	/* Number found */
	return (i);
}


/* A number with a name */
typedef struct named_num named_num;

struct named_num
{
	cptr name;		/* The name of this thing */
	int num;			/* A number associated with it */
};


/* Index of spell type names */
static named_num gf_desc[] =
{
	{"GF_ACID", 				GF_ACID				},
	{"GF_ELEC", 				GF_ELEC				},
	{"GF_FIRE",		 			GF_FIRE				},
	{"GF_COLD", 				GF_COLD				},
	{"GF_POIS", 				GF_POIS				},
	{"GF_LITE",					GF_LITE				},
	{"GF_DARK",					GF_DARK				},
	{"GF_LITE_WEAK",			GF_LITE_WEAK		},
	{"GF_DARK_WEAK",			GF_DARK_WEAK		},
	{"GF_NETHER",				GF_NETHER			},
	{"GF_WATER",				GF_WATER			},
	{"GF_PLASMA",				GF_PLASMA			},
	{"GF_SHARDS",				GF_SHARDS			},
	{"GF_SOUND",				GF_SOUND			},
	{"GF_CONFUSION",			GF_CONFUSION		},
	{"GF_CHAOS",				GF_CHAOS			},
	{"GF_STONE",				GF_STONE			},
	{"GF_DISENCHANT",			GF_DISENCHANT		},
	{"GF_FORCE",				GF_FORCE			},
	{"GF_INERTIA",				GF_INERTIA			},
	{"GF_TIME",					GF_TIME				},
	{"GF_GRAVITY",				GF_GRAVITY			},
	{"GF_ICE",					GF_ICE				},
	{"GF_NUKE",					GF_NUKE				},
	{"GF_ROCKET",				GF_ROCKET			},
	{"GF_MISSILE",				GF_MISSILE			},
	{"GF_PHYSICAL",				GF_PHYSICAL			},
	{"GF_BLUNT",				GF_BLUNT			},
	{"GF_EDGED",				GF_EDGED			},
	{"GF_MANA",					GF_MANA				},
	{"GF_METEOR",				GF_METEOR			},
	{"GF_VOLCANIC_BOMB",		GF_VOLCANIC_BOMB	},
	{"GF_DISINTEGRATE",			GF_DISINTEGRATE		},
	{"GF_HOLY_FIRE",			GF_HOLY_FIRE		},
	{"GF_HELL_FIRE",			GF_HELL_FIRE		},
	{"GF_GODLY_SPEAR",			GF_GODLY_SPEAR		},
	{"GF_PURE_FIRE",			GF_PURE_FIRE		},
	{"GF_PURE_AQUA",			GF_PURE_AQUA		},
	{"GF_PURE_EARTH",			GF_PURE_EARTH		},
	{"GF_PURE_WIND",			GF_PURE_WIND		},
	{"GF_CAUSE_1",				GF_CAUSE_1			},
	{"GF_CAUSE_2",				GF_CAUSE_2			},
	{"GF_CAUSE_3",				GF_CAUSE_3			},
	{"GF_CAUSE_4",				GF_CAUSE_4			},
	{"GF_HAND_DOOM",			GF_HAND_DOOM		},
	{"GF_DEATH_RAY",			GF_DEATH_RAY		},
	{"GF_PSI",					GF_PSI				},
	{"GF_MIND_BLAST",			GF_MIND_BLAST		},
	{"GF_BRAIN_SMASH",			GF_BRAIN_SMASH		},
	{"GF_KILL_WALL",			GF_KILL_WALL		},
	{"GF_KILL_DOOR",			GF_KILL_DOOR		},
	{"GF_KILL_TRAP",			GF_KILL_TRAP		},
	{"GF_MAKE_TRAP",			GF_MAKE_TRAP		},
	{"GF_MAKE_TREE",			GF_MAKE_TREE		},
	{"GF_MAKE_GLYPH",			GF_MAKE_GLYPH		},
	{"GF_OLD_CLONE",			GF_OLD_CLONE		},
	{"GF_OLD_POLY",				GF_OLD_POLY			},
	{"GF_OLD_HEAL",				GF_OLD_HEAL			},
	{"GF_OLD_SPEED",			GF_OLD_SPEED		},
	{"GF_OLD_SLOW",				GF_OLD_SLOW			},
	{"GF_OLD_CONF",				GF_OLD_CONF			},
	{"GF_OLD_SLEEP",			GF_OLD_SLEEP		},
	{"GF_OLD_DRAIN",			GF_OLD_DRAIN		},
	{"GF_OLD_STONE",			GF_OLD_STONE		},
	{"GF_STAR_HEAL",			GF_STAR_HEAL		},
	{"GF_NEW_SLOW",				GF_NEW_SLOW			},
	{"GF_NEW_DRAIN",			GF_NEW_DRAIN		},
	{"GF_STASIS",				GF_STASIS			},
	{"GF_STASIS_EVIL",			GF_STASIS_EVIL		},
	{"GF_STUN",					GF_STUN				},
	{"GF_CHARM",				GF_CHARM			},
	{"GF_SILENT",				GF_SILENT			},
	{"GF_CRUSADE",				GF_CRUSADE			},
	{"GF_AWAY_EVIL",			GF_AWAY_EVIL		},
	{"GF_AWAY_ALL",				GF_AWAY_ALL			},
	{"GF_TURN_ALL",				GF_TURN_ALL			},
	{"GF_DISP_UNDEAD",			GF_DISP_UNDEAD		},
	{"GF_DISP_EVIL",			GF_DISP_EVIL		},
	{"GF_DISP_GOOD",			GF_DISP_GOOD		},
	{"GF_DISP_ALL",				GF_DISP_ALL			},
	{"GF_CONTROL_UNDEAD",		GF_CONTROL_UNDEAD	},
	{"GF_CONTROL_ANIMAL",		GF_CONTROL_ANIMAL	},
	{"GF_CONTROL_BEAST",		GF_CONTROL_BEAST	},
	{"GF_CONTROL_DRAGON",		GF_CONTROL_DRAGON	},
	{"GF_GENOCIDE",				GF_GENOCIDE			},
	{"GF_GENOCIDE_UNDEAD",		GF_GENOCIDE_UNDEAD	},
	{"GF_DRAIN_MANA",			GF_DRAIN_MANA		},
	{"GF_ANIM_DEAD",			GF_ANIM_DEAD		},
	{"GF_ATTACK",				GF_ATTACK			},
	{"GF_PHOTO",				GF_PHOTO			},
	{"GF_CAPTURE",				GF_CAPTURE			},
	{"GF_STRIKE_NOVA",			GF_STRIKE_NOVA		},
	{"GF_WORD_OF_PAIN",			GF_WORD_OF_PAIN		},
	{"GF_SHINING",				GF_SHINING			},
	{"GF_SPECIAL_STONE",		GF_SPECIAL_STONE	},
	{"GF_DUAL_DRAIN",			GF_DUAL_DRAIN		},
	{"GF_DRAIN_SOUL",			GF_DRAIN_SOUL		},
	{"GF_ERASE_ELEM",			GF_ERASE_ELEM		},
	{"GF_CAVE_TEMP",			GF_CAVE_TEMP		},
	{"GF_WATER_FLOW",			GF_WATER_FLOW		},
	{"GF_LAVA_FLOW",			GF_LAVA_FLOW		},
	{NULL, 						0					}
};


/*
 * Parse a sub-file of the "extra info" (format shown below)
 *
 * Each "action" line has an "action symbol" in the first column,
 * followed by a colon, followed by some command specific info,
 * usually in the form of "tokens" separated by colons or slashes.
 *
 * Blank lines, lines starting with white space, and lines starting
 * with pound signs ("#") are ignored (as comments).
 *
 * Note the use of "tokenize()" to allow the use of both colons and
 * slashes as delimeters, while still allowing final tokens which
 * may contain any characters including "delimiters".
 *
 * Note the use of "strtol()" to allow all "integers" to be encoded
 * in decimal, hexidecimal, or octal form.
 *
 * Note that "monster zero" is used for the "player" attr/char, "object
 * zero" will be used for the "stack" attr/char, and "feature zero" is
 * used for the "nothing" attr/char.
 *
 * Parse another file recursively, see below for details
 *   %:<filename>
 *
 * Specify the attr/char values for "monsters" by race index
 *   R:<num>:<a>:<c>
 *
 * Specify the attr/char values for "objects" by kind index
 *   K:<num>:<a>:<c>
 *
 * Specify the attr/char values for "features" by feature index
 *   F:<num>:<a>:<c>
 *
 * Specify the attr/char values for unaware "objects" by kind tval
 *   U:<tv>:<a>:<c>
 *
 * Specify the attr/char values for inventory "objects" by kind tval
 *   E:<tv>:<a>:<c>
 *
 * Define a macro action, given an encoded macro action
 *   A:<str>
 *
 * Create a normal macro, given an encoded macro trigger
 *   P:<str>
 *
 * Create a command macro, given an encoded macro trigger
 *   C:<str>
 *
 * Create a keyset mapping
 *   S:<key>:<key>:<dir>
 *
 * Turn an option off, given its name
 *   X:<str>
 *
 * Turn an option on, given its name
 *   Y:<str>
 *
 * Specify visual information, given an index, and some data
 *   V:<num>:<kv>:<rv>:<gv>:<bv>
 *
 * Specify the set of colors to use when drawing a zapped spell
 *   Z:<type>:<str>
 *
 * Specify a macro trigger template and macro trigger names.
 *   T:<template>:<modifier chr>:<modifier name1>:<modifier name2>:...
 *   T:<trigger>:<keycode>:<shift-keycode>
 *
 */

errr process_pref_file_command(char *buf)
{
	int i, j, n1, n2;

	char *zz[16];


	/* Require "?:*" format */
	if (buf[1] != ':') return (1);


	switch (buf[0])
	{
	/* Process "R:<num>:<a>/<c>" -- attr/char for monster races */
	case 'R':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			monster_race *r_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= (max_r_idx + runeweapon_num)) return (1);
			r_ptr = &r_info[i];
			if (n1) r_ptr->x_attr = n1;
			if (n2) r_ptr->x_char = n2;
			return (0);
		}
		break;

	/* Process "K:<num>:<a>/<c>"  -- attr/char for object kinds */
	case 'K':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			object_kind *k_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_k_idx) return (1);
			k_ptr = &k_info[i];
			if (n1) k_ptr->x_attr = n1;
			if (n2) k_ptr->x_char = n2;
			return (0);
		}
		break;

	/* Process "F:<num>:<a>/<c>" -- attr/char for terrain features */
	case 'F':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			feature_type *f_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_f_idx) return (1);
			f_ptr = &f_info[i];
			if (n1) f_ptr->x_attr = n1;
			if (n2) f_ptr->x_char = n2;
			return (0);
		}
		break;

	/* Process "S:<num>:<a>/<c>" -- attr/char for special things */
	case 'S':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (byte)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			misc_to_attr[j] = n1;
			misc_to_char[j] = n2;
			return (0);
		}
		break;

	/* Process "U:<tv>:<a>/<c>" -- attr/char for unaware items */
	case 'U':
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			for (i = 1; i < max_k_idx; i++)
			{
				object_kind *k_ptr = &k_info[i];
				if (k_ptr->tval == j)
				{
					if (n1) k_ptr->d_attr = n1;
					if (n2) k_ptr->d_char = n2;
				}
			}
			return (0);
		}
	break;

	/* Process "E:<tv>:<a>" -- attribute for inventory objects */
	case 'E':
		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			j = (byte)strtol(zz[0], NULL, 0) % 128;
			n1 = strtol(zz[1], NULL, 0);
			if (n1) tval_to_attr[j] = n1;
			return (0);
		}
	break;


	/* Process "A:<str>" -- save an "action" for later */
	case 'A':
		text_to_ascii(macro__buf, buf+2);
		return (0);

	/* Process "P:<str>" -- normal macro */
	case 'P':
	{
		char tmp[1024];
		text_to_ascii(tmp, buf+2);
		macro_add(tmp, macro__buf);
		return (0);
	}


	/* Process "C:<str>" -- create keymap */
	case 'C':
	{
		int mode;

		char tmp[1024];

		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) != 2) return (1);

		mode = strtol(zz[0], NULL, 0);
		if ((mode < 0) || (mode >= KEYMAP_MODES)) return (1);

		text_to_ascii(tmp, zz[1]);
		if (!tmp[0] || tmp[1]) return (1);
		i = (byte)(tmp[0]);

		string_free(keymap_act[mode][i]);

		keymap_act[mode][i] = string_make(macro__buf);

		return (0);
	}


	/* Process "V:<num>:<kv>:<rv>:<gv>:<bv>" -- visual info */
	case 'V':
		if (tokenize(buf+2, 5, zz, TOKENIZE_CHECKQUOTE) == 5)
		{
			i = (byte)strtol(zz[0], NULL, 0);
			angband_color_table[i][0] = (byte)strtol(zz[1], NULL, 0);
			angband_color_table[i][1] = (byte)strtol(zz[2], NULL, 0);
			angband_color_table[i][2] = (byte)strtol(zz[3], NULL, 0);
			angband_color_table[i][3] = (byte)strtol(zz[4], NULL, 0);
			return (0);
		}
		break;

	/* Process "X:<str>" -- turn option off */
	case 'X':
	case 'Y':
		for (i = 0; option_info[i].o_desc; i++)
		{
			int os = option_info[i].o_set;
			int ob = option_info[i].o_bit;

			if (option_info[i].o_var &&
			    option_info[i].o_text &&
			    streq(option_info[i].o_text, buf + 2))
			{
				if (p_ptr->playing && OPT_PAGE_BIRTH == option_info[i].o_page && !p_ptr->wizard)
				{
#ifdef JP
					msg_format("������ץ������ѹ��Ǥ��ޤ���! '%s'", buf);	
#else
					msg_format("Startup options can not changed! '%s'", buf);	
#endif
					msg_print(NULL);
					return 0;
				}

				if (buf[0] == 'X')
				{
					/* Clear */
					option_flag[os] &= ~(1L << ob);
					(*option_info[i].o_var) = FALSE;
				}
				else
				{
					/* Set */
					option_flag[os] |= (1L << ob);
					(*option_info[i].o_var) = TRUE;
				}
				return 0;
			}
		}

		/* don't know that option. ignore it.*/
#ifdef JP
		msg_format("���ץ�����̾��������������ޤ��� %s", buf);
#else
		msg_format("Ignored invalid option: %s", buf);
#endif
		msg_print(NULL);
		return 0;

	/* Process "Z:<type>:<str>" -- set spell color */
	case 'Z':
	{
		/* Find the colon */
		char *t = my_strchr(buf + 2, ':');

		/* Oops */
		if (!t) return 1;

		/* Nuke the colon */
		*(t++) = '\0';

		for (i = 0; gf_desc[i].name; i++)
		{
			/* Match this type */
			if (streq(gf_desc[i].name, buf + 2))
			{
				/* Remember this color set */
				gf_color[gf_desc[i].num] = quark_add(t);

				/* Success */
				return 0;
			}
		}
		break;
	}

	/* Initialize macro trigger names and a template */
	/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
	/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
	case 'T':
	{
		int tok = tokenize(buf+2, 2+MAX_MACRO_MOD, zz, 0);

		/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
		if (tok >= 4)
		{
			int i;
			int num;

			if (macro_template != NULL)
			{
				num = strlen(macro_modifier_chr);

				/* Kill the template string */
				string_free(macro_template);
				macro_template = NULL;

				/* Kill flag characters of modifier keys */
				string_free(macro_modifier_chr);

				/* Kill corresponding modifier names */
				for (i = 0; i < num; i++)
				{
					string_free(macro_modifier_name[i]);
				}

				/* Kill trigger name strings */
				for (i = 0; i < max_macrotrigger; i++)
				{
					string_free(macro_trigger_name[i]);
					string_free(macro_trigger_keycode[0][i]);
					string_free(macro_trigger_keycode[1][i]);
				}

				max_macrotrigger = 0;
			}
			
			if (*zz[0] == '\0') return 0; /* clear template */

			/* Number of modifier flags */
			num = strlen(zz[1]);

			/* Limit the number */
			num = MIN(MAX_MACRO_MOD, num);

			/* Stop if number of modifier is not correct */
			if (2 + num != tok) return 1;

			/* Get a template string */
			macro_template = string_make(zz[0]);

			/* Get flag characters of modifier keys */
			macro_modifier_chr = string_make(zz[1]);

			/* Get corresponding modifier names */
			for (i = 0; i < num; i++)
			{
				macro_modifier_name[i] = string_make(zz[2+i]);
			}
		}

		/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
		else if (tok >= 2)
		{
			char buf[1024];
			int m;
			char *t, *s;
			if (max_macrotrigger >= MAX_MACRO_TRIG)
			{
#ifdef JP
				msg_print("�ޥ���ȥꥬ�������꤬¿�����ޤ�!");
#else
				msg_print("Too many macro triggers!");
#endif
				return 1;
			}
			m = max_macrotrigger;
			max_macrotrigger++;

			/* Take into account the escape character  */
			t = buf;
			s = zz[0];
			while (*s)
			{
				if ('\\' == *s) s++;
				*t++ = *s++;
			}
			*t = '\0';

			/* Get a trigger name */
			macro_trigger_name[m] = string_make(buf);

			/* Get the corresponding key code */
			macro_trigger_keycode[0][m] = string_make(zz[1]);

			if (tok == 3)
			{
				/* Key code of a combination of it with the shift key */
				macro_trigger_keycode[1][m] = string_make(zz[2]);
			}
			else
			{
				macro_trigger_keycode[1][m] = string_make(zz[1]);
			}
		}

		/* No error */
		return 0;
	}
	}

	/* Failure */
	return 1;
}


/*
 * Helper function for "process_pref_file()"
 *
 * Input:
 *   v: output buffer array
 *   f: final character
 *
 * Output:
 *   result
 */
static cptr process_pref_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';
	static char tmp[8];

	/* Initial */
	s = (*sp);

	/* Skip spaces */
	while (isspace(*s)) s++;

	/* Save start */
	b = s;

	/* Default */
	v = "?o?o?";

	/* Analyze */
	if (*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_pref_file_expr(&s, &f);

		/* Oops */
		if (!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		else if (streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if (streq(t, "EQU"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(p, t)) v = "0";
			}
		}

		/* Function: LEQ */
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && (strcmp(p, t) > 0)) v = "0";
			}
		}

		/* Function: GEQ */
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && (strcmp(p, t) < 0)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
		}

		/* Verify ending */
		if (f != b2) v = "?x?x?";

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';
	}

	/* Other */
	else
	{
		/* Accept all printables except spaces and brackets */
		while (isprint(*s) && !my_strchr(" []", *s)) ++s;

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';

		/* Variable */
		if (*b == '$')
		{
			/* System */
			if (streq(b+1, "SYS"))
			{
				v = ANGBAND_SYS;
			}

			else if (streq(b+1, "KEYBOARD"))
			{
				v = ANGBAND_KEYBOARD;
			}

			/* Graphics */
			else if (streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

			/* Monochrome mode */
			else if (streq(b+1, "MONOCHROME"))
			{
				if (arg_monochrome)
					v = "ON";
				else
					v = "OFF";
			}

			/* Element */
			else if (streq(b+1, "ELEM"))
			{
#ifdef JP
				v = E_elem_names[p_ptr->pelem];
#else
				v = elem_names[p_ptr->pelem];
#endif
			}

			/* Race */
			else if (streq(b+1, "RACE"))
			{
#ifdef JP
				v = p_name + rp_ptr->E_name;
#else
				v = p_name + rp_ptr->name;
#endif
			}

			/* Class */
			else if (streq(b+1, "CLASS"))
			{
#ifdef JP
				v = c_name + cp_ptr->E_name;
#else
				v = c_name + cp_ptr->name;
#endif
			}

			/* Player */
			else if (streq(b+1, "PLAYER"))
			{
				static char tmp_player_name[32];
				char *pn, *tpn;
				for (pn = player_name, tpn = tmp_player_name; *pn; pn++, tpn++)
				{
#ifdef JP
					if (iskanji(*pn))
					{
						*(tpn++) = *(pn++);
						*tpn = *pn;
						continue;
					}
#endif
					*tpn = my_strchr(" []", *pn) ? '_' : *pn;
				}
				*tpn = '\0';
				v = tmp_player_name;
			}

			/* Sex */
			else if (streq(b+1, "SEX"))
			{
#ifdef JP
					v = sp_ptr->E_title;
#else
					v = sp_ptr->title;
#endif
			}

			/* Racial level */
			else if (streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%02ld", p_ptr->lev);
				v = tmp;
			}

			/* Class level */
			else if (!strncmp(b+1, "CLEVEL", 6))
			{
				char *pp = b + 1 + 6;
				char p1 = '(', p2 = ')';

				/* Current class level */
				if (!*pp)
				{
					sprintf(tmp, "%02ld", p_ptr->cexp_info[p_ptr->pclass].clev);
					v = tmp;
				}

				/* Specified class level */
				else if (*pp == p1)
				{
					char class_name[40];
					char *rp = my_strchr(pp + 2, p2);

					/* Default */
					v = "00";

					pp++;
					if (rp && !*(rp + 1) && (((int)(rp - pp) + 1) < (sizeof class_name)))
					{
						int i;

						strcpy(class_name, pp);
						class_name[(int)(rp - pp)] = '\0'; /* Terminate right parentheses */

						for (i = 0; i < max_c_idx; i++)
						{
#ifdef JP
							if (streq(c_name + class_info[i].E_name, class_name))
#else
							if (streq(c_name + class_info[i].name, class_name))
#endif
							{
								sprintf(tmp, "%02ld", p_ptr->cexp_info[i].clev);
								v = tmp;
								break;
							}
						}
					}
				}
			}
		}

		/* Constant */
		else
		{
			v = b;
		}
	}

	/* Save */
	(*fp) = f;

	/* Save */
	(*sp) = s;

	/* Result */
	return (v);
}



/*
 * Open the "user pref file" and parse it.
 */
static errr process_pref_file_aux(cptr name, bool read_pickpref)
{
	FILE *fp;

	char buf[1024];

	char old[1024];

	int line = -1;

	errr err = 0;

	bool bypass = FALSE;


	/* Open the file */
	fp = my_fopen(name, "r");

	/* No such file */
	if (!fp) return (-1);

	/* Process the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Count lines */
		line++;

		/* Skip "empty" lines */
		if (!buf[0]) continue;

		/* Skip "blank" lines */
#ifdef JP
		if (!iskanji(buf[0]))
#endif
		if (isspace(buf[0])) continue;

		/* Skip comments */
		if (buf[0] == '#') continue;


		/* Save a copy */
		strcpy(old, buf);


		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_pref_file_expr(&s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			continue;
		}

		/* Apply conditionals */
		if (bypass) continue;


		/* Process "%:<file>" */
		if (buf[0] == '%')
		{
			static int depth_count = 0;

			/* Ignore if deeper than 20 level */
			if (depth_count > 20) continue;

			/* Count depth level */
			depth_count++;

			/* Process that file if allowed */
			if (read_pickpref)
				(void)process_pickpref_file(buf + 2);
			else
				(void)process_pref_file(buf + 2);

			/* Set back depth level */
			depth_count--;

			/* Continue */
			continue;
		}


		/* Process the line */
		err = process_pref_file_command(buf);

		/* This is not original pref line... */
		if (err)
		{
			if (!read_pickpref)
				break;
			err = process_pickpref_file_line(buf);
		}
	}


	/* Error */
	if (err)
	{
		/* Print error message */
		/* ToDo: Add better error messages */
#ifdef JP
		msg_format("�ե�����'%s'��%d�Ԥǥ��顼�ֹ�%d�Υ��顼��", name, line, err);
		msg_format("('%s'�������)", old);
#else
		msg_format("Error %d in line %d of file '%s'.", err, line, name);
		msg_format("Parsing '%s'", old);
#endif
		msg_print(NULL);
	}

	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



/*
 * Process the "user pref file" with the given name
 *
 * See the functions above for a list of legal "commands".
 *
 * We also accept the special "?" and "%" directives, which
 * allow conditional evaluation and filename inclusion.
 */
errr process_pref_file(cptr name)
{
	char buf[1024];

	errr err1, err2;

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_PREF, name);

	/* Process the system pref file */
	err1 = process_pref_file_aux(buf, FALSE);

	/* Stop at parser errors, but not at non-existing file */
	if (err1 > 0) return err1;


	/* Drop priv's */
	safe_setuid_drop();
	
	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);
	
	/* Process the user pref file */
	err2 = process_pref_file_aux(buf, FALSE);

	/* Grab priv's */
	safe_setuid_grab();


	/* User file does not exist, but read system pref file */
	if (err2 < 0 && !err1)
		return -2;

	/* Result of user file processing */
	return err2;
}



#ifdef CHECK_TIME

/*
 * Operating hours for ANGBAND (defaults to non-work hours)
 */
static char days[7][29] =
{
	"SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
	"MON:XXXXXXXX.........XXXXXXX",
	"TUE:XXXXXXXX.........XXXXXXX",
	"WED:XXXXXXXX.........XXXXXXX",
	"THU:XXXXXXXX.........XXXXXXX",
	"FRI:XXXXXXXX.........XXXXXXX",
	"SAT:XXXXXXXXXXXXXXXXXXXXXXXX"
};

/*
 * Restict usage (defaults to no restrictions)
 */
static bool check_time_flag = FALSE;

#endif


/*
 * Handle CHECK_TIME
 */
errr check_time(void)
{

#ifdef CHECK_TIME

	time_t      c;
	struct tm   *tp;

	/* No restrictions */
	if (!check_time_flag) return (0);

	/* Check for time violation */
	c = time((time_t *)0);
	tp = localtime(&c);

	/* Violation */
	if (days[tp->tm_wday][tp->tm_hour + 4] != 'X') return (1);

#endif

	/* Success */
	return (0);
}



/*
 * Initialize CHECK_TIME
 */
errr check_time_init(void)
{

#ifdef CHECK_TIME

	FILE        *fp;

	char	buf[1024];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "time.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Assume restrictions */
	check_time_flag = TRUE;

	/* Parse the file */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Chop the buffer */
		buf[29] = '\0';

		/* Extract the info */
		if (prefix(buf, "SUN:")) strcpy(days[0], buf);
		if (prefix(buf, "MON:")) strcpy(days[1], buf);
		if (prefix(buf, "TUE:")) strcpy(days[2], buf);
		if (prefix(buf, "WED:")) strcpy(days[3], buf);
		if (prefix(buf, "THU:")) strcpy(days[4], buf);
		if (prefix(buf, "FRI:")) strcpy(days[5], buf);
		if (prefix(buf, "SAT:")) strcpy(days[6], buf);
	}

	/* Close it */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}



#ifdef CHECK_LOAD

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif

typedef struct statstime statstime;

struct statstime
{
	int                 cp_time[4];
	int                 dk_xfer[4];
	unsigned int        v_pgpgin;
	unsigned int        v_pgpgout;
	unsigned int        v_pswpin;
	unsigned int        v_pswpout;
	unsigned int        v_intr;
	int                 if_ipackets;
	int                 if_ierrors;
	int                 if_opackets;
	int                 if_oerrors;
	int                 if_collisions;
	unsigned int        v_swtch;
	long                avenrun[3];
	struct timeval      boottime;
	struct timeval      curtime;
};

/*
 * Maximal load (if any).
 */
static int check_load_value = 0;

#endif


/*
 * Handle CHECK_LOAD
 */
errr check_load(void)
{

#ifdef CHECK_LOAD

	struct statstime    st;

	/* Success if not checking */
	if (!check_load_value) return (0);

	/* Check the load */
	if (0 == rstat("localhost", &st))
	{
		long val1 = (long)(st.avenrun[2]);
		long val2 = (long)(check_load_value) * FSCALE;

		/* Check for violation */
		if (val1 >= val2) return (1);
	}

#endif

	/* Success */
	return (0);
}


/*
 * Initialize CHECK_LOAD
 */
errr check_load_init(void)
{

#ifdef CHECK_LOAD

	FILE        *fp;

	char	buf[1024];

	char	temphost[MAXHOSTNAMELEN+1];
	char	thishost[MAXHOSTNAMELEN+1];


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "load.txt");

	/* Open the "load" file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Default load */
	check_load_value = 100;

	/* Get the host name */
	(void)gethostname(thishost, (sizeof thishost) - 1);

	/* Parse it */
	while (0 == my_fgets(fp, buf, sizeof(buf)))
	{
		int value;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Parse, or ignore */
		if (sscanf(buf, "%s%d", temphost, &value) != 2) continue;

		/* Skip other hosts */
		if (!streq(temphost, thishost) &&
		    !streq(temphost, "localhost")) continue;

		/* Use that value */
		check_load_value = value;

		/* Done */
		break;
	}

	/* Close the file */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}


#define ENTRY_BARE_HAND          0
#define ENTRY_TWO_HANDS          1
#define ENTRY_RIGHT_HAND1        2
#define ENTRY_LEFT_HAND1         3
#define ENTRY_BARE_HAND2         4
#define ENTRY_LEFT_HAND2         5
#define ENTRY_RIGHT_HAND2        6
#define ENTRY_SHOOT_HIT_DAM      7
#define ENTRY_SHOOT_POWER        8
#define ENTRY_SPEED              9
#define ENTRY_BASE_AC           10
#define ENTRY_RACIAL_LEVEL      11
#define ENTRY_CUR_RACIAL_EXP    12
#define ENTRY_MAX_RACIAL_EXP    13
#define ENTRY_RACIAL_EXP_TO_ADV 14
#define ENTRY_CLASS_LEVEL       15
#define ENTRY_CUR_CLASS_EXP     16
#define ENTRY_MAX_CLASS_EXP     17
#define ENTRY_CLASS_EXP_TO_ADV  18
#define ENTRY_GOLD              19
#define ENTRY_DAY               20
#define ENTRY_HP                21
#define ENTRY_SP                22
#define ENTRY_PLAY_TIME         23
#define ENTRY_SKILL_FIGHT       24
#define ENTRY_SKILL_SHOOT       25
#define ENTRY_SKILL_SAVING      26
#define ENTRY_SKILL_STEALTH     27
#define ENTRY_SKILL_PERCEP      28
#define ENTRY_SKILL_SEARCH      29
#define ENTRY_SKILL_DISARM      30
#define ENTRY_SKILL_DEVICE      31
#define ENTRY_BLOWS             32
#define ENTRY_SHOTS             33
#define ENTRY_AVG_DMG           34
#define ENTRY_INFRA             35
#define ENTRY_ANTI_MAGIC        36
#define ENTRY_NAME              37
#define ENTRY_SEX               38
#define ENTRY_RACE              39
#define ENTRY_CLASS             40
#define ENTRY_ALIGN             41
#define ENTRY_AGE               42
#define ENTRY_SOCIAL            43
#define ENTRY_HEIGHT            44
#define ENTRY_WEIGHT            45
#define ENTRY_ELEM              46


static struct
{
	int col;
	int row;
	int len;
	char header[20];
} disp_player_line[]
#ifdef JP
= {
	{ 1, 10, 25, "�Ƿ⽤��(��Ʈ)"},
	{ 1, 10, 25, "�Ƿ⽤��(ξ��)"},
	{ 1, 10, 25, "�Ƿ⽤��(����)"},
	{ 1, 10, 25, "�Ƿ⽤��(����)"},
	{ 1, 11, 25, "�Ƿ⽤��(��Ʈ)"},
	{ 1, 11, 25, "�Ƿ⽤��(����)"},
	{ 1, 11, 25, "�Ƿ⽤��(����)"},
	{ 1, 15, 25, "�ͷ⹶�⽤��"},
	{ 1, 16, 25, "�ͷ������Ψ"},
	{ 1, 20, 25, "��®"},
	{ 1, 19, 25, "AC"},
	{29, 10, 21, "��²��٥�"},
	{29, 11, 21, "��²�и���"},
	{29, 12, 21, "�����²�и�"},
	{29, 13, 21, "����²��٥�"},
	{29, 15, 21, "���饹��٥�"},
	{29, 16, 21, "���饹�и���"},
	{29, 17, 21, "����C.�и�"},
	{29, 18, 21, "��C.��٥�"},
	{29, 20, 21, "�����"},
	{29,  7, 21, "����"},
	{29,  5, 21, "HP"},
	{29,  6, 21, "MP"},
	{29,  8, 21, "�ץ쥤����"},
	{54, 10, -1, "�Ƿ⹶��  :"},
	{54, 11, -1, "�ͷ⹶��  :"},
	{54, 12, -1, "��ˡ�ɸ�  :"},
	{54, 13, -1, "��̩��ư  :"},
	{54, 14, -1, "�γ�      :"},
	{54, 15, -1, "õ��      :"},
	{54, 16, -1, "���      :"},
	{54, 17, -1, "��ˡƻ��  :"},
	{ 1, 12, 25, "�Ƿ���"},
	{ 1, 17, 25, "�ͷ���"},
	{ 1, 13, 25, "ʿ�ѥ��᡼��"},
	{54, 19, -1, "�ֳ�������:"},
	{54, 20, -1, "�������  :"},
	{26,  1, -1, "̾��  : "},
	{ 1,  3, -1, "����   : "},
	{ 1,  4, -1, "��²   : "},
	{ 1,  5, -1, "���饹 : "},
	{ 1,  6, -1, "���饤�����: "},
	{ 1,  7, 12, "ǯ��"},
	{ 14,  7, 12, "�ϰ�"},
	{ 1,  8, 12, "��Ĺ"},
	{ 14,  8, 12, "�ν�"},
	{29,  3, -1, "������� : "},
};
#else
= {
	{ 1, 10, 25, "Bare hand"},
	{ 1, 10, 25, "Two hands"},
	{ 1, 10, 25, "Right hand"},
	{ 1, 10, 25, "Left hand"},
	{ 1, 11, 25, "Left hand"},
	{ 1, 11, 25, "Right hand"},
	{ 1, 15, 25, "Shooting"},
	{ 1, 16, 25, "Multiplier"},
	{ 1, 20, 25, "Speed"},
	{ 1, 19, 25, "AC"},
	{29, 10, 21, "Racial Level"},
	{29, 11, 21, "R.Experience"},
	{29, 12, 21, "Max R.Exp"},
	{29, 13, 21, "R.Exp to Adv"},
	{29, 15, 21, "Class Level"},
	{29, 16, 21, "C.Experience"},
	{29, 17, 21, "Max C.Exp"},
	{29, 18, 21, "C.Exp to Adv"},
	{29, 20, 21, "Gold"},
	{29,  7, 21, "Time"},
	{29,  5, 21, "Hit point"},
	{29,  6, 21, "SP (Mana)"},
	{29,  8, 21, "Play time"},
	{54, 10, -1, "Fighting    : "},
	{54, 11, -1, "Bows/Throw  : "},
	{54, 12, -1, "Saving Throw: "},
	{54, 13, -1, "Stealth     : "},
	{54, 14, -1, "Perception  : "},
	{54, 15, -1, "Searching   : "},
	{54, 16, -1, "Disarming   : "},
	{54, 17, -1, "Magic Device: "},
	{ 1, 12, 25, "Blows/Round"},
	{ 1, 17, 25, "Shots/Round"},
	{ 1, 13, 25, "AverageDmg/Rnd"},
	{54, 19, -1, "Infra-Vision: "},
	{54, 20, -1, "AM Field    : "},
	{26,  1, -1, "Name  : "},
	{ 1,  3, -1, "Sex    : "},
	{ 1,  4, -1, "Race   : "},
	{ 1,  5, -1, "Class  : "},
	{ 1,  6, -1, "Align  : "},
	{ 1,  7, 12, "Age"},
	{ 1,  7, 12, "S.Class"},
	{ 1,  8, 12, "Height"},
	{ 1,  8, 12, "Weight"},
	{29,  3, -1, "Element : "},
};
#endif

static void display_player_one_line(int entry, cptr val, byte attr)
{
	char buf[40];

	int row = disp_player_line[entry].row;
	int col = disp_player_line[entry].col;
	int len = disp_player_line[entry].len;
	cptr head = disp_player_line[entry].header;

	int head_len = strlen(head);

	Term_putstr(col, row, -1, TERM_WHITE, head);

	if (!val)
		return;

	if (len > 0)
	{
		int val_len = len - head_len;
		sprintf(buf, "%*.*s", val_len, val_len, val);
		Term_putstr(col + head_len, row, -1, attr, buf);
	}
	else
	{
		Term_putstr(col + head_len, row, -1, attr, val);
	}

	return;
}


/*
 * Prints the following information on the screen.
 */
static void display_player_middle(void)
{
	char buf[160];
	int show_tohit, show_todam;
	object_type *o_ptr;
	int tmul = 0;
	cexp_info_type *cexp_ptr = &p_ptr->cexp_info[p_ptr->pclass];

	if (p_ptr->migite)
	{
		show_tohit = p_ptr->dis_to_h[0];
		show_todam = p_ptr->dis_to_d[0];

		o_ptr = &inventory[INVEN_RARM];

		/* Hack -- add in weapon info if known */
		if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
		if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

		/* Melee attacks */
		sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);

		/* Dump the bonuses to hit/dam */
		if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			display_player_one_line(ENTRY_BARE_HAND, buf, TERM_L_BLUE);
		else if (p_ptr->ryoute)
			display_player_one_line(ENTRY_TWO_HANDS, buf, TERM_L_BLUE);
		else if (left_hander)
			display_player_one_line(ENTRY_LEFT_HAND1, buf, TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_RIGHT_HAND1, buf, TERM_L_BLUE);
	}

	if (p_ptr->hidarite)
	{
		show_tohit = p_ptr->dis_to_h[1];
		show_todam = p_ptr->dis_to_d[1];

		o_ptr = &inventory[INVEN_LARM];

		/* Hack -- add in weapon info if known */
		if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
		if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

		/* Melee attacks */
		sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);

		/* Dump the bonuses to hit/dam */
		if (left_hander)
			display_player_one_line(ENTRY_RIGHT_HAND2, buf, TERM_L_BLUE);
		else if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			display_player_one_line(ENTRY_BARE_HAND2, buf, TERM_L_BLUE);
		else
			display_player_one_line(ENTRY_LEFT_HAND2, buf, TERM_L_BLUE);
	}

	/* Range weapon */
	o_ptr = &inventory[INVEN_BOW];

	/* Base skill */
	show_tohit = p_ptr->dis_to_h_b;
	show_todam = 0;

	/* Apply weapon bonuses */
	if (o_ptr->k_idx)
	{
		int attack_var = skill_lev_var[p_ptr->weapon_exp[get_weapon_type(&k_info[o_ptr->k_idx])]/10];

		if (object_is_known(o_ptr)) show_tohit += o_ptr->to_h;
		if (object_is_known(o_ptr)) show_todam += o_ptr->to_d;

		show_tohit += attack_var * 4 - 8;
		show_todam += attack_var - 1;
	}

	/* Range attacks */
	display_player_one_line(ENTRY_SHOOT_HIT_DAM, format("(%+d,%+d)", show_tohit, show_todam), TERM_L_BLUE);

	if (o_ptr->k_idx)
	{
		tmul = bow_tmul(o_ptr);

		/* Get extra "power" from "extra might" */
		if ((o_ptr->sval != SV_ROCKET_LAUNCHER) && p_ptr->dis_xtra_might) tmul++;

		tmul = tmul * (100 + ((get_weapon_type(&k_info[o_ptr->k_idx]) != WT_GUN) ?
			((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128) : 0));
	}

	/* shoot power */
	display_player_one_line(ENTRY_SHOOT_POWER, format("x%d.%02d", tmul/100, tmul%100), TERM_L_BLUE);

	/* Dump the armor class */
	display_player_one_line(ENTRY_BASE_AC, format("[%d,%+d]", p_ptr->dis_ac, p_ptr->dis_to_a), TERM_L_BLUE);

	/* Dump speed */
	{
		bool is_fast = p_ptr->fast;
		int tmp_speed = 0;
		byte attr;
		int i;

		i = p_ptr->pspeed-110;

		/* Hack -- Visually "undo" the Search Mode Slowdown */
		if (p_ptr->action == ACTION_SEARCH) i += 10;

		if (i > 0)
		{
			if (!p_ptr->riding)
				attr = TERM_L_GREEN;
			else
				attr = TERM_GREEN;
		}
		else if (i == 0)
		{
			if (!p_ptr->riding)
				attr = TERM_L_BLUE;
			else
				attr = TERM_GREEN;
		}
		else
		{
			if (!p_ptr->riding)
				attr = TERM_L_UMBER;
			else
				attr = TERM_RED;
		}

		if (!p_ptr->riding)
		{
			if (is_fast) tmp_speed += 10;
			if (p_ptr->slow) tmp_speed -= 10;
		}
		else
		{
			if (MON_FAST(&m_list[p_ptr->riding])) tmp_speed += 10;
			if (MON_SLOW(&m_list[p_ptr->riding])) tmp_speed -= 10;
		}

		if (tmp_speed)
		{
			if (!p_ptr->riding)
				sprintf(buf, "(%+d%+d)", i-tmp_speed, tmp_speed);
			else
#ifdef JP
				sprintf(buf, "������ (%+d%+d)", i-tmp_speed, tmp_speed);
#else
				sprintf(buf, "Riding (%+d%+d)", i-tmp_speed, tmp_speed);
#endif

			if (tmp_speed > 0)
				attr = TERM_YELLOW;
			else
				attr = TERM_VIOLET;
		}
		else
		{
			if (!p_ptr->riding)
				sprintf(buf, "(%+d)", i);
			else
#ifdef JP
				sprintf(buf, "������ (%+d)", i);
#else
				sprintf(buf, "Riding (%+d)", i);
#endif
		}
	
		display_player_one_line(ENTRY_SPEED, buf, attr);
	}

	/* Dump character racial level */
	display_player_one_line(ENTRY_RACIAL_LEVEL, format("%d", p_ptr->lev), TERM_L_GREEN);

	/* Dump racial experience */
	if (p_ptr->exp >= p_ptr->max_exp)
		display_player_one_line(ENTRY_CUR_RACIAL_EXP, format("%ld", p_ptr->exp), TERM_L_GREEN);
	else
		display_player_one_line(ENTRY_CUR_RACIAL_EXP, format("%ld", p_ptr->exp), TERM_YELLOW);

	/* Dump max racial experience */
	display_player_one_line(ENTRY_MAX_RACIAL_EXP, format("%ld", p_ptr->max_exp), TERM_L_GREEN);

	/* Dump racial exp to advance */
	if (p_ptr->lev >= PY_MAX_LEVEL)
		display_player_one_line(ENTRY_RACIAL_EXP_TO_ADV, "*****", TERM_L_GREEN);
	else
		display_player_one_line(ENTRY_RACIAL_EXP_TO_ADV, format("%ld", (s32b)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L)), TERM_L_GREEN);

	/* Dump character class level */
	display_player_one_line(ENTRY_CLASS_LEVEL, format("%d", cexp_ptr->clev), TERM_L_GREEN);

	/* Dump class experience */
	if (cexp_ptr->cexp >= cexp_ptr->max_cexp)
		display_player_one_line(ENTRY_CUR_CLASS_EXP, format("%ld", cexp_ptr->cexp), TERM_L_GREEN);
	else
		display_player_one_line(ENTRY_CUR_CLASS_EXP, format("%ld", cexp_ptr->cexp), TERM_YELLOW);

	/* Dump max class experience */
	display_player_one_line(ENTRY_MAX_CLASS_EXP, format("%ld", cexp_ptr->max_cexp), TERM_L_GREEN);

	/* Dump class exp to advance */
	if (cexp_ptr->clev >= PY_MAX_LEVEL)
		display_player_one_line(ENTRY_CLASS_EXP_TO_ADV, "*****", TERM_L_GREEN);
	else
		display_player_one_line(ENTRY_CLASS_EXP_TO_ADV, format("%ld", (s32b)(player_exp[cexp_ptr->clev - 1] * p_ptr->cexpfact[p_ptr->pclass] / 100L)), TERM_L_GREEN);

	/* Dump gold */
	display_player_one_line(ENTRY_GOLD, format("%ld", p_ptr->au_sum), TERM_L_GREEN);

	/* Dump Day */
	{
		int day, hour, min;
		extract_day_hour_min(&day, &hour, &min);

		sprintf(buf, 
#ifdef JP
			"%d���� %2d:%02d", 
#else
			"Day %d %2d:%02d", 
#endif
			day, hour, min);
	}
	display_player_one_line(ENTRY_DAY, buf, TERM_L_GREEN);

	/* Dump hit point */
	if (p_ptr->chp >= p_ptr->mhp) 
		display_player_one_line(ENTRY_HP, format("%5d/%5d", p_ptr->chp , p_ptr->mhp), TERM_L_GREEN);
	else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10) 
		display_player_one_line(ENTRY_HP, format("%5d/%5d", p_ptr->chp , p_ptr->mhp), TERM_YELLOW);
	else
		display_player_one_line(ENTRY_HP, format("%5d/%5d", p_ptr->chp , p_ptr->mhp), TERM_RED);

	/* Dump mana power */
	if (p_ptr->csp >= p_ptr->msp) 
		display_player_one_line(ENTRY_SP, format("%5d/%5d", p_ptr->csp , p_ptr->msp), TERM_L_GREEN);
	else if (p_ptr->csp > (p_ptr->msp * hitpoint_warn) / 10) 
		display_player_one_line(ENTRY_SP, format("%5d/%5d", p_ptr->csp , p_ptr->msp), TERM_YELLOW);
	else
		display_player_one_line(ENTRY_SP, format("%5d/%5d", p_ptr->csp , p_ptr->msp), TERM_RED);

	/* Dump play time */
	display_player_one_line(ENTRY_PLAY_TIME, format("%.2lu:%.2lu:%.2lu", playtime/(60*60), (playtime/60)%60, playtime%60), TERM_L_GREEN);
}


/*
 * Hack -- pass color info around this file
 */
static byte likert_color = TERM_WHITE;


/*
 * Returns a "rating" of x depending on y
 */
static cptr likert(int x, int y)
{
	static char dummy[20] = "";
	int rate;

	/* Paranoia */
	if (y <= 0) y = 1;

	/* Negative value */
	if (x < 0)
	{
		likert_color = TERM_L_DARK;
#ifdef JP
		return ("����");
#else
		return ("Very Bad");
#endif

	}

	/* Analyze the value */
	rate = x / y;
	switch (rate)
	{
		case 0:
		case 1:
		{
			likert_color = TERM_RED;
#ifdef JP
			return ("����");
#else
			return ("Bad");
#endif

		}
		case 2:
		{
			likert_color = TERM_L_RED;
#ifdef JP
			return ("����");
#else
			return ("Poor");
#endif

		}
		case 3:
		case 4:
		{
			likert_color = TERM_ORANGE;
#ifdef JP
			return ("����");
#else
			return ("Fair");
#endif

		}
		case 5:
		{
			likert_color = TERM_YELLOW;
#ifdef JP
			return ("�ɤ�");
#else
			return ("Good");
#endif

		}
		case 6:
		{
			likert_color = TERM_YELLOW;
#ifdef JP
			return ("�����ɤ�");
#else
			return ("Very Good");
#endif

		}
		case 7:
		case 8:
		{
			likert_color = TERM_L_GREEN;
#ifdef JP
			return ("���");
#else
			return ("Excellent");
#endif

		}
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		{
			likert_color = TERM_GREEN;
#ifdef JP
			return ("Ķ��");
#else
			return ("Superb");
#endif

		}
		case 14:
		case 15:
		case 16:
		case 17:
		{
			likert_color = TERM_BLUE;
#ifdef JP
			return ("��ͺŪ");
#else
			return ("Heroic");
#endif

		}
		default:
		{
			if (rate > SKILL_LIKERT_LEGENDARY_MAX)
			{
				likert_color = TERM_WHITE;
#ifdef JP
				sprintf(dummy,"����Ū [%d]", rate - SKILL_LIKERT_LEGENDARY_MAX);
#else
				sprintf(dummy," Mythical [%d]", rate - SKILL_LIKERT_LEGENDARY_MAX);
#endif
			}
			else
			{
				likert_color = TERM_VIOLET;
#ifdef JP
				sprintf(dummy,"����Ū [%d]", rate - SKILL_LIKERT_HEROIC_MAX);
#else
				sprintf(dummy,"Legendary [%d]", rate - SKILL_LIKERT_HEROIC_MAX);
#endif

			}

			return dummy;
		}
	}
}


/*
 * Prints ratings on certain abilities
 *
 * This code is "imitated" elsewhere to "dump" a character sheet.
 */
static void display_player_various(void)
{
	int         tmp, damage[2], blows1, blows2, i, basedam;
	int			xthn, xthb, xfos, xsrh;
	int			xdis, xdev, xsav, xstl;
	cptr		desc;
	char		raw_desc[10];
	int         muta_att = 0;
	u32b flgs[TR_FLAG_SIZE];
	int		shots, shot_frac;

	object_type		*o_ptr;

	if (p_ptr->muta2 & MUT2_HORNS)     muta_att++;
	if (p_ptr->muta2 & MUT2_SCOR_TAIL) muta_att++;
	if (p_ptr->muta2 & MUT2_BEAK)      muta_att++;
	if (p_ptr->muta2 & MUT2_TRUNK)     muta_att++;
	if (p_ptr->muta2 & MUT2_TENTACLES) muta_att++;

	xthn = p_ptr->skill_thn + (p_ptr->to_h_m * BTH_PLUS_ADJ);
	if (xthn > (SKILL_LIKERT_MYTHICAL_MAX * SKILL_DIV_XTHN)) xthn = SKILL_LIKERT_MYTHICAL_MAX * SKILL_DIV_XTHN;

	/* Shooting Skill (with current bow and normal missile) */
	o_ptr = &inventory[INVEN_BOW];
	tmp = p_ptr->to_h_b + o_ptr->to_h;
	xthb = p_ptr->skill_thb + (tmp * BTH_PLUS_ADJ);
	if (xthb > (SKILL_LIKERT_MYTHICAL_MAX * SKILL_DIV_XTHB)) xthb = SKILL_LIKERT_MYTHICAL_MAX * SKILL_DIV_XTHB;

	/* If the player is wielding one? */
	if (o_ptr->k_idx)
	{
		s32b energy_fire = bow_energy(o_ptr);

		/* Calculate shots per round */
		shots = p_ptr->num_fire * 100;
		shot_frac = (shots * 100 / energy_fire) % 100;
		shots = shots / energy_fire;
	}
	else
	{
		shots = 0;
		shot_frac = 0;
	}

	for(i = 0; i < 2; i++)
	{
		damage[i] = p_ptr->dis_to_d[i]*100;

		/* Average damage per round */
		o_ptr = &inventory[INVEN_RARM+i];

		if (o_ptr->k_idx)
		{
			object_kind *k_ptr = &k_info[o_ptr->k_idx];
			bool known = object_is_known(o_ptr);
			int known_dd = known ? o_ptr->dd : k_ptr->dd;
			int known_ds = known ? o_ptr->ds : k_ptr->ds;

			if (known) damage[i] += o_ptr->to_d * 100;
			basedam = ((known_dd + p_ptr->to_dd[i]) * (known_ds + p_ptr->to_ds[i] + 1)) * 50;
			object_flags_known(o_ptr, flgs);
			if (have_flag(flgs, TR_EXTRA_VORPAL))
			{
				/* vorpal blade */
				basedam *= 5;
				basedam /= 3;
			}
			else if (have_flag(flgs, TR_VORPAL))
			{
				/* vorpal flag only */
				basedam *= 11;
				basedam /= 9;
			}
			if (have_flag(flgs, TR_FORCE_WEAPON) && (p_ptr->csp > (known_dd * known_ds / 5)))
				basedam = basedam * 7 / 2;

			damage[i] += basedam;
		}

		if (damage[i] < 0) damage[i] = 0;
	}
	blows1 = p_ptr->migite ? p_ptr->num_blow[0]: 0;
	blows2 = p_ptr->hidarite ? p_ptr->num_blow[1] : 0;

	/* Basic abilities */

	xdis = p_ptr->skill_dis;
	xdev = p_ptr->skill_dev;
	xsav = p_ptr->skill_sav;
	xstl = p_ptr->skill_stl;
	xsrh = p_ptr->skill_srh;
	xfos = p_ptr->skill_fos;


	desc = likert(xthn, SKILL_DIV_XTHN);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xthn, SKILL_DIV_XTHN);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_FIGHT, desc, likert_color);

	desc = likert(xthb, SKILL_DIV_XTHB);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xthb, SKILL_DIV_XTHB);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_SHOOT, desc, likert_color);

	desc = likert(xsav, SKILL_DIV_XSAV);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xsav, SKILL_DIV_XSAV);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_SAVING, desc, likert_color);

	desc = likert(xstl, SKILL_DIV_XSTL);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xstl, SKILL_DIV_XSTL);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_STEALTH, desc, likert_color);

	desc = likert(xfos, SKILL_DIV_XFOS);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xfos, SKILL_DIV_XFOS);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_PERCEP, desc, likert_color);

	desc = likert(xsrh, SKILL_DIV_XSRH);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xsrh, SKILL_DIV_XSRH);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_SEARCH, desc, likert_color);

	desc = likert(xdis, SKILL_DIV_XDIS);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xdis, SKILL_DIV_XDIS);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_DISARM, desc, likert_color);

	desc = likert(xdev, SKILL_DIV_XDEV);
	if (p_ptr->wizard)
	{
		sprintf(raw_desc, "%d/%d", xdev, SKILL_DIV_XDEV);
		desc = raw_desc;
	}
	display_player_one_line(ENTRY_SKILL_DEVICE, desc, likert_color);

	if (!muta_att)
		display_player_one_line(ENTRY_BLOWS, format("%d+%d", blows1, blows2), TERM_L_BLUE);
	else
		display_player_one_line(ENTRY_BLOWS, format("%d+%d+%d", blows1, blows2, muta_att), TERM_L_BLUE);

	display_player_one_line(ENTRY_SHOTS, format("%d.%02d", shots, shot_frac), TERM_L_BLUE);


	if ((damage[0]+damage[1]) == 0)
		desc = "nil!";
	else
		desc = format("%d+%d", blows1 * damage[0] / 100, blows2 * damage[1] / 100);

	display_player_one_line(ENTRY_AVG_DMG, desc, TERM_L_BLUE);

	display_player_one_line(ENTRY_INFRA, format("%d feet", p_ptr->see_infra * 10), TERM_WHITE);
	display_player_one_line(ENTRY_ANTI_MAGIC, format("%d feet", p_ptr->anti_magic_field * 10), TERM_WHITE);
}


static void tim_player_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (p_ptr->hero || p_ptr->shero)
		add_flag(flgs, TR_RES_FEAR);
	if (p_ptr->tim_invis)
		add_flag(flgs, TR_SEE_INVIS);
	if (p_ptr->tim_esp)
		add_flag(flgs, TR_TELEPATHY);
	if (p_ptr->fast || p_ptr->slow)
		add_flag(flgs, TR_SPEED);
	if (p_ptr->oppose_acid)
		add_flag(flgs, TR_RES_ACID);
	if (p_ptr->oppose_elec)
		add_flag(flgs, TR_RES_ELEC);
	if (p_ptr->oppose_fire)
		add_flag(flgs, TR_RES_FIRE);
	if (p_ptr->oppose_cold)
		add_flag(flgs, TR_RES_COLD);
	if (p_ptr->oppose_pois)
		add_flag(flgs, TR_RES_POIS);
	if (p_ptr->special_attack & ATTACK_EVIL)
	{
		add_flag(flgs, TR_SLAY_GOOD);
		add_flag(flgs, TR_SLAY_LIVING);
	}
	if (p_ptr->special_attack & ATTACK_ACID)
		add_flag(flgs, TR_BRAND_ACID);
	if (p_ptr->special_attack & ATTACK_ELEC)
		add_flag(flgs, TR_BRAND_ELEC);
	if (p_ptr->special_attack & ATTACK_FIRE)
		add_flag(flgs, TR_BRAND_FIRE);
	if (p_ptr->special_attack & ATTACK_COLD)
		add_flag(flgs, TR_BRAND_COLD);
	if (p_ptr->special_attack & ATTACK_POIS)
		add_flag(flgs, TR_BRAND_POIS);
	if (WRAITH_FORM())
	{
		add_flag(flgs, TR_REFLECT);
		if (p_ptr->wraith_form) add_flag(flgs, TR_WRAITH);
	}

	if (p_ptr->magicdef)
	{
		add_flag(flgs, TR_RES_BLIND);
		add_flag(flgs, TR_RES_CONF);
		add_flag(flgs, TR_REFLECT);
		add_flag(flgs, TR_FREE_ACT);
		add_flag(flgs, TR_LEVITATION);
		add_flag(flgs, TR_RES_MAGIC);
	}
	if (p_ptr->shield)
	{
		add_flag(flgs, TR_RES_STONE);
	}
	if (p_ptr->tim_sh_fire)
	{
		add_flag(flgs, TR_SH_FIRE);
	}
	if (p_ptr->tim_sh_elec)
	{
		add_flag(flgs, TR_SH_ELEC);
	}
	if (p_ptr->tim_sh_cold)
	{
		add_flag(flgs, TR_SH_COLD);
	}
	if (p_ptr->tim_inc_blow || p_ptr->tim_dec_blow)
	{
		add_flag(flgs, TR_BLOWS);
	}
	if (p_ptr->chargespell)
	{
		add_flag(flgs, TR_INT);
		add_flag(flgs, TR_WIS);
		add_flag(flgs, TR_REGEN_MANA);
	}
	if (p_ptr->zoshonel_protect)
	{
		add_flag(flgs, TR_STR);
		add_flag(flgs, TR_DEX);
		add_flag(flgs, TR_SPEED);
		add_flag(flgs, TR_IM_FIRE);
		add_flag(flgs, TR_SH_FIRE);
	}
	if (p_ptr->mermaid_in_water)
	{
		add_flag(flgs, TR_SPEED);
	}
}


/* Mode flags for displaying player flags */
#define DP_CURSE   0x0001
#define DP_IMM     0x0002
#define DP_WP      0x0008
#define DP_WP_B    0x0010


/*
 * Equippy chars
 */
static void display_player_equippy(int y, int x, u16b mode)
{
	int i, max_i;

	byte a;
	char c;

	object_type *o_ptr;

	/* Weapon flags need only two column */
	if (mode & DP_WP) max_i = INVEN_LARM + 1;
	else if (mode & DP_WP_B) max_i = INVEN_BOW + 1;
	else max_i = INVEN_TOTAL;

	/* Dump equippy chars */
	for (i = INVEN_RARM; i < max_i; i++)
	{
		/* Object */
		o_ptr = &inventory[i];

		a = object_attr(o_ptr);
		c = object_char(o_ptr);

		/* Clear the part of the screen */
		if (!equippy_chars || !o_ptr->k_idx)
		{
			c = ' ';
			a = TERM_DARK;
		}

		/* Dump */
		Term_putch(x + i - INVEN_RARM, y, a, c);
	}
}


void print_equippy(void)
{
	display_player_equippy(ROW_EQUIPPY, COL_EQUIPPY, 0);
}

/*
 *
 */

static void known_obj_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	/* Check equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b o_flgs[TR_FLAG_SIZE];

		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;

		/* Known flags */
		object_flags_known(o_ptr, o_flgs);

		if (have_flag(flgs, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
		if (have_flag(flgs, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
		if (have_flag(flgs, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
		if (have_flag(flgs, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);
	}
}

#define ART_MENTAL_P(SLOT, NAME1) \
		(inventory[(SLOT)].k_idx && (inventory[(SLOT)].name1 == (NAME1)) && (inventory[(SLOT)].ident & IDENT_MENTAL))

static void player_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (pclass_is_(CLASS_VAMPIRE))
		add_flag(flgs, TR_RES_DARK);

	if ((prace_is_(RACE_GHOST)) || (prace_is_(RACE_SKELETON)))
		add_flag(flgs, TR_RES_NETHER);

	if ((rp_ptr->r_flags & PRF_NO_DIGEST) || (cp_ptr->c_flags & PCF_NO_DIGEST))
		add_flag(flgs, TR_SLOW_DIGEST);

	if (p_ptr->evil_equip)
	{
		if ((ART_MENTAL_P(INVEN_RARM, ART_DAGDA) || ART_MENTAL_P(INVEN_LARM, ART_DAGDA))
		    && (ART_MENTAL_P(INVEN_LEFT, ART_EVIL_RING) || ART_MENTAL_P(INVEN_RIGHT, ART_EVIL_RING))
		    && ART_MENTAL_P(INVEN_BODY, ART_DEATH)
		    && ART_MENTAL_P(INVEN_HEAD, ART_SKULL_MASK))
		{
			add_flag(flgs, TR_RES_DARK);
			add_flag(flgs, TR_RES_NETHER);
		}
	}

	/* Extract flags and store */
	player_flags(p_ptr->flags);

	if (have_flag(p_ptr->flags, TR_IM_ACID)) add_flag(flgs, TR_RES_ACID);
	if (have_flag(p_ptr->flags, TR_IM_ELEC)) add_flag(flgs, TR_RES_ELEC);
	if (have_flag(p_ptr->flags, TR_IM_FIRE)) add_flag(flgs, TR_RES_FIRE);
	if (have_flag(p_ptr->flags, TR_IM_COLD)) add_flag(flgs, TR_RES_COLD);

	if (have_flag(p_ptr->flags, TR_KILL_EVIL)) add_flag(flgs, TR_SLAY_EVIL);
	if (have_flag(p_ptr->flags, TR_KILL_GOOD)) add_flag(flgs, TR_SLAY_GOOD);
	if (have_flag(p_ptr->flags, TR_KILL_UNDEAD)) add_flag(flgs, TR_SLAY_UNDEAD);
	if (have_flag(p_ptr->flags, TR_KILL_LIVING)) add_flag(flgs, TR_SLAY_LIVING);
	if (have_flag(p_ptr->flags, TR_KILL_DEMON)) add_flag(flgs, TR_SLAY_DEMON);
	if (have_flag(p_ptr->flags, TR_KILL_DRAGON)) add_flag(flgs, TR_SLAY_DRAGON);
	if (have_flag(p_ptr->flags, TR_KILL_HUMAN)) add_flag(flgs, TR_SLAY_HUMAN);
	if (have_flag(p_ptr->flags, TR_KILL_ANIMAL)) add_flag(flgs, TR_SLAY_ANIMAL);
	if (have_flag(p_ptr->flags, TR_KILL_ORC)) add_flag(flgs, TR_SLAY_ORC);
	if (have_flag(p_ptr->flags, TR_KILL_TROLL)) add_flag(flgs, TR_SLAY_TROLL);
	if (have_flag(p_ptr->flags, TR_KILL_GIANT)) add_flag(flgs, TR_SLAY_GIANT);

	if (have_flag(p_ptr->flags, TR_EXTRA_VORPAL)) add_flag(flgs, TR_VORPAL);
}

static void tim_player_immunity(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (WRAITH_FORM())
		add_flag(flgs, TR_RES_DARK);

	if (p_ptr->zoshonel_protect)
		add_flag(flgs, TR_RES_FIRE);
}

static void player_vuln_flags(u32b flgs[TR_FLAG_SIZE])
{
	int i;

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (p_ptr->muta3 & MUT3_VULN_ELEM)
	{
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_FIRE);
		add_flag(flgs, TR_RES_COLD);
	}
	if (p_ptr->hurt_lite)
		add_flag(flgs, TR_RES_LITE);
	if (prace_is_(RACE_FAIRY))
		add_flag(flgs, TR_RES_DARK);
	if (prace_is_(RACE_PUMPKINHEAD))
		add_flag(flgs, TR_RES_FIRE);

	if (p_ptr->ogre_equip)
	{
		if ((ART_MENTAL_P(INVEN_RARM, ART_OGRE_BLADE) || ART_MENTAL_P(INVEN_LARM, ART_OGRE_BLADE))
		    && (ART_MENTAL_P(INVEN_LARM, ART_OGRE_SHIELD) || ART_MENTAL_P(INVEN_RARM, ART_OGRE_SHIELD))
		    && ART_MENTAL_P(INVEN_BODY, ART_OGRE_ARMOR)
		    && ART_MENTAL_P(INVEN_HEAD, ART_OGRE_HELM))
			add_flag(flgs, TR_RES_LITE);
	}

	if (p_ptr->zoshonel_protect)
		add_flag(flgs, TR_RES_COLD);

	if (p_ptr->mermaid_in_water)
		add_flag(flgs, TR_RES_ELEC);

	if (p_ptr->weak_fire)
		add_flag(flgs, TR_RES_FIRE);

	if (p_ptr->weak_aqua)
		add_flag(flgs, TR_RES_COLD);

	if (p_ptr->weak_earth)
	{
		add_flag(flgs, TR_RES_ACID);
		add_flag(flgs, TR_RES_SHARDS);
		add_flag(flgs, TR_RES_STONE);
	}

	if (p_ptr->weak_wind)
	{
		add_flag(flgs, TR_RES_ELEC);
		add_flag(flgs, TR_RES_SOUND);
	}
}


/*
 * A struct for storing misc. flags
 */
typedef struct
{
	u32b tim_player_flags[TR_FLAG_SIZE];
	u32b player_imm[TR_FLAG_SIZE];
	u32b tim_player_imm[TR_FLAG_SIZE];
	u32b player_vuln[TR_FLAG_SIZE];
	u32b known_obj_imm[TR_FLAG_SIZE];
} all_player_flags;


/*
 * Helper function, see below
 */
static void display_flag_aux(int row, int col, cptr header,
				    int flag1, all_player_flags *f, u16b mode)
{
	int     i;
	bool    vuln = FALSE;
	int max_i;

	if (have_flag(f->player_vuln, flag1) &&
	    !(have_flag(f->known_obj_imm, flag1) ||
	      have_flag(f->player_imm, flag1) ||
	      have_flag(f->tim_player_imm, flag1)))
		vuln = TRUE;

	/* Header */
	if (!(mode & DP_IMM)) c_put_str(TERM_WHITE, header, row, col);

	/* Advance */
	col += strlen(header) + 1;

	/* Weapon flags need only two column */
	if (mode & DP_WP) max_i = INVEN_LARM + 1;
	else if (mode & DP_WP_B) max_i = INVEN_BOW + 1;
	else max_i = INVEN_TOTAL;

	/* Check equipment */
	for (i = INVEN_RARM; i < max_i; i++)
	{
		u32b flgs[TR_FLAG_SIZE];
		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];

		/* Known flags */
		object_flags_known(o_ptr, flgs);


		/* Default */
		if (!(mode & DP_IMM))
			c_put_str((byte)(vuln ? TERM_RED : TERM_SLATE), ".", row, col);

		/* Check flags */
		if (mode & DP_CURSE)
		{
			if ((mode & DP_CURSE) && (o_ptr->curse_flags & (TRC_CURSED | TRC_HEAVY_CURSE)))
				c_put_str(TERM_WHITE, "+", row, col);
			if ((mode & DP_CURSE) && (o_ptr->curse_flags & TRC_PERMA_CURSE))
				c_put_str(TERM_WHITE, "*", row, col);
		}
		else
		{
			if (have_flag(flgs, flag1))
				c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE),
				          (mode & DP_IMM) ? "*" : "+", row, col);
		}

		/* Advance */
		col++;
	}

	/* Assume that player flag is already written */
	if (mode & DP_IMM) return;

	/* Default */
	c_put_str((byte)(vuln ? TERM_RED : TERM_SLATE), ".", row, col);

	/* Player flags */
	if (have_flag(p_ptr->flags, flag1)) c_put_str((byte)(vuln ? TERM_L_RED : TERM_WHITE), "+", row, col);

	/* Timed player flags */
	if (have_flag(f->tim_player_flags, flag1)) c_put_str((byte)(vuln ? TERM_ORANGE : TERM_YELLOW), "#", row, col);

	/* Immunity */
	if (have_flag(f->tim_player_imm, flag1)) c_put_str(TERM_YELLOW, "*", row, col);
	if (have_flag(f->player_imm, flag1)) c_put_str(TERM_WHITE, "*", row, col);

	/* Vulnerability */
	if (vuln) c_put_str(TERM_RED, "v", row, col + 1);
}


/*
 * Special display, part 1
 */
static void display_player_flag_info(void)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	player_flags(p_ptr->flags);
	tim_player_flags(f.tim_player_flags);
	player_immunity(f.player_imm);
	tim_player_immunity(f.tim_player_imm);
	known_obj_immunity(f.known_obj_imm);
	player_vuln_flags(f.player_vuln);

	/*** Set 1 ***/

	row = 12;
	col = 1;

	display_player_equippy(row-2, col+8, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
	display_flag_aux(row+0, col, "�ѻ�  :", TR_RES_ACID, &f, 0);
	display_flag_aux(row+0, col, "�ѻ�  :", TR_IM_ACID, &f, DP_IMM);
	display_flag_aux(row+1, col, "���ŷ�:", TR_RES_ELEC, &f, 0);
	display_flag_aux(row+1, col, "���ŷ�:", TR_IM_ELEC, &f, DP_IMM);
	display_flag_aux(row+2, col, "�Ѳб�:", TR_RES_FIRE, &f, 0);
	display_flag_aux(row+2, col, "�Ѳб�:", TR_IM_FIRE, &f, DP_IMM);
	display_flag_aux(row+3, col, "���䵤:", TR_RES_COLD, &f, 0);
	display_flag_aux(row+3, col, "���䵤:", TR_IM_COLD, &f, DP_IMM);
	display_flag_aux(row+4, col, "����  :", TR_RES_POIS, &f, 0);
	display_flag_aux(row+5, col, "������:", TR_RES_LITE, &f, 0);
	display_flag_aux(row+6, col, "�ѰŹ�:", TR_RES_DARK, &f, 0);
	display_flag_aux(row+7, col, "������:", TR_RES_SHARDS, &f, 0);
	display_flag_aux(row+8, col, "������:", TR_RES_BLIND, &f, 0);
	display_flag_aux(row+9, col, "�Ѻ���:", TR_RES_CONF, &f, 0);
#else
	display_flag_aux(row+0, col, "Acid  :", TR_RES_ACID, &f, 0);
	display_flag_aux(row+0, col, "Acid  :", TR_IM_ACID, &f, DP_IMM);
	display_flag_aux(row+1, col, "Elec  :", TR_RES_ELEC, &f, 0);
	display_flag_aux(row+1, col, "Elec  :", TR_IM_ELEC, &f, DP_IMM);
	display_flag_aux(row+2, col, "Fire  :", TR_RES_FIRE, &f, 0);
	display_flag_aux(row+2, col, "Fire  :", TR_IM_FIRE, &f, DP_IMM);
	display_flag_aux(row+3, col, "Cold  :", TR_RES_COLD, &f, 0);
	display_flag_aux(row+3, col, "Cold  :", TR_IM_COLD, &f, DP_IMM);
	display_flag_aux(row+4, col, "Poison:", TR_RES_POIS, &f, 0);
	display_flag_aux(row+5, col, "Light :", TR_RES_LITE, &f, 0);
	display_flag_aux(row+6, col, "Dark  :", TR_RES_DARK, &f, 0);
	display_flag_aux(row+7, col, "Shard :", TR_RES_SHARDS, &f, 0);
	display_flag_aux(row+8, col, "Blind :", TR_RES_BLIND, &f, 0);
	display_flag_aux(row+9, col, "Conf  :", TR_RES_CONF, &f, 0);
#endif


	/*** Set 2 ***/

	row = 12;
	col = 26;

	display_player_equippy(row-2, col+8, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
	display_flag_aux(row+0, col, "�ѹ첻:", TR_RES_SOUND, &f, 0);
	display_flag_aux(row+1, col, "���Ϲ�:", TR_RES_NETHER, &f, 0);
	display_flag_aux(row+2, col, "���в�:", TR_RES_STONE, &f, 0);
	display_flag_aux(row+3, col, "�ѥ���:", TR_RES_CHAOS, &f, 0);
	display_flag_aux(row+4, col, "������:", TR_RES_DISEN, &f, 0);
	display_flag_aux(row+5, col, "�Ѷ���:", TR_RES_FEAR, &f, 0);
	display_flag_aux(row+6, col, "ȿ��  :", TR_REFLECT, &f, 0);
	display_flag_aux(row+7, col, "�бꥪ:", TR_SH_FIRE, &f, 0);
	display_flag_aux(row+8, col, "�ŵ���:", TR_SH_ELEC, &f, 0);
	display_flag_aux(row+9, col, "�䵤��:", TR_SH_COLD, &f, 0);
#else
	display_flag_aux(row+0, col, "Sound :", TR_RES_SOUND, &f, 0);
	display_flag_aux(row+1, col, "Nether:", TR_RES_NETHER, &f, 0);
	display_flag_aux(row+2, col, "Stone :", TR_RES_STONE, &f, 0);
	display_flag_aux(row+3, col, "Chaos :", TR_RES_CHAOS, &f, 0);
	display_flag_aux(row+4, col, "Disnch:", TR_RES_DISEN, &f, 0);
	display_flag_aux(row+5, col, "Fear  :", TR_RES_FEAR, &f, 0);
	display_flag_aux(row+6, col, "Reflct:", TR_REFLECT, &f, 0);
	display_flag_aux(row+7, col, "AuFire:", TR_SH_FIRE, &f, 0);
	display_flag_aux(row+8, col, "AuElec:", TR_SH_ELEC, &f, 0);
	display_flag_aux(row+9, col, "AuCold:", TR_SH_COLD, &f, 0);
#endif


	/*** Set 3 ***/

	row = 12;
	col = 51;

	display_player_equippy(row-2, col+12, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+12);

#ifdef JP
	display_flag_aux(row+0, col, "��®      :", TR_SPEED, &f, 0);
	display_flag_aux(row+1, col, "������    :", TR_FREE_ACT, &f, 0);
	display_flag_aux(row+2, col, "Ʃ���λ�ǧ:", TR_SEE_INVIS, &f, 0);
	display_flag_aux(row+3, col, "�и����ݻ�:", TR_HOLD_LIFE, &f, 0);
	display_flag_aux(row+4, col, "�ٹ�      :", TR_WARNING, &f, 0);
	display_flag_aux(row+5, col, "�پò�    :", TR_SLOW_DIGEST, &f, 0);
	display_flag_aux(row+6, col, "�޲���    :", TR_REGEN, &f, 0);
	display_flag_aux(row+7, col, "��ͷ      :", TR_LEVITATION, &f, 0);
	display_flag_aux(row+8, col, "�ʱ����  :", TR_LITE, &f, 0);
	display_flag_aux(row+9, col, "����      :", 0, &f, DP_CURSE);
#else
	display_flag_aux(row+0, col, "Speed     :", TR_SPEED, &f, 0);
	display_flag_aux(row+1, col, "FreeAction:", TR_FREE_ACT, &f, 0);
	display_flag_aux(row+2, col, "SeeInvisi.:", TR_SEE_INVIS, &f, 0);
	display_flag_aux(row+3, col, "Hold Life :", TR_HOLD_LIFE, &f, 0);
	display_flag_aux(row+4, col, "Warning   :", TR_WARNING, &f, 0);
	display_flag_aux(row+5, col, "SlowDigest:", TR_SLOW_DIGEST, &f, 0);
	display_flag_aux(row+6, col, "Regene.   :", TR_REGEN, &f, 0);
	display_flag_aux(row+7, col, "Levitation:", TR_LEVITATION, &f, 0);
	display_flag_aux(row+8, col, "Perm Lite :", TR_LITE, &f, 0);
	display_flag_aux(row+9, col, "Cursed    :", 0, &f, DP_CURSE);
#endif

}


/*
 * Special display, part 2
 */
static void display_player_other_flag_info(void)
{
	int row;
	int col;

	all_player_flags f;

	/* Extract flags and store */
	player_flags(p_ptr->flags);
	tim_player_flags(f.tim_player_flags);
	player_immunity(f.player_imm);
	tim_player_immunity(f.tim_player_imm);
	known_obj_immunity(f.known_obj_imm);
	player_vuln_flags(f.player_vuln);

	/*** Set 1 ***/

	row = 3;
	col = 1;

	display_player_equippy(row-2, col+12, DP_WP_B);
	c_put_str(TERM_WHITE, "abc@", row-1, col+12);

#ifdef JP
	display_flag_aux(row+ 0, col, "�ٰ� ���� :", TR_SLAY_EVIL, &f, DP_WP_B);
	display_flag_aux(row+ 0, col, "�ٰ� ���� :", TR_KILL_EVIL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "���� ���� :", TR_SLAY_GOOD, &f, DP_WP_B);
	display_flag_aux(row+ 1, col, "���� ���� :", TR_KILL_GOOD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 2, col, "�Ի� ���� :", TR_SLAY_UNDEAD, &f, DP_WP_B);
	display_flag_aux(row+ 2, col, "�Ի� ���� :", TR_KILL_UNDEAD, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 3, col, "��̿ ���� :", TR_SLAY_LIVING, &f, DP_WP_B);
	display_flag_aux(row+ 3, col, "��̿ ���� :", TR_KILL_LIVING, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 4, col, "���� ���� :", TR_SLAY_DEMON, &f, DP_WP_B);
	display_flag_aux(row+ 4, col, "���� ���� :", TR_KILL_DEMON, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 5, col, "ζ ����   :", TR_SLAY_DRAGON, &f, DP_WP_B);
	display_flag_aux(row+ 5, col, "ζ ����   :", TR_KILL_DRAGON, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 6, col, "�ʹ� ���� :", TR_SLAY_HUMAN, &f, DP_WP_B);
	display_flag_aux(row+ 6, col, "�ʹ� ���� :", TR_KILL_HUMAN, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 7, col, "ưʪ ���� :", TR_SLAY_ANIMAL, &f, DP_WP_B);
	display_flag_aux(row+ 7, col, "ưʪ ���� :", TR_KILL_ANIMAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 8, col, "����������:", TR_SLAY_ORC, &f, DP_WP_B);
	display_flag_aux(row+ 8, col, "����������:", TR_KILL_ORC, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 9, col, "�ȥ������:", TR_SLAY_TROLL, &f, DP_WP_B);
	display_flag_aux(row+ 9, col, "�ȥ������:", TR_KILL_TROLL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+10, col, "��� ���� :", TR_SLAY_GIANT, &f, DP_WP_B);
	display_flag_aux(row+10, col, "��� ���� :", TR_KILL_GIANT, &f, (DP_WP|DP_IMM));

	display_flag_aux(row+12, col, "�ϲ�      :", TR_BRAND_ACID, &f, DP_WP_B);
	display_flag_aux(row+13, col, "�ŷ�      :", TR_BRAND_ELEC, &f, DP_WP_B);
	display_flag_aux(row+14, col, "�ƴ�      :", TR_BRAND_FIRE, &f, DP_WP_B);
	display_flag_aux(row+15, col, "���      :", TR_BRAND_COLD, &f, DP_WP_B);
	display_flag_aux(row+16, col, "�ǻ�      :", TR_BRAND_POIS, &f, DP_WP_B);

	display_flag_aux(row+18, col, "�Ծ�      :", TR_UNHOLY, &f, DP_WP_B);
#else
	display_flag_aux(row+ 0, col, "Slay Evil :", TR_SLAY_EVIL, &f, DP_WP_B);
	display_flag_aux(row+ 0, col, "Slay Evil :", TR_KILL_EVIL, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 1, col, "Slay Good :", TR_SLAY_GOOD, &f, DP_WP_B);
	display_flag_aux(row+ 1, col, "Slay Good :", TR_KILL_GOOD, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 2, col, "Slay Und. :", TR_SLAY_UNDEAD, &f, DP_WP_B);
	display_flag_aux(row+ 2, col, "Slay Und. :", TR_KILL_UNDEAD, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 3, col, "Slay Liv. :", TR_SLAY_LIVING, &f, DP_WP_B);
	display_flag_aux(row+ 3, col, "Slay Liv. :", TR_KILL_LIVING, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 4, col, "Slay Demon:", TR_SLAY_DEMON, &f, DP_WP_B);
	display_flag_aux(row+ 4, col, "Slay Demon:", TR_KILL_DEMON, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 5, col, "Slay Drag.:", TR_SLAY_DRAGON, &f, DP_WP_B);
	display_flag_aux(row+ 5, col, "Slay Drag.:", TR_KILL_DRAGON, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 6, col, "Slay Human:", TR_SLAY_HUMAN, &f, DP_WP_B);
	display_flag_aux(row+ 6, col, "Slay Human:", TR_KILL_HUMAN, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 7, col, "Slay Anim.:", TR_SLAY_ANIMAL, &f, DP_WP_B);
	display_flag_aux(row+ 7, col, "Slay Anim.:", TR_KILL_ANIMAL, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 8, col, "Slay Orc  :", TR_SLAY_ORC, &f, DP_WP_B);
	display_flag_aux(row+ 8, col, "Slay Orc  :", TR_KILL_ORC, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+ 9, col, "Slay Troll:", TR_SLAY_TROLL, &f, DP_WP_B);
	display_flag_aux(row+ 9, col, "Slay Troll:", TR_KILL_TROLL, &f, (DP_WP_B|DP_IMM));
	display_flag_aux(row+10, col, "Slay Giant:", TR_SLAY_GIANT, &f, DP_WP_B);
	display_flag_aux(row+10, col, "Slay Giant:", TR_KILL_GIANT, &f, (DP_WP_B|DP_IMM));

	display_flag_aux(row+12, col, "Acid Brand:", TR_BRAND_ACID, &f, DP_WP_B);
	display_flag_aux(row+13, col, "Elec Brand:", TR_BRAND_ELEC, &f, DP_WP_B);
	display_flag_aux(row+14, col, "Fire Brand:", TR_BRAND_FIRE, &f, DP_WP_B);
	display_flag_aux(row+15, col, "Cold Brand:", TR_BRAND_COLD, &f, DP_WP_B);
	display_flag_aux(row+16, col, "Poison Brd:", TR_BRAND_POIS, &f, DP_WP_B);

	display_flag_aux(row+18, col, "Unholy    :", TR_UNHOLY, &f, DP_WP_B);
#endif


	/*** Set 2.1 ***/

	row = 3;
	col = col + 12 + 8;

	display_player_equippy(row-2, col+12, DP_WP);
	c_put_str(TERM_WHITE, "ab@", row-1, col+12);

#ifdef JP
	display_flag_aux(row+ 0, col, "�ڤ�̣    :", TR_VORPAL, &f, DP_WP);
	display_flag_aux(row+ 0, col, "�ڤ�̣    :", TR_EXTRA_VORPAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "�Ͽ�      :", TR_IMPACT, &f, DP_WP);
	display_flag_aux(row+ 2, col, "�۷�      :", TR_VAMPIRIC, &f, DP_WP);
	display_flag_aux(row+ 3, col, "����������:", TR_CHAOTIC, &f, DP_WP);
	display_flag_aux(row+ 4, col, "����      :", TR_FORCE_WEAPON, &f, DP_WP);
	display_flag_aux(row+ 5, col, "����      :", TR_RIDING, &f, DP_WP);
	display_flag_aux(row+ 6, col, "��ڳ      :", TR_THROW, &f, DP_WP);
#else
	display_flag_aux(row+ 0, col, "Sharpness :", TR_VORPAL, &f, DP_WP);
	display_flag_aux(row+ 0, col, "Sharpness :", TR_EXTRA_VORPAL, &f, (DP_WP|DP_IMM));
	display_flag_aux(row+ 1, col, "Quake     :", TR_IMPACT, &f, DP_WP);
	display_flag_aux(row+ 2, col, "Vampicic  :", TR_VAMPIRIC, &f, DP_WP);
	display_flag_aux(row+ 3, col, "Chatic    :", TR_CHAOTIC, &f, DP_WP);
	display_flag_aux(row+ 4, col, "Force Wep.:", TR_FORCE_WEAPON, &f, DP_WP);
	display_flag_aux(row+ 5, col, "Riding    :", TR_RIDING, &f, DP_WP);
	display_flag_aux(row+ 6, col, "Throw     :", TR_THROW, &f, DP_WP);
#endif


	/*** Set 2.2 ***/

	row += 10;

	display_player_equippy(row-2, col+12, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+12);

#ifdef JP
	display_flag_aux(row+ 0, col, "���ϰݻ�  :", TR_SUST_STR, &f, 0);
	display_flag_aux(row+ 1, col, "��ǽ�ݻ�  :", TR_SUST_INT, &f, 0);
	display_flag_aux(row+ 2, col, "�����ݻ�  :", TR_SUST_WIS, &f, 0);
	display_flag_aux(row+ 3, col, "���Ѱݻ�  :", TR_SUST_DEX, &f, 0);
	display_flag_aux(row+ 4, col, "�ѵװݻ�  :", TR_SUST_CON, &f, 0);
	display_flag_aux(row+ 5, col, "̥�ϰݻ�  :", TR_SUST_CHR, &f, 0);
	display_flag_aux(row+ 6, col, "����ˡ    :", TR_RES_MAGIC, &f, 0);
	display_flag_aux(row+ 7, col, "ȿ��ˡ    :", TR_NO_MAGIC, &f, 0);
	display_flag_aux(row+ 8, col, "�������  :", TR_ANTI_MAGIC, &f, 0);
#else
	display_flag_aux(row+ 0, col, "Sust Str  :", TR_SUST_STR, &f, 0);
	display_flag_aux(row+ 1, col, "Sust Int  :", TR_SUST_INT, &f, 0);
	display_flag_aux(row+ 2, col, "Sust Wis  :", TR_SUST_WIS, &f, 0);
	display_flag_aux(row+ 3, col, "Sust Dex  :", TR_SUST_DEX, &f, 0);
	display_flag_aux(row+ 4, col, "Sust Con  :", TR_SUST_CON, &f, 0);
	display_flag_aux(row+ 5, col, "Sust Chr  :", TR_SUST_CHR, &f, 0);
	display_flag_aux(row+ 6, col, "Res. Magic:", TR_RES_MAGIC, &f, 0);
	display_flag_aux(row+ 7, col, "Anti Magic:", TR_NO_MAGIC, &f, 0);
	display_flag_aux(row+ 8, col, "AM Field  :", TR_ANTI_MAGIC, &f, 0);
#endif


	/*** Set 3 ***/

	row = 3;
	col = col + 12 + 17;

	display_player_equippy(row-2, col+14, 0);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+14);

#ifdef JP
	display_flag_aux(row+ 0, col, "�ɲù���    :", TR_BLOWS, &f, 0);
	display_flag_aux(row+ 1, col, "�η�        :", TR_TUNNEL, &f, 0);
	display_flag_aux(row+ 2, col, "�ֳ�������  :", TR_INFRA, &f, 0);
	display_flag_aux(row+ 3, col, "��ˡƻ�����:", TR_MAGIC_MASTERY, &f, 0);
	display_flag_aux(row+ 4, col, "��̩        :", TR_STEALTH, &f, 0);
	display_flag_aux(row+ 5, col, "õ��        :", TR_SEARCH, &f, 0);
	display_flag_aux(row+ 6, col, "�ƥ�ѥ���  :", TR_TELEPATHY, &f, 0);
	display_flag_aux(row+ 7, col, "��ʡ        :", TR_BLESSED, &f, 0);
	display_flag_aux(row+ 8, col, "ȿ�ƥ�ݡ���:", TR_NO_TELE, &f, 0);
	display_flag_aux(row+ 9, col, "���ϵ޲���  :", TR_REGEN_MANA, &f, 0);
	display_flag_aux(row+10, col, "�������ϸ���:", TR_DEC_MANA, &f, 0);
	display_flag_aux(row+11, col, "����ٸ���  :", TR_EASY_SPELL, &f, 0);
	display_flag_aux(row+12, col, "ͩ�β�      :", TR_WRAITH, &f, 0);
	display_flag_aux(row+13, col, "���ݶ���    :", TR_FEAR_FIELD, &f, 0);

	display_flag_aux(row+15, col, "�и��͸���  :", TR_DRAIN_EXP, &f, 0);
	display_flag_aux(row+16, col, "��ƥ�ݡ���:", TR_TELEPORT, &f, 0);
	display_flag_aux(row+17, col, "ȿ��        :", TR_AGGRAVATE, &f, 0);
	display_flag_aux(row+18, col, "���Ťα�ǰ  :", TR_TY_CURSE, &f, 0);
#else
	display_flag_aux(row+ 0, col, "Add Blows   :", TR_BLOWS, &f, 0);
	display_flag_aux(row+ 1, col, "Add Tunnel  :", TR_TUNNEL, &f, 0);
	display_flag_aux(row+ 2, col, "Add Infra   :", TR_INFRA, &f, 0);
	display_flag_aux(row+ 3, col, "Add Device  :", TR_MAGIC_MASTERY, &f, 0);
	display_flag_aux(row+ 4, col, "Add Stealth :", TR_STEALTH, &f, 0);
	display_flag_aux(row+ 5, col, "Add Search  :", TR_SEARCH, &f, 0);
	display_flag_aux(row+ 6, col, "Telepathy   :", TR_TELEPATHY, &f, 0);
	display_flag_aux(row+ 7, col, "Blessed     :", TR_BLESSED, &f, 0);
	display_flag_aux(row+ 8, col, "No Teleport :", TR_NO_TELE, &f, 0);
	display_flag_aux(row+ 9, col, "Regen. Mana :", TR_REGEN_MANA, &f, 0);
	display_flag_aux(row+10, col, "Econom. Mana:", TR_DEC_MANA, &f, 0);
	display_flag_aux(row+11, col, "Easy Spell  :", TR_EASY_SPELL, &f, 0);
	display_flag_aux(row+12, col, "Wraith Form :", TR_WRAITH, &f, 0);
	display_flag_aux(row+13, col, "Fear Field  :", TR_FEAR_FIELD, &f, 0);

	display_flag_aux(row+15, col, "Drain Exp   :", TR_DRAIN_EXP, &f, 0);
	display_flag_aux(row+16, col, "Rnd.Teleport:", TR_TELEPORT, &f, 0);
	display_flag_aux(row+17, col, "Aggravate   :", TR_AGGRAVATE, &f, 0);
	display_flag_aux(row+18, col, "TY Curse    :", TR_TY_CURSE, &f, 0);
#endif

}


/*
 * Special display, part 2a
 */
static void display_player_misc_info(void)
{
	char	buf[80];

	/* Display basics */
#ifdef JP
	put_str("̾��  :", 1, 26);
	put_str("����  :", 3, 1);
	if (!(cp_ptr->c_flags & PCF_REINCARNATE)) put_str("��²  :", 4, 1);
	put_str("���饹:", 5, 1);
#else
	put_str("Name  :", 1, 26);
	put_str("Sex   :", 3, 1);
	if (!(cp_ptr->c_flags & PCF_REINCARNATE)) put_str("Race  :", 4, 1);
	put_str("Class :", 5, 1);
#endif

	c_put_str(TERM_L_BLUE, player_name, 1, 34);
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 9);
	if (!(cp_ptr->c_flags & PCF_REINCARNATE)) c_put_str(TERM_L_BLUE, p_name + rp_ptr->name, 4, 9);
	c_put_str(TERM_L_BLUE, c_name + cp_ptr->name, 5, 9);

	/* Display extras */
#ifdef JP
	put_str("��٥�:", 6, 1);
	put_str("C-Lv. :", 7, 1);
	put_str("HP    :", 8, 1);
	put_str("MP    :", 9, 1);
#else
	put_str("Level :", 6, 1);
	put_str("CLevel:", 7, 1);
	put_str("Hits  :", 8, 1);
	put_str("Mana  :", 9, 1);
#endif


	(void)sprintf(buf, "%d", (int)p_ptr->lev);
	c_put_str(TERM_L_BLUE, buf, 6, 9);
	(void)sprintf(buf, "%d", (int)p_ptr->cexp_info[p_ptr->pclass].clev);
	c_put_str(TERM_L_BLUE, buf, 7, 9);
	(void)sprintf(buf, "%d/%d", (int)p_ptr->chp, (int)p_ptr->mhp);
	c_put_str(TERM_L_BLUE, buf, 8, 9);
	(void)sprintf(buf, "%d/%d", (int)p_ptr->csp, (int)p_ptr->msp);
	c_put_str(TERM_L_BLUE, buf, 9, 9);
}


/*
 * Special display, part 2b
 *
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MICoMorgoth, will be a '*'
 * No mod, no sustain, will be a slate '.'
 */
static void display_player_stat_info(void)
{
	int i, e_adj;
	int stat_col, stat;
	int row, col;

	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];

	byte a;
	char c;

	char buf[80];


	/* Column */
	stat_col = 22;

	/* Row */
	row = 3;

	/* Print out the labels for the columns */
#ifdef JP
	c_put_str(TERM_WHITE, "ǽ��", row, stat_col+1);
	c_put_str(TERM_BLUE, "  ����", row, stat_col+7);
	c_put_str(TERM_L_BLUE, "  ��²  ���� ", row, stat_col+13);
	c_put_str(TERM_L_GREEN, "���", row, stat_col+28);
	c_put_str(TERM_YELLOW, "����", row, stat_col+35);
#else
	c_put_str(TERM_WHITE, "Stat", row, stat_col+1);
	c_put_str(TERM_BLUE, "  Base", row, stat_col+7);
	c_put_str(TERM_L_BLUE, "Rac      Mod", row, stat_col+13);
	c_put_str(TERM_L_GREEN, "Actual", row, stat_col+26);
	c_put_str(TERM_YELLOW, "Current", row, stat_col+32);
#endif


	/* Display the stats */
	for (i = 0; i < A_MAX; i++)
	{
		int r_adj = (cp_ptr->c_flags & PCF_REINCARNATE) ? cp_ptr->c_bonus[i] : rp_ptr->r_adj[i];

		/* Calculate equipment adjustment */
		e_adj = 0;

		/* Icky formula to deal with the 18 barrier */
		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - p_ptr->stat_max[i]) / 10;
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - p_ptr->stat_max[i];
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - 18) / 10 - p_ptr->stat_max[i] + 18;

		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - (p_ptr->stat_max[i] - 19) / 10 - 19;

		e_adj -= r_adj;

		if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			/* Reduced name of stat */
			c_put_str(TERM_WHITE, stat_names_reduced[i], row + i+1, stat_col+1);
		else
			c_put_str(TERM_WHITE, stat_names[i], row + i+1, stat_col+1);


		/* Internal "natural" max value.  Maxes at 18/200 */
		/* This is useful to see if you are maxed out */
		cnv_stat(p_ptr->stat_max[i], buf);
		if (p_ptr->stat_max[i] == STAT_MAX_MAX)
		{
#ifdef JP
			c_put_str(TERM_WHITE, "!", row + i+1, stat_col + 6);
#else
			c_put_str(TERM_WHITE, "!", row + i+1, stat_col + 4);
#endif
		}
		c_put_str(TERM_BLUE, buf, row + i+1, stat_col + 13 - strlen(buf));

		/* Race, class, and equipment modifiers */
		(void)sprintf(buf, "%3d", r_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 16);
		(void)sprintf(buf, "%3d", (int)e_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 22);

		/* Actual maximal modified value */
		cnv_stat(p_ptr->stat_top[i], buf);
		c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 26);

		/* Only display stat_use if not maximal */
		if (p_ptr->stat_use[i] < p_ptr->stat_top[i])
		{
			cnv_stat(p_ptr->stat_use[i], buf);
			c_put_str(TERM_YELLOW, buf, row + i+1, stat_col + 33);
		}
	}

	/* Column */
	col = stat_col + 41;

	/* Header and Footer */
	c_put_str(TERM_WHITE, "abcdefghijkl@", row, col);
#ifdef JP
	c_put_str(TERM_L_GREEN, "ǽ�Ͻ���", row - 1, col);
#else
	c_put_str(TERM_L_GREEN, "Modification", row - 1, col);
#endif


	/* Process equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Access object */
		o_ptr = &inventory[i];

		/* Acquire "known" flags */
		object_flags_known(o_ptr, flgs);

		/* Initialize color based of sign of to_stat[stat]. */
		for (stat = 0; stat < A_MAX; stat++)
		{
			int to_stat = o_ptr->to_stat[stat];

			/* Default */
			a = TERM_SLATE;
			c = '.';

			/* Boost */
			if (have_flag(flgs, stat))
			{
				/* Default */
				c = '*';

				/* Good */
				if (to_stat > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (to_stat < 10) c = '0' + to_stat;
				}

				/* 0 */
				else
				{
					c = '0';
				}

				if (have_flag(flgs, stat + TR_SUST_STR))
				{
					/* Dark green for sustained stats */
					a = TERM_GREEN;
				}

				/* Bad */
				if (to_stat < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (to_stat > -10) c = '0' - to_stat;
				}
			}

			/* Sustain */
			else if (have_flag(flgs, stat + TR_SUST_STR))
			{
				/* Dark green "s" */
				a = TERM_GREEN;
				c = 's';
			}

			/* Dump proper character */
			Term_putch(col, row + stat+1, a, c);
		}

		/* Advance */
		col++;
	}

	/* Check stats */
	for (stat = 0; stat < A_MAX; stat++)
	{
		/* Default */
		a = TERM_SLATE;
		c = '.';

		/* Mutations ... */
		if (p_ptr->muta3 || p_ptr->chargespell || p_ptr->zoshonel_protect)
		{
			int dummy = 0;

			if (stat == A_STR)
			{
				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy += 4;
				if (p_ptr->muta3 & MUT3_PUNY) dummy -= 4;
				if (p_ptr->zoshonel_protect) dummy += 4;
			}
			else if (stat == A_WIS || stat == A_INT)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
				if (p_ptr->chargespell) dummy += 4;
			}
			else if (stat == A_DEX)
			{
				if (p_ptr->muta3 & MUT3_IRON_SKIN) dummy -= 1;
				if (p_ptr->muta3 & MUT3_LIMBER) dummy += 3;
				if (p_ptr->muta3 & MUT3_ARTHRITIS) dummy -= 3;
				if (p_ptr->zoshonel_protect) dummy += 4;
			}
			else if (stat == A_CON)
			{
				if (p_ptr->muta3 & MUT3_RESILIENT) dummy += 4;
				if (p_ptr->muta3 & MUT3_XTRA_FAT) dummy += 2;
				if (p_ptr->muta3 & MUT3_ALBINO) dummy -= 4;
				if (p_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 2;
			}
			else if (stat == A_CHR)
			{
				if (p_ptr->muta3 & MUT3_SILLY_VOI) dummy -= 4;
				if (p_ptr->muta3 & MUT3_BLANK_FAC) dummy -= 1;
				if (p_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 1;
				if (p_ptr->muta3 & MUT3_SCALES) dummy -= 1;
				if (p_ptr->muta3 & MUT3_WART_SKIN) dummy -= 2;
				if (p_ptr->muta3 & MUT3_ILL_NORM) dummy = 0;
			}

			/* Boost */
			if (dummy)
			{
				/* Default */
				c = '*';

				/* Good */
				if (dummy > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (dummy < 10) c = '0' + dummy;
				}

				/* Bad */
				if (dummy < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (dummy > -10) c = '0' - dummy;
				}
			}
		}


		/* Sustain */
		if (have_flag(flgs, stat + TR_SUST_STR))
		{
			/* Dark green "s" */
			a = TERM_GREEN;
			c = 's';
		}


		/* Dump */
		Term_putch(col, row + stat+1, a, c);
	}
}


/*
 * Display the character on the screen (various modes)
 *
 * The top one and bottom two lines are left blank.
 *
 * Mode 0 = standard display with skills
 * Mode 1 = standard display with history
 * Mode 2 = summary of various things
 * Mode 3 = summary of various things (part 2)
 * Mode 4 = mutations
 */
void display_player(int mode)
{
	int i;

	char	buf[80];


	/* XXX XXX XXX */
	if ((p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3) && display_mutations)
		mode = (mode % 5);
	else
		mode = (mode % 4);

	/* Erase screen */
	clear_from(0);

	/* Standard */
	if ((mode == 0) || (mode == 1))
	{
		/* Name, Sex, Race, Class */
		display_player_one_line(ENTRY_NAME, player_name, TERM_L_BLUE);
		display_player_one_line(ENTRY_SEX, sp_ptr->title, TERM_L_BLUE);
		if (!(cp_ptr->c_flags & PCF_REINCARNATE)) display_player_one_line(ENTRY_RACE, p_name + rp_ptr->name, TERM_L_BLUE);
		display_player_one_line(ENTRY_CLASS, c_name + cp_ptr->name, TERM_L_BLUE);

		/* Age, Height, Weight, Social */
		/* ��Ĺ�ϥ�����᡼�ȥ�ˡ��νŤϥ��������ѹ����Ƥ���ޤ� */
#ifdef JP
		display_player_one_line(ENTRY_AGE, format("%d��" ,(int)p_ptr->age), TERM_L_BLUE);
		display_player_one_line(ENTRY_HEIGHT, format("%dcm" ,(int)((p_ptr->ht*254)/100)), TERM_L_BLUE);
		display_player_one_line(ENTRY_WEIGHT, format("%dkg" ,(int)((p_ptr->wt*4536)/10000)), TERM_L_BLUE);
		display_player_one_line(ENTRY_SOCIAL, format("%d  " ,(int)p_ptr->sc), TERM_L_BLUE);
#else
		display_player_one_line(ENTRY_AGE, format("%d" ,(int)p_ptr->age), TERM_L_BLUE);
		display_player_one_line(ENTRY_HEIGHT, format("%d" ,(int)p_ptr->ht), TERM_L_BLUE);
		display_player_one_line(ENTRY_WEIGHT, format("%d" ,(int)p_ptr->wt), TERM_L_BLUE);
		display_player_one_line(ENTRY_SOCIAL, format("%d" ,(int)p_ptr->sc), TERM_L_BLUE);
#endif
		if (p_ptr->no_elem) display_player_one_line (ENTRY_ELEM, "    ", TERM_DARK);
		else if ((p_ptr->opposite_pelem) && !(p_ptr->no_elem)) display_player_one_line(ENTRY_ELEM, format("%s(%s)", elem_names[get_cur_pelem()], elem_names[p_ptr->pelem]), TERM_L_BLUE);
		else display_player_one_line(ENTRY_ELEM, format("%s", elem_names[get_cur_pelem()]), TERM_L_BLUE);
		display_player_one_line(ENTRY_ALIGN, format("%s-%s", your_alignment_gne(), your_alignment_lnc()), TERM_L_BLUE);


		/* Display the stats */
		for (i = 0; i < A_MAX; i++)
		{
			/* Special treatment of "injured" stats */
			if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			{
				int value;

				/* Use lowercase stat name */
				put_str(stat_names_reduced[i], 3 + i, 54);

				/* Get the current stat */
				value = p_ptr->stat_use[i];

				/* Obtain the current stat (modified) */
				cnv_stat(value, buf);

				/* Display the current stat (modified) */
				c_put_str(TERM_YELLOW, buf, 3 + i, 61);

				/* Acquire the max stat */
				value = p_ptr->stat_top[i];

				/* Obtain the maximum stat (modified) */
				cnv_stat(value, buf);

				/* Display the maximum stat (modified) */
				c_put_str(TERM_L_GREEN, buf, 3 + i, 68);
			}

			/* Normal treatment of "normal" stats */
			else
			{
				/* Assume uppercase stat name */
				put_str(stat_names[i], 3 + i, 54);

				/* Obtain the current stat (modified) */
				cnv_stat(p_ptr->stat_use[i], buf);

				/* Display the current stat (modified) */
				c_put_str(TERM_L_GREEN, buf, 3 + i, 61);
			}

			if (p_ptr->stat_max[i] == STAT_MAX_MAX)
			{
#ifdef JP
				c_put_str(TERM_WHITE, "!", 3+i, 59);
#else
				c_put_str(TERM_WHITE, "!", 3+i, 59-2);
#endif
			}
		}

		/* Display "history" info */
		if (mode == 1)
		{
			char statmsg[1000];

#ifdef JP
			put_str("(����饯����������Ω��)", 11, 25);
#else
			put_str("(Character Background)", 11, 25);
#endif

			for (i = 0; i < 4; i++)
			{
				put_str(p_ptr->history[i], i + 12, 10);
			}

			*statmsg = '\0';

			if (p_ptr->is_dead && p_ptr->total_winner)
			{
				if (astral_mode)
#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��ϻ�Ԥε��¤������Ԥ�������ष����");
#else
					sprintf(statmsg, "...You retired from the adventure after surviving from Death Palace.");
#endif
				else if (streq(p_ptr->died_from, "walstanian"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ��״�������ǥ����륹���ͤΰŻ��Ԥ˰Ż����줿��");
				else if (streq(p_ptr->died_from, "gargastan"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ��״�������ǥ��륬������ͤΰŻ��Ԥ˰Ż����줿����");
				else if (streq(p_ptr->died_from, "bacrum"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ��״�������ǥХ����ͤΰŻ��Ԥ˰Ż����줿����");
				else if (streq(p_ptr->died_from, "z_or_l"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ��״����������Φ����ΰŻ��Ԥ˰Ż����줿����");
				else if (streq(p_ptr->died_from, "valeria"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ��״�������ǿȸ������ΰŻ��Ԥ˰Ż����줿����");
				else if (streq(p_ptr->died_from, "lord"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ������ꥢ�η���Ȥʤä���");
				else if (streq(p_ptr->died_from, "ogre"))
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ忿�Υ������Ȥʤä���");
				else
#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��Ͼ����θ�%s������", (p_ptr->is_dead & DEATH_SNAP_DRAGON) ? "�����Ѳ�" : "����");
#else
					sprintf(statmsg, "...You %s after the winning.", (p_ptr->is_dead & DEATH_SNAP_DRAGON) ? "were changed into a weapon" : "retired from the adventure");
#endif
			}
			else if (p_ptr->is_dead)
			{
				cptr die_verb;

				if (p_ptr->is_dead & DEATH_SNAP_DRAGON)
#ifdef JP
					die_verb = "��������Ѳ�����";
#else
					die_verb = "changed into a weapon";
#endif
				else if (p_ptr->is_dead & DEATH_STONED)
#ifdef JP
					die_verb = "�в����줿";
#else
					die_verb = "stoned";
#endif
				else
#ifdef JP
					die_verb = "�����줿";
#else
					die_verb = "killed";
#endif

				if (!dun_level)
				{
#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ���%s��%s��%s��", map_name(), p_ptr->died_from, die_verb);
#else
					sprintf(statmsg, "...You were %s by %s in %s.", die_verb, p_ptr->died_from, map_name());
#endif
				}
				else if (p_ptr->inside_quest && quest_is_fixed(p_ptr->inside_quest))
				{
					/* Get the quest text */
					/* Beware that INIT_ASSIGN resets the cur_num. */
					init_flags = INIT_ASSIGN;

					process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��ϡ��������ȡ�%s�פ�%s��%s��", quest[p_ptr->inside_quest].name, p_ptr->died_from, die_verb);
#else
					sprintf(statmsg, "...You were %s by %s in the quest '%s'.", die_verb, p_ptr->died_from, quest[p_ptr->inside_quest].name);
#endif
				}
				else
				{
#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��ϡ�%s��%d����%s��%s��", map_name(), dun_level, p_ptr->died_from, die_verb);
#else
					sprintf(statmsg, "...You were %s by %s on level %d of %s.", die_verb, p_ptr->died_from, dun_level, map_name());
#endif
				}
			}
			else if (character_dungeon)
			{
				if (!dun_level)
				{
#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��ϸ��ߡ�%s�ˤ��롣", map_name());
#else
					sprintf(statmsg, "...Now, you are in %s.", map_name());
#endif
				}
				else if (p_ptr->inside_quest && quest_is_fixed(p_ptr->inside_quest))
				{
					/* Clear the text */
					/* Must be done before doing INIT_SHOW_TEXT */
					for (i = 0; i < 10; i++)
					{
						quest_text[i][0] = '\0';
					}
					quest_text_line = 0;

					/* Get the quest text */
					init_flags = INIT_SHOW_TEXT;

					process_dungeon_file("q_info.txt", 0, 0, 0, 0);

#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��ϸ��ߡ��������ȡ�%s�פ��������", quest[p_ptr->inside_quest].name);
#else
					sprintf(statmsg, "...Now, you are in the quest '%s'.", quest[p_ptr->inside_quest].name);
#endif
				}
				else
				{
#ifdef JP
					sprintf(statmsg, "�Ĥ��ʤ��ϸ��ߡ�%s��%d����õ�����Ƥ��롣", map_name(), dun_level);
#else
					sprintf(statmsg, "...Now, you are exploring level %d of %s.", dun_level, map_name());
#endif
				}
			}

			if (*statmsg)
			{
				char temp[64*2], *t;
				roff_to_buf(statmsg, 60, temp, sizeof temp);
				t = temp;
				for(i=0 ; i<2 ; i++)
				{
					if(t[0]==0)
						break; 
					else
					{
						put_str(t, i + 5 + 12, 10);
						t += strlen(t)+1;
					}
				}
			}

		}

		/* Display "various" info */
		else
		{
			display_player_middle();
			display_player_various();
		}
	}

	/* Special */
	else if (mode == 2)
	{
		/* See "http://www.cs.berkeley.edu/~davidb/angband.html" */

		/* Dump the info */
		display_player_misc_info();
		display_player_stat_info();
		display_player_flag_info();
	}

	/* Special */
	else if (mode == 3)
	{
		display_player_other_flag_info();
	}

	else if (mode == 4)
	{
		do_cmd_knowledge_mutations();
	}
}

/*
 *
 */
static void dump_aux_display_player(FILE *fff)
{
	int x, y;
	byte a;
	char c;
	char		buf[1024];

	/* Display player */
	display_player(0);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
#ifdef JP
		fprintf(fff, "%s\n", buf);
#else
		fprintf(fff, "%s\n", buf);
#endif

	}

	/* Display history */
	display_player(1);

	/* Dump part of the screen */
	for (y = 10; y < 19; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 1) */
	display_player(2);

	/* Dump part of the screen */
	for (y = 2; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if (a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");

	/* Display flags (part 2) */
	display_player(3);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it (Ignore equippy tile graphic) */
			if (a < 128)
				buf[x] = c;
			else
				buf[x] = ' ';
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");
}


/*
 *
 */
static void dump_aux_pet(FILE *fff)
{
	int i;
	bool pet = FALSE;
	char pet_name[80];

	for (i = m_max - 1; i >= 1; i--)
	{
		monster_type *m_ptr = &m_list[i];

		if (!m_ptr->r_idx) continue;
		if (!is_pet(m_ptr)) continue;
		if (!m_ptr->nickname && (p_ptr->riding != i)) continue;
		if (!pet)
		{
#ifdef JP
			fprintf(fff, "\n  [��ʥڥå�]\n\n");
#else
			fprintf(fff, "\n  [Leading Pets]\n\n");
#endif
			pet = TRUE;
		}
		monster_desc(pet_name, m_ptr, MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
		fprintf(fff, "%s\n", pet_name);
	}
	if (pet) fprintf(fff, "\n");
}


/*
 *
 */
static void dump_aux_stock_pet(FILE *fff)
{
	int i, stock_mon_num;
	cptr paren = ")";
	char m_name[80];

	for (stock_mon_num = 0; stock_mon_num < MAX_STOCK_MON; stock_mon_num++)
	{
		if (!stock_mon[stock_mon_num].r_idx) break;
	}

	if (stock_mon_num)
	{
#ifdef JP
		fprintf(fff, "\n  [�椬�ȤΥڥå�]\n\n");
#else
		fprintf(fff, "\n  [Home Pets]\n\n");
#endif

		/* Dump all available pets */
		for (i = 0; i < stock_mon_num; i++)
		{
			monster_desc(m_name, &stock_mon[i], MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);
			fprintf(fff, "%c%s %s\n", I2A(i), paren, m_name);
		}

		/* Add an empty line */
		fprintf(fff, "\n");
	}
}


/*
 *
 */
static void dump_aux_class_history(FILE *fff)
{
	int i;

#ifdef JP
	fprintf(fff, "\n  [�и����Ƥ������饹]\n\n");
	fprintf(fff, "���饹                 ��٥�             �и���  \n");
#else
	fprintf(fff, "\n  [Experienced Classes]\n\n");
	fprintf(fff, "Class                  Level                 Exp  \n");
#endif
	fprintf(fff, "--------------------------------------------------\n");

	for (i = 0; i < max_c_idx; i++)
	{
		cexp_info_type *cexp_ptr = &p_ptr->cexp_info[i];

		if (!cexp_ptr->max_max_clev) continue;

		if (cexp_ptr->max_clev)
		{
#ifdef L64
			fprintf(fff, "%-16s   %2d/%2d(/%2d)  %8d/%8d\n", c_name + class_info[i].name,
				cexp_ptr->clev, cexp_ptr->max_clev, cexp_ptr->max_max_clev, cexp_ptr->cexp, cexp_ptr->max_cexp);
#else
			fprintf(fff, "%-16s   %2ld/%2ld(/%2ld)  %8ld/%8ld\n", c_name + class_info[i].name,
				cexp_ptr->clev, cexp_ptr->max_clev, cexp_ptr->max_max_clev, cexp_ptr->cexp, cexp_ptr->max_cexp);
#endif
		}
		else
		{
#ifdef L64
			fprintf(fff, "%-16s   --/--(/%2d)  --------/--------\n",
				c_name + class_info[i].name, cexp_ptr->max_max_clev);
#else
			fprintf(fff, "%-16s   --/--(/%2ld)  --------/--------\n",
				c_name + class_info[i].name, cexp_ptr->max_max_clev);
#endif
		}
	}

	fprintf(fff, "\n");

}


/*
 *
 */
static void dump_aux_quest(FILE *fff)
{
	int i;
	int *quest_num;
	int dummy;

#ifdef JP
	fprintf(fff, "\n  [�������Ⱦ���]\n");
#else
	fprintf(fff, "\n  [Quest Information]\n");
#endif

	/* Allocate Memory */
	C_MAKE(quest_num, max_quests, int);

	/* Sort by compete level */
	for (i = 1; i < max_quests; i++)
	{
		quest_num[i] = i;
	}
	ang_sort_comp = ang_sort_comp_quest_num;
	ang_sort_swap = ang_sort_swap_quest_num;
	ang_sort(quest_num, &dummy, max_quests);

	/* Dump Quest Information */
	fputc('\n', fff);
	do_cmd_knowledge_quests_completed(fff, quest_num);
	fputc('\n', fff);
	do_cmd_knowledge_quests_failed(fff, quest_num);

	/* Free Memory */
	C_KILL(quest_num, max_quests, int);

	if (quest[QUEST_BARMAMUTHA_L].status == QUEST_STATUS_FINISHED)
	{
		fprintf(fff, "  ���ʤ��ϥХ�ޥ�å��Υ����륹���ͼ����轻̱�򳧻����ˤ�����\n\n");
	}
	else if (quest[QUEST_BARMAMUTHA_C].status == QUEST_STATUS_FINISHED)
	{
		fprintf(fff, "  ���ʤ��ϥХ�ޥ�å��εԻ���ȿ�Ф�������������ä���\n\n");
	}

}


/*
 *
 */
static void dump_aux_last_message(FILE *fff)
{
	int i;

	if (p_ptr->is_dead && !p_ptr->total_winner)
	{
#ifdef JP
		fprintf(fff, "\n  [%sľ���Υ�å�����]\n\n", (p_ptr->is_dead & DEATH_SNAP_DRAGON) ? "�����Ѥ��" : "���");
#else
		fprintf(fff, "\n  [Last Messages]\n\n");
#endif
		for (i = MIN(message_num(), 30); i >= 0; i--)
		{
			fprintf(fff,"> %s\n",message_str((s16b)i));
		}
		fprintf(fff, "\n");
	}

}


/*
 *
 */
static void dump_aux_recall(FILE *fff)
{
	int y;

#ifdef JP
	fprintf(fff, "\n ���Ծ��:\n");
#else
	fprintf(fff, "\n Recall Depth:\n");
#endif
	for (y = 1; y < max_d_idx; y++)
	{
		bool seiha = FALSE;

		if (!d_info[y].maxdepth) continue;
		if (!max_dlv[y]) continue;
		if (d_info[y].final_guardian)
		{
			if (!r_info[d_info[y].final_guardian].max_num) seiha = TRUE;
		}
		else if (max_dlv[y] == d_info[y].maxdepth) seiha = TRUE;

#ifdef JP
		fprintf(fff, "   %c%-12s: %3d ��\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#else
		fprintf(fff, "   %c%-16s: level %3d\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#endif
	}
}


/*
 *
 */
static void dump_aux_options(FILE *fff)
{
#ifdef JP
	fprintf(fff, "\n  [���ץ��������]\n");
#else
	fprintf(fff, "\n  [Option settings]\n");
#endif


	if (easy_band)
#ifdef JP
		fprintf(fff, "\n �鿴���Ѵ�ñ�⡼��: ON");
#else
		fprintf(fff, "\n Easy Mode:          ON");
#endif

	if (preserve_mode)
#ifdef JP
		fprintf(fff, "\n ��¸�⡼��:         ON");
#else
		fprintf(fff, "\n Preserve Mode:      ON");
#endif

	else
#ifdef JP
		fprintf(fff, "\n ��¸�⡼��:         OFF");
#else
		fprintf(fff, "\n Preserve Mode:      OFF");
#endif


	if (always_small_levels)
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   ON");
#else
		fprintf(fff, "\n Small Levels:       ON");
#endif

	else if (small_levels)
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   ENABLED");
#else
		fprintf(fff, "\n Small Levels:       ENABLED");
#endif

	else
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   OFF");
#else
		fprintf(fff, "\n Small Levels:       OFF");
#endif


	if (ironman_rooms)
#ifdef JP
		fprintf(fff, "\n ���̤Ǥʤ�����:     ON");
#else
		fprintf(fff, "\n Unusual Rooms:      ON");
#endif


	if (astral_mode)
#ifdef JP
		fprintf(fff, "\n �����ȥ��⡼��:   ON");
#else
		fprintf(fff, "\n Astral Mode:        ON");
#endif


	if (empty_levels)
#ifdef JP
		fprintf(fff, "\n ���꡼��:           ON");
#else
		fprintf(fff, "\n Arena Levels:       ON");
#endif

	else
#ifdef JP
		fprintf(fff, "\n ���꡼��:           OFF");
#else
		fprintf(fff, "\n Arena Levels:       OFF");
#endif


	fprintf(fff, "\n");

	if (p_ptr->noscore)
#ifdef JP
		fprintf(fff, "\n ���������ʤ��Ȥ򤷤Ƥ��ޤäƤޤ���");
#else
		fprintf(fff, "\n You have done something illegal.");
#endif

	fprintf(fff, "\n");

}


/*
 *
 */
static void dump_aux_arena(FILE *fff)
{
	if (astral_mode) return;

	if ((p_ptr->arena_number == 99) || (p_ptr->arena_number < 0))
	{
		if (p_ptr->arena_number == 99)
		{
#ifdef JP
			fprintf(fff, "\n Ʈ����: ����\n");
#else
			fprintf(fff, "\n Arena: Defeated\n");
#endif
		}
		else
		{
#ifdef JP
			fprintf(fff, "\n Ʈ����: %d�����%s����������\n", -p_ptr->arena_number,
				r_name + r_info[arena_info[-1 - p_ptr->arena_number].r_idx].name);
#else
			fprintf(fff, "\n Arena: Defeated by %s in the %d%s fight\n",
				r_name + r_info[arena_info[-1 - p_ptr->arena_number].r_idx].name,
				-p_ptr->arena_number, get_ordinal_number_suffix(-p_ptr->arena_number));
#endif
		}
	}
	else if (p_ptr->arena_number > (MAX_ARENA_MONS + 4))
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: ��ˤΥ����ԥ���\n");
#else
		fprintf(fff, "\n Arena: Ultimate Champion\n");
#endif
	}
	else if (p_ptr->arena_number > (MAX_ARENA_MONS + 2))
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: ���Υ����ԥ���\n");
#else
		fprintf(fff, "\n Arena: Higher Champion\n");
#endif
	}
	else if (p_ptr->arena_number > (MAX_ARENA_MONS - 1))
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: �����ԥ���\n");
#else
		fprintf(fff, "\n Arena: Champion\n");
#endif
	}
	else
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: %2d��\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number));
#else
		fprintf(fff, "\n Arena: %2d Victor%s\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number), (p_ptr->arena_number>1) ? "ies" : "y");
#endif
	}

	fprintf(fff, "\n");
}


/*
 *
 */
static void dump_aux_monsters(FILE *fff)
{
	/* Monsters slain */

	int k;
	long uniq_total = 0;
	long norm_total = 0;
	s16b *who;

	/* Sort by monster level */
	u16b why = 2;

#ifdef JP
	fprintf(fff, "\n  [�ݤ�����󥹥���]\n\n");
#else
	fprintf(fff, "\n  [Defeated monsters]\n\n");
#endif

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

		for (k = 1; k < (max_r_idx + runeweapon_num); k++)
		{
			monster_race *r_ptr = &r_info[k];

			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				bool dead = (r_ptr->max_num == 0);

				if (!(misc_event_flags & EVENT_LIBERATION_OF_ARMORICA) && (k == MON_RONWE)) dead = FALSE;

				if (monster_is_runeweapon(k))
				{
					if (!(runeweapon_list[runeweapon_num_from(k)].status & RW_STATUS_FOUND)) dead = FALSE;
				}
				if (dead)
				{
					norm_total++;

					/* Add a unique monster to the list */
					who[uniq_total++] = k;
				}
			}

			/* Normal monsters */
			else
			{
				if (r_ptr->r_pkills > 0)
				{
					norm_total += r_ptr->r_pkills;
				}
			}
		}

		if (norm_total < 1)
#ifdef JP
			fprintf(fff,"\n �ޤ�Ũ���ݤ��Ƥ��ޤ���\n");
#else
			fprintf(fff,"\n You have defeated no enemies yet.\n");
#endif

	/* Defeated more than one normal monsters */
	else if (uniq_total == 0)
	{
#ifdef JP
		fprintf(fff,"%ld�Τ�Ũ���ݤ��Ƥ��ޤ���\n", norm_total);
#else
		fprintf(fff,"You have defeated %ld %s.\n", norm_total, norm_total == 1 ? "enemy" : "enemies");
#endif
	}
	/* Defeated more than one unique monsters */
	else /* if (uniq_total > 0) */
	{
#ifdef JP
		fprintf(fff, "%ld�ΤΥ�ˡ�������󥹥�����ޤࡢ���%ld�Τ�Ũ���ݤ��Ƥ��ޤ���\n", uniq_total, norm_total); 
#else
		fprintf(fff, "You have defeated %ld %s including %ld unique monster%s in total.\n", norm_total, norm_total == 1 ? "enemy" : "enemies", uniq_total, (uniq_total == 1 ? "" : "s"));
#endif


		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array by dungeon depth of monsters */
		ang_sort(who, &why, uniq_total);

#ifdef JP
		fprintf(fff, "\n�Ծ��%ld�ΤΥ�ˡ�������󥹥�����\n", MIN(uniq_total, 10));
#else
		fprintf(fff, "\n< Unique monsters top %ld >\n", MIN(uniq_total, 10));
#endif

		/* Print top 10 */
		for (k = uniq_total - 1; k >= 0 && k >= uniq_total - 10; k--)
		{
			monster_race *r_ptr = &r_info[who[k]];

			if (!(misc_event_flags & EVENT_LIBERATION_OF_ARMORICA) && (who[k] == MON_RONWE)) continue;

#ifdef JP
			fprintf(fff, "  %-40s (��٥�%3d)\n", (r_name + r_ptr->name), r_ptr->level); 
#else
			fprintf(fff, "  %-40s (level %3d)\n", (r_name + r_ptr->name), r_ptr->level); 
#endif
		}

	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, s16b);
}


/*
 *
 */
static void dump_aux_resurrection_cnt(FILE *fff)
{
	if (p_ptr->resurrection_cnt)
	{
#ifdef JP
		fprintf(fff,"\n ��ˡ�䥢���ƥ�ʤɤˤ���������: %d ��\n", p_ptr->resurrection_cnt);
#else
		fprintf(fff,"\n Count of self resurrection by magic, item, etc.: %d times\n", p_ptr->resurrection_cnt);
#endif
	}
}


/*
 *
 */
static void dump_aux_materialize_cnt(FILE *fff)
{
	if (p_ptr->materialize_cnt)
	{
#ifdef JP
		fprintf(fff,"\n ��ˡ�������󥹥����������: %d ��\n", p_ptr->materialize_cnt);
#else
		fprintf(fff,"\n Unique monster resurrection count: %d times\n", p_ptr->materialize_cnt);
#endif
	}
}


/*
 *
 */
static void dump_aux_reincarnate_cnt(FILE *fff)
{
	if (p_ptr->reincarnate_cnt)
	{
#ifdef JP
#ifdef L64
		fprintf(fff,"\n�꡼�󥫡��ͥ��Ȳ��: %d �� (�ǹ��٥�: %d)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#else
		fprintf(fff,"\n�꡼�󥫡��ͥ��Ȳ��: %d �� (�ǹ��٥�: %ld)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#endif
#else
#ifdef L64
		fprintf(fff,"\nReincarnation count: %d times (Maximum level: %d)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#else
		fprintf(fff,"\nReincarnation count: %d times (Maximum level: %ld)\n", p_ptr->reincarnate_cnt, p_ptr->max_max_plv);
#endif
#endif
	}
}


/*
 *
 */
static void dump_aux_chaos_frame(FILE *fff)
{
	int i;

#ifdef JP
	fprintf(fff, "\n\n  [�������ե졼��]\n\n");
#else
	fprintf(fff, "\n\n  [Chaos Frame]\n\n");
#endif
	for (i = 0; i < ETHNICITY_NUM; i++)
	{
		fprintf(fff, "%20s  %+4d\n", ethnicity_names[i], chaos_frame[i]);
	}

}


/*
 *
 */
static void dump_aux_mutations(FILE *fff)
{
	if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
	{
#ifdef JP
		fprintf(fff, "\n\n  [�����Ѱ�]\n\n");
#else
		fprintf(fff, "\n\n  [Mutations]\n\n");
#endif

		dump_mutations(fff);
	}

	/* Skip some lines */
	fprintf(fff, "\n\n");
}


/*
 *
 */
static void dump_aux_runeweapon_ability(FILE *fff)
{
	if (p_ptr->is_dead & DEATH_SNAP_DRAGON)
	{
		/* Dump the "Snap Dragon" weapon */
		object_type *o_ptr;
		char o_name[MAX_NLEN];

#ifdef JP
		fprintf(fff, "  [���Ȥ��Ƥ�ǽ��]\n\n");
#else
		fprintf(fff, "  [Character Ability as a Runeweapon]\n\n");
#endif
		o_ptr = &runeweapon_list[0].weapon;
		object_desc(o_name, o_ptr, 0);
		fprintf(fff, "%s\n", o_name);
		screen_object(o_ptr, fff, TRUE);

		fprintf(fff, "\n\n");
	}
}


/*
 *
 */
static void dump_aux_equipment_inventory(FILE *fff)
{
	int i;
	cptr paren = ")";
	char o_name[MAX_NLEN];

	/* Dump the equipment */
	if (equip_cnt)
	{
#ifdef JP
		fprintf(fff, "  [����饯��������]\n\n");
#else
		fprintf(fff, "  [Character Equipment]\n\n");
#endif

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			object_desc(o_name, &inventory[i], 0);
			if ((i == INVEN_LARM) && p_ptr->ryoute)
#ifdef JP
				strcpy(o_name, "(����ξ�����)");
#else
				strcpy(o_name, "(wielding with two-hands)");
#endif
			fprintf(fff, "%c%s %s\n",
				index_to_label(i), paren, o_name);
		}
		fprintf(fff, "\n\n");
	}

	/* Dump the inventory */
#ifdef JP
	fprintf(fff, "  [����饯���λ���ʪ]\n\n");
#else
	fprintf(fff, "  [Character Inventory]\n\n");
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Don't dump the empty slots */
		if (!inventory[i].k_idx) break;

		/* Dump the inventory slots */
		object_desc(o_name, &inventory[i], 0);
		fprintf(fff, "%c%s %s\n", index_to_label(i), paren, o_name);
	}

	/* Add an empty line */
	fprintf(fff, "\n\n");
}


/*
 *
 */
static void dump_aux_home_museum(FILE *fff)
{
	cptr		paren = ")";
	char o_name[MAX_NLEN];
	store_type  *st_ptr;

	/* Do we need it?? */
	/* process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x); */

	/* Print the home */
	st_ptr = &town[1].store[STORE_HOME];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		int i;
		int x = 1;

#ifdef JP
		fprintf(fff, "  [�椬�ȤΥ����ƥ�]\n");
#else
		fprintf(fff, "  [Home Inventory]\n");
#endif

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if ((i % 12) == 0)
#ifdef JP
				fprintf(fff, "\n ( %d �ڡ��� )\n", x++);
#else
				fprintf(fff, "\n ( page %d )\n", x++);
#endif
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}


	/* Print the home */
	st_ptr = &town[1].store[STORE_MUSEUM];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		int i;
		int x = 1;

#ifdef JP
		fprintf(fff, "  [��ʪ�ۤΥ����ƥ�]\n");
#else
		fprintf(fff, "  [Museum]\n");
#endif

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
			if ((i % 12) == 0) fprintf(fff, "\n ( %d �ڡ��� )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
#else
			if ((i % 12) == 0) fprintf(fff, "\n ( page %d )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], 0);
			fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
#endif

		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}
}


/*
 * Output the character dump to a file
 */
errr make_character_dump(FILE *fff)
{
#ifdef JP
	fprintf(fff, "  [TOband2 %d.%d.%d ����饯������]\n\n",
		T_VER_MAJOR, T_VER_MINOR, T_VER_PATCH);
#else
	fprintf(fff, "  [TOband2 %d.%d.%d Character Dump]\n\n",
		T_VER_MAJOR, T_VER_MINOR, T_VER_PATCH);
#endif

	update_playtime();

	dump_aux_display_player(fff);
	dump_aux_last_message(fff);
	dump_aux_class_history(fff);
	dump_aux_pet(fff);
	dump_aux_stock_pet(fff);
	dump_aux_quest(fff);

#ifdef JP
	fprintf(fff, "\n\n  [����¾�ξ���]\n");
#else
	fprintf(fff, "\n\n  [Miscellaneous Information]\n");
#endif

	dump_aux_recall(fff);
	dump_aux_arena(fff);
	dump_aux_resurrection_cnt(fff);
	dump_aux_materialize_cnt(fff);
	dump_aux_reincarnate_cnt(fff);
	dump_aux_options(fff);
	dump_aux_monsters(fff);
	dump_aux_chaos_frame(fff);
	dump_aux_mutations(fff);
	dump_aux_runeweapon_ability(fff);
	dump_aux_equipment_inventory(fff);
	dump_aux_home_museum(fff);

#ifdef JP
	fprintf(fff, "  [�����å�����: \"%s\"]\n\n", get_check_sum());
#else
	fprintf(fff, "  [Check Sum: \"%s\"]\n\n", get_check_sum());
#endif

	return 0;
}

/*
 * Hack -- Dump a character description file
 */
errr file_character(cptr name)
{
	int		fd = -1;
	FILE		*fff = NULL;
	char		buf[1024];

	/* Drop priv's */
	safe_setuid_drop();

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		(void)fd_close(fd);

		/* Build query */
#ifdef JP
		(void)sprintf(out_val, "��¸����ե����� %s �˾�񤭤��ޤ���? ", buf);
#else
		(void)sprintf(out_val, "Replace existing file %s? ", buf);
#endif


		/* Ask */
		if (get_check_strict(out_val, CHECK_NO_HISTORY)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* Invalid file */
	if (!fff)
	{
		/* Message */
#ifdef JP
		prt("����饯������Υե�����ؤν񤭽Ф��˼��Ԥ��ޤ�����", 0, 0);
#else
		prt("Character dump failed!", 0, 0);
#endif

		(void)inkey();

		/* Error */
		return (-1);
	}

	(void)make_character_dump(fff);

	/* Close it */
	my_fclose(fff);

	/* Grab priv's */
	safe_setuid_grab();

	/* Message */
#ifdef JP
	msg_print("����饯������Υե�����ؤν񤭽Ф����������ޤ�����");
#else
	msg_print("Character dump successful.");
#endif

	msg_print(NULL);

	/* Success */
	return (0);
}


/*
 * Recursive file perusal.
 *
 * Return FALSE on 'Q', otherwise TRUE.
 *
 * Process various special text in the input file, including
 * the "menu" structures used by the "help file" system.
 *
 * XXX XXX XXX Consider using a temporary file.
 *
 * XXX XXX XXX Allow the user to "save" the current file.
 */
bool show_file(bool show_version, cptr name, cptr what, int line, int mode)
{
	int i, n, k;

	/* Number of "real" lines passed by */
	int next = 0;

	/* Number of "real" lines in the file */
	int size = 0;

	/* Backup value for "line" */
	int back = 0;

	/* Color of the next line */
	byte color = TERM_WHITE;

	/* Loop counter */
	int cnt;

	/* This screen has sub-screens */
	bool menu = FALSE;

	/* Current help file */
	FILE *fff = NULL;

	/* Find this string (if any) */
	cptr find = NULL;

	/* Jump to this tag */
	cptr tag = NULL;

	/* Hold a string to find */
	char finder[81];

	/* Hold a string to show */
	char shower[81];

	/* Filename */
	char filename[1024];

	/* Describe this thing */
	char caption[128];

	/* Path buffer */
	char path[1024];

	/* General buffer */
	char buf[1024];

	/* Lower case version of the buffer, for searching */
	char lc_buf[1024];

	/* Aux pointer for making lc_buf (and find!) lowercase */
	cptr lc_buf_ptr;

	/* Sub-menu information */
	char hook[68][32];

	bool reverse = (line < 0);

	int wid, hgt, rows;

	Term_get_size(&wid, &hgt);
	rows = hgt - 4;

	/* Wipe finder */
	strcpy(finder, "");

	/* Wipe shower */
	strcpy(shower, "");

	/* Wipe caption */
	strcpy(caption, "");

	/* Wipe the hooks */
	for (i = 0; i < 68; i++)
	{
		hook[i][0] = '\0';
	}

	/* Copy the filename */
	strcpy(filename, name);

	n = strlen(filename);

	/* Extract the tag from the filename */
	for (i = 0; i < n; i++)
	{
		if (filename[i] == '#')
		{
			filename[i] = '\0';
			tag = filename + i + 1;
			break;
		}
	}

	/* Redirect the name */
	name = filename;

	/* Hack XXX XXX XXX */
	if (what)
	{
		/* Caption */
		strcpy(caption, what);

		/* Access the "file" */
		strcpy(path, name);

		/* Open */
		fff = my_fopen(path, "r");
	}

	/* Look in "help" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
		sprintf(caption, "�إ�ס��ե�����'%s'", name);
#else
		sprintf(caption, "Help file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR_HELP, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
		sprintf(caption, "���ݥ��顼���ե�����'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR_INFO, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Build the filename */
		path_build(path, sizeof(path), ANGBAND_DIR, name);

		for (i = 0; path[i]; i++)
			if ('\\' == path[i])
				path[i] = PATH_SEP[0];

		/* Caption */
#ifdef JP
		sprintf(caption, "���ݥ��顼���ե�����'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Oops */
	if (!fff)
	{
		/* Message */
#ifdef JP
		msg_format("'%s'�򥪡��ץ�Ǥ��ޤ���", name);
#else
		msg_format("Cannot open '%s'.", name);
#endif

		msg_print(NULL);

		/* Oops */
		return (TRUE);
	}


	/* Pre-Parse the file */
	while (TRUE)
	{
		char *str;

		/* Read a line or stop */
		if (my_fgets(fff, buf, sizeof(buf))) break;

		/* Get a color */
		if (prefix(buf, "#####")) str = &buf[6];
		else str = buf;

		/* XXX Parse "menu" items */
		if (prefix(str, "***** "))
		{
			/* Notice "menu" requests */
			if ((str[6] == '[') && isalpha(str[7]))
			{
				/* This is a menu file */
				menu = TRUE;

				/* Extract the menu item */
				k = str[7] - 'A';

				if ((str[8] == ']') && (str[9] == ' '))
				{
					/* Extract the menu item */
					strncpy(hook[k], str + 10, 31);

					/* Make sure it's null-terminated */
					hook[k][31] = '\0';
				}
			}
			/* Notice "tag" requests */
			else if (str[6] == '<')
			{
				str[strlen(str) - 1] = '\0';
				if (tag && streq(str + 7, tag)) line = next;
			}

			/* Skip this */
			continue;
		}

		/* Count the "real" lines */
		next++;
	}

	/* Save the number of "real" lines */
	size = next;

	/* start from bottom when reverse mode */
	if (line == -1) line = ((size-1)/rows)*rows;

	/* Display the file */
	while (TRUE)
	{
		/* Clear screen */
		Term_clear();

		/* Restart when necessary */
		if (line >= size - rows) line = size - rows;
		if (line < 0) line = 0;

		/* Re-open the file if needed */
		if (next > line)
		{
			/* Close it */
			my_fclose(fff);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			/* Oops */
			if (!fff) return (FALSE);

			/* File has been restarted */
			next = 0;
		}

		/* Goto the selected line */
		while (next < line)
		{
			/* Get a line */
			if (my_fgets(fff, buf, sizeof(buf))) break;

			/* Skip tags/links */
			if (prefix(buf, "***** ")) continue;

			/* Count the lines */
			next++;
		}

		/* Dump the next 20, or rows, lines of the file */
		for (i = 0; i < rows; )
		{
			int print_x, x;
			cptr str;

			/* Hack -- track the "first" line */
			if (!i) line = next;

			/* Get a line of the file or stop */
			if (my_fgets(fff, buf, sizeof(buf))) break;

			/* Hack -- skip "special" lines */
			if (prefix(buf, "***** ")) continue;

			/* Get a color */
			if (prefix(buf, "#####"))
			{
				color = color_char_to_attr(buf[5]);
				str = &buf[6];
			}
			else
			{
				color = TERM_WHITE;
				str = buf;
			}

			/* Count the "real" lines */
			next++;

			/* Make a lower case version of str for searching */
			strcpy(lc_buf, str);

			for (lc_buf_ptr = lc_buf; *lc_buf_ptr != 0; lc_buf_ptr++)
			{
#ifdef JP
				if (iskanji(*lc_buf_ptr))
					lc_buf_ptr++;
				else
#endif
					lc_buf[lc_buf_ptr-lc_buf] = tolower(*lc_buf_ptr);
			}

			/* Hack -- keep searching */
			if (find && !i && !my_strstr(lc_buf, find)) continue;

			/* Hack -- stop searching */
			find = NULL;

			/* Dump the line */
			x = 0;
			print_x = 0;
			while (str[x])
			{
				/* Color ? */
				if (prefix(str + x, "[[[[["))
				{
					byte c = color_char_to_attr(str[x + 5]);
					x += 6;

					/* Ok print the link name */
					while (str[x] != ']')
					{
						Term_putch(print_x, i + 2, c, str[x]);
						x++;
						print_x++;
					}
				}
				else
				{
					Term_putch(print_x, i + 2, color, str[x]);
					print_x++;
				}

				x++;
			}

			/* Hilite "shower" */
			if (shower[0])
			{
				cptr s2 = lc_buf;

				/* Display matches */
				while ((s2 = my_strstr(s2, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(s2-lc_buf, i+2, len, TERM_YELLOW, &str[s2-lc_buf]);

					/* Advance */
					s2 += len;
				}
			}

			/* Count the printed lines */
			i++;
		}

		/* Hack -- failed search */
		if (find)
		{
			bell();
			line = back;
			find = NULL;
			continue;
		}


		/* Show a general "title" */
		if (show_version)
		{
			prt(format(
#ifdef JP
				"[TOband2 %d.%d.%d, %s, %d/%d]",
#else
				"[TOband2 %d.%d.%d, %s, Line %d/%d]",
#endif

			    T_VER_MAJOR, T_VER_MINOR, T_VER_PATCH,
			    caption, line, size), 0, 0);
		}
		else
		{
			prt(format(
#ifdef JP
				"[%s, %d/%d]",
#else
				"[%s, Line %d/%d]",
#endif
				caption, line, size), 0, 0);
		}

		/* Prompt -- small files */
		if (size <= rows)
		{
			/* Wait for it */
#ifdef JP
			prt("[����:(?)�إ�� (ESC)��λ]", hgt - 1, 0);
#else
			prt("[Press ESC to exit.]", hgt - 1, 0);
#endif

		}

		/* Prompt -- large files */
		else
		{
#ifdef JP
			if(reverse)
				prt("[����:(RET/���ڡ���)�� (-)�� (?)�إ�� (ESC)��λ]", hgt - 1, 0);
			else
				prt("[����:(RET/���ڡ���)�� (-)�� (?)�إ�� (ESC)��λ]", hgt - 1, 0);
#else
			prt("[Press Return, Space, -, =, /, |, or ESC to exit.]", hgt - 1, 0);
#endif
		}

		/* Get a keypress */
		k = inkey();

		/* Hack -- return to last screen */
		if (k == '<') break;

		/* Show the help for the help */
		if (k == '?')
		{
			/* Hack - prevent silly recursion */
#ifdef JP
			if (strcmp(name, "jhelpinfo.txt") != 0)
				show_file(TRUE, "jhelpinfo.txt", NULL, 0, mode);
#else
			if (strcmp(name, "helpinfo.txt") != 0)
				show_file(TRUE, "helpinfo.txt", NULL, 0, mode);
#endif
		}

		/* Hack -- try showing */
		if (k == '=')
		{
			/* Get "shower" */
#ifdef JP
			prt("��Ĵ: ", hgt - 1, 0);
#else
			prt("Show: ", hgt - 1, 0);
#endif

			(void)askfor_aux(shower, 80);
		}

		/* Hack -- try finding */
		if (k == '/')
		{
			/* Get "finder" */
#ifdef JP
			prt("����: ", hgt - 1, 0);
#else
			prt("Find: ", hgt - 1, 0);
#endif


			if (askfor_aux(finder, 80))
			{
				/* Find it */
				find = finder;
				back = line;
				line = line + 1;

				/* Make finder lowercase */
				for (cnt = 0; finder[cnt] != 0; cnt++)
				{
#ifdef JP
					if (iskanji(finder[cnt]))
						cnt++;
					else
#endif
						finder[cnt] = tolower(finder[cnt]);
				}

				/* Show it */
				strcpy(shower, finder);
			}
		}

		/* Hack -- go to a specific line */
		if (k == '#')
		{
			char tmp[81];
#ifdef JP
			prt("��: ", hgt - 1, 0);
#else
			prt("Goto Line: ", hgt - 1, 0);
#endif

			strcpy(tmp, "0");

			if (askfor_aux(tmp, 80))
			{
				line = atoi(tmp);
			}
		}

		/* Hack -- go to a specific file */
		if (k == '%')
		{
			char tmp[81];
#ifdef JP
			prt("�ե����롦�͡���: ", hgt - 1, 0);
			strcpy(tmp, "jhelp.hlp");
#else
			prt("Goto File: ", hgt - 1, 0);
			strcpy(tmp, "help.hlp");
#endif


			if (askfor_aux(tmp, 80))
			{
				if (!show_file(TRUE, tmp, NULL, 0, mode)) k = 'q';
			}
		}

		/* Allow backing up */
		if (k == '-')
		{
			line = line + (reverse ? rows : -rows);
			if (line < 0) line = 0;
		}

		/* Advance a single line */
		if ((k == '\n') || (k == '\r'))
		{
			line = line + (reverse ? -1 : 1);
			if (line < 0) line = 0;
		}


		/* Move up / down */
		if (k == '8')
		{
			line--;
			if (line < 0) line = 0;
		}

		if (k == '2') line++;

		/* Advance one page */
		if (k == ' ')
		{
			line = line + (reverse ? -rows : rows);
			if (line < 0) line = ((size-1)/rows)*rows;
		}

		/* Recurse on numbers */
		if (menu)
		{
			int key = -1;

			if (isalpha(k))
				key = k - 'A';

			if ((key > -1) && hook[key][0])
			{
				/* Recurse on that file */
				if (!show_file(TRUE, hook[key], NULL, 0, mode))
					k = 'q';
			}
		}

		/* Hack, dump to file */
		if (k == '|')
		{
			FILE *ffp;
			char buff[1024];
			char xtmp[82];

			strcpy (xtmp, "");

#ifdef JP
			if (!get_string("�ե�����̾: ", xtmp, 80))
#else
			if (!get_string("File name: ", xtmp, 80))
#endif
			{
				continue;
			}
 
			/* Close it */
			my_fclose(fff);

			/* Drop priv's */
			safe_setuid_drop();

			/* Build the filename */
			path_build(buff, sizeof(buff), ANGBAND_DIR_USER, xtmp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			ffp = my_fopen(buff, "w");

			/* Oops */
			if (!(fff && ffp))
			{
#ifdef JP
				msg_print("�ե����뤬�����ޤ���");
#else
				msg_print("Failed to open file.");
#endif

				k = ESCAPE;
				break;
			}

			sprintf(xtmp, "%s: %s", player_name, what);
			my_fputs(ffp, xtmp, 80);
			my_fputs(ffp, "\n", 80);

			while (!my_fgets(fff, buff, sizeof(buff)))
				my_fputs(ffp, buff, 80);

			/* Close it */
			my_fclose(fff);
			my_fclose(ffp);

			/* Grab priv's */
			safe_setuid_grab();

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");
		}

		/* Exit on escape */
		if (k == ESCAPE) break;
		if (k == 'q') break;
	}

	/* Close the file */
	my_fclose(fff);

	/* Escape */
	if (k == 'q') return (FALSE);

	/* Normal return */
	return (TRUE);
}


/*
 * Peruse the On-Line-Help
 */
void do_cmd_help(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the main help file */
#ifdef JP
	(void)show_file(TRUE, "jhelp.hlp", NULL, 0, 0);
#else
	(void)show_file(TRUE, "help.hlp", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Process the player name.
 * Extract a clean "base name".
 * Build the savefile name if needed.
 */
void process_player_name(bool sf)
{
	int i, k = 0;


	/* Cannot be too long */
#if defined(MACINTOSH) || defined(MSDOS) || defined(USE_EMX) || defined(AMIGA) || defined(ACORN) || defined(VM)
#ifdef MSDOS
	if (strlen(player_name) > 8)
#else
	if (strlen(player_name) > 15)
#endif
	{
		/* Name too long */
#ifdef JP
		quit_fmt("'%s'�Ȥ���̾����Ĺ�����ޤ���", player_name);
#else
		quit_fmt("The name '%s' is too long!", player_name);
#endif

	}
#endif

	/* Cannot contain "icky" characters */
	for (i = 0; player_name[i]; i++)
	{
		/* No control characters */
#ifdef JP
		if (iskanji(player_name[i])){i++;continue;}
		if (iscntrl( (unsigned char)player_name[i]))
#else
		if (iscntrl(player_name[i]))
#endif

		{
			/* Illegal characters */
#ifdef JP
			quit_fmt("'%s' �Ȥ���̾���������ʥ���ȥ��륳���ɤ�ޤ�Ǥ��ޤ���", player_name);
#else
			quit_fmt("The name '%s' contains control chars!", player_name);
#endif

		}
	}


#ifdef MACINTOSH

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif


		/* Convert "dot" to "underscore" */
		if (c == '.') c = '_';

		/* Accept all the letters */
		player_base[k++] = c;
	}

#else

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif

		/* Accept some letters */
#ifdef JP
		if(iskanji(c)){
		  if(k + 2 >= sizeof(player_base) || !player_name[i+1]) break;
		  player_base[k++] = c;
		  i++;
		  player_base[k++] = player_name[i];
		}
#ifdef SJIS
		else if (iskana(c)) player_base[k++] = c;
#endif
		else
#endif
		/* Convert path separator to underscore */
		if (!strncmp(PATH_SEP, player_name+i, strlen(PATH_SEP))){
			player_base[k++] = '_';
			i += strlen(PATH_SEP);
		}
		/* Convert space, dot, and underscore to underscore */
#ifdef MSDOS
		else if (my_strchr(" \"*+,./:;<=>?[\\]|", c)) player_base[k++] = '_';
#elif defined(WINDOWS)
		else if (my_strchr("\"*,/:;<>?\\|", c)) player_base[k++] = '_';
#endif
		else if (isprint(c)) player_base[k++] = c;
	}

#endif


#if defined(MSDOS)

	/* Hack -- max length */
	if (k > 8) k = 8;

#endif

	/* Terminate */
	player_base[k] = '\0';

	/* Require a "base" name */
	if (!player_base[0]) strcpy(player_base, "PLAYER");


#ifdef SAVEFILE_MUTABLE

	/* Accept */
	sf = TRUE;

#endif
	if (!savefile_base[0] && savefile[0])
	{
		cptr s;
		s = savefile;
		while (1)
		{
			cptr t;
			t = my_strstr(s, PATH_SEP);
			if (!t)
				break;
			s = t+1;
		}
		strcpy(savefile_base, s);
	}

	if (!savefile_base[0] || !savefile[0])
		sf = TRUE;

	/* Change the savefile name */
	if (sf)
	{
		char temp[128];

		strcpy(savefile_base, player_base);

#ifdef SAVEFILE_USE_UID
		/* Rename the savefile, using the player_uid and player_base */
		(void)sprintf(temp, "%d.%s", player_uid, player_base);
#else
		/* Rename the savefile, using the player_base */
		(void)sprintf(temp, "%s", player_base);
#endif

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		(void)sprintf(temp, "%s.sv", player_base);
#endif /* VM */

		/* Build the filename */
		path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
	}
}


/*
 * Gets a name for the character, reacting to name changes.
 *
 * Assumes that "display_player(0)" has just been called
 *
 * Perhaps we should NOT ask for a name (at "birth()") on
 * Unix machines?  XXX XXX
 *
 * What a horrible name for a global function.  XXX XXX XXX
 */
void get_name(void)
{
	char tmp[64];

	/* Save the player name */
	strcpy(tmp, player_name);

	/* Prompt for a new name */
#ifdef JP
	if (get_string("����饯������̾�������Ϥ��Ʋ�����: ", tmp, 15))
#else
	if (get_string("Enter a name for your character: ", tmp, 15))
#endif
	{
		/* Use the name */
		strcpy(player_name, tmp);
	}

	if (0 == strlen(player_name))
	{
		/* Use default name */
		strcpy(player_name, "PLAYER");
	}

	/* Re-Draw the name (in light blue) */
	Term_erase(34, 1, 255);
	c_put_str(TERM_L_BLUE, player_name, 1, 34);

	/* Erase the prompt, etc */
	clear_from(22);
}



/*
 * Hack -- commit suicide
 */
void do_cmd_suicide(void)
{
	int i;

	/* Flush input */
	flush();

	/* Verify Retirement */
	if (p_ptr->total_winner)
	{
		/* Verify */
#ifdef JP
		if (!get_check_strict("���ष�ޤ���? ", CHECK_NO_HISTORY)) return;
#else
		if (!get_check_strict("Do you want to retire? ", CHECK_NO_HISTORY)) return;
#endif

	}

	/* Verify Suicide */
	else
	{
		/* Verify */
#ifdef JP
		if (!get_check("�����˼������ޤ�����")) return;
#else
		if (!get_check("Do you really want to commit suicide? ")) return;
#endif
	}


	if (!p_ptr->noscore)
	{
		/* Special Verification for suicide */
#ifdef JP
		prt("��ǧ�Τ��� '@' �򲡤��Ʋ�������", 0, 0);
#else
		prt("Please verify SUICIDE by typing the '@' sign: ", 0, 0);
#endif

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;
	}

	/* Stop playing */
	p_ptr->playing = FALSE;

	/* Kill the player */
	p_ptr->is_dead |= DEATH_DEAD;

	/* Leaving */
	p_ptr->leaving = TRUE;

	if (!p_ptr->total_winner)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "���󥸥���õ������˾���Ƽ���������");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �����४���С� --------");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up all hope to commit suicide.");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");
	}

	/* Cause of death */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "���潪λ");
#else
	(void)strcpy(p_ptr->died_from, "Quitting");
#endif

}



/*
 * Save the game
 */
void do_cmd_save_game(int is_autosave)
{
	/* Autosaves do not disturb */
	if (is_autosave)
	{
#ifdef JP
		msg_print("��ư��������");
#else
		msg_print("Autosaving the game...");
#endif

	}
	else
	{
		/* Disturb the player */
		disturb(1, 0);
	}

	/* Clear messages */
	msg_print(NULL);

	/* Handle stuff */
	handle_stuff();

	/* Message */
#ifdef JP
	prt("������򥻡��֤��Ƥ��ޤ�...", 0, 0);
#else
	prt("Saving game...", 0, 0);
#endif


	/* Refresh */
	Term_fresh();

	/* The player is not dead */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "(������)");
#else
	(void)strcpy(p_ptr->died_from, "(saved)");
#endif


	/* Forbid suspend */
	signals_ignore_tstp();

	/* Save the player */
	if (save_player())
	{
#ifdef JP
		prt("������򥻡��֤��Ƥ��ޤ�... ��λ", 0, 0);
#else
		prt("Saving game... done.", 0, 0);
#endif

	}

	/* Save failed (oops) */
	else
	{
#ifdef MACH_O_CARBON
		char temp[128];

		sprintf(temp, "%ld.save", playtime);
		path_build(savefile, sizeof(savefile), ANGBAND_DIR_SAVE, temp);
		if (save_player()) msg_print("��̾(playtime.save)����¸���ޤ�����");
		else
#endif

#ifdef JP
		prt("������򥻡��֤��Ƥ��ޤ�... ���ԡ�", 0, 0);
#else
		prt("Saving game... failed!", 0, 0);
#endif
	}

	/* Allow suspend again */
	signals_handle_tstp();

	/* Refresh */
	Term_fresh();

	/* Note that the player is not dead */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "(�����������Ƥ���)");
#else
	(void)strcpy(p_ptr->died_from, "(alive and well)");
#endif

	/* Update stuff */
	update_stuff();

	/* Initialize monster process */
	mproc_init();
}


/*
 * Save the game and exit
 */
void do_cmd_save_and_exit(void)
{
	p_ptr->playing = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "----����������----");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "---- Save and Exit Game ----");
#endif
}


/*
 * Hack -- Calculates the total number of points earned		-JWT-
 */
long total_points(void)
{
	int mult = 100;
	u32b point, point_h, point_l;
	u32b point_exp, point_dlv, point_winner, point_ogre;

	if (!preserve_mode) mult += 10;
	if (!smart_learn) mult -= 20;
	if (smart_cheat) mult += 30;
	if (!powerup_home) mult += 50;
	if (ironman_rooms) mult += 100;

	if (mult < 5) mult = 5;

	point_exp = p_ptr->max_max_exp * 100 / p_ptr->expfact;
	if (point_exp > 99999L) point_exp = 99999L;

	point_dlv = p_ptr->max_max_dlv * p_ptr->max_dlv_mult;

	point_winner = 1000L * p_ptr->winner_mult;
	point_ogre = 1000L * p_ptr->ogre_mult;

	point_l = point_exp + point_dlv + point_winner;
	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;

	point = (point_h << 16) + (point_l) + point_ogre;

	if (easy_band)
	{
		point = 1;
		if (p_ptr->total_winner) point++;
		if (!r_info[MON_FILARHH].max_num) point++;
	}

	return point;
}


#define GRAVE_LINE_WIDTH 31

/*
 * Centers a string within a GRAVE_LINE_WIDTH character string		-JWT-
 */
static void center_string(char *buf, cptr str)
{
	int i, j;

	/* Total length */
	i = strlen(str);

	/* Necessary border */
	j = GRAVE_LINE_WIDTH / 2 - i / 2;

	/* Mega-Hack */
	(void)sprintf(buf, "%*s%s%*s", j, "", str, GRAVE_LINE_WIDTH - i - j, "");
}


/*
 * Redefinable "print_tombstone" action
 */
bool (*tombstone_aux)(void) = NULL;


/*
 * Display a "tomb-stone"
 */
static void print_tomb(void)
{
	bool done = FALSE;
#ifdef JP
	cptr die_verb = (p_ptr->is_dead & DEATH_STONED) ? "�в����줿" : "�����줿";
#else
	cptr die_verb = (p_ptr->is_dead & DEATH_STONED) ? "Stoned" : "Killed";
#endif
	int clev = p_ptr->cexp_info[p_ptr->pclass].clev;

	/* Do we use a special tombstone ? */
	if (tombstone_aux)
	{
		/* Use tombstone hook */
		done = (*tombstone_aux)();
	}

	/* Print the text-tombstone */
	if (!done)
	{
		cptr   p;
		char   tmp[160];
		char   buf[1024];
		char   dummy[80];
		char   *t;
		FILE   *fp;
		time_t ct = time((time_t)0);
#ifdef JP
		int    extra_line = 0;
#endif

		/* Clear screen */
		Term_clear();

		/* Build the filename */
#ifdef JP
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "dead_j.txt");
#else
		path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, "dead.txt");
#endif

		/* Open the News file */
		fp = my_fopen(buf, "r");

		/* Dump */
		if (fp)
		{
			int i = 0;

			/* Dump the file to the screen */
			while (0 == my_fgets(fp, buf, sizeof(buf)))
			{
				/* Display and advance */
				put_str(buf, i++, 0);
			}

			/* Close */
			my_fclose(fp);
		}

		/* King or Queen */
		if ((p_ptr->total_winner && !astral_mode) || (p_ptr->lev > PY_MAX_LEVEL))
		{
#ifdef JP
			/* �����ڤ��ؤ� */
			p = "����ʤ��";
#else
			p = "Magnificent";
#endif
		}

		/* Normal */
		else
		{
			p = c_text + class_info[p_ptr->pclass].title[(clev - 1) / 5];
		}

		center_string(buf, player_name);
		put_str(buf, 6, 11);

#ifndef JP
		center_string(buf, "the");
		put_str(buf, 7, 11);
#endif
		center_string(buf, p);
		put_str(buf, 8, 11);


		center_string(buf, c_name + cp_ptr->name);

		put_str(buf, 10, 11);

#ifdef JP
		(void)sprintf(tmp, "��٥�: %d", (int)p_ptr->lev);
#else
		(void)sprintf(tmp, "Level: %d", (int)p_ptr->lev);
#endif

		center_string(buf, tmp);
		put_str(buf, 11, 11);

#ifdef JP
		(void)sprintf(tmp, "�и���: %ld", (long)p_ptr->exp);
#else
		(void)sprintf(tmp, "Exp: %ld", (long)p_ptr->exp);
#endif
		center_string(buf, tmp);
		put_str(buf, 12, 11);

#ifdef JP
		(void)sprintf(tmp, "�����: %ld", (long)p_ptr->au_sum);
#else
		(void)sprintf(tmp, "AU: %ld", (long)p_ptr->au_sum);
#endif
		center_string(buf, tmp);
		put_str(buf, 13, 11);

#ifdef JP
		/* ��˹����դ򥪥ꥸ�ʥ���٤���ɽ�� */
		if (streq(p_ptr->died_from, "���潪λ"))
		{
			strcpy(tmp, "<����>");
		}
		else if (streq(p_ptr->died_from, "ripe"))
		{
			strcpy(tmp, "������ŷ��������");
		}
		else if (streq(p_ptr->died_from, "walstanian") ||
		         streq(p_ptr->died_from, "gargastan") ||
		         streq(p_ptr->died_from, "bacrum") ||
		         streq(p_ptr->died_from, "z_or_l") ||
		         streq(p_ptr->died_from, "valeria"))
		{
			strcpy(tmp, "�ɵҤζ��Ƥ��ݤ��");
		}
		else if (streq(p_ptr->died_from, "lord"))
		{
			strcpy(tmp, "̾���礳����̲��");
		}
		else if (streq(p_ptr->died_from, "ogre"))
		{
			strcpy(tmp, "���Υ������������");
		}
		else if (streq(p_ptr->died_from, "survive"))
		{
			strcpy(tmp, "���Ը��ŷ��������");
		}
		else
		{
			roff_to_buf(p_ptr->died_from, GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
			t = tmp + strlen(tmp) + 1;
			if (*t)
			{
				strcpy(dummy, t); /* 2nd line */
				if (*(t + strlen(t) + 1)) /* Does 3rd line exist? */
				{
					for (t = dummy + strlen(dummy) - 2; iskanji(*(t - 1)); t--) /* Loop */;
					strcpy(t, "��");
				}
				else if (my_strstr(tmp, "��") && suffix(dummy, "��"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "��");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if (strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				else if (my_strstr(tmp, "��") && suffix(dummy, "��"))
				{
					char dummy2[80];
					char *name_head = my_strstr(tmp, "��");
					sprintf(dummy2, "%s%s", name_head, dummy);
					if (strlen(dummy2) <= GRAVE_LINE_WIDTH)
					{
						strcpy(dummy, dummy2);
						*name_head = '\0';
					}
				}
				center_string(buf, dummy);
				put_str(buf, 15, 11);
				extra_line = 1;
			}
		}
		center_string(buf, tmp);
		put_str(buf, 14, 11);

		if (!streq(p_ptr->died_from, "ripe") &&
		    !streq(p_ptr->died_from, "walstanian") &&
		    !streq(p_ptr->died_from, "gargastan") &&
		    !streq(p_ptr->died_from, "bacrum") &&
		    !streq(p_ptr->died_from, "z_or_l") &&
		    !streq(p_ptr->died_from, "valeria") &&
		    !streq(p_ptr->died_from, "lord") &&
		    !streq(p_ptr->died_from, "ogre") &&
		    !streq(p_ptr->died_from, "survive"))
		{
			if( dun_level == 0 )
			{
				cptr town = p_ptr->town_num ? "��" : "����";
				if(streq(p_ptr->died_from, "���潪λ"))
				{
					sprintf(tmp, "%s�ǻ���", town);
				}
				else
				{
					sprintf(tmp, "��%s��%s", town, die_verb);
				}
			}
			else
			{
				if(streq(p_ptr->died_from, "���潪λ"))
				{
					sprintf(tmp, "%d ���ǻ���", dun_level);
				}
				else
				{
					sprintf(tmp, "�� %d ����%s", dun_level, die_verb);
				}
			}
			center_string(buf, tmp);
			put_str(buf, 15 + extra_line, 11);
		}
#else
		(void)sprintf(tmp, "%s on Level %d", die_verb, dun_level);
		center_string(buf, tmp);
		put_str(buf, 14, 11);

		roff_to_buf(format("by %s.", p_ptr->died_from), GRAVE_LINE_WIDTH + 1, tmp, sizeof tmp);
		center_string(buf, tmp);
		put_str(buf, 15, 11);
		t = tmp + strlen(tmp) + 1;
		if (*t)
		{
			strcpy(dummy, t); /* 2nd line */
			if (*(t + strlen(t) + 1)) /* Does 3rd line exist? */
			{
				int dummy_len = strlen(dummy);
				strcpy(dummy + MIN(dummy_len, GRAVE_LINE_WIDTH - 3), "...");
			}
			center_string(buf, dummy);
			put_str(buf, 16, 11);
		}
#endif

		(void)sprintf(tmp, "%-.24s", ctime(&ct));
		center_string(buf, tmp);
		put_str(buf, 17, 11);

#ifdef JP
		msg_format("���褦�ʤ顢%s!", player_name);
#else
		msg_format("Goodbye, %s!", player_name);
#endif
	}
}


/*
 * Display some character info
 */
static void show_info(void)
{
	int             i, j, k, l;
	object_type		*o_ptr;
	store_type		*st_ptr;

	/* Hack -- Know everything in the inven/equip */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);
	}

	for (i = 1; i < max_towns; i++)
	{
		st_ptr = &town[i].store[STORE_HOME];

		/* Hack -- Know everything in the home */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			o_ptr = &st_ptr->stock[j];

			/* Skip non-objects */
			if (!o_ptr->k_idx) continue;

			/* Aware and Known */
			object_aware(o_ptr);
			object_known(o_ptr);
		}
	}

	/* Hack -- Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Flush all input keys */
	flush();

	/* Flush messages */
	msg_print(NULL);


	/* Describe options */
#ifdef JP
	prt("����饯�����ε�Ͽ��ե�����˽񤭽Ф����Ȥ��Ǥ��ޤ���", 21, 0);
	prt("�꥿���󥭡��ǥ���饯�����򸫤ޤ���ESC�����Ǥ��ޤ���", 22, 0);
#else
	prt("You may now dump a character record to one or more files.", 21, 0);
	prt("Then, hit RETURN to see the character, or ESC to abort.", 22, 0);
#endif


	/* Dump character records as requested */
	while (TRUE)
	{
		char out_val[160];

		/* Prompt */
#ifdef JP
		put_str("�ե�����͡���: ", 23, 0);
#else
		put_str("Filename: ", 23, 0);
#endif


		/* Default */
		strcpy(out_val, "");

		/* Ask for filename (or abort) */
		if (!askfor_aux(out_val, 60)) return;

		/* Return means "show on screen" */
		if (!out_val[0]) break;

		/* Save screen */
		screen_save();

		/* Dump a character file */
		(void)file_character(out_val);

		/* Load screen */
		screen_load();
	}

	update_playtime();

	/* Display player */
	display_player(0);

	/* Prompt for inventory */
#ifdef JP
	prt("���������򲡤��Ȥ���˾���³���ޤ� (ESC������): ", 23, 0);
#else
	prt("Hit any key to see more information (ESC to abort): ", 23, 0);
#endif


	/* Allow abort at this point */
	if (inkey() == ESCAPE) return;


	/* Show equipment and inventory */

	/* Equipment -- if any */
	if (equip_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_equip(0);
#ifdef JP
		prt("�������Ƥ��������ƥ�: -³��-", 0, 0);
#else
		prt("You are using: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Inventory -- if any */
	if (inven_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_inven(0);
#ifdef JP
		prt("���äƤ��������ƥ�: -³��-", 0, 0);
#else
		prt("You are carrying: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Homes in the different towns */
	for (l = 1; l < max_towns; l++)
	{
		st_ptr = &town[l].store[STORE_HOME];

		/* Home -- if anything there */
		if (st_ptr->stock_num)
		{
			/* Display contents of the home */
			for (k = 0, i = 0; i < st_ptr->stock_num; k++)
			{
				/* Clear screen */
				Term_clear();

				/* Show 12 items */
				for (j = 0; (j < 12) && (i < st_ptr->stock_num); j++, i++)
				{
					char o_name[MAX_NLEN];
					char tmp_val[80];

					/* Acquire item */
					o_ptr = &st_ptr->stock[i];

					/* Print header, clear line */
					sprintf(tmp_val, "%c) ", I2A(j));
					prt(tmp_val, j+2, 4);

					/* Display object description */
					object_desc(o_name, o_ptr, 0);
					c_put_str(tval_to_attr[o_ptr->tval], o_name, j+2, 7);
				}

				/* Caption */
#ifdef JP
				prt(format("�椬�Ȥ��֤��Ƥ��ä������ƥ� ( %d �ڡ���): -³��-", k+1), 0, 0);
#else
				prt(format("Your home contains (page %d): -more-", k+1), 0, 0);
#endif


				/* Wait for it */
				if (inkey() == ESCAPE) return;
			}
		}
	}
}


static bool check_score(void)
{
	/* Clear screen */
	Term_clear();

	/* No score file */
	if (highscore_fd < 0)
	{
#ifdef JP
		msg_print("���������ե����뤬���ѤǤ��ޤ���");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return FALSE;
	}

#ifndef SCORE_WIZARDS
	/* Wizard-mode pre-empts scoring */
	if (p_ptr->noscore & 0x000F)
	{
#ifdef JP
		msg_print("���������ɡ��⡼�ɤǤϥ���������Ͽ����ޤ���");
#else
		msg_print("Score not registered for wizards.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_BORGS
	/* Borg-mode pre-empts scoring */
	if (p_ptr->noscore & 0x00F0)
	{
#ifdef JP
		msg_print("�ܡ������⡼�ɤǤϥ���������Ͽ����ޤ���");
#else
		msg_print("Score not registered for borgs.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_CHEATERS
	/* Cheaters are not scored */
	if (p_ptr->noscore & 0xFF00)
	{
#ifdef JP
		msg_print("�������ä��ͤϥ���������Ͽ����ޤ���");
#else
		msg_print("Score not registered for cheaters.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

	/* Interupted */
#ifdef JP
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "������λ"))
#else
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "Interrupting"))
#endif

	{
#ifdef JP
		msg_print("������λ�Τ��᥹��������Ͽ����ޤ���");
#else
		msg_print("Score not registered due to interruption.");
#endif

		msg_print(NULL);
		return FALSE;
	}

	/* Quitter */
#ifdef JP
if (!p_ptr->total_winner && streq(p_ptr->died_from, "���潪λ"))
#else
	if (!p_ptr->total_winner && streq(p_ptr->died_from, "Quitting"))
#endif

	{
#ifdef JP
		msg_print("���潪λ�Τ��᥹��������Ͽ����ޤ���");
#else
		msg_print("Score not registered due to quitting.");
#endif

		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

/*
 * Close up the current game (player may or may not be dead)
 *
 * This function is called only from "main.c" and "signals.c".
 */
void close_game(void)
{
	char buf[1024];

/*	cptr p = "[i:����饯���ξ���, f:�ե�����񤭽Ф�, t:������, x:*����*, ESC:�����ཪλ]"; */

	/* Handle stuff */
	handle_stuff();

	/* Flush the messages */
	msg_print(NULL);

	/* Flush the input */
	flush();


	/* No suspending now */
	signals_ignore_tstp();


	/* Hack -- Character is now "icky" */
	character_icky = TRUE;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);


	/* Handle death */
	if (p_ptr->is_dead)
	{
		/* Handle retirement */
		if (p_ptr->total_winner)
		{
			if (astral_mode) survived_finish();
			else kingly();
		}

		/* Save memories */
#ifdef JP
		if (!cheat_save || get_check("�����ǡ����򥻡��֤��ޤ����� "))
#else
		if (!cheat_save || get_check("Save death? "))
#endif
		{

#ifdef JP
			if (!save_player()) msg_print("�����ּ��ԡ�");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}

		/* You are dead */
		if (!(p_ptr->is_dead & DEATH_SNAP_DRAGON)) print_tomb();
		else Term_clear();

		flush();

		/* Show more info */
		show_info();

		/* Clear screen */
		Term_clear();

		if (check_score())
		{
			(void)top_twenty();
		}
		else if (highscore_fd >= 0)
		{
			display_scores_aux(0, 10, -1, NULL);
		}
	}

	/* Still alive */
	else
	{
		/* Save the game */
		do_cmd_save_game(FALSE);

		/* Prompt for scores XXX XXX XXX */
#ifdef JP
		prt("�꥿���󥭡��� ESC �����򲡤��Ʋ�������", 0, 40);
#else
		prt("Press Return (or Escape).", 0, 40);
#endif


		/* Predict score (or ESCAPE) */
		if (inkey() != ESCAPE) predict_score();
	}


	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;

	/* Kill all temporal files */
	clear_saved_floor_files();

	/* Allow suspending now */
	signals_handle_tstp();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 * XXX XXX Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(void)
{
	/* If nothing important has happened, just quit */
#ifdef JP
	if (!character_generated || character_saved) quit("�۵޻���");
#else
	if (!character_generated || character_saved) quit("panic");
#endif


	/* Mega-Hack -- see "msg_print()" */
	msg_flag = FALSE;

	/* Clear the top line */
	prt("", 0, 0);

	/* Hack -- turn off some things */
	disturb(1, 0);

	/* Mega-Hack -- Delay death */
	if ((p_ptr->chp < 0) || p_ptr->is_dead) p_ptr->is_dead &= ~(DEATH_DEAD);
	if (p_ptr->stoning >= 250) p_ptr->is_dead &= ~(DEATH_STONED);

	/* Hardcode panic save */
	p_ptr->panic_save = 1;

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Indicate panic save */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "(�۵ޥ�����)");
#else
	(void)strcpy(p_ptr->died_from, "(panic save)");
#endif


	/* Panic save, or get worried */
#ifdef JP
	if (!save_player()) quit("�۵ޥ����ּ��ԡ�");
#else
	if (!save_player()) quit("panic save failed!");
#endif


	/* Successful panic save */
#ifdef JP
	quit("�۵ޥ�����������");
#else
	quit("panic save succeeded!");
#endif

}


/*
 * Get a random line from a file
 * Based on the monster speech patch by Matt Graham,
 */
errr get_rnd_line(cptr file_name, int entry, char *output)
{
	FILE    *fp;
	char    buf[1024];
	int     line, counter, test, numentries = 0;
	int     line_num = 0;


	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_FILE, file_name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Failed */
	if (!fp) return (-1);

	/* Find the entry of the monster */
	while (TRUE)
	{
		/* Get a line from the file */
		if (my_fgets(fp, buf, sizeof(buf)) == 0)
		{
			/* Count the lines */
			line_num++;

			/* Look for lines starting with 'N:' */
			if ((buf[0] == 'N') && (buf[1] == ':'))
			{
				/* Allow default lines */
				if (buf[2] == '*')
				{
					/* Default lines */
					break;
				}
				else if (buf[2] == 'M')
				{
					if (r_info[entry].flags1 & RF1_MALE)
					{
						break;
					}
				}
				else if (buf[2] == 'F')
				{
					if (r_info[entry].flags1 & RF1_FEMALE)
					{
						break;
					}
				}
				/* Get the monster number */
				else if (sscanf(&(buf[2]), "%d", &test) != EOF)
				{
					/* Is it the right monster? */
					if (test == entry)
					{
						break;
					}
				}
				else
				{
					/* Error while converting the monster number */
					msg_format("Error in line %d of %s!",
					          line_num, file_name);
					my_fclose(fp);
					return (-1);
				}
			}
		}
		else
		{
			/* Reached end of file */
			my_fclose(fp);
			return (-1);
		}
	}

	/* Get the number of entries */
	while (TRUE)
	{
		/* Get the line */
		if (my_fgets(fp, buf, sizeof(buf)) == 0)
		{
			/* Count the lines */
			line_num++;

			/* Look for the number of entries */
			if (isdigit(buf[0]))
			{
				int i;
				bool digit = TRUE;

				for (i = 1; buf[i] && (buf[i] != '\n') && (buf[i] != '\r'); i++)
				{
					if (!isdigit(buf[i]))
					{
						digit = FALSE;
						break;
					}
				}

				/* Get the number of entries */
				if (digit) numentries = atoi(buf);
				break;
			}
		}
		else
		{
			/* Count the lines */
			line_num++;

			/* Reached end of file without finding the number */
			msg_format("Error in line %d of %s!",
			          line_num, file_name);

			my_fclose(fp);
			return (-1);
		}
	}

	if (numentries > 0)
	{
		/* Grab an appropriate line number */
		line = randint0(numentries);

		/* Get the random line */
		for (counter = 0; counter <= line; counter++)
		{
			/* Count the lines */
			line_num++;

			while(TRUE)
			{
				test = my_fgets(fp, buf, sizeof(buf));
				if(test || buf[0] != '#')
					break;
			}

			if (test==0)
			{
				/* Found the line */
				if (counter == line) break;
			}
			else
			{
				/* Error - End of file */
				msg_format("Error in line %d of %s!",
				          line_num, file_name);

				my_fclose(fp);
				return (-1);
			}
		}

		/* Copy the line */
		strcpy(output, buf);
	}
	else
	{
		return (-1);
	}

	/* Close the file */
	my_fclose(fp);

	/* Success */
	return (0);
}


#ifdef JP
errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count)
{
	int i, j, kanji;
	errr result = 1;
	for (i = 0; i < count; i++)
	{
		result = get_rnd_line(file_name, entry, output);
		if (result) break;
		kanji = 0;
		for (j = 0; output[j]; j++) kanji |= iskanji(output[j]);
		if (kanji) break;
	}
	return result;
}
#endif

/*
 * Process file for auto picker/destroyer.
 */
errr process_pickpref_file(cptr name)
{
	char buf[1024];

	errr err = 0;

	/* Drop priv's */
	safe_setuid_drop();

	/* Build the filename */
	path_build(buf, sizeof(buf), ANGBAND_DIR_USER, name);

	err = process_pref_file_aux(buf, TRUE);

	/* Grab priv's */
	safe_setuid_grab();

	/* Result */
	return (err);
}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

	/* Flush output */
	Term_fresh();

	/* Suspend the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 0);

	/* Suspend ourself */
	(void)kill(0, SIGSTOP);

	/* Resume the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 1);

	/* Redraw the term */
	Term_redraw();

	/* Flush the term */
	Term_fresh();

#endif

	/* Restore handler */
	(void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	/* Count the signals */
	signal_count++;


	/* Terminate dead characters */
	if (p_ptr->is_dead & DEATH_DEAD)
	{
		/* Mark the savefile */
#ifdef JP
		(void)strcpy(p_ptr->died_from, "������λ");
#else
		(void)strcpy(p_ptr->died_from, "Abortion");
#endif

		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
		quit("������λ");
#else
		quit("interrupt");
#endif

	}

	/* Allow suicide (after 5) */
	else if (signal_count >= 5)
	{
		/* Cause of "death" */
#ifdef JP
		(void)strcpy(p_ptr->died_from, "������λ��");
#else
		(void)strcpy(p_ptr->died_from, "Interrupting");
#endif


		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Stop playing */
		p_ptr->playing = FALSE;

		/* Suicide */
		p_ptr->is_dead |= DEATH_DEAD;

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
		quit("������λ");
#else
		quit("interrupt");
#endif

	}

	/* Give warning (after 4) */
	else if (signal_count >= 4)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);

		/* Clear the top line */
		Term_erase(0, 0, 255);

		/* Display the cause */
#ifdef JP
		Term_putstr(0, 0, -1, TERM_WHITE, "��θ�ξ�μ�����");
#else
		Term_putstr(0, 0, -1, TERM_WHITE, "Contemplating suicide!");
#endif


		/* Flush */
		Term_fresh();
	}

	/* Give warning (after 2) */
	else if (signal_count >= 2)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);
	}

	/* Restore handler */
	(void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void handle_signal_abort(int sig)
{
	int wid, hgt, rows;

	Term_get_size(&wid, &hgt);
	rows = hgt - 4;

	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	forget_lite();
	forget_view();
	clear_mon_lite();

	/* Clear the bottom line */
	Term_erase(0, hgt - 1, 255);

	/* Give a warning */
	Term_putstr(0, hgt - 1, -1, TERM_RED,
#ifdef JP
	            "���������եȤΥХ������Ӥ����äƤ�����");
#else
	            "A gruesome software bug LEAPS out at you!");
#endif


	/* Message */
#ifdef JP
	Term_putstr(45, hgt - 1, -1, TERM_RED, "�۵ޥ�����...");
#else
	Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save...");
#endif


	/* Flush output */
	Term_fresh();

	/* Panic Save */
	p_ptr->panic_save = 1;

	/* Panic save */
#ifdef JP
	(void)strcpy(p_ptr->died_from, "(�۵ޥ�����)");
#else
	(void)strcpy(p_ptr->died_from, "(panic save)");
#endif


	/* Forbid suspend */
	signals_ignore_tstp();

	/* Attempt to save */
	if (save_player())
	{
#ifdef JP
		Term_putstr(45, hgt - 1, -1, TERM_RED, "�۵ޥ�����������");
#else
		Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save succeeded!");
#endif

	}

	/* Save failed */
	else
	{
#ifdef JP
		Term_putstr(45, hgt - 1, -1, TERM_RED, "�۵ޥ����ּ��ԡ�");
#else
		Term_putstr(45, hgt - 1, -1, TERM_RED, "Panic save failed!");
#endif

	}

	/* Flush output */
	Term_fresh();

	/* Quit */
#ifdef JP
	quit("���եȤΥХ�");
#else
	quit("software bug");
#endif

}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{

#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif


#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif


#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif


#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}


#else	/* HANDLE_SIGNALS */


/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}
#endif	/* HANDLE_SIGNALS */
