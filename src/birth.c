/* File: birth.c */

/* Purpose: create a player character */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Forward declare
 */
typedef struct birther birther;

/*
 * A structure to hold "rolled" information
 */
struct birther
{
	s16b age;
	s16b wt;
	s16b ht;
	s16b sc;

	s32b au;

	s16b stat[7];

	char history[4][60];
};



/*
 * The last character displayed
 */
static birther prev;



/*
 * Forward declare
 */
typedef struct hist_type hist_type;

/*
 * Player background information
 */
struct hist_type
{
	cptr info;                   /* Textual History */

	byte roll;                   /* Frequency of this entry */
	byte chart;                  /* Chart index */
	byte next;                   /* Next chart index */
	byte bonus;                  /* Social Class Bonus + 50 */
};


/*
 * Background information (see below)
 *
 * Chart progression by race:
 *   Human/Dunadan -->  1 -->  2 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Half-Elf      -->  4 -->  1 -->  2 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Elf/High-Elf/Dark-Elf  -->  7 -->  8 -->  9 --> 54 --> 55 --> 56
 *   Hobbit        --> 10 --> 11 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Gnome         --> 13 --> 14 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Dwarf         --> 16 --> 17 --> 18 --> 57 --> 58 --> 59 --> 60 --> 61
 *   Half-Orc      --> 19 --> 20 -->  2 -->  3 --> 50 --> 51 --> 52 --> 53
 *   Half-Troll    --> 22 --> 23 --> 62 --> 63 --> 64 --> 65 --> 66
 *   Giant         --> 24 --> 23 --> 62 --> 63 --> 64 --> 65 --> 66
 *
 * XXX XXX XXX This table *must* be correct or drastic errors may occur!
 */
