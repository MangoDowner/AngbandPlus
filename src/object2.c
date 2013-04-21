/* File: object.c */

/* Purpose: misc code for objects */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"



/*
 * Delete a dungeon object
 */
void delete_object_idx(int i_idx)
{
    object_type *i_ptr = &i_list[i_idx];

    int y = i_ptr->iy;
    int x = i_ptr->ix;

    cave_type *c_ptr = &cave[y][x];


    /* Wipe the object */
    WIPE(i_ptr, object_type);


    /* Object is gone */
    c_ptr->i_idx = 0;

    /* Visual update */
    lite_spot(y, x);
}


/*
 * Deletes object from given location
 */
void delete_object(int y, int x)
{
    cave_type *c_ptr;

    /* Refuse "illegal" locations */
    if (!in_bounds(y, x)) return;

    /* Find where it was */
    c_ptr = &cave[y][x];

    /* Delete the object */
    if (c_ptr->i_idx) delete_object_idx(c_ptr->i_idx);
}



/*
 * Compact and Reorder the object list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" objects, we base the saving throw on a
 * combination of object level, distance from player, and current
 * "desperation".
 *
 * After "compacting" (if needed), we "reorder" the objects into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
void compact_objects(int size)
{
    int			i, y, x, num, cnt;

    int                 cur_lev, cur_dis, chance;


    /* Compact */
    if (size)
    {
        /* Message */
        msg_print("Compacting objects...");

        /* Redraw map */
        p_ptr->redraw |= (PR_MAP | PR_AROUND);
    }


    /* Compact at least 'size' objects */
    for (num = 0, cnt = 1; num < size; cnt++)
    {
        /* Get more vicious each iteration */
        cur_lev = 5 * cnt;

        /* Get closer each iteration */
        cur_dis = 5 * (20 - cnt);

        /* Examine the objects */
        for (i = 1; i < i_max; i++)
        {
            object_type *i_ptr = &i_list[i];

            object_kind *k_ptr = &k_info[i_ptr->k_idx];

            /* Skip dead objects */
            if (!i_ptr->k_idx) continue;

            /* Hack -- High level objects start out "immune" */
            if (k_ptr->level > cur_lev) continue;

            /* Get the location */
            y = i_ptr->iy;
            x = i_ptr->ix;

            /* Nearby objects start out "immune" */
            if ((cur_dis > 0) && (distance(py, px, y, x) < cur_dis)) continue;

            /* Saving throw */
            chance = 90;

            /* Hack -- only compact artifacts in emergencies */
            if (artifact_p(i_ptr) && (cnt < 1000)) chance = 100;

            /* Apply the saving throw */
            if (rand_int(100) < chance) continue;

            /* Delete it */
            delete_object_idx(i);

            /* Count it */
            num++;
        }
    }


    /* Excise dead objects (backwards!) */
    for (i = i_max - 1; i >= 1; i--)
    {
        /* Get the i'th object */
        object_type *i_ptr = &i_list[i];

        /* Skip real objects */
        if (i_ptr->k_idx) continue;

        /* One less object */
        i_max--;

        /* Reorder */
        if (i != i_max)
        {
            int ny = i_list[i_max].iy;
            int nx = i_list[i_max].ix;

            /* Update the cave */
            cave[ny][nx].i_idx = i;

            /* Structure copy */
            i_list[i] = i_list[i_max];

            /* Wipe the hole */
            WIPE(&i_list[i_max], object_type);
        }
    }

    /* Reset "i_nxt" */
    i_nxt = i_max;


    /* Reset "i_top" */
    i_top = 0;

    /* Collect "live" objects */
    for (i = 0; i < i_max; i++)
    {
        /* Collect indexes */
        i_fast[i_top++] = i;
    }
}




/*
 * Delete all the items when player leaves the level
 * Note -- we do NOT visually reflect these (irrelevant) changes
 */
void wipe_i_list()
{
    int i;

    /* Delete the existing objects */
    for (i = 1; i < i_max; i++)
    {
        object_type *i_ptr = &i_list[i];

        /* Skip dead objects */
        if (!i_ptr->k_idx) continue;

        /* Mega-Hack -- preserve artifacts */
        if (p_ptr->preserve)
        {
            /* Hack -- Preserve unknown artifacts */
            if (artifact_p(i_ptr) && !object_known_p(i_ptr))
            {
                /* Mega-Hack -- Preserve the artifact */
                a_info[i_ptr->name1].cur_num = 0;
            }
        }

        /* Object is gone */
        cave[i_ptr->iy][i_ptr->ix].i_idx = 0;

        /* Wipe the object */
        WIPE(i_ptr, object_type);
    }

    /* Restart free/heap pointers */
    i_nxt = i_max = 1;

    /* No more objects */
    i_top = 0;
}


/*
 * Acquires and returns the index of a "free" object.
 *
 * This routine should almost never fail, but in case it does,
 * we must be sure to handle "failure" of this routine.
 *
 * Note that this function must maintain the special "i_fast"
 * array of pointers to "live" objects.
 */
s16b i_pop(void)
{
    int i, n, k;


    /* Initial allocation */
    if (i_max < MAX_I_IDX)
    {
        /* Get next space */
        i = i_max;

        /* Expand object array */
        i_max++;

        /* Update "i_fast" */
        i_fast[i_top++] = i;

        /* Use this object */
        return (i);
    }


    /* Check for some space */
    for (n = 1; n < MAX_I_IDX; n++)
    {
        /* Get next space */
        i = i_nxt;

        /* Advance (and wrap) the "next" pointer */
        if (++i_nxt >= MAX_I_IDX) i_nxt = 1;

        /* Skip objects in use */
        if (i_list[i].k_idx) continue;

        /* Verify space XXX XXX */
        if (i_top >= MAX_I_IDX) continue;

        /* Verify not allocated */
        for (k = 0; k < i_top; k++)
        {
            /* Hack -- Prevent errors */
            if (i_fast[k] == i) i = 0;
        }

        /* Oops XXX XXX */
        if (!i) continue;

        /* Update "i_fast" */
        i_fast[i_top++] = i;

        /* Use this object */
        return (i);
    }


    /* Warn the player */
    if (character_dungeon) msg_print("Too many objects!");

    /* Oops */
    return (0);
}




/*
 * Choose an object kind that seems "appropriate" to the given level
 *
 * This function uses the "allocation table" built in "init.c".
 *
 * There is a small chance (1/20) of "boosting" the given depth by
 * a potentially large amount (see below).
 *
 * It is (slightly) more likely to acquire an object of the given level
 * than one of a lower level.  This is done by choosing several objects
 * appropriate to the given level and keeping the "hardest" one.
 *
 * XXX XXX XXX Note that this function may fail (very rarely).
 */
s16b get_obj_num(int level)
{
    int		i, p, k;

    int		k_idx;


    /* Obtain the table */
    s16b *t_lev = alloc_kind_index;
    kind_entry *table = alloc_kind_table;


    /* Hack -- Sometimes "boost" level */
    if (level > 0)
    {
        /* Occasionally, get a "better" object */
        if (rand_int(GREAT_OBJ) == 0)
        {
            /* What a bizarre calculation */
            level = 1 + (level * MAX_DEPTH / randint(MAX_DEPTH));
        }

        /* Maximum level */
        if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;
    }


    /* Hack -- Pick an object */
    for (k = 0; k < 10000; k++)
    {
        /* Town level is easy */
        if (level <= 0)
        {
            /* Pick a level 0 entry */
            i = rand_int(t_lev[0]);
        }

        /* Other levels sometimes have great stuff */
        else
        {
            /* Roll for rerolling */
            p = rand_int(100);

            /* Pick any object at or below the given level */
            i = rand_int(t_lev[level]);

            /* Try for a "better" item twice (10%) */
            if (p < 10)
            {
                /* Pick another object at or below the given level */
                int j = rand_int(t_lev[level]);

                /* Keep it if it is "better" */
                if (table[i].locale < table[j].locale) i = j;
            }

            /* Try for a "better" item once (50%) */
            if (p < 60)
            {
                /* Pick another object at or below the given level */
                int j = rand_int(t_lev[level]);

                /* Keep it if it is "better" */
                if (table[i].locale < table[j].locale) i = j;
            }
        }

        /* Access the "k_idx" of the chosen item */
        k_idx = table[i].k_idx;


        /* Hack -- apply the hook (if given) */
        if (get_obj_num_hook && (!(*get_obj_num_hook)(k_idx))) continue;


        /* Hack -- prevent embedded chests */
        if (opening_chest && (k_info[k_idx].tval == TV_CHEST)) continue;


        /* Roll for "rarity" */
        if (rand_int(table[i].chance) != 0) continue;


        /* Use this object */
        return (k_idx);
    }


    /* Oops */
    return (0);
}






/*
 * Known is true when the "attributes" of an object are "known".
 * These include tohit, todam, toac, cost, and pval (charges).
 *
 * Note that "knowing" an object gives you everything that an "awareness"
 * gives you, and much more.  In fact, the player is always "aware" of any
 * item of which he has full "knowledge".
 *
 * But having full knowledge of, say, one "wand of wonder", does not, by
 * itself, give you knowledge, or even awareness, of other "wands of wonder".
 * It happens that most "identify" routines (including "buying from a shop")
 * will make the player "aware" of the object as well as fully "know" it.
 *
 * This routine also removes any inscriptions generated by "feelings".
 */
void object_known(object_type *i_ptr)
{
    /* Remove "default inscriptions" */
    if (i_ptr->note && (i_ptr->ident & ID_SENSE))
    {
        /* Access the inscription */
        cptr q = quark_str(i_ptr->note);

        /* Hack -- Remove auto-inscriptions */
        if ((streq(q, "cursed")) ||
            (streq(q, "broken")) ||
            (streq(q, "good")) ||
            (streq(q, "average")) ||
            (streq(q, "excellent")) ||
            (streq(q, "worthless")) ||
            (streq(q, "special")) ||
            (streq(q, "terrible")))
        {
            /* Forget the inscription */
            i_ptr->note = 0;
        }
    }

    /* Clear the "Felt" info */
    i_ptr->ident &= ~ID_SENSE;

    /* Clear the "Empty" info */
    i_ptr->ident &= ~ID_EMPTY;

    /* Now we know about the item */
    i_ptr->ident |= ID_KNOWN;
}





/*
 * The player is now aware of the effects of the given object.
 */
void object_aware(object_type *i_ptr)
{
    /* Fully aware of the effects */
    k_info[i_ptr->k_idx].aware = TRUE;
}



/*
 * Something has been "sampled"
 */
void object_tried(object_type *i_ptr)
{
    /* Mark it as tried (even if "aware") */
    k_info[i_ptr->k_idx].tried = TRUE;
}



/*
 * Return the "value" of an "unknown" item
 * Make a guess at the value of non-aware items
 */
static s32b object_value_base(object_type *i_ptr)
{
    object_kind *k_ptr = &k_info[i_ptr->k_idx];

    /* Aware item -- use template cost */
    if (object_aware_p(i_ptr)) return (k_ptr->cost);

    /* Analyze the type */
    switch (i_ptr->tval)
    {
        /* Un-aware Food */
        case TV_FOOD: return (5L);

        /* Un-aware Potions */
        case TV_POTION: return (20L);

        /* Un-aware Scrolls */
        case TV_SCROLL: return (20L);

        /* Un-aware Staffs */
        case TV_STAFF: return (70L);

        /* Un-aware Wands */
        case TV_WAND: return (50L);

        /* Un-aware Rods */
        case TV_ROD: return (90L);

        /* Un-aware Rings */
        case TV_RING: return (45L);

        /* Un-aware Amulets */
        case TV_AMULET: return (45L);
    }

    /* Paranoia -- Oops */
    return (0L);
}


/*
 * Return the "real" price of a "known" item, not including discounts
 *
 * Wand and staffs get cost for each charge
 *
 * Armor is worth an extra 100 gold per bonus point to armor class.
 *
 * Weapons are worth an extra 100 gold per bonus point (AC,TH,TD).
 *
 * Missiles are only worth 5 gold per bonus point, since they
 * usually appear in groups of 20, and we want the player to get
 * the same amount of cash for any "equivalent" item.  Note that
 * missiles never have any of the "pval" flags, and in fact, they
 * only have a few of the available flags, primarily of the "slay"
 * and "brand" and "ignore" variety.
 *
 * Armor with a negative armor bonus is worthless.
 * Weapons with negative hit+damage bonuses are worthless.
 *
 * Every wearable item with a "pval" bonus is worth extra (see below).
 */
