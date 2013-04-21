/* File: misc.c */

/* Purpose: misc code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"




/*
 * Converts stat num into a six-char (right justified) string
 */
void cnv_stat(int val, char *out_val)
{
    /* Above 18 */
    if (val > 18)
    {
        int bonus = (val - 18);

        if (bonus >= 220)
        {
            sprintf(out_val, "18/%3s", "***");
        }
        else if (bonus >= 100)
        {
            sprintf(out_val, "18/%03d", bonus);
        }
        else
        {
            sprintf(out_val, " 18/%02d", bonus);
        }
    }

    /* From 3 to 18 */
    else
    {
        sprintf(out_val, "    %2d", val);
    }
}



/*
 * Modify a stat value by a "modifier", return new value
 *
 * Stats go up: 3,4,...,17,18,18/10,18/20,...,18/220
 * Or even: 18/13, 18/23, 18/33, ..., 18/220
 *
 * Stats go down: 18/220, 18/210,..., 18/10, 18, 17, ..., 3
 * Or even: 18/13, 18/03, 18, 17, ..., 3
 */
s16b modify_stat_value(int value, int amount)
{
    int    i;

    /* Reward */
    if (amount > 0)
    {
        /* Apply each point */
        for (i = 0; i < amount; i++)
        {
            /* One point at a time */
            if (value < 18) value++;

            /* Ten "points" at a time */
            else value += 10;
        }
    }

    /* Penalty */
    else if (amount < 0)
    {
        /* Apply each point */
        for (i = 0; i < (0 - amount); i++)
        {
            /* Ten points at a time */
            if (value >= 18+10) value -= 10;

            /* Hack -- prevent weirdness */
            else if (value > 18) value = 18;

            /* One point at a time */
            else if (value > 3) value--;
        }
    }

    /* Return new value */
    return (value);
}



/*
 * Print character info at given row, column in a 13 char field
 */
static void prt_field(cptr info, int row, int col)
{
    /* Dump 13 spaces to clear */
    c_put_str(TERM_WHITE, "             ", row, col);

    /* Dump the info itself */
    c_put_str(TERM_L_BLUE, info, row, col);
}




/*
 * Print character stat in given row, column
 */
static void prt_stat(int stat)
{
    char tmp[32];

    /* Display "injured" stat */
    if (p_ptr->stat_cur[stat] < p_ptr->stat_max[stat])
    {
        put_str(stat_names_reduced[stat], ROW_STAT + stat, 0);
        cnv_stat(p_ptr->stat_use[stat], tmp);
        c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
    }

    /* Display "healthy" stat */
    else
    {
        put_str(stat_names[stat], ROW_STAT + stat, 0);
        cnv_stat(p_ptr->stat_use[stat], tmp);
        c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
    }
}




/*
 * Prints "title", including "wizard" or "winner" as needed.
 */
static void prt_title()
{
    cptr p = "";

    /* Wizard */
    if (wizard)
    {
        p = "[=-WIZARD-=]";
    }

    /* Winner */
    else if (total_winner || (p_ptr->lev > PY_MAX_LEVEL))
    {
        p = (p_ptr->male ? "**KING**" : "**QUEEN**");
    }

    /* Normal */
    else
    {
        p = player_title[p_ptr->pclass][(p_ptr->lev-1)/5];
    }

    prt_field(p, ROW_TITLE, COL_TITLE);
}


/*
 * Prints level
 */
static void prt_level()
{
    char tmp[32];

    sprintf(tmp, "%6d", p_ptr->lev);

    if (p_ptr->lev >= p_ptr->max_plv)
    {
        put_str("LEVEL ", ROW_LEVEL, 0);
        c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 6);
    }
    else
    {
        put_str("Level ", ROW_LEVEL, 0);
        c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 6);
    }
}


/*
 * Display the experience
 */
static void prt_exp()
{
    char out_val[32];

    (void)sprintf(out_val, "%8ld", (long)p_ptr->exp);

    if (p_ptr->exp >= p_ptr->max_exp)
    {
        put_str("EXP ", ROW_EXP, 0);
        c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 4);
    }
    else
    {
        put_str("Exp ", ROW_EXP, 0);
        c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 4);
    }
}


/*
 * Prints current gold
 */
static void prt_gold()
{
    char tmp[32];

    put_str("AU ", ROW_GOLD, COL_GOLD);
    sprintf(tmp, "%9ld", (long)p_ptr->au);
    c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_GOLD + 3);
}



/*
 * Prints current AC
 */
static void prt_ac()
{
    char tmp[32];

    put_str("Cur AC ", ROW_AC, COL_AC);
    sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
    c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 7);
}


/*
 * Prints Cur/Max hit points
 */
static void prt_hp()
{
    char tmp[32];

    byte color;


    put_str("Max HP ", ROW_MAXHP, COL_MAXHP);

    sprintf(tmp, "%5d", p_ptr->mhp);
    color = TERM_L_GREEN;

    c_put_str(color, tmp, ROW_MAXHP, COL_MAXHP + 7);


    put_str("Cur HP ", ROW_CURHP, COL_CURHP);

    sprintf(tmp, "%5d", p_ptr->chp);

    if (p_ptr->chp >= p_ptr->mhp)
    {
        color = TERM_L_GREEN;
    }
    else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
    {
        color = TERM_YELLOW;
    }
    else
    {
        color = TERM_RED;
    }

    c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 7);
}


/*
 * Prints players max/cur spell points
 */
static void prt_sp()
{
    char tmp[32];
    byte color;


    /* Do not show mana unless it matters */
    if (!mp_ptr->spell_book) return;


    put_str("Max SP ", ROW_MAXSP, COL_MAXSP);

    sprintf(tmp, "%5d", p_ptr->msp);
    color = TERM_L_GREEN;

    c_put_str(color, tmp, ROW_MAXSP, COL_MAXSP + 7);


    put_str("Cur SP ", ROW_CURSP, COL_CURSP);

    sprintf(tmp, "%5d", p_ptr->csp);

    if (p_ptr->csp >= p_ptr->msp)
    {
        color = TERM_L_GREEN;
    }
    else if (p_ptr->csp > (p_ptr->msp * hitpoint_warn) / 10)
    {
        color = TERM_YELLOW;
    }
    else
    {
        color = TERM_RED;
    }

    /* Show mana */
    c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 7);
}


/*
 * Prints depth in stat area
 */
static void prt_depth()
{
    char depths[32];

    if (!dun_level)
    {
        (void)strcpy(depths, "Town");
    }
    else if (depth_in_feet)
    {
        (void)sprintf(depths, "%d ft", dun_level * 50);
    }
    else
    {
        (void)sprintf(depths, "Lev %d", dun_level);
    }

    /* Right-Adjust the "depth", and clear old values */
    prt(format("%7s", depths), 23, COL_DEPTH);
}


/*
 * Prints status of hunger
 */