static hist_type bg[] =
{
	{"You are the illegitimate and unacknowledged child ",  10, 1, 2, 25},
	{"You are the illegitimate but acknowledged child ",    20, 1, 2, 35},
	{"You are one of several children ",               95, 1, 2, 45},
	{"You are the first child ",                 100, 1, 2, 50},

	{"of a Serf.  ",                              40, 2, 3, 65},
	{"of a Yeoman.  ",                            65, 2, 3, 80},
	{"of a Townsman.  ",                          80, 2, 3, 90},
	{"of a Guildsman.  ",                         90, 2, 3, 105},
	{"of a Landed Knight.  ",                          96, 2, 3, 120},
	{"of a Titled Noble.  ",                      99, 2, 3, 130},
	{"of a Royal Blood Line.  ",                 100, 2, 3, 140},

	{"You are the black sheep of the family.  ",       20, 3, 50, 20},
	{"You are a credit to the family.  ",              80, 3, 50, 55},
	{"You are a well liked child.  ",                 100, 3, 50, 60},

	{"Your mother was of the Teleri.  ",               40, 4, 1, 50},
	{"Your father was of the Teleri.  ",               75, 4, 1, 55},
	{"Your mother was of the Noldor.  ",               90, 4, 1, 55},
	{"Your father was of the Noldor.  ",               95, 4, 1, 60},
	{"Your mother was of the Vanyar.  ",               98, 4, 1, 65},
	{"Your father was of the Vanyar.  ",              100, 4, 1, 70},

	{"You are one of several children ",               60, 7, 8, 50},
	{"You are the only child ",                       100, 7, 8, 55},

	{"of a Teleri ",                              75, 8, 9, 50},
	{"of a Noldor ",                              95, 8, 9, 55},
	{"of a Vanyar ",                             100, 8, 9, 60},

	{"Ranger.  ",                            40, 9, 54, 80},
	{"Archer.  ",                            70, 9, 54, 90},
	{"Warrior.  ",                           87, 9, 54, 110},
	{"Mage.  ",                                   95, 9, 54, 125},
	{"Prince.  ",                            99, 9, 54, 140},
	{"King.  ",                                  100, 9, 54, 145},

	{"You are one of several children of a Hobbit ",        85, 10, 11, 45},
	{"You are the only child of a Hobbit ",              100, 10, 11, 55},

	{"Bum.  ",                                    20, 11, 3, 55},
	{"Tavern Owner.  ",                           30, 11, 3, 80},
	{"Miller.  ",                            40, 11, 3, 90},
	{"Home Owner.  ",                             50, 11, 3, 100},
	{"Burglar.  ",                           80, 11, 3, 110},
	{"Warrior.  ",                           95, 11, 3, 115},
	{"Mage.  ",                                   99, 11, 3, 125},
	{"Clan Elder.  ",                            100, 11, 3, 140},

	{"You are one of several children of a Gnome ",         85, 13, 14, 45},
	{"You are the only child of a Gnome ",            100, 13, 14, 55},

	{"Beggar.  ",                            20, 14, 3, 55},
	{"Braggart.  ",                               50, 14, 3, 70},
	{"Prankster.  ",                              75, 14, 3, 85},
	{"Warrior.  ",                           95, 14, 3, 100},
	{"Mage.  ",                                  100, 14, 3, 125},

	{"You are one of two children of a Dwarven ",           25, 16, 17, 40},
	{"You are the only child of a Dwarven ",               100, 16, 17, 50},

	{"Thief.  ",                             10, 17, 18, 60},
	{"Prison Guard.  ",                           25, 17, 18, 75},
	{"Miner.  ",                             75, 17, 18, 90},
	{"Warrior.  ",                           90, 17, 18, 110},
	{"Priest.  ",                            99, 17, 18, 130},
	{"King.  ",                                  100, 17, 18, 150},

	{"You are the black sheep of the family.  ",       15, 18, 57, 10},
	{"You are a credit to the family.  ",              85, 18, 57, 50},
	{"You are a well liked child.  ",                 100, 18, 57, 55},

	{"Your mother was an Orc, but it is unacknowledged.  ",      25, 19, 20,
25},
	{"Your father was an Orc, but it is unacknowledged.  ",     100, 19, 20,
25},

	{"You are the adopted child ",                    100, 20, 2, 50},
	{"Your mother was a Cave-Troll ",                  30, 22, 23, 20},
	{"Your father was a Cave-Troll ",                  60, 22, 23, 25},
	{"Your mother was a Hill-Troll ",                  75, 22, 23, 30},
	{"Your father was a Hill-Troll ",                  90, 22, 23, 35},
	{"Your mother was a Water-Troll ",                 95, 22, 23, 40},
	{"Your father was a Water-Troll ",                100, 22, 23, 45},

	{"Cook.  ",                                    5, 23, 62, 60},
	{"Warrior.  ",                           95, 23, 62, 55},
	{"Shaman.  ",                            99, 23, 62, 65},
	{"Clan Chief.  ",                            100, 23, 62, 80},

	{"Your mother was a hill-giant ", 30, 24, 23, 20},
	{"Your father was a hill-giant ", 50, 24, 23, 30},
	{"Your mother was a stone-giant ", 80, 24, 23, 35},
	{"Your father was a stone-giant ", 96, 24, 23, 40},

	{"You have dark brown eyes, ",                     20, 50, 51, 50},
	{"You have brown eyes, ",                          60, 50, 51, 50},
	{"You have hazel eyes, ",                          70, 50, 51, 50},
	{"You have green eyes, ",                          80, 50, 51, 50},
	{"You have blue eyes, ",                      90, 50, 51, 50},
	{"You have blue-gray eyes, ",                100, 50, 51, 50},

	{"straight ",                            70, 51, 52, 50},
	{"wavy ",                                90, 51, 52, 50},
	{"curly ",                                   100, 51, 52, 50},

	{"black hair, ",                              30, 52, 53, 50},
	{"brown hair, ",                              70, 52, 53, 50},
	{"auburn hair, ",                             80, 52, 53, 50},
	{"red hair, ",                           90, 52, 53, 50},
	{"blond hair, ",                             100, 52, 53, 50},

	{"and a very dark complexion.",                    10, 53, 0, 50},
	{"and a dark complexion.",                         30, 53, 0, 50},
	{"and an average complexion.",                     80, 53, 0, 50},
	{"and a fair complexion.",                         90, 53, 0, 50},
	{"and a very fair complexion.",                   100, 53, 0, 50},

	{"You have light grey eyes, ",                     85, 54, 55, 50},
	{"You have light blue eyes, ",                     95, 54, 55, 50},
	{"You have light green eyes, ",                   100, 54, 55, 50},

	{"straight ",                            75, 55, 56, 50},
	{"wavy ",                               100, 55, 56, 50},

	{"black hair, and a fair complexion.",             75, 56, 0, 50},
	{"brown hair, and a fair complexion.",             85, 56, 0, 50},
	{"blond hair, and a fair complexion.",             95, 56, 0, 50},
	{"silver hair, and a fair complexion.",           100, 56, 0, 50},

	{"You have dark brown eyes, ",                     99, 57, 58, 50},
	{"You have glowing red eyes, ",                   100, 57, 58, 60},
	{"straight ",                            90, 58, 59, 50},
	{"wavy ",                               100, 58, 59, 50},

	{"black hair, ",                              75, 59, 60, 50},
	{"brown hair, ",                             100, 59, 60, 50},

	{"a one foot beard, ",                        25, 60, 61, 50},
	{"a two foot beard, ",                        60, 60, 61, 51},
	{"a three foot beard, ",                      90, 60, 61, 53},
	{"a four foot beard, ",                      100, 60, 61, 55},

	{"and a dark complexion.",                        100, 61, 0, 50},

	{"You have slime green eyes, ",                    60, 62, 63, 50},
	{"You have puke yellow eyes, ",                    85, 62, 63, 50},
	{"You have blue-bloodshot eyes, ",                 99, 62, 63, 50},
	{"You have glowing red eyes, ",                   100, 62, 63, 55},

	{"dirty ",                                    33, 63, 64, 50},
	{"mangy ",                                    66, 63, 64, 50},
	{"oily ",                               100, 63, 64, 50},

	{"sea-weed green hair, ",                          33, 64, 65, 50},
	{"bright red hair, ",                         66, 64, 65, 50},
	{"dark purple hair, ",                       100, 64, 65, 50},

	{"and green ",                           25, 65, 66, 50},
	{"and blue ",                            50, 65, 66, 50},
	{"and white ",                           75, 65, 66, 50},
	{"and black ",                          100, 65, 66, 50},

	{"ulcerous skin.",                            33, 66, 0, 50},
	{"scabby skin.",                              66, 66, 0, 50},
	{"leprous skin.",                            100, 66, 0, 50}
};



/*
 * Current stats
 */
