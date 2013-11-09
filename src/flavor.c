/* Purpose: Object flavor code */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 */
static bool object_easy_know(int i)
{
    object_kind *k_ptr = &k_info[i];

    /* Analyze the "tval" */
    switch (k_ptr->tval)
    {
        /* Spellbooks */
        case TV_LIFE_BOOK:
        case TV_SORCERY_BOOK:
        case TV_NATURE_BOOK:
        case TV_CHAOS_BOOK:
        case TV_DEATH_BOOK:
        case TV_TRUMP_BOOK:
        case TV_ARCANE_BOOK:
        case TV_CRAFT_BOOK:
        case TV_DAEMON_BOOK:
        case TV_CRUSADE_BOOK:
        case TV_NECROMANCY_BOOK:
        case TV_ARMAGEDDON_BOOK:
        case TV_MUSIC_BOOK:
        case TV_HISSATSU_BOOK:
        case TV_HEX_BOOK:
        case TV_RAGE_BOOK:
        case TV_BURGLARY_BOOK:
        {
            return (TRUE);
        }

        /* Simple items */
        case TV_FLASK:
        case TV_JUNK:
        case TV_BOTTLE:
        case TV_SKELETON:
        case TV_SPIKE:
        case TV_WHISTLE:
        {
            return (TRUE);
        }

        /* All Food, Potions, Scrolls, Rods */
        case TV_FOOD:
        case TV_POTION:
        case TV_SCROLL:
        case TV_ROD:
        {
            return (TRUE);
        }
    }

    /* Nope */
    return (FALSE);
}


/*
 * Create a name from random parts.
 */
void get_table_name_aux(char *out_string)
{
#define MAX_SYLLABLES 164       /* Used with scrolls (see below) */

    static cptr syllables[MAX_SYLLABLES] = {
        "a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
        "arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
        "bie", "bin", "bit", "bjor", "blu", "bot", "bu",
        "byt", "comp", "con", "cos", "cre", "dalf", "dan",
        "den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
        "esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
        "gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
        "ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
        "kho", "kli", "klis", "la", "lech", "man", "mar",
        "me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
        "nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
        "od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
        "ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
        "ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
        "see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
        "sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
        "tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
        "ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
        "vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
        "yerg", "yp", "zun", "tri", "blaa", "jah", "bul", "on",
        "foo", "ju", "xuxu"
    };

    int testcounter = randint1(3) + 1;

    strcpy(out_string, "");

    if (randint1(3) == 2)
    {
        while (testcounter--)
            strcat(out_string, syllables[randint0(MAX_SYLLABLES)]);
    }
    else
    {
        char Syllable[80];
        testcounter = randint1(2) + 1;
        while (testcounter--)
        {
            (void)get_rnd_line("elvish.txt", 0, Syllable);
            strcat(out_string, Syllable);
        }
    }

    out_string[0] = toupper(out_string[1]);

    out_string[16] = '\0';
}


/*
 * Create a name from random parts with quotes.
 */
void get_table_name(char *out_string)
{
    char buff[80];
    get_table_name_aux(buff);

    sprintf(out_string, "'%s'", buff);
}


/*
 * Make random Sindarin name
 */
void get_table_sindarin_aux(char *out_string)
{
    char Syllable[80];

    get_rnd_line("sname.txt", 1, Syllable);
    strcpy(out_string, Syllable);

    get_rnd_line("sname.txt", 2, Syllable);
    strcat(out_string, Syllable);
}


/*
 * Make random Sindarin name with quotes
 */
void get_table_sindarin(char *out_string)
{
    char buff[80];
    get_table_sindarin_aux(buff);

    sprintf(out_string, "'%s'", buff);
}


/*
 * Shuffle flavor indices of a group of objects with given tval
 */
static void shuffle_flavors(byte tval)
{
    s16b *k_idx_list;
    int k_idx_list_num = 0;
    int i;

    /* Allocate an array for a list of k_idx */
    C_MAKE(k_idx_list, max_k_idx, s16b);

    /* Search objects with given tval for shuffle */
    for (i = 0; i < max_k_idx; i++)
    {
        object_kind *k_ptr = &k_info[i];

        /* Skip non-Rings */
        if (k_ptr->tval != tval) continue;

        /* Paranoia -- Skip objects without flavor */
        if (!k_ptr->flavor) continue;

        /* Skip objects with a fixed flavor name */
        if (have_flag(k_ptr->flags, TR_FIXED_FLAVOR)) continue;

        /* Remember k_idx */
        k_idx_list[k_idx_list_num] = i;

        /* Increase number of remembered indices */
        k_idx_list_num++;
    }

    /* Shuffle flavors */
    for (i = 0; i < k_idx_list_num; i++)
    {
        object_kind *k1_ptr = &k_info[k_idx_list[i]];
        object_kind *k2_ptr = &k_info[k_idx_list[randint0(k_idx_list_num)]];

        /* Swap flavors of this pair */
        s16b tmp = k1_ptr->flavor;
        k1_ptr->flavor = k2_ptr->flavor;
        k2_ptr->flavor = tmp;
    }

    /* Free an array for a list of k_idx */
    C_KILL(k_idx_list, max_k_idx, s16b);
}

/*
 * Prepare the "variable" part of the "k_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
void flavor_init(void)
{
    int i;

    /* Hack -- Use the "simple" RNG */
    Rand_quick = TRUE;

    /* Hack -- Induce consistant flavors */
    Rand_value = seed_flavor;


    /* Initialize flavor index of each object by itself */
    for (i = 0; i < max_k_idx; i++)
    {
        object_kind *k_ptr = &k_info[i];

        /* Skip objects without flavor name */
        if (!k_ptr->flavor_name) continue;

        /*
         * Initialize flavor index to itself
         *  -> Shuffle it later
         */
        k_ptr->flavor = i;
    }

    /* Shuffle Staves */
    shuffle_flavors(TV_STAFF);

    /* Shuffle Wands */
    shuffle_flavors(TV_WAND);

    /* Shuffle Rods */
    shuffle_flavors(TV_ROD);

    /* Shuffle Mushrooms */
    shuffle_flavors(TV_FOOD);

    /* Shuffle Potions */
    shuffle_flavors(TV_POTION);

    /* Shuffle Scrolls */
    shuffle_flavors(TV_SCROLL);


    /* Hack -- Use the "complex" RNG */
    Rand_quick = FALSE;

    /* Analyze every object */
    for (i = 1; i < max_k_idx; i++)
    {
        object_kind *k_ptr = &k_info[i];

        /* Skip "empty" objects */
        if (!k_ptr->name) continue;

        /* No flavor yields aware */
        if (!k_ptr->flavor) k_ptr->aware = TRUE;

        /* Check for "easily known" */
        k_ptr->easy_know = object_easy_know(i);
    }
}


/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
    /* Copy the char */
    *t++ = c;

    /* Terminate */
    *t = '\0';

    /* Result */
    return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
    /* Copy the string */
    while (*s) *t++ = *s++;

    /* Terminate */
    *t = '\0';

    /* Result */
    return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
    uint p;

    /* Find "size" of "n" */
    for (p = 1; n >= p * 10; p = p * 10) /* loop */;

    /* Dump each digit */
    while (p >= 1)
    {
        /* Dump the digit */
        *t++ = '0' + n / p;

        /* Remove the digit */
        n = n % p;

        /* Process next digit */
        p = p / 10;
    }

    /* Terminate */
    *t = '\0';

    /* Result */
    return (t);
}