static void prt_hunger()
{
    /* Fainting / Starving */
    if (p_ptr->food < PY_FOOD_FAINT)
    {
        c_put_str(TERM_RED, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
    }

    /* Weak */
    else if (p_ptr->food < PY_FOOD_WEAK)
    {
        c_put_str(TERM_ORANGE, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
    }

    /* Hungry */
    else if (p_ptr->food < PY_FOOD_ALERT)
    {
        c_put_str(TERM_YELLOW, "Hungry", ROW_HUNGRY, COL_HUNGRY);
    }

    /* Normal */
    else if (p_ptr->food < PY_FOOD_FULL)
    {
        c_put_str(TERM_L_GREEN, "      ", ROW_HUNGRY, COL_HUNGRY);
    }

    /* Full */
    else if (p_ptr->food < PY_FOOD_MAX)
    {
        c_put_str(TERM_L_GREEN, "Full  ", ROW_HUNGRY, COL_HUNGRY);
    }

    /* Gorged */
    else
    {
        c_put_str(TERM_GREEN, "Gorged", ROW_HUNGRY, COL_HUNGRY);
    }
}


/*
 * Prints Blind status
 */
static void prt_blind(void)
{
    if (p_ptr->blind)
    {
        c_put_str(TERM_ORANGE, "Blind", ROW_BLIND, COL_BLIND);
    }
    else
    {
        put_str("     ", ROW_BLIND, COL_BLIND);
    }
}


/*
 * Prints Confusion status
 */
static void prt_confused(void)
{
    if (p_ptr->confused)
    {
        c_put_str(TERM_ORANGE, "Confused", ROW_CONFUSED, COL_CONFUSED);
    }
    else
    {
        put_str("        ", ROW_CONFUSED, COL_CONFUSED);
    }
}


/*
 * Prints Fear status
 */
static void prt_afraid()
{
    if (p_ptr->afraid)
    {
        c_put_str(TERM_ORANGE, "Afraid", ROW_AFRAID, COL_AFRAID);
    }
    else
    {
        put_str("      ", ROW_AFRAID, COL_AFRAID);
    }
}


/*
 * Prints Poisoned status
 */
static void prt_poisoned(void)
{
    if (p_ptr->poisoned)
    {
        c_put_str(TERM_ORANGE, "Poisoned", ROW_POISONED, COL_POISONED);
    }
    else
    {
        put_str("        ", ROW_POISONED, COL_POISONED);
    }
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static void prt_state(void)
{
    byte attr = TERM_WHITE;

    char text[16];


    /* Paralysis */
    if (p_ptr->paralyzed)
    {
        attr = TERM_RED;

        strcpy(text, "Paralyzed!");
    }

    /* Resting */
    else if (resting)
    {
        int i;

        /* Start with "Rest" */
        strcpy(text, "Rest      ");

        /* Extensive (timed) rest */
        if (resting >= 1000)
        {
            i = resting / 100;
            text[9] = '0';
            text[8] = '0';
            text[7] = '0' + (i % 10);
            if (i >= 10)
            {
                i = i / 10;
                text[6] = '0' + (i % 10);
                if (i >= 10)
                {
                    text[5] = '0' + (i / 10);
                }
            }
        }

        /* Long (timed) rest */
        else if (resting >= 100)
        {
            i = resting;
            text[9] = '0' + (i % 10);
            i = i / 10;
            text[8] = '0' + (i % 10);
            text[7] = '0' + (i / 10);
        }

        /* Medium (timed) rest */
        else if (resting >= 10)
        {
            i = resting;
            text[9] = '0' + (i % 10);
            text[8] = '0' + (i / 10);
        }

        /* Short (timed) rest */
        else if (resting > 0)
        {
            i = resting;
            text[9] = '0' + (i);
        }

        /* Rest until healed */
        else if (resting == -1)
        {
            text[5] = text[6] = text[7] = text[8] = text[9] = '*';
        }

        /* Rest until done */
        else if (resting == -2)
        {
            text[5] = text[6] = text[7] = text[8] = text[9] = '&';
        }
    }

    /* Repeating */
    else if (command_rep)
    {
        if (command_rep > 999)
        {
            (void)sprintf(text, "Rep. %3d00", command_rep / 100);
        }
        else
        {
            (void)sprintf(text, "Repeat %3d", command_rep);
        }
    }

    /* Searching */
    else if (p_ptr->searching)
    {
        strcpy(text, "Searching ");
    }

    /* Nothing interesting */
    else
    {
        strcpy(text, "          ");
    }

    /* Display the info (or blanks) */
    c_put_str(attr, text, ROW_STATE, COL_STATE);
}


/*
 * Prints the speed of a character.			-CJS-
 */
static void prt_speed()
{
    int i = p_ptr->pspeed;

    int attr = TERM_WHITE;
    char buf[32] = "";

    /* Hack -- Visually "undo" the Search Mode Slowdown */
    if (p_ptr->searching) i += 10;

    /* Fast */
    if (i > 110)
    {
        attr = TERM_L_GREEN;
        sprintf(buf, "Fast (+%d)", (i - 110));
    }

    /* Slow */
    else if (i < 110)
    {
        attr = TERM_L_UMBER;
        sprintf(buf, "Slow (-%d)", (110 - i));
    }

    /* Display the speed */
    c_put_str(attr, format("%-14s", buf), ROW_SPEED, COL_SPEED);
}


static void prt_study()
{
    if (p_ptr->new_spells)
    {
        put_str("Study", ROW_STUDY, 64);
    }
    else
    {
        put_str("     ", ROW_STUDY, COL_STUDY);
    }
}


static void prt_cut()
{
    int c = p_ptr->cut;

    if (c > 1000)
    {
        c_put_str(TERM_L_RED, "Mortal wound", ROW_CUT, COL_CUT);
    }
    else if (c > 200)
    {
        c_put_str(TERM_RED, "Deep gash   ", ROW_CUT, COL_CUT);
    }
    else if (c > 100)
    {
        c_put_str(TERM_RED, "Severe cut  ", ROW_CUT, COL_CUT);
    }
    else if (c > 50)
    {
        c_put_str(TERM_ORANGE, "Nasty cut   ", ROW_CUT, COL_CUT);
    }
    else if (c > 25)
    {
        c_put_str(TERM_ORANGE, "Bad cut     ", ROW_CUT, COL_CUT);
    }
    else if (c > 10)
    {
        c_put_str(TERM_YELLOW, "Light cut   ", ROW_CUT, COL_CUT);
    }
    else if (c)
    {
        c_put_str(TERM_YELLOW, "Graze       ", ROW_CUT, COL_CUT);
    }
    else
    {
        put_str("            ", ROW_CUT, COL_CUT);
    }
}



static void prt_stun(void)
{
    int s = p_ptr->stun;

    if (s > 100)
    {
        c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
    }
    else if (s > 50)
    {
        c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, COL_STUN);
    }
    else if (s)
    {
        c_put_str(TERM_ORANGE, "Stun        ", ROW_STUN, COL_STUN);
    }
    else
    {
        put_str("            ", ROW_STUN, COL_STUN);
    }
}



/*
 * Redraw the "monster health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "monster health bar" provides visual feedback on the "health"
 * of the monster currently being "tracked".  There are several ways
 * to "track" a monster, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the monster health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target monster when bored.  Note that the
 * health-bar stops tracking any monster that "disappears".
 */
static void health_redraw(void)
{

#ifdef DRS_SHOW_HEALTH_BAR

    /* Disabled */
    if (!show_health_bar) return;

    /* Not tracking */
    if (!health_who)
    {
        /* Erase the health bar */
        Term_erase(COL_INFO, ROW_INFO, 12, 1);
    }

    /* Tracking an unseen monster */
    else if (!m_list[health_who].ml)
    {
        /* Indicate that the monster health is "unknown" */
        Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");
    }

    /* Tracking a hallucinatory monster */
    else if (p_ptr->image)
    {
        /* Indicate that the monster health is "unknown" */
        Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");
    }

    /* Tracking a dead monster (???) */
    else if (!m_list[health_who].hp < 0)
    {
        /* Indicate that the monster health is "unknown" */
        Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");
    }

    /* Tracking a visible monster */
    else
    {
        int pct, len;
        /* Customized monster health bar -- colorless fear/sleep status */
        const char hb_normal[] = "**********";
        const char hb_fearful[] = "FFFFFFFFFF";
        const char hb_sleeping[] = "SSSSSSSSSS";
        const char *hb_ptr = hb_normal;

        monster_type *m_ptr = &m_list[health_who];

        /* Default to almost dead */
        byte attr = TERM_RED;

        /* Extract the "percent" of health */
        pct = 100L * m_ptr->hp / m_ptr->maxhp;

        /* Badly wounded */
        if (pct >= 10) attr = TERM_L_RED;

        /* Wounded */
        if (pct >= 25) attr = TERM_ORANGE;

        /* Somewhat Wounded */
        if (pct >= 60) attr = TERM_YELLOW;

        /* Healthy */
        if (pct >= 100) attr = TERM_L_GREEN;

        /* Afraid */
        if (m_ptr->monfear) { attr = TERM_VIOLET; hb_ptr = hb_fearful; }

        /* Asleep */
        if (m_ptr->csleep) { attr = TERM_BLUE; hb_ptr = hb_sleeping; }

        /* Convert percent into "health" */
        len = (pct < 10) ? 1 : (pct < 90) ? (pct / 10 + 1) : 10;

        /* Default to "unknown" */
        Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");

        /* Dump the current "health" (use appropriate symbols) */
        Term_putstr(COL_INFO + 1, ROW_INFO, len, attr, hb_ptr);
    }

#endif

}



/*
 * Display basic info (mostly left of map)
 */
static void prt_frame_basic()
{
    int i;

    /* Race and Class */
    prt_field(rp_ptr->title, ROW_RACE, COL_RACE);
    prt_field(cp_ptr->title, ROW_CLASS, COL_CLASS);

    /* Title */
    prt_title();

    /* Level/Experience */
    prt_level();
    prt_exp();

    /* All Stats */
    for (i = 0; i < 6; i++) prt_stat(i);

    /* Armor */
    prt_ac();

    /* Hitpoints */
    prt_hp();

    /* Spell points (mana) */
    prt_sp();

    /* Gold */
    prt_gold();

    /* Special */
    if (show_health_bar) health_redraw();

    /* Current depth (bottom right) */
    prt_depth();
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra()
{
    /* Cut/Stun */
    prt_cut();
    prt_stun();

    /* Food */
    prt_hunger();

    /* Various */
    prt_blind();
    prt_confused();
    prt_afraid();
    prt_poisoned();

    /* State */
    prt_state();

    /* Speed */
    prt_speed();

    /* Study spells */
    prt_study();
}



/*
 * Hack -- auto-map around the player
 */
static void fix_around(void)
{

#ifdef GRAPHIC_MIRROR

    /* Hack -- require a special window */
    if (term_mirror && use_mirror_around)
    {
        /* Use the recall window */
        Term_activate(term_mirror);

        /* Redraw map */
        display_map();

        /* Refresh */
        Term_fresh();

        /* Re-activate the main screen */
        Term_activate(term_screen);
    }

#endif

}


/*
 * Hack -- describe the current monster race
 */
static void fix_recent(void)
{

#ifdef GRAPHIC_RECALL

    /* Hack -- require a special window */
    if (term_recall && use_recall_recent)
    {
        /* Use the recall window */
        Term_activate(term_recall);

        /* Display monster info */
        if (recent_idx) display_roff(recent_idx);

        /* Refresh */
        Term_fresh();

        /* Re-activate the main screen */
        Term_activate(term_screen);
    }

#endif

#ifdef GRAPHIC_MIRROR

    /* Hack -- require a special window */
    if (term_mirror && use_mirror_recent)
    {
        /* Use the recall window */
        Term_activate(term_mirror);

        /* Display monster info */
        if (recent_idx) display_roff(recent_idx);

        /* Refresh */
        Term_fresh();

        /* Re-activate the main screen */
        Term_activate(term_screen);
    }

#endif

}


/*
 * Hack -- display inventory/equipment in the choice/mirror window
 */
static void fix_choose(void)
{

#ifdef GRAPHIC_CHOICE

    /* Show the inven/equip in "term_choice" */
    if (term_choice && use_choice_normal)
    {
        /* Activate the choice window */
        Term_activate(term_choice);

        /* Hack -- show inventory */
        if (!choose_default)
        {
            /* Display the inventory */
            display_inven();
        }

        /* Hack -- show equipment */
        else
        {
            /* Display the equipment */
            display_equip();
        }

        /* Refresh */
        Term_fresh();

        /* Re-activate the main screen */
        Term_activate(term_screen);
    }

#endif

#ifdef GRAPHIC_MIRROR

    /* Show the equip/inven in "term_mirror" */
    if (term_mirror && use_mirror_normal)
    {
        /* Activate the mirror window */
        Term_activate(term_mirror);

        /* Hack -- show inventory */
        if (choose_default)
        {
            /* Display the inventory */
            display_inven();
        }

        /* Hack -- show equipment */
        else
        {
            /* Display the equipment */
            display_equip();
        }

        /* Refresh */
        Term_fresh();

        /* Re-activate the main screen */
        Term_activate(term_screen);
    }

#endif

}



/*
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
static void calc_spells(void)
{
    int			i, j, k, levels;
    int			num_allowed, num_known;

    magic_type		*s_ptr;

    cptr p = ((mp_ptr->spell_book == TV_MAGIC_BOOK) ? "spell" : "prayer");


    /* Hack -- must be literate */
    if (!mp_ptr->spell_book) return;

    /* Hack -- wait for creation */
    if (!character_generated) return;

    /* Hack -- handle "xtra" mode */
    if (character_xtra) return;


    /* Determine the number of spells allowed */
    levels = p_ptr->lev - mp_ptr->spell_first + 1;

    /* Hack -- no negative spells */
    if (levels < 0) levels = 0;

    /* Extract total allowed spells */
    num_allowed = (adj_mag_study[p_ptr->stat_ind[mp_ptr->spell_stat]] *
                   levels / 2);

    /* Assume none known */
    num_known = 0;

    /* Count the number of spells we know */
    for (j = 0; j < 64; j++)
    {
        /* Count known spells */
        if ((j < 32) ?
            (spell_learned1 & (1L << j)) :
            (spell_learned2 & (1L << (j - 32))))
        {
            num_known++;
        }
    }

    /* See how many spells we must forget or may learn */
    p_ptr->new_spells = num_allowed - num_known;



    /* Forget spells which are too hard */
    for (i = 63; i >= 0; i--)
    {
        /* Efficiency -- all done */
        if (!spell_learned1 && !spell_learned2) break;

        /* Access the spell */
        j = spell_order[i];

        /* Skip non-spells */
        if (j >= 99) continue;

        /* Get the spell */
        s_ptr = &mp_ptr->info[j];

        /* Skip spells we are allowed to know */
        if (s_ptr->slevel <= p_ptr->lev) continue;

        /* Is it known? */
        if ((j < 32) ?
            (spell_learned1 & (1L << j)) :
            (spell_learned2 & (1L << (j - 32))))
        {
            /* Mark as forgotten */
            if (j < 32)
            {
                spell_forgotten1 |= (1L << j);
            }
            else
            {
                spell_forgotten2 |= (1L << (j - 32));
            }

            /* No longer known */
            if (j < 32)
            {
                spell_learned1 &= ~(1L << j);
            }
            else
            {
                spell_learned2 &= ~(1L << (j - 32));
            }

            /* Message */
            msg_format("You have forgotten the %s of %s.", p,
                       spell_names[mp_ptr->spell_type][j]);

            /* One more can be learned */
            p_ptr->new_spells++;
        }
    }


    /* Forget spells if we know too many spells */
    for (i = 63; i >= 0; i--)
    {
        /* Stop when possible */
        if (p_ptr->new_spells >= 0) break;

        /* Efficiency -- all done */
        if (!spell_learned1 && !spell_learned2) break;

        /* Get the (i+1)th spell learned */
        j = spell_order[i];

        /* Skip unknown spells */
        if (j >= 99) continue;

        /* Forget it (if learned) */
        if ((j < 32) ?
            (spell_learned1 & (1L << j)) :
            (spell_learned2 & (1L << (j - 32))))
        {
            /* Mark as forgotten */
            if (j < 32)
            {
                spell_forgotten1 |= (1L << j);
            }
            else
            {
                spell_forgotten2 |= (1L << (j - 32));
            }

            /* No longer known */
            if (j < 32)
            {
                spell_learned1 &= ~(1L << j);
            }
            else
            {
                spell_learned2 &= ~(1L << (j - 32));
            }

            /* Message */
            msg_format("You have forgotten the %s of %s.", p,
                       spell_names[mp_ptr->spell_type][j]);

            /* One more can be learned */
            p_ptr->new_spells++;
        }
    }


    /* Check for spells to remember */
    for (i = 0; i < 64; i++)
    {
        /* None left to remember */
        if (p_ptr->new_spells <= 0) break;

        /* Efficiency -- all done */
        if (!spell_forgotten1 && !spell_forgotten2) break;

        /* Get the next spell we learned */
        j = spell_order[i];

        /* Skip unknown spells */
        if (j >= 99) break;

        /* Access the spell */
        s_ptr = &mp_ptr->info[j];

        /* Skip spells we cannot remember */
        if (s_ptr->slevel > p_ptr->lev) continue;

        /* First set of spells */
        if ((j < 32) ?
            (spell_forgotten1 & (1L << j)) :
            (spell_forgotten2 & (1L << (j - 32))))
        {
            /* No longer forgotten */
            if (j < 32)
            {
                spell_forgotten1 &= ~(1L << j);
            }
            else
            {
                spell_forgotten2 &= ~(1L << (j - 32));
            }

            /* Known once more */
            if (j < 32)
            {
                spell_learned1 |= (1L << j);
            }
            else
            {
                 spell_learned2 |= (1L << (j - 32));
            }

            /* Message */
            msg_format("You have remembered the %s of %s.",
                       p, spell_names[mp_ptr->spell_type][j]);

            /* One less can be learned */
            p_ptr->new_spells--;
        }
    }


    /* Assume no spells available */
    k = 0;

    /* Count spells that can be learned */
    for (j = 0; j < 64; j++)
    {
        /* Access the spell */
        s_ptr = &mp_ptr->info[j];

        /* Skip spells we cannot remember */
        if (s_ptr->slevel > p_ptr->lev) continue;

        /* Skip spells we already know */
        if ((j < 32) ?
            (spell_learned1 & (1L << j)) :
            (spell_learned2 & (1L << (j - 32))))
        {
            continue;
        }

        /* Count it */
        k++;
    }

    /* Cannot learn more spells than exist */
    if (p_ptr->new_spells > k) p_ptr->new_spells = k;

    /* Learn new spells */
    if (p_ptr->new_spells && !p_ptr->old_spells)
    {
        /* Message */
        msg_format("You can learn some new %ss now.", p);

        /* Display "study state" later */
        p_ptr->redraw |= (PR_STUDY);
    }

    /* No more spells */
    else if (!p_ptr->new_spells && p_ptr->old_spells)
    {
        /* Display "study state" later */
        p_ptr->redraw |= (PR_STUDY);
    }

    /* Save the new_spells value */
    p_ptr->old_spells = p_ptr->new_spells;
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static void calc_mana(void)
{
    int		new_mana, levels, cur_wgt, max_wgt;

    object_type	*i_ptr;


    /* Hack -- Must be literate */
    if (!mp_ptr->spell_book) return;


    /* Extract "effective" player level */
    levels = (p_ptr->lev - mp_ptr->spell_first) + 1;

    /* Hack -- no negative mana */
    if (levels < 0) levels = 0;

    /* Extract total mana */
    new_mana = adj_mag_mana[p_ptr->stat_ind[mp_ptr->spell_stat]] * levels / 2;

    /* Hack -- usually add one mana */
    if (new_mana) new_mana++;


    /* Only mages are affected */
    if (mp_ptr->spell_book == TV_MAGIC_BOOK)
    {
        u32b f1, f2, f3;

        /* Assume player is not encumbered by gloves */
        p_ptr->cumber_glove = FALSE;

        /* Get the gloves */
        i_ptr = &inventory[INVEN_HANDS];

        /* Examine the gloves */
        object_flags(i_ptr, &f1, &f2, &f3);

        /* Normal gloves hurt mage-type spells */
        if (i_ptr->k_idx &&
            !(f2 & TR2_FREE_ACT) &&
            !((f1 & TR1_DEX) && (i_ptr->pval > 0)))
        {
            /* Encumbered */
            p_ptr->cumber_glove = TRUE;

            /* Reduce mana */
            new_mana = (3 * new_mana) / 4;
        }
    }


    /* Assume player not encumbered by armor */
    p_ptr->cumber_armor = FALSE;

    /* Weigh the armor */
    cur_wgt = 0;
    cur_wgt += inventory[INVEN_BODY].weight;
    cur_wgt += inventory[INVEN_HEAD].weight;
    cur_wgt += inventory[INVEN_ARM].weight;
    cur_wgt += inventory[INVEN_OUTER].weight;
    cur_wgt += inventory[INVEN_HANDS].weight;
    cur_wgt += inventory[INVEN_FEET].weight;

    /* Determine the weight allowance */
    max_wgt = mp_ptr->spell_weight;

    /* Heavy armor penalizes mana */
    if (((cur_wgt - max_wgt) / 10) > 0)
    {
        /* Encumbered */
        p_ptr->cumber_armor = TRUE;

        /* Reduce mana */
        new_mana -= ((cur_wgt - max_wgt) / 10);
    }


    /* Mana can never be negative */
    if (new_mana < 0) new_mana = 0;


    /* Maximum mana has changed */
    if (p_ptr->msp != new_mana)
    {
        /* Player has no mana now */
        if (!new_mana)
        {
            /* No mana left */
            p_ptr->csp = 0;
            p_ptr->csp_frac = 0;
        }

        /* Player had no mana, has some now */
        else if (!p_ptr->msp)
        {
            /* Reset mana */
            p_ptr->csp = new_mana;
            p_ptr->csp_frac = 0;
        }

        /* Player had some mana, adjust current mana */
        else
        {
            s32b value;

            /* change current mana proportionately to change of max mana, */
            /* divide first to avoid overflow, little loss of accuracy */
            value = ((((long)p_ptr->csp << 16) + p_ptr->csp_frac) /
                     p_ptr->msp * new_mana);

            /* Extract mana components */
            p_ptr->csp = (value >> 16);
            p_ptr->csp_frac = (value & 0xFFFF);
        }

        /* Save new mana */
        p_ptr->msp = new_mana;

        /* Display mana later */
        p_ptr->redraw |= (PR_MANA);
    }


    /* Hack -- handle "xtra" mode */
    if (character_xtra) return;

    /* Take note when "glove state" changes */
    if (p_ptr->old_cumber_glove != p_ptr->cumber_glove)
    {
        /* Message */
        if (p_ptr->cumber_glove)
        {
            msg_print("Your covered hands feel unsuitable for spellcasting.");
        }
        else
        {
            msg_print("Your hands feel more suitable for spellcasting.");
        }

        /* Save it */
        p_ptr->old_cumber_glove = p_ptr->cumber_glove;
    }


    /* Take note when "armor state" changes */
    if (p_ptr->old_cumber_armor != p_ptr->cumber_armor)
    {
        /* Message */
        if (p_ptr->cumber_armor)
        {
            msg_print("The weight of your armor encumbers your movement.");
        }
        else
        {
            msg_print("You feel able to move more freely.");
        }

        /* Save it */
        p_ptr->old_cumber_armor = p_ptr->cumber_armor;
    }
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
static void calc_hitpoints()
{
    int bonus, mhp;

    /* Un-inflate "half-hitpoint bonus per level" value */
    bonus = ((int)(adj_con_mhp[p_ptr->stat_ind[A_CON]]) - 128);

    /* Calculate hitpoints */
    mhp = player_hp[p_ptr->lev-1] + (bonus * p_ptr->lev / 2);

    /* Always have at least one hitpoint per level */
    if (mhp < p_ptr->lev + 1) mhp = p_ptr->lev + 1;

    /* Factor in the hero / superhero settings */
    if (p_ptr->hero) mhp += 10;
    if (p_ptr->shero) mhp += 30;

    /* New maximum hitpoints */
    if (mhp != p_ptr->mhp)
    {
        s32b value;

        /* change current hit points proportionately to change of mhp */
        /* divide first to avoid overflow, little loss of accuracy */
        value = (((long)p_ptr->chp << 16) + p_ptr->chp_frac) / p_ptr->mhp;
        value = value * mhp;
        p_ptr->chp = (value >> 16);
        p_ptr->chp_frac = (value & 0xFFFF);

        /* Save the new max-hitpoints */
        p_ptr->mhp = mhp;

        /* Display hitpoints (later) */
        p_ptr->redraw |= (PR_HP);
    }
}



/*
 * Extract and set the current "lite radius"
 */
static void calc_torch(void)
{
    object_type *i_ptr = &inventory[INVEN_LITE];

    /* Assume no light */
    p_ptr->cur_lite = 0;

    /* Player is glowing */
    if (p_ptr->lite) p_ptr->cur_lite = 1;

    /* Examine actual lites */
    if (i_ptr->tval == TV_LITE)
    {
        /* Torches (with fuel) provide some lite */
        if ((i_ptr->sval == SV_LITE_TORCH) && (i_ptr->pval > 0))
        {
            p_ptr->cur_lite = 1;
        }

        /* Lanterns (with fuel) provide more lite */
        if ((i_ptr->sval == SV_LITE_LANTERN) && (i_ptr->pval > 0))
        {
            p_ptr->cur_lite = 2;
        }

        /* Artifact Lites provide permanent, bright, lite */
        if (artifact_p(i_ptr)) p_ptr->cur_lite = 3;
    }

    /* Reduce lite when running if requested */
    if (running && view_reduce_lite)
    {
        /* Reduce the lite radius if needed */
        if (p_ptr->cur_lite > 1) p_ptr->cur_lite = 1;
    }

    /* Notice changes in the "lite radius" */
    if (p_ptr->old_lite != p_ptr->cur_lite)
    {
        /* Update the lite */
        p_ptr->update |= (PU_LITE);

        /* Update the monsters */
        p_ptr->update |= (PU_MONSTERS);

        /* Remember the old lite */
        p_ptr->old_lite = p_ptr->cur_lite;
    }
}



/*
 * Computes current weight limit.
 */
static int weight_limit(void)
{
    int i;

    /* Weight limit based only on strength */
    i = adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100;

    /* Return the result */
    return (i);
}


/*
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * This function induces various "status" messages.
 */
static void calc_bonuses(void)
{
    int			i, j, hold;

    int			old_speed;

    int			old_telepathy;
    int			old_see_inv;

    int			old_dis_ac;
    int			old_dis_to_a;

    int			extra_blows;
    int			extra_shots;

    object_type		*i_ptr;

    u32b		f1, f2, f3;


    /* Save the old speed */
    old_speed = p_ptr->pspeed;

    /* Save the old vision stuff */
    old_telepathy = p_ptr->telepathy;
    old_see_inv = p_ptr->see_inv;

    /* Save the old armor class */
    old_dis_ac = p_ptr->dis_ac;
    old_dis_to_a = p_ptr->dis_to_a;


    /* Clear extra blows/shots */
    extra_blows = extra_shots = 0;

    /* Clear the stat modifiers */
    for (i = 0; i < 6; i++) p_ptr->stat_add[i] = 0;


    /* Clear the Displayed/Real armor class */
    p_ptr->dis_ac = p_ptr->ac = 0;

    /* Clear the Displayed/Real Bonuses */
    p_ptr->dis_to_h = p_ptr->to_h = 0;
    p_ptr->dis_to_d = p_ptr->to_d = 0;
    p_ptr->dis_to_a = p_ptr->to_a = 0;


    /* Clear all the flags */
    p_ptr->aggravate = FALSE;
    p_ptr->teleport = FALSE;
    p_ptr->exp_drain = FALSE;
    p_ptr->bless_blade = FALSE;
    p_ptr->xtra_might = FALSE;
    p_ptr->impact = FALSE;
    p_ptr->see_inv = FALSE;
    p_ptr->free_act = FALSE;
    p_ptr->slow_digest = FALSE;
    p_ptr->regenerate = FALSE;
    p_ptr->ffall = FALSE;
    p_ptr->hold_life = FALSE;
    p_ptr->telepathy = FALSE;
    p_ptr->lite = FALSE;
    p_ptr->sustain_str = FALSE;
    p_ptr->sustain_int = FALSE;
    p_ptr->sustain_wis = FALSE;
    p_ptr->sustain_con = FALSE;
    p_ptr->sustain_dex = FALSE;
    p_ptr->sustain_chr = FALSE;
    p_ptr->resist_acid = FALSE;
    p_ptr->resist_elec = FALSE;
    p_ptr->resist_fire = FALSE;
    p_ptr->resist_cold = FALSE;
    p_ptr->resist_pois = FALSE;
    p_ptr->resist_conf = FALSE;
    p_ptr->resist_sound = FALSE;
    p_ptr->resist_lite = FALSE;
    p_ptr->resist_dark = FALSE;
    p_ptr->resist_chaos = FALSE;
    p_ptr->resist_disen = FALSE;
    p_ptr->resist_shard = FALSE;
    p_ptr->resist_nexus = FALSE;
    p_ptr->resist_blind = FALSE;
    p_ptr->resist_neth = FALSE;
    p_ptr->resist_fear = FALSE;
    p_ptr->immune_acid = FALSE;
    p_ptr->immune_elec = FALSE;
    p_ptr->immune_fire = FALSE;
    p_ptr->immune_cold = FALSE;



    /* Base infravision (purely racial) */
    p_ptr->see_infra = rp_ptr->infra;


    /* Base skill -- disarming */
    p_ptr->skill_dis = rp_ptr->r_dis + cp_ptr->c_dis;

    /* Base skill -- magic devices */
    p_ptr->skill_dev = rp_ptr->r_dev + cp_ptr->c_dev;

    /* Base skill -- saving throw */
    p_ptr->skill_sav = rp_ptr->r_sav + cp_ptr->c_sav;

    /* Base skill -- stealth */
    p_ptr->skill_stl = rp_ptr->r_stl + cp_ptr->c_stl;

    /* Base skill -- searching ability */
    p_ptr->skill_srh = rp_ptr->r_srh + cp_ptr->c_srh;

    /* Base skill -- searching frequency */
    p_ptr->skill_fos = rp_ptr->r_fos + cp_ptr->c_fos;

    /* Base skill -- combat (normal) */
    p_ptr->skill_thn = rp_ptr->r_thn + cp_ptr->c_thn;

    /* Base skill -- combat (shooting) */
    p_ptr->skill_thb = rp_ptr->r_thb + cp_ptr->c_thb;

    /* Base skill -- combat (throwing) */
    p_ptr->skill_tht = rp_ptr->r_thb + cp_ptr->c_thb;

    /* Base skill -- digging */
    p_ptr->skill_dig = 0;


    /* Elf */
    if (p_ptr->prace == RACE_ELF) p_ptr->resist_lite = TRUE;

    /* Hobbit */
    if (p_ptr->prace == RACE_HOBBIT) p_ptr->sustain_dex = TRUE;

    /* Gnome */
    if (p_ptr->prace == RACE_GNOME) p_ptr->free_act = TRUE;

    /* Dwarf */
    if (p_ptr->prace == RACE_DWARF) p_ptr->resist_blind = TRUE;

    /* Half-Orc */
    if (p_ptr->prace == RACE_HALF_ORC) p_ptr->resist_dark = TRUE;

    /* Half-Troll */
    if (p_ptr->prace == RACE_HALF_TROLL) p_ptr->sustain_str = TRUE;

    /* Dunadan */
    if (p_ptr->prace == RACE_DUNADAN) p_ptr->sustain_con = TRUE;

    /* High Elf */
    if (p_ptr->prace == RACE_HIGH_ELF) p_ptr->resist_lite = TRUE;
    if (p_ptr->prace == RACE_HIGH_ELF) p_ptr->see_inv = TRUE;

    /* Kobold */
    if (p_ptr->prace == RACE_KOBOLD) p_ptr->resist_pois = TRUE;

    /* Start with "normal" speed */
    p_ptr->pspeed = 110;

    /* Start with a single blow per turn */
    p_ptr->num_blow = 1;

    /* Start with a single shot per turn */
    p_ptr->num_fire = 1;

    /* Reset the "xtra" tval */
    p_ptr->tval_xtra = 0;

    /* Reset the "ammo" tval */
    p_ptr->tval_ammo = 0;


    /* Hack -- apply racial/class stat maxes */
    if (p_ptr->maximize)
    {
        /* Apply the racial modifiers */
        for (i = 0; i < 6; i++)
        {
            /* Modify the stats for "race" */
            p_ptr->stat_add[i] += (rp_ptr->r_adj[i] + cp_ptr->c_adj[i]);
        }
    }


    /* Scan the usable inventory */
    for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
    {
        i_ptr = &inventory[i];

        /* Skip missing items */
        if (!i_ptr->k_idx) continue;

        /* Extract the item flags */
        object_flags(i_ptr, &f1, &f2, &f3);

        /* Affect stats */
        if (f1 & TR1_STR) p_ptr->stat_add[A_STR] += i_ptr->pval;
        if (f1 & TR1_INT) p_ptr->stat_add[A_INT] += i_ptr->pval;
        if (f1 & TR1_WIS) p_ptr->stat_add[A_WIS] += i_ptr->pval;
        if (f1 & TR1_DEX) p_ptr->stat_add[A_DEX] += i_ptr->pval;
        if (f1 & TR1_CON) p_ptr->stat_add[A_CON] += i_ptr->pval;
        if (f1 & TR1_CHR) p_ptr->stat_add[A_CHR] += i_ptr->pval;

        /* Affect stealth */
        if (f1 & TR1_STEALTH) p_ptr->skill_stl += i_ptr->pval;

        /* Affect searching ability (factor of five) */
        if (f1 & TR1_SEARCH) p_ptr->skill_srh += (i_ptr->pval * 5);

        /* Affect searching frequency (factor of five) */
        if (f1 & TR1_SEARCH) p_ptr->skill_fos += (i_ptr->pval * 5);

        /* Affect infravision */
        if (f1 & TR1_INFRA) p_ptr->see_infra += i_ptr->pval;

        /* Affect digging (factor of 20) */
        if (f1 & TR1_TUNNEL) p_ptr->skill_dig += (i_ptr->pval * 20);

        /* Affect speed */
        if (f1 & TR1_SPEED) p_ptr->pspeed += i_ptr->pval;

        /* Affect blows */
        if (f1 & TR1_BLOWS) extra_blows += i_ptr->pval;

        /* Hack -- cause earthquakes */
        if (f1 & TR1_IMPACT) p_ptr->impact = TRUE;

        /* Boost shots */
        if (f3 & TR3_XTRA_SHOTS) extra_shots++;

        /* Various flags */
        if (f3 & TR3_AGGRAVATE) p_ptr->aggravate = TRUE;
        if (f3 & TR3_TELEPORT) p_ptr->teleport = TRUE;
        if (f3 & TR3_DRAIN_EXP) p_ptr->exp_drain = TRUE;
        if (f3 & TR3_BLESSED) p_ptr->bless_blade = TRUE;
        if (f3 & TR3_XTRA_MIGHT) p_ptr->xtra_might = TRUE;
        if (f3 & TR3_SLOW_DIGEST) p_ptr->slow_digest = TRUE;
        if (f3 & TR3_REGEN) p_ptr->regenerate = TRUE;
        if (f3 & TR3_TELEPATHY) p_ptr->telepathy = TRUE;
        if (f3 & TR3_LITE) p_ptr->lite = TRUE;
        if (f3 & TR3_SEE_INVIS) p_ptr->see_inv = TRUE;
        if (f3 & TR3_FEATHER) p_ptr->ffall = TRUE;
        if (f2 & TR2_FREE_ACT) p_ptr->free_act = TRUE;
        if (f2 & TR2_HOLD_LIFE) p_ptr->hold_life = TRUE;

        /* Immunity flags */
        if (f2 & TR2_IM_FIRE) p_ptr->immune_fire = TRUE;
        if (f2 & TR2_IM_ACID) p_ptr->immune_acid = TRUE;
        if (f2 & TR2_IM_COLD) p_ptr->immune_cold = TRUE;
        if (f2 & TR2_IM_ELEC) p_ptr->immune_elec = TRUE;

        /* Resistance flags */
        if (f2 & TR2_RES_ACID) p_ptr->resist_acid = TRUE;
        if (f2 & TR2_RES_ELEC) p_ptr->resist_elec = TRUE;
        if (f2 & TR2_RES_FIRE) p_ptr->resist_fire = TRUE;
        if (f2 & TR2_RES_COLD) p_ptr->resist_cold = TRUE;
        if (f2 & TR2_RES_POIS) p_ptr->resist_pois = TRUE;
        if (f2 & TR2_RES_CONF) p_ptr->resist_conf = TRUE;
        if (f2 & TR2_RES_SOUND) p_ptr->resist_sound = TRUE;
        if (f2 & TR2_RES_LITE) p_ptr->resist_lite = TRUE;
        if (f2 & TR2_RES_DARK) p_ptr->resist_dark = TRUE;
        if (f2 & TR2_RES_CHAOS) p_ptr->resist_chaos = TRUE;
        if (f2 & TR2_RES_DISEN) p_ptr->resist_disen = TRUE;
        if (f2 & TR2_RES_SHARDS) p_ptr->resist_shard = TRUE;
        if (f2 & TR2_RES_NEXUS) p_ptr->resist_nexus = TRUE;
        if (f2 & TR2_RES_BLIND) p_ptr->resist_blind = TRUE;
        if (f2 & TR2_RES_NETHER) p_ptr->resist_neth = TRUE;

        /* Sustain flags */
        if (f2 & TR2_SUST_STR) p_ptr->sustain_str = TRUE;
        if (f2 & TR2_SUST_INT) p_ptr->sustain_int = TRUE;
        if (f2 & TR2_SUST_WIS) p_ptr->sustain_wis = TRUE;
        if (f2 & TR2_SUST_DEX) p_ptr->sustain_dex = TRUE;
        if (f2 & TR2_SUST_CON) p_ptr->sustain_con = TRUE;
        if (f2 & TR2_SUST_CHR) p_ptr->sustain_chr = TRUE;

        /* Modify the base armor class */
        p_ptr->ac += i_ptr->ac;

        /* The base armor class is always known */
        p_ptr->dis_ac += i_ptr->ac;

        /* Apply the bonuses to armor class */
        p_ptr->to_a += i_ptr->to_a;

        /* Apply the mental bonuses to armor class, if known */
        if (object_known_p(i_ptr)) p_ptr->dis_to_a += i_ptr->to_a;

        /* Hack -- do not apply "weapon" bonuses */
        if (i == INVEN_WIELD) continue;

        /* Hack -- do not apply "bow" bonuses */
        if (i == INVEN_BOW) continue;

        /* Apply the bonuses to hit/damage */
        p_ptr->to_h += i_ptr->to_h;
        p_ptr->to_d += i_ptr->to_d;

        /* Apply the mental bonuses tp hit/damage, if known */
        if (object_known_p(i_ptr)) p_ptr->dis_to_h += i_ptr->to_h;
        if (object_known_p(i_ptr)) p_ptr->dis_to_d += i_ptr->to_d;
    }


    /* Calculate stats */
    for (i = 0; i < 6; i++)
    {
        int top, use, ind;


        /* Extract the new "stat_use" value for the stat */
        top = modify_stat_value(p_ptr->stat_max[i], p_ptr->stat_add[i]);

        /* Notice changes */
        if (p_ptr->stat_top[i] != top)
        {
            /* Save the new value */
            p_ptr->stat_top[i] = top;

            /* Redisplay the stats later */
            p_ptr->redraw |= (PR_STATS);
        }


        /* Extract the new "stat_use" value for the stat */
        use = modify_stat_value(p_ptr->stat_cur[i], p_ptr->stat_add[i]);

        /* Notice changes */
        if (p_ptr->stat_use[i] != use)
        {
            /* Save the new value */
            p_ptr->stat_use[i] = use;

            /* Redisplay the stats later */
            p_ptr->redraw |= (PR_STATS);
        }


        /* Values: 3, 4, ..., 17 */
        if (use <= 18) ind = (use - 3);

        /* Ranges: 18/00-18/09, ..., 18/210-18/219 */
        else if (use <= 18+219) ind = (15 + (use - 18) / 10);

        /* Range: 18/220+ */
        else ind = (37);

        /* Notice changes */
        if (p_ptr->stat_ind[i] != ind)
        {
            /* Save the new index */
            p_ptr->stat_ind[i] = ind;

            /* Change in CON affects Hitpoints */
            if (i == A_CON)
            {
                p_ptr->update |= (PU_HP);
            }

            /* Change in INT may affect Mana/Spells */
            else if (i == A_INT)
            {
                if (mp_ptr->spell_stat == A_INT)
                {
                    p_ptr->update |= (PU_MANA | PU_SPELLS);
                }
            }

            /* Change in WIS may affect Mana/Spells */
            else if (i == A_WIS)
            {
                if (mp_ptr->spell_stat == A_WIS)
                {
                    p_ptr->update |= (PU_MANA | PU_SPELLS);
                }
            }
        }
    }


    /* Apply temporary "stun" */
    if (p_ptr->stun > 50)
    {
        p_ptr->to_h -= 20;
        p_ptr->dis_to_h -= 20;
        p_ptr->to_d -= 20;
        p_ptr->dis_to_d -= 20;
    }
    else if (p_ptr->stun)
    {
        p_ptr->to_h -= 5;
        p_ptr->dis_to_h -= 5;
        p_ptr->to_d -= 5;
        p_ptr->dis_to_d -= 5;
    }


    /* Invulnerability */
    if (p_ptr->invuln)
    {
        p_ptr->to_a += 100;
        p_ptr->dis_to_a += 100;
    }

    /* Temporary blessing */
    if (p_ptr->blessed)
    {
        p_ptr->to_a += 5;
        p_ptr->dis_to_a += 5;
        p_ptr->to_h += 10;
        p_ptr->dis_to_h += 10;
    }

    /* Temprory shield */
    if (p_ptr->shield)
    {
        p_ptr->to_a += 50;
        p_ptr->dis_to_a += 50;
    }

    /* Temporary "Hero" */
    if (p_ptr->hero)
    {
        p_ptr->to_h += 12;
        p_ptr->dis_to_h += 12;
    }

    /* Temporary "Beserk" */
    if (p_ptr->shero)
    {
        p_ptr->to_h += 24;
        p_ptr->dis_to_h += 24;
        p_ptr->to_a -= 10;
        p_ptr->dis_to_a -= 10;
    }

    /* Temporary "fast" */
    if (p_ptr->fast)
    {
        p_ptr->pspeed += 10;
    }

    /* Temporary "slow" */
    if (p_ptr->slow)
    {
        p_ptr->pspeed -= 10;
    }

    /* Temporary see invisible */
    if (p_ptr->tim_invis)
    {
        p_ptr->see_inv = TRUE;
    }

    /* Temporary infravision boost */
    if (p_ptr->tim_infra)
    {
        p_ptr->see_infra++;
    }


    /* Hack -- Res Chaos -> Res Conf */
    if (p_ptr->resist_chaos)
    {
        p_ptr->resist_conf = TRUE;
    }

    /* Hack -- Hero/Shero -> Res fear */
    if (p_ptr->hero || p_ptr->shero)
    {
        p_ptr->resist_fear = TRUE;
    }


    /* Hack -- Telepathy Change */
    if (p_ptr->telepathy != old_telepathy)
    {
        p_ptr->update |= (PU_MONSTERS);
    }

    /* Hack -- See Invis Change */
    if (p_ptr->see_inv != old_see_inv)
    {
        p_ptr->update |= (PU_MONSTERS);
    }


    /* Extract the current weight (in tenth pounds) */
    j = total_weight;

    /* Extract the "weight limit" (in tenth pounds) */
    i = weight_limit();

    /* XXX XXX XXX Apply "encumbrance" from weight */
    if (j > i/2) p_ptr->pspeed -= ((j - (i/2)) / (i / 10));

    /* Bloating slows the player down (a little) */
    if (p_ptr->food >= PY_FOOD_MAX) p_ptr->pspeed -= 10;

    /* Searching slows the player down */
    if (p_ptr->searching) p_ptr->pspeed -= 10;

    /* Display the speed (if needed) */
    if (p_ptr->pspeed != old_speed) p_ptr->redraw |= (PR_SPEED);


    /* Actual Modifier Bonuses (Un-inflate stat bonuses) */
    p_ptr->to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
    p_ptr->to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
    p_ptr->to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
    p_ptr->to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);

    /* Displayed Modifier Bonuses (Un-inflate stat bonuses) */
    p_ptr->dis_to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
    p_ptr->dis_to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
    p_ptr->dis_to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
    p_ptr->dis_to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);


    /* Redraw armor (if needed) */
    if (p_ptr->dis_ac != old_dis_ac) p_ptr->redraw |= (PR_ARMOR);
    if (p_ptr->dis_to_a != old_dis_to_a) p_ptr->redraw |= (PR_ARMOR);



    /* Obtain the "hold" value */
    hold = adj_str_hold[p_ptr->stat_ind[A_STR]];


    /* Examine the "current bow" */
    i_ptr = &inventory[INVEN_BOW];


    /* Assume not heavy */
    p_ptr->heavy_shoot = FALSE;

    /* It is hard to carholdry a heavy bow */
    if (hold < i_ptr->weight / 10)
    {
        /* Hard to wield a heavy bow */
        p_ptr->to_h += 2 * (hold - i_ptr->weight / 10);
        p_ptr->dis_to_h += 2 * (hold - i_ptr->weight / 10);

        /* Heavy Bow */
        p_ptr->heavy_shoot = TRUE;
    }


    /* Compute "extra shots" if needed */
    if (i_ptr->k_idx && !p_ptr->heavy_shoot)
    {
        /* Take note of required "tval" for missiles */
        switch (i_ptr->sval)
        {
            case SV_SLING:
                p_ptr->tval_ammo = TV_SHOT;
                break;

            case SV_SHORT_BOW:
            case SV_LONG_BOW:
                p_ptr->tval_ammo = TV_ARROW;
                break;

            case SV_LIGHT_XBOW:
            case SV_HEAVY_XBOW:
                p_ptr->tval_ammo = TV_BOLT;
                break;
        }

        /* Hack -- Reward High Level Rangers using Bows */
        if ((p_ptr->pclass == CLASS_RANGER) && (p_ptr->tval_ammo == TV_ARROW))
        {
            /* Extra shot at level 20 */
            if (p_ptr->lev >= 20) p_ptr->num_fire++;

            /* Extra shot at level 40 */
            if (p_ptr->lev >= 40) p_ptr->num_fire++;
        }

        /* Add in the "bonus shots" */
        p_ptr->num_fire += extra_shots;

        /* Require at least one shot */
        if (p_ptr->num_fire < 1) p_ptr->num_fire = 1;

        /* Hack -- Give high-level Warriors Fear resistance */
        if ((p_ptr->pclass == CLASS_WARRIOR) && (p_ptr->lev >= 25))
        {
            p_ptr->resist_fear = TRUE;
        }
    }



    /* Examine the "main weapon" */
    i_ptr = &inventory[INVEN_WIELD];


    /* Assume not heavy */
    p_ptr->heavy_wield = FALSE;

    /* It is hard to hold a heavy weapon */
    if (hold < i_ptr->weight / 10)
    {
        /* Hard to wield a heavy weapon */
        p_ptr->to_h += 2 * (hold - i_ptr->weight / 10);
        p_ptr->dis_to_h += 2 * (hold - i_ptr->weight / 10);

        /* Heavy weapon */
        p_ptr->heavy_wield = TRUE;
    }


    /* Normal weapons */
    if (i_ptr->k_idx && !p_ptr->heavy_wield)
    {
        int str_index, dex_index;

        int num = 0, wgt = 0, mul = 0, div = 0;

        /* Analyze the class */
        switch (p_ptr->pclass)
        {
            /* Warrior */
            case CLASS_WARRIOR: num = 6; wgt = 30; mul = 5; break;

            /* Mage */
            case CLASS_MAGE:    num = 4; wgt = 35; mul = 3; div *= 2; break;

            /* Priest */
            case CLASS_PRIEST:  num = 5; wgt = 40; mul = 2; break;

            /* Rogue */
            case CLASS_ROGUE:   num = 5; wgt = 30; mul = 3; break;

            /* Ranger */
            case CLASS_RANGER:  num = 5; wgt = 35; mul = 4; break;

            /* Paladin */
            case CLASS_PALADIN: num = 5; wgt = 35; mul = 4; break;
        }

        /* Enforce a minimum "weight" (tenth pounds) */
        div = ((i_ptr->weight < wgt) ? wgt : i_ptr->weight);

        /* Access the strength vs weight */
        str_index = (adj_str_blow[p_ptr->stat_ind[A_STR]] * mul / div);

        /* Maximal value */
        if (str_index > 11) str_index = 11;

        /* Index by dexterity */
        dex_index = (adj_dex_blow[p_ptr->stat_ind[A_DEX]]);

        /* Maximal value */
        if (dex_index > 11) dex_index = 11;

        /* Use the blows table */
        p_ptr->num_blow = blows_table[str_index][dex_index];

        /* Maximal value */
        if (p_ptr->num_blow > num) p_ptr->num_blow = num;

        /* Add in the "bonus blows" */
        p_ptr->num_blow += extra_blows;

        /* Require at least one blow */
        if (p_ptr->num_blow < 1) p_ptr->num_blow = 1;

        /* Boost digging skill by weapon weight */
        p_ptr->skill_dig += (i_ptr->weight / 10);
    }


    /* Assume okay */
    p_ptr->icky_wield = FALSE;

    /* Priest weapon penalty for non-blessed edged weapons */
    if ((p_ptr->pclass == CLASS_PRIEST) && (!p_ptr->bless_blade) &&
        ((i_ptr->tval == TV_SWORD) || (i_ptr->tval == TV_POLEARM)))
    {
        /* Reduce the real bonuses */
        p_ptr->to_h -= 2;
        p_ptr->to_d -= 2;

        /* Reduce the mental bonuses */
        p_ptr->dis_to_h -= 2;
        p_ptr->dis_to_d -= 2;

        /* Icky weapon */
        p_ptr->icky_wield = TRUE;
    }


    /* Affect Skill -- stealth (bonus one) */
    p_ptr->skill_stl += 1;

    /* Affect Skill -- disarming (DEX and INT) */
    p_ptr->skill_dis += adj_dex_dis[p_ptr->stat_ind[A_DEX]];
    p_ptr->skill_dis += adj_int_dis[p_ptr->stat_ind[A_INT]];

    /* Affect Skill -- magic devices (INT) */
    p_ptr->skill_dev += adj_int_dev[p_ptr->stat_ind[A_INT]];

    /* Affect Skill -- saving throw (WIS) */
    p_ptr->skill_sav += adj_wis_sav[p_ptr->stat_ind[A_WIS]];

    /* Affect Skill -- digging (STR) */
    p_ptr->skill_dig += adj_str_dig[p_ptr->stat_ind[A_STR]];


    /* Affect Skill -- disarming (Level, by Class) */
    p_ptr->skill_dis += (cp_ptr->x_dis * p_ptr->lev / 10);

    /* Affect Skill -- magic devices (Level, by Class) */
    p_ptr->skill_dev += (cp_ptr->x_dev * p_ptr->lev / 10);

    /* Affect Skill -- saving throw (Level, by Class) */
    p_ptr->skill_sav += (cp_ptr->x_sav * p_ptr->lev / 10);

    /* Affect Skill -- stealth (Level, by Class) */
    p_ptr->skill_stl += (cp_ptr->x_stl * p_ptr->lev / 10);

    /* Affect Skill -- search ability (Level, by Class) */
    p_ptr->skill_srh += (cp_ptr->x_srh * p_ptr->lev / 10);

    /* Affect Skill -- search frequency (Level, by Class) */
    p_ptr->skill_fos += (cp_ptr->x_fos * p_ptr->lev / 10);

    /* Affect Skill -- combat (normal) (Level, by Class) */
    p_ptr->skill_thn += (cp_ptr->x_thn * p_ptr->lev / 10);

    /* Affect Skill -- combat (shooting) (Level, by Class) */
    p_ptr->skill_thb += (cp_ptr->x_thb * p_ptr->lev / 10);

    /* Affect Skill -- combat (throwing) (Level, by Class) */
    p_ptr->skill_tht += (cp_ptr->x_thb * p_ptr->lev / 10);


    /* Limit Skill -- stealth from 0 to 30 */
    if (p_ptr->skill_stl > 30) p_ptr->skill_stl = 30;
    if (p_ptr->skill_stl < 0) p_ptr->skill_stl = 0;

    /* Limit Skill -- digging from 1 up */
    if (p_ptr->skill_dig < 1) p_ptr->skill_dig = 1;


    /* Hack -- handle "xtra" mode */
    if (character_xtra) return;

    /* Take note when "heavy bow" changes */
    if (p_ptr->old_heavy_shoot != p_ptr->heavy_shoot)
    {
        /* Message */
        if (p_ptr->heavy_shoot)
        {
            msg_print("You have trouble wielding such a heavy bow.");
        }
        else if (inventory[INVEN_BOW].k_idx)
        {
            msg_print("You have no trouble wielding your bow.");
        }
        else
        {
            msg_print("You feel relieved to put down your heavy bow.");
        }

        /* Save it */
        p_ptr->old_heavy_shoot = p_ptr->heavy_shoot;
    }


    /* Take note when "heavy weapon" changes */
    if (p_ptr->old_heavy_wield != p_ptr->heavy_wield)
    {
        /* Message */
        if (p_ptr->heavy_wield)
        {
            msg_print("You have trouble wielding such a heavy weapon.");
        }
        else if (inventory[INVEN_WIELD].k_idx)
        {
            msg_print("You have no trouble wielding your weapon.");
        }
        else
        {
            msg_print("You feel relieved to put down your heavy weapon.");
        }

        /* Save it */
        p_ptr->old_heavy_wield = p_ptr->heavy_wield;
    }


    /* Take note when "illegal weapon" changes */
    if (p_ptr->old_icky_wield != p_ptr->icky_wield)
    {
        /* Message */
        if (p_ptr->icky_wield)
        {
            msg_print("You do not feel comfortable with your weapon.");
        }
        else if (inventory[INVEN_WIELD].k_idx)
        {
            msg_print("You feel comfortable with your weapon.");
        }
        else
        {
            msg_print("You feel more comfortable after removing your weapon.");
        }

        /* Save it */
        p_ptr->old_icky_wield = p_ptr->icky_wield;
    }
}



/*
 * Handle "p_ptr->notice"
 */
void notice_stuff(void)
{
    /* Notice stuff */
    if (!p_ptr->notice) return;


    /* Combine the pack */
    if (p_ptr->notice & PN_COMBINE)
    {
        p_ptr->notice &= ~(PN_COMBINE);
        combine_pack();
    }

    /* Reorder the pack */
    if (p_ptr->notice & PN_REORDER)
    {
        p_ptr->notice &= ~(PN_REORDER);
        reorder_pack();
    }
}


/*
 * Handle "p_ptr->update"
 */
void update_stuff(void)
{
    /* Update stuff */
    if (!p_ptr->update) return;


    if (p_ptr->update & PU_BONUS)
    {
        p_ptr->update &= ~(PU_BONUS);
        calc_bonuses();
    }

    if (p_ptr->update & PU_TORCH)
    {
        p_ptr->update &= ~(PU_TORCH);
        calc_torch();
    }

    if (p_ptr->update & PU_HP)
    {
        p_ptr->update &= ~(PU_HP);
        calc_hitpoints();
    }

    if (p_ptr->update & PU_MANA)
    {
        p_ptr->update &= ~(PU_MANA);
        calc_mana();
    }

    if (p_ptr->update & PU_SPELLS)
    {
        p_ptr->update &= ~(PU_SPELLS);
        calc_spells();
    }


    /* Character is not ready yet, no screen updates */
    if (!character_generated) return;


    /* Character is in "icky" mode, no screen updates */
    if (character_icky) return;


    if (p_ptr->update & PU_UN_LITE)
    {
        p_ptr->update &= ~(PU_UN_LITE);
        forget_lite();
    }

    if (p_ptr->update & PU_UN_VIEW)
    {
        p_ptr->update &= ~(PU_UN_VIEW);
        forget_view();
    }


    if (p_ptr->update & PU_VIEW)
    {
        p_ptr->update &= ~(PU_VIEW);
        update_view();
    }

    if (p_ptr->update & PU_LITE)
    {
        p_ptr->update &= ~(PU_LITE);
        update_lite();
    }


    if (p_ptr->update & PU_FLOW)
    {
        p_ptr->update &= ~(PU_FLOW);
        update_flow();
    }


    if (p_ptr->update & PU_DISTANCE)
    {
        p_ptr->update &= ~(PU_DISTANCE);
        p_ptr->update &= ~(PU_MONSTERS);
        update_monsters(TRUE);
    }

    if (p_ptr->update & PU_MONSTERS)
    {
        p_ptr->update &= ~(PU_MONSTERS);
        update_monsters(FALSE);
    }
}


/*
 * Handle "p_ptr->redraw"
 */
void redraw_stuff(void)
{
    /* Redraw stuff */
    if (!p_ptr->redraw) return;


    /* Hack -- Redraw "recent" monster race */
    if (p_ptr->redraw & PR_RECENT)
    {
        p_ptr->redraw &= ~(PR_RECENT);
        fix_recent();
    }

    /* Hack -- Redraw "choices" or whatever */
    if (p_ptr->redraw & PR_CHOOSE)
    {
        p_ptr->redraw &= ~(PR_CHOOSE);
        fix_choose();
    }


    /* Character is not ready yet, no screen updates */
    if (!character_generated) return;


    /* Character is in "icky" mode, no screen updates */
    if (character_icky) return;


    /* Hack -- Redraw "around" the player */
    if (p_ptr->redraw & PR_AROUND)
    {
        p_ptr->redraw &= ~(PR_AROUND);
        fix_around();
    }


    /* Hack -- clear the screen */
    if (p_ptr->redraw & PR_WIPE)
    {
        p_ptr->redraw &= ~PR_WIPE;
        msg_print(NULL);
        clear_screen();
    }


    if (p_ptr->redraw & PR_MAP)
    {
        p_ptr->redraw &= ~(PR_MAP);
        prt_map();
    }


    if (p_ptr->redraw & PR_BASIC)
    {
        p_ptr->redraw &= ~(PR_BASIC);
        p_ptr->redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
        p_ptr->redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
        p_ptr->redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
        p_ptr->redraw &= ~(PR_DEPTH | PR_HEALTH);
        prt_frame_basic();
    }

    if (p_ptr->redraw & PR_MISC)
    {
        p_ptr->redraw &= ~(PR_MISC);
        prt_field(rp_ptr->title, ROW_RACE, COL_RACE);
        prt_field(cp_ptr->title, ROW_CLASS, COL_CLASS);
    }

    if (p_ptr->redraw & PR_TITLE)
    {
        p_ptr->redraw &= ~(PR_TITLE);
        prt_title();
    }

    if (p_ptr->redraw & PR_LEV)
    {
        p_ptr->redraw &= ~(PR_LEV);
        prt_level();
    }

    if (p_ptr->redraw & PR_EXP)
    {
        p_ptr->redraw &= ~(PR_EXP);
        prt_exp();
    }

    if (p_ptr->redraw & PR_STATS)
    {
        p_ptr->redraw &= ~(PR_STATS);
        prt_stat(A_STR);
        prt_stat(A_INT);
        prt_stat(A_WIS);
        prt_stat(A_DEX);
        prt_stat(A_CON);
        prt_stat(A_CHR);
    }

    if (p_ptr->redraw & PR_ARMOR)
    {
        p_ptr->redraw &= ~(PR_ARMOR);
        prt_ac();
    }

    if (p_ptr->redraw & PR_HP)
    {
        p_ptr->redraw &= ~(PR_HP);
        prt_hp();
    }

    if (p_ptr->redraw & PR_MANA)
    {
        p_ptr->redraw &= ~(PR_MANA);
        prt_sp();
    }

    if (p_ptr->redraw & PR_GOLD)
    {
        p_ptr->redraw &= ~(PR_GOLD);
        prt_gold();
    }

    if (p_ptr->redraw & PR_DEPTH)
    {
        p_ptr->redraw &= ~(PR_DEPTH);
        prt_depth();
    }

    if (p_ptr->redraw & PR_HEALTH)
    {
        p_ptr->redraw &= ~(PR_HEALTH);
        if (show_health_bar) health_redraw();
    }


    if (p_ptr->redraw & PR_EXTRA)
    {
        p_ptr->redraw &= ~(PR_EXTRA);
        p_ptr->redraw &= ~(PR_CUT | PR_STUN);
        p_ptr->redraw &= ~(PR_HUNGER);
        p_ptr->redraw &= ~(PR_BLIND | PR_CONFUSED);
        p_ptr->redraw &= ~(PR_AFRAID | PR_POISONED);
        p_ptr->redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY);
        prt_frame_extra();
    }

    if (p_ptr->redraw & PR_CUT)
    {
        p_ptr->redraw &= ~(PR_CUT);
        prt_cut();
    }

    if (p_ptr->redraw & PR_STUN)
    {
        p_ptr->redraw &= ~(PR_STUN);
        prt_stun();
    }

    if (p_ptr->redraw & PR_HUNGER)
    {
        p_ptr->redraw &= ~(PR_HUNGER);
        prt_hunger();
    }

    if (p_ptr->redraw & PR_BLIND)
    {
        p_ptr->redraw &= ~(PR_BLIND);
        prt_blind();
    }

    if (p_ptr->redraw & PR_CONFUSED)
    {
        p_ptr->redraw &= ~(PR_CONFUSED);
        prt_confused();
    }

    if (p_ptr->redraw & PR_AFRAID)
    {
        p_ptr->redraw &= ~(PR_AFRAID);
        prt_afraid();
    }

    if (p_ptr->redraw & PR_POISONED)
    {
        p_ptr->redraw &= ~(PR_POISONED);
        prt_poisoned();
    }

    if (p_ptr->redraw & PR_STATE)
    {
        p_ptr->redraw &= ~(PR_STATE);
        prt_state();
    }

    if (p_ptr->redraw & PR_SPEED)
    {
        p_ptr->redraw &= ~(PR_SPEED);
        prt_speed();
    }

    if (p_ptr->redraw & PR_STUDY)
    {
        p_ptr->redraw &= ~(PR_STUDY);
        prt_study();
    }
}




/*
 * Handle "p_ptr->update" and "p_ptr->redraw"
 */
void handle_stuff(void)
{
    /* Update stuff */
    if (p_ptr->update) update_stuff();

    /* Redraw stuff */
    if (p_ptr->redraw) redraw_stuff();
}