static s16b         stat_use[7];

/*
 * Autoroll limit
 */
static s16b         stat_limit[7];

/*
 * Autoroll matches
 */
static s32b         stat_match[7];

/*
 * Autoroll round
 */
static s32b         auto_round;

/*
 * Last round
 */
static s32b         last_round;


/*
 * Save the current data for later
 */
static void save_prev_data(void)
{
	int i;


	/*** Save the current data ***/

	/* Save the data */
	prev.age = p_ptr->age;
	prev.wt = p_ptr->wt;
	prev.ht = p_ptr->ht;
	prev.sc = p_ptr->sc;
	prev.au = p_ptr->au;

	/* Save the stats */
	for (i = 0; i < 7; i++)
	{
		prev.stat[i] = p_ptr->stat_max[i];
	}

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(prev.history[i], history[i]);
	}
}


/*
 * Load the previous data
 */
static void load_prev_data(void)
{
	int        i;

	birther   temp;
	/*** Save the current data ***/

	/* Save the data */
	temp.age = p_ptr->age;
	temp.wt = p_ptr->wt;
	temp.ht = p_ptr->ht;
	temp.sc = p_ptr->sc;
	temp.au = p_ptr->au;

	/* Save the stats */
	for (i = 0; i < 7; i++)
	{
		temp.stat[i] = p_ptr->stat_max[i];
	}

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(temp.history[i], history[i]);
	}


	/*** Load the previous data ***/

	/* Load the data */
	p_ptr->age = prev.age;
	p_ptr->wt = prev.wt;
	p_ptr->ht = prev.ht;
	p_ptr->sc = prev.sc;
	p_ptr->au = prev.au;

	/* Load the stats */
	for (i = 0; i < 7; i++)
	{
		p_ptr->stat_max[i] = prev.stat[i];
		p_ptr->stat_cur[i] = prev.stat[i];
	}

	/* Load the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(history[i], prev.history[i]);
	}


	/*** Save the current data ***/

	/* Save the data */
	prev.age = temp.age;
	prev.wt = temp.wt;
	prev.ht = temp.ht;
	prev.sc = temp.sc;
	prev.au = temp.au;
	/* Save the stats */
	for (i = 0; i < 7; i++)
	{
		prev.stat[i] = temp.stat[i];
	}

	/* Save the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(prev.history[i], temp.history[i]);
	}
}

/*
 * Choose stat mods
*/
static void choose_mods(void)
{
	char c;
	int i, total;
	char inp[5];

	put_str("Apply modifications to your stats? (Y/N/Q/?)", 22, 2);
	do
	{
		c = inkey();
		if(c=='n' || c=='N')
		{
			clear_from(20);
			return;
		}
		if(c=='Q') quit(NULL);
		if(c=='?') do_cmd_help("help.hlp");
		} while(c!='y' && c!='Y');

	/* Get the mods */
	clear_from(12);
	put_str("Please enter a modifier for each stat.", 12, 2);
	put_str("Note: Each modification must be between -2 and +4, and the", 13, 2);
	put_str("total of the modifiers may be no larger than 5.", 14, 2);
	do
	{
		total = 0;
		for(i=0; i<6; i++)
		{
			put_str(stat_names[i], i+15, 5);
			do
			{
				strcpy(inp, "");

				/* Move the cursor */
				put_str("", 15 + i, 10);
				if (!askfor_aux(inp, 5)) inp[0]='\0';
				c = atoi(inp);
			} while(c<-2 || c>4);
			total += c;
			p_ptr->stat_mod[i] = c;
		}
	} while(total>5);
}
/*
 * Returns adjusted stat -JK-  Algorithm by -JWT-
 *
 * auto_roll is boolean and states maximum changes should be used rather
 * than random ones to allow specification of higher values to wait for
 *
 * The "p_ptr->maximize" code is important   -BEN-
 */
static int adjust_stat(int value, s16b amount, int auto_roll)
{
	int i;

	/* Negative amounts */
	if (amount < 0)
	{
		/* Apply penalty */
		for (i = 0; i < (0 - amount); i++)
		{
			if (value >= 18+10)
			{
				value -= 10;
			}
			else if (value > 18)
			{
				value = 18;
			}
			else if (value > 3)
			{
				value--;
			}
		}
	}

	/* Positive amounts */
	else if (amount > 0)
	{
		/* Apply reward */
		for (i = 0; i < amount; i++)
		{
			if (value < 18)
			{
				value++;
			}
			else if (p_ptr->maximize)
			{
				value += 10;
			}
			else if (value < 18+70)
			{
				value += ((auto_roll ? 15 : randint(15)) + 5);
			}
			else if (value < 18+90)
			{
				value += ((auto_roll ? 6 : randint(6)) + 2);
			}
			else if (value < 18+100)
			{
				value++;
			}
		}
	}

	/* Return the result */
	return (value);
}




/*
 * Roll for a characters stats
 *
 * For efficiency, we include a chunk of "calc_bonuses()".
 */