static s32b object_value_real(object_type *i_ptr)
{
    s32b value;

    u32b f1, f2, f3;

    object_kind *k_ptr = &k_info[i_ptr->k_idx];


    /* Hack -- "worthless" items */
    if (!k_ptr->cost) return (0L);

    /* Base cost */
    value = k_ptr->cost;


    /* Extract some flags */
    object_flags(i_ptr, &f1, &f2, &f3);


    /* Artifact */
    if (i_ptr->name1)
    {
        artifact_type *a_ptr = &a_info[i_ptr->name1];

        /* Hack -- "worthless" artifacts */
        if (!a_ptr->cost) return (0L);

        /* Hack -- Use the artifact cost instead */
        value = a_ptr->cost;
    }

    /* Ego-Item */
    else if (i_ptr->name2)
    {
        ego_item_type *e_ptr = &e_info[i_ptr->name2];

        /* Hack -- "worthless" ego-items */
        if (!e_ptr->cost) return (0L);

        /* Hack -- Reward the ego-item with a bonus */
        value += e_ptr->cost;
    }


    /* Analyze pval bonus */
    switch (i_ptr->tval)
    {
        case TV_SHOT:
        case TV_ARROW:
        case TV_BOLT:
        case TV_BOW:
        case TV_DIGGING:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_HELM:
        case TV_CROWN:
        case TV_SHIELD:
        case TV_CLOAK:
        case TV_SOFT_ARMOR:
        case TV_HARD_ARMOR:
        case TV_DRAG_ARMOR:
        case TV_LITE:
        case TV_AMULET:
        case TV_RING:

            /* Hack -- Negative "pval" is always bad */
            if (i_ptr->pval < 0) return (0L);

            /* No pval */
            if (!i_ptr->pval) break;

            /* Give credit for stat bonuses */
            if (f1 & TR1_STR) value += (i_ptr->pval * 200L);
            if (f1 & TR1_INT) value += (i_ptr->pval * 200L);
            if (f1 & TR1_WIS) value += (i_ptr->pval * 200L);
            if (f1 & TR1_DEX) value += (i_ptr->pval * 200L);
            if (f1 & TR1_CON) value += (i_ptr->pval * 200L);
            if (f1 & TR1_CHR) value += (i_ptr->pval * 200L);

            /* Give credit for stealth and searching */
            if (f1 & TR1_STEALTH) value += (i_ptr->pval * 100L);
            if (f1 & TR1_SEARCH) value += (i_ptr->pval * 100L);

            /* Give credit for infra-vision and tunneling */
            if (f1 & TR1_INFRA) value += (i_ptr->pval * 50L);
            if (f1 & TR1_TUNNEL) value += (i_ptr->pval * 50L);

            /* Give credit for extra attacks */
            if (f1 & TR1_BLOWS) value += (i_ptr->pval * 2000L);

            /* Give credit for speed bonus */
            if (f1 & TR1_SPEED) value += (i_ptr->pval * 30000L);

            break;
    }


    /* Analyze the item */
    switch (i_ptr->tval)
    {
        /* Wands/Staffs */
        case TV_WAND:
        case TV_STAFF:

            /* Pay extra for charges */
            value += ((value / 20) * i_ptr->pval);

            /* Done */
            break;

        /* Rings/Amulets */
        case TV_RING:
        case TV_AMULET:

            /* Hack -- negative bonuses are bad */
            if (i_ptr->to_a < 0) return (0L);
            if (i_ptr->to_h < 0) return (0L);
            if (i_ptr->to_d < 0) return (0L);

            /* Give credit for bonuses */
            value += ((i_ptr->to_h + i_ptr->to_d + i_ptr->to_a) * 100L);

            /* Done */
            break;

        /* Armor */
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_CLOAK:
        case TV_CROWN:
        case TV_HELM:
        case TV_SHIELD:
        case TV_SOFT_ARMOR:
        case TV_HARD_ARMOR:
        case TV_DRAG_ARMOR:

            /* Hack -- negative armor bonus */
            if (i_ptr->to_a < 0) return (0L);

            /* Give credit for bonuses */
            value += ((i_ptr->to_h + i_ptr->to_d + i_ptr->to_a) * 100L);

            /* Done */
            break;

        /* Bows/Weapons */
        case TV_BOW:
        case TV_DIGGING:
        case TV_HAFTED:
        case TV_SWORD:
        case TV_POLEARM:

            /* Hack -- negative hit/damage bonuses */
            if (i_ptr->to_h + i_ptr->to_d < 0) return (0L);

            /* Factor in the bonuses */
            value += ((i_ptr->to_h + i_ptr->to_d + i_ptr->to_a) * 100L);

            /* Hack -- Factor in extra damage dice */
            if ((i_ptr->dd > k_ptr->dd) && (i_ptr->ds == k_ptr->ds))
            {
                value += (i_ptr->dd - k_ptr->dd) * i_ptr->ds * 100L;
            }

            /* Done */
            break;

        /* Ammo */
        case TV_SHOT:
        case TV_ARROW:
        case TV_BOLT:

            /* Hack -- negative hit/damage bonuses */
            if (i_ptr->to_h + i_ptr->to_d < 0) return (0L);

            /* Factor in the bonuses */
            value += ((i_ptr->to_h + i_ptr->to_d) * 5L);

            /* Hack -- Factor in extra damage dice */
            if ((i_ptr->dd > k_ptr->dd) && (i_ptr->ds == k_ptr->ds))
            {
                value += (i_ptr->dd - k_ptr->dd) * i_ptr->ds * 5L;
            }

            /* Done */
            break;
    }


    /* Return the value */
    return (value);
}


/*
 * Return the price of an item including plusses (and charges)
 *
 * This function returns the "value" of the given item (qty one)
 *
 * Never notice "unknown" bonuses or properties, including "curses",
 * since that would give the player information he did not have.
 *
 * Note that discounted items stay discounted forever, even if
 * the discount is "forgotten" by the player via memory loss.
 */
s32b object_value(object_type *i_ptr)
{
    s32b value;


    /* Unknown items -- acquire a base value */
    if (object_known_p(i_ptr))
    {
        /* Broken items -- worthless */
        if (broken_p(i_ptr)) return (0L);

        /* Cursed items -- worthless */
        if (cursed_p(i_ptr)) return (0L);

        /* Real value (see above) */
        value = object_value_real(i_ptr);
    }

    /* Known items -- acquire the actual value */
    else
    {
        /* Hack -- Felt broken items */
        if ((i_ptr->ident & ID_SENSE) && broken_p(i_ptr)) return (0L);

        /* Hack -- Felt cursed items */
        if ((i_ptr->ident & ID_SENSE) && cursed_p(i_ptr)) return (0L);

        /* Base value (see above) */
        value = object_value_base(i_ptr);
    }


    /* Apply discount (if any) */
    if (i_ptr->discount) value -= (value * i_ptr->discount / 100L);


    /* Return the final value */
    return (value);
}





/*
 * Determine if an item can "absorb" a second item
 *
 * See "object_absorb()" for the actual "absorption" code.
 *
 * If permitted, we allow wands/staffs (if they are known to have equal
 * charges) and rods (if fully charged) to combine.
 *
 * Note that rods/staffs/wands are then unstacked when they are used.
 *
 * If permitted, we allow weapons/armor to stack, if they both known.
 *
 * Food, potions, scrolls, and "easy know" items always stack.
 *
 * Chests never stack (for various reasons).
 *
 * We do NOT allow activatable items (artifacts or dragon scale mail)
 * to stack, to keep the "activation" code clean.  Artifacts may stack,
 * but only with another identical artifact (which does not exist).
 *
 * Ego items may stack as long as they have the same ego-item type.
 * This is primarily to allow ego-missiles to stack.
 */
bool object_similar(object_type *i_ptr, object_type *j_ptr)
{
    int total = i_ptr->number + j_ptr->number;


    /* Require identical object types */
    if (i_ptr->k_idx != j_ptr->k_idx) return (0);


    /* Analyze the items */
    switch (i_ptr->tval)
    {
        /* Chests */
        case TV_CHEST:

            /* Never okay */
            return (0);


        /* Food and Potions and Scrolls */
        case TV_FOOD:
        case TV_POTION:
        case TV_SCROLL:

            /* Assume okay */
            break;

        /* Staffs and Wands */
        case TV_STAFF:
        case TV_WAND:

            /* Require knowledge */
            if (!object_known_p(i_ptr) || !object_known_p(j_ptr)) return (0);

            /* Fall through */

        /* Staffs and Wands and Rods */
        case TV_ROD:

            /* Require permission */
            if (!stack_allow_wands) return (0);

            /* Require identical charges */
            if (i_ptr->pval != j_ptr->pval) return (0);

            /* Probably okay */
            break;


        /* Weapons and Armor */
        case TV_BOW:
        case TV_DIGGING:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_HELM:
        case TV_CROWN:
        case TV_SHIELD:
        case TV_CLOAK:
        case TV_SOFT_ARMOR:
        case TV_HARD_ARMOR:
        case TV_DRAG_ARMOR:

            /* Require permission */
            if (!stack_allow_items) return (0);

            /* XXX XXX XXX Require identical "sense" status */
            /* if ((i_ptr->ident & ID_SENSE) != */
            /*     (j_ptr->ident & ID_SENSE)) return (0); */

            /* Fall through */

        /* Rings, Amulets, Lites */
        case TV_RING:
        case TV_AMULET:
        case TV_LITE:

            /* Require full knowledge of both items */
            if (!object_known_p(i_ptr) || !object_known_p(j_ptr)) return (0);

            /* Fall through */

        /* Missiles */
        case TV_BOLT:
        case TV_ARROW:
        case TV_SHOT:

            /* Require identical "bonuses" */
            if (i_ptr->to_h != j_ptr->to_h) return (FALSE);
            if (i_ptr->to_d != j_ptr->to_d) return (FALSE);
            if (i_ptr->to_a != j_ptr->to_a) return (FALSE);

            /* Require identical "pval" code */
            if (i_ptr->pval != j_ptr->pval) return (FALSE);

            /* Require identical "artifact" names */
            if (i_ptr->name1 != j_ptr->name1) return (FALSE);

            /* Require identical "ego-item" names */
            if (i_ptr->name2 != j_ptr->name2) return (FALSE);

            /* Hack -- Never stack "powerful" items */
            if (i_ptr->xtra1 || j_ptr->xtra1) return (FALSE);

            /* Hack -- Never stack recharging items */
            if (i_ptr->timeout || j_ptr->timeout) return (FALSE);

            /* Require identical "values" */
            if (i_ptr->ac != j_ptr->ac) return (FALSE);
            if (i_ptr->dd != j_ptr->dd) return (FALSE);
            if (i_ptr->ds != j_ptr->ds) return (FALSE);

            /* Probably okay */
            break;

        /* Various */
        default:

            /* Require knowledge */
            if (!object_known_p(i_ptr) || !object_known_p(j_ptr)) return (0);

            /* Probably okay */
            break;
    }


    /* Hack -- Require identical "cursed" status */
    if ((i_ptr->ident & ID_CURSED) != (j_ptr->ident & ID_CURSED)) return (0);

    /* Hack -- Require identical "broken" status */
    if ((i_ptr->ident & ID_BROKEN) != (j_ptr->ident & ID_BROKEN)) return (0);


    /* Hack -- require semi-matching "inscriptions" */
    if (i_ptr->note && j_ptr->note && (i_ptr->note != j_ptr->note)) return (0);

    /* Hack -- normally require matching "inscriptions" */
    if (!stack_force_notes && (i_ptr->note != j_ptr->note)) return (0);

    /* Hack -- normally require matching "discounts" */
    if (!stack_force_costs && (i_ptr->discount != j_ptr->discount)) return (0);


    /* Maximal "stacking" limit */
    if (total >= MAX_STACK_SIZE) return (0);


    /* They match, so they must be similar */
    return (TRUE);
}


/*
 * Allow one item to "absorb" another, assuming they are similar
 */
void object_absorb(object_type *i_ptr, object_type *j_ptr)
{
    int total = i_ptr->number + j_ptr->number;

    /* Add together the item counts */
    i_ptr->number = ((total < MAX_STACK_SIZE) ? total : (MAX_STACK_SIZE - 1));

    /* Hack -- blend "known" status */
    if (object_known_p(j_ptr)) object_known(i_ptr);

    /* Hack -- blend "rumour" status */
    if (j_ptr->ident & ID_RUMOUR) i_ptr->ident |= ID_RUMOUR;

    /* Hack -- blend "mental" status */
    if (j_ptr->ident & ID_MENTAL) i_ptr->ident |= ID_MENTAL;

    /* Hack -- blend "inscriptions" */
    if (j_ptr->note) i_ptr->note = j_ptr->note;

    /* Hack -- could average discounts XXX XXX XXX */
    /* Hack -- save largest discount XXX XXX XXX */
    if (i_ptr->discount < j_ptr->discount) i_ptr->discount = j_ptr->discount;
}



