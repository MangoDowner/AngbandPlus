/* File: wizard2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"
#include "cmds.h"
#include "script.h"


#ifdef ALLOW_DEBUG


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_ben(void)
{

#ifdef MONSTER_FLOW

	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, y, x;


	for (i = 0; i < MONSTER_FLOW_DEPTH; ++i)
	{
		/* Update map */
		for (y = Term->offset_y; y < Term->offset_y + SCREEN_HGT; y++)
		{
			for (x = Term->offset_x; x < Term->offset_x + SCREEN_WID; x++)
			{
				byte a = TERM_RED;

				if (!in_bounds_fully(y, x)) continue;

				/* Display proper cost */
				if (cave_cost[y][x] != i) continue;

				/* Reliability in yellow */
				if (cave_when[y][x] == cave_when[py][px])
				{
					a = TERM_YELLOW;
				}

				/* Display player/floors/walls */
				if ((y == py) && (x == px))
				{
					print_rel('@', a, y, x);
				}
				else if (cave_floor_bold(y, x))
				{
					print_rel('*', a, y, x);
				}
				else
				{
					print_rel('#', a, y, x);
				}
			}
		}

		/* Prompt */
		prt(format("Depth %d: ", i), 0, 0);

		/* Get key */
		if (inkey() == ESCAPE) break;

		/* Redraw map */
		prt_map();
	}

	/* Done */
	prt("", 0, 0);

	/* Redraw map */
	prt_map();

#else /* MONSTER_FLOW */

	/* Oops */
	msg_print("Oops");

#endif /* MONSTER_FLOW */

}



/*
 * Output a long int in binary format.
 */
static void prt_binary(u32b flags, int row, int col)
{
	int i;
	u32b bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_BLUE, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_WHITE, '-');
		}
	}
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (target_okay())
	{
		/* Teleport to the target */
		teleport_player_to(p_ptr->target_row, p_ptr->target_col);
	}
}



/*
 * Aux function for "do_cmd_wiz_change()"
 */
static void do_cmd_wiz_change_aux(void)
{
	int i;

	int tmp_int;

	long tmp_long;

	char tmp_val[160];

	char ppp[80];


	/* Query the stats */
	for (i = 0; i < A_MAX; i++)
	{
		/* Prompt */
		strnfmt(ppp, sizeof(ppp), "%s (3-118): ", stat_names[i]);

		/* Default */
		strnfmt(tmp_val, sizeof(tmp_val), "%d", p_ptr->stat_max[i]);

		/* Query */
		if (!get_string(ppp, tmp_val, 4)) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > 18+100) tmp_int = 18+100;
		else if (tmp_int < 3) tmp_int = 3;

		/* Save it */
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = tmp_int;
	}


	/* Default */
	strnfmt(tmp_val, sizeof(tmp_val), "%ld", (long)(p_ptr->au));

	/* Query */
	if (!get_string("Gold: ", tmp_val, 10)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->au = tmp_long;


	/* Default */
	strnfmt(tmp_val, sizeof(tmp_val), "%ld", (long)(p_ptr->exp));

	/* Query */
	if (!get_string("Experience: ", tmp_val, 10)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->exp = tmp_long;

	/* Update */
	check_experience();

	/* Default */
	strnfmt(tmp_val, sizeof(tmp_val), "%ld", (long)(p_ptr->max_exp));

	/* Query */
	if (!get_string("Max Exp: ", tmp_val, 10)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->max_exp = tmp_long;

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
 * Wizard routines for creating objects and modifying them
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * Here are the low-level functions
 *
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
 * Display an item's properties
 */
static void wiz_display_item(const object_type *o_ptr)
{
	int j = 0;

	u32b f1, f2, f3, f4;

	char buf[256];


	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3, &f4);

	/* Clear screen */
	Term_clear();

	/* Describe fully */
	object_desc_spoil(buf, sizeof(buf), o_ptr, TRUE, 3);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
	           o_ptr->k_idx, k_info[o_ptr->k_idx].level,
	           o_ptr->tval, o_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
	           o_ptr->number, o_ptr->weight,
	           o_ptr->ac, o_ptr->dd, o_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
	           o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d), 6, j);

	prt(format("name1 = %-4d  name2 = %-4d  cost = %ld",
	           o_ptr->name1, o_ptr->name2, (long)object_value(o_ptr)), 7, j);

	prt(format("ident = %04x  timeout = %-d",
	           o_ptr->ident, o_ptr->timeout), 8, j);

	prt("+------------FLAGS1------------+", 10, j);
	prt("AFFECT..........SLAY.......BRAND", 11, j);
	prt("                ae      xxxpaefc", 12, j);
	prt("siwdcc  ssidsasmnvudotgddduoclio", 13, j);
	prt("tnieoh  trnipthgiinmrrnrrmniierl", 14, j);
	prt("rtsxna..lcfgdkttmldncltggndsdced", 15, j);
	prt_binary(f1, 16, j);

	prt("+------------FLAGS2------------+", 17, j);
	prt("SUST........IMM.RESIST.........", 18, j);
	prt("            afecaefcpfldbc s n  ", 19, j);
	prt("siwdcc      cilocliooeialoshnecd", 20, j);
	prt("tnieoh      irelierliatrnnnrethi", 21, j);
	prt("rtsxna......decddcedsrekdfddxhss", 22, j);
	prt_binary(f2, 23, j);

	prt("+------------FLAGS3------------+", 10, j+32);
	prt("s   ts h     tadiiii   aiehs  hp", 11, j+32);
	prt("lf  eefo     egrgggg  bcnaih  vr", 12, j+32);
	prt("we  lerl    ilgannnn  ltssdo  ym", 13, j+32);
	prt("da reied    merirrrr  eityew ccc", 14, j+32);
	prt("itlepnel    ppanaefc  svaktm uuu", 15, j+32);
	prt("ghigavai    aoveclio  saanyo rrr", 16, j+32);
	prt("seteticf    craxierl  etropd sss", 17, j+32);
	prt("trenhste    tttpdced  detwes eee", 18, j+32);
	prt_binary(f3, 19, j+32);
	
	/* flags4 ?? */
}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	int tval;
	cptr desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