static void get_stats(void)
{
	int       i, j;

	int       bonus;

	int       dice[21];


	/* Roll and verify some stats */
	while (TRUE)
	{
		/* Roll some dice */
		for (j = i = 0; i < 21; i++)
		{
			/* Roll the dice */
			dice[i] = randint(3 + i % 3);

			/* Collect the maximum */
			j += dice[i];
		}

		/* Verify totals */
		if ((j > 42) && (j < 72)) break;
	}
	/* Acquire the stats */
	for (i = 0; i < 7; i++)
	{
		/* Extract 5 + 1d3 + 1d4 + 1d5 */
		j = 5 + dice[3*i] + dice[3*i+1] + dice[3*i+2];
		/* Save that value */
		p_ptr->stat_max[i] = j;
		/* Obtain a "bonus" for "race" and "mods" */
		bonus = rp_ptr->r_adj[i] + p_ptr->stat_mod[i];

		/* Variable stat maxes */
		if (p_ptr->maximize)
		{
			/* Start fully healed */
			p_ptr->stat_cur[i] = p_ptr->stat_max[i];

			/* Efficiency -- Apply the racial/mod bonuses */
			stat_use[i] = modify_stat_value(p_ptr->stat_max[i], bonus);
		}

		/* Fixed stat maxes */
		else
		{
			/* Apply the bonus to the stat (somewhat randomly) */
			stat_use[i] = adjust_stat(p_ptr->stat_max[i], bonus, FALSE);

			/* Save the resulting stat maximum */
			p_ptr->stat_cur[i] = p_ptr->stat_max[i] = stat_use[i];
		}
	}
}


/*
 * Roll for some info that the auto-roller ignores
 */
static void get_extra(void)
{
	int       i, j, min_value, max_value;


	/* Level one */
	p_ptr->max_plv = p_ptr->lev = 1;

	/* Hitdice */
	p_ptr->hitdie = rp_ptr->r_mhp;

	/* Initial hitpoints */
	p_ptr->chp = p_ptr->mhp = p_ptr->hitdie;

	/* Minimum hitpoints at highest level */
	min_value = (PY_MAX_LEVEL * (p_ptr->hitdie - 1) * 3) / 8;
	min_value += PY_MAX_LEVEL;

	/* Maximum hitpoints at highest level */
	max_value = (PY_MAX_LEVEL * (p_ptr->hitdie - 1) * 5) / 8;
	max_value += PY_MAX_LEVEL;
	/* Pre-calculate level 1 hitdice */
	player_hp[0] = p_ptr->hitdie;

	/* Roll out the hitpoints */
	while (TRUE)
	{
		/* Roll the hitpoint values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			j = randint(p_ptr->hitdie);
			player_hp[i] = player_hp[i-1] + j;
		}

		/* XXX Could also require acceptable "mid-level" hitpoints */

		/* Require "valid" hitpoints at highest level */
		if (player_hp[PY_MAX_LEVEL-1] < min_value) continue;
		if (player_hp[PY_MAX_LEVEL-1] > max_value) continue;

		/* Acceptable */
		break;
	}
}


/*
 * Get the racial history, and social class, using the "history charts".
 */
static void get_history(void)
{
	int       i, n, chart, roll, social_class;

	char *s, *t;

	char buf[240];



	/* Clear the previous history strings */
	for (i = 0; i < 4; i++) history[i][0] = '\0';


	/* Clear the history text */
	buf[0] = '\0';

	/* Initial social class */
	social_class = randint(4);

	/* Starting place */
	switch (p_ptr->prace)
	{
		case RACE_HUMAN:
		case RACE_DUNADAN:
		{
			chart = 1;
			break;
		}
		case RACE_HALF_ELF:
		{
			chart = 4;
			break;
		}
		case RACE_ELF:
		case RACE_HIGH_ELF:
		case RACE_DARK_ELF:
		{
			chart = 7;
			break;
		}

		case RACE_HOBBIT:
		{
			chart = 10;
			break;
		}

		case RACE_GNOME:
		{
			chart = 13;
			break;
		}

		case RACE_DWARF:
		{
			chart = 16;
			break;
		}

		case RACE_HALF_ORC:
		{
			chart = 19;
			break;
		}

		case RACE_HALF_TROLL:
		{
			chart = 22;
			break;
		}

		case RACE_GIANT:
		{
			chart = 23;
			break;
		}

		default:
		{
			chart = 0;
			break;
		}
	}

	/* Process the history */
	while (chart)
	{
		/* Start over */
		i = 0;
		/* Roll for nobility */
		roll = randint(100);

		/* Access the proper entry in the table */
		while ((chart != bg[i].chart) || (roll > bg[i].roll)) i++;

		/* Acquire the textual history */
		(void)strcat(buf, bg[i].info);

		/* Add in the social class */
		social_class += (int)(bg[i].bonus) - 50;

		/* Enter the next chart */
		chart = bg[i].next;
	}



	/* Verify social class */
	if (social_class > 100) social_class = 100;
	else if (social_class < 1) social_class = 1;

	/* Save the social class */
	p_ptr->sc = social_class;


	/* Skip leading spaces */
	for (s = buf; *s == ' '; s++) /* loop */;

	/* Get apparent length */
	n = strlen(s);

	/* Kill trailing spaces */
	while ((n > 0) && (s[n-1] == ' ')) s[--n] = '\0';


	/* Start at first line */
	i = 0;
	/* Collect the history */
	while (TRUE)
	{
		/* Extract remaining length */
		n = strlen(s);
		/* All done */
		if (n < 60)
		{
			/* Save one line of history */
			strcpy(history[i++], s);

			/* All done */
			break;
		}

		/* Find a reasonable break-point */
		for (n = 60; ((n > 0) && (s[n-1] != ' ')); n--) /* loop */;

		/* Save next location */
		t = s + n;

		/* Wipe trailing spaces */
		while ((n > 0) && (s[n-1] == ' ')) s[--n] = '\0';

		/* Save one line of history */
		strcpy(history[i++], s);

		/* Start next line */
		for (s = t; *s == ' '; s++) /* loop */;
	}
}