/*
 * Find the index of the object_kind with the given tval and sval
 */
s16b lookup_kind(int tval, int sval)
{
    int k;

    /* Look for it */
    for (k = 1; k < MAX_K_IDX; k++)
    {
        object_kind *k_ptr = &k_info[k];

        /* Found a match */
        if ((k_ptr->tval == tval) && (k_ptr->sval == sval)) return (k);
    }

    /* Oops */
    msg_format("No object (%d,%d)", tval, sval);

    /* Oops */
    return (0);
}


/*
 * Clear an item
 */
void invwipe(object_type *i_ptr)
{
    /* Clear the record */
    WIPE(i_ptr, object_type);
}


/*
 * Make "i_ptr" a "clean" copy of the given "kind" of object
 */
void invcopy(object_type *i_ptr, int k_idx)
{
    object_kind *k_ptr = &k_info[k_idx];

    /* Clear the record */
    WIPE(i_ptr, object_type);

    /* Save the kind index */
    i_ptr->k_idx = k_idx;

    /* Efficiency -- tval/sval */
    i_ptr->tval = k_ptr->tval;
    i_ptr->sval = k_ptr->sval;

    /* Default "pval" */
    i_ptr->pval = k_ptr->pval;

    /* Default number */
    i_ptr->number = 1;

    /* Default weight */
    i_ptr->weight = k_ptr->weight;

    /* Default magic */
    i_ptr->to_h = k_ptr->to_h;
    i_ptr->to_d = k_ptr->to_d;
    i_ptr->to_a = k_ptr->to_a;

    /* Default power */
    i_ptr->ac = k_ptr->ac;
    i_ptr->dd = k_ptr->dd;
    i_ptr->ds = k_ptr->ds;

    /* Hack -- worthless items are always "broken" */
    if (k_ptr->cost <= 0) i_ptr->ident |= ID_BROKEN;

    /* Hack -- cursed items are always "cursed" */
    if (k_ptr->flags3 & TR3_CURSED) i_ptr->ident |= ID_CURSED;
}





/*
 * Help determine an "enchantment bonus" for an object.
 *
 * To avoid floating point but still provide a smooth distribution of bonuses,
 * we simply round the results of division in such a way as to "average" the
 * correct floating point value.
 *
 * This function has been changed.  It uses "randnor()" to choose values from
 * a normal distribution, whose mean moves from zero towards the max as the
 * level increases, and whose standard deviation is equal to 1/4 of the max,
 * and whose values are forced to lie between zero and the max, inclusive.
 *
 * Since the "level" rarely passes 100 before Morgoth is dead, it is very
 * rare to get the "full" enchantment on an object, even a deep levels.
 *
 * It is always possible (albeit unlikely) to get the "full" enchantment.
 *
 * A sample distribution of values from "m_bonus(10, N)" is shown below:
 *
 *   N       0     1     2     3     4     5     6     7     8     9    10
 * ---    ----  ----  ----  ----  ----  ----  ----  ----  ----  ----  ----
 *   0   66.37 13.01  9.73  5.47  2.89  1.31  0.72  0.26  0.12  0.09  0.03
 *   8   46.85 24.66 12.13  8.13  4.20  2.30  1.05  0.36  0.19  0.08  0.05
 *  16   30.12 27.62 18.52 10.52  6.34  3.52  1.95  0.90  0.31  0.15  0.05
 *  24   22.44 15.62 30.14 12.92  8.55  5.30  2.39  1.63  0.62  0.28  0.11
 *  32   16.23 11.43 23.01 22.31 11.19  7.18  4.46  2.13  1.20  0.45  0.41
 *  40   10.76  8.91 12.80 29.51 16.00  9.69  5.90  3.43  1.47  0.88  0.65
 *  48    7.28  6.81 10.51 18.27 27.57 11.76  7.85  4.99  2.80  1.22  0.94
 *  56    4.41  4.73  8.52 11.96 24.94 19.78 11.06  7.18  3.68  1.96  1.78
 *  64    2.81  3.07  5.65  9.17 13.01 31.57 13.70  9.30  6.04  3.04  2.64
 *  72    1.87  1.99  3.68  7.15 10.56 20.24 25.78 12.17  7.52  4.42  4.62
 *  80    1.02  1.23  2.78  4.75  8.37 12.04 27.61 18.07 10.28  6.52  7.33
 *  88    0.70  0.57  1.56  3.12  6.34 10.06 15.76 30.46 12.58  8.47 10.38
 *  96    0.27  0.60  1.25  2.28  4.30  7.60 10.77 22.52 22.51 11.37 16.53
 * 104    0.22  0.42  0.77  1.36  2.62  5.33  8.93 13.05 29.54 15.23 22.53
 * 112    0.15  0.20  0.56  0.87  2.00  3.83  6.86 10.06 17.89 27.31 30.27
 * 120    0.03  0.11  0.31  0.46  1.31  2.48  4.60  7.78 11.67 25.53 45.72
 * 128    0.02  0.01  0.13  0.33  0.83  1.41  3.24  6.17  9.57 14.22 64.07
 */
static s16b m_bonus(int max, int level)
{
    int bonus, stand, extra, value;


    /* Paranoia -- enforce maximal "level" */
    if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;


    /* The "bonus" moves towards the max */
    bonus = ((max * level) / MAX_DEPTH);

    /* Hack -- determine fraction of error */
    extra = ((max * level) % MAX_DEPTH);

    /* Hack -- simulate floating point computations */
    if (rand_int(MAX_DEPTH) < extra) bonus++;


    /* The "stand" is equal to one quarter of the max */
    stand = (max / 4);

    /* Hack -- determine fraction of error */
    extra = (max % 4);

    /* Hack -- simulate floating point computations */
    if (rand_int(4) < extra) stand++;


    /* Choose an "interesting" value */
    value = randnor(bonus, stand);

    /* Enforce the minimum value */
    if (value < 0) return (0);

    /* Enforce the maximum value */
    if (value > max) return (max);

    /* Result */
    return (value);
}




/*
 * Cheat -- describe a created object for the user
 */
static void object_mention(object_type *i_ptr)
{
    char i_name[80];

    /* Describe */
    object_desc_store(i_name, i_ptr, FALSE, 0);

    /* Artifact */
    if (artifact_p(i_ptr))
    {
        /* Silly message */
        msg_format("Artifact (%s)", i_name);
    }

    /* Ego-item */
    else if (ego_item_p(i_ptr))
    {
        /* Silly message */
        msg_format("Ego-item (%s)", i_name);
    }

    /* Normal item */
    else
    {
        /* Silly message */
        msg_format("Object (%s)", i_name);
    }
}




/*
 * Mega-Hack -- Attempt to create one of the "Special Objects"
 *
 * We are only called from "place_object()", and we assume that
 * "apply_magic()" is called immediately after we return.
 *
 * Note -- see "make_artifact()" and "apply_magic()"
 */
static bool make_artifact_special(object_type *i_ptr)
{
    int			i;

    int			k_idx = 0;


    /* No artifacts in the town */
    if (!dun_level) return (FALSE);

    /* Check the artifact list (just the "specials") */
    for (i = 0; i < ART_MIN_NORMAL; i++)
    {
        artifact_type *a_ptr = &a_info[i];

        /* Skip "empty" artifacts */
        if (!a_ptr->name) continue;

        /* Cannot make an artifact twice */
        if (a_ptr->cur_num) continue;

        /* XXX XXX Enforce minimum "depth" (loosely) */
        if (a_ptr->level > dun_level)
        {
            /* Acquire the "out-of-depth factor" */
            int d = (a_ptr->level - dun_level) * 2;

            /* Roll for out-of-depth creation */
            if (rand_int(d) != 0) continue;
        }

        /* Artifact "rarity roll" */
        if (rand_int(a_ptr->rarity) != 0) return (0);

        /* Find the base object */
        k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);

        /* XXX XXX Enforce minimum "object" level (loosely) */
        if (k_info[k_idx].level > object_level)
        {
            /* Acquire the "out-of-depth factor" */
            int d = (k_info[k_idx].level - object_level) * 5;

            /* Roll for out-of-depth creation */
            if (rand_int(d) != 0) continue;
        }

        /* Assign the template */
        invcopy(i_ptr, k_idx);

        /* Mega-Hack -- mark the item as an artifact */
        i_ptr->name1 = i;

        /* Success */
        return (TRUE);
    }

    /* Failure */
    return (FALSE);
}


/*
 * Attempt to change an object into an artifact
 *
 * This routine should only be called by "apply_magic()"
 *
 * Note -- see "make_artifact_special()" and "apply_magic()"
 */
static bool make_artifact(object_type *i_ptr)
{
    int i;


    /* No artifacts in the town */
    if (!dun_level) return (FALSE);

    /* Paranoia -- no "plural" artifacts */
    if (i_ptr->number != 1) return (FALSE);

    /* Check the artifact list (skip the "specials") */
    for (i = ART_MIN_NORMAL; i < MAX_A_IDX; i++)
    {
        artifact_type *a_ptr = &a_info[i];

        /* Skip "empty" items */
        if (!a_ptr->name) continue;

        /* Cannot make an artifact twice */
        if (a_ptr->cur_num) continue;

        /* Must have the correct fields */
        if (a_ptr->tval != i_ptr->tval) continue;
        if (a_ptr->sval != i_ptr->sval) continue;

        /* XXX XXX Enforce minimum "depth" (loosely) */
        if (a_ptr->level > dun_level)
        {
            /* Acquire the "out-of-depth factor" */
            int d = (a_ptr->level - dun_level) * 2;

            /* Roll for out-of-depth creation */
            if (rand_int(d) != 0) continue;
        }

        /* We must make the "rarity roll" */
        if (rand_int(a_ptr->rarity) != 0) continue;

        /* Hack -- mark the item as an artifact */
        i_ptr->name1 = i;

        /* Success */
        return (TRUE);
    }

    /* Failure */
    return (FALSE);
}


/*
 * Charge a new wand.
 */
static void charge_wand(object_type *i_ptr)
{
    switch (i_ptr->sval)
    {
        case SV_WAND_HEAL_MONSTER:	i_ptr->pval = randint(20) + 8; break;
        case SV_WAND_HASTE_MONSTER:	i_ptr->pval = randint(20) + 8; break;
        case SV_WAND_CLONE_MONSTER:	i_ptr->pval = randint(5)  + 3; break;
        case SV_WAND_TELEPORT_AWAY:	i_ptr->pval = randint(5)  + 6; break;
        case SV_WAND_DISARMING:		i_ptr->pval = randint(5)  + 4; break;
        case SV_WAND_TRAP_DOOR_DEST:	i_ptr->pval = randint(8)  + 6; break;
        case SV_WAND_STONE_TO_MUD:	i_ptr->pval = randint(4)  + 3; break;
        case SV_WAND_LITE:		i_ptr->pval = randint(10) + 6; break;
        case SV_WAND_SLEEP_MONSTER:	i_ptr->pval = randint(15) + 8; break;
        case SV_WAND_SLOW_MONSTER:	i_ptr->pval = randint(10) + 6; break;
        case SV_WAND_CONFUSE_MONSTER:	i_ptr->pval = randint(12) + 6; break;
        case SV_WAND_FEAR_MONSTER:	i_ptr->pval = randint(5)  + 3; break;
        case SV_WAND_DRAIN_LIFE:	i_ptr->pval = randint(3)  + 3; break;
        case SV_WAND_POLYMORPH:		i_ptr->pval = randint(8)  + 6; break;
        case SV_WAND_STINKING_CLOUD:	i_ptr->pval = randint(8)  + 6; break;
        case SV_WAND_MAGIC_MISSILE:	i_ptr->pval = randint(10) + 6; break;
        case SV_WAND_ACID_BOLT:		i_ptr->pval = randint(8)  + 6; break;
        case SV_WAND_ELEC_BOLT:		i_ptr->pval = randint(8)  + 6; break;
        case SV_WAND_FIRE_BOLT:		i_ptr->pval = randint(8)  + 6; break;
        case SV_WAND_COLD_BOLT:		i_ptr->pval = randint(5)  + 6; break;
        case SV_WAND_ACID_BALL:		i_ptr->pval = randint(5)  + 2; break;
        case SV_WAND_ELEC_BALL:		i_ptr->pval = randint(8)  + 4; break;
        case SV_WAND_FIRE_BALL:		i_ptr->pval = randint(4)  + 2; break;
        case SV_WAND_COLD_BALL:		i_ptr->pval = randint(6)  + 2; break;
        case SV_WAND_WONDER:		i_ptr->pval = randint(15) + 8; break;
        case SV_WAND_ANNIHILATION:	i_ptr->pval = randint(2)  + 1; break;
        case SV_WAND_DRAGON_FIRE:	i_ptr->pval = randint(3)  + 1; break;
        case SV_WAND_DRAGON_COLD:	i_ptr->pval = randint(3)  + 1; break;
        case SV_WAND_DRAGON_BREATH:	i_ptr->pval = randint(3)  + 1; break;
    }
}