static const tval_desc tvals[] =
{
	{ TV_SWORD,             "Sword"                },
	{ TV_POLEARM,           "Polearm"              },
	{ TV_HAFTED,            "Hafted Weapon"        },
	{ TV_BOW,               "Bow"                  },
	{ TV_ARROW,             "Arrows"               },
	{ TV_BOLT,              "Bolts"                },
	{ TV_SHOT,              "Shots"                },
	{ TV_SHIELD,            "Shield"               },
	{ TV_CROWN,             "Crown"                },
	{ TV_HELM,              "Helm"                 },
	{ TV_GLOVES,            "Gloves"               },
	{ TV_BOOTS,             "Boots"                },
	{ TV_CLOAK,             "Cloak"                },
	{ TV_DRAG_ARMOR,        "Dragon Scale Mail"    },
	{ TV_HARD_ARMOR,        "Hard Armor"           },
	{ TV_SOFT_ARMOR,        "Soft Armor"           },
	{ TV_RING,              "Ring"                 },
	{ TV_AMULET,            "Amulet"               },
	{ TV_LITE,              "Lite"                 },
	{ TV_POTION,            "Potion"               },
	{ TV_SCROLL,            "Scroll"               },
	{ TV_WAND,              "Wand"                 },
	{ TV_STAFF,             "Staff"                },
	{ TV_ROD,               "Rod"                  },
	{ TV_PRAYER_BOOK,       "Priest Book"          },
	{ TV_MAGIC_BOOK,        "Magic Book"           },
	{ TV_NEWM_BOOK,         "Book of Nature magic"    },
	{ TV_LUCK_BOOK,         "Book of Chance magic"    },
	{ TV_CHEM_BOOK,         "Alchemy Book"         },
	{ TV_DARK_BOOK,         "Book of Black magic"    },
	/*{ TV_MIND_BOOK,         "Book of Mind Powers"    },*/
	{ TV_SPIKE,             "Spikes"               },
	{ TV_DIGGING,           "Digger"               },
	{ TV_CHEST,             "Chest"                },
	{ TV_FOOD,              "Food"                 },
	{ TV_FLASK,             "Flask/grenade"        },
	{ TV_SPECIAL,           "Class Special"        },
	{ TV_SKELETON,          "Skeletons"            },
	{ TV_BOTTLE,            "Empty bottle"         },
	{ TV_JUNK,              "Junk"                 },
	{ TV_GOLD,              "Gold"                 },
	{ 0,                    NULL                   }
};



/*
 * Get an object kind for creation (or zero)
 *
 * List up to 60 choices in three columns
 */