/*
 * Computes character's age, height, and weight
 */
static void get_ahw(void)
{
	/* Calculate the age */
	p_ptr->age = rp_ptr->b_age + randint(rp_ptr->m_age);

	/* Calculate the height/weight for males */
	if (p_ptr->psex)
	{
		p_ptr->ht = randnor(rp_ptr->m_b_ht, rp_ptr->m_m_ht);
		p_ptr->wt = randnor(rp_ptr->m_b_wt, rp_ptr->m_m_wt);
	}

	/* Calculate the height/weight for females */
	else
	{
		p_ptr->ht = randnor(rp_ptr->f_b_ht, rp_ptr->f_m_ht);
		p_ptr->wt = randnor(rp_ptr->f_b_wt, rp_ptr->f_m_wt);
	}
}




/*
 * Get the player's starting money
 */
static void get_money(void)
{
	int        i, gold;
	/* Social Class determines starting gold */
	gold = (p_ptr->sc * 6) + randint(100) + 300;

	/* Process the stats */
	for (i = 0; i < 6; i++)
	{
		/* Mega-Hack -- reduce gold for high stats */
		if (stat_use[i] >= 18+50) gold -= 300;
		else if (stat_use[i] >= 18+20) gold -= 200;
		else if (stat_use[i] > 18) gold -= 150;
		else gold -= (stat_use[i] - 8) * 10;
	}

	/* Minimum 100 gold */
	if (gold < 100) gold = 100;

	/* She charmed the banker into it! -CJS- */
	/* She slept with the banker.. :) -GDH-  */
	if (!p_ptr->psex) gold += 50;

	/* Save the gold */
	p_ptr->au = gold*4;
}

/* Roll the skills */
static void get_skills(void)
{
	byte j, k;
	for(j=0; j<S_NUM; j++)
	{
		k = rp_ptr->start[j];
		p_ptr->cur_skill[j] = randint(k)+randint(k)+randint(k);
		p_ptr->max_skill[j] = p_ptr->cur_skill[j];
		p_ptr->min_skill[j] = p_ptr->cur_skill[j];
	}
}

/* Get exp */
static void get_exp(void)
{
	s16b i, t=0;

	/* Get lifexp */
	p_ptr->exp = rp_ptr->lifexp * rp_ptr->lifexp;

	/* Decrease for stat mods */
	for(i=0; i<6; i++) t += p_ptr->stat_mod[i];
	p_ptr->exp += (5-t) * 60;
	p_ptr->max_exp = p_ptr->exp;
}

/*
 * Display stat values, subset of "put_stats()"
 *
 * See 'display_player()' for basic method.
 */
static void birth_put_stats(void)
{
	int       i, p;
	byte attr;

	char buf[80];
	/* Put the stats (and percents) */
	for (i = 0; i < 6; i++)
	{
		/* Put the stat */
		cnv_stat(stat_use[i], buf);
		c_put_str(TERM_L_GREEN, buf, 2 + i, 66);

		/* Put the percent */
		if (stat_match[i])
		{
			p = 1000L * stat_match[i] / auto_round;
			attr = (p < 100) ? TERM_YELLOW : TERM_L_GREEN;
			sprintf(buf, "%3d.%d%%", p/10, p%10);
			c_put_str(attr, buf, 2 + i, 73);
		}

		/* Never happened */
		else
		{
			c_put_str(TERM_RED, "(NONE)", 2 + i, 73);
		}
	}
}


/*
 * Clear all the global "character" data
 */
static void player_wipe(void)
{
	int i;


	/* Hack -- zero the struct */
	WIPE(p_ptr, player_type);

	/* Wipe the history */
	for (i = 0; i < 4; i++)
	{
		strcpy(history[i], "");
	}


	/* No weight */
	total_weight = 0;

	/* No items */
	inven_cnt = 0;
	equip_cnt = 0;

	/* Clear the inventory */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_wipe(&inventory[i]);
	}


	/* Start with no artifacts made yet */
	for (i = 0; i < MAX_A_IDX; i++)
	{
		artifact_type *a_ptr = &a_info[i];
		a_ptr->cur_num = 0;
	}


	/* Start with no quests */
	for (i = 0; i < MAX_Q_IDX; i++)
	{
		q_list[i].level = 0;
	}

	/* Add a special quest */
	q_list[0].level = 99;
	/* Add a second quest */
	q_list[1].level = 100;


	/* Reset the "objects" */
	for (i = 1; i < MAX_K_IDX; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Reset "tried" */
		k_ptr->tried = FALSE;

		/* Reset "aware" */
		k_ptr->aware = FALSE;
	}


	/* Reset the "monsters" */
	for (i = 1; i < MAX_R_IDX; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Hack -- Reset the counter */
		r_ptr->cur_num = 0;

		/* Hack -- Reset the max counter */
		r_ptr->max_num = 100;
		/* Hack -- Reset the max counter */
		if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 1;

		/* Clear player kills */
		r_ptr->r_pkills = 0;
	}

	/* Hack -- no ghosts */
	r_info[MAX_R_IDX-1].max_num = 0;


	/* Hack -- Well fed player */
	p_ptr->food = PY_FOOD_FULL - 1;

	/* Wipe the spells */
	spell_learned1 = spell_learned2 = 0L;
	spell_worked1 = spell_worked2 = 0L;
	spell_forgotten1 = spell_forgotten2 = 0L;
	for (i = 0; i < 64; i++) spell_order[i] = 99;


	/* Clear "cheat" options */
	cheat_peek = FALSE;
	cheat_hear = FALSE;
	cheat_room = FALSE;
	cheat_xtra = FALSE;
	cheat_know = FALSE;
	cheat_live = FALSE;

	/* Assume no winning game */
	total_winner = FALSE;

	/* Assume no panic save */
	panic_save = 0;

	/* Assume no cheating */
	noscore = 0;
}