/*
 * Charge a new staff.
 */
static void charge_staff(object_type *i_ptr)
{
    switch (i_ptr->sval)
    {
        case SV_STAFF_DARKNESS:		i_ptr->pval = randint(8)  + 8; break;
        case SV_STAFF_SLOWNESS:		i_ptr->pval = randint(8)  + 8; break;
        case SV_STAFF_HASTE_MONSTERS:	i_ptr->pval = randint(8)  + 8; break;
        case SV_STAFF_SUMMONING:	i_ptr->pval = randint(3)  + 1; break;
        case SV_STAFF_TELEPORTATION:	i_ptr->pval = randint(4)  + 5; break;
        case SV_STAFF_IDENTIFY:		i_ptr->pval = randint(15) + 5; break;
        case SV_STAFF_REMOVE_CURSE:	i_ptr->pval = randint(3)  + 4; break;
        case SV_STAFF_STARLITE:		i_ptr->pval = randint(5)  + 6; break;
        case SV_STAFF_LITE:		i_ptr->pval = randint(20) + 8; break;
        case SV_STAFF_MAPPING:		i_ptr->pval = randint(5)  + 5; break;
        case SV_STAFF_DETECT_GOLD:	i_ptr->pval = randint(20) + 8; break;
        case SV_STAFF_DETECT_ITEM:	i_ptr->pval = randint(15) + 6; break;
        case SV_STAFF_DETECT_TRAP:	i_ptr->pval = randint(5)  + 6; break;
        case SV_STAFF_DETECT_DOOR:	i_ptr->pval = randint(8)  + 6; break;
        case SV_STAFF_DETECT_INVIS:	i_ptr->pval = randint(15) + 8; break;
        case SV_STAFF_DETECT_EVIL:	i_ptr->pval = randint(15) + 8; break;
        case SV_STAFF_CURE_LIGHT:	i_ptr->pval = randint(5)  + 6; break;
        case SV_STAFF_CURING:		i_ptr->pval = randint(3)  + 4; break;
        case SV_STAFF_HEALING:		i_ptr->pval = randint(2)  + 1; break;
        case SV_STAFF_THE_MAGI:		i_ptr->pval = randint(2)  + 2; break;
        case SV_STAFF_SLEEP_MONSTERS:	i_ptr->pval = randint(5)  + 6; break;
        case SV_STAFF_SLOW_MONSTERS:	i_ptr->pval = randint(5)  + 6; break;
        case SV_STAFF_SPEED:		i_ptr->pval = randint(3)  + 4; break;
        case SV_STAFF_PROBING:		i_ptr->pval = randint(6)  + 2; break;
        case SV_STAFF_DISPEL_EVIL:	i_ptr->pval = randint(3)  + 4; break;
        case SV_STAFF_POWER:		i_ptr->pval = randint(3)  + 1; break;
        case SV_STAFF_HOLINESS:		i_ptr->pval = randint(2)  + 2; break;
        case SV_STAFF_GENOCIDE:		i_ptr->pval = randint(2)  + 1; break;
        case SV_STAFF_EARTHQUAKES:	i_ptr->pval = randint(5)  + 3; break;
        case SV_STAFF_DESTRUCTION:	i_ptr->pval = randint(3)  + 1; break;
    }
}



/*
 * Apply magic to an item known to be a "weapon"
 *
 * Hack -- note special base damage dice boosting
 * Hack -- note special processing for weapon/digger
 * Hack -- note special rating boost for dragon scale mail
 */
static void a_m_aux_1(object_type *i_ptr, int level, int power)
{
    int tohit1 = randint(5) + m_bonus(5, level);
    int todam1 = randint(5) + m_bonus(5, level);

    int tohit2 = m_bonus(10, level);
    int todam2 = m_bonus(10, level);


    /* Good */
    if (power > 0)
    {
        /* Enchant */
        i_ptr->to_h += tohit1;
        i_ptr->to_d += todam1;

        /* Very good */
        if (power > 1)
        {
            /* Enchant again */
            i_ptr->to_h += tohit2;
            i_ptr->to_d += todam2;
        }
    }

    /* Cursed */
    else if (power < 0)
    {
        /* Penalize */
        i_ptr->to_h -= tohit1;
        i_ptr->to_d -= todam1;

        /* Very cursed */
        if (power < -1)
        {
            /* Penalize again */
            i_ptr->to_h -= tohit2;
            i_ptr->to_d -= todam2;
        }

        /* Cursed (if "bad") */
        if (i_ptr->to_h + i_ptr->to_d < 0) i_ptr->ident |= ID_CURSED;
    }


    /* Analyze type */
    switch (i_ptr->tval)
    {
        case TV_DIGGING:

            /* Very good */
            if (power > 1)
            {
                /* Special Ego-item */
                i_ptr->name2 = EGO_DIGGING;
            }

            /* Bad */
            else if (power < 0)
            {
                /* Hack -- Reverse digging bonus */
                i_ptr->pval = 0 - (i_ptr->pval);
            }

            /* Very bad */
            else if (power < -1)
            {
                /* Hack -- Horrible digging bonus */
                i_ptr->pval = 0 - (5 + randint(5));
            }

            break;


        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:

            /* Very Good */
            if (power > 1)
            {
                /* Roll for an ego-item */
                switch (randint(29))
                {
                    case 1:
                        i_ptr->name2 = EGO_HA;
                        break;

                    case 2:
                        i_ptr->name2 = EGO_DF;
                        break;

                    case 3:
                        i_ptr->name2 = EGO_BRAND_ACID;
                        break;

                    case 4:
                        i_ptr->name2 = EGO_BRAND_ELEC;
                        break;

                    case 5:
                        i_ptr->name2 = EGO_BRAND_FIRE;
                        break;

                    case 6:
                        i_ptr->name2 = EGO_BRAND_COLD;
                        break;

                    case 7: case 8:
                        i_ptr->name2 = EGO_SLAY_ANIMAL;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_ANIMAL;
                        }
                        break;

                    case 9: case 10:
                        i_ptr->name2 = EGO_SLAY_DRAGON;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_DRAGON;
                        }
                        break;

                    case 11: case 12:
                        i_ptr->name2 = EGO_SLAY_EVIL;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_EVIL;
                        }
                        break;

                    case 13: case 14:
                        i_ptr->name2 = EGO_SLAY_UNDEAD;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_UNDEAD;
                        }
                        break;

                    case 15: case 16: case 17:
                        i_ptr->name2 = EGO_SLAY_ORC;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_ORC;
                        }
                        break;

                    case 18: case 19: case 20:
                        i_ptr->name2 = EGO_SLAY_TROLL;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_TROLL;
                        }
                        break;

                    case 21: case 22: case 23:
                        i_ptr->name2 = EGO_SLAY_GIANT;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_GIANT;
                        }
                        break;

                    case 24: case 25: case 26:
                        i_ptr->name2 = EGO_SLAY_DEMON;
                        if (rand_int(100) < 20)
                        {
                            i_ptr->name2 = EGO_KILL_DEMON;
                        }
                        break;

                    case 27:
                        i_ptr->name2 = EGO_WEST;
                        break;

                    case 28:
                        i_ptr->name2 = EGO_BLESS_BLADE;
                        break;

                    case 29:
                        i_ptr->name2 = EGO_ATTACKS;
                        break;
                }

                /* Hack -- Super-charge the damage dice */
                while (rand_int(10L * i_ptr->dd * i_ptr->ds) == 0) i_ptr->dd++;

                /* Hack -- Lower the damage dice */
                if (i_ptr->dd > 9) i_ptr->dd = 9;
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Roll for ego-item */
                if (rand_int(MAX_DEPTH) < level)
                {
                    i_ptr->name2 = EGO_MORGUL;
                }
            }

            break;


        case TV_BOW:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(10))
                {
                    case 1:
                        i_ptr->name2 = EGO_EXTRA_MIGHT;
                        break;

                    case 2:
                        i_ptr->name2 = EGO_EXTRA_SHOTS;
                        break;

                    case 3: case 4: case 5: case 6:
                        i_ptr->name2 = EGO_VELOCITY;
                        break;

                    case 7: case 8: case 9: case 10:
                        i_ptr->name2 = EGO_ACCURACY;
                        break;
                }
            }

            break;


        case TV_BOLT:
        case TV_ARROW:
        case TV_SHOT:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(11))
                {
                    case 1: case 2: case 3:
                        i_ptr->name2 = EGO_WOUNDING;
                        break;

                    case 4:
                        i_ptr->name2 = EGO_FLAME;
                        break;

                    case 5:
                        i_ptr->name2 = EGO_FROST;
                        break;

                    case 6: case 7:
                        i_ptr->name2 = EGO_HURT_ANIMAL;
                        break;

                    case 8: case 9:
                        i_ptr->name2 = EGO_HURT_EVIL;
                        break;

                    case 10:
                        i_ptr->name2 = EGO_HURT_DRAGON;
                        break;

		    case 11:
		    	i_ptr->name2 = EGO_VENOM;
		    	break;
                }

                /* Hack -- super-charge the damage dice */
                while (rand_int(4L * i_ptr->dd * i_ptr->ds) == 0) i_ptr->dd++;

                /* Hack -- restrict the damage dice */
                if (i_ptr->dd > 9) i_ptr->dd = 9;
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Roll for ego-item */
                if (rand_int(MAX_DEPTH) < level)
                {
                    i_ptr->name2 = EGO_BACKBITING;
                }
            }

            break;
    }
}


/*
 * Apply magic to an item known to be "armor"
 *
 * Hack -- note special processing for crown/helm
 * Hack -- note special processing for robe of permanence
 */