static int wiz_create_itemtype(void)
{
	int i, num, max_num;
	int col, row;
	int tval;

	cptr tval_desc;
	char ch;

	int choice[60];
	static const char choice_name[] = "abcdefghijklmnopqrst"
	                                  "ABCDEFGHIJKLMNOPQRST"
	                                  "0123456789:;<=>?@%&*";
	const char *cp;

	char buf[160];


	/* Clear screen */
	Term_clear();

	/* Print all tval's and their descriptions */
	for (num = 0; (num < 60) && tvals[num].tval; num++)
	{
		row = 2 + (num % 20);
		col = 30 * (num / 20);
		ch  = choice_name[num];
		prt(format("[%c] %s", ch, tvals[num].desc), row, col);
	}

	/* We need to know the maximal possible tval_index */
	max_num = num;

	/* Choose! */
	if (!get_com("Get what type of object? ", &ch)) return (0);

	/* Analyze choice */
	num = -1;
	if ((cp = strchr(choice_name, ch)) != NULL)
		num = cp - choice_name;

	/* Bail out if choice is illegal */
	if ((num < 0) || (num >= max_num)) return (0);

	/* Base object type chosen, fill in tval */
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	/* Clear screen */
	Term_clear();

	/* We have to search the whole itemlist. */
	for (num = 0, i = 1; (num < 60) && (i < z_info->k_max); i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Analyze matching items */
		if (k_ptr->tval == tval)
		{
			/* Hack -- Skip instant artifacts */
			if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;

			/* Prepare it */
			row = 2 + (num % 20);
			col = 30 * (num / 20);
			ch  = choice_name[num];

			/* Get the "name" of object "i" */
			object_kind_name(buf, sizeof buf, i, TRUE);

			/* Print it */
			prt(format("[%c] %s", ch, buf), row, col);

			/* Remember the object index */
			choice[num++] = i;
		}
	}

	/* Me need to know the maximal possible remembered object_index */
	max_num = num;

	/* Choose! */
	if (!get_com(format("What Kind of %s? ", tval_desc), &ch)) return (0);

	/* Analyze choice */
	num = -1;
	if ((cp = strchr(choice_name, ch)) != NULL)
		num = cp - choice_name;

	/* Bail out if choice is "illegal" */
	if ((num < 0) || (num >= max_num)) return (0);

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(object_type *o_ptr)
{
	cptr p;
	char tmp_val[80];


	/* Hack -- leave artifacts alone (why? it's wizard mode) */
	/* if (artifact_p(o_ptr)) return; */

	p = "Enter new 'pval' setting: ";
	strnfmt(tmp_val, sizeof(tmp_val), "%d", o_ptr->pval);
	if (!get_string(p, tmp_val, 6)) return;
	o_ptr->pval = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_a' setting: ";
	strnfmt(tmp_val, sizeof(tmp_val), "%d", o_ptr->to_a);
	if (!get_string(p, tmp_val, 6)) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_h' setting: ";
	strnfmt(tmp_val, sizeof(tmp_val), "%d", o_ptr->to_h);
	if (!get_string(p, tmp_val, 6)) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_d' setting: ";
	strnfmt(tmp_val, sizeof(tmp_val), "%d", o_ptr->to_d);
	if (!get_string(p, tmp_val, 6)) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
static void wiz_reroll_item(object_type *o_ptr)
{
	object_type *i_ptr;
	object_type object_type_body;

	char ch;
	bool changed = FALSE;

	/* Hack -- leave artifacts alone */
	if (artifact_p(o_ptr)) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Copy the object */
	object_copy(i_ptr, o_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(i_ptr);

		/* Ask wizard what to do. */
		if (!get_com("[a]ccept, [c]ursed, [n]ormal, [g]ood, [e]xcellent, [s]pecial? ", &ch))
			break;

		/* Create/change it! */
		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Apply cursed magic, but first clear object */
		else if (ch == 'c' || ch == 'C')
		{
			object_prep(i_ptr, o_ptr->k_idx);
			apply_magic(i_ptr, -2, FALSE, FALSE, FALSE);
		}

		/* Apply normal magic, but first clear object */
		else if (ch == 'n' || ch == 'N')
		{
			object_prep(i_ptr, o_ptr->k_idx);
			apply_magic(i_ptr, p_ptr->depth, FALSE, FALSE, FALSE);
		}

		/* Apply good magic, but first clear object */
		else if (ch == 'g' || ch == 'G')
		{
			object_prep(i_ptr, o_ptr->k_idx);
			apply_magic(i_ptr, p_ptr->depth, FALSE, TRUE, FALSE);
		}

		/* Apply great magic, but first clear object */
		else if (ch == 'e' || ch == 'E')
		{
			object_prep(i_ptr, o_ptr->k_idx);
			apply_magic(i_ptr, p_ptr->depth, FALSE, TRUE, TRUE);
		}

		/* Apply great magic and allow artifacts */
		else if (ch == 's' || ch == 'S')
		{
			object_prep(i_ptr, o_ptr->k_idx);
			uniqdrop = 3;
			apply_magic(i_ptr, p_ptr->depth, TRUE, TRUE, TRUE);
			uniqdrop = 0;
		}
	}
	
	/* Remove broken flag from staffs with good egos */
	/* even if their object kind is normally broken. */
    if ((broken_p(i_ptr)) && (i_ptr->tval == TV_STAFF))
    {
		if (i_ptr->name2)
		{
           ego_item_type *e_ptr = &e_info[i_ptr->name2];

		   /* not broken */
		   if (e_ptr->cost) i_ptr->ident &= ~(IDENT_BROKEN);
        }
        /* not broken if it has good to-hit and to-dam bonuses */
        else if ((i_ptr->to_h > 2) && (i_ptr->to_d > 2))
        {
           i_ptr->ident &= ~(IDENT_BROKEN);
        }
    }

	/* Notice change */
	if (changed)
	{
		/* Restore the position information */
		i_ptr->iy = o_ptr->iy;
		i_ptr->ix = o_ptr->ix;
		i_ptr->next_o_idx = o_ptr->next_o_idx;
		i_ptr->marked = o_ptr->marked;

		/* Apply changes */
		object_copy(o_ptr, i_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
	}
}



/*
 * Maximum number of rolls
 */
#define TEST_ROLL 100000


/*
 * Try to create an item again. Output some statistics.    -Bernd-
 *
 * The statistics are correct now.  We acquire a clean grid, and then
 * repeatedly place an object in this grid, copying it into an item
 * holder, and then deleting the object.  We fiddle with the artifact
 * counter flags to prevent weirdness.  We use the items to collect
 * statistics on item creation relative to the initial item.
 */
static void wiz_statistics(object_type *o_ptr)
{
	long i, matches, better, worse, other;
	char ch;
	cptr quality;
	bool good, great;

	object_type *i_ptr;
	object_type object_type_body;

	cptr q = "Rolls: %ld, Matches: %ld, Better: %ld, Worse: %ld, Other: %ld";


	/* Mega-Hack -- allow multiple artifacts XXX XXX XXX */
	if (artifact_p(o_ptr)) a_info[o_ptr->name1].cur_num = 0;


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(o_ptr);

		/* Get choices */
		if (!get_com(pmt, &ch)) break;

		if (ch == 'n' || ch == 'N')
		{
			good = FALSE;
			great = FALSE;
			quality = "normal";
		}
		else if (ch == 'g' || ch == 'G')
		{
			good = TRUE;
			great = FALSE;
			quality = "good";
		}
		else if (ch == 'e' || ch == 'E')
		{
			good = TRUE;
			great = TRUE;
			quality = "excellent";
		}
		else
		{
#if 0 /* unused */
			good = FALSE;
			great = FALSE;
#endif /* unused */
			break;
		}

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.",
		           quality, p_ptr->depth);
		message_flush();

		/* Set counters to zero */
		matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= TEST_ROLL; i++)
		{
			/* Output every few rolls */
			if ((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if (inkey())
				{
					/* Flush */
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			/* Get local object */
			i_ptr = &object_type_body;

			/* Wipe the object */
			object_wipe(i_ptr);

			/* Create an object */
			make_object(i_ptr, good, great);


			/* Mega-Hack -- allow multiple artifacts XXX XXX XXX */
			if (artifact_p(i_ptr)) a_info[i_ptr->name1].cur_num = 0;


			/* Test for the same tval and sval. */
			if ((o_ptr->tval) != (i_ptr->tval)) continue;
			if ((o_ptr->sval) != (i_ptr->sval)) continue;

			/* Check for match */
			if ((i_ptr->pval == o_ptr->pval) &&
			    (i_ptr->to_a == o_ptr->to_a) &&
			    (i_ptr->to_h == o_ptr->to_h) &&
			    (i_ptr->to_d == o_ptr->to_d))
			{
				matches++;
			}

			/* Check for better */
			else if ((i_ptr->pval >= o_ptr->pval) &&
			         (i_ptr->to_a >= o_ptr->to_a) &&
			         (i_ptr->to_h >= o_ptr->to_h) &&
			         (i_ptr->to_d >= o_ptr->to_d))
			{
				better++;
			}

			/* Check for worse */
			else if ((i_ptr->pval <= o_ptr->pval) &&
			         (i_ptr->to_a <= o_ptr->to_a) &&
			         (i_ptr->to_h <= o_ptr->to_h) &&
			         (i_ptr->to_d <= o_ptr->to_d))
			{
				worse++;
			}

			/* Assume different */
			else
			{
				other++;
			}
		}

		/* Final dump */
		msg_format(q, i, matches, better, worse, other);
		message_flush();
	}


	/* Hack -- Normally only make a single artifact */
	if (artifact_p(o_ptr)) a_info[o_ptr->name1].cur_num = 1;
}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(object_type *o_ptr, bool carried)
{
	int tmp_int;
	char tmp_val[3];

	/* Never duplicate artifacts */
	if (artifact_p(o_ptr)) return;


	/* Default */
	strnfmt(tmp_val, sizeof(tmp_val), "%d", o_ptr->number);

	/* Query */
	if (get_string("Quantity: ", tmp_val, 3))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;

		/* Adjust total weight being carried */
		if (carried)
		{
			/* Remove the weight of the old number of objects */
			p_ptr->total_weight -= (o_ptr->number * o_ptr->weight);

			/* Add the weight of the new number of objects */
			p_ptr->total_weight += (tmp_int * o_ptr->weight);
		}

		/* Adjust charge for rods */
		if (o_ptr->tval == TV_ROD)
		{
			o_ptr->charges = (o_ptr->charges / o_ptr->number) * tmp_int;
		}

		/* Accept modifications */
		o_ptr->number = tmp_int;
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
	int item;

	object_type *i_ptr;
	object_type object_type_body;
	object_type *o_ptr;

	char ch;
	cptr q, s;

	bool changed = FALSE;


	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Save screen */
	screen_save();


	/* Get local object */
	i_ptr = &object_type_body;

	/* Copy object */
	object_copy(i_ptr, o_ptr);


	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(i_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch))
			break;

		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(i_ptr);
		}

		if (ch == 'r' || ch == 'r')
		{
			wiz_reroll_item(i_ptr);
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(i_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			bool carried = (item >= 0) ? TRUE : FALSE;
			wiz_quantity_item(i_ptr, carried);
		}
	}


	/* Load screen */
	screen_load();


	/* Accept change */
	if (changed)
	{
		/* Message */
		msg_print("Changes accepted.");

		/* Change */
		object_copy(o_ptr, i_ptr);
		
		/* cheat: completely identify it */
		/* but not artifacts because the note about when you find an */
		/* artifact is activated in the identify function */
		if ((cheat_noid) && (!artifact_p(o_ptr)))
		{
			object_aware(o_ptr);
			object_known(o_ptr);
			o_ptr->ident |= (IDENT_MENTAL);
		}

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
	}

	/* Ignore change */
	else
	{
		msg_print("Changes ignored.");
	}
}


/*
 * Wizard routine for creating objects
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items. XXX XXX XXX
 */
static void wiz_create_item(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	object_type *i_ptr;
	object_type object_type_body;

	int k_idx;


	/* Save screen */
	screen_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	/* Load screen */
	screen_load();


	/* Return if failed */
	if (!k_idx) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Create the item */
	object_prep(i_ptr, k_idx);

	/* Apply magic (no messages, no artifacts) */
	apply_magic(i_ptr, p_ptr->depth, FALSE, FALSE, FALSE);

	if (k_info[k_idx].tval == TV_GOLD)
	{
		/* Hack -- Base coin cost */
		s32b base = k_info[k_idx].cost;

		/* Determine how much the treasure is "worth" */
		i_ptr->pval = (base + (8L * randint(base)) + randint(8));
	}

	/* cheat: completely identify it */
	if ((cheat_noid) && (!artifact_p(i_ptr)))
	{
		object_aware(i_ptr);
		object_known(i_ptr);
		i_ptr->ident |= (IDENT_MENTAL);
	}

	/* Drop the object from heaven */
	drop_near(i_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Create the artifact with the specified number
 */
static void wiz_create_artifact(int a_idx)
{
	object_type *i_ptr;
	object_type object_type_body;
	int k_idx;

	artifact_type *a_ptr = &a_info[a_idx];

	/* Ignore "empty" artifacts */
	if (!a_ptr->name) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(i_ptr);

	/* Acquire the "kind" index */
	k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);

	/* Oops */
	if (!k_idx) return;

	/* Create the artifact */
	object_prep(i_ptr, k_idx);

	/* Save the name */
	i_ptr->name1 = a_idx;

	/* Extract the fields */
	i_ptr->pval = a_ptr->pval;
	i_ptr->ac = a_ptr->ac;
	i_ptr->dd = a_ptr->dd;
	i_ptr->ds = a_ptr->ds;
	i_ptr->to_a = a_ptr->to_a;
	i_ptr->to_h = a_ptr->to_h;
	i_ptr->to_d = a_ptr->to_d;
	i_ptr->weight = a_ptr->weight;
	i_ptr->esprace = a_ptr->esprace;

    /* Mark that the artifact has been created. */
	a_ptr->cur_num = 1;
		
	/* cheat: completely identify it */
	if (cheat_noid)
	{
		object_aware(i_ptr);
		object_known(i_ptr);
		i_ptr->ident |= (IDENT_MENTAL);
	}

	/* Drop the artifact from heaven */
	drop_near(i_ptr, -1, p_ptr->py, p_ptr->px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(void)
{
	int maxhp = p_ptr->mhp;
	if (p_ptr->timed[TMD_FALSE_LIFE]) maxhp += 2 * (p_ptr->lev + 10);

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
	p_ptr->chp = maxhp;
	p_ptr->chp_frac = 0;

	/* Restore mana */
	p_ptr->csp = p_ptr->msp;
	p_ptr->csp_frac = 0;

	/* Cure stuff */
	(void)clear_timed(TMD_BLIND);
	(void)clear_timed(TMD_CONFUSED);
	(void)clear_timed(TMD_POISONED);
	(void)clear_timed(TMD_AFRAID);
	(void)clear_timed(TMD_CHARM);
	(void)clear_timed(TMD_PARALYZED);
	(void)clear_timed(TMD_IMAGE);
	(void)clear_timed(TMD_STUN);
	(void)clear_timed(TMD_CUT);
	(void)clear_timed(TMD_SLOW);
	(void)clear_timed(TMD_AMNESIA);
	(void)clear_timed(TMD_CURSE);

	/* No longer hungry */
	(void)set_food(PY_FOOD_MAX - 1);

	/* Cure slime and silver poison */
	p_ptr->silver = PY_SILVER_HEALTHY;
	p_ptr->slime = PY_SLIME_HEALTHY;

	/* Redraw everything */
	do_cmd_redraw();
}


/* Create a chosen terrain type */
static void wiz_terrain(void)
{
	char ppp[80];
	char tmp_val[160];
	s16b tcode;
	int feat = 99;

	/* Prompt */
	strnfmt(ppp, sizeof(ppp), "Enter index of the terrain type to create (from terrain.txt):");
	/* (add more options later) */
	/*strnfmt(ppp, sizeof(ppp), "mode: 1=vault, 2=cavern, 3=force GV, 4=destroyed, or 20+theme code.");*/

	/* Default */
	strnfmt(tmp_val, sizeof(tmp_val), "1");

	/* Ask for a level */
	if (!get_string(ppp, tmp_val, 11)) return;

	/* Extract request */
	tcode = atoi(tmp_val);
	
	switch (tcode)
	{
		case 1: feat = FEAT_FLOOR;      break;
		case 2: feat = FEAT_INVIS;      break;
		case 3: feat = FEAT_GLYPH;      break;
		case 4: feat = FEAT_OPEN;       break;
		case 5: feat = FEAT_BROKEN;     break;
		case 6: feat = FEAT_LESS;       break;
		case 7: feat = FEAT_MORE;       break;
		case 16: feat = FEAT_TRAP_HEAD + 0x00;     break; /* trap door */
		case 17: feat = FEAT_TRAP_HEAD + 0x01;     break; /* pit/earthquake trap */
		case 18: feat = FEAT_TRAP_HEAD + 0x02;     break; /* spiked pit/deep pit trap */
		case 19: feat = FEAT_TRAP_HEAD + 0x03;     break; /* poisoned spiked pit trap */
		case 20: feat = FEAT_TRAP_HEAD + 0x04;     break; /* summoning rune trap */
		case 21: feat = FEAT_TRAP_HEAD + 0x05;     break; /* teleport trap */
		case 22: feat = FEAT_TRAP_HEAD + 0x06;     break; /* fire trap */
		case 23: feat = FEAT_TRAP_HEAD + 0x07;     break; /* acid trap */
		case 24: feat = FEAT_TRAP_HEAD + 0x08;     break; /* dart (slow) trap */
		case 25: feat = FEAT_TRAP_HEAD + 0x09;     break; /* dart (-STR) trap */
		case 26: feat = FEAT_TRAP_HEAD + 0x0A;     break; /* dart (-DEX)/drain charges trap */
		case 27: feat = FEAT_TRAP_HEAD + 0x0B;     break; /* dart (-CON) trap */
		case 28: feat = FEAT_TRAP_HEAD + 0x0C;     break; /* blinding gas trap */
		case 29: feat = FEAT_TRAP_HEAD + 0x0D;     break; /* confusion gas trap */
		case 30: feat = FEAT_TRAP_HEAD + 0x0E;     break; /* poison gas/ grepse poison trap */
		case 31: feat = FEAT_TRAP_HEAD + 0x0F;     break; /* sleep gas/ halucenation trap */
		case 32: feat = FEAT_TRAP_HEAD + 0x10;     break; /* dart (mana drain) trap */
		case 33: feat = FEAT_TRAP_HEAD + 0x11;     break; /* new trap */
		case 43: feat = FEAT_OPEN_PIT;      break;
		case 44: feat = FEAT_WATER;         break;
		case 45: feat = FEAT_DOOR_CLOSE;    break;
		case 46: feat = FEAT_DOOR_LOCKD;    break;
		case 47: feat = FEAT_DOOR_STUCK;    break;
		case 48: feat = FEAT_SECRET;        break;
		case 49: feat = FEAT_RUBBLE;        break;
		case 50: feat = FEAT_MAGMA;         break;
		case 51: feat = FEAT_QUARTZ;        break;
		case 52: feat = FEAT_MAGMA_H;       break;
		case 53: feat = FEAT_QUARTZ_H;      break;
		case 54: feat = FEAT_MAGMA_K;       break;
		case 55: feat = FEAT_QUARTZ_K;      break;
		case 56: feat = FEAT_WALL_EXTRA;    break;
		case 57: feat = FEAT_WALL_INNER;    break;
		case 58: feat = FEAT_WALL_OUTER;    break;
		case 59: feat = FEAT_WALL_SOLID;    break;
		case 60: feat = FEAT_PERM_EXTRA;    break;
		case 61: feat = FEAT_PERM_INNER;    break;
		case 62: feat = FEAT_PERM_OUTER;    break;
		case 63: feat = FEAT_PERM_SOLID;    break;
		case 64: feat = FEAT_SMRUBBLE;      break;
	}
	if ((tcode > 7) && (tcode < 16)) msg_print("You cannot create shops.");
	else if (feat == 99) msg_print("There is no feature with that index.");

	if (feat == 99) return;

	cave_set_feat(p_ptr->py, p_ptr->px, feat);

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Update stuff */
	update_stuff();
}


/* modify the jump to any level command */
void cmd_wiz_jump_mod(void)
{
	char ppp[80];
	char tmp_val[160];

	/* Prompt */
	strnfmt(ppp, sizeof(ppp), "mode: 1=vault, 2=cavern, 20=random theme, or 20+theme code.");
	/* (add more options later) */
	/*strnfmt(ppp, sizeof(ppp), "mode: 1=vault, 2=cavern, 3=force GV, 4=destroyed, or 20+theme code.");*/

	/* Default */
	strnfmt(tmp_val, sizeof(tmp_val), "1");

	/* Ask for a level */
	if (!get_string(ppp, tmp_val, 11)) return;

	/* Extract request */
	p_ptr->speclev = atoi(tmp_val);
}

/*
 * Go to any level
 */
void do_cmd_wiz_jump(bool limited)
{
	/* Ask for level */
	if (p_ptr->command_arg <= 0)
	{
		char ppp[80];

		char tmp_val[160];

		/* Prompt */
		if ((limited) && (p_ptr->max_depth < 99))
			strnfmt(ppp, sizeof(ppp), "Jump to level (0-%d): ", p_ptr->max_depth + 1);
		else if (limited)
			strnfmt(ppp, sizeof(ppp), "Jump to level (0-%d): ", p_ptr->max_depth);
		else strnfmt(ppp, sizeof(ppp), "Jump to level (0-%d): ", MAX_DEPTH-1);

		/* Default */
		strnfmt(tmp_val, sizeof(tmp_val), "%d", p_ptr->depth);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 11)) return;

		/* Extract request */
		p_ptr->command_arg = atoi(tmp_val);
	}

	/* Paranoia */
	if (p_ptr->command_arg < 0) p_ptr->command_arg = 0;

	/* Paranoia */
	if (p_ptr->command_arg > MAX_DEPTH - 1) p_ptr->command_arg = MAX_DEPTH - 1;

	/* teleporter box limits */
	if (limited)
	{
		if (p_ptr->command_arg == p_ptr->depth) return; /* no alter reality */
		else if ((p_ptr->command_arg > p_ptr->max_depth + 1) && (p_ptr->max_depth < 99)) 
			p_ptr->command_arg = p_ptr->depth + 1;
		else if (p_ptr->command_arg > p_ptr->max_depth + 1) p_ptr->command_arg = p_ptr->depth - 1;
	}

	/* Accept request */
	msg_format("You jump to dungeon level %d.", p_ptr->command_arg);

	/* New depth */
	p_ptr->depth = p_ptr->command_arg;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type *i_ptr;
	object_type object_type_body;

	/* Scan every object */
	for (i = 1; i < z_info->k_max; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= p_ptr->command_arg)
		{
			/* Get local object */
			i_ptr = &object_type_body;

			/* Prepare object */
			object_prep(i_ptr, i);

			/* Awareness */
			object_aware(i_ptr);
		}
	}
}


/*
 * Hack -- Rerate Hitpoints
 */
static void do_cmd_rerate(void)
{
	int min_value, max_value, i, percent;

	min_value = (PY_MAX_LEVEL * 3 * (p_ptr->hitdie - 1)) / 8;
	min_value += PY_MAX_LEVEL;

	max_value = (PY_MAX_LEVEL * 5 * (p_ptr->hitdie - 1)) / 8;
	max_value += PY_MAX_LEVEL;

	p_ptr->player_hp[0] = p_ptr->hitdie;

	/* Rerate */
	while (1)
	{
		/* Collect values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			p_ptr->player_hp[i] = randint(p_ptr->hitdie);
			p_ptr->player_hp[i] += p_ptr->player_hp[i - 1];
		}

		/* Legal values */
		if ((p_ptr->player_hp[PY_MAX_LEVEL - 1] >= min_value) &&
		    (p_ptr->player_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}

	percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
	                (p_ptr->hitdie + ((PY_MAX_LEVEL - 1) * p_ptr->hitdie)));

	/* Update and redraw hitpoints */
	p_ptr->update |= (PU_HP);
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);

	/* Handle stuff */
	handle_stuff();

	/* Message */
	msg_format("Current Life Rating is %d/100.", percent);
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
		(void)summon_specific(py, px, p_ptr->depth, 0);
	}
}