/*
 * Helper function for 'player_birth()'
 *
 * The delay may be reduced, but is recommended to keep players
 * from continuously rolling up characters, which can be VERY
 * expensive CPU wise.
 */
static bool player_birth_aux(void)
{
	int            i, j, k, m, n, v;

	int mode = 0;
  
	bool flag = FALSE;
	bool prev = FALSE;
  
	cptr str;
  
	char c;
  
	char p2 = ')';
	char b1 = '[';
	char b2 = ']';
  
	char buf[80];

	bool autoroll = FALSE;
  

	/*** Intro ***/
  
	/* Clear screen */
	Term_clear();
	/* Title everything */
	put_str("Name        :", 2, 1);
	put_str("Sex         :", 3, 1);
	put_str("Race        :", 4, 1);

	/* Dump the default name */
	c_put_str(TERM_L_BLUE, player_name, 2, 15);


	/*** Instructions ***/

	/* Display some helpful information */
	Term_putstr(5, 10, -1, TERM_WHITE,
		"Please answer the following questions.  Most of the questions");
	Term_putstr(5, 11, -1, TERM_WHITE,
		"display a set of standard answers, and many will also accept");
	Term_putstr(5, 12, -1, TERM_WHITE,
		"some special responses, including 'Q' to quit, 'S' to restart,");
	Term_putstr(5, 13, -1, TERM_WHITE,
		"and '?' for help.  Note that 'Q' and 'S' must be capitalized.");


	/*** Player sex ***/

	/* Extra info */
	Term_putstr(5, 15, -1, TERM_WHITE,
		"Your 'sex' does not have any significant gameplay effects.");

	/* Prompt for "Sex" */
	for (n = 0; n < MAX_SEXES; n++)
	{
		/* Analyze */
		p_ptr->psex = n;
		sp_ptr = &sex_info[p_ptr->psex];
		str = sp_ptr->title;
		/* Display */
		sprintf(buf, "%c%c %s", I2A(n), p2, str);
		put_str(buf, 21 + (n/5), 2 + 15 * (n%5));
	}

	/* Choose */
	while (1)
	{
		sprintf(buf, "Choose a sex (%c-%c): ", I2A(0), I2A(n-1));
		put_str(buf, 20, 2);
		c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == 'S') return (FALSE);
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < n)) break;
		if (c == '?') do_cmd_help("help.hlp");
		else bell();
	}

	/* Set sex */
	p_ptr->psex = k;
	sp_ptr = &sex_info[p_ptr->psex];
	str = sp_ptr->title;

	/* Display */
	c_put_str(TERM_L_BLUE, str, 3, 15);

	/* Clean up */
	clear_from(15);


	/*** Player race ***/

	/* Extra info */
	Term_putstr(5, 15, -1, TERM_WHITE,
		"Your 'race' determines various intrinsic factors and bonuses.");

	/* Dump races */
	for (n = 0; n < MAX_RACES; n++)
	{
		/* Analyze */
		p_ptr->prace = n;
		rp_ptr = &race_info[p_ptr->prace];
		str = rp_ptr->title;
		
		/* Display */
		sprintf(buf, "%c%c %s", I2A(n), p2, str);
		put_str(buf, 21 + (n/5), 2 + 15 * (n%5));
	}

	/* Choose */
	while (1)
	{
		sprintf(buf, "Choose a race (%c-%c): ", I2A(0), I2A(n-1));
		put_str(buf, 20, 2);
		c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == 'S') return (FALSE);
		k = (islower(c) ? A2I(c) : -1);
		if ((k >= 0) && (k < n)) break;
		if (c == '?') do_cmd_help("help.hlp");
		else bell();
	}

	/* Set race */
	p_ptr->prace = k;
	rp_ptr = &race_info[p_ptr->prace];
	str = rp_ptr->title;

	/* Display */
	c_put_str(TERM_L_BLUE, str, 4, 15);

	/* Clean up */
	clear_from(15);
  
	/*** Maximize mode ***/

	/* Extra info */
	Term_putstr(5, 15, -1, TERM_WHITE,
		"Using 'maximize' mode makes the game harder at the start,");
	Term_putstr(5, 16, -1, TERM_WHITE,
		"but often makes it easier to win.");

	/* Ask about "maximize" mode */
 
	while (1)
	{
		put_str("Use 'maximize' mode? (Y/N/S/Q/?) ", 20, 2);
		c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == 'S') return (FALSE);
		if (c == ESCAPE) break;
		if ((c == 'y') || (c == 'n')) break;
		else if (c == '?') do_cmd_help("help.hlp");
		else bell();
	}

	/* Set "maximize" mode */
	p_ptr->maximize = (c == 'y');

	/* Clear */
	clear_from(15);

	/*** Preserve mode ***/