static void a_m_aux_2(object_type *i_ptr, int level, int power)
{
    int toac1 = randint(5) + m_bonus(5, level);

    int toac2 = m_bonus(10, level);


    /* Good */
    if (power > 0)
    {
        /* Enchant */
        i_ptr->to_a += toac1;

        /* Very good */
        if (power > 1)
        {
            /* Enchant again */
            i_ptr->to_a += toac2;
        }
    }

    /* Cursed */
    else if (power < 0)
    {
        /* Penalize */
        i_ptr->to_a -= toac1;

        /* Very cursed */
        if (power < -1)
        {
            /* Penalize again */
            i_ptr->to_a -= toac2;
        }

        /* Cursed (if "bad") */
        if (i_ptr->to_a < 0) i_ptr->ident |= ID_CURSED;
    }


    /* Analyze type */
    switch (i_ptr->tval)
    {
        case TV_DRAG_ARMOR:

            /* Rating boost */
            rating += 30;

            /* Mention the item */
            if (cheat_peek) object_mention(i_ptr);

            break;


        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:

            /* Very good */
            if (power > 1)
            {
                /* Hack -- Try for "Robes of the Magi" */
                if ((i_ptr->tval == TV_SOFT_ARMOR) &&
                    (i_ptr->sval == SV_ROBE) &&
                    (rand_int(100) < 10))
                {
                    i_ptr->name2 = EGO_PERMANENCE;
                    break;
                }

                /* Roll for ego-item */
                switch (randint(19))
                {
                    case 1: case 2: case 3: case 4:
                        i_ptr->name2 = EGO_RESIST_ACID;
                        break;

                    case 5: case 6: case 7: case 8:
                        i_ptr->name2 = EGO_RESIST_ELEC;
                        break;

                    case 9: case 10: case 11: case 12:
                        i_ptr->name2 = EGO_RESIST_FIRE;
                        break;

                    case 13: case 14: case 15: case 16:
                        i_ptr->name2 = EGO_RESIST_COLD;
                        break;

                    case 17: case 18:
                        i_ptr->name2 = EGO_RESISTANCE;
                        break;

                    default:
                        i_ptr->name2 = EGO_ELVENKIND;
                        break;
                }
            }

            break;


        case TV_SHIELD:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(10))
                {
                    case 1:
                        i_ptr->name2 = EGO_ENDURE_ACID;
                        break;

                    case 2: case 3: case 4:
                        i_ptr->name2 = EGO_ENDURE_ELEC;
                        break;

                    case 5: case 6:
                        i_ptr->name2 = EGO_ENDURE_FIRE;
                        break;

                    case 7: case 8: case 9:
                        i_ptr->name2 = EGO_ENDURE_COLD;
                        break;

                    default:
                        i_ptr->name2 = EGO_ENDURANCE;
                        break;
                }
            }

            break;


        case TV_GLOVES:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(10))
                {
                    case 1: case 2: case 3: case 4:
                        i_ptr->name2 = EGO_FREE_ACTION;
                        break;

                    case 5: case 6: case 7:
                        i_ptr->name2 = EGO_SLAYING;
                        break;

                    case 8: case 9:
                        i_ptr->name2 = EGO_AGILITY;
                        break;

                    case 10:
                        i_ptr->name2 = EGO_POWER;
                        break;
                }
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Roll for ego-item */
                switch (randint(2))
                {
                    case 1:
                        i_ptr->name2 = EGO_CLUMSINESS;
                        break;
                    default:
                        i_ptr->name2 = EGO_WEAKNESS;
                        break;
                }
            }

            break;


        case TV_BOOTS:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(24))
                {
                    case 1:
                        i_ptr->name2 = EGO_SPEED;
                        break;

                    case 2: case 3: case 4: case 5:
                        i_ptr->name2 = EGO_MOTION;
                        break;

                    case 6: case 7: case 8: case 9:
                    case 10: case 11: case 12: case 13:
                        i_ptr->name2 = EGO_QUIET;
                        break;

                    default:
                        i_ptr->name2 = EGO_SLOW_DESCENT;
                        break;
                }
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Roll for ego-item */
                switch (randint(3))
                {
                    case 1:
                        i_ptr->name2 = EGO_NOISE;
                        break;
                    case 2:
                        i_ptr->name2 = EGO_SLOWNESS;
                        break;
                    case 3:
                        i_ptr->name2 = EGO_ANNOYANCE;
                        break;
                }
            }

            break;


        case TV_CROWN:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(8))
                {
                    case 1:
                        i_ptr->name2 = EGO_MAGI;
                        break;

                    case 2:
                        i_ptr->name2 = EGO_MIGHT;
                        break;

                    case 3:
                        i_ptr->name2 = EGO_TELEPATHY;
                        break;

                    case 4:
                        i_ptr->name2 = EGO_REGENERATION;
                        break;

                    case 5: case 6:
                        i_ptr->name2 = EGO_LORDLINESS;
                        break;

                    default:
                        i_ptr->name2 = EGO_SEEING;
                        break;
                }
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Roll for ego-item */
                switch (randint(7))
                {
                    case 1: case 2:
                        i_ptr->name2 = EGO_STUPIDITY;
                        break;
                    case 3: case 4:
                        i_ptr->name2 = EGO_NAIVETY;
                        break;
                    case 5:
                        i_ptr->name2 = EGO_UGLINESS;
                        break;
                    case 6:
                        i_ptr->name2 = EGO_SICKLINESS;
                        break;
                    case 7:
                        i_ptr->name2 = EGO_TELEPORTATION;
                        break;
                }
            }

            break;


        case TV_HELM:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(14))
                {
                    case 1: case 2:
                        i_ptr->name2 = EGO_INTELLIGENCE;
                        break;

                    case 3: case 4:
                        i_ptr->name2 = EGO_WISDOM;
                        break;

                    case 5: case 6:
                        i_ptr->name2 = EGO_BEAUTY;
                        break;

                    case 7: case 8:
                        i_ptr->name2 = EGO_SEEING;
                        break;

                    case 9: case 10:
                        i_ptr->name2 = EGO_LITE;
                        break;

                    default:
                        i_ptr->name2 = EGO_INFRAVISION;
                        break;
                }
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Roll for ego-item */
                switch (randint(7))
                {
                    case 1: case 2:
                        i_ptr->name2 = EGO_STUPIDITY;
                        break;
                    case 3: case 4:
                        i_ptr->name2 = EGO_NAIVETY;
                        break;
                    case 5:
                        i_ptr->name2 = EGO_UGLINESS;
                        break;
                    case 6:
                        i_ptr->name2 = EGO_SICKLINESS;
                        break;
                    case 7:
                        i_ptr->name2 = EGO_TELEPORTATION;
                        break;
                }
            }

            break;


        case TV_CLOAK:

            /* Very good */
            if (power > 1)
            {
                /* Roll for ego-item */
                switch (randint(17))
                {
                    case 1: case 2: case 3: case 4:
                    case 5: case 6: case 7: case 8:
                        i_ptr->name2 = EGO_PROTECTION;
                        break;

                    case 9: case 10: case 11: case 12:
                    case 13: case 14: case 15: case 16:
                        i_ptr->name2 = EGO_STEALTH;
                        break;

                    case 17:
                        i_ptr->name2 = EGO_AMAN;
                        break;
                }
            }

            /* Very cursed */
            else if (power < -1)
            {
                /* Choose some damage */
                switch (randint(3))
                {
                    case 1:
                        i_ptr->name2 = EGO_IRRITATION;
                        break;
                    case 2:
                        i_ptr->name2 = EGO_VULNERABILITY;
                        break;
                    case 3:
                        i_ptr->name2 = EGO_ENVELOPING;
                        break;
                }
            }

            break;
    }
}



/*
 * Apply magic to an item known to be a "ring" or "amulet"
 *
 * Hack -- note special rating boost for ring of speed
 * Hack -- note special rating boost for amulet of the magi
 * Hack -- note special "pval boost" code for ring of speed
 * Hack -- note that some items must be cursed (or blessed)
 */
static void a_m_aux_3(object_type *i_ptr, int level, int power)
{
    /* Apply magic (good or bad) according to type */
    switch (i_ptr->tval)
    {
        case TV_RING:
        {
            /* Analyze */
            switch (i_ptr->sval)
            {
                /* Strength, Constitution, Dexterity, Intelligence */
                case SV_RING_STR:
                case SV_RING_CON:
                case SV_RING_DEX:
                case SV_RING_INT:
                {
                    /* Stat bonus */
                    i_ptr->pval = 1 + m_bonus(5, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse pval */
                        i_ptr->pval = 0 - (i_ptr->pval);
                    }

                    break;
                }

                /* Ring of Speed! */
                case SV_RING_SPEED:
                {
                    /* Base speed (1 to 10) */
                    i_ptr->pval = randint(5) + m_bonus(5, level);

                    /* Super-charge the ring */
                    while (rand_int(100) < 50) i_ptr->pval++;

                    /* Cursed Ring */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse pval */
                        i_ptr->pval = 0 - (i_ptr->pval);

                        break;
                    }

                    /* Rating boost */
                    rating += 25;

                    /* Mention the item */
                    if (cheat_peek) object_mention(i_ptr);

                    break;
                }

                /* Searching */
                case SV_RING_SEARCHING:
                {
                    /* Bonus to searching */
                    i_ptr->pval = 1 + m_bonus(5, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse pval */
                        i_ptr->pval = 0 - (i_ptr->pval);
                    }

                    break;
                }

                /* Flames, Acid, Ice */
                case SV_RING_FLAMES:
                case SV_RING_ACID:
                case SV_RING_ICE:
                {
                    /* Bonus to armor class */
                    i_ptr->to_a = 5 + randint(5) + m_bonus(10, level);
                    break;
                }

                /* Weakness, Stupidity */
                case SV_RING_WEAKNESS:
                case SV_RING_STUPIDITY:
                {
                    /* Broken */
                    i_ptr->ident |= ID_BROKEN;

                    /* Cursed */
                    i_ptr->ident |= ID_CURSED;

                    /* Penalize */
                    i_ptr->pval = 0 - (1 + m_bonus(5, level));

                    break;
                }

                /* WOE, Stupidity */
                case SV_RING_WOE:
                {
                    /* Broken */
                    i_ptr->ident |= ID_BROKEN;

                    /* Cursed */
                    i_ptr->ident |= ID_CURSED;

                    /* Penalize */
                    i_ptr->to_a = 0 - (5 + m_bonus(10, level));
                    i_ptr->pval = 0 - (1 + m_bonus(5, level));

                    break;
                }

                /* Ring of damage */
                case SV_RING_DAMAGE:
                {
                    /* Bonus to damage */
                    i_ptr->to_d = 1 + randint(5) + m_bonus(6, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse bonus */
                        i_ptr->to_d = 0 - (i_ptr->to_d);
                    }

                    break;
                }

                /* Ring of Accuracy */
                case SV_RING_ACCURACY:
                {
                    /* Bonus to hit */
                    i_ptr->to_h = 1 + randint(5) + m_bonus(6, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse tohit */
                        i_ptr->to_h = 0 - (i_ptr->to_h);
                    }

                    break;
                }

                /* Ring of Protection */
                case SV_RING_PROTECTION:
                {
                    /* Bonus to armor class */
                    i_ptr->to_a = 5 + randint(5) + m_bonus(10, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse toac */
                        i_ptr->to_a = 0 - (i_ptr->to_a);
                    }

                    break;
                }

                /* Ring of Slaying */
                case SV_RING_SLAYING:
                {
                    /* Bonus to damage and to hit */
                    i_ptr->to_d = 1 + randint(5) + m_bonus(6, level);
                    i_ptr->to_h = 1 + randint(5) + m_bonus(6, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse bonuses */
                        i_ptr->to_h = 0 - (i_ptr->to_h);
                        i_ptr->to_d = 0 - (i_ptr->to_d);
                    }

                    break;
                }
            }

            break;
        }

        case TV_AMULET:
        {
            /* Analyze */
            switch (i_ptr->sval)
            {
                /* Amulet of wisdom/charisma */
                case SV_AMULET_WISDOM:
                case SV_AMULET_CHARISMA:
                {
                    i_ptr->pval = 1 + m_bonus(5, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse bonuses */
                        i_ptr->pval = 0 - (i_ptr->pval);
                    }

                    break;
                }

                /* Amulet of searching */
                case SV_AMULET_SEARCHING:
                {
                    i_ptr->pval = randint(5) + m_bonus(5, level);

                    /* Cursed */
                    if (power < 0)
                    {
                        /* Broken */
                        i_ptr->ident |= ID_BROKEN;

                        /* Cursed */
                        i_ptr->ident |= ID_CURSED;

                        /* Reverse bonuses */
                        i_ptr->pval = 0 - (i_ptr->pval);
                    }

                    break;
                }

                /* Amulet of the Magi -- never cursed */
                case SV_AMULET_THE_MAGI:
                {
                    i_ptr->pval = randint(5) + m_bonus(5, level);
                    i_ptr->to_a = randint(5) + m_bonus(5, level);

                    /* Boost the rating */
                    rating += 25;

                    /* Mention the item */
                    if (cheat_peek) object_mention(i_ptr);

                    break;
                }

                /* Amulet of Doom -- always cursed */
                case SV_AMULET_DOOM:
                {
                    /* Broken */
                    i_ptr->ident |= ID_BROKEN;

                    /* Cursed */
                    i_ptr->ident |= ID_CURSED;

                    /* Penalize */
                    i_ptr->pval = 0 - (randint(5) + m_bonus(5, level));
                    i_ptr->to_a = 0 - (randint(5) + m_bonus(5, level));

                    break;
                }
            }

            break;
        }
    }
}


/*
 * Apply magic to an item known to be "boring"
 *
 * Hack -- note the special code for various items
 */
static void a_m_aux_4(object_type *i_ptr, int level, int power)
{
    /* Apply magic (good or bad) according to type */
    switch (i_ptr->tval)
    {
        case TV_LITE:

            /* Hack -- Torches -- random fuel */
            if (i_ptr->sval == SV_LITE_TORCH)
            {
                if (i_ptr->pval) i_ptr->pval = randint(i_ptr->pval);
            }

            /* Hack -- Lanterns -- random fuel */
            if (i_ptr->sval == SV_LITE_LANTERN)
            {
                if (i_ptr->pval) i_ptr->pval = randint(i_ptr->pval);
            }

            break;


        case TV_WAND:

            /* Hack -- charge wands */
            charge_wand(i_ptr);

            break;


        case TV_STAFF:

            /* Hack -- charge staffs */
            charge_staff(i_ptr);

            break;


        case TV_CHEST:

            /* Hack -- skip ruined chests */
            if (k_info[i_ptr->k_idx].level <= 0) break;

            /* Hack -- pick a "difficulty" */
            i_ptr->pval = randint(k_info[i_ptr->k_idx].level);

            /* Never exceed "difficulty" of 55 to 59 */
            if (i_ptr->pval > 55) i_ptr->pval = 55 + rand_int(5);

            break;
    }
}