/*
 * Summon a creature of the specified type
 *
 * This function is rather dangerous XXX XXX XXX
 */
static void do_cmd_wiz_named(int r_idx, bool slp)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, x, y;
	monster_race *r_ptr;

	/* Paranoia */
	if (!r_idx) return;
	if (r_idx >= z_info->r_max-1) return;

	/* maximum population */
	r_ptr = &r_info[r_idx];
	if ((r_ptr->curpop + r_ptr->cur_num >= r_ptr->maxpop) && (r_ptr->maxpop))
	{
		if (!get_check("exceed racial monster population per game? ")) return;
	}

	/* Try 10 times */
	for (i = 0; i < 10; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&y, &x, py, px, d, 0);

		/* Require empty grids */
		if (!cave_can_occupy_bold(y, x)) continue;

		/* Place it (allow groups) */
		if (place_monster_aux(y, x, r_idx, slp, TRUE)) break;
	}
}



/*
 * Hack -- Delete all nearby monsters
 */
static void do_cmd_wiz_zap(int d)
{
	int i;

	/* Banish everyone nearby */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		 monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip nonmonsters (like ordinary trees) */
		if (r_ptr->flags7 & (RF7_NONMONSTER)) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > d) continue;

		/* Delete the monster */
		delete_monster_idx(i, FALSE);
	}

	/* Update monster list window */
	p_ptr->window |= PW_MONLIST;
}