/* Extra info */
	Term_putstr(5, 15, -1, TERM_WHITE,
		"Using 'preserve' mode makes it difficult to 'lose' artifacts,");
	Term_putstr(5, 16, -1, TERM_WHITE,
		"but eliminates the 'special' feelings about some levels.");

	/* Ask about "preserve" mode */
	while (1)
	{
		put_str("Use 'preserve' mode? (Y/N/S/Q/?) ", 20, 2);
		c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == 'S') return (FALSE);
		if (c == ESCAPE) break;
		if ((c == 'y') || (c == 'n')) break;
		if (c == '?') do_cmd_help("help.hlp");
		else bell();
	}

	/* Set "preserve" mode */
	p_ptr->preserve = (c == 'y');

	/* Clear */
	clear_from(20);


	/* Choose stat mods */
	choose_mods();


#ifdef ALLOW_AUTOROLLER

	/*** Autoroll ***/

	/* Extra info */
	Term_putstr(5, 15, -1, TERM_WHITE,
		"The 'autoroller' allows you to specify certain 'minimal' stats,");
	Term_putstr(5, 16, -1, TERM_WHITE,
		"but be warned that your various stats may not be independant!");

	/* Ask about "auto-roller" mode */
	while (1)
	{
		put_str("Use the Auto-Roller? (Y/N/S/Q/?) ", 20, 2);
		c = inkey();
		if (c == 'Q') quit(NULL);
		if (c == 'S') return (FALSE);
		if (c == ESCAPE) break;
		if ((c == 'y') || (c == 'n')) break;
		if (c == '?') do_cmd_help("help.hlp");
		else bell();
	}

	/* Set "autoroll" */
	autoroll = (c == 'y');
	/* Clear */
	clear_from(20);
  
	/* Initialize */
	if (autoroll)
	{
		int mval[7];
		char inp[80];


		/* Clear fields */
		auto_round = 0L;
		last_round = 0L;

		/* Clean up */
		clear_from(10);

		/* Prompt for the minimum stats */
		put_str("Enter minimum attribute for: ", 14, 2);

		/* Output the maximum stats */
		for (i = 0; i < 6; i++)
		{
			/* Reset the "success" counter */
			stat_match[i] = 0;

			/* Race/Mod bonus */
			j = rp_ptr->r_adj[i] + p_ptr->stat_mod[i];

			/* Obtain the "maximal" stat */
			m = adjust_stat(17, j, TRUE);

			/* Save the maximum */
			mval[i] = m;

			/* Extract a textual format */
			/* cnv_stat(m, inp); */

			/* Above 18 */
			if (m > 18)
			{
				sprintf(inp, "(Max of 18/%02d):", (m - 18));
			}

			/* From 3 to 18 */
			else
			{
				sprintf(inp, "(Max of %2d):", m);
			}

			/* Prepare a prompt */
			sprintf(buf, "%-5s%-20s", stat_names[i], inp);

			/* Dump the prompt */
			put_str(buf, 15 + i, 5);
		}
		/* Input the minimum stats */
		for (i = 0; i < 6; i++)
		{
			/* Get a minimum stat */
			while (TRUE)
			{
				char *s;
				/* Move the cursor */
				put_str("", 15 + i, 30);
				/* Default */
				strcpy(inp, "");

				/* Get a response (or escape) */
				if (!askfor_aux(inp, 8)) inp[0] = '\0';

				/* Hack -- add a fake slash */
				strcat(inp, "/");

				/* Hack -- look for the "slash" */
				s = strchr(inp, '/');

				/* Hack -- Nuke the slash */
				*s++ = '\0';

				/* Hack -- Extract an input */
				v = atoi(inp) + atoi(s);

				/* Break on valid input */
				if (v <= mval[i]) break;
			}

			/* Save the minimum stat */
			stat_limit[i] = (v > 0) ? v : 0;
		}
	}