/*
 * Complete the "creation" of an object by applying "magic" to the item
 *
 * This includes not only rolling for random bonuses, but also putting the
 * finishing touches on ego-items and artifacts, giving charges to wands and
 * staffs, giving fuel to lites, and placing traps on chests.
 *
 * In particular, note that "Instant Artifacts", if "created" by an external
 * routine, must pass through this function to complete the actual creation.
 *
 * The base "chance" of the item being "good" increases with the "level"
 * parameter, which is usually derived from the dungeon level, being equal
 * to the level plus 10, up to a maximum of 75.  If "good" is true, then
 * the object is guaranteed to be "good".  If an object is "good", then
 * the chance that the object will be "great" (ego-item or artifact), also
 * increases with the "level", being equal to half the level, plus 5, up to
 * a maximum of 20.  If "great" is true, then the object is guaranteed to be
 * "great".  At dungeon level 65 and below, 15/100 objects are "great".
 *
 * If the object is not "good", there is a chance it will be "cursed", and
 * if it is "cursed", there is a chance it will be "broken".  These chances
 * are related to the "good" / "great" chances above.
 *
 * Otherwise "normal" rings and amulets will be "good" half the time and
 * "cursed" half the time, unless the ring/amulet is always good or cursed.
 *
 * If "okay" is true, and the object is going to be "great", then there is
 * a chance that an artifact will be created.  This is true even if both the
 * "good" and "great" arguments are false.  As a total hack, if "great" is
 * true, then the item gets 3 extra "attempts" to become an artifact.
 */
void apply_magic(object_type *i_ptr, int lev, bool okay, bool good, bool great)
{
    int i, rolls, f1, f2, power;


    /* Maximum "level" for various things */
    if (lev > MAX_DEPTH - 1) lev = MAX_DEPTH - 1;


    /* Base chance of being "good" */
    f1 = lev + 10;

    /* Maximal chance of being "good" */
    if (f1 > 75) f1 = 75;

    /* Base chance of being "great" */
    f2 = f1 / 2;

    /* Maximal chance of being "great" */
    if (f2 > 20) f2 = 20;


    /* Assume normal */
    power = 0;

    /* Roll for "good" */
    if (good || magik(f1))
    {
        /* Assume "good" */
        power = 1;

        /* Roll for "great" */
        if (great || magik(f2)) power = 2;
    }

    /* Roll for "cursed" */
    else if (magik(f1))
    {
        /* Assume "cursed" */
        power = -1;

        /* Roll for "broken" */
        if (magik(f2)) power = -2;
    }


    /* Assume no rolls */
    rolls = 0;

    /* Get one roll if excellent */
    if (power >= 2) rolls = 1;

    /* Hack -- Get four rolls if forced great */
    if (great) rolls = 4;

    /* Hack -- Get no rolls if not allowed */
    if (!okay || i_ptr->name1) rolls = 0;

    /* Roll for artifacts if allowed */
    for (i = 0; i < rolls; i++)
    {
        /* Roll for an artifact */
        if (make_artifact(i_ptr)) break;
    }


    /* Hack -- analyze artifacts */
    if (i_ptr->name1)
    {
        artifact_type *a_ptr = &a_info[i_ptr->name1];

        /* Hack -- Mark the artifact as "created" */
        a_ptr->cur_num = 1;

        /* Extract the other fields */
        i_ptr->pval = a_ptr->pval;
        i_ptr->ac = a_ptr->ac;
        i_ptr->dd = a_ptr->dd;
        i_ptr->ds = a_ptr->ds;
        i_ptr->to_a = a_ptr->to_a;
        i_ptr->to_h = a_ptr->to_h;
        i_ptr->to_d = a_ptr->to_d;
        i_ptr->weight = a_ptr->weight;

        /* Hack -- extract the "broken" flag */
        if (!a_ptr->cost) i_ptr->ident |= ID_BROKEN;

        /* Hack -- extract the "cursed" flag */
        if (a_ptr->flags3 & TR3_CURSED) i_ptr->ident |= ID_CURSED;

        /* Mega-Hack -- increase the rating */
        rating += 10;

        /* Mega-Hack -- increase the rating again */
        if (a_ptr->cost > 50000L) rating += 10;

        /* Set the good item flag */
        good_item_flag = TRUE;

        /* Cheat -- peek at the item */
        if (cheat_peek) object_mention(i_ptr);

        /* Done */
        return;
    }


    /* Apply magic */
    switch (i_ptr->tval)
    {
        case TV_DIGGING:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_SWORD:
        case TV_BOW:
        case TV_SHOT:
        case TV_ARROW:
        case TV_BOLT:
            if (power) a_m_aux_1(i_ptr, lev, power);
            break;

        case TV_DRAG_ARMOR:
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
        case TV_SHIELD:
        case TV_HELM:
        case TV_CROWN:
        case TV_CLOAK:
        case TV_GLOVES:
        case TV_BOOTS:
            if (power) a_m_aux_2(i_ptr, lev, power);
            break;

        case TV_RING:
        case TV_AMULET:
            if (!power && (rand_int(100) < 50)) power = -1;
            a_m_aux_3(i_ptr, lev, power);
            break;

        default:
            a_m_aux_4(i_ptr, lev, power);
            break;
    }


    /* Hack -- analyze ego-items */
    if (i_ptr->name2)
    {
        ego_item_type *e_ptr = &e_info[i_ptr->name2];

        /* Hack -- extra powers */
        switch (i_ptr->name2)
        {
            /* Weapon (Holy Avenger) */
            case EGO_HA:
                i_ptr->xtra1 = EGO_XTRA_SUSTAIN;
                break;

            /* Weapon (Defender) */
            case EGO_DF:
                i_ptr->xtra1 = EGO_XTRA_SUSTAIN;
                break;

            /* Weapon (Blessed) */
            case EGO_BLESS_BLADE:
                i_ptr->xtra1 = EGO_XTRA_ABILITY;
                break;

            /* Robe of Permanance */
            case EGO_PERMANENCE:
                i_ptr->xtra1 = EGO_XTRA_POWER;
                break;

            /* Armor of Elvenkind */
            case EGO_ELVENKIND:
                i_ptr->xtra1 = EGO_XTRA_POWER;
                break;

            /* Crown of the Magi */
            case EGO_MAGI:
                i_ptr->xtra1 = EGO_XTRA_ABILITY;
                break;

            /* Cloak of Aman */
            case EGO_AMAN:
                i_ptr->xtra1 = EGO_XTRA_POWER;
                break;
        }

        /* Randomize the "xtra" power */
        if (i_ptr->xtra1) i_ptr->xtra2 = randint(256);

        /* Hack -- acquire "broken" flag */
        if (!e_ptr->cost) i_ptr->ident |= ID_BROKEN;

        /* Hack -- acquire "cursed" flag */
        if (e_ptr->flags3 & TR3_CURSED) i_ptr->ident |= ID_CURSED;

        /* Hack -- apply extra penalties if needed */
        if (cursed_p(i_ptr) || broken_p(i_ptr))
        {
            /* Hack -- obtain bonuses */
            if (e_ptr->max_to_h) i_ptr->to_h -= randint(e_ptr->max_to_h);
            if (e_ptr->max_to_d) i_ptr->to_d -= randint(e_ptr->max_to_d);
            if (e_ptr->max_to_a) i_ptr->to_a -= randint(e_ptr->max_to_a);

            /* Hack -- obtain pval */
            if (e_ptr->max_pval) i_ptr->pval -= randint(e_ptr->max_pval);
        }

        /* Hack -- apply extra bonuses if needed */
        else
        {
            /* Hack -- obtain bonuses */
            if (e_ptr->max_to_h) i_ptr->to_h += randint(e_ptr->max_to_h);
            if (e_ptr->max_to_d) i_ptr->to_d += randint(e_ptr->max_to_d);
            if (e_ptr->max_to_a) i_ptr->to_a += randint(e_ptr->max_to_a);

            /* Hack -- obtain pval */
            if (e_ptr->max_pval) i_ptr->pval += randint(e_ptr->max_pval);
        }

        /* Hack -- apply rating bonus */
        rating += e_ptr->rating;

        /* Cheat -- describe the item */
        if (cheat_peek) object_mention(i_ptr);

        /* Done */
        return;
    }


    /* Examine real objects */
    if (i_ptr->k_idx)
    {
        object_kind *k_ptr = &k_info[i_ptr->k_idx];

        /* Hack -- acquire "broken" flag */
        if (!k_ptr->cost) i_ptr->ident |= ID_BROKEN;

        /* Hack -- acquire "cursed" flag */
        if (k_ptr->flags3 & TR3_CURSED) i_ptr->ident |= ID_CURSED;
    }
}



/*
 * Hack -- determine if a template is "good"
 */
static bool kind_is_good(int k_idx)
{
    object_kind *k_ptr = &k_info[k_idx];

    /* Analyze the item type */
    switch (k_ptr->tval)
    {
        /* Armor -- Good unless damaged */
        case TV_HARD_ARMOR:
        case TV_SOFT_ARMOR:
        case TV_DRAG_ARMOR:
        case TV_SHIELD:
        case TV_CLOAK:
        case TV_BOOTS:
        case TV_GLOVES:
        case TV_HELM:
        case TV_CROWN:
            if (k_ptr->to_a < 0) return (FALSE);
            return (TRUE);

        /* Weapons -- Good unless damaged */
        case TV_BOW:
        case TV_SWORD:
        case TV_HAFTED:
        case TV_POLEARM:
        case TV_DIGGING:
            if (k_ptr->to_h < 0) return (FALSE);
            if (k_ptr->to_d < 0) return (FALSE);
            return (TRUE);

        /* Ammo -- Arrows/Bolts are good unless damaged*/
        case TV_BOLT:
        case TV_ARROW:
            if (k_ptr->to_h < 0) return (FALSE);
            if (k_ptr->to_d < 0) return (FALSE);
            return (TRUE);

        /* Books -- High level books are good */
        case TV_MAGIC_BOOK:
            return (k_ptr->sval >= SV_MBOOK_MIN_GOOD);
        case TV_PRAYER_BOOK:
            return (k_ptr->sval >= SV_PBOOK_MIN_GOOD);

        /* Rings -- Rings of Speed are good */
        case TV_RING:
            return (k_ptr->sval == SV_RING_SPEED);

        /* Amulets -- Amulets of the Magi are good */
        case TV_AMULET:
            return (k_ptr->sval == SV_AMULET_THE_MAGI);
    }

    /* Assume not good */
    return (FALSE);
}



/*
 * Attempt to place an object (normal or good/great) at the given location.
 *
 * This routine plays nasty games to generate the "special artifacts".
 *
 * This routine uses "object_level" for the "generation level".
 *
 * This routine requires a clean floor grid destination.
 */
void place_object(int y, int x, bool good, bool great)
{
    int			i_idx, prob, base;

    int			old = rating;

    object_type		forge;


    /* Paranoia -- check bounds */
    if (!in_bounds(y, x)) return;

    /* Require clean floor space */
    if (!clean_grid_bold(y, x)) return;


    /* Chance of "special object" */
    prob = (good ? 10 : 1000);

    /* Base level for the object */
    base = (good ? (object_level + 10) : object_level);


    /* Hack -- clear out the forgery */
    invwipe(&forge);

    /* Generate a special object, or a normal object */
    if ((rand_int(prob) != 0) || !make_artifact_special(&forge))
    {
        int k_idx;

        /* Require "good" object */
        if (good) get_obj_num_hook = kind_is_good;

        /* Pick a random object */
        k_idx = get_obj_num(base);

        /* Hack -- forget the hook */
        get_obj_num_hook = NULL;

        /* Handle failure */
        if (!k_idx) return;

        /* Prepare the object */
        invcopy(&forge, k_idx);
    }

    /* Apply magic (allow artifacts) */
    apply_magic(&forge, object_level, TRUE, good, great);

    /* Hack -- generate multiple spikes/missiles */
    switch (forge.tval)
    {
        case TV_SPIKE:
        case TV_SHOT:
        case TV_ARROW:
        case TV_BOLT:
            forge.number = damroll(6,7);
    }


    /* Make an object */
    i_idx = i_pop();

    /* Success */
    if (i_idx)
    {
        cave_type		*c_ptr;
        object_type		*i_ptr;

        i_ptr = &i_list[i_idx];

        (*i_ptr) = (forge);

        i_ptr->iy = y;
        i_ptr->ix = x;

        c_ptr = &cave[y][x];
        c_ptr->i_idx = i_idx;

        /* Notice "okay" out-of-depth objects (unless already noticed) */
        if (!cursed_p(i_ptr) && !broken_p(i_ptr) &&
            (rating == old) && (k_info[i_ptr->k_idx].level > dun_level))
        {
            /* Rating increase */
            rating += (k_info[i_ptr->k_idx].level - dun_level);

            /* Cheat -- peek at items */
            if (cheat_peek) object_mention(i_ptr);
        }
    }
}