/*
 * Un-hide all monsters
 */
static void do_cmd_wiz_unhide(int d)
{
	int i;

	/* Process monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;
		
		/* monster is temporarily dead */
		if (m_ptr->temp_death) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > d) continue;

		/* Optimize -- Repair flags */
		repair_mflag_mark = repair_mflag_show = TRUE;

		/* Detect the monster */
		m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

		/* Update the monster */
		update_mon(i, 0);
	}
}

#if blah
/* test in_same_room() */
static void wiztest_nsame_room(void)
{
	int dir, ty, tx;
	
	/* never use old target & target specific spot, not a direction */
	spellswitch = 13;
	if (!get_aim_dir(&dir)) return;
	
	ty = p_ptr->target_row;
	tx = p_ptr->target_col;
	
	if (in_same_room(p_ptr->py, p_ptr->px, ty, tx))
		msg_print("yes, that is in the same room.");
	else
		msg_print("no, not in the same room.");
	
	/* reset stuff */
	p_ptr->target_row = 0;
	p_ptr->target_col = 0;
	spellswitch = 0;
}
#endif


/*
 * Query the dungeon
 */
static void do_cmd_wiz_query(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int y, x;

	char cmd;

	u16b mask = 0x00;


	/* Get a "debug command" */
	if (!get_com("Debug Command Query: ", &cmd)) return;

	/* Extract a flag */
	switch (cmd)
	{
		case '0': mask = (1 << 0); break;
		case '1': mask = (1 << 1); break;
		case '2': mask = (1 << 2); break;
		case '3': mask = (1 << 3); break;
		case '4': mask = (1 << 4); break;
		case '5': mask = (1 << 5); break;
		case '6': mask = (1 << 6); break;
		case '7': mask = (1 << 7); break;

		case 'm': mask |= (CAVE_MARK); break;
		case 'g': mask |= (CAVE_GLOW); break;
		case 'r': mask |= (CAVE_ROOM); break;
		case 'i': mask |= (CAVE_ICKY); break;
		case 's': mask |= (CAVE_SEEN); break;
		case 'v': mask |= (CAVE_VIEW); break;
		case 't': mask |= (CAVE_TEMP); break;
		case 'w': mask |= (CAVE_WALL); break;
	}

	/* Scan map */
	for (y = Term->offset_y; y < Term->offset_y + SCREEN_HGT; y++)
	{
		for (x = Term->offset_x; x < Term->offset_x + SCREEN_WID; x++)
		{
			byte a = TERM_RED;

			if (!in_bounds_fully(y, x)) continue;

			/* Given mask, show only those grids */
			if (mask && !(cave_info[y][x] & mask)) continue;

			/* Given no mask, show unknown grids */
			if (!mask && (cave_info[y][x] & (CAVE_MARK))) continue;

			/* Color */
			if (cave_floor_bold(y, x)) a = TERM_YELLOW;

			/* Display player/floors/walls */
			if ((y == py) && (x == px))
			{
				print_rel('@', a, y, x);
			}
			else if (cave_floor_bold(y, x))
			{
				print_rel('*', a, y, x);
			}
			else
			{
				print_rel('#', a, y, x);
			}
		}
	}

	/* Get keypress */
	msg_print("Press any key.");
	message_flush();

	/* Redraw map */
	prt_map();
}