/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
    uint p, n;

    /* Negative */
    if (v < 0)
    {
        /* Take the absolute value */
        n = 0 - v;

        /* Use a "minus" sign */
        *t++ = '-';
    }

    /* Positive (or zero) */
    else
    {
        /* Use the actual number */
        n = v;

        /* Use a "plus" sign */
        *t++ = '+';
    }

    /* Find "size" of "n" */
    for (p = 1; n >= p * 10; p = p * 10) /* loop */;

    /* Dump each digit */
    while (p >= 1)
    {
        /* Dump the digit */
        *t++ = '0' + n / p;

        /* Remove the digit */
        n = n % p;

        /* Process next digit */
        p = p / 10;
    }

    /* Terminate */
    *t = '\0';

    /* Result */
    return (t);
}


/*
 * Structs and tables for Auto Inscription for flags
 */

typedef struct flag_insc_table
{
    cptr english;
    int flag;
    int except_flag;
} flag_insc_table;

static flag_insc_table flag_insc_plus[] =
{
    { "At", TR_BLOWS, -1 },
    { "Sp", TR_SPEED, -1 },
    { "St", TR_STR, -1 },
    { "In", TR_INT, -1 },
    { "Wi", TR_WIS, -1 },
    { "Dx", TR_DEX, -1 },
    { "Cn", TR_CON, -1 },
    { "Ch", TR_CHR, -1 },
    { "Md", TR_MAGIC_MASTERY, -1 },
    { "Sl", TR_STEALTH, -1 },
    { "Sr", TR_SEARCH, -1 },
    { "If", TR_INFRA, -1 },
    { "Dg", TR_TUNNEL, -1 },
    { "Lf", TR_LIFE, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_minus[] =
{
    { "St", TR_DEC_STR, -1 },
    { "In", TR_DEC_INT, -1 },
    { "Wi", TR_DEC_WIS, -1 },
    { "Dx", TR_DEC_DEX, -1 },
    { "Cn", TR_DEC_CON, -1 },
    { "Ch", TR_DEC_CHR, -1 },
    { "Sl", TR_DEC_STEALTH, -1 },
    { "Sp", TR_DEC_SPEED, -1 },
    { "Lf", TR_DEC_LIFE, -1 },
    { "Md", TR_DEC_MAGIC_MASTERY, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_immune[] =
{
    { "Ac", TR_IM_ACID, -1 },
    { "El", TR_IM_ELEC, -1 },
    { "Fi", TR_IM_FIRE, -1 },
    { "Co", TR_IM_COLD, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_resistance[] =
{
    { "Ac", TR_RES_ACID, TR_IM_ACID },
    { "El", TR_RES_ELEC, TR_IM_ELEC },
    { "Fi", TR_RES_FIRE, TR_IM_FIRE },
    { "Co", TR_RES_COLD, TR_IM_COLD },
    { "Po", TR_RES_POIS, -1 },
    { "Li", TR_RES_LITE, -1 },
    { "Dk", TR_RES_DARK, -1 },
    { "Sh", TR_RES_SHARDS, -1 },
    { "Bl", TR_RES_BLIND, -1 },
    { "Cf", TR_RES_CONF, -1 },
    { "So", TR_RES_SOUND, -1 },
    { "Nt", TR_RES_NETHER, -1 },
    { "Nx", TR_RES_NEXUS, -1 },
    { "Ca", TR_RES_CHAOS, -1 },
    { "Di", TR_RES_DISEN, -1 },
    { "Fe", TR_RES_FEAR, -1 },
    { "Ti", TR_RES_TIME, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_vulnerability[] =
{
    { "Ac", TR_VULN_ACID, -1 },
    { "El", TR_VULN_ELEC, -1 },
    { "Fi", TR_VULN_FIRE, -1 },
    { "Co", TR_VULN_COLD, -1 },
    { "Po", TR_VULN_POIS, -1 },
    { "Li", TR_VULN_LITE, -1 },
    { "Dk", TR_VULN_DARK, -1 },
    { "Sh", TR_VULN_SHARDS, -1 },
    { "Bl", TR_VULN_BLIND, -1 },
    { "Cf", TR_VULN_CONF, -1 },
    { "So", TR_VULN_SOUND, -1 },
    { "Nt", TR_VULN_NETHER, -1 },
    { "Nx", TR_VULN_NEXUS, -1 },
    { "Ca", TR_VULN_CHAOS, -1 },
    { "Di", TR_VULN_DISEN, -1 },
    { "Fe", TR_VULN_FEAR, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_misc[] =
{
    { "Es", TR_EASY_SPELL, -1 },
    { "Dm", TR_DEC_MANA, -1 },
    { "Th", TR_THROW, -1 },
    { "Rf", TR_REFLECT, -1 },
    { "Fa", TR_FREE_ACT, -1 },
    { "Si", TR_SEE_INVIS, -1 },
    { "Hl", TR_HOLD_LIFE, -1 },
    { "Sd", TR_SLOW_DIGEST, -1 },
    { "Rg", TR_REGEN, -1 },
    { "Lv", TR_LEVITATION, -1 },
    { "Lu", TR_LITE, -1 },
    { "Wr", TR_WARNING, -1 },
    { "Xm", TR_XTRA_MIGHT, -1 },
    { "Xs", TR_XTRA_SHOTS, -1 },
    { "Te", TR_TELEPORT, -1 },
    { "Ag", TR_AGGRAVATE, -1 },
    { "Bs", TR_BLESSED, -1 },
    { "Ty", TR_TY_CURSE, -1 },
    { "Ds", TR_DARKNESS, -1 },
    { "Wm", TR_WEAPONMASTERY, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_aura[] =
{
    { "F", TR_SH_FIRE, -1 },
    { "E", TR_SH_ELEC, -1 },
    { "C", TR_SH_COLD, -1 },
    { "Sh", TR_SH_SHARDS, -1 },
    { "At", TR_SH_REVENGE, -1 },
    { "M", TR_NO_MAGIC, -1 },
    { "T", TR_NO_TELE, -1 },
    { "Sm", TR_NO_SUMMON, -1 },
    { "Mr", TR_MAGIC_RESISTANCE, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_brand[] =
{
    { "A", TR_BRAND_ACID, -1 },
    { "E", TR_BRAND_ELEC, -1 },
    { "F", TR_BRAND_FIRE, -1 },
    { "Co", TR_BRAND_COLD, -1 },
    { "P", TR_BRAND_POIS, -1 },
    { "Ca", TR_CHAOTIC, -1 },
    { "V", TR_VAMPIRIC, -1 },
    { "Q", TR_IMPACT, -1 },
    { "S", TR_VORPAL, -1 },
    { "S", TR_VORPAL2, -1 },
    { "M", TR_FORCE_WEAPON, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_kill[] =
{
    { "*", TR_KILL_EVIL, -1 },
    { "p", TR_KILL_HUMAN, -1 },
    { "D", TR_KILL_DRAGON, -1 },
    { "o", TR_KILL_ORC, -1 },
    { "T", TR_KILL_TROLL, -1 },
    { "P", TR_KILL_GIANT, -1 },
    { "U", TR_KILL_DEMON, -1 },
    { "L", TR_KILL_UNDEAD, -1 },
    { "Z", TR_KILL_ANIMAL, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_slay[] =
{
    { "*", TR_SLAY_EVIL, TR_KILL_EVIL },
    { "p", TR_SLAY_HUMAN, TR_KILL_HUMAN },
    { "D", TR_SLAY_DRAGON, TR_KILL_DRAGON },
    { "o", TR_SLAY_ORC, TR_KILL_ORC },
    { "T", TR_SLAY_TROLL, TR_KILL_TROLL },
    { "P", TR_SLAY_GIANT, TR_KILL_GIANT },
    { "U", TR_SLAY_DEMON, TR_KILL_DEMON },
    { "L", TR_SLAY_UNDEAD, TR_KILL_UNDEAD },
    { "Z", TR_SLAY_ANIMAL, TR_KILL_ANIMAL },
    { "A", TR_SLAY_GOOD, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp1[] =
{
    { "Tele", TR_TELEPATHY, -1 },
    { "Evil", TR_ESP_EVIL, -1 },
    { "Good", TR_ESP_GOOD, -1 },
    { "Nolv", TR_ESP_NONLIVING, -1 },
    { "Uniq", TR_ESP_UNIQUE, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_esp2[] =
{
    { "p", TR_ESP_HUMAN, -1 },
    { "D", TR_ESP_DRAGON, -1 },
    { "o", TR_ESP_ORC, -1 },
    { "T", TR_ESP_TROLL, -1 },
    { "P", TR_ESP_GIANT, -1 },
    { "U", TR_ESP_DEMON, -1 },
    { "L", TR_ESP_UNDEAD, -1 },
    { "Z", TR_ESP_ANIMAL, -1 },
    { NULL, 0, -1 }
};

static flag_insc_table flag_insc_sust[] =
{
    { "St", TR_SUST_STR, -1 },
    { "In", TR_SUST_INT, -1 },
    { "Wi", TR_SUST_WIS, -1 },
    { "Dx", TR_SUST_DEX, -1 },
    { "Cn", TR_SUST_CON, -1 },
    { "Ch", TR_SUST_CHR, -1 },
    { NULL, 0, -1 }
};

/* Simple macro for get_inscription() */
#define ADD_INSC(STR) (void)(ptr = object_desc_str(ptr, (STR)))

/*
 *  Helper function for get_inscription()
 */
static char *inscribe_flags_aux(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE], char *ptr)
{
    while (fi_ptr->english)
    {
        if (have_flag(flgs, fi_ptr->flag) &&
            (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
            ADD_INSC(fi_ptr->english);
        fi_ptr++;
    }

    return ptr;
}


/*
 *  Special variation of have_flag for auto-inscription
 */
static bool have_flag_of(flag_insc_table *fi_ptr, u32b flgs[TR_FLAG_SIZE])
{
    while (fi_ptr->english)
    {
        if (have_flag(flgs, fi_ptr->flag) &&
           (fi_ptr->except_flag == -1 || !have_flag(flgs, fi_ptr->except_flag)))
            return (TRUE);
        fi_ptr++;
    }

    return (FALSE);
}

static char *get_ability_abbreviation(char *ptr, object_type *o_ptr, bool all)
{
    char *prev_ptr = ptr;
    u32b flgs[TR_FLAG_SIZE];

    /* Extract the flags */
    object_flags(o_ptr, flgs);


    /* Remove obvious flags */
    if (!all)
    {
        object_kind *k_ptr = &k_info[o_ptr->k_idx];
        int j;
                
        /* Base object */
        for (j = 0; j < TR_FLAG_SIZE; j++)
            flgs[j] &= ~k_ptr->flags[j];

        if (object_is_fixed_artifact(o_ptr))
        {
            artifact_type *a_ptr = &a_info[o_ptr->name1];
                    
            for (j = 0; j < TR_FLAG_SIZE; j++)
                flgs[j] &= ~a_ptr->flags[j];
        }

        if (object_is_ego(o_ptr))
        {
            ego_item_type *e_ptr = &e_info[o_ptr->name2];
                    
            for (j = 0; j < TR_FLAG_SIZE; j++)
                flgs[j] &= ~e_ptr->flags[j];
        }
    }


    /* Plusses */
    ptr = inscribe_flags_aux(flag_insc_plus, flgs, ptr);

    /* Minusses */
    if (have_flag_of(flag_insc_minus, flgs))
    {
        if (ptr != prev_ptr)
        {
            ADD_INSC(";");
            prev_ptr = ptr;
        }
        ADD_INSC("-");
    }
    ptr = inscribe_flags_aux(flag_insc_minus, flgs, ptr);

    /* Immunity */
    if (have_flag_of(flag_insc_immune, flgs))
    {
        if (ptr != prev_ptr)
        {
            ADD_INSC(";");
            prev_ptr = ptr;
        }
        ADD_INSC("*");
    }
    ptr = inscribe_flags_aux(flag_insc_immune, flgs, ptr);

    /* Vulnerability */
    if (have_flag_of(flag_insc_vulnerability, flgs))
    {
        if (ptr != prev_ptr)
        {
            ADD_INSC(";");
            prev_ptr = ptr;
        }
        ADD_INSC("-");
    }
    ptr = inscribe_flags_aux(flag_insc_vulnerability, flgs, ptr);

    /* Resistance */
    if (have_flag_of(flag_insc_resistance, flgs))
    {
        if (ptr != prev_ptr)
        {
            ADD_INSC(";");
            prev_ptr = ptr;
        }
    }
    ptr = inscribe_flags_aux(flag_insc_resistance, flgs, ptr);

    /* Misc Ability */
    if (have_flag_of(flag_insc_misc, flgs))
    {
        if (ptr != prev_ptr)
        {
            ADD_INSC(";");
            prev_ptr = ptr;
        }
    }
    ptr = inscribe_flags_aux(flag_insc_misc, flgs, ptr);

    /* Aura */
    if (have_flag_of(flag_insc_aura, flgs))
    {
        ADD_INSC("[");
    }
    ptr = inscribe_flags_aux(flag_insc_aura, flgs, ptr);

    /* Brand Weapon */
    if (have_flag_of(flag_insc_brand, flgs))
        ADD_INSC("|");
    ptr = inscribe_flags_aux(flag_insc_brand, flgs, ptr);

    /* Kill Weapon */
    if (have_flag_of(flag_insc_kill, flgs))
        ADD_INSC("/X");
    ptr = inscribe_flags_aux(flag_insc_kill, flgs, ptr);

    /* Slay Weapon */
    if (have_flag_of(flag_insc_slay, flgs))
        ADD_INSC("/");
    ptr = inscribe_flags_aux(flag_insc_slay, flgs, ptr);

    /* Esp */
    if (have_flag_of(flag_insc_esp1, flgs))
        ADD_INSC("~");
    ptr = inscribe_flags_aux(flag_insc_esp1, flgs, ptr);
    if (have_flag_of(flag_insc_esp2, flgs))
        ADD_INSC("~");
    ptr = inscribe_flags_aux(flag_insc_esp2, flgs, ptr);

    /* sustain */
    if (have_flag_of(flag_insc_sust, flgs))
    {
        ADD_INSC("(");
    }
    ptr = inscribe_flags_aux(flag_insc_sust, flgs, ptr);

    *ptr = '\0';

    return ptr;
}


/*
 *  Get object inscription with auto inscription of object flags.
 */
static void get_inscription(char *buff, object_type *o_ptr)
{
    cptr insc = quark_str(o_ptr->inscription);
    char *ptr = buff;

    /* Not fully identified */
    if (!(o_ptr->ident & IDENT_MENTAL))
    {
        /* Copy until end of line or '#' */
        while (*insc)
        {
            if (*insc == '#') break;
            *buff++ = *insc++;
        }

        *buff = '\0';
        return;
    }

    *buff = '\0';
    for (; *insc; insc++)
    {
        /* Ignore fake artifact inscription */
        if (*insc == '#') break;

        /* {%} will be automatically converted */
        else if ('%' == *insc)
        {
            bool all;
            cptr start = ptr;

            /* check for too long inscription */
            if (ptr >= buff + MAX_NLEN) continue;

            if ('a' == insc[1] && 'l' == insc[2] && 'l' == insc[3])
            {
                all = TRUE;
                insc += 3;
            }
            else
            {
                all = FALSE;
            }

            ptr = get_ability_abbreviation(ptr, o_ptr, all);

            if (ptr == start)
                ADD_INSC(" ");
        }
        else
        {
            *ptr++ = *insc;
        }
    }
    *ptr = '\0';
}


/*
 * Creates a description of the item "o_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be MAX_NLEN chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under MAX_NLEN-1 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "k_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * Mode:
 *   OD_NAME_ONLY        : The Cloak of Death
 *   OD_NAME_AND_ENCHANT : The Cloak of Death [1,+3]
 *   OD_OMIT_INSCRIPTION : The Cloak of Death [1,+3] (+2 to Stealth)
 *   0                   : The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 *
 *   OD_OMIT_PREFIX      : Forbidden numeric prefix
 *   OD_NO_PLURAL        : Forbidden use of plural 
 *   OD_STORE            : Assume to be aware and known
 *   OD_NO_FLAVOR        : Allow to hidden flavor
 *   OD_FORCE_FLAVOR     : Get un-shuffled flavor name
 */
void object_desc(char *buf, object_type *o_ptr, u32b mode)
{
    /* Extract object kind name */
    cptr            kindname = k_name + k_info[o_ptr->k_idx].name;

    /* Extract default "base" string */
    cptr            basenm = kindname;

    /* Assume no "modifier" string */
    cptr            modstr = "";

    int             power;

    bool            aware = FALSE;
    bool            known = FALSE;
    bool            flavor = TRUE;

    bool            show_weapon = FALSE;
    bool            show_armour = FALSE;

    cptr            s, s0;
    char            *t;

    char            p1 = '(', p2 = ')';
    char            b1 = '[', b2 = ']';
    char            c1 = '{', c2 = '}';

    char            tmp_val[MAX_NLEN+160];
    char            tmp_val2[MAX_NLEN+10];
    char            fake_insc_buf[30];

    u32b flgs[TR_FLAG_SIZE];

    int          slot;
    object_type *bow_ptr = NULL;

    object_kind *k_ptr = &k_info[o_ptr->k_idx];
    object_kind *flavor_k_ptr = &k_info[k_ptr->flavor];

    /* Extract some flags */
    object_flags(o_ptr, flgs);

    /* See if the object is "aware" */
    if (object_is_aware(o_ptr)) aware = TRUE;

    /* Hack object_is_aware() is wrong in this case.
       Anybody know how k_info[].aware gets set?  Perhaps flavor_init? */
    if (o_ptr->name1 == ART_HAND_OF_VECNA || o_ptr->name1 == ART_EYE_OF_VECNA)
        aware = FALSE;

    /* See if the object is "known" */
    if (object_is_known(o_ptr)) known = TRUE;

    /* Allow flavors to be hidden when aware */
    if (aware && ((mode & OD_NO_FLAVOR) || plain_descriptions)) flavor = FALSE;

    /* Object is in the inventory of a store or spoiler */
    if ((mode & OD_STORE) || (o_ptr->ident & IDENT_STORE))
    {
        /* Don't show flavors */
        flavor = FALSE;

        /* Pretend known and aware */
        aware = TRUE;
        known = TRUE;
    }

    /* Force to be flavor name only */
    if (mode & OD_FORCE_FLAVOR)
    {
        aware = FALSE;
        flavor = TRUE;
        known = FALSE;

        /* Cancel shuffling */
        flavor_k_ptr = k_ptr;
    }

    /* Analyze the object */
    switch (o_ptr->tval)
    {
        /* Some objects are easy to describe */
        case TV_SKELETON:
        case TV_BOTTLE:
        case TV_JUNK:
        case TV_SPIKE:
        case TV_FLASK:
        case TV_CHEST:
        case TV_WHISTLE:
        {
            break;
        }

        case TV_CAPTURE:
        {
            monster_race *r_ptr = &r_info[o_ptr->pval];

            if (known)
            {
                if (!o_ptr->pval)
                {
                    modstr = " (empty)";
                }
                else
                {
                    cptr t = r_name + r_ptr->name;

                    if (!(r_ptr->flags1 & RF1_UNIQUE))
                    {
                        sprintf(tmp_val2, " (%s%s)", (is_a_vowel(*t) ? "an " : "a "), t);

                        modstr = tmp_val2;
                    }
                    else
                    {
                        sprintf(tmp_val2, "(%s)", t);

                        modstr = t;
                    }
                }
            }
            break;
        }

        /* Figurines/Statues */
        case TV_FIGURINE:
        case TV_STATUE:
        {
            monster_race *r_ptr = &r_info[o_ptr->pval];

            cptr t = r_name + r_ptr->name;

            if (!(r_ptr->flags1 & RF1_UNIQUE))
            {
                sprintf(tmp_val2, "%s%s", (is_a_vowel(*t) ? "an " : "a "), t);

                modstr = tmp_val2;
            }
            else
            {
                modstr = t;
            }
            break;
        }

        /* Corpses */
        case TV_CORPSE:
        {
            monster_race *r_ptr = &r_info[o_ptr->pval];

            modstr = r_name + r_ptr->name;

            if (r_ptr->flags1 & RF1_UNIQUE)
                basenm = "& % of #";
            else
                basenm = "& # %";
            break;
        }

        /* Missiles/ Bows/ Weapons */
        case TV_SHOT:
        case TV_BOLT:
        case TV_ARROW:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_DIGGING:
        {
            show_weapon = TRUE;
            break;
        }

        case TV_BOW:
        {
            if (o_ptr->sval != SV_HARP && o_ptr->sval != SV_CRIMSON && o_ptr->sval != SV_RAILGUN)
                show_weapon = TRUE;

            break;
        }

        /* Armour */
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_CROWN:
        case TV_HELM:
        case TV_SHIELD:
        case TV_SOFT_ARMOR:
        case TV_HARD_ARMOR:
        case TV_DRAG_ARMOR:
        {
            if (o_ptr->name1 == ART_HAND_OF_VECNA)
            {
                modstr = k_name + flavor_k_ptr->flavor_name;
                if (!flavor)    basenm = "& Hand~ of %";
                else if (aware) break;
                else            basenm = "& # Hand~";
            }
            else
                show_armour = TRUE;
            break;
        }

        /* Lites (including a few "Specials") */
        case TV_LITE:
        {
            if (o_ptr->name1 == ART_EYE_OF_VECNA)
            {
                modstr = k_name + flavor_k_ptr->flavor_name;
                if (!flavor)    basenm = "& Eye~ of %";
                else if (aware) break;
                else            basenm = "& # Eye~";
            }
            break;
        }

        case TV_AMULET:
        case TV_RING:
            if (o_ptr->to_h || o_ptr->to_d)
                show_weapon = TRUE;
            break;

        case TV_CARD:
        {
            break;
        }

        case TV_RUNE:
        {
            break;
        }

        case TV_STAFF:
        {
            /* Color the object */
            modstr = k_name + flavor_k_ptr->flavor_name;

            if (!flavor)    basenm = "& Staff~ of %";
            else if (aware) basenm = "& # Staff~ of %";
            else            basenm = "& # Staff~";

            break;
        }

        case TV_WAND:
        {
            /* Color the object */
            modstr = k_name + flavor_k_ptr->flavor_name;

            if (!flavor)    basenm = "& Wand~ of %";
            else if (aware) basenm = "& # Wand~ of %";
            else            basenm = "& # Wand~";

            break;
        }

        case TV_ROD:
        {
            /* Color the object */
            modstr = k_name + flavor_k_ptr->flavor_name;

            if (!flavor)    basenm = "& Rod~ of %";
            else if (aware) basenm = "& # Rod~ of %";
            else            basenm = "& # Rod~";
            break;
        }

        case TV_SCROLL:
        {
            /* Color the object */
            modstr = k_name + flavor_k_ptr->flavor_name;

            if (!flavor)    basenm = "& Scroll~ of %";
            else if (aware) basenm = "& Scroll~ titled \"#\" of %";
            else            basenm = "& Scroll~ titled \"#\"";

            break;
        }

        case TV_POTION:
        {
            /* Color the object */
            modstr = k_name + flavor_k_ptr->flavor_name;

            if (!flavor)    basenm = "& Potion~ of %";
            else if (aware) basenm = "& # Potion~ of %";
            else            basenm = "& # Potion~";
            break;
        }

        case TV_FOOD:
        {
            /* Ordinary food is "boring" */
            if (!k_ptr->flavor_name) break;

            /* Color the object */
            modstr = k_name + flavor_k_ptr->flavor_name;

            if (!flavor)    basenm = "& Mushroom~ of %";
            else if (aware) basenm = "& # Mushroom~ of %";
            else            basenm = "& # Mushroom~";
            break;
        }

        case TV_PARCHMENT:
        {
            basenm = "& Parchment~ - %";
            break;
        }

        /* Magic Books */
        case TV_LIFE_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Life Magic %";
            else
                basenm = "& Life Spellbook~ %";
            break;
        }

        case TV_SORCERY_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Sorcery %";
            else
                basenm = "& Sorcery Spellbook~ %";
            break;
        }

        case TV_NATURE_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Nature Magic %";
            else
                basenm = "& Nature Spellbook~ %";
            break;
        }

        case TV_CHAOS_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Chaos Magic %";
            else
                basenm = "& Chaos Spellbook~ %";
            break;
        }

        case TV_DEATH_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Death Magic %";
            else
                basenm = "& Death Spellbook~ %";
            break;
        }

        case TV_TRUMP_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Trump Magic %";
            else
                basenm = "& Trump Spellbook~ %";
            break;
        }

        case TV_ARCANE_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Arcane Magic %";
            else
                basenm = "& Arcane Spellbook~ %";
            break;
        }

        case TV_CRAFT_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Craft Magic %";
            else
                basenm = "& Craft Spellbook~ %";
            break;
        }

        case TV_DAEMON_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Daemon Magic %";
            else
                basenm = "& Daemon Spellbook~ %";
            break;
        }

        case TV_CRUSADE_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Crusade Magic %";
            else
                basenm = "& Crusade Spellbook~ %";
            break;
        }

        case TV_NECROMANCY_BOOK:
        {
            basenm = "& Necromancy Spellbook~ %";
            break;
        }

        case TV_RAGE_BOOK:
        {
            basenm = "& Rage Spellbook~ %";
            break;
        }

        case TV_BURGLARY_BOOK:
        {
            basenm = "& Thieves' Guide~ %";
            break;
        }

        case TV_ARMAGEDDON_BOOK:
            basenm = "& Armageddon Spellbook~ %";
            break;

        case TV_MUSIC_BOOK:
        {
            basenm = "& Song Book~ %";
            break;
        }

        case TV_HISSATSU_BOOK:
        {
            basenm = "Book~ of Kendo %";
            break;
        }

        case TV_HEX_BOOK:
        {
            if (mp_ptr->spell_book == TV_LIFE_BOOK)
                basenm = "& Book~ of Hex Magic %";
            else
                basenm = "& Hex Spellbook~ %";
            break;
        }

        /* Hack -- Gold/Gems */
        case TV_GOLD:
        {
            strcpy(buf, basenm);
            return;
        }

        /* Used in the "inventory" routine */
        default:
        {
            strcpy(buf, "(nothing)");
            return;
        }
    }

    /* Use full name from k_info or a_info */
    if (aware && have_flag(flgs, TR_FULL_NAME))
    {
        if (known && o_ptr->name1) basenm = a_name + a_info[o_ptr->name1].name;
        else if (known && o_ptr->name2) basenm = e_name + e_info[o_ptr->name2].name;
        else basenm = kindname;
    }

    /* Start dumping the result */
    t = tmp_val;

    if (o_ptr->marked & OM_RESERVED)
        t = object_desc_str(t, "<<Hold>> ");

    /* The object "expects" a "number" */
    if (basenm[0] == '&')
    {
        /* Skip the ampersand (and space) */
        s = basenm + 2;

        /* No prefix */
        if (mode & OD_OMIT_PREFIX)
        {
            /* Nothing */
        }

        /* Hack -- None left */
        else if (o_ptr->number <= 0)
        {
            t = object_desc_str(t, "no more ");
        }

        /* Extract the number */
        else if (o_ptr->number > 1)
        {
            t = object_desc_num(t, o_ptr->number);
            t = object_desc_chr(t, ' ');
        }

        /* Hack -- The only one of its kind */
        else if ((known && object_is_artifact(o_ptr)) ||
                 ((o_ptr->tval == TV_CORPSE) &&
                  (r_info[o_ptr->pval].flags1 & RF1_UNIQUE)))
        {
            t = object_desc_str(t, "The ");
        }

        /* A single one */
        else
        {
            bool vowel;

            switch (*s)
            {
            case '#': vowel = is_a_vowel(modstr[0]); break;
            case '%': vowel = is_a_vowel(*kindname); break;
            default:  vowel = is_a_vowel(*s); break;
            }

            if (vowel)
            {
                /* A single one, with a vowel */
                t = object_desc_str(t, "an ");
            }
            else
            {
                /* A single one, without a vowel */
                t = object_desc_str(t, "a ");
            }
        }
    }

    /* Hack -- objects that "never" take an article */
    else
    {
        /* No ampersand */
        s = basenm;

        /* No pref */
        if (mode & OD_OMIT_PREFIX)
        {
            /* Nothing */
        }

        /* Hack -- all gone */
        else if (o_ptr->number <= 0)
        {
            t = object_desc_str(t, "no more ");
        }

        /* Prefix a number if required */
        else if (o_ptr->number > 1)
        {
            t = object_desc_num(t, o_ptr->number);
            t = object_desc_chr(t, ' ');
        }

        /* Hack -- The only one of its kind */
        else if (known && object_is_artifact(o_ptr))
        {
            t = object_desc_str(t, "The ");
        }

        /* Hack -- single items get no prefix */
        else
        {
            /* Nothing */
        }
    }

    /* Copy the string */
    for (s0 = NULL; *s || s0; )
    {
        /* The end of the flavour/kind string. */
        if (!*s)
        {
            s = s0 + 1;
            s0 = NULL;
        }

        /* Begin to append the modifier (flavor) */
        else if ((*s == '#') && !s0)
        {
            s0 = s;
            s = modstr;

            /* Paranoia -- Never append multiple modstrs */
            modstr = "";
        }

        /* Begin to append the kind name */
        else if ((*s == '%') && !s0)
        {
            s0 = s;
            s = kindname;

            /* Paranoia -- Never append multiple kindnames */
            kindname = "";
        }

        /* Pluralizer */
        else if (*s == '~')
        {
            /* Add a plural if needed */
            if (!(mode & OD_NO_PLURAL) && (o_ptr->number != 1))
            {
                char k = t[-1];

                /* XXX XXX XXX Mega-Hack */

                /* Hack -- "Cutlass-es" and "Torch-es" */
                if ((k == 's') || (k == 'h')) *t++ = 'e';

                /* Add an 's' */
                *t++ = 's';
            }
            s++;
        }
        /* Normal */
        else
        {
            /* Copy */
            *t++ = *s++;
        }
    }

    /* Terminate */
    *t = '\0';


    if (object_is_smith(o_ptr))
    {
        t = object_desc_str(t,format(" of %s the Smith",player_name));
    }

    /* Hack -- Append "Artifact" or "Special" names */
    if (known && !have_flag(flgs, TR_FULL_NAME))
    {
        /* Is it a new random artifact ? */
        if (o_ptr->art_name)
        {
            t = object_desc_chr(t, ' ');
            t = object_desc_str(t, quark_str(o_ptr->art_name));
        }

        /* Grab any artifact name */
        else if (object_is_fixed_artifact(o_ptr))
        {
            artifact_type *a_ptr = &a_info[o_ptr->name1];

            t = object_desc_chr(t, ' ');
            t = object_desc_str(t, a_name + a_ptr->name);
        }

        /* Grab any ego-item name */
        else
        {
            if (object_is_ego(o_ptr))
            {
                ego_item_type *e_ptr = &e_info[o_ptr->name2];

                t = object_desc_chr(t, ' ');
                t = object_desc_str(t, e_name + e_ptr->name);
            }

            if (o_ptr->inscription && my_strchr(quark_str(o_ptr->inscription), '#'))
            {
                /* Find the '#' */
                cptr str = my_strchr(quark_str(o_ptr->inscription), '#');

                /* Add the false name */
                t = object_desc_chr(t, ' ');
                t = object_desc_str(t, &str[1]);
            }
        }
    }

    /* No more details wanted */
    if (mode & OD_NAME_ONLY) goto object_desc_done;

    /* Hack -- Chests must be described in detail */
    if (o_ptr->tval == TV_CHEST)
    {
        /* Not searched yet */
        if (!known)
        {
            /* Nothing */
        }

        /* May be "empty" */
        else if (!o_ptr->pval)
        {
            t = object_desc_str(t, " (empty)");
        }

        /* May be "disarmed" */
        else if (o_ptr->pval < 0)
        {
            if (chest_traps[0 - o_ptr->pval])
            {
                t = object_desc_str(t, " (disarmed)");
            }
            else
            {
                t = object_desc_str(t, " (unlocked)");
            }
        }

        /* Describe the traps, if any */
        else
        {
            /* Describe the traps */
            switch (chest_traps[o_ptr->pval])
            {
                case 0:
                {
                    t = object_desc_str(t, " (Locked)");
                    break;
                }
                case CHEST_LOSE_STR:
                {
                    t = object_desc_str(t, " (Poison Needle)");
                    break;
                }
                case CHEST_LOSE_CON:
                {
                    t = object_desc_str(t, " (Poison Needle)");
                    break;
                }
                case CHEST_POISON:
                {
                    t = object_desc_str(t, " (Gas Trap)");
                    break;
                }
                case CHEST_PARALYZE:
                {
                    t = object_desc_str(t, " (Gas Trap)");
                    break;
                }
                case CHEST_EXPLODE:
                {
                    t = object_desc_str(t, " (Explosion Device)");
                    break;
                }
                case CHEST_SUMMON:
                case CHEST_BIRD_STORM:
                case CHEST_E_SUMMON:
                case CHEST_H_SUMMON:
                {
                    t = object_desc_str(t, " (Summoning Runes)");
                    break;
                }
                case CHEST_RUNES_OF_EVIL:
                {
                    t = object_desc_str(t, " (Gleaming Black Runes)");
                    break;
                }
                case CHEST_ALARM:
                {
                    t = object_desc_str(t, " (Alarm)");
                    break;
                }
                default:
                {
                    t = object_desc_str(t, " (Multiple Traps)");
                    break;
                }
            }
        }
    }


    /* Display the item like a weapon */
    if (have_flag(flgs, TR_SHOW_MODS)) show_weapon = TRUE;

    /* Display the item like a weapon */
    if (object_is_smith(o_ptr) && (o_ptr->xtra3 == 1 + ESSENCE_SLAY_GLOVE))
        show_weapon = TRUE;

    /* Display the item like a weapon */
    if (o_ptr->to_h && o_ptr->to_d) show_weapon = TRUE;

    /* Display the item like armour */
    if (o_ptr->ac) show_armour = TRUE;


    /* Dump base weapon info */
    switch (o_ptr->tval)
    {
    /* Missiles and Weapons */
    case TV_SHOT:
    case TV_BOLT:
    case TV_ARROW:
    case TV_HAFTED:
    case TV_POLEARM:
    case TV_SWORD:
    case TV_DIGGING:
    {
        int hand = equip_which_hand(o_ptr);
        int dd = o_ptr->dd;
        int ds = o_ptr->ds;

        if (p_ptr->big_shot && o_ptr->tval == p_ptr->shooter_info.tval_ammo)
            dd *= 2;

        if (hand >= 0 && hand < MAX_HANDS)
        {
            dd += p_ptr->weapon_info[hand].to_dd;
            ds += p_ptr->weapon_info[hand].to_ds;
        }

        /* Append a "damage" string */
        t = object_desc_chr(t, ' ');
        t = object_desc_chr(t, p1);
        t = object_desc_num(t, dd);
        t = object_desc_chr(t, 'd');
        t = object_desc_num(t, ds);
        t = object_desc_chr(t, p2);

        /* All done */
        break;
    }
    /* Bows get a special "damage string" */
    case TV_BOW:
        if (o_ptr->sval == SV_HARP) break;
        if (o_ptr->sval == SV_CRIMSON) break;
        if (o_ptr->sval == SV_RAILGUN) break;

        /* Mega-Hack -- Extract the "base power" */
        power = bow_tmul(o_ptr->sval);

        /* Are we describing a wielded bow? */
        if (equip_is_worn(o_ptr))
            power += p_ptr->shooter_info.to_mult;
        else if (have_flag(flgs, TR_XTRA_MIGHT))
            power += 1;

        /* Append a special "damage" string */
        t = object_desc_chr(t, ' ');
        t = object_desc_chr(t, p1);
        t = object_desc_chr(t, 'x');
        t = object_desc_num(t, power);
        t = object_desc_chr(t, p2);

        /* All done */
        break;
    }

    if (mode & OD_NAME_AND_DICE) goto object_desc_done;

    /* Add the weapon bonuses */
    if (known)
    {
        if (o_ptr->tval == TV_BOW && (o_ptr->sval == SV_HARP || o_ptr->sval == SV_CRIMSON || o_ptr->sval == SV_RAILGUN))
        {
        }
        /* Show the tohit/todam on request */
        else if (show_weapon)
        {
            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, p1);
            t = object_desc_int(t, o_ptr->to_h);
            t = object_desc_chr(t, ',');
            t = object_desc_int(t, o_ptr->to_d);
            t = object_desc_chr(t, p2);
        }

        /* Show the tohit if needed */
        else if (o_ptr->to_h)
        {
            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, p1);
            t = object_desc_int(t, o_ptr->to_h);
            t = object_desc_chr(t, p2);
        }

        /* Show the todam if needed */
        else if (o_ptr->to_d)
        {
            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, p1);
            t = object_desc_int(t, o_ptr->to_d);
            t = object_desc_chr(t, p2);
        }
    }

    slot = equip_find_object(TV_BOW, SV_ANY);
    if (slot)
        bow_ptr = equip_obj(slot);

    /* If have a firing weapon + ammo matches bow */
    if (bow_ptr && o_ptr->tval == p_ptr->shooter_info.tval_ammo)
    {
        int avgdam;        
        int tmul = bow_tmul(bow_ptr->sval);
        s16b energy_fire = bow_energy(bow_ptr->sval);

        if (p_ptr->big_shot && o_ptr->tval == p_ptr->shooter_info.tval_ammo)
            avgdam = o_ptr->dd * 2 * (o_ptr->ds + 1) * 10 / 2;
        else
            avgdam = o_ptr->dd * (o_ptr->ds + 1) * 10 / 2;

        /* See if the bow is "known" - then set damage bonus */
        if (object_is_known(bow_ptr)) avgdam += (bow_ptr->to_d * 10);
        if (weaponmaster_is_(WEAPONMASTER_CROSSBOWS) && p_ptr->lev >= 15)
            avgdam += (1 + p_ptr->lev/10)*10;

        /* Effect of ammo */
        if (known) avgdam += (o_ptr->to_d * 10);

        /* Get extra "power" from "extra might" */
        tmul += p_ptr->shooter_info.to_mult;

        tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

        /* Launcher multiplier */
        avgdam *= tmul;
        avgdam /= (100 * 10);

        /* Get extra damage from concentration */
        if (p_ptr->concent) avgdam = boost_concentration_damage(avgdam);

        /* Testing */
        avgdam += p_ptr->shooter_info.to_d;

        if (avgdam < 0) avgdam = 0;

        /* Display (shot damage/ avg damage) */
        t = object_desc_chr(t, ' ');
        t = object_desc_chr(t, p1);
        t = object_desc_num(t, avgdam);
        t = object_desc_chr(t, '/');

        if (p_ptr->shooter_info.num_fire == 0)
        {
            t = object_desc_chr(t, '0');
        }
        else
        {
            /* Calc effects of energy */
            avgdam *= (p_ptr->shooter_info.num_fire * 100);
            avgdam /= energy_fire;
            t = object_desc_num(t, avgdam);
        }

        t = object_desc_chr(t, p2);
    }
    else if ((p_ptr->pclass == CLASS_NINJA) && (o_ptr->tval == TV_SPIKE))
    {
        int avgdam = p_ptr->mighty_throw ? (1 + 3) : 1;
        s16b energy_fire = 100 - p_ptr->lev;

        avgdam += ((p_ptr->lev + 30) * (p_ptr->lev + 30) - 900) / 55;

        /* Display (shot damage/ avg damage) */
        t = object_desc_chr(t, ' ');
        t = object_desc_chr(t, p1);
        t = object_desc_num(t, avgdam);
        t = object_desc_chr(t, '/');

        /* Calc effects of energy */
        avgdam = 100 * avgdam / energy_fire;

        t = object_desc_num(t, avgdam);
        t = object_desc_chr(t, p2);
    }

    /* Add the armor bonuses */
    if (known)
    {
        /* Show the armor class info */
        if (show_armour)
        {
            int ac = o_ptr->ac;
            int to_a = o_ptr->to_a;

            if (prace_is_(RACE_CENTAUR) && object_is_body_armour(o_ptr))
            {
                ac -= ac / 3;
                to_a -= to_a / 3;
            }

            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, b1);
            t = object_desc_num(t, ac);
            t = object_desc_chr(t, ',');
            t = object_desc_int(t, to_a);
            t = object_desc_chr(t, b2);
        }

        /* No base armor, but does increase armor */
        else if (o_ptr->to_a)
        {
            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, b1);
            t = object_desc_int(t, o_ptr->to_a);
            t = object_desc_chr(t, b2);
        }
    }

    /* Hack -- always show base armor */
    else if (show_armour)
    {
        t = object_desc_chr(t, ' ');
        t = object_desc_chr(t, b1);
        t = object_desc_num(t, o_ptr->ac);
        t = object_desc_chr(t, b2);
    }

    if (o_ptr->rune)
    {
        t = object_desc_chr(t, ' ');
        t = object_desc_str(t, rune_desc(o_ptr->rune));
    }

    /* No more details wanted */
    if (mode & OD_NAME_AND_ENCHANT) goto object_desc_done;


    if (known) /* Known item only */
    {
        /*
         * Hack -- Wands and Staffs have charges.  Make certain how many charges
         * a stack of staffs really has is clear. -LM-
         */
        if (((o_ptr->tval == TV_STAFF) || (o_ptr->tval == TV_WAND)))
        {
            /* Dump " (N charges)" */
            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, p1);

            /* Clear explaination for staffs. */
            if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
            {
                t = object_desc_num(t, o_ptr->number);
                t = object_desc_str(t, "x ");
            }
            t = object_desc_num(t, o_ptr->pval);
            t = object_desc_str(t, " charge");
            if (o_ptr->pval != 1) t = object_desc_chr(t, 's');

            t = object_desc_chr(t, p2);
        }
        /* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
         * be in any state of charge or discharge, this now includes a number. -LM-
         */
        else if (o_ptr->tval == TV_ROD)
        {
            /* Hack -- Dump " (# charging)" if relevant */
            if (o_ptr->timeout)
            {
                /* Stacks of rods display an exact count of charging rods. */
                if (o_ptr->number > 1)
                {
                    /* Paranoia. */
                    if (k_ptr->pval == 0) k_ptr->pval = 1;

                    /* Find out how many rods are charging, by dividing
                     * current timeout by each rod's maximum timeout.
                     * Ensure that any remainder is rounded up.  Display
                     * very discharged stacks as merely fully discharged.
                     */
                    power = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
                    if (power > o_ptr->number) power = o_ptr->number;

                    /* Display prettily. */
                    t = object_desc_str(t, " (");
                    t = object_desc_num(t, power);
                    t = object_desc_str(t, " charging)");
                }

                /* "one Rod of Perception (1 charging)" would look tacky. */
                else
                {
                    t = object_desc_str(t, " (charging)");
                }
            }
        }

        /* Dump "pval" flags for wearable items */
        if (have_pval_flags(flgs) && o_ptr->pval)
        {
            /* Start the display */
            t = object_desc_chr(t, ' ');
            t = object_desc_chr(t, p1);

            /* Dump the "pval" itself */
            t = object_desc_int(t, o_ptr->pval);

            /* Do not display the "pval" flags */
            if (have_flag(flgs, TR_HIDE_TYPE))
            {
                /* Nothing */
            }

            /* Speed */
            else if (have_flag(flgs, TR_SPEED))
            {
                /* Dump " to speed" */
                t = object_desc_str(t, " to speed");
            }

            /* Attack speed */
            else if (have_flag(flgs, TR_BLOWS))
            {
                /* Add " attack" */
                t = object_desc_str(t, " attack");

                /* Add "attacks" */
                if (ABS(o_ptr->pval) != 1) t = object_desc_chr(t, 's');
            }

            /* Stealth */
            else if (have_flag(flgs, TR_STEALTH))
            {
                /* Dump " to stealth" */
                t = object_desc_str(t, " to stealth");
            }

            /* Search */
            else if (have_flag(flgs, TR_SEARCH))
            {
                /* Dump " to searching" */
                t = object_desc_str(t, " to searching");
            }

            /* Infravision */
            else if (have_flag(flgs, TR_INFRA))
            {
                /* Dump " to infravision" */
                t = object_desc_str(t, " to infravision");
            }

            /* Finish the display */
            t = object_desc_chr(t, p2);
        }

        if (have_flag(flgs, TR_MAGIC_MASTERY))
        {
            int pct = device_power_aux(100, o_ptr->pval) - 100;
            if (pct >= 0)
                t = object_desc_str(t, format(" <+%d%%>", pct));
            else
                t = object_desc_str(t, format(" <%d%%>", pct));
        }
        else if (have_flag(flgs, TR_DEC_MAGIC_MASTERY))
        {
            int pct = device_power_aux(100, -o_ptr->pval) - 100;
            t = object_desc_str(t, format(" <%d%%>", pct));
        }

        if (have_flag(flgs, TR_SPELL_POWER))
        {
            int pct = spell_power_aux(100, o_ptr->pval) - 100;
            t = object_desc_str(t, format(" <+%d%%>", pct));
        }
        else if (have_flag(flgs, TR_DEC_SPELL_POWER))
        {
            int pct = spell_power_aux(100, -o_ptr->pval) - 100;
            t = object_desc_str(t, format(" <%d%%>", pct));
        }

        if (have_flag(flgs, TR_SPELL_CAP))
        {
            int pct = spell_cap_aux(100, o_ptr->pval) - 100;
            if (pct > 0)
                t = object_desc_str(t, format(" [+%d%%]", pct));
            else
                t = object_desc_str(t, format(" [%d%%]", pct));
        }
        else if (have_flag(flgs, TR_DEC_SPELL_CAP))
        {
            int pct = spell_cap_aux(100, -o_ptr->pval) - 100;
            t = object_desc_str(t, format(" [%d%%]", pct));
        }

        /* Hack -- Process Lanterns/Torches */
        if ((o_ptr->tval == TV_LITE) && (!(o_ptr->name1 || o_ptr->art_name || (o_ptr->sval == SV_LITE_FEANOR))))
        {
            /* Hack -- Turns of light for normal lites */
            t = object_desc_str(t, " (with ");

            if (o_ptr->name2 == EGO_LITE_DURATION) t = object_desc_num(t, o_ptr->xtra4 * 2);
            else t = object_desc_num(t, o_ptr->xtra4);
            t = object_desc_str(t, " turns of light)");
        }

        /* Indicate charging objects, but not rods. */
        if (o_ptr->timeout && (o_ptr->tval != TV_ROD))
        {
            t = object_desc_str(t, " (charging)");
        }
    }


    /* No more details wanted */
    if (mode & OD_OMIT_INSCRIPTION) goto object_desc_done;


    /* Prepare real inscriptions in a buffer */
    tmp_val2[0] = '\0';

    /* Auto abbreviation inscribe */
    if ((abbrev_extra || abbrev_all) && (o_ptr->ident & IDENT_MENTAL))
    {
        if (!o_ptr->inscription || !my_strchr(quark_str(o_ptr->inscription), '%'))
        {
            bool all;

            all = abbrev_all;
            if ((o_ptr->tval == TV_RING || o_ptr->tval == TV_AMULET) && o_ptr->art_name)
                all = TRUE;
            get_ability_abbreviation(tmp_val2, o_ptr, all);
        }
    }

    /* Use the standard inscription if available */
    if (o_ptr->inscription)
    {
        char buff[1024];

        if (tmp_val2[0]) strcat(tmp_val2, ", ");

        /* Get inscription and convert {%} */
        get_inscription(buff, o_ptr);

        /* strcat with correct treating of kanji */
        my_strcat(tmp_val2, buff, sizeof(tmp_val2));
    }


    /* No fake inscription yet */
    fake_insc_buf[0] = '\0';

    /* Use the game-generated "feeling" otherwise, if available */
    if (o_ptr->feeling)
    {
        strcpy(fake_insc_buf, game_inscriptions[o_ptr->feeling]);
    }

    /* Note "cursed" if the item is known to be cursed */
    else if (object_is_cursed(o_ptr) && (known || (o_ptr->ident & IDENT_SENSE)))
    {
        strcpy(fake_insc_buf, "cursed");
    }

    /* Note "unidentified" if the item is unidentified */
    else if ( (o_ptr->tval == TV_LITE || o_ptr->tval == TV_FIGURINE)
           && aware 
           && !known
           && !(o_ptr->ident & IDENT_SENSE) )
    {
        strcpy(fake_insc_buf, "unidentified");
    }

    /* Mega-Hack -- note empty wands/staffs */
    else if (!known && (o_ptr->ident & IDENT_EMPTY))
    {
        strcpy(fake_insc_buf, "empty");
    }

    /* Note "tried" if the object has been tested unsuccessfully */
    else if (!aware && object_is_tried(o_ptr))
    {
        strcpy(fake_insc_buf, "tried");
    }

    /* Note the discount, if any */
    if (o_ptr->discount)
    {
        if ((o_ptr->ident & IDENT_STORE) || show_discounts)
        {
            char discount_num_buf[4];

            /* Append to other fake inscriptions if any */
            if (fake_insc_buf[0]) strcat(fake_insc_buf, ", ");

            (void)object_desc_num(discount_num_buf, o_ptr->discount);
            strcat(fake_insc_buf, discount_num_buf);
            strcat(fake_insc_buf, "% off");
        }
    }


    /* Append the inscription, if any */
    if (fake_insc_buf[0] || tmp_val2[0])
    {
        /* Append the inscription */
        t = object_desc_chr(t, ' ');
        t = object_desc_chr(t, c1);

        /* Append fake inscriptions */
        if (fake_insc_buf[0])
        {
            t = object_desc_str(t, fake_insc_buf);
        }

        /* Append a separater */
        if (fake_insc_buf[0] && tmp_val2[0])
        {
            t = object_desc_chr(t, ',');
            t = object_desc_chr(t, ' ');
        }

        /* Append real inscriptions */
        if (tmp_val2[0])
        {
            t = object_desc_str(t, tmp_val2);
        }

        t = object_desc_chr(t, c2);
    }

object_desc_done:
    my_strcpy(buf, tmp_val, MAX_NLEN);
}