/*
 * Scatter some "great" objects near the player
 */
void acquirement(int y1, int x1, int num, bool great)
{
    int        y, x, i, d;

    /* Scatter some objects */
    for (; num > 0; --num)
    {
        /* Check near the player for space */
        for (i = 0; i < 25; ++i)
        {
            /* Increasing Distance */
            d = (i + 4) / 5;

            /* Pick a location */
            scatter(&y, &x, y1, x1, d, 0);

            /* Must have a clean grid */
            if (!clean_grid_bold(y, x)) continue;

            /* Place a good (or great) object */
            place_object(y, x, TRUE, great);

            /* Notice */
            note_spot(y, x);

            /* Redraw */
            lite_spot(y, x);

            /* Under the player */
            if ((y == py) && (x == px))
            {
                /* Message */
                msg_print ("You feel something roll beneath your feet.");
            }

            /* Placement accomplished */
            break;
        }
    }
}





/*
 * Places a random trap at the given location.
 *
 * The location must be a valid, empty, clean, floor grid.
 *
 * Note that all traps start out as "invisible" and "untyped", and then
 * when they are "discovered" (by detecting them or setting them off),
 * the trap is "instantiated" as a visible, "typed", trap.
 */
void place_trap(int y, int x)
{
    cave_type *c_ptr;

    /* Paranoia -- verify location */
    if (!in_bounds(y, x)) return;

    /* Require empty, clean, floor grid */
    if (!naked_grid_bold(y, x)) return;

    /* Access the grid */
    c_ptr = &cave[y][x];

    /* Place an invisible trap */
    c_ptr->ftyp = 0x02;
}


/*
 * XXX XXX XXX Do not use these hard-coded values.
 */
#define OBJ_GOLD_LIST	480	/* First "gold" entry */
#define MAX_GOLD	18	/* Number of "gold" entries */

/*
 * Places a treasure (Gold or Gems) at given location
 * The location must be a valid, empty, floor grid.
 */
void place_gold(int y, int x)
{
    int		i, i_idx;

    s32b	base;

    cave_type	*c_ptr;
    object_type	*i_ptr;


    /* Paranoia -- check bounds */
    if (!in_bounds(y, x)) return;

    /* Require clean floor grid */
    if (!clean_grid_bold(y, x)) return;


    /* Hack -- Pick a Treasure variety */
    i = ((randint(object_level + 2) + 2) / 2) - 1;

    /* Apply "extra" magic */
    if (rand_int(GREAT_OBJ) == 0)
    {
        i += randint(object_level + 1);
    }

    /* Hack -- Creeping Coins only generate "themselves" */
    if (coin_type) i = coin_type;

    /* Do not create "illegal" Treasure Types */
    if (i >= MAX_GOLD) i = MAX_GOLD - 1;


    /* Make an object */
    i_idx = i_pop();

    /* Success */
    if (i_idx)
    {
        i_ptr = &i_list[i_idx];

        invcopy(i_ptr, OBJ_GOLD_LIST + i);

        i_ptr->iy = y;
        i_ptr->ix = x;

        c_ptr = &cave[y][x];
        c_ptr->i_idx = i_idx;

        /* Hack -- Base coin cost */
        base = k_info[OBJ_GOLD_LIST+i].cost;

        /* Determine how much the treasure is "worth" */
        i_ptr->pval = (base + (8L * randint(base)) + randint(8));
    }
}



/*
 * Let an item 'i_ptr' fall to the ground at or near (y,x).
 * The initial location is assumed to be "in_bounds()".
 *
 * This function takes a parameter "chance".  This is the percentage
 * chance that the item will "disappear" instead of drop.  If the object
 * has been thrown, then this is the chance of disappearance on contact.
 *
 * Hack -- this function uses "chance" to determine if it should produce
 * some form of "description" of the drop event (under the player).
 *
 * This function should probably be broken up into a function to determine
 * a "drop location", and several functions to actually "drop" an object.
 *
 * XXX XXX XXX Consider allowing objects to combine on the ground.
 */
void drop_near(object_type *i_ptr, int chance, int y, int x)
{
    int		k, d, ny, nx, y1, x1, i_idx;

    cave_type	*c_ptr;

    bool flag = FALSE;


    /* Start at the drop point */
    ny = y1 = y;  nx = x1 = x;

    /* See if the object "survives" the fall */
    if (artifact_p(i_ptr) || (rand_int(100) >= chance))
    {
        /* Start at the drop point */
        ny = y1 = y; nx = x1 = x;

        /* Try (20 times) to find an adjacent usable location */
        for (k = 0; !flag && (k < 20); ++k)
        {
            /* Distance distribution */
            d = ((k + 14) / 15);

            /* Pick a "nearby" location */
            scatter(&ny, &nx, y1, x1, d, 0);

            /* Require clean floor space */
            if (!clean_grid_bold(ny, nx)) continue;

            /* Here looks good */
            flag = TRUE;
        }
    }

    /* Try really hard to place an artifact */
    if (!flag && artifact_p(i_ptr))
    {
        /* Start at the drop point */
        ny = y1 = y;  nx = x1 = x;

        /* Try really hard to drop it */
        for (k = 0; !flag && (k < 1000); k++)
        {
            d = 1;

            /* Pick a location */
            scatter(&ny, &nx, y1, x1, d, 0);

            /* Do not move through walls */
            if (!floor_grid_bold(ny,nx)) continue;

            /* Hack -- "bounce" to that location */
            y1 = ny; x1 = nx;

            /* Get the cave grid */
            c_ptr = &cave[ny][nx];

            /* XXX XXX XXX */

            /* Nothing here?  Use it */
            if (!(c_ptr->i_idx)) flag = TRUE;

            /* After trying 99 places, crush any (normal) object */
            else if ((k>99) && valid_grid(ny,nx)) flag = TRUE;
        }

        /* Hack -- Artifacts will destroy ANYTHING to stay alive */
        if (!flag)
        {
            char i_name[80];

            /* Location */
            ny = y;
            nx = x;

            /* Always okay */
            flag = TRUE;

            /* Description */
            object_desc(i_name, i_ptr, FALSE, 0);

            /* Message */
            msg_format("The %s crashes to the floor.", i_name);
        }
    }


    /* Successful drop */
    if (flag)
    {
        /* Assume fails */
        flag = FALSE;

        /* XXX XXX XXX */

        /* Crush anything under us (for artifacts) */
        delete_object(ny,nx);

        /* Make a new object */
        i_idx = i_pop();

        /* Success */
        if (i_idx)
        {
            /* Structure copy */
            i_list[i_idx] = *i_ptr;

            /* Access */
            i_ptr = &i_list[i_idx];

            /* Locate */
            i_ptr->iy = ny;
            i_ptr->ix = nx;

            /* Place */
            c_ptr = &cave[ny][nx];
            c_ptr->i_idx = i_idx;

            /* Note the spot */
            note_spot(ny, nx);

            /* Draw the spot */
            lite_spot(ny, nx);

            /* Sound */
            sound(SOUND_DROP);

            /* Mega-Hack -- no message if "dropped" by player */
            /* Message when an object falls under the player */
            if (chance && (ny == py) && (nx == px))
            {
                msg_print("You feel something roll beneath your feet.");
            }

            /* Success */
            flag = TRUE;
        }
    }


    /* Poor little object */
    if (!flag)
    {
        char i_name[80];

        /* Describe */
        object_desc(i_name, i_ptr, FALSE, 0);

        /* Message */
        msg_format("The %s disappear%s.",
                   i_name, ((i_ptr->number == 1) ? "s" : ""));
    }
}




/*
 * Hack -- instantiate a trap
 *
 * XXX XXX XXX This routine should be redone to reflect trap "level".
 * That is, it does not make sense to have spiked pits at 50 feet.
 * Actually, it is not this routine, but the "trap instantiation"
 * code, which should also check for "trap doors" on quest levels.
 */
void pick_trap(int y, int x)
{
    int f;

    cave_type *c_ptr = &cave[y][x];

    /* Paranoia -- Verify terrain */
    if (c_ptr->ftyp != 0x02) return;

    /* Pick a trap */
    while (1)
    {
        /* Hack -- pick a trap */
        f = 0x10 + rand_int(16);

        /* Hack -- no trap doors on quest levels */
        if ((f == 0x10) && is_quest(dun_level)) continue;

        /* Hack -- no trap doors on the deepest level */
        if ((f == 0x10) && (dun_level >= MAX_DEPTH-1)) continue;

        /* Done */
        break;
    }

    /* Activate the trap */
    c_ptr->ftyp = f;

    /* Notice */
    note_spot(y, x);

    /* Redraw */
    lite_spot(y, x);
}




/*
 * Describe the charges on an item in the inventory.
 */
void inven_item_charges(int item)
{
    object_type *i_ptr = &inventory[item];

    /* Require staff/wand */
    if ((i_ptr->tval != TV_STAFF) && (i_ptr->tval != TV_WAND)) return;

    /* Require known item */
    if (!object_known_p(i_ptr)) return;

    /* Multiple charges */
    if (i_ptr->pval != 1)
    {
        /* Print a message */
        msg_format("You have %d charges remaining.", i_ptr->pval);
    }

    /* Single charge */
    else
    {
        /* Print a message */
        msg_format("You have %d charge remaining.", i_ptr->pval);
    }
}


/*
 * Describe an item in the inventory.
 */
void inven_item_describe(int item)
{
    object_type	*i_ptr = &inventory[item];

    char	i_name[80];

    /* Get a description */
    object_desc(i_name, i_ptr, TRUE, 3);

    /* Print a message */
    msg_format("You have %s.", i_name);
}


/*
 * Increase the "number" of an item in the inventory
 */
void inven_item_increase(int item, int num)
{
    object_type *i_ptr = &inventory[item];

    /* Apply */
    num += i_ptr->number;

    /* Bounds check */
    if (num > 255) num = 255;
    else if (num < 0) num = 0;

    /* Un-apply */
    num -= i_ptr->number;

    /* Change the number and weight */
    if (num)
    {
        /* Add the number */
        i_ptr->number += num;

        /* Add the weight */
        total_weight += (num * i_ptr->weight);

        /* Redraw the choice window */
        p_ptr->redraw |= (PR_CHOOSE);

        /* Recalculate bonuses */
        p_ptr->update |= (PU_BONUS);

        /* Recalculate mana XXX */
        p_ptr->update |= (PU_MANA);

        /* Combine the pack */
        p_ptr->notice |= (PN_COMBINE);
    }
}


/*
 * Erase an inventory slot if it has no more items
 */
void inven_item_optimize(int item)
{
    object_type *i_ptr = &inventory[item];

    /* Only optimize real items */
    if (!i_ptr->k_idx) return;

    /* Only optimize empty items */
    if (i_ptr->number) return;

    /* The item is in the pack */
    if (item < INVEN_WIELD)
    {
        int i;

        /* One less item */
        inven_cnt--;

        /* Slide everything down */
        for (i = item; i < INVEN_PACK; i++)
        {
            /* Structure copy */
            inventory[i] = inventory[i+1];
        }

        /* Erase the "final" slot */
        invwipe(&inventory[i]);
    }

    /* The item is being wielded */
    else
    {
        /* One less item */
        equip_cnt--;

        /* Erase the empty slot */
        invwipe(&inventory[item]);

        /* Recalculate bonuses */
        p_ptr->update |= (PU_BONUS);

        /* Recalculate torch */
        p_ptr->update |= (PU_TORCH);

        /* Recalculate mana XXX */
        p_ptr->update |= (PU_MANA);
    }

    /* Redraw the choice window */
    p_ptr->redraw |= (PR_CHOOSE);
}