#endif /* ALLOW_AUTOROLLER */

	/* Clean up */
	clear_from(10);


	/*** Generate ***/

	/* Roll */
	while (TRUE)
	{
		/* Feedback */
		if (autoroll)
		{
			Term_clear();
			put_str("Name        :", 2, 1);
			put_str("Sex         :", 3, 1);
			put_str("Race        :", 4, 1);
			c_put_str(TERM_L_BLUE, player_name, 2, 15);
			c_put_str(TERM_L_BLUE, (p_ptr->psex ? "Male" : "Female"), 3,
15);
			c_put_str(TERM_L_BLUE, rp_ptr->title, 4, 15);
			/* Label stats */
			put_str("STR:", 2 + A_STR, 61);
			put_str("INT:", 2 + A_INT, 61);
			put_str("WIS:", 2 + A_WIS, 61);
			put_str("DEX:", 2 + A_DEX, 61);
			put_str("CON:", 2 + A_CON, 61);
			put_str("CHR:", 2 + A_CHR, 61);

			/* Note when we started */
			last_round = auto_round;

			/* Indicate the state */
			put_str("(Hit ESC to abort)", 11, 61);

			/* Label count */
			put_str("Round:", 9, 61);
		}

		/* Otherwise just get a character */
		else
		{
			/* Get a new character */
			get_stats();
		}

		/* Auto-roll */
		while (autoroll)
		{
			bool accept = TRUE;

			/* Get a new character */
			get_stats();

			/* Advance the round */
			auto_round++;

			/* Hack -- Prevent overflow */
			if (auto_round >= 1000000L) break;

			/* Check and count acceptable stats */
			for (i = 0; i < 6; i++)
			{
				/* This stat is okay */
				if (stat_use[i] >= stat_limit[i])
				{
				     stat_match[i]++;
				}
				/* This stat is not okay */
				else
				{
				     accept = FALSE;
				}
			}

			/* Break if "happy" */
			if (accept) break;

			/* Take note every 25 rolls */
			flag = (!(auto_round % 25L));

			/* Update display occasionally */
			if (flag || (auto_round < last_round + 100))
			{
				/* Dump data */
				birth_put_stats();

				/* Dump round */
				put_str(format("%6ld", auto_round), 9, 73);

				/* Make sure they see everything */
				Term_fresh();

				/* Delay 1/10 second */
#ifdef UNIX
				if (flag) Term_xtra(TERM_XTRA_DELAY, 100);
#endif

				/* Do not wait for a key */
				inkey_scan = TRUE;

				/* Check for a keypress */
				if (inkey()) break;
			}
		}

		/* Flush input */
		flush();


		/*** Display ***/

		/* Mode */
		mode = 0;


		/* Roll for base hitpoints */
		get_extra();

		/* Roll for age/height/weight */
		get_ahw();
		/* Roll for social class */
		get_history();
		/* Roll for gold */
		get_money();

		/* Roll skills */
		get_skills();

		/* Get exp */
		get_exp();

		/* Input loop */
		while (TRUE)
		{
			/* Calculate the bonuses and hitpoints */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* No magic yet */
			p_ptr->realm = NONE;
			p_ptr->luck_known = FALSE;
			mp_ptr = &magic_info[NONE];

			/* Flags */
			p_ptr->barehand = S_KARATE;
			p_ptr->wchange = 100+randint(150);

			p_ptr->schange = 0;

			/* Update stuff */
			update_stuff();

			/* Display the player */
			display_player(mode);

			/* Prepare a prompt (must squeeze everything in) */
			Term_gotoxy(2, 23);
			Term_addch(TERM_WHITE, b1);
			Term_addstr(-1, TERM_WHITE, "'r' to reroll");
			if (prev) Term_addstr(-1, TERM_WHITE, ", 'p' for prev");
			if (mode) Term_addstr(-1, TERM_WHITE, ", 'h' for Misc.");
			else Term_addstr(-1, TERM_WHITE, ", 'h' for History");
			Term_addstr(-1, TERM_WHITE, ", or ESC to accept");
			Term_addch(TERM_WHITE, b2);

			/* Prompt and get a command */
			c = inkey();

			/* Quit */
			if (c == 'Q') quit(NULL);

			/* Start over */
		    if (c == 'S') return (FALSE);

			/* Escape accepts the roll */
			if (c == ESCAPE) break;

			/* Reroll this character */
			if ((c == ' ') || (c == 'r')) break;

			/* Previous character */
			if (prev && (c == 'p'))
			{
				load_prev_data();
				continue;
			}

			/* Toggle the display */
			if ((c == 'H') || (c == 'h'))
			{
				mode = ((mode != 0) ? 0 : 1);
				continue;
			}

			/* Help */
			if (c == '?')
			{
				do_cmd_help("help.hlp");
				continue;
			}

			/* Warning */
			bell();
		}

		/* Are we done? */
		if (c == ESCAPE) break;

		/* Save this for the "previous" character */
		save_prev_data();

		/* Note that a previous roll exists */
		prev = TRUE;
	}

	/* Clear prompt */
	clear_from(23);


	/*** Finish up ***/

	/* Get a name, recolor it, prepare savefile */
	get_name();

	/* Prompt for it */
	prt("['Q' to suicide, 'S' to start over, or ESC to continue]", 23, 10);

	/* Get a key */
	c = inkey();
	/* Quit */
	if (c == 'Q') quit(NULL);

	/* Start over */
	if (c == 'S') return (FALSE);
	/* Accept */
	return (TRUE);
}


/*
 * Create a new character.
 *
 * Note that we may be called with "junk" leftover in the various
 * fields, so we must be sure to clear them first.
 */
void player_birth(void)
{
	int i, n;


	/* Create a new character */
	while (1)
	{
		/* Wipe the player */
		player_wipe();

		/* Roll up a new character */
		if (player_birth_aux()) break;
	}


	/* Note player birth in the message recall */
	message_add(" ");
	message_add("  ");
	message_add("====================");
	message_add("  ");
	message_add(" ");

	/* Shops */
	for (n = 0; n < MAX_STORES; n++)
	{
		/* Initialize */
		store_init(n);

		/* Ignore home */
		if (n == MAX_STORES - 2) continue;

		/* Maintain the shop (ten times) */
		for (i = 0; i < 10; i++) store_maint(n);
	}
}