/*
 * Ask for and parse a "debug command"
 *
 * The "p_ptr->command_arg" may have been set.
 */
void do_cmd_debug(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	char cmd;


	/* Get a "debug command" */
	if (!get_com("Debug Command: ", &cmd)) return;

	/* Analyze the command */
	switch (cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		case '\n':
		case '\r':
		{
			break;
		}

#ifdef ALLOW_SPOILERS

		/* Hack -- Generate Spoilers */
		case '"':
		{
			do_cmd_spoilers();
			break;
		}

#endif


		/* Hack -- Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Cure all maladies */
		case 'a':
		{
			do_cmd_wiz_cure_all();
			break;
		}

		/* Teleport to target */
		case 'b':
		{
			do_cmd_wiz_bamf();
			break;
		}

		/* create Blah object(s) */
		case 'B':
		{
			spellswitch = 5;  /* creates object that's not good or great */
			if (p_ptr->command_arg <= 0) p_ptr->command_arg = 1;
			acquirement(py, px, p_ptr->command_arg, FALSE);
			spellswitch = 0;
			break;
		}

		/* Create chosen object */
		case 'c':
		{
			wiz_create_item();
			break;
		}

		/* Create an artifact */
		case 'C':
		{
			wiz_create_artifact(p_ptr->command_arg);
			break;
		}

		/* Detect everything */
		case 'D':
		{
			detect_all(TRUE);
			break;
		}

		/* Detect (almost) everything */
		case 'd':
		{
			detect_all(FALSE);
			break;
		}

		/* Edit character */
		case 'e':
		{
			do_cmd_wiz_change();
			break;
		}

		/* View item info */
		case 'f':
		{
			(void)identify_fully();
			break;
		}

		/* Good Objects */
		case 'g':
		{
			if (p_ptr->command_arg <= 0) p_ptr->command_arg = 1;
			acquirement(py, px, p_ptr->command_arg, FALSE);
			break;
		}

		/* Hitpoint rerating */
		case 'h':
		{
			do_cmd_rerate();
			break;
		}

		/* Identify */
		case 'i':
		{
			(void)ident_spell();
			break;
		}

		/* Go up or down in the dungeon (choose) */
		case 'J':
		{
			cmd_wiz_jump_mod();
			/* fall through */
		}

		/* Go up or down in the dungeon */
		case 'j':
		{
			do_cmd_wiz_jump(FALSE);
			break;
		}

		/* Self-Knowledge */
		case 'k':
		{
			self_knowledge(TRUE);
			break;
		}

		/* Learn about objects */
		case 'l':
		{
			do_cmd_wiz_learn();
			break;
		}

		/* Magic Mapping */
		case 'm':
		{
			map_area(0);
			break;
		}

		/* Summon Named Monster */
		case 'n':
		{
			do_cmd_wiz_named(p_ptr->command_arg, TRUE);
			break;
		}

		/* Object playing routines */
		case 'o':
		{
			do_cmd_wiz_play();
			break;
		}

		/* Phase Door */
		case 'p':
		{
			teleport_player(10);
			break;
		}

		/* Query the dungeon */
		case 'q':
		{
			do_cmd_wiz_query();
			break;
		}

		/* Summon Random Monster(s) */
		case 's':
		{
			if (p_ptr->command_arg <= 0) p_ptr->command_arg = 1;
			do_cmd_wiz_summon(p_ptr->command_arg);
			break;
		}

		/* Teleport */
		case 't':
		{
			teleport_player(100);
			break;
		}

		/* create chosen terrain */
		case 'T':
		{
			wiz_terrain();
			break;
		}

#if blah
		/* temporary wizmode command: test in_same_room() */
		case 'R':
		{
			wiztest_nsame_room();
			break;
		}
#endif

		/* Un-hide all monsters */
		case 'u':
		{
			if (p_ptr->command_arg <= 0) p_ptr->command_arg = 255;
			do_cmd_wiz_unhide(p_ptr->command_arg);
			break;
		}

		/* Very Good Objects */
		case 'v':
		{
			if (p_ptr->command_arg <= 0) p_ptr->command_arg = 1;
			acquirement(py, px, p_ptr->command_arg, TRUE);
			break;
		}

		/* Wizard Light the Level */
		case 'w':
		{
			wiz_lite(TRUE);
			break;
		}

		/* Increase Experience */
		case 'x':
		{
			if (p_ptr->command_arg)
			{
				gain_exp(p_ptr->command_arg);
			}
			else
			{
				gain_exp(p_ptr->exp + 1);
			}
			break;
		}

		/* Zap Monsters (Banishment) */
		case 'z':
		{
			if (p_ptr->command_arg <= 0) p_ptr->command_arg = MAX_SIGHT;
			do_cmd_wiz_zap(p_ptr->command_arg);
			break;
		}

		/* Hack */
		case '_':
		{
			do_cmd_wiz_hack_ben();
			break;
		}

		/* Oops */
		default:
		{
			msg_print("That is not a valid debug command.");
			break;
		}
	}
}


#endif