/*
 * Describe the charges on an item on the floor.
 */
void floor_item_charges(int item)
{
    object_type *i_ptr = &i_list[item];

    /* Require staff/wand */
    if ((i_ptr->tval != TV_STAFF) && (i_ptr->tval != TV_WAND)) return;

    /* Require known item */
    if (!object_known_p(i_ptr)) return;

    /* Multiple charges */
    if (i_ptr->pval != 1)
    {
        /* Print a message */
        msg_format("There are %d charges remaining.", i_ptr->pval);
    }

    /* Single charge */
    else
    {
        /* Print a message */
        msg_format("There is %d charge remaining.", i_ptr->pval);
    }
}



/*
 * Describe an item in the inventory.
 */
void floor_item_describe(int item)
{
    object_type	*i_ptr = &i_list[item];

    char	i_name[80];

    /* Get a description */
    object_desc(i_name, i_ptr, TRUE, 3);

    /* Print a message */
    msg_format("You see %s.", i_name);
}


/*
 * Increase the "number" of an item on the floor
 */
void floor_item_increase(int item, int num)
{
    object_type *i_ptr = &i_list[item];

    /* Apply */
    num += i_ptr->number;

    /* Bounds check */
    if (num > 255) num = 255;
    else if (num < 0) num = 0;

    /* Un-apply */
    num -= i_ptr->number;

    /* Change the number */
    i_ptr->number += num;
}


/*
 * Optimize an item on the floor (destroy "empty" items)
 */
void floor_item_optimize(int item)
{
    object_type *i_ptr = &i_list[item];

    /* Paranoia -- be sure it exists */
    if (!i_ptr->k_idx) return;

    /* Only optimize empty items */
    if (i_ptr->number) return;

    /* Delete it */
    delete_object_idx(item);
}





/*
 * Check if we have space for an item in the pack without overflow
 */
bool inven_carry_okay(object_type *i_ptr)
{
    int i;

    /* Empty slot? */
    if (inven_cnt < INVEN_PACK) return (TRUE);

    /* Similar slot? */
    for (i = 0; i < INVEN_PACK; i++)
    {
        /* Get that item */
        object_type *j_ptr = &inventory[i];

        /* Check if the two items can be combined */
        if (object_similar(j_ptr, i_ptr)) return (TRUE);
    }

    /* Nope */
    return (FALSE);
}


/*
 * Add an item to the players inventory, and return the slot used.
 *
 * If the new item can combine with an existing item in the inventory,
 * it will do so, using "object_similar()" and "object_absorb()", otherwise,
 * the item will be placed into the "proper" location in the inventory.
 *
 * This function can be used to "over-fill" the player's pack, but only
 * once, and such an action must trigger the "overflow" code immediately.
 * Note that when the pack is being "over-filled", the new item must be
 * placed into the "overflow" slot, and the "overflow" must take place
 * before the pack is reordered, but (optionally) after the pack is
 * combined.  This may be tricky.  See "dungeon.c" for info.
 */
s16b inven_carry(object_type *i_ptr)
{
    int         i, j, k;
    int		n = -1;

    object_type	*j_ptr;


    /* Check for combining */
    for (j = 0; j < INVEN_PACK; j++)
    {
        j_ptr = &inventory[j];

        /* Skip empty items */
        if (!j_ptr->k_idx) continue;

        /* Hack -- track last item */
        n = j;

        /* Check if the two items can be combined */
        if (object_similar(j_ptr, i_ptr))
        {
            /* Combine the items */
            object_absorb(j_ptr, i_ptr);

            /* Increase the weight */
            total_weight += (i_ptr->number * i_ptr->weight);

            /* Redraw the choice window */
            p_ptr->redraw |= (PR_CHOOSE);

            /* Recalculate bonuses */
            p_ptr->update |= (PU_BONUS);

            /* Success */
            return (j);
        }
    }


    /* Paranoia */
    if (inven_cnt > INVEN_PACK) return (-1);


    /* Find an empty slot */
    for (j = 0; j <= INVEN_PACK; j++)
    {
        j_ptr = &inventory[j];

        /* Use it if found */
        if (!j_ptr->k_idx) break;
    }

    /* Use that slot */
    i = j;


    /* Hack -- pre-reorder the pack */
    if (i < INVEN_PACK)
    {
        s32b		i_value, j_value;

        /* Get the "value" of the item */
        i_value = object_value(i_ptr);

        /* Scan every occupied slot */
        for (j = 0; j < INVEN_PACK; j++)
        {
            j_ptr = &inventory[j];

            /* Use empty slots */
            if (!j_ptr->k_idx) break;

            /* Hack -- readable books always come first */
            if ((i_ptr->tval == mp_ptr->spell_book) &&
                (j_ptr->tval != mp_ptr->spell_book)) break;
            if ((j_ptr->tval == mp_ptr->spell_book) &&
                (i_ptr->tval != mp_ptr->spell_book)) continue;

            /* Objects sort by decreasing type */
            if (i_ptr->tval > j_ptr->tval) break;
            if (i_ptr->tval < j_ptr->tval) continue;

            /* Non-aware (flavored) items always come last */
            if (!object_aware_p(i_ptr)) continue;
            if (!object_aware_p(j_ptr)) break;

            /* Objects sort by increasing sval */
            if (i_ptr->sval < j_ptr->sval) break;
            if (i_ptr->sval > j_ptr->sval) continue;

            /* Unidentified objects always come last */
            if (!object_known_p(i_ptr)) continue;
            if (!object_known_p(j_ptr)) break;

	    /* Hack - otherwise identical rods sort by increasing
	       recharge time  --dsb */
            if (i_ptr->tval == TV_ROD) {
            	if (i_ptr->pval < j_ptr->pval) break;
            	if (i_ptr->pval > j_ptr->pval) continue;
            }

            /* Determine the "value" of the pack item */
            j_value = object_value(j_ptr);

            /* Objects sort by decreasing value */
            if (i_value > j_value) break;
            if (i_value < j_value) continue;
        }

        /* Use that slot */
        i = j;

        /* Structure slide (make room) */
        for (k = n; k >= i; k--)
        {
            /* Hack -- Slide the item */
            inventory[k+1] = inventory[k];
        }

	/* Paranoia -- Wipe the new slot */
        invwipe(&inventory[i]);
    }


    /* Structure copy to insert the new item */
    inventory[i] = (*i_ptr);

    /* Forget the old location */
    inventory[i].iy = inventory[i].ix = 0;

    /* Increase the weight, prepare to redraw */
    total_weight += (i_ptr->number * i_ptr->weight);

    /* Count the items */
    inven_cnt++;

    /* Redraw choice window */
    p_ptr->redraw |= (PR_CHOOSE);

    /* Recalculate bonuses */
    p_ptr->update |= (PU_BONUS);

    /* Reorder pack */
    p_ptr->notice |= (PN_REORDER);

    /* Return the slot */
    return (i);
}




/*
 * Combine items in the pack
 *
 * Note special handling of the "overflow" slot
 */
void combine_pack(void)
{
    int		i, j;

    object_type	*i_ptr;
    object_type	*j_ptr;

    bool	flag = FALSE;


    /* Combine the pack (backwards) */
    for (i = INVEN_PACK; i > 0; i--)
    {
        /* Get the item */
        i_ptr = &inventory[i];

        /* Skip empty items */
        if (!i_ptr->k_idx) continue;

        /* Scan the items above that item */
        for (j = 0; j < i; j++)
        {
            /* Get the item */
            j_ptr = &inventory[j];

            /* Skip empty items */
            if (!j_ptr->k_idx) continue;

            /* Can we drop "i_ptr" onto "j_ptr"? */
            if (object_similar(j_ptr, i_ptr))
            {
		/* Take note */
		flag = TRUE;

                /* Add together the item counts */
                object_absorb(j_ptr, i_ptr);

                /* One object is gone */
                inven_cnt--;

                /* Erase the last object */
                invwipe(&inventory[i]);

                /* Redraw the choice window */
                p_ptr->redraw |= (PR_CHOOSE);

                /* XXX XXX XXX Reorder the pack */
                p_ptr->notice |= (PN_REORDER);

		/* Done */
		break;
            }
        }
    }

    /* Message */
    if (flag) msg_print("You combine some items in your pack.");
}


/*
 * Reorder items in the pack
 *
 * Note special handling of the "overflow" slot
 *
 * Note special handling of empty slots  XXX XXX XXX XXX
 */
void reorder_pack(void)
{
    int		i, j, k;

    s32b	i_value;
    s32b	j_value;

    object_type *i_ptr;
    object_type *j_ptr;

    object_type	temp;

    bool	flag = FALSE;


    /* Re-order the pack (forwards) */
    for (i = 0; i < INVEN_PACK; i++)
    {
	/* Mega-Hack -- allow "proper" over-flow */
	if ((i == INVEN_PACK) && (inven_cnt == INVEN_PACK)) break;

        /* Get the item */
        i_ptr = &inventory[i];

        /* Skip empty slots */
        if (!i_ptr->k_idx) continue;

        /* Get the "value" of the item */
        i_value = object_value(i_ptr);

        /* Scan every occupied slot */
        for (j = 0; j < INVEN_PACK; j++)
        {
            /* Get the item already there */
            j_ptr = &inventory[j];

            /* Use empty slots */
            if (!j_ptr->k_idx) break;

            /* Hack -- readable books always come first */
            if ((i_ptr->tval == mp_ptr->spell_book) &&
                (j_ptr->tval != mp_ptr->spell_book)) break;
            if ((j_ptr->tval == mp_ptr->spell_book) &&
                (i_ptr->tval != mp_ptr->spell_book)) continue;

            /* Objects sort by decreasing type */
            if (i_ptr->tval > j_ptr->tval) break;
            if (i_ptr->tval < j_ptr->tval) continue;

            /* Non-aware (flavored) items always come last */
            if (!object_aware_p(i_ptr)) continue;
            if (!object_aware_p(j_ptr)) break;

            /* Objects sort by increasing sval */
            if (i_ptr->sval < j_ptr->sval) break;
            if (i_ptr->sval > j_ptr->sval) continue;

            /* Unidentified objects always come last */
            if (!object_known_p(i_ptr)) continue;
            if (!object_known_p(j_ptr)) break;

	    /* Hack -- otherwise identical rods sort by increasing
	       recharge time  --dsb */
            if (i_ptr->tval == TV_ROD)
            {
            	if (i_ptr->pval < j_ptr->pval) break;
            	if (i_ptr->pval > j_ptr->pval) continue;
            }

            /* Determine the "value" of the pack item */
            j_value = object_value(j_ptr);

            /* Objects sort by decreasing value */
            if (i_value > j_value) break;
            if (i_value < j_value) continue;
        }

        /* Never move down */
        if (j >= i) continue;

        /* Take note */
        flag = TRUE;

        /* Save the moving item */
        temp = inventory[i];

        /* Structure slide (make room) */
        for (k = i; k > j; k--)
        {
            /* Slide the item */
            inventory[k] = inventory[k-1];
        }

	/* Insert the moved item */
	inventory[j] = temp;

        /* Redraw the choice window */
        p_ptr->redraw |= (PR_CHOOSE);
    }

    /* Message */
    if (flag) msg_print("You reorder some items in your pack.");
}




/*
 * Hack -- process the objects
 */
void process_objects(void)
{
    int i, k;

    object_type *i_ptr;


    /* Hack -- only every ten game turns */
    if ((turn % 10) != 5) return;


    /* Process objects */
    for (k = i_top - 1; k >= 0; k--)
    {
        /* Access index */
        i = i_fast[k];

        /* Access object */
        i_ptr = &i_list[i];

        /* Excise dead objects */
        if (!i_ptr->k_idx)
        {
            /* Excise it */
            i_fast[k] = i_fast[--i_top];

            /* Skip */
            continue;
        }

        /* Recharge rods on the ground */
        if ((i_ptr->tval == TV_ROD) && (i_ptr->pval)) i_ptr->pval--;
    }


#ifdef SHIMMER_OBJECTS

#if 0
    /* Optimize */
    if (scan_objects)
    {
        /* Process the objects */
        for (i = 1; i < i_max; i++)
        {
            i_ptr = &i_list[i];

            /* Skip dead objects */
            if (!i_ptr->k_idx) continue;

            /* XXX XXX XXX Skip unseen objects */

            /* Shimmer Multi-Hued Objects XXX XXX XXX */
            lite_spot(i_ptr->iy, i_ptr->ix);
        }

        /* Clear the flag */
        scan_objects = FALSE;
    }
#endif

#endif

}

