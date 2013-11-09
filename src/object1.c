/* File: object1.c */

/* Purpose: Object code, part 1 */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#if defined(MACINTOSH) || defined(MACH_O_CARBON)
#ifdef verify
#undef verify
#endif
#endif
/*
 * Reset the "visual" lists
 *
 * This involves resetting various things to their "default" state.
 *
 * If the "prefs" flag is TRUE, then we will also load the appropriate
 * "user pref file" based on the current setting of the "use_graphics"
 * flag.  This is useful for switching "graphics" on/off.
 *
 * The features, objects, and monsters, should all be encoded in the
 * relevant "font.prf" and/or "graf.prf" files.  XXX XXX XXX
 *
 * The "prefs" parameter is no longer meaningful.  XXX XXX XXX
 */
void reset_visuals(void)
{
	int i;

	/* Extract some info about terrain features */
	for (i = 0; i < max_f_idx; i++)
	{
		feature_type *f_ptr = &f_info[i];

		/* Assume we will use the underlying values */
		f_ptr->x_attr = f_ptr->d_attr;
		f_ptr->x_char = f_ptr->d_char;
	}

	/* Extract default attr/char code for objects */
	for (i = 0; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Default attr/char */
		k_ptr->x_attr = k_ptr->d_attr;
		k_ptr->x_char = k_ptr->d_char;
	}

	/* Extract default attr/char code for monsters */
	for (i = 0; i < (max_r_idx + runeweapon_num); i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Default attr/char */
		r_ptr->x_attr = r_ptr->d_attr;
		r_ptr->x_char = r_ptr->d_char;
	}

	/* Normal symbols */
	{
		char buf[1024];

		/* Process "font.prf" */
		process_pref_file("font.prf");

		/* Access the "character" pref file */
		sprintf(buf, "font-%s.prf", player_base);

		/* Process "font-<playername>.prf" */
		process_pref_file(buf);
	}
}


/*
 * Obtain the "flags" for an item
 */
void object_flags(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE])
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	int i;

	/* Base object */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = k_ptr->flags[i];

	/* Artifact */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] = a_ptr->flags[i];
	}

	/* Ego-item */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= e_ptr->flags[i];
	}

	/* Random artifact ! */
	for (i = 0; i < A_MAX; i++)
		if (o_ptr->to_stat[i]) add_flag(flgs, a_to_tr[i]);
	for (i = 0; i < OB_MAX; i++)
		if (o_ptr->to_misc[i]) add_flag(flgs, ob_to_tr[i]);
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] |= o_ptr->art_flags[i];

	if ((o_ptr->tval >= TV_BULLET) && (o_ptr->tval <= TV_SHELL) && o_ptr->xtra3)
	{
		int add = o_ptr->xtra3 - 1;

		if (add < TR_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
	}

	/* Remove flags from empty lites */
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval <= SV_LITE_LANTERN) && !o_ptr->xtra4)
	{
		switch (o_ptr->name2)
		{
		case EGO_LITE_AURA_FIRE:
			remove_flag(flgs, TR_SH_FIRE);
			break;
		case EGO_LITE_INFRA:
			remove_flag(flgs, TR_INFRA);
			break;
		case EGO_LITE_EYE:
			remove_flag(flgs, TR_RES_BLIND);
			remove_flag(flgs, TR_SEE_INVIS);
			break;
		}
	}
}



/*
 * Obtain the "flags" for an item which are known to the player
 */
void object_flags_known(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE])
{
	bool spoil = FALSE;
	int i;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Clear */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = 0L;

	if (!object_aware_p(o_ptr)) return;

	/* Base object */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		flgs[i] = k_ptr->flags[i];

	/* Must be identified */
	if (!object_known_p(o_ptr)) return;

	/* Ego-item (known basic flags) */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= e_ptr->flags[i];
	}


#ifdef SPOIL_ARTIFACTS
	/* Full knowledge for some artifacts */
	if (artifact_p(o_ptr) || o_ptr->art_name) spoil = TRUE;
#endif /* SPOIL_ARTIFACTS */

#ifdef SPOIL_EGO_ITEMS
	/* Full knowledge for some ego-items */
	if (ego_item_p(o_ptr)) spoil = TRUE;
#endif /* SPOIL_EGO_ITEMS */

	/* Need full knowledge or spoilers */
	if (spoil || (o_ptr->ident & IDENT_MENTAL))
	{
		/* Artifact */
		if (o_ptr->name1)
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			for (i = 0; i < TR_FLAG_SIZE; i++)
				flgs[i] = a_ptr->flags[i];
		}

		/* Random artifact ! */
		for (i = 0; i < A_MAX; i++)
			if (o_ptr->to_stat[i]) add_flag(flgs, a_to_tr[i]);
		for (i = 0; i < OB_MAX; i++)
			if (o_ptr->to_misc[i]) add_flag(flgs, ob_to_tr[i]);
		for (i = 0; i < TR_FLAG_SIZE; i++)
			flgs[i] |= o_ptr->art_flags[i];
	}

	if ((o_ptr->tval >= TV_BULLET) && (o_ptr->tval <= TV_SHELL) && o_ptr->xtra3)
	{
		int add = o_ptr->xtra3 - 1;

		if (add < TR_FLAG_MAX)
		{
			add_flag(flgs, add);
		}
	}

	/* Remove flags from empty lites */
	if ((o_ptr->tval == TV_LITE) && (o_ptr->sval <= SV_LITE_LANTERN) && !o_ptr->xtra4)
	{
		switch (o_ptr->name2)
		{
		case EGO_LITE_AURA_FIRE:
			remove_flag(flgs, TR_SH_FIRE);
			break;
		case EGO_LITE_INFRA:
			remove_flag(flgs, TR_INFRA);
			break;
		case EGO_LITE_EYE:
			remove_flag(flgs, TR_RES_BLIND);
			remove_flag(flgs, TR_SEE_INVIS);
			break;
		}
	}
}


/*
 * Obtain the "Ethnicity" of an object
 */
int get_object_ethnicity(object_type *o_ptr)
{
	u32b gen_flags = 0L;

	if (!o_ptr->k_idx) return NO_ETHNICITY;

	gen_flags = k_info[o_ptr->k_idx].gen_flags;
	if (o_ptr->name1) gen_flags |= a_info[o_ptr->name1].gen_flags;
	else if (o_ptr->name2) gen_flags |= e_info[o_ptr->name2].gen_flags;

	if (gen_flags & TRG_WALSTANIAN) return ETHNICITY_WALSTANIAN;
	if (gen_flags & TRG_GARGASTAN)  return ETHNICITY_GARGASTAN;
	if (gen_flags & TRG_BACRUM)     return ETHNICITY_BACRUM;
	if (gen_flags & TRG_ZENOBIAN)   return ETHNICITY_ZENOBIAN;
	if (gen_flags & TRG_LODIS)      return ETHNICITY_LODIS;

	switch (o_ptr->tval)
	{
	case TV_STATUE:
	case TV_FIGURINE:
	case TV_CORPSE:
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];
			if (r_ptr->flags2 & RF2_WALSTANIAN) return ETHNICITY_WALSTANIAN;
			if (r_ptr->flags2 & RF2_GARGASTAN)  return ETHNICITY_GARGASTAN;
			if (r_ptr->flags2 & RF2_BACRUM)     return ETHNICITY_BACRUM;
			if (r_ptr->flags2 & RF2_ZENOBIAN)   return ETHNICITY_ZENOBIAN;
			if (r_ptr->flags2 & RF2_LODIS)      return ETHNICITY_LODIS;
		}
		break;
	}

	return NO_ETHNICITY;
}


/*
 * Obtain the "Another weapon type" of an object
 */
byte get_weapon_type(object_kind *k_ptr)
{
	if (!k_ptr) return WT_NONE;

	switch (k_ptr->tval)
	{
	case TV_BOW:
		switch (k_ptr->sval)
		{
		case SV_PISTOL:
		case SV_ASSAULT_RIFLE:
		case SV_SNIPER_RIFLE:
		case SV_SHOTGUN:
		case SV_ROCKET_LAUNCHER:
		case SV_RUNEGUN:
			return WT_GUN;

		case SV_SHORT_BOW:
		case SV_LONG_BOW:
		case SV_BOWGUN:
		case SV_CROSSBOW:
		case SV_RUNEBOW:
			return WT_BOW;
		}
		break;

	case TV_DIGGING:
		return WT_AXE;

	case TV_HAFTED:
		switch (k_ptr->sval)
		{
		case SV_CLUB:
		case SV_HALT_HAMMER:
		case SV_PAUA_HAMMER:
		case SV_GREAT_HAMMER:
		case SV_RUNEHAMMER:
			return WT_HAMMER;

		case SV_WHIP:
		case SV_RUNEWHIP:
			return WT_WHIP;

		case SV_QUARTERSTAFF:
		case SV_SCIPPLAYS_STAFF:
		case SV_FLAIL:
		case SV_LEAD_FILLED_MACE:
		case SV_MACE_OF_DISRUPTION:
		case SV_WIZSTAFF:
		case SV_LIFE_STAFF:
		case SV_CLEAR_STAFF:
		case SV_GROND:
		case SV_RUNESTAFF:
			return WT_STAFF;

		case SV_FAN:
		case SV_RUNEFAN:
			return WT_FAN;

		case SV_SINAI:
			return WT_KATANA;
		}
		break;

	case TV_POLEARM:
		switch (k_ptr->sval)
		{
		case SV_FRANCISCA:
		case SV_BROAD_AXE:
		case SV_BATTLE_AXE:
		case SV_GREAT_AXE:
		case SV_HEAVY_AXE:
		case SV_RUNEAXE:
			return WT_AXE;

		case SV_SPEAR:
		case SV_SLENDER_SPEAR:
		case SV_BROAD_SPEAR:
		case SV_GLAIVE:
		case SV_HALBERD:
			return WT_SPEAR;

		case SV_SCYTHE:
		case SV_SCYTHE_OF_SLICING:
		case SV_DEATH_SCYTHE:
			return WT_SCYTHE;

		case SV_LANCE:
		case SV_HEAVY_LANCE:
		case SV_RUNESPEAR:
			return WT_LANCE;
		}
		break;

	case TV_SWORD:
		switch (k_ptr->sval)
		{
		case SV_DAGGER:
		case SV_MAIN_GAUCHE:
		case SV_RAPIER:
		case SV_SHORT_SWORD:
		case SV_MINIMUM_DAGGER:
			return WT_SMALL_SWORD;

		case SV_YOUTOU:
		case SV_KATANA:
			return WT_KATANA;

		case SV_BROAD_SWORD:
		case SV_LONG_SWORD:
		case SV_BASTARD_SWORD:
		case SV_DIAMOND_EDGE:
		case SV_MITHRIL_SWORD:
		case SV_RUNEBLADE:
			return WT_SWORD;

		case SV_TWO_HANDED_SWORD:
		case SV_DARK_SWORD:
		case SV_FALCHION:
			return WT_GREAT_SWORD;

		case SV_MADU:
		case SV_RUNECLAW:
			return WT_CLAW;
		}
		break;
	}

	return WT_NONE;
}


/*
 * Hack -- describe an item currently in a store's inventory
 * This allows an item to *look* like the player is "aware" of it
 */
void object_desc_store(char *buf, object_type *o_ptr, int pref, int mode)
{
	/* Save the "aware" flag */
	bool hack_aware = object_aware_p(o_ptr);

	/* Save the "known" flag */
	bool hack_known = (o_ptr->ident & (IDENT_KNOWN)) ? TRUE : FALSE;


	/* Set the "known" flag */
	o_ptr->ident |= (IDENT_KNOWN);

	/* Force "aware" for description */
	k_info[o_ptr->k_idx].aware = TRUE;


	/* Describe the object */
	object_desc(buf, o_ptr, pref, mode);


	/* Restore "aware" flag */
	k_info[o_ptr->k_idx].aware = hack_aware;

	/* Clear the known flag */
	if (!hack_known) o_ptr->ident &= ~(IDENT_KNOWN);
}




/*
 * Determine the "Activation" (if any) for an artifact
 * Return a string, or NULL for "no activation"
 */
cptr item_activation(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Require activation ability */
#ifdef JP
	if (!(have_flag(flgs, TR_ACTIVATE))) return ("�ʤ�");
#else
	if (!(have_flag(flgs, TR_ACTIVATE))) return ("nothing");
#endif



	/*
	 * We need to deduce somehow that it is a random artifact -- one
	 * problem: It could be a random artifact which has NOT YET received
	 * a name. Thus we eliminate other possibilities instead of checking
	 * for art_name
	 */

	if (!(o_ptr->name1) &&
		 !(o_ptr->name2) &&
		 !(o_ptr->xtra1) &&
		  (o_ptr->xtra2))
	{
		switch (o_ptr->xtra2)
		{
			case ACT_SUNLIGHT:
			{
#ifdef JP
return "���۸��� : 10 ��������";
#else
				return "beam of sunlight every 10 turns";
#endif

			}
			case ACT_BO_MISS_1:
			{
#ifdef JP
return "�ޥ��å����ߥ�����(2d6) : 2 ��������";
#else
				return "magic missile (2d6) every 2 turns";
#endif

			}
			case ACT_BA_POIS_1:
			{
#ifdef JP
return "������ (12), Ⱦ�� 3 , 4+d4 ��������";
#else
				return "stinking cloud (12), rad. 3, every 4+d4 turns";
#endif

			}
			case ACT_BO_ELEC_1:
			{
#ifdef JP
return "����������ܥ��(4d8) : 5+d5 ��������";
#else
				return "lightning bolt (4d8) every 5+d5 turns";
#endif

			}
			case ACT_BO_ACID_1:
			{
#ifdef JP
return "�����åɡ��ܥ��(5d8) : 6+d6 ��������";
#else
				return "acid bolt (5d8) every 6+d6 turns";
#endif

			}
			case ACT_BO_COLD_1:
			{
#ifdef JP
return "���������ܥ��(6d8) : 7+d7 ��������";
#else
				return "frost bolt (6d8) every 7+d7 turns";
#endif

			}
			case ACT_BO_FIRE_1:
			{
#ifdef JP
return "�ե��������ܥ��(9d8) : 8+d8 ��������";
#else
				return "fire bolt (9d8) every 8+d8 turns";
#endif

			}
			case ACT_BA_COLD_1:
			{
#ifdef JP
return "������ɡ��ܡ��� (48) : 400 ��������";
#else
				return "ball of cold (48) every 400 turns";
#endif

			}
			case ACT_BA_FIRE_1:
			{
#ifdef JP
return "�ե��������ܡ��� (72) : 400 ��������";
#else
				return "ball of fire (72) every 400 turns";
#endif

			}
			case ACT_DRAIN_1:
			{
#ifdef JP
return "�ҥåȥݥ���ȵۼ� (100) : 100+d100 ��������";
#else
				return "drain life (100) every 100+d100 turns";
#endif

			}
			case ACT_BA_COLD_2:
			{
#ifdef JP
return "������ɡ��ܡ��� (100) : 300 ��������";
#else
				return "ball of cold (100) every 300 turns";
#endif

			}
			case ACT_BA_ELEC_2:
			{
#ifdef JP
return "����������ܡ��� (100) : 500 ��������";
#else
				return "ball of lightning (100) every 500 turns";
#endif

			}
			case ACT_DRAIN_2:
			{
#ifdef JP
return "�ҥåȥݥ���ȵۼ�(120) : 400 ��������";
#else
				return "drain life (120) every 400 turns";
#endif

			}
			case ACT_VAMPIRE_1:
			{
#ifdef JP
return "�۷�ɥ쥤�� (3*50) : 400 ��������";
#else
				return "vampiric drain (3*50) every 400 turns";
#endif

			}
			case ACT_BO_MISS_2:
			{
#ifdef JP
return "�� (150) : 90+d90 ��������";
#else
				return "arrows (150) every 90+d90 turns";
#endif

			}
			case ACT_BA_FIRE_2:
			{
#ifdef JP
return "�ե��������ܡ��� (120) : 225+d225 ��������";
#else
				return "fire ball (120) every 225+d225 turns";
#endif

			}
			case ACT_BA_COLD_3:
			{
#ifdef JP
return "������ɡ��ܡ��� (200) : 325+d325 ��������";
#else
				return "ball of cold (200) every 325+d325 turns";
#endif

			}
			case ACT_BA_ELEC_3:
			{
#ifdef JP
return "����������ܡ��� (250) : 425+d425 ��������";
#else
				return "ball of lightning (250) every 425+d425 turns";
#endif

			}
			case ACT_WHIRLWIND:
			{
#ifdef JP
return "���ޥ����� : 250 ��������";
#else
				return "whirlwind attack every 250 turns";
#endif

			}
			case ACT_VAMPIRE_2:
			{
#ifdef JP
return "�۷�ɥ쥤�� (3*100) : 400 ��������";
#else
				return "vampiric drain (3*100) every 400 turns";
#endif

			}
			case ACT_CALL_CHAOS:
			{
#ifdef JP
return "call chaos : 350 ��������"; /*nuke me*/
#else
				return "call chaos every 350 turns";
#endif

			}
			case ACT_ROCKET:
			{
#ifdef JP
return "���å� (120+level) : 400 ��������";
#else
				return "launch rocket (120+level) every 400 turns";
#endif

			}
			case ACT_DISP_EVIL:
			{
#ifdef JP
return "�ٰ��໶ (level*5) : 300+d300 ��������";
#else
				return "dispel evil (level*5) every 300+d300 turns";
#endif

			}
			case ACT_BA_MISS_3:
			{
#ifdef JP
return "������ȤΥ֥쥹 (300) : 500 ��������";
#else
				return "elemental breath (300) every 500 turns";
#endif

			}
			case ACT_DISP_GOOD:
			{
#ifdef JP
return "�����໶ (level*5) : 300+d300 ��������";
#else
				return "dispel good (level*5) every 300+d300 turns";
#endif

			}
			case ACT_SUMMON1:
			{
				return "����󥾥���ͥ� : 666 ��������";
			}
			case ACT_SUMMON2:
			{
				return "����󥰥롼�� : 666 ��������";
			}
			case ACT_SUMMON3:
			{
				return "�����С��� : 666 ��������";
			}
			case ACT_SUMMON4:
			{
				return "�����ϡ��ͥ� : 666 ��������";
			}
			case ACT_SUMMON5:
			{
				return "����󥤥��奿�� : 666 ��������";
			}
			case ACT_SUMMON6:
			{
				return "����󥢥���� : 666 ��������";
			}
			case ACT_DEC_RAIN:
			{
				return "���Ф� : 500 ��������";
			}
			case ACT_INC_RAIN:
			{
				return "���Ჽ : 500 ��������";
			}
			case ACT_DEC_WIND:
			{
				return "���� : 500 ��������";
			}
			case ACT_INC_WIND:
			{
				return "���� : 500 ��������";
			}
			case ACT_DEC_TEMP:
			{
				return "���Ȳ� : 500 ��������";
			}
			case ACT_INC_TEMP:
			{
				return "���䲽 : 500 ��������";
			}
			case ACT_SH_FIRE:
			{
#ifdef JP
				return "��Υ����� (���� 20+d20) : 500 ��������";
#else
				return "fiery aura (dur 20+d20) every 500 turns";
#endif
			}
			case ACT_SH_ELEC:
			{
#ifdef JP
				return "�ŵ��Υ����� (���� 20+d20) : 500 ��������";
#else
				return "electrical aura (dur 20+d20) every 500 turns";
#endif
			}
			case ACT_SH_COLD:
			{
#ifdef JP
				return "�䵤�Υ����� (���� 20+d20) : 500 ��������";
#else
				return "cold aura (dur 20+d20) every 500 turns";
#endif
			}
			case ACT_SH_SHARDS:
			{
#ifdef JP
				return "���ҤΥ����� (���� 20+d20) : 500 ��������";
#else
				return "shards aura (dur 20+d20) every 500 turns";
#endif
			}
			case ACT_CONFUSE:
			{
#ifdef JP
return "�ѥ˥å�����󥹥��� : 15 ��������";
#else
				return "confuse monster every 15 turns";
#endif

			}
			case ACT_SLEEP:
			{
#ifdef JP
return "���ϤΥ�󥹥�����̲�餻�� : 55 ��������";
#else
				return "sleep nearby monsters every 55 turns";
#endif

			}
			case ACT_QUAKE:
			{
#ifdef JP
return "�Ͽ� (Ⱦ�� 10) : 50 ��������";
#else
				return "earthquake (rad 10) every 50 turns";
#endif

			}
			case ACT_TERROR:
			{
#ifdef JP
return "���� : 3 * (level+10) ��������";
#else
				return "terror every 3 * (level+10) turns";
#endif

			}
			case ACT_TELE_AWAY:
			{
#ifdef JP
return "�ƥ�ݡ��ȡ��������� : 150 ��������";
#else
				return "teleport away every 200 turns";
#endif

			}
			case ACT_BANISH_EVIL:
			{
#ifdef JP
return "�ٰ����� : 250+d250 ��������";
#else
				return "banish evil every 250+d250 turns";
#endif

			}
			case ACT_GENOCIDE:
			{
#ifdef JP
return "���� : 500 ��������";
#else
				return "genocide every 500 turns";
#endif

			}
			case ACT_MASS_GENO:
			{
#ifdef JP
return "�������� : 1000 ��������";
#else
				return "mass genocide every 1000 turns";
#endif

			}
			case ACT_CHARM_ANIMAL:
			{
#ifdef JP
return "ưʪ̥λ : 300 ��������";
#else
				return "charm animal every 300 turns";
#endif

			}
			case ACT_CHARM_UNDEAD:
			{
#ifdef JP
return "�Իཾ° : 333 ��������";
#else
				return "enslave undead every 333 turns";
#endif

			}
			case ACT_CHARM_OTHER:
			{
#ifdef JP
return "��󥹥���̥λ : 400 ��������";
#else
				return "charm monster every 400 turns";
#endif

			}
			case ACT_CHARM_ANIMALS:
			{
#ifdef JP
return "ưʪͧ�� : 500 ��������";
#else
				return "animal friendship every 500 turns";
#endif

			}
			case ACT_CHARM_OTHERS:
			{
#ifdef JP
return "����̥λ : 750 ��������";
#else
				return "mass charm every 750 turns";
#endif

			}
			case ACT_SUMMON_ANIMAL:
			{
#ifdef JP
return "ưʪ���� : 200+d300 ��������";
#else
				return "summon animal every 200+d300 turns";
#endif

			}
			case ACT_SUMMON_PHANTOM:
			{
#ifdef JP
return "�Ȥ���� : 200+d200 ��������";
#else
				return "summon phantasmal servant every 200+d200 turns";
#endif

			}
			case ACT_SUMMON_ELEMENTAL:
			{
#ifdef JP
return "�����󥿥뾤�� : 750 ��������";
#else
				return "summon elemental every 750 turns";
#endif

			}
			case ACT_SUMMON_DEMON:
			{
#ifdef JP
return "���⾤�� : 666+d333 ��������";
#else
				return "summon demon every 666+d333 turns";
#endif

			}
			case ACT_SUMMON_UNDEAD:
			{
#ifdef JP
return "�Իྤ�� : 666+d333 ��������";
#else
				return "summon undead every 666+d333 turns";
#endif

			}
			case ACT_CURE_LW:
			{
#ifdef JP
return "ͦ������ & 30 hp ���� : 10 ��������";
#else
				return "remove fear & heal 30 hp every 10 turns";
#endif

			}
			case ACT_CURE_MW:
			{
#ifdef JP
return "4d8 hp & ������ : 3+d3 ��������";
#else
				return "heal 4d8 & wounds every 3+d3 turns";
#endif

			}
			case ACT_CURE_POISON:
			{
#ifdef JP
return "ͦ������/�Ǿä� : 5 ��������";
#else
				return "remove fear and cure poison every 5 turns";
#endif

			}
			case ACT_REST_LIFE:
			{
#ifdef JP
return "�и������� : 450 ��������";
#else
				return "restore life levels every 450 turns";
#endif

			}
			case ACT_REST_ALL:
			{
#ifdef JP
return "�����ơ������ȷи��Ͳ��� : 750 ��������";
#else
				return "restore stats and life levels every 750 turns";
#endif

			}
			case ACT_CURE_700:
			{
#ifdef JP
return "700 hp ���� : 250 ��������";
#else
				return "heal 700 hit points every 250 turns";
#endif

			}
			case ACT_CURE_1000:
			{
#ifdef JP
return "1000 hp ���� : 888 ��������";
#else
				return "heal 1000 hit points every 888 turns";
#endif

			}
			case ACT_ESP:
			{
#ifdef JP
return "���Ū�� ESP (���� 25+d30) : 200 ��������";
#else
				return "temporary ESP (dur 25+d30) every 200 turns";
#endif

			}
			case ACT_BERSERK:
			{
#ifdef JP
return "�ε����ȤȽ�ʡ (���� 50+d50) : 100+d100 ��������";
#else
				return "heroism and blessed (dur 50+d50) every 100+d100 turns";
#endif

			}
			case ACT_PROT_EVIL:
			{
#ifdef JP
return "�мٰ��볦 (���� level*3 + d25) : 225+d225 ��������";
#else
				return "protect evil (dur level*3 + d25) every 225+d225 turns";
#endif

			}
			case ACT_RESIST_ALL:
			{
#ifdef JP
return "��������Ǥؤ����� (���� 40+d40) : 200 ��������";
#else
				return "resist elements (dur 40+d40) every 200 turns";
#endif

			}
			case ACT_SPEED:
			{
#ifdef JP
return "��® (���� 20+d20) : 250 ��������";
#else
				return "speed (dur 20+d20) every 250 turns";
#endif

			}
			case ACT_XTRA_SPEED:
			{
#ifdef JP
return "��® (���� 75+d75) : 200+d200 ��������";
#else
				return "speed (dur 75+d75) every 200+d200 turns";
#endif

			}
			case ACT_WRAITH:
			{
#ifdef JP
return "�쥤���� (level/2 + d(level/2)) : 1000 ��������";
#else
				return "wraith form (level/2 + d(level/2)) every 1000 turns";
#endif

			}
			case ACT_INVULN:
			{
#ifdef JP
return "̵Ũ (���� 8+d8) : 1000 ��������";
#else
				return "invulnerability (dur 8+d8) every 1000 turns";
#endif

			}
			case ACT_LIGHT:
			{
#ifdef JP
return "���վ��� (���᡼�� 2d15) : 10+d10 ��������";
#else
				return "light area (dam 2d15) every 10+d10 turns";
#endif

			}
			case ACT_MAP_LIGHT:
			{
#ifdef JP
return "���վ��� (���᡼�� 2d15) & ���եޥå� : 50+d50 ��������";
#else
				return "light (dam 2d15) & map area every 50+d50 turns";
#endif

			}
			case ACT_DETECT_ALL:
			{
#ifdef JP
return "������ : 55+d55 ��������";
#else
				return "detection every 55+d55 turns";
#endif

			}
			case ACT_DETECT_XTRA:
			{
#ifdef JP
return "�����Ρ�õ����*����* : 1000 ��������";
#else
				return "detection, probing and identify true every 1000 turns";
#endif

			}
			case ACT_ID_FULL:
			{
#ifdef JP
return "*����* : 750 ��������";
#else
				return "identify true every 750 turns";
#endif

			}
			case ACT_ID_PLAIN:
			{
#ifdef JP
return "���� : 10 ��������";
#else
				return "identify spell every 10 turns";
#endif

			}
			case ACT_RUNE_EXPLO:
			{
#ifdef JP
return "��ȯ�롼�� : 200 ��������";
#else
				return "explosive rune every 200 turns";
#endif

			}
			case ACT_RUNE_PROT:
			{
#ifdef JP
return "���Υ롼�� : 400 ��������";
#else
				return "rune of protection every 400 turns";
#endif

			}
			case ACT_SATIATE:
			{
#ifdef JP
return "��ʢ��­ : 200 ��������";
#else
				return "satisfy hunger every 200 turns";
#endif

			}
			case ACT_DEST_DOOR:
			{
#ifdef JP
return "�ɥ��˲� : 10 ��������";
#else
				return "destroy doors every 10 turns";
#endif

			}
			case ACT_STONE_MUD:
			{
#ifdef JP
return "�����ϲ� : 5 ��������";
#else
				return "stone to mud every 5 turns";
#endif

			}
			case ACT_RECHARGE:
			{
#ifdef JP
return "�ƽ�Ŷ : 70 ��������";
#else
				return "recharging every 70 turns";
#endif

			}
			case ACT_ALCHEMY:
			{
#ifdef JP
return "ϣ��� : 500 ��������";
#else
				return "alchemy every 500 turns";
#endif

			}
			case ACT_DIM_DOOR:
			{
#ifdef JP
return "�������� : 100 ��������";
#else
				return "dimension door every 100 turns";
#endif

			}
			case ACT_TELEPORT:
			{
#ifdef JP
return "�ƥ�ݡ��� (range 100) : 45 ��������";
#else
				return "teleport (range 100) every 45 turns";
#endif

			}
			case ACT_RECALL:
			{
#ifdef JP
return "���Ԥξ� : 200 ��������";
#else
				return "word of recall every 200 turns";
#endif

			}
			default:
			{
#ifdef JP
return "̤���";
#else
				return "something undefined";
#endif

			}
		}
	}

	/* Some artifacts can be activated */
	switch (o_ptr->name1)
	{
		case ART_FREUDE_HELM:
		{
#ifdef JP
			return "�мٰ��볦�ȼٰ��໶ : 200 ��������";
#else
			return "protection from evil and dispel evil every 200 turns";
#endif

		}
		case ART_SARA:
		{
#ifdef JP
return "����������ܥ��(4d8) : 6+d6 ��������";
#else
			return "lightning bolt (4d8) every 6+d6 turns";
#endif

		}
		case ART_BERSALIA:
		{
#ifdef JP
return "���������ܡ���(48) : 5+d5 ��������";
#else
			return "frost ball (48) every 5+d5 turns";
#endif

		}
		case ART_NORN:
		{
#ifdef JP
return "���ݽ���/�Ǿä� : 5 ��������";
#else
			return "remove fear and cure poison every 5 turns";
#endif

		}
		case ART_SONIC_BLADE:
		{
#ifdef JP
return "���������ܡ���(100) : 300 ��������";
#else
			return "frost ball (100) every 300 turns";
#endif

		}
		case ART_BRUNHILD:
		{
#ifdef JP
return "�����������ȳ���";
#else
			return "open chaos gate";
#endif
		}
		case ART_RAPTURE_ROSE:
		{
#ifdef JP
return "����ե��������ܡ���(72) : 100 ��������";
#else
			return "large fire ball (72) every 100 turns";
#endif

		}
		case ART_ROSHFEL:
		{
#ifdef JP
return "���ԡ���(20+d20������) : 200 ��������";
#else
			return "haste self (20+d20 turns) every 200 turns";
#endif

		}
		case ART_BOREAS:
		{
#ifdef JP
return "����������ܡ��� (200) ���ŷ�ؤ����� : 300 ��������";
#else
				return "ball of elec (200) and resist elec every 300 turns";
#endif

		}
		case ART_TURMIL:
		{
#ifdef JP
return "��̿�ϵۼ�(90) : 70 ��������";
#else
			return "drain life (90) every 70 turns";
#endif

		}
		case ART_LEVIATHAN:
		{
#ifdef JP
return "�ɥ�/�ȥ�å�ʴ�� : 10 ��������";
#else
			return "door and trap destruction every 10 turns";
#endif

		}
		case ART_SATANS_BULLOVA:
		{
#ifdef JP
return "̽���ؤξ��� (200) : 500+d200��������";
#else
			return "call for netherworld every 500+d200 turns";
#endif
		}
		case ART_SAVAGE:
		{
#ifdef JP
return "������ϥꥱ���� (��ŷ��+8) : 800 ��������";
#else
			return "call hurricane (weather +8) every 800 turns";
#endif

		}
		case ART_ZADOVA:
		{
#ifdef JP
return "�ٴ� (�볦���Ũ���в�,���᡼���ʤ�) : 200+d100��������";
#else
			return "evil gaze (stone monsters in sight) every 200+d100 turns";
#endif

		}
		case ART_BAIAN:
		{
#ifdef JP
return "���� : 10 ��������";
#else
			return "identify every 10 turns";
#endif

		}
		case ART_WARREN:
		{
#ifdef JP
return "õ���������Ρ������� : 1000 ��������";
#else
			return "probing, detection and full id every 1000 turns";
#endif

		}
		case ART_GYPSY_QUEEN:
		{
#ifdef JP
return "�������� : 1000 ��������";
#else
			return "mass genocide every 1000 turns";
#endif

		}
		case ART_GRAMLOCK:
		{
#ifdef JP
return "����ե��������ܡ��� (200) : 200 ��������";
#else
			return "large fire ball (200) every 200 turns";
#endif

		}
		case ART_ZENOBIA:
		{
#ifdef JP
return "��ʡ���ҡ�����ʬ����® : 100+d50 ��������";
#else
			return "bless, heroism, speed every 100+d50 turns";
#endif

		}
		case ART_SISTEENA:
		{
#ifdef JP
return "ƨ�� : 35 ��������";
#else
			return "a getaway every 35 turns";
#endif

		}
		case ART_OSRIC:
		{
#ifdef JP
return "�䵤�Υӡ��� (100) : 100+d100 ��������";
#else
			return "cold beam (100) every 100+d100 turns";
#endif

		}
		case ART_GUNGNIR:
		{
#ifdef JP
return "����������ܡ��� (100) : 500 ��������";
#else
			return "lightning ball (100) every 500 turns";
#endif

		}
		case ART_EVIL_SPEAR:
		{
#ifdef JP
return "�۷���� : 200 ��������";
#else
			return "drain bolt every 200 turns";
#endif

		}
		case ART_SOULKEEPER:
		{
#ifdef JP
return "���ϲ���(1000) : 888 ��������";
#else
			return "heal (1000) every 888 turns";
#endif

		}
		case ART_LANCELOT_H:
		{
#ifdef JP
return ("���� (777)���������ҡ�����ʬ : 300 ��������");
#else
			return ("heal (777), curing and heroism every 300 turns");
#endif

		}
		case ART_DENIM:
		{
#ifdef JP
return "���� : 500 ��������";
#else
			return "genocide every 500 turns";
#endif

		}
		case ART_ALBELEO:
		{
#ifdef JP
return "��̿������ : 450 ��������";
#else
			return "restore life levels every 450 turns";
#endif

		}
		case ART_BENTISCA:
		{
#ifdef JP
return "�ƥ�ݡ��ȡ��������� : 150 ��������";
#else
			return "teleport away every 150 turns";
#endif

		}
		case ART_BELZBUTE:
		{
#ifdef JP
return "������(20+d20������) : 111 ��������";
#else
			return "resistance (20+d20 turns) every 111 turns";
#endif

		}
		case ART_CLARE:
		{
#ifdef JP
return "���꡼��(II) : 55 ��������";
#else
			return "Sleep II every 55 turns";
#endif

		}
		case ART_AISHA:
		{
#ifdef JP
return "�������в�����������(80) : 50+d50 ��������";
#else
			return "curing, cure stoning, heal (80) every 50+d50 turns";
#endif

		}
		case ART_GRINCER_COAT:
		{
#ifdef JP
return "�ƥ�ݡ��� : 45 ��������";
#else
			return "teleport every 45 turns";
#endif

		}
		case ART_GUACHARO:
		{
#ifdef JP
return "�ե��������ܥ��(9d8) : 8+d8 ��������";
#else
			return "fire bolt (9d8) every 8+d8 turns";
#endif

		}
		case ART_DEBARDES:
		{
#ifdef JP
return "���������ܥ��(6d8) : 7+d7 ��������";
#else
			return "frost bolt (6d8) every 7+d7 turns";
#endif

		}
		case ART_ERIG:
		{
#ifdef JP
return "��ˡ����(150) : 90+d90 ��������";
#else
			return "a magical arrow (150) every 90+d90 turns";
#endif

		}
		case ART_RENDAL:
		{
#ifdef JP
return "������ : 55+d55 ��������";
#else
			return "detection every 55+d55 turns";
#endif

		}
		case ART_DANIKA:
		{
#ifdef JP
return "���ϲ���(700) : 250 ��������";
#else
			return "heal (700) every 250 turns";
#endif

		}
		case ART_RUNGVIE:
		{
#ifdef JP
return "���������ܡ���(150) : 1000 ��������";
#else
			return "star ball (150) every 1000 turns";
#endif

		}
		case ART_BLADETURNER:
		{
#ifdef JP
return "*����*�Υ֥쥹 (300), �ε����ȡ���ʡ������";
#else
			return "breathe *elements* (300), hero, bless, and resistance";
#endif

		}
		case ART_LEGACY:
		{
#ifdef JP
return "����ߥ͡������ : 10+d10 ��������";
#else
			return "illumination every 10+d10 turns";
#endif

		}
		case ART_PUMPKIN:
		{
#ifdef JP
return "�ѥ�ץ���إå�ʣ������ : 400+d200 ��������";
#else
			return "summon Pumpkin Heads every 400+d200 turns";
#endif

		}
		case ART_HABORYM_EYE:
		{
#ifdef JP
return "���ϤȰ����ؤ�����Τ�� : 20+d20 ��������";
#else
			return "clairvoyance, draining you every 20+d20 turns";
#endif

		}
		case ART_RODERIC:
		{
			switch (get_your_alignment_gne())
			{
			case ALIGN_GNE_GOOD:
#ifdef JP
				return "�ٰ��໶(x5) : 200+d200 ��������";
#else
				return "dispel evil (x5) every 200+d200 turns";
#endif
			case ALIGN_GNE_NEUTRAL:
#ifdef JP
				return "��󥹥����໶(x5) : 200+d200 ��������";
#else
				return "dispel monsters (x5) every 200+d200 turns";
#endif
			case ALIGN_GNE_EVIL:
#ifdef JP
				return "�����໶(x5) : 200+d200 ��������";
#else
				return "dispel good (x5) every 200+d200 turns";
#endif
			}
		}
		case ART_PRESANCE:
		{
#ifdef JP
			return "�ٰ��໶(x5) : 200+d200 ��������";
#else
			return "dispel evil (x5) every 200+d200 turns";
#endif

		}
		case ART_RESIST:
		{
#ifdef JP
return "�мٰ��볦 : 225+d225 ��������";
#else
			return "protection from evil every 225+d225 turns";
#endif

		}
		case ART_TULKAS:
		{
#ifdef JP
return "���ԡ���(75+d75������) : 150+d150 ��������";
#else
			return "haste self (75+d75 turns) every 150+d150 turns";
#endif

		}
		case ART_SACRED_RING:
		{
#ifdef JP
return "�Х˥å���(���ʤ��)(150) : 225+d225 ��������";
#else
			return "vanish (holy ball) (150) every 225+d225 turns";
#endif

		}
		case ART_EVIL_RING:
		{
#ifdef JP
return "���ӥ�(�ٰ��ʵ�)(250) : 325+d325 ��������";
#else
			return "abyss (evil ball) (250) every 325+d325 turns";
#endif

		}
		case ART_DEMON_RING:
		{
#ifdef JP
return "��������(�˾��μٰ��ʵ�)(999) : 1000 ��������";
#else
			return "dark lore (minimum evil ball) (999) every 1000 turns";
#endif

		}
		case ART_FIRECREST:
		{
#ifdef JP
return "�����񤤤��� : 450+d450 ��������";
#else
			return "bizarre things every 450+d450 turns";
#endif

		}
		case ART_GARINGA: case ART_SKULL_MASK:
		{
#ifdef JP
			return "�������ؤζ��ݤθ��� : 3*(��٥�+10) ��������";
#else
			return "rays of fear in every direction every 3*(level+10) turns";
#endif

		}
		case ART_EUROS:
		{
#ifdef JP
return "�ε�����, ���ԡ���(50+d50������) : 100+d200 ��������";
#else
			return "hero and +10 to speed (50+d50 turns) every 100+200d turns";
#endif
		}
		case ART_ZEPHYRUS:
		{
#ifdef JP
return "����������ܡ���(200) : 250 ��������";
#else
			return "lightning ball (200) every 250 turns";
#endif

		}
		case ART_RED:
		{
#ifdef JP
return "��󥹥������� : 40+d40��������";
#else
			return "frighten monsters every 40+d40 turns";
#endif
		}
		case ART_BLUE:
		{
#ifdef JP
return "����ζ�� : 55+d55��������";
#else
			return "dispel small life every 55+d55 turns";
#endif
		}
		case ART_YENDOR:
		{
#ifdef JP
return "���Ͻ�Ŷ : 200 ��������";
#else
			return "Recharge item every 200 turns";
#endif
		}
		case ART_HOLY_LANCE:
		{
#ifdef JP
return "�ۡ��꡼���(���ʤ��Υӡ���)(150) : 200 ��������";
#else
			return "holy lance (holy fire beam) (100) every 200 turns";
#endif

		}
		case ART_IGNIS:
		{
#ifdef JP
return "����ե��������ܡ���(120) : 15 ��������";
#else
			return "large fire ball (120) every 15 turns";
#endif

		}
		case ART_ANGELIC_ARMOR:
		{
#ifdef JP
			return "�мٰ��볦 : 200+d200 ��������";
#else
			return "protection from evil every 200 + d200 turns";
#endif
		}
		case ART_RIPPLES_STAFF:
		{
#ifdef JP
			return "���Ϥ���(120) : 20+d20 ��������";
#else
			return "a mana bolt (120) every 20+d20 turns";
#endif
		}
		case ART_BLOOD_WHIP:
		{
#ifdef JP
return "��̿�ϵۼ�(90) : 70 ��������";
#else
			return "drain life (90) every 70 turns";
#endif

		}
		case ART_CORAL:
		{
#ifdef JP
return "����ӥ���֥롼 (��ŷ��-8) : 800��������";
#else
			return "Caribbean blue (weather -8) every 800 turns";
#endif
		}
		case ART_COMET:
		{
#ifdef JP
return "���ʤ���� (75) : 50+d50��������";
#else
			return "holy orb (75) every 50+d50 turns";
#endif
		}
		case ART_MARITZA:
		{
#ifdef JP
return "���㡼�����ڥ� (50+d25������) : 200��������";
#else
			return "charging spell (50+d25 turns) every 200 turns";
#endif
		}
		case ART_OGRE_BLADE:
		{
#ifdef JP
			return "���ʥåץɥ饴�� (���ؤ��Ѳ�)";
#else
			return "Snap Dragon (Change into a weapon)";
#endif
		}
		case ART_LIFE_STAFF:
		{
#ifdef JP
return "���ϲ���(80) : 30 ��������";
#else
			return "heal (80) every 30 turns";
#endif
		}
		case ART_CLEAR_STAFF:
		{
#ifdef JP
return "���ֲ��� : 10 ��������";
#else
			return "curing every 10 turns";
#endif
		}
		case ART_FAKE:
		{
#ifdef JP
return "�褯�ʤ����� :  20+d20 ��������";
#else
			return "bad things every 20+d20 turns";
#endif
		}
		case ART_NIGHT:
		{
#ifdef JP
return "�Ź�����(250) : 150+d150 ��������";
#else
			return "darkness storm (250) every 150+d150 turns";
#endif

		}
		case ART_FIREBIRD:
		{
#ifdef JP
			return "���1��̥λ : 400 ��������";
#else
			return "charm a beast every 400 turns";
#endif

		}
		case ART_CAMERA:
		{
#ifdef JP
			return "�̿����� : ���ĤǤ�";
#else
			return "Take a Photograph every turn";
#endif

		}
		case ART_VOLUPTUOUS:
		{
#ifdef JP
			return "����̥λ : 20 ��������";
#else
			return "charm males every 20 turns";
#endif

		}
		case ART_LIGHTNING_BOW:
		{
#ifdef JP
			return "�˾���*��*�ε� : 200 ��������";
#else
			return "minimum *wind* ball every 200 turns";
#endif

		}
		case ART_AMU_FIRE:
		{
#ifdef JP
			return "�ץ쥤������󥾥���ͥ� : 300 ��������";
#else
			return "Pray / Summon  300 turns";
#endif

		}
		case ART_AMU_AQUA:
		{
#ifdef JP
			return "�ץ쥤������󥰥롼�� : 300 ��������";
#else
			return "Pray / Summon  300 turns";
#endif

		}
		case ART_AMU_EARTH:
		{
#ifdef JP
			return "�ץ쥤�������С��� : 300 ��������";
#else
			return "Pray / Summon  300 turns";
#endif

		}
		case ART_AMU_WIND:
		{
#ifdef JP
			return "�ץ쥤�������ϡ��ͥ� : 300 ��������";
#else
			return "Pray / Summon  300 turns";
#endif

		}
	}

	/* Branch on the ego-item type */
	switch (o_ptr->name2)
	{
	case EGO_WARP:
#ifdef JP
		return "�������� : 50+d25 ��������";
#else
		return "dimension door every 50+d25 turns";
#endif
	case EGO_SHOOTING_STAR:
#ifdef JP
		return "ή���� : ���ĤǤ�";
#else
		return "shooting star strike every turn";
#endif
	case EGO_EARTHQUAKES:
#ifdef JP
		return "�Ͽ� : 100+d100 ��������";
#else
		return "earthquake every 100+d100 turns";
#endif
	case EGO_LITE_ILLUMINATION:
#ifdef JP
		return "����ߥ͡������ : 10+d10 ��������";
#else
		return "illumination every 10+d10 turns";
#endif
	case EGO_RING_HERO:
#ifdef JP
		return "�ε����� : 100+d100��������";
#else
		return "heroism every 100+d100 turns";
#endif
	case EGO_RING_MAGIC_MIS:
#ifdef JP
		return "�ޥ��å����ߥ�����(2d6) : 2 ��������";
#else
		return "magic missile (2d6) every 2 turns";
#endif
	case EGO_RING_FIRE_BOLT:
#ifdef JP
		return "�ե��������ܥ��(9d8) : 8+d8 ��������";
#else
		return "fire bolt (9d8) every 8+d8 turns";
#endif
	case EGO_RING_COLD_BOLT:
#ifdef JP
		return "���������ܥ��(6d8) : 7+d7 ��������";
#else
		return "frost bolt (6d8) every 7+d7 turns";
#endif
	case EGO_RING_ELEC_BOLT:
#ifdef JP
		return "����������ܥ��(4d8) : 5+d5 ��������";
#else
		return "lightning bolt (4d8) every 5+d5 turns";
#endif
	case EGO_RING_ACID_BOLT:
#ifdef JP
		return "�����åɡ��ܥ��(5d8) : 6+d6 ��������";
#else
		return "acid bolt (5d8) every 6+d6 turns";
#endif
	case EGO_RING_MANA_BOLT:
#ifdef JP
		return "���Ϥ���(120) : 120+d120 ��������";
#else
		return "a mana bolt (120) every 120+d120 turns";
#endif
	case EGO_RING_FIRE_BALL:
#ifdef JP
		return "�ե��������ܡ��� (100) : 80+d80 ��������";
#else
		return "fire ball (100) every 80+d80 turns";
#endif
	case EGO_RING_COLD_BALL:
#ifdef JP
		return "������ɡ��ܡ��� (100) : 80+d80 ��������";
#else
		return "cold ball (100) every 80+d80 turns";
#endif
	case EGO_RING_ELEC_BALL:
#ifdef JP
		return "����������ܡ��� (100) : 80+d80 ��������";
#else
		return "elec ball (100) every 80+d80 turns";
#endif
	case EGO_RING_ACID_BALL:
#ifdef JP
		return "�����åɡ��ܡ��� (100) : 80+d80 ��������";
#else
		return "acid ball (100) every 80+d80 turns";
#endif
	case EGO_RING_MANA_BALL:
#ifdef JP
		return "���Ϥ��� (250) : 300 ��������";
#else
		return "mana storm (250) every 300 turns";
#endif
	case EGO_RING_DRAGON_F:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_FLAMES))
#ifdef JP
			return "*�б�*�Υ֥쥹 (200) �ȲФؤ����� : 200 ��������";
#else
			return "breath of *fire* (200) and resist fire every 200 turns";
#endif
		else
#ifdef JP
			return "*�б�*�Υ֥쥹 (200) : 250 ��������";
#else
			return "*fire* breath (200) every 250 turns";
#endif
	case EGO_RING_DRAGON_C:
		if ((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_ICE))
#ifdef JP
			return "*��*�Υ֥쥹 (200) ���䵤�ؤ����� : 200 ��������";
#else
			return "breath of *aqua* (200) and resist cold every 200 turns";
#endif
		else
#ifdef JP
			return "*��*�Υ֥쥹 (200) : 250 ��������";
#else
			return "*aqua* breath (200) every 250 turns";
#endif
	case EGO_RING_D_SPEED:
#ifdef JP
		return "���ԡ���(15+d30������) : 100 ��������";
#else
		return "haste self (15+d30 turns) every 100 turns";
#endif
	case EGO_RING_BERSERKER:
#ifdef JP
		return "����β�(25+d25������) : 75+d75 ��������";
#else
		return "berserk (25+d25 turns) every 75+d75 turns";
#endif
	case EGO_RING_M_DETECT:
#ifdef JP
		return "����󥹥������� : 150 ��������";
#else
		return "detect all monsters every 150 turns";
#endif
	case EGO_RING_TELE_AWAY:
#ifdef JP
		return "�ƥ�ݡ��ȡ��������� : 150 ��������";
#else
		return "teleport away every 150 turns";
#endif
	case EGO_AMU_IDENT:
#ifdef JP
		return "���� : 10 ��������";
#else
		return "identify every 10 turns";
#endif
	case EGO_AMU_CHARM:
#ifdef JP
		return "��󥹥���̥λ : 200 ��������";
#else
		return "charm monster every 200 turns";
#endif
	case EGO_AMU_JUMP:
#ifdef JP
		return "���硼�ȡ��ƥ�ݡ��� : 10+d10 ��������";
#else
		return "blink every 10+d10 turns";
#endif
	case EGO_AMU_TELEPORT:
#ifdef JP
		return "�ƥ�ݡ��� : 50+d50 ��������";
#else
		return "teleport every 50+d50 turns";
#endif
	case EGO_AMU_D_DOOR:
#ifdef JP
		return "�������� : 200 ��������";
#else
		return "dimension door every 200 turns";
#endif
	case EGO_AMU_RES_FIRE_:
#ifdef JP
		return "�б�ؤ����� : 50+d50��������";
#else
		return "resist fire every 50+d50 turns";
#endif
	case EGO_AMU_RES_COLD_:
#ifdef JP
		return "�䵤�ؤ����� : 50+d50��������";
#else
		return "resist cold every 50+d50 turns";
#endif
	case EGO_AMU_RES_ELEC_:
#ifdef JP
		return "�ŷ�ؤ����� : 50+d50��������";
#else
		return "resist elec every 50+d50 turns";
#endif
	case EGO_AMU_RES_ACID_:
#ifdef JP
		return "���ؤ����� : 50+d50��������";
#else
		return "resist acid every 50+d50 turns";
#endif
	case EGO_AMU_DETECTION:
#ifdef JP
		return "������ : 55+d55��������";
#else
		return "detect all floor every 55+d55 turns";
#endif
	}

	/* Branch on the base item */
	switch (o_ptr->tval)
	{
	case TV_STONE:
#ifdef JP
		return "Υæ : ���٤���";
#else
		return "escape only once";
#endif
	case TV_LITE:
		if (o_ptr->sval == SV_LITE_MAGICAL_LAMP)
#ifdef JP
			return "�ǥ����ˤ˴ꤦ : ���٤���";
#else
			return "Wish to Djinni only once";
#endif
		break;
	case TV_AMULET:
		switch (o_ptr->sval)
		{
		case SV_AMULET_FOL:
#ifdef JP
			return "�ץ쥤������ͥ� : ���٤���";
#else
			return "pray fire element only once";
#endif
		case SV_AMULET_OHN:
#ifdef JP
			return "�ץ쥤���롼�� : ���٤���";
#else
			return "pray aqua element only once";
#endif
		case SV_AMULET_SOL:
#ifdef JP
			return "�ץ쥤�С��� : ���٤���";
#else
			return "pray earth element only once";
#endif
		case SV_AMULET_VAN:
#ifdef JP
			return "�ץ쥤�ϡ��ͥ� : ���٤���";
#else
			return "pray wind element only once";
#endif
		}
		break;
	case TV_RING:
		switch (o_ptr->sval)
		{
		case SV_RING_FLAMES:
#ifdef JP
			return "�ե��������ܡ��� (100) �ȲФؤ����� : 50+d50 ��������";
#else
			return "ball of fire (100) and resist fire every 50+d50 turns";
#endif
		case SV_RING_ICE:
#ifdef JP
			return "������ɡ��ܡ��� (100) ���䵤�ؤ����� : 50+d50 ��������";
#else
			return "ball of cold (100) and resist cold every 50+d50 turns";
#endif
		case SV_RING_ACID:
#ifdef JP
			return "�����åɡ��ܡ��� (100) �Ȼ��ؤ����� : 50+d50 ��������";
#else
			return "ball of acid (100) and resist acid every 50+d50 turns";
#endif
		case SV_RING_ELEC:
#ifdef JP
			return "����������ܡ��� (100) ���ŷ�ؤ����� : 50+d50 ��������";
#else
			return "ball of elec (100) and resist elec every 50+d50 turns";
#endif
		}
		break;
	case TV_TRUMP:
#ifdef JP
		return "��󥹥������ᤨ�롢���ϲ������롣";
#else
		return "captures or releases a monster.";
#endif
	}

#ifdef JP
	return ("��̯�ʸ�");
#else
	return ("a strange glow");
#endif
}


static char *affect_stat_text[A_MAX] =
{
#ifdef JP
	"��������Ϥ˱ƶ���ڤܤ���",
	"�������ǽ�˱ƶ���ڤܤ���",
	"����ϸ����˱ƶ���ڤܤ���",
	"����ϴ��Ѥ��˱ƶ���ڤܤ���",
	"������ѵ��Ϥ˱ƶ���ڤܤ���",
	"�����̥�Ϥ˱ƶ���ڤܤ���",
#else
	"It affects your strength.",
	"It affects your intelligence.",
	"It affects your wisdom.",
	"It affects your dexterity.",
	"It affects your constitution.",
	"It affects your charisma.",
#endif
};

static char *affect_misc_text[OB_MAX] =
{
#ifdef JP
	"�������ˡƻ�����ǽ�Ϥ˱ƶ���ڤܤ���",
	"����ϱ�̩��ưǽ�Ϥ˱ƶ���ڤܤ���",
	"�����õ��ǽ�Ϥ˱ƶ���ڤܤ���",
	"������ֳ������Ϥ˱ƶ���ڤܤ���",
	"����Ϻη�ǽ�Ϥ˱ƶ���ڤܤ���",
	"����ϥ��ԡ��ɤ˱ƶ���ڤܤ���",
	"������Ƿ����˱ƶ���ڤܤ���",
	"�����ȿ��ˡ�ե������Ⱦ�¤˱ƶ���ڤܤ���",
#else
	"It affects your ability to use magic devices.",
	"It affects your stealth.",
	"It affects your searching.",
	"It affects your infravision.",
	"It affects your ability to tunnel.",
	"It affects your speed.",
	"It affects your attack speed.",
	"It affects anti-magic field radius.",
#endif
};

static char *print_affect(char *buf, char *text, int val)
{
	sprintf(buf, "%s(%c%d)", text, ((val >= 0) ? '+' : '-'), ABS(val));

	return buf;
}

/*
 * Describe a "fully identified" item
 */
bool screen_object(object_type *o_ptr, FILE *fff, bool real)
{
	int i = 0, j, k;
	int trivial_info = 0;

	u32b flgs[TR_FLAG_SIZE];

	cptr info[128];
	char o_name[MAX_NLEN];
	int wid, hgt;

	char affect_stat_buf[A_MAX][128];
	char affect_misc_buf[OB_MAX][128];
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Extract the description */
	if (object_is_snapdragon_runeweapon(o_ptr))
	{
		runeweapon_type *runeweapon = &runeweapon_list[o_ptr->xtra3];
		bool show_history = FALSE;

		for (j = 0; j < 4; j++)
		{
			if (strlen(runeweapon->history[j])) show_history = TRUE;
		}

		if (show_history)
		{
			char intro_msg[80];
#ifdef JP
			sprintf(intro_msg, "����%s������������Ω������ޤ�Ƥ���...", runeweapon->ancestor);
#else
			sprintf(intro_msg, "Inscribed background of ancestor %s...", runeweapon->ancestor);
#endif
			info[i++] = intro_msg;
			for (j = 0; j < 4; j++) info[i++] = runeweapon->history[j];
		}
	}
	else
	{
		char temp[70 * 20];
		cptr text_ptr;

		if (o_ptr->name1) text_ptr = a_text + a_info[o_ptr->name1].text;
		else if (o_ptr->tval == TV_TAROT) text_ptr = tarot_info[o_ptr->pval].text;
		else text_ptr = k_text + k_info[lookup_kind(o_ptr->tval, o_ptr->sval)].text;

		roff_to_buf(text_ptr, 77 - 15, temp, sizeof temp);
		for (j = 0; temp[j]; j += 1 + strlen(&temp[j]))
		{ info[i] = &temp[j]; i++;}
	}

	if (object_is_equipment(o_ptr))
	{
		/* Descriptions of a basic equipment is just a flavor */
		trivial_info = i;
	}

	/* Mega-Hack -- describe activation */
	if (have_flag(flgs, TR_ACTIVATE))
	{
#ifdef JP
		info[i++] = "��ư�����Ȥ��θ���...";
#else
		info[i++] = "It can be activated for...";
#endif

		info[i++] = item_activation(o_ptr);
#ifdef JP
		info[i++] = "...�������������Ƥ��ʤ���Фʤ�ʤ���";
#else
		info[i++] = "...if it is being worn.";
#endif

	}

	/* If you equip them, you will be revived. */
	if ((o_ptr->name1 == ART_BLESSING) || (o_ptr->name1 == ART_BLISS))
	{
#ifdef JP
		info[i++] = "������������Ƥ���Ȼ��Ǥ�1�٤�������Ǥ��롣";
#else
		info[i++] = "You can be revived only once if it is being worn.";
#endif

	}

	/* If you equip this, you will be reincarnated. */
	if (o_ptr->name1 == ART_LICH)
	{
#ifdef JP
		info[i++] = "������������ƻ�̤ȥ�å��Ȥ���ž���Ǥ����礬���롣";
#else
		info[i++] = "You may be reincarnated as Lich if it is being worn.";
#endif
	}

	/* Sexual restriction */
	if (have_flag(flgs, TR_FEMALE_ONLY))
	{
#ifdef JP
		info[i++] = "����Ͻ����Τߤ������Ǥ��롣";
#else
		info[i++] = "It is for ladies only.";
#endif
	}
	if (have_flag(flgs, TR_MALE_ONLY))
	{
#ifdef JP
		info[i++] = "����������Τߤ������Ǥ��롣";
#else
		info[i++] = "It is for mens only.";
#endif
	}

	/* Figurines, a hack */
	if (o_ptr->tval == TV_FIGURINE)
	{
#ifdef JP
		info[i++] = "������ꤲ�����ڥåȤ��Ѳ����롣";
#else
		info[i++] = "It will transform into a pet when thrown.";
#endif

	}

	if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE))
	{
#ifdef JP
info[i++] = "����ϼ�ʬ���Ȥ˹��⤬�֤äƤ��뤳�Ȥ����롣";
#else
		info[i++] = "It causes you to strike yourself sometimes.";
#endif

#ifdef JP
info[i++] = "�����̵Ũ�ΥХꥢ���ڤ�������";
#else
		info[i++] = "It always penetrates invulnerability barriers.";
#endif
	}

	if (have_flag(flgs, TR_EASY_SPELL))
	{
#ifdef JP
		info[i++] = "�������ˡ������٤򲼤��롣";
#else
		info[i++] = "It affects your ability to cast spells.";
#endif
	}

	if (o_ptr->name2 == EGO_AMU_FOOL)
	{
#ifdef JP
		info[i++] = "�������ˡ������٤�夲�롣";
#else
		info[i++] = "It interferes with casting spells.";
#endif
	}

	if (o_ptr->name2 == EGO_RING_THROW)
	{
#ifdef JP
		info[i++] = "�����ʪ�򶯤��ꤲ�뤳�Ȥ��ǽ�ˤ��롣";
#else
		info[i++] = "It provides great strength when you throw an item.";
#endif
	}

	if (o_ptr->name2 == EGO_AMU_NAIVETY)
	{
#ifdef JP
		info[i++] = "�������ˡ���Ϥ򲼤��롣";
#else
		info[i++] = "It decreases your magic resistance.";
#endif
	}

	if (o_ptr->tval == TV_STATUE)
	{
#ifdef JP
		info[i++] = "����������˾���ȳڤ�����";
#else
		info[i++] = "It is cheerful.";
#endif
	}

	/* Hack -- describe lite's */
	if (o_ptr->tval == TV_LITE)
	{
		if (o_ptr->name2 == EGO_LITE_DARKNESS)
		{
			if (o_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
				info[i++] = "������������Ⱦ�¤򶹤��(Ⱦ�¤�-3)��";
#else
				info[i++] = "It decreases radius of light source by 3.";
#endif
			}
			else if (o_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
				info[i++] = "������������Ⱦ�¤򶹤��(Ⱦ�¤�-2)��";
#else
				info[i++] = "It decreases radius of light source by 2.";
#endif
			}
			else
			{
#ifdef JP
				info[i++] = "������������Ⱦ�¤򶹤��(Ⱦ�¤�-1)��";
#else
				info[i++] = "It decreases radius of light source by 1.";
#endif
			}
		}
		else if (artifact_p(o_ptr) || (o_ptr->sval == SV_LITE_MAGICAL_LAMP))
		{
#ifdef JP
info[i++] = "����ϱʱ�ʤ�������(Ⱦ�� 3)������롣";
#else
			info[i++] = "It provides light (radius 3) forever.";
#endif

		}
		else if (o_ptr->name2 == EGO_LITE_SHINE)
		{
			if (o_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
info[i++] = "����ϱʱ�ʤ�������(Ⱦ�� 3)������롣";
#else
				info[i++] = "It provides light (radius 3) forever.";
#endif

			}
			else if (o_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
info[i++] = "�����ǳ�����ˤ�ä�������(Ⱦ�� 3)������롣";
#else
				info[i++] = "It provides light (radius 3) when fueled.";
#endif

			}
			else
			{
#ifdef JP
info[i++] = "�����ǳ�����ˤ�ä�������(Ⱦ�� 2)������롣";
#else
				info[i++] = "It provides light (radius 2) when fueled.";
#endif

			}
		}
		else
		{
			if (o_ptr->sval == SV_LITE_FEANOR)
			{
#ifdef JP
info[i++] = "����ϱʱ�ʤ�������(Ⱦ�� 2)������롣";
#else
				info[i++] = "It provides light (radius 2) forever.";
#endif

			}
			else if (o_ptr->sval == SV_LITE_LANTERN)
			{
#ifdef JP
info[i++] = "�����ǳ�����ˤ�ä�������(Ⱦ�� 2)������롣";
#else
				info[i++] = "It provides light (radius 2) when fueled.";
#endif

			}
			else if (o_ptr->sval == SV_LITE_EMPTY)
			{
				if (have_flag(o_ptr->art_flags, TR_LITE))
				{
					info[i++] = "����ϱʱ�ʤ�������(Ⱦ�� 2)������롣";
				}
				else
				{
					info[i++] = "����ϸ���ʤ���";
				}
			}
			else
			{
#ifdef JP
				info[i++] = "�����ǳ�����ˤ�ä�������(Ⱦ�� 1)������롣";
#else
				info[i++] = "It provides light (radius 1) when fueled.";
#endif

			}
		}
		if (o_ptr->name2 == EGO_LITE_LONG)
		{
#ifdef JP
			info[i++] = "�����Ĺ�������������������롣";
#else
			info[i++] = "It provides light for much longer time.";
#endif
		}
	}

	if (have_flag(flgs, TR_WRAITH))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ�����ʪ�������롣";
#else
		info[i++] = "It renders you incorporeal.";
#endif
	}


	/* And then describe it fully */

	if (have_flag(flgs, TR_RIDING))
	{
		if ((o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
#ifdef JP
			info[i++] = "����Ͼ���������˻Ȥ��䤹����";
#else
			info[i++] = "It is made for use while riding.";
#endif
		else
#ifdef JP
			info[i++] = "����Ͼ�����Ǥ�Ȥ��䤹����";
#else
			info[i++] = "It is suitable for use while riding.";
#endif

	}

	for (j = 0; j < A_MAX; j++)
	{
		if (have_flag(flgs, a_to_tr[j]))
		{
			if (real) info[i++] = print_affect(affect_stat_buf[j], affect_stat_text[j], o_ptr->to_stat[j]);
			else info[i++] = affect_stat_text[j];
		}
	}
	for (j = 0; j < OB_MAX; j++)
	{
		if (have_flag(flgs, ob_to_tr[j]))
		{
			if (real) info[i++] = print_affect(affect_misc_buf[j], affect_misc_text[j], o_ptr->to_misc[j]);
			else info[i++] = affect_misc_text[j];
		}
	}

	if (have_flag(flgs, TR_BRAND_ACID))
	{
#ifdef JP
		info[i++] = "����ϻ��ˤ�ä��礭�ʥ��᡼����Ϳ���롣";
#else
		info[i++] = "It does extra damage from acid.";
#endif

	}
	if (have_flag(flgs, TR_BRAND_ELEC))
	{
#ifdef JP
		info[i++] = "������ŷ�ˤ�ä��礭�ʥ��᡼����Ϳ���롣";
#else
		info[i++] = "It does extra damage from electricity.";
#endif

	}
	if (have_flag(flgs, TR_BRAND_FIRE))
	{
#ifdef JP
		info[i++] = "����ϲб�ˤ�ä��礭�ʥ��᡼����Ϳ���롣";
#else
		info[i++] = "It does extra damage from fire.";
#endif

	}
	if (have_flag(flgs, TR_BRAND_COLD))
	{
#ifdef JP
		info[i++] = "������䵤�ˤ�ä��礭�ʥ��᡼����Ϳ���롣";
#else
		info[i++] = "It does extra damage from frost.";
#endif

	}

	if (have_flag(flgs, TR_BRAND_POIS))
	{
#ifdef JP
		info[i++] = "�����Ũ���Ǥ��롣";
#else
		info[i++] = "It poisons your foes.";
#endif

	}

	if (have_flag(flgs, TR_CHAOTIC))
	{
#ifdef JP
		info[i++] = "����ϥ�����Ū�ʸ��̤�ڤܤ���";
#else
		info[i++] = "It produces chaotic effects.";
#endif

	}

	if (have_flag(flgs, TR_VAMPIRIC))
	{
#ifdef JP
info[i++] = "�����Ũ����ҥåȥݥ���Ȥ�ۼ����롣";
#else
		info[i++] = "It drains life from your foes.";
#endif

	}

	if (have_flag(flgs, TR_IMPACT))
	{
#ifdef JP
		info[i++] = "������Ͽ̤򵯤������Ȥ��Ǥ��롣";
#else
		info[i++] = "It can cause earthquakes.";
#endif

	}

	if (o_ptr->name2 == EGO_EARTHQUAKES)
	{
#ifdef JP
		info[i++] = "�����Ũ��ۯ۰�Ȥ����롣";
#else
		info[i++] = "It stuns your foes.";
#endif

	}

	if (have_flag(flgs, TR_EXTRA_VORPAL))
	{
#ifdef JP
		info[i++] = "���������Ū���ڤ�̣���Ԥ�Ũ����ڤ�ˤ��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "It is legendary sharp and can slice your foes.";
#endif

	}
	else if (have_flag(flgs, TR_VORPAL))
	{
#ifdef JP
		info[i++] = "����������ڤ�̣���Ԥ�Ũ�����Ǥ��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "It is very sharp and can cut your foes.";
#endif

	}

	if (have_flag(flgs, TR_KILL_DRAGON))
	{
#ifdef JP
		info[i++] = "����ϥɥ饴��ˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of dragons.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_DRAGON))
	{
#ifdef JP
		info[i++] = "����ϥɥ饴����Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against dragons.";
#endif

	}

	if (have_flag(flgs, TR_KILL_ORC))
	{
#ifdef JP
		info[i++] = "����ϥ������ˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of orcs.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_ORC))
	{
#ifdef JP
		info[i++] = "����ϥ��������Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against orcs.";
#endif

	}

	if (have_flag(flgs, TR_KILL_TROLL))
	{
#ifdef JP
		info[i++] = "����ϥȥ��ˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of trolls.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_TROLL))
	{
#ifdef JP
		info[i++] = "����ϥȥ����Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against trolls.";
#endif

	}

	if (have_flag(flgs, TR_KILL_GIANT))
	{
#ifdef JP
		info[i++] = "����ϵ�ͤˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of giants.";
#endif
	}
	else if (have_flag(flgs, TR_SLAY_GIANT))
	{
#ifdef JP
		info[i++] = "����ϥ��㥤����Ȥ��Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against giants.";
#endif

	}

	if (have_flag(flgs, TR_KILL_DEMON))
	{
#ifdef JP
		info[i++] = "����ϥǡ����ˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of demons.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_DEMON))
	{
#ifdef JP
		info[i++] = "����ϥǡ������Ф������ʤ��Ϥ�ȯ�����롣";
#else
		info[i++] = "It strikes at demons with holy wrath.";
#endif

	}

	if (have_flag(flgs, TR_KILL_UNDEAD))
	{
#ifdef JP
		info[i++] = "����ϥ���ǥåɤˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of undead.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_UNDEAD))
	{
#ifdef JP
		info[i++] = "����ϥ���ǥåɤ��Ф������ʤ��Ϥ�ȯ�����롣";
#else
		info[i++] = "It strikes at undead with holy wrath.";
#endif

	}

	if (have_flag(flgs, TR_KILL_LIVING))
	{
#ifdef JP
		info[i++] = "�������̿�Τ���ԤˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of living creatures.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_LIVING))
	{
#ifdef JP
		info[i++] = "�������̿�Τ���Ԥ��Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against living creatures.";
#endif

	}

	if (have_flag(flgs, TR_KILL_GOOD))
	{
#ifdef JP
		info[i++] = "��������ɤ�¸�ߤˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of good monsters.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_GOOD))
	{
#ifdef JP
		info[i++] = "��������ɤ�¸�ߤ��Ф��Ƽٰ����Ϥǹ��⤹�롣";
#else
		info[i++] = "It fights against good with unholy fury.";
#endif

	}

	if (have_flag(flgs, TR_KILL_EVIL))
	{
#ifdef JP
		info[i++] = "����ϼٰ��ʤ�¸�ߤˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of evil monsters.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_EVIL))
	{
#ifdef JP
		info[i++] = "����ϼٰ��ʤ�¸�ߤ��Ф������ʤ��Ϥǹ��⤹�롣";
#else
		info[i++] = "It fights against evil with holy fury.";
#endif

	}

	if (have_flag(flgs, TR_KILL_ANIMAL))
	{
#ifdef JP
		info[i++] = "����ϼ�������ưʪ�ˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of natural creatures.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_ANIMAL))
	{
#ifdef JP
		info[i++] = "����ϼ�������ưʪ���Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against natural creatures.";
#endif

	}

	if (have_flag(flgs, TR_KILL_HUMAN))
	{
#ifdef JP
		info[i++] = "����Ͽʹ֤ˤȤäƤ�ŷŨ�Ǥ��롣";
#else
		info[i++] = "It is a great bane of humans.";
#endif

	}
	else if (have_flag(flgs, TR_SLAY_HUMAN))
	{
#ifdef JP
		info[i++] = "����Ͽʹ֤��Ф����ä˶���٤��Ϥ�ȯ�����롣";
#else
		info[i++] = "It is especially deadly against humans.";
#endif

	}

	if (have_flag(flgs, TR_FORCE_WEAPON))
	{
#ifdef JP
		info[i++] = "����ϻ��ѼԤ����Ϥ�Ȥäƹ��⤹�롣";
#else
		info[i++] = "It powerfully strikes at a monster using your mana.";
#endif

	}
	if (have_flag(flgs, TR_DEC_MANA))
	{
#ifdef JP
		info[i++] = "��������Ϥξ���򲡤����롣";
#else
		info[i++] = "It decreases your mana consumption.";
#endif

	}
	if (have_flag(flgs, TR_SUST_STR))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ������Ϥ�ݻ����롣";
#else
		info[i++] = "It sustains your strength.";
#endif

	}
	if (have_flag(flgs, TR_SUST_INT))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ�����ǽ��ݻ����롣";
#else
		info[i++] = "It sustains your intelligence.";
#endif

	}
	if (have_flag(flgs, TR_SUST_WIS))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ��θ�����ݻ����롣";
#else
		info[i++] = "It sustains your wisdom.";
#endif

	}
	if (have_flag(flgs, TR_SUST_DEX))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ��δ��Ѥ���ݻ����롣";
#else
		info[i++] = "It sustains your dexterity.";
#endif

	}
	if (have_flag(flgs, TR_SUST_CON))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ����ѵ��Ϥ�ݻ����롣";
#else
		info[i++] = "It sustains your constitution.";
#endif

	}
	if (have_flag(flgs, TR_SUST_CHR))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ���̥�Ϥ�ݻ����롣";
#else
		info[i++] = "It sustains your charisma.";
#endif

	}

	if (have_flag(flgs, TR_IM_ACID))
	{
#ifdef JP
		info[i++] = "����ϻ����Ф��봰�����ȱ֤�����롣";
#else
		info[i++] = "It provides immunity to acid.";
#endif

	}
	if (have_flag(flgs, TR_IM_ELEC))
	{
#ifdef JP
		info[i++] = "������ŷ���Ф��봰�����ȱ֤�����롣";
#else
		info[i++] = "It provides immunity to electricity.";
#endif

	}
	if (have_flag(flgs, TR_IM_FIRE))
	{
#ifdef JP
		info[i++] = "����ϲФ��Ф��봰�����ȱ֤�����롣";
#else
		info[i++] = "It provides immunity to fire.";
#endif

	}
	if (have_flag(flgs, TR_IM_COLD))
	{
#ifdef JP
		info[i++] = "����ϴ������Ф��봰�����ȱ֤�����롣";
#else
		info[i++] = "It provides immunity to cold.";
#endif

	}

	if (have_flag(flgs, TR_THROW))
	{
#ifdef JP
		info[i++] = "�����Ũ���ꤲ���礭�ʥ��᡼����Ϳ���뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "It is perfectly balanced for throwing.";
#endif
	}

	if (have_flag(flgs, TR_FREE_ACT))
	{
#ifdef JP
		info[i++] = "�����������Ф��봰�����ȱ֤�����롣";
#else
		info[i++] = "It provides immunity to paralysis.";
#endif

	}
	if (have_flag(flgs, TR_HOLD_LIFE))
	{
#ifdef JP
		info[i++] = "�������̿�ϵۼ����Ф�������������롣";
#else
		info[i++] = "It provides resistance to life draining.";
#endif

	}
	if (have_flag(flgs, TR_RES_FEAR))
	{
#ifdef JP
		info[i++] = "����϶��ݤؤδ���������������롣";
#else
		info[i++] = "It makes you completely fearless.";
#endif

	}
	if (have_flag(flgs, TR_RES_ACID))
	{
#ifdef JP
		info[i++] = "����ϻ��ؤ�����������롣";
#else
		info[i++] = "It provides resistance to acid.";
#endif

	}
	if (have_flag(flgs, TR_RES_ELEC))
	{
#ifdef JP
		info[i++] = "������ŷ�ؤ�����������롣";
#else
		info[i++] = "It provides resistance to electricity.";
#endif

	}
	if (have_flag(flgs, TR_RES_FIRE))
	{
#ifdef JP
		info[i++] = "����ϲФؤ�����������롣";
#else
		info[i++] = "It provides resistance to fire.";
#endif

	}
	if (have_flag(flgs, TR_RES_COLD))
	{
#ifdef JP
		info[i++] = "����ϴ����ؤ�����������롣";
#else
		info[i++] = "It provides resistance to cold.";
#endif

	}
	if (have_flag(flgs, TR_RES_POIS))
	{
#ifdef JP
		info[i++] = "������Ǥؤ�����������롣";
#else
		info[i++] = "It provides resistance to poison.";
#endif

	}

	if (have_flag(flgs, TR_RES_LITE))
	{
#ifdef JP
		info[i++] = "����������ؤ�����������롣";
#else
		info[i++] = "It provides resistance to light.";
#endif

	}
	if (have_flag(flgs, TR_RES_DARK))
	{
#ifdef JP
		info[i++] = "����ϰŹ��ؤ�����������롣";
#else
		info[i++] = "It provides resistance to dark.";
#endif

	}

	if (have_flag(flgs, TR_RES_BLIND))
	{
#ifdef JP
		info[i++] = "��������ܤؤ�����������롣";
#else
		info[i++] = "It provides resistance to blindness.";
#endif

	}
	if (have_flag(flgs, TR_RES_CONF))
	{
#ifdef JP
		info[i++] = "����Ϻ���ؤ�����������롣";
#else
		info[i++] = "It provides resistance to confusion.";
#endif

	}
	if (have_flag(flgs, TR_RES_SOUND))
	{
#ifdef JP
		info[i++] = "����Ϲ첻�ؤ�����������롣";
#else
		info[i++] = "It provides resistance to sound.";
#endif

	}
	if (have_flag(flgs, TR_RES_SHARDS))
	{
#ifdef JP
		info[i++] = "��������Ҥؤ�����������롣";
#else
		info[i++] = "It provides resistance to shards.";
#endif

	}

	if (have_flag(flgs, TR_RES_NETHER))
	{
#ifdef JP
		info[i++] = "������Ϲ��ؤ�����������롣";
#else
		info[i++] = "It provides resistance to nether.";
#endif

	}
	if (have_flag(flgs, TR_RES_STONE))
	{
#ifdef JP
		info[i++] = "������в��ؤ�����������롣";
#else
		info[i++] = "It provides resistance to stone.";
#endif

	}
	if (have_flag(flgs, TR_RES_CHAOS))
	{
#ifdef JP
		info[i++] = "����ϥ������ؤ�����������롣";
#else
		info[i++] = "It provides resistance to chaos.";
#endif

	}
	if (have_flag(flgs, TR_RES_DISEN))
	{
#ifdef JP
		info[i++] = "����������ؤ�����������롣";
#else
		info[i++] = "It provides resistance to disenchantment.";
#endif

	}
	if ((o_ptr->tval == TV_CLOAK) && (o_ptr->sval == SV_RAINCOAT))
	{
#ifdef JP
		info[i++] = "����Ͽ�ؤ�����������롣";
#else
		info[i++] = "It provides resistance to water.";
#endif

	}

	if (o_ptr->name2 == EGO_BAT)
	{
#ifdef JP
		info[i++] = "����ϰŻ�ǽ�Ϥ�����롣";
#else
		info[i++] = "It provides resistance to water.";
#endif

	}

	if (have_flag(flgs, TR_RES_MAGIC))
	{
#ifdef JP
		info[i++] = "�������ˡ�ؤ�����������롣";
#else
		info[i++] = "It provides resistance to magic.";
#endif
	}

	if (have_flag(flgs, TR_FEATHER))
	{
#ifdef JP
		info[i++] = "���������⤯���Ȥ��ǽ�ˤ��롣";
#else
		info[i++] = "It allows you to levitate.";
#endif

	}
	if ((have_flag(flgs, TR_LITE)) && (o_ptr->sval != SV_LITE_EMPTY))
	{
		if ((o_ptr->name2 == EGO_DARK) || (o_ptr->name1 == ART_NIGHT))
#ifdef JP
			info[i++] = "������������Ⱦ�¤򶹤��(Ⱦ�¤�-1)��";
#else
			info[i++] = "It decreases radius of your light source by 1.";
#endif
		else
#ifdef JP
			info[i++] = "����ϱʱ��������������(Ⱦ�¤�+1)��";
#else
			info[i++] = "It provides permanent light. (radius +1)";
#endif

	}
	if (have_flag(flgs, TR_SEE_INVIS))
	{
#ifdef JP
		info[i++] = "�����Ʃ���ʥ�󥹥����򸫤뤳�Ȥ��ǽ�ˤ��롣";
#else
		info[i++] = "It allows you to see invisible monsters.";
#endif

	}
	if (have_flag(flgs, TR_TELEPATHY))
	{
#ifdef JP
		info[i++] = "����ϥƥ�ѥ���ǽ�Ϥ�����롣";
#else
		info[i++] = "It gives telepathic powers.";
#endif

	}
	if (have_flag(flgs, TR_SLOW_DIGEST))
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ��ο�����դ��٤����롣";
#else
		info[i++] = "It slows your metabolism.";
#endif

	}
	if (have_flag(flgs, TR_REGEN))
	{
#ifdef JP
		info[i++] = "��������ϲ����Ϥ򶯲����롣";
#else
		info[i++] = "It speeds your regenerative powers.";
#endif

	}
	if (have_flag(flgs, TR_REGEN_MANA))
	{
#ifdef JP
		info[i++] = "��������ϲ����Ϥ򶯲����롣";
#else
		info[i++] = "It speeds your mana regenerative powers.";
#endif

	}
	if (have_flag(flgs, TR_WARNING))
	{
#ifdef JP
		info[i++] = "����ϴ����Ф��Ʒٹ��ȯ���롣";
#else
		info[i++] = "It warns you of danger";
#endif

	}
	if (have_flag(flgs, TR_REFLECT))
	{
#ifdef JP
		info[i++] = "��������ܥ�Ȥ�ȿ�ͤ��롣";
#else
		info[i++] = "It reflects bolts and arrows.";
#endif

	}
	if (have_flag(flgs, TR_SH_FIRE))
	{
#ifdef JP
		info[i++] = "����ϱ�ΥХꥢ��ĥ�롣";
#else
		info[i++] = "It produces a fiery sheath.";
#endif

	}
	if (have_flag(flgs, TR_SH_ELEC))
	{
#ifdef JP
		info[i++] = "������ŵ��ΥХꥢ��ĥ�롣";
#else
		info[i++] = "It produces an electric sheath.";
#endif

	}
	if (have_flag(flgs, TR_SH_COLD))
	{
#ifdef JP
		info[i++] = "������䵤�ΥХꥢ��ĥ�롣";
#else
		info[i++] = "It produces a sheath of coldness.";
#endif

	}
	if (have_flag(flgs, TR_NO_MAGIC))
	{
#ifdef JP
		info[i++] = "�����ȿ��ˡ�Хꥢ��ĥ�롣";
#else
		info[i++] = "It produces an anti-magic shell.";
#endif

	}
	if (have_flag(flgs, TR_NO_TELE))
	{
#ifdef JP
		info[i++] = "����ϥƥ�ݡ��Ȥ���⤹�롣";
#else
		info[i++] = "It prevents teleportation.";
#endif

	}
	if (have_flag(flgs, TR_XTRA_MIGHT))
	{
#ifdef JP
		info[i++] = "������𡿥ܥ�ȡ��Ƥ��궯�Ϥ�ȯ�ͤ��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "It fires missiles with extra might.";
#endif

	}
	if (have_flag(flgs, TR_XTRA_SHOTS))
	{
#ifdef JP
		info[i++] = "������𡿥ܥ�ȡ��Ƥ������᤯ȯ�ͤ��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "It fires missiles excessively fast.";
#endif

	}

	if (have_flag(flgs, TR_BLESSED))
	{
#ifdef JP
		info[i++] = "����Ͽ��˽�ʡ����Ƥ��롣";
#else
		info[i++] = "It has been blessed by the gods.";
#endif

	}
	if (have_flag(flgs, TR_UNHOLY))
	{
#ifdef JP
		info[i++] = "����ϼٰ����Ҥ�Ƥ��롣";
#else
		info[i++] = "It has been poluted by evil.";
#endif

	}

	if (have_flag(flgs, TR_FEAR_FIELD))
	{
#ifdef JP
		info[i++] = "�����Ũ�Ф���Ԥι������붲�ݥե�����ɤ�ĥ�롣";
#else
		info[i++] = "It produces a fear field decreases opponents' attacks.";
#endif

	}

	if (o_ptr->tval == TV_ROCKET)
	{
		switch ((int)o_ptr->xtra3 - 1)
		{
		case TR_FORCE_WEAPON:
			info[i++] = "��������Ϥ������򵯤�����";
			break;

		case TR_CHAOTIC:
			info[i++] = "����ϥ������������򵯤�����";
			break;

		case TR_VAMPIRIC:
			info[i++] = "�������̿�Ϥ�å�������򵯤�����";
			break;

		case TR_SLAY_EVIL:
			info[i++] = "����Ͽ������Ϥ������򵯤�����";
			break;

		case TR_IMPACT:
			info[i++] = "������Ϸ���ʴ�դ��롣";
			break;

		case TR_BRAND_POIS:
			info[i++] = "������ǥ����������򵯤�����";
			break;

		case TR_BRAND_ACID:
			info[i++] = "����ϻ��������򵯤�����";
			break;

		case TR_BRAND_ELEC:
			info[i++] = "����ϰ�ʤ������򵯤�����";
			break;

		case TR_BRAND_FIRE:
			info[i++] = "����ϲб�������򵯤�����";
			break;

		case TR_BRAND_COLD:
			info[i++] = "������䵤�������򵯤�����";
			break;

		case TR_SLAY_GOOD:
			info[i++] = "����ϼٰ����Ϥ������򵯤�����";
			break;

		default:
			if (o_ptr->xtra4 >= ROCKET_ANTIGRAV)
				info[i++] = "����Ͻ��Ϥ������򵯤�����";
			break;
		}
	}

	if (cursed_p(o_ptr))
	{
		if (o_ptr->curse_flags & TRC_PERMA_CURSE)
		{
#ifdef JP
			info[i++] = "����ϱʱ�μ������������Ƥ��롣";
#else
			info[i++] = "It is permanently cursed.";
#endif

		}
		else if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
		{
#ifdef JP
			info[i++] = "����϶��Ϥʼ������������Ƥ��롣";
#else
			info[i++] = "It is heavily cursed.";
#endif

		}
		else
		{
#ifdef JP
			info[i++] = "����ϼ����Ƥ��롣";
#else
			info[i++] = "It is cursed.";
#endif

		}
	}

	if ((have_flag(flgs, TR_TY_CURSE)) || (o_ptr->curse_flags & TRC_TY_CURSE))
	{
#ifdef JP
		info[i++] = "��������Ťβҡ�������ǰ���ɤäƤ��롣";
#else
		info[i++] = "It carries an ancient foul curse.";
#endif

	}
	if ((have_flag(flgs, TR_AGGRAVATE)) || (o_ptr->curse_flags & TRC_AGGRAVATE))
	{
#ifdef JP
		info[i++] = "������ն�Υ�󥹥������ܤ餻�롣";
#else
		info[i++] = "It aggravates nearby creatures.";
#endif

	}
	if ((have_flag(flgs, TR_DRAIN_EXP)) || (o_ptr->curse_flags & TRC_DRAIN_EXP))
	{
#ifdef JP
		info[i++] = "����Ϸи��ͤ�ۤ���롣";
#else
		info[i++] = "It drains experience.";
#endif

	}
	if (o_ptr->curse_flags & TRC_SLOW_REGEN)
	{
#ifdef JP
		info[i++] = "����ϲ����Ϥ���롣";
#else
		info[i++] = "It slows your regenerative powers.";
#endif

	}
	if (o_ptr->curse_flags & TRC_ADD_L_CURSE)
	{
#ifdef JP
		info[i++] = "����ϼ夤���������䤹��";
#else
		info[i++] = "It adds weak curses.";
#endif

	}
	if (o_ptr->curse_flags & TRC_ADD_H_CURSE)
	{
#ifdef JP
		info[i++] = "����϶��Ϥʼ��������䤹��";
#else
		info[i++] = "It adds heavy curses.";
#endif

	}
	if (o_ptr->curse_flags & TRC_CALL_ANIMAL)
	{
#ifdef JP
		info[i++] = "�����ưʪ��ƤӴ󤻤롣";
#else
		info[i++] = "It attracts animals.";
#endif

	}
	if (o_ptr->curse_flags & TRC_CALL_DEMON)
	{
#ifdef JP
		info[i++] = "����ϰ����ƤӴ󤻤롣";
#else
		info[i++] = "It attracts demons.";
#endif

	}
	if (o_ptr->curse_flags & TRC_CALL_DRAGON)
	{
#ifdef JP
		info[i++] = "����ϥɥ饴���ƤӴ󤻤롣";
#else
		info[i++] = "It attracts dragons.";
#endif

	}
	if (o_ptr->curse_flags & TRC_COWARDICE)
	{
#ifdef JP
		info[i++] = "����϶��ݴ��������������";
#else
		info[i++] = "It makes you subject to cowardice.";
#endif

	}
	if ((have_flag(flgs, TR_TELEPORT)) || (o_ptr->curse_flags & TRC_TELEPORT))
	{
#ifdef JP
		info[i++] = "����ϥ�����ʥƥ�ݡ��Ȥ������������";
#else
		info[i++] = "It induces random teleportation.";
#endif

	}
	if (o_ptr->curse_flags & TRC_LOW_MELEE)
	{
#ifdef JP
		info[i++] = "����Ϲ���򳰤��䤹����";
#else
		info[i++] = "It causes you to miss blows.";
#endif

	}
	if (o_ptr->curse_flags & TRC_LOW_AC)
	{
#ifdef JP
		info[i++] = "����Ϲ��������䤹����";
#else
		info[i++] = "It helps your enemies' blows.";
#endif

	}
	if (o_ptr->curse_flags & TRC_LOW_MAGIC)
	{
#ifdef JP
		info[i++] = "�������ˡ�򾧤��ˤ������롣";
#else
		info[i++] = "It encumbers you while spellcasting.";
#endif

	}
	if (o_ptr->curse_flags & TRC_FAST_DIGEST)
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ��ο�����դ�®�����롣";
#else
		info[i++] = "It speeds your metabolism.";
#endif

	}
	if (o_ptr->curse_flags & TRC_DRAIN_HP)
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ������Ϥ�ۤ���롣";
#else
		info[i++] = "It drains you.";
#endif

	}
	if (o_ptr->curse_flags & TRC_DRAIN_MANA)
	{
#ifdef JP
		info[i++] = "����Ϥ��ʤ������Ϥ�ۤ���롣";
#else
		info[i++] = "It drains your mana.";
#endif

	}

	switch (get_object_ethnicity(o_ptr))
	{
#ifdef JP
	case ETHNICITY_WALSTANIAN:
		info[i++] = "����ϥ����륹���˴ؤ�꤬���롣";
		break;

	case ETHNICITY_GARGASTAN:
		info[i++] = "����ϥ��륬������˴ؤ�꤬���롣";
		break;

	case ETHNICITY_BACRUM:
		info[i++] = "����ϥХ����˴ؤ�꤬���롣";
		break;

	case ETHNICITY_ZENOBIAN:
		info[i++] = "����ϥ��Υӥ��˴ؤ�꤬���롣";
		break;

	case ETHNICITY_LODIS:
		info[i++] = "����ϥ��ǥ����˴ؤ�꤬���롣";
		break;
#else
	case ETHNICITY_WALSTANIAN:
		info[i++] = "It is related to Walstanian.";
		break;

	case ETHNICITY_GARGASTAN:
		info[i++] = "It is related to Gargastan.";
		break;

	case ETHNICITY_BACRUM:
		info[i++] = "It is related to Bacrum.";
		break;

	case ETHNICITY_ZENOBIAN:
		info[i++] = "It is related to Zenobian.";
		break;

	case ETHNICITY_LODIS:
		info[i++] = "It is related to Lodis.";
		break;
#endif
	}

	/* Describe about this kind of object instead of THIS fake object */
	if (!real)
	{
		switch (o_ptr->tval)
		{
		case TV_RING:
			switch (o_ptr->sval)
			{
			case SV_RING_LORDLY:
#ifdef JP
				info[i++] = "����ϴ��Ĥ��Υ����������������롣";
#else
				info[i++] = "It provides some random resistances.";
#endif
				break;
			}
			break;

		case TV_AMULET:
			switch (o_ptr->sval)
			{
			case SV_AMULET_RESISTANCE:
#ifdef JP
				info[i++] = "������Ǥؤ������������������롣";
#else
				info[i++] = "It may provide resistance to poison.";
#endif
#ifdef JP
				info[i++] = "����ϥ�����������������������롣";
#else
				info[i++] = "It may provide a random resistances.";
#endif
				break;
			}
			break;
		}
	}

	if (have_flag(flgs, TR_IGNORE_ACID) &&
	    have_flag(flgs, TR_IGNORE_ELEC) &&
	    have_flag(flgs, TR_IGNORE_FIRE) &&
	    have_flag(flgs, TR_IGNORE_COLD))
	{
#ifdef JP
	  info[i++] = "����ϻ����ŷ⡦�бꡦ�䵤�ǤϽ��Ĥ��ʤ���";
#else
	  info[i++] = "It cannot be harmed by the elements.";
#endif
	}
	else
	{
		if (have_flag(flgs, TR_IGNORE_ACID))
		{
#ifdef JP
			info[i++] = "����ϻ��ǤϽ��Ĥ��ʤ���";
#else
			info[i++] = "It cannot be harmed by acid.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_ELEC))
		{
#ifdef JP
			info[i++] = "������ŷ�ǤϽ��Ĥ��ʤ���";
#else
			info[i++] = "It cannot be harmed by electricity.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_FIRE))
		{
#ifdef JP
			info[i++] = "����ϲб�ǤϽ��Ĥ��ʤ���";
#else
			info[i++] = "It cannot be harmed by fire.";
#endif
		}
		if (have_flag(flgs, TR_IGNORE_COLD))
		{
#ifdef JP
			info[i++] = "������䵤�ǤϽ��Ĥ��ʤ���";
#else
			info[i++] = "It cannot be harmed by cold.";
#endif
		}
	}

	if ((o_ptr->tval >= TV_BOW) && (o_ptr->tval <= TV_SWORD))
	{
		char weapon_type[80];
#ifdef JP
		sprintf(weapon_type, "��勵����: %s", weapon_skill_name[get_weapon_type(k_ptr)]);
#else
		sprintf(weapon_type, "Weapon Type: %s", weapon_skill_name[get_weapon_type(k_ptr)]);
#endif
		info[i++] = weapon_type;
	}

	/* No special effects */
	if (!i) return (FALSE);

	if (fff != NULL)
	{
		for (j = 0; j < i; j++)
		{
			/* Show the info */
			fprintf(fff, "   %s\n", info[j]);
		}
	}
	else
	{
		/* Save the screen */
		screen_save();

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Display Item name */
		if (real)
			object_desc(o_name, o_ptr, TRUE, 3);
		else
			object_desc_store(o_name, o_ptr, TRUE, 0);

		prt(o_name, 0, 0);

		/* Erase the screen */
		for (k = 1; k < hgt; k++) prt("", k, 13);

		/* Label the information */
		if ((o_ptr->tval == TV_STATUE) && (o_ptr->sval == SV_PHOTO))
		{
			monster_race *r_ptr = &r_info[o_ptr->pval];
			int namelen = strlen(r_name + r_ptr->name);
			prt(format("%s: '", r_name + r_ptr->name), 1, 15);
			c_prt(r_ptr->d_attr, format("%c", r_ptr->d_char), 1, 18+namelen);
			prt("'", 1, 19+namelen);
		}
		else
#ifdef JP
			prt("     �����ƥ��ǽ��:", 1, 15);
#else
			prt("     Item Attributes:", 1, 15);
#endif

		/* We will print on top of the map (column 13) */
		for (k = 2, j = 0; j < i; j++)
		{
			/* Show the info */
			prt(info[j], k++, 15);

			/* Every 20 entries (lines 2 to 21), start over */
			if ((k == hgt - 2) && (j+1 < i))
			{
#ifdef JP
				prt("-- ³�� --", k, 15);
#else
				prt("-- more --", k, 15);
#endif
				inkey();
				for (; k > 2; k--) prt("", k, 15);
			}
		}

		/* Wait for it */
#ifdef JP
		prt("[���������򲡤��ȥ���������ޤ�]", k, 15);
#else
		prt("[Press any key to continue]", k, 15);
#endif

		inkey();

		/* Restore the screen */
		screen_load();
	}

	/* Gave knowledge */
	return (TRUE);
}



/*
 * Convert an inventory index into a one character label
 * Note that the label does NOT distinguish inven/equip.
 */
char index_to_label(int i)
{
	/* Indexes for "inven" are easy */
	if (i < INVEN_RARM) return (I2A(i));

	/* Indexes for "equip" are offset */
	return (I2A(i - INVEN_RARM));
}


/*
 * Convert a label into the index of an item in the "inven"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_inven(int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1);

	/* Verify the index */
	if ((i < 0) || (i > INVEN_PACK)) return (-1);

	/* Empty slots can never be chosen */
	if (!inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}


/*
 * Convert a label into the index of a item in the "equip"
 * Return "-1" if the label does not indicate a real item
 */
s16b label_to_equip(int c)
{
	int i;

	/* Convert */
	i = (islower(c) ? A2I(c) : -1) + INVEN_RARM;

	/* Verify the index */
	if ((i < INVEN_RARM) || (i >= INVEN_TOTAL)) return (-1);

	/* Empty slots can never be chosen */
	if (!inventory[i].k_idx) return (-1);

	/* Return the index */
	return (i);
}



/*
 * Determine which equipment slot (if any) an item likes
 */
s16b wield_slot(object_type *o_ptr)
{
	/* Slot for equipment */
	switch (o_ptr->tval)
	{
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		{
			if (!inventory[INVEN_RARM].k_idx) return (INVEN_RARM);
			if (inventory[INVEN_LARM].k_idx) return (INVEN_RARM);
			return (INVEN_LARM);
		}

		case TV_CARD:
		case TV_TRUMP:
		case TV_SHIELD:
		{
			if (!inventory[INVEN_LARM].k_idx) return (INVEN_LARM);
			if (inventory[INVEN_RARM].k_idx) return (INVEN_LARM);
			return (INVEN_RARM);
		}

		case TV_BOW:
		{
			return (INVEN_BOW);
		}

		case TV_RING:
		{
			/* Use the right hand first */
			if (!inventory[INVEN_RIGHT].k_idx) return (INVEN_RIGHT);

			/* Use the left hand for swapping (by default) */
			return (INVEN_LEFT);
		}

		case TV_AMULET:
		case TV_STONE:
		{
			return (INVEN_NECK);
		}

		case TV_LITE:
		{
			return (INVEN_LITE);
		}

		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		{
			return (INVEN_BODY);
		}

		case TV_CLOAK:
		{
			return (INVEN_OUTER);
		}

		case TV_CROWN:
		case TV_HELM:
		{
			return (INVEN_HEAD);
		}

		case TV_GLOVES:
		{
			return (INVEN_HANDS);
		}

		case TV_BOOTS:
		{
			return (INVEN_FEET);
		}
	}

	/* No slot available */
	return (-1);
}


/*
 * Return a string mentioning how a given item is carried
 */
cptr mention_use(int i)
{
	cptr p;

	/* Examine the location */
	switch (i)
	{
#ifdef JP
		case INVEN_RARM: p = p_ptr->ryoute ? " ξ��" : (left_hander ? " ����" : " ����"); break;
#else
		case INVEN_RARM: p = "Wielding"; break;
#endif

#ifdef JP
		case INVEN_LARM:   p = (left_hander ? " ����" : " ����"); break;
#else
		case INVEN_LARM:   p = "On arm"; break;
#endif

#ifdef JP
		case INVEN_BOW:   p = "�ͷ���"; break;
#else
		case INVEN_BOW:   p = "Shooting"; break;
#endif

#ifdef JP
		case INVEN_LEFT:  p = (left_hander ? "�����" : "�����"); break;
#else
		case INVEN_LEFT:  p = "On left hand"; break;
#endif

#ifdef JP
		case INVEN_RIGHT: p = (left_hander ? "�����" : "�����"); break;
#else
		case INVEN_RIGHT: p = "On right hand"; break;
#endif

#ifdef JP
		case INVEN_NECK:  p = "  ��"; break;
#else
		case INVEN_NECK:  p = "Around neck"; break;
#endif

#ifdef JP
		case INVEN_LITE:  p = " ����"; break;
#else
		case INVEN_LITE:  p = "Light source"; break;
#endif

#ifdef JP
		case INVEN_BODY:  p = "  ��"; break;
#else
		case INVEN_BODY:  p = "On body"; break;
#endif

#ifdef JP
		case INVEN_OUTER: p = "�Τξ�"; break;
#else
		case INVEN_OUTER: p = "About body"; break;
#endif

#ifdef JP
		case INVEN_HEAD:  p = "  Ƭ"; break;
#else
		case INVEN_HEAD:  p = "On head"; break;
#endif

#ifdef JP
		case INVEN_HANDS: p = "  ��"; break;
#else
		case INVEN_HANDS: p = "On hands"; break;
#endif

#ifdef JP
		case INVEN_FEET:  p = "  ­"; break;
#else
		case INVEN_FEET:  p = "On feet"; break;
#endif

#ifdef JP
		default:          p = "���å�"; break;
#else
		default:          p = "In pack"; break;
#endif

	}

	/* Hack -- Heavy weapon */
	if (i == INVEN_RARM)
	{
		if (p_ptr->heavy_wield[0])
		{
#ifdef JP
			p = "������";
#else
			p = "Just lifting";
#endif

		}
	}

	/* Hack -- Heavy weapon */
	if (i == INVEN_LARM)
	{
		if (p_ptr->heavy_wield[1])
		{
#ifdef JP
			p = "������";
#else
			p = "Just lifting";
#endif

		}
	}

	/* Hack -- Heavy bow */
	if (i == INVEN_BOW)
	{
		object_type *o_ptr;
		o_ptr = &inventory[i];
		if (adj_str_hold[p_ptr->stat_ind[A_STR]] < o_ptr->weight / 10)
		{
#ifdef JP
			p = "������";
#else
			p = "Just holding";
#endif

		}
	}

	/* Return the result */
	return (p);
}


/*
 * Return a string describing how a given item is being worn.
 * Currently, only used for items in the equipment, not inventory.
 */
cptr describe_use(int i)
{
	cptr p;

	switch (i)
	{
#ifdef JP
		case INVEN_RARM: p = p_ptr->ryoute ? "ξ����������Ƥ���" : (left_hander ? "������������Ƥ���" : "������������Ƥ���"); break;
#else
		case INVEN_RARM: p = "attacking monsters with"; break;
#endif

#ifdef JP
		case INVEN_LARM:   p = (left_hander ? "������������Ƥ���" : "������������Ƥ���"); break;
#else
		case INVEN_LARM:   p = "wearing on your arm"; break;
#endif

#ifdef JP
		case INVEN_BOW:   p = "�ͷ��Ѥ��������Ƥ���"; break;
#else
		case INVEN_BOW:   p = "shooting missiles with"; break;
#endif

#ifdef JP
		case INVEN_LEFT:  p = (left_hander ? "����λؤˤϤ�Ƥ���" : "����λؤˤϤ�Ƥ���"); break;
#else
		case INVEN_LEFT:  p = "wearing on your left hand"; break;
#endif

#ifdef JP
		case INVEN_RIGHT: p = (left_hander ? "����λؤˤϤ�Ƥ���" : "����λؤˤϤ�Ƥ���"); break;
#else
		case INVEN_RIGHT: p = "wearing on your right hand"; break;
#endif

#ifdef JP
		case INVEN_NECK:  p = "��ˤ����Ƥ���"; break;
#else
		case INVEN_NECK:  p = "wearing around your neck"; break;
#endif

#ifdef JP
		case INVEN_LITE:  p = "�����ˤ��Ƥ���"; break;
#else
		case INVEN_LITE:  p = "using to light the way"; break;
#endif

#ifdef JP
		case INVEN_BODY:  p = "�Τ���Ƥ���"; break;
#else
		case INVEN_BODY:  p = "wearing on your body"; break;
#endif

#ifdef JP
		case INVEN_OUTER: p = "�ȤˤޤȤäƤ���"; break;
#else
		case INVEN_OUTER: p = "wearing on your back"; break;
#endif

#ifdef JP
		case INVEN_HEAD:  p = "Ƭ�ˤ��֤äƤ���"; break;
#else
		case INVEN_HEAD:  p = "wearing on your head"; break;
#endif

#ifdef JP
		case INVEN_HANDS: p = "��ˤĤ��Ƥ���"; break;
#else
		case INVEN_HANDS: p = "wearing on your hands"; break;
#endif

#ifdef JP
		case INVEN_FEET:  p = "­�ˤϤ��Ƥ���"; break;
#else
		case INVEN_FEET:  p = "wearing on your feet"; break;
#endif

#ifdef JP
		default:          p = "���å������äƤ���"; break;
#else
		default:          p = "carrying in your pack"; break;
#endif

	}

	/* Hack -- Heavy weapon */
	if (i == INVEN_RARM)
	{
		object_type *o_ptr;
		int hold = adj_str_hold[p_ptr->stat_ind[A_STR]];

		if (p_ptr->ryoute) hold *= 2;
		o_ptr = &inventory[i];
		if (hold < o_ptr->weight / 10)
		{
#ifdef JP
			p = "�������";
#else
			p = "just lifting";
#endif

		}
	}

	/* Hack -- Heavy bow */
	if (i == INVEN_BOW)
	{
		object_type *o_ptr;
		o_ptr = &inventory[i];
		if (adj_str_hold[p_ptr->stat_ind[A_STR]] < o_ptr->weight / 10)
		{
#ifdef JP
			p = "���Ĥ����������դ�";
#else
			p = "just holding";
#endif

		}
	}

	/* Return the result */
	return p;
}


/* Hack: Check if a spellbook is one of the realms we can use. -- TY */

bool check_book_realm(const byte book_tval, const byte book_sval)
{
	if ((book_tval < TV_MAGERY_BOOK) || (book_tval > (TV_MAGERY_BOOK + MAX_REALM - 1))) return FALSE;
	if ((p_ptr->pclass == CLASS_DRAGOON) && (book_tval == TV_WITCH_BOOK))
		return (book_sval == 0);
	if (((p_ptr->pclass == CLASS_WITCH) || (p_ptr->pclass == CLASS_ARCHMAGE)) && (book_tval == TV_DEATH_BOOK))
		return (book_sval == 0);
	return can_use_realm(tval2realm(book_tval));
}


/*
 * Check an item against the item tester info
 */
bool item_tester_okay(object_type *o_ptr)
{
	/* Hack -- allow listing empty slots */
	if (item_tester_full) return (TRUE);

	/* Require an item */
	if (!o_ptr->k_idx) return (FALSE);

	/* Hack -- ignore "gold" */
	if (o_ptr->tval == TV_GOLD) return (FALSE);

	/* Check the tval */
	if (item_tester_tval)
	{
		/* Is it a spellbook? If so, we need a hack -- TY */
		if ((item_tester_tval <= TV_CRUSADE_BOOK) &&
			(item_tester_tval >= TV_MAGERY_BOOK))
			return check_book_realm(o_ptr->tval, o_ptr->sval);
		else
			if (item_tester_tval != o_ptr->tval) return (FALSE);
	}

	/* Check the hook */
	if (item_tester_hook)
	{
		if (!(*item_tester_hook)(o_ptr)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}




/*
 * Choice window "shadow" of the "show_inven()" function
 */
void display_inven(void)
{
	register        int i, n, z = 0;
	object_type     *o_ptr;
	byte            attr = TERM_WHITE;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];


	/* Find the "final" slot */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Track */
		z = i + 1;
	}

	/* Display the pack */
	for (i = 0; i < z; i++)
	{
		/* Examine the item */
		o_ptr = &inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Is this item "acceptable"? */
		if (item_tester_okay(o_ptr))
		{
			/* Prepare an "index" */
			tmp_val[0] = index_to_label(i);

			/* Bracket the "index" --(-- */
			tmp_val[1] = ')';
		}

		/* Display the index (or blank space) */
		Term_putstr(0, i, 3, TERM_WHITE, tmp_val);

		/* Obtain an item description */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Obtain the length of the description */
		n = strlen(o_name);

		/* Get a color */
		attr = tval_to_attr[o_ptr->tval % 128];

		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			attr = TERM_L_DARK;
		}

		/* Display the entry itself */
		Term_putstr(3, i, n, attr, o_name);

		/* Erase the rest of the line */
		Term_erase(3+n, i, 255);

		/* Display the weight if needed */
		if (show_weights && o_ptr->weight)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt),lbtokg2(wgt) );
#else
			sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			Term_putstr(71, i, -1, TERM_WHITE, tmp_val);
		}
	}

	/* Erase the rest of the window */
	for (i = z; i < Term->hgt; i++)
	{
		/* Erase the line */
		Term_erase(0, i, 255);
	}
}



/*
 * Choice window "shadow" of the "show_equip()" function
 */
void display_equip(void)
{
	register        int i, n;
	object_type     *o_ptr;
	byte            attr = TERM_WHITE;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];


	/* Display the equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Examine the item */
		o_ptr = &inventory[i];

		/* Start with an empty "index" */
		tmp_val[0] = tmp_val[1] = tmp_val[2] = ' ';

		/* Is this item "acceptable"? */
		if (item_tester_okay(o_ptr))
		{
			/* Prepare an "index" */
			tmp_val[0] = index_to_label(i);

			/* Bracket the "index" --(-- */
			tmp_val[1] = ')';
		}

		/* Display the index (or blank space) */
		Term_putstr(0, i - INVEN_RARM, 3, TERM_WHITE, tmp_val);

		/* Obtain an item description */
		if ((i == INVEN_LARM) && p_ptr->ryoute)
		{
#ifdef JP
			strcpy(o_name, "(����ξ�����)");
#else
			strcpy(o_name, "(wielding with two-hands)");
#endif
			attr = 1;
		}
		else
		{
			object_desc(o_name, o_ptr, TRUE, 3);
			attr = tval_to_attr[o_ptr->tval % 128];
		}

		/* Obtain the length of the description */
		n = strlen(o_name);

		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			attr = TERM_L_DARK;
		}

		/* Display the entry itself */
		Term_putstr(3, i - INVEN_RARM, n, attr, o_name);

		/* Erase the rest of the line */
		Term_erase(3+n, i - INVEN_RARM, 255);

		/* Display the slot description (if needed) */
		if (show_labels)
		{
			Term_putstr(61, i - INVEN_RARM, -1, TERM_WHITE, "<--");
			Term_putstr(65, i - INVEN_RARM, -1, TERM_WHITE, mention_use(i));
		}

		/* Display the weight (if needed) */
		if (show_weights && o_ptr->weight)
		{
			int wgt = o_ptr->weight * o_ptr->number;
			int col = (show_labels ? 52 : 71);
#ifdef JP
			sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt));
#else
			sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			Term_putstr(col, i - INVEN_RARM, -1, TERM_WHITE, tmp_val);
		}
	}

	/* Erase the rest of the window */
	for (i = INVEN_TOTAL - INVEN_RARM; i < Term->hgt; i++)
	{
		/* Clear that line */
		Term_erase(0, i, 255);
	}
}



/*
 * Find the "first" inventory object with the given "tag".
 *
 * A "tag" is a numeral "n" appearing as "@n" anywhere in the
 * inscription of an object.  Alphabetical characters don't work as a
 * tag in this form.
 *
 * Also, the tag "@xn" will work as well, where "n" is a any tag-char,
 * and "x" is the "current" command_cmd code.
 */
static int get_tag(int *cp, char tag)
{
	int i;
	cptr s;


	/**** Find a tag in the form of {@x#} (allow alphabet tag) ***/

	/* Check every object */
	for (i = 0; i < INVEN_TOTAL; ++i)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Skip empty inscriptions */
		if (!o_ptr->inscription) continue;

		/* Skip non-choice */
		if (!item_tester_okay(o_ptr)) continue;

		/* Find a '@' */
		s = strchr(quark_str(o_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the special tags */
			if ((s[1] == command_cmd) && (s[2] == tag))
			{
				/* Save the actual inventory ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = strchr(s + 1, '@');
		}
	}


	/**** Find a tag in the form of {@#} (allows only numerals)  ***/

	/* Don't allow {@#} with '#' being alphabet */
	if (tag < '0' || '9' < tag)
	{
		/* No such tag */
		return FALSE;
	}

	/* Check every object */
	for (i = 0; i < INVEN_TOTAL; ++i)
	{
		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Skip empty inscriptions */
		if (!o_ptr->inscription) continue;

		/* Skip non-choice */
		if (!item_tester_okay(o_ptr)) continue;

		/* Find a '@' */
		s = strchr(quark_str(o_ptr->inscription), '@');

		/* Process all tags */
		while (s)
		{
			/* Check the normal tags */
			if (s[1] == tag)
			{
				/* Save the actual inventory ID */
				*cp = i;

				/* Success */
				return (TRUE);
			}

			/* Find another '@' */
			s = strchr(s + 1, '@');
		}
	}

	/* No such tag */
	return (FALSE);
}



/*
 * Display the inventory.
 *
 * Hack -- do not display "trailing" empty slots
 */
int show_inven(int target_item)
{
	cptr alphabet_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int             i, j, k, l, z = 0;
	int             col, cur_col, len;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	char            tmp_val[80];
	int             out_index[23];
	byte            out_color[23];
	char            out_desc[23][MAX_NLEN];
	int             target_item_label = 0;
	int             wid, hgt;
	char inven_spellbook_label[52+1];

	/* Starting column */
	col = command_gap;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Default "max-length" */
	len = wid - col - 1;


	/* Find the "final" slot */
	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Track */
		z = i + 1;
	}

	/*** Move around label characters with correspond tags ***/

	/* Prepare normal labels */
	strcpy(inven_spellbook_label, alphabet_chars);

	/* Move each label */
	for (i = 0; i < 52; i++)
	{
		int index;
		char c = alphabet_chars[i];

		/* Find a tag with this label */
		if (get_tag(&index, c))
		{
			/* Delete the over writen label */
			if (inven_spellbook_label[i] == c)
				inven_spellbook_label[i] = ' ';

			/* Move the label to the place of correspond tag */
			inven_spellbook_label[index] = c;
		}
	}

	/* Display the inventory */
	for (k = 0, i = 0; i < z; i++)
	{
		o_ptr = &inventory[i];

		/* Is this item acceptable? */
		if (!item_tester_okay(o_ptr)) continue;

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Save the object index, color, and description */
		out_index[k] = i;
		out_color[k] = tval_to_attr[o_ptr->tval % 128];

		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			out_color[k] = TERM_L_DARK;
		}

		(void)strcpy(out_desc[k], o_name);

		/* Find the predicted "line length" */
		l = strlen(out_desc[k]) + 5;

		/* Be sure to account for the weight */
		if (show_weights) l += 9;

		/* Account for icon if displayed */
		if (show_item_graph)
		{
			l += 2;
			if (use_bigtile) l++;
		}

		/* Maintain the maximum length */
		if (l > len) len = l;

		/* Advance to next "line" */
		k++;
	}

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = out_index[j];

		/* Get the item */
		o_ptr = &inventory[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "��");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "  ");
		}
		else if (i <= INVEN_PACK)
		{
			sprintf(tmp_val, "%c)", inven_spellbook_label[i]);
		}
		else
		{
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", index_to_label(i));
		}

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if (show_item_graph)
		{
			byte  a = object_attr(o_ptr);
			char c = object_char(o_ptr);

#ifdef AMIGA
			if (a & 0x80) a |= 0x40;
#endif

			Term_draw(cur_col, j + 1, a, c);
			if (use_bigtile)
			{
				cur_col++;
				if (a & 0x80)
					Term_draw(cur_col, j + 1, 255, -1);
			}
			cur_col += 2;
		}


		/* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, cur_col);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			(void)sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			(void)sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			put_str(tmp_val, j + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	/* Save the new column */
	command_gap = col;

	return target_item_label;
}



/*
 * Display the equipment.
 */
int show_equip(int target_item)
{
	int             i, j, k, l;
	int             col, cur_col, len;
	object_type     *o_ptr;
	char            tmp_val[80];
	char            o_name[MAX_NLEN];
	int             out_index[23];
	byte            out_color[23];
	char            out_desc[23][MAX_NLEN];
	int             target_item_label = 0;
	int             wid, hgt;


	/* Starting column */
	col = command_gap;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Maximal length */
	len = wid - col - 1;


	/* Scan the equipment list */
	for (k = 0, i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Is this item acceptable? */
		if (!item_tester_okay(o_ptr) && (!((i == INVEN_LARM) && p_ptr->ryoute) || item_tester_no_ryoute)) continue;

		/* Description */
		object_desc(o_name, o_ptr, TRUE, 3);

		if ((i == INVEN_LARM) && p_ptr->ryoute)
		{
#ifdef JP
			(void)strcpy(out_desc[k],"(����ξ�����)");
#else
			(void)strcpy(out_desc[k],"(wielding with two-hands)");
#endif
			out_color[k] = 1;
		}
		else
		{
			(void)strcpy(out_desc[k], o_name);
			out_color[k] = tval_to_attr[o_ptr->tval % 128];
		}

		out_index[k] = i;
		/* Grey out charging items */
		if (o_ptr->timeout)
		{
			out_color[k] = TERM_L_DARK;
		}

		/* Extract the maximal length (see below) */
#ifdef JP
		l = strlen(out_desc[k]) + (2 + 1);
#else
		l = strlen(out_desc[k]) + (2 + 3);
#endif


		/* Increase length for labels (if needed) */
#ifdef JP
		if (show_labels) l += (7 + 2);
#else
		if (show_labels) l += (14 + 2);
#endif


		/* Increase length for weight (if needed) */
		if (show_weights) l += 9;

		if (show_item_graph) l += 2;

		/* Maintain the max-length */
		if (l > len) len = l;

		/* Advance the entry */
		k++;
	}

	/* Hack -- Find a column to start in */
#ifdef JP
	col = (len > wid - 6) ? 0 : (wid - len - 1);
#else
	col = (len > wid - 4) ? 0 : (wid - len - 1);
#endif


	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = out_index[j];

		/* Get the item */
		o_ptr = &inventory[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "��");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "  ");
		}
		else
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", index_to_label(i));

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j+1, col);

		cur_col = col + 3;

		/* Display graphics for object, if desired */
		if (show_item_graph)
		{
			byte a = object_attr(o_ptr);
			char c = object_char(o_ptr);

#ifdef AMIGA
			if (a & 0x80) a |= 0x40;
#endif

			Term_draw(cur_col, j + 1, a, c);
			if (use_bigtile)
			{
				cur_col++;
				if (a & 0x80)
					Term_draw(cur_col, j + 1, 255, -1);
			}
			cur_col += 2;
		}

		/* Use labels */
		if (show_labels)
		{
			/* Mention the use */
#ifdef JP
			(void)sprintf(tmp_val, "%-7s: ", mention_use(i));
#else
			(void)sprintf(tmp_val, "%-14s: ", mention_use(i));
#endif

			put_str(tmp_val, j+1, cur_col);

			/* Display the entry itself */
#ifdef JP
			c_put_str(out_color[j], out_desc[j], j+1, cur_col + 9);
#else
			c_put_str(out_color[j], out_desc[j], j+1, cur_col + 16);
#endif
		}

		/* No labels */
		else
		{
			/* Display the entry itself */
			c_put_str(out_color[j], out_desc[j], j+1, cur_col);
		}

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			(void)sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			(void)sprintf(tmp_val, "%3d.%d lb", wgt / 10, wgt % 10);
#endif

			put_str(tmp_val, j+1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	/* Save the new column */
	command_gap = col;

	return target_item_label;
}




/*
 * Flip "inven" and "equip" in any sub-windows
 */
void toggle_inven_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		/* Unused */
		if (!angband_term[j]) continue;

		/* Flip inven to equip */
		if (window_flag[j] & (PW_INVEN))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_INVEN);
			window_flag[j] |= (PW_EQUIP);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);
		}

		/* Flip inven to equip */
		else if (window_flag[j] & (PW_EQUIP))
		{
			/* Flip flags */
			window_flag[j] &= ~(PW_EQUIP);
			window_flag[j] |= (PW_INVEN);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN);
		}
	}
}



/*
 * Verify the choice of an item.
 *
 * The item can be negative to mean "item on floor".
 */
static bool verify(cptr prompt, int item)
{
	char        o_name[MAX_NLEN];
	char        out_val[MAX_NLEN+20];
	object_type *o_ptr;


	/* Inventory */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Floor */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Describe */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Prompt */
#ifdef JP
	(void)sprintf(out_val, "%s%s�Ǥ���? ", prompt, o_name);
#else
	(void)sprintf(out_val, "%s %s? ", prompt, o_name);
#endif


	/* Query */
	return (get_check(out_val));
}


#ifdef JP
#undef strchr
#define strchr strchr_j
#endif


/*
 * Hack -- allow user to "prevent" certain choices
 *
 * The item can be negative to mean "item on floor".
 */
static bool get_item_allow(int item)
{
	cptr s;

	object_type *o_ptr;

	/* Inventory */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Floor */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* No inscription */
	if (!o_ptr->inscription) return (TRUE);

	/* Find a '!' */
	s = strchr(quark_str(o_ptr->inscription), '!');

	/* Process preventions */
	while (s)
	{
		/* Check the "restriction" */
		if ((s[1] == command_cmd) || (s[1] == '*'))
		{
			/* Verify the choice */
#ifdef JP
			if (!verify("������", item)) return (FALSE);
#else
			if (!verify("Really try", item)) return (FALSE);
#endif

		}

		/* Find another '!' */
		s = strchr(s + 1, '!');
	}

	/* Allow it */
	return (TRUE);
}



/*
 * Auxiliary function for "get_item()" -- test an index
 */
static bool get_item_okay(int i)
{
	/* Illegal items */
	if ((i < 0) || (i >= INVEN_TOTAL)) return (FALSE);

	/* Verify the item */
	if (!item_tester_okay(&inventory[i])) return (FALSE);

	/* Assume okay */
	return (TRUE);
}



/*
 * Determine whether get_item() can get some item or not
 * assuming mode = (USE_EQUIP | USE_INVEN | USE_FLOOR).
 */
bool can_get_item(void)
{
	int j, floor_list[23], floor_num = 0;

	for (j = 0; j < INVEN_TOTAL; j++)
		if (item_tester_okay(&inventory[j]))
			return TRUE;

	floor_num = scan_floor(floor_list, py, px, 0x01);
	if (floor_num)
		return TRUE;

	return FALSE;
}

/*
 * Let the user select an item, save its "index"
 *
 * Return TRUE only if an acceptable item was chosen by the user.
 *
 * The selected item must satisfy the "item_tester_hook()" function,
 * if that hook is set, and the "item_tester_tval", if that value is set.
 *
 * All "item_tester" restrictions are cleared before this function returns.
 *
 * The user is allowed to choose acceptable items from the equipment,
 * inventory, or floor, respectively, if the proper flag was given,
 * and there are any acceptable items in that location.
 *
 * The equipment or inventory are displayed (even if no acceptable
 * items are in that location) if the proper flag was given.
 *
 * If there are no acceptable items available anywhere, and "str" is
 * not NULL, then it will be used as the text of a warning message
 * before the function returns.
 *
 * Note that the user must press "-" to specify the item on the floor,
 * and there is no way to "examine" the item on the floor, while the
 * use of "capital" letters will "examine" an inventory/equipment item,
 * and prompt for its use.
 *
 * If a legal item is selected from the inventory, we save it in "cp"
 * directly (0 to 35), and return TRUE.
 *
 * If a legal item is selected from the floor, we save it in "cp" as
 * a negative (-1 to -511), and return TRUE.
 *
 * If no item is available, we do nothing to "cp", and we display a
 * warning message, using "str" if available, and return FALSE.
 *
 * If no item is selected, we do nothing to "cp", and return FALSE.
 *
 * Global "p_ptr->command_new" is used when viewing the inventory or equipment
 * to allow the user to enter a command while viewing those screens, and
 * also to induce "auto-enter" of stores, and other such stuff.
 *
 * Global "p_ptr->command_see" may be set before calling this function to start
 * out in "browse" mode.  It is cleared before this function returns.
 *
 * Global "p_ptr->command_wrk" is used to choose between equip/inven listings.
 * If it is TRUE then we are viewing inventory, else equipment.
 *
 * We always erase the prompt when we are done, leaving a blank line,
 * or a warning message, if appropriate, if no items are available.
 */
bool get_item(int *cp, cptr pmt, cptr str, int mode)
{
	s16b this_o_idx, next_o_idx = 0;

	char which = ' ';

	int j, k, i1, i2, e1, e2;

	bool done, item;

	bool oops = FALSE;

	bool equip = FALSE;
	bool inven = FALSE;
	bool floor = FALSE;

	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;
	int max_equip = 0;

	if (easy_floor || use_menu) return get_item_floor(cp, pmt, str, mode);

	/* Extract args */
	if (mode & (USE_EQUIP)) equip = TRUE;
	if (mode & (USE_INVEN)) inven = TRUE;
	if (mode & (USE_FLOOR)) floor = TRUE;

	/* Get the item index */
	if (repeat_pull(cp))
	{
		/* Floor item? */
		if (floor && (*cp < 0))
		{
			object_type *o_ptr;

			/* Special index */
			k = 0 - (*cp);

			/* Acquire object */
			o_ptr = &o_list[k];

			/* Validate the item */
			if (item_tester_okay(o_ptr))
			{
				/* Forget the item_tester_tval restriction */
				item_tester_tval = 0;

				/* Forget the item_tester_hook restriction */
				item_tester_hook = NULL;

				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return (TRUE);
			}
		}

		else if ((inven && (*cp >= 0) && (*cp < INVEN_PACK)) ||
		         (equip && (*cp >= INVEN_RARM) && (*cp < INVEN_TOTAL)))
		{
		/* Verify the item */
			if (get_item_okay(*cp))
			{
				/* Forget the item_tester_tval restriction */
				item_tester_tval = 0;

				/* Forget the item_tester_hook restriction */
				item_tester_hook = NULL;

				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return (TRUE);
			}
		}
	}

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Not done */
	done = FALSE;

	/* No item selected */
	item = FALSE;


	/* Full inventory */
	i1 = 0;
	i2 = INVEN_PACK - 1;

	/* Forbid inventory */
	if (!inven) i2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_PACK; j++)
			if (item_tester_okay(&inventory[j])) max_inven++;
	}

	/* Restrict inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(i1))) i1++;
	while ((i1 <= i2) && (!get_item_okay(i2))) i2--;


	/* Full equipment */
	e1 = INVEN_RARM;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if (!equip) e2 = -1;
	else if (use_menu)
	{
		for (j = INVEN_RARM; j < INVEN_TOTAL; j++)
			if (item_tester_okay(&inventory[j])) max_equip++;
		if (p_ptr->ryoute && !item_tester_no_ryoute) max_equip++;
	}

	/* Restrict equipment indexes */
	while ((e1 <= e2) && (!get_item_okay(e1))) e1++;
	while ((e1 <= e2) && (!get_item_okay(e2))) e2--;



	/* Restrict floor usage */
	if (floor)
	{
		/* Scan all objects in the grid */
		for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Accept the item on the floor if legal */
			if (item_tester_okay(o_ptr)) allow_floor = TRUE;
		}
	}

	/* Require at least one legal choice */
	if (!allow_floor && (i1 > i2) && (e1 > e2))
	{
		/* Cancel p_ptr->command_see */
		command_see = FALSE;

		/* Oops */
		oops = TRUE;

		/* Done */
		done = TRUE;
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if (command_see && command_wrk && equip)

		{
			command_wrk = TRUE;
		}

		/* Use inventory if allowed */
		else if (inven)
		{
			command_wrk = FALSE;
		}

		/* Use equipment if allowed */
		else if (equip)
		{
			command_wrk = TRUE;
		}

		/* Use inventory for floor */
		else
		{
			command_wrk = FALSE;
		}
	}


	/*
	 * �ɲå��ץ����(always_show_list)�����ꤵ��Ƥ�����Ͼ�˰�����ɽ������
	 */
	if ((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if (command_see)
	{
		/* Save screen */
		screen_save();
	}


	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if (!angband_term[j]) continue;

			/* Count windows displaying inven */
			if (window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if (window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if ((command_wrk && ni && !ne) ||
		    (!command_wrk && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		/* Update */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Redraw windows */
		window_stuff();

		/* Inventory screen */
		if (!command_wrk)
		{
			/* Redraw if needed */
			if (command_see) get_item_label = show_inven(menu_line);
		}

		/* Equipment screen */
		else
		{
			/* Redraw if needed */
			if (command_see) get_item_label = show_equip(menu_line);
		}

		/* Viewing inventory */
		if (!command_wrk)
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "����ʪ:");
#else
			sprintf(out_val, "Inven:");
#endif


			/* Some legal items */
			if ((i1 <= i2) && !use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif

				index_to_label(i1), index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'����,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif


			/* Append */
#ifdef JP
			if (equip) strcat(out_val, format(" %s ������,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if (equip) strcat(out_val, format(" %s for Equip,", use_menu ? "4 or 6" : "/"));
#endif

		}

		/* Viewing equipment */
		else
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "������:");
#else
			sprintf(out_val, "Equip:");
#endif


			/* Some legal items */
			if ((e1 <= e2) && !use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif

				index_to_label(e1), index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'����,");
#else
			if (!command_see) strcat(out_val, " * to see,");
#endif


			/* Append */
#ifdef JP
			if (inven) strcat(out_val, format(" %s ����ʪ,", use_menu ? "'4'or'6'" : "'/'"));
#else
			if (inven) strcat(out_val, format(" %s for Inven,", use_menu ? "4 or 6" : "'/'"));
#endif

		}

		/* Indicate legality of the "floor" item */
#ifdef JP
		if (allow_floor) strcat(out_val, " '-'����,");
#else
		if (allow_floor) strcat(out_val, " - for floor,");
#endif


		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);


		/* Get a key */
		which = inkey();

		if (use_menu)
		{
		int max_line = (command_wrk ? max_equip : max_inven);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
				break;
			}

			case '2':
			case 'j':
			case 'J':
			{
				menu_line++;
				break;
			}

			case '4':
			case '6':
			case 'h':
			case 'H':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;
				max_line = (command_wrk ? max_equip : max_inven);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if (command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if (!get_item_okay(get_item_label))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if (!get_item_allow(get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
		}
		if (menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Hide the list */
				if (command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					/* Load screen */
					screen_load();
				}

				/* Show the list */
				else
				{
					/* Save screen */
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '/':
			{
				/* Verify legality */
				if (!inven || !equip)
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				command_wrk = !command_wrk;

				/* Need to redraw */
				break;
			}

			case '-':
			{
				/* Use floor item */
				if (allow_floor)
				{
					/* Scan all objects in the grid */
					for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
					{
						object_type *o_ptr;

						/* Acquire object */
						o_ptr = &o_list[this_o_idx];

						/* Acquire next object */
						next_o_idx = o_ptr->next_o_idx;

						/* Validate the item */
						if (!item_tester_okay(o_ptr)) continue;

						/* Special index */
						k = 0 - this_o_idx;

						/* Verify the item (if required) */
#ifdef JP
						if (other_query_flag && !verify("������", k)) continue;
#else
						if (other_query_flag && !verify("Try", k)) continue;
#endif


						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(k)) continue;

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
						break;
					}

					/* Outer break */
					if (done) break;
				}

				/* Oops */
				bell();
				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				/* Look up the tag */
				if (!get_tag(&k, which))
				{
					bell();
					break;
				}

				/* Hack -- Validate the item */
				if ((k < INVEN_RARM) ? !inven : !equip)
				{
					bell();
					break;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}

#if 0
			case '\n':
			case '\r':
			{
				/* Choose "default" inventory item */
				if (!command_wrk)
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
#endif

			default:
			{
				int ver;
				bool not_found = FALSE;

				/* Look up the alphabetical tag */
				if (!get_tag(&k, which))
				{
					not_found = TRUE;
				}

				/* Hack -- Validate the item */
				if ((k < INVEN_RARM) ? !inven : !equip)
				{
					not_found = TRUE;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					not_found = TRUE;
				}

				if (!not_found)
				{
					/* Accept that choice */
					(*cp) = k;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = tolower(which);

				/* Convert letter to inventory index */
				if (!command_wrk)
				{
					if (which == '(') k = i1;
					else if (which == ')') k = i2;
					else k = label_to_inven(which);
				}

				/* Convert letter to equipment index */
				else
				{
					if (which == '(') k = e1;
					else if (which == ')') k = e2;
					else k = label_to_equip(which);
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
				if (ver && !verify("������", k))
#else
				if (ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}


	/* Fix the screen if necessary */
	if (command_see)
	{
		/* Load screen */
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	item_tester_no_ryoute = FALSE;

	/* Forget the item_tester_hook restriction */
	item_tester_hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if (toggle) toggle_inven_equip();

	/* Update */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff();

	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if (oops && str) msg_print(str);

	if (item)
	{
		repeat_push(*cp);
		command_cmd = 0; /* Hack -- command_cmd is no longer effective */
	}

	/* Result */
	return (item);
}



/*
 * scan_floor --
 *
 * Return a list of o_list[] indexes of items at the given cave
 * location. Valid flags are:
 *
 *		mode & 0x01 -- Item tester
 *		mode & 0x02 -- Marked items only
 *		mode & 0x04 -- Stop after first
 */
int scan_floor(int *items, int y, int x, int mode)
{
	int this_o_idx, next_o_idx;

	int num = 0;

	/* Sanity */
	if (!in_bounds(y, x)) return 0;

	/* Scan all objects in the grid */
	for (this_o_idx = cave[y][x].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Item tester */
		if ((mode & 0x01) && !item_tester_okay(o_ptr)) continue;

		/* Marked */
		if ((mode & 0x02) && !o_ptr->marked) continue;

		/* Accept this item */
		/* XXX Hack -- Enforce limit */
		if (num < 23)
			items[num] = this_o_idx;

		num++;

		/* Only one */
		if (mode & 0x04) break;
	}

	/* Result */
	return num;
}

/*
 * Display a list of the items on the floor at the given location.
 */
int show_floor(int target_item, int y, int x, int *min_width)
{
	int i, j, k, l;
	int col, len;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char tmp_val[80];

	int out_index[23];
	byte out_color[23];
	char out_desc[23][MAX_NLEN];
	int target_item_label = 0;

	int floor_list[23], floor_num;
	int wid, hgt;

	/* Get size */
	Term_get_size(&wid, &hgt);

	/* Default length */
	len = MAX((*min_width), 20);


	/* Scan for objects in the grid, using item_tester_okay() */
	floor_num = scan_floor(floor_list, y, x, 0x01);

	/* Display the inventory */
	for (k = 0, i = 0; i < floor_num && i < 23; i++)
	{
		o_ptr = &o_list[floor_list[i]];

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Save the index */
		out_index[k] = i;

		/* Acquire inventory color */
		out_color[k] = tval_to_attr[o_ptr->tval & 0x7F];

		/* Save the object description */
		strcpy(out_desc[k], o_name);

		/* Find the predicted "line length" */
		l = strlen(out_desc[k]) + 5;

		/* Be sure to account for the weight */
		if (show_weights) l += 9;

		/* Maintain the maximum length */
		if (l > len) len = l;

		/* Advance to next "line" */
		k++;
	}

	/* Save width */
	*min_width = len;

	/* Find the column to start in */
	col = (len > wid - 4) ? 0 : (wid - len - 1);

	/* Output each entry */
	for (j = 0; j < k; j++)
	{
		/* Get the index */
		i = floor_list[out_index[j]];

		/* Get the item */
		o_ptr = &o_list[i];

		/* Clear the line */
		prt("", j + 1, col ? col - 2 : col);

		if (use_menu && target_item)
		{
			if (j == (target_item-1))
			{
#ifdef JP
				strcpy(tmp_val, "��");
#else
				strcpy(tmp_val, "> ");
#endif
				target_item_label = i;
			}
			else strcpy(tmp_val, "   ");
		}
		else
			/* Prepare an index --(-- */
			sprintf(tmp_val, "%c)", index_to_label(j));

		/* Clear the line with the (possibly indented) index */
		put_str(tmp_val, j + 1, col);

		/* Display the entry itself */
		c_put_str(out_color[j], out_desc[j], j + 1, col + 3);

		/* Display the weight if needed */
		if (show_weights)
		{
			int wgt = o_ptr->weight * o_ptr->number;
#ifdef JP
			sprintf(tmp_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
#else
			sprintf(tmp_val, "%3d.%1d lb", wgt / 10, wgt % 10);
#endif

			put_str(tmp_val, j + 1, wid - 9);
		}
	}

	/* Make a "shadow" below the list (only if needed) */
	if (j && (j < 23)) prt("", j + 1, col ? col - 2 : col);

	return target_item_label;
}

/*
 * This version of get_item() is called by get_item() when
 * the easy_floor is on.
 */
bool get_item_floor(int *cp, cptr pmt, cptr str, int mode)
{
	char n1 = ' ', n2 = ' ', which = ' ';

	int j, k, i1, i2, e1, e2;

	bool done, item;

	bool oops = FALSE;

	/* Extract args */
	bool equip = (mode & USE_EQUIP) ? TRUE : FALSE;
	bool inven = (mode & USE_INVEN) ? TRUE : FALSE;
	bool floor = (mode & USE_FLOOR) ? TRUE : FALSE;

	bool allow_equip = FALSE;
	bool allow_inven = FALSE;
	bool allow_floor = FALSE;

	bool toggle = FALSE;

	char tmp_val[160];
	char out_val[160];

	int floor_num, floor_list[23], floor_top = 0;
	int min_width = 0;

	int menu_line = (use_menu ? 1 : 0);
	int max_inven = 0;
	int max_equip = 0;

	/* Get the item index */
	if (repeat_pull(cp))
	{
		/* Floor item? */
		if (floor && (*cp < 0))
		{
			object_type *o_ptr;

			/* Special index */
			k = 0 - (*cp);

			/* Acquire object */
			o_ptr = &o_list[k];

			/* Validate the item */
			if (item_tester_okay(o_ptr))
			{
				/* Forget the item_tester_tval restriction */
				item_tester_tval = 0;

				/* Forget the item_tester_hook restriction */
				item_tester_hook = NULL;

				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return (TRUE);
			}
		}

		else if ((inven && (*cp >= 0) && (*cp < INVEN_PACK)) ||
		         (equip && (*cp >= INVEN_RARM) && (*cp < INVEN_TOTAL)))
		{
			/* Verify the item */
			if (get_item_okay(*cp))
			{
				/* Forget the item_tester_tval restriction */
				item_tester_tval = 0;

				/* Forget the item_tester_hook restriction */
				item_tester_hook = NULL;

				command_cmd = 0; /* Hack -- command_cmd is no longer effective */

				/* Success */
				return (TRUE);
			}
		}
	}

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Not done */
	done = FALSE;

	/* No item selected */
	item = FALSE;


	/* Full inventory */
	i1 = 0;
	i2 = INVEN_PACK - 1;

	/* Forbid inventory */
	if (!inven) i2 = -1;
	else if (use_menu)
	{
		for (j = 0; j < INVEN_PACK; j++)
			if (item_tester_okay(&inventory[j])) max_inven++;
	}

	/* Restrict inventory indexes */
	while ((i1 <= i2) && (!get_item_okay(i1))) i1++;
	while ((i1 <= i2) && (!get_item_okay(i2))) i2--;


	/* Full equipment */
	e1 = INVEN_RARM;
	e2 = INVEN_TOTAL - 1;

	/* Forbid equipment */
	if (!equip) e2 = -1;
	else if (use_menu)
	{
		for (j = INVEN_RARM; j < INVEN_TOTAL; j++)
			if (item_tester_okay(&inventory[j])) max_equip++;
		if (p_ptr->ryoute && !item_tester_no_ryoute) max_equip++;
	}

	/* Restrict equipment indexes */
	while ((e1 <= e2) && (!get_item_okay(e1))) e1++;
	while ((e1 <= e2) && (!get_item_okay(e2))) e2--;


	/* Count "okay" floor items */
	floor_num = 0;

	/* Restrict floor usage */
	if (floor)
	{
		/* Scan all objects in the grid */
		floor_num = scan_floor(floor_list, py, px, 0x01);
	}

	/* Accept inventory */
	if (i1 <= i2) allow_inven = TRUE;

	/* Accept equipment */
	if (e1 <= e2) allow_equip = TRUE;

	/* Accept floor */
	if (floor_num) allow_floor = TRUE;

	/* Require at least one legal choice */
	if (!allow_inven && !allow_equip && !allow_floor)
	{
		/* Cancel p_ptr->command_see */
		command_see = FALSE;

		/* Oops */
		oops = TRUE;

		/* Done */
		done = TRUE;
	}

	/* Analyze choices */
	else
	{
		/* Hack -- Start on equipment if requested */
		if (command_see && (command_wrk == (USE_EQUIP))
			&& allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use inventory if allowed */
		else if (allow_inven)
		{
			command_wrk = (USE_INVEN);
		}

		/* Use equipment if allowed */
		else if (allow_equip)
		{
			command_wrk = (USE_EQUIP);
		}

		/* Use floor if allowed */
		else if (allow_floor)
		{
			command_wrk = (USE_FLOOR);
		}
	}

	/*
	 * �ɲå��ץ����(always_show_list)�����ꤵ��Ƥ�����Ͼ�˰�����ɽ������
	 */
	if ((always_show_list == TRUE) || use_menu) command_see = TRUE;

	/* Hack -- start out in "display" mode */
	if (command_see)
	{
		/* Save screen */
		screen_save();
	}

	/* Repeat until done */
	while (!done)
	{
		int get_item_label = 0;

		/* Show choices */
		int ni = 0;
		int ne = 0;

		/* Scan windows */
		for (j = 0; j < 8; j++)
		{
			/* Unused */
			if (!angband_term[j]) continue;

			/* Count windows displaying inven */
			if (window_flag[j] & (PW_INVEN)) ni++;

			/* Count windows displaying equip */
			if (window_flag[j] & (PW_EQUIP)) ne++;
		}

		/* Toggle if needed */
		if ((command_wrk == (USE_EQUIP) && ni && !ne) ||
			(command_wrk == (USE_INVEN) && !ni && ne))
		{
			/* Toggle */
			toggle_inven_equip();

			/* Track toggles */
			toggle = !toggle;
		}

		/* Update */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Redraw windows */
		window_stuff();

		/* Inventory screen */
		if (command_wrk == (USE_INVEN))
		{
			/* Extract the legal requests */
			n1 = I2A(i1);
			n2 = I2A(i2);

			/* Redraw if needed */
			if (command_see) get_item_label = show_inven(menu_line);
		}

		/* Equipment screen */
		else if (command_wrk == (USE_EQUIP))
		{
			/* Extract the legal requests */
			n1 = I2A(e1 - INVEN_RARM);
			n2 = I2A(e2 - INVEN_RARM);

			/* Redraw if needed */
			if (command_see) get_item_label = show_equip(menu_line);
		}

		/* Floor screen */
		else if (command_wrk == (USE_FLOOR))
		{
			j = floor_top;
			k = MIN(floor_top + 23, floor_num) - 1;

			/* Extract the legal requests */
			n1 = I2A(j - floor_top);
			n2 = I2A(k - floor_top);

			/* Redraw if needed */
			if (command_see) get_item_label = show_floor(menu_line, py, px, &min_width);
		}

		/* Viewing inventory */
		if (command_wrk == (USE_INVEN))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "����ʪ:");
#else
			sprintf(out_val, "Inven:");
#endif

			if (!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif

				index_to_label(i1), index_to_label(i2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'����,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif


			/* Append */
#ifdef JP
			if (allow_equip)
			{
				if (!use_menu)
					strcat(out_val, " '/' ������,");
				else if (allow_floor)
					strcat(out_val, " '6' ������,");
				else
					strcat(out_val, " '4'or'6' ������,");
			}
#else
			if (allow_equip)
			{
				if (!use_menu)
					strcat(out_val, " / for Equip,");
				else if (allow_floor)
					strcat(out_val, " 6 for Equip,");
				else
					strcat(out_val, " 4 or 6 for Equip,");
			}
#endif

			/* Append */
			if (allow_floor)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '-'����,");
				else if (allow_equip)
					strcat(out_val, " '4' ����,");
				else
					strcat(out_val, " '4'or'6' ����,");
#else
				if (!use_menu)
					strcat(out_val, " - for floor,");
				else if (allow_equip)
					strcat(out_val, " 4 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}

		}

		/* Viewing equipment */
		else if (command_wrk == (USE_EQUIP))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "������:");
#else
			sprintf(out_val, "Equip:");
#endif


			if (!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',",
#else
				sprintf(tmp_val, " %c-%c,'(',')',",
#endif

				index_to_label(e1), index_to_label(e2));

				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'����,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif


			/* Append */
			if (allow_inven)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '/' ����ʪ,");
				else if (allow_floor)
					strcat(out_val, " '4' ����ʪ,");
				else
					strcat(out_val, " '4'or'6' ����ʪ,");
#else

				if (!use_menu)
					strcat(out_val, " / for Inven,");
				else if (allow_floor)
					strcat(out_val, " 4 for Inven,");
				else
					strcat(out_val, " 4 or 6 for Inven,");
#endif
			}

			/* Append */
			if (allow_floor)
			{
#ifdef JP
				if (!use_menu)
					strcat(out_val, " '-'����,");
				else if (allow_inven)
					strcat(out_val, " '6' ����,");
				else
					strcat(out_val, " '4'or'6' ����,");
#else
				if (!use_menu)
					strcat(out_val, " - for floor,");
				else if (allow_inven)
					strcat(out_val, " 6 for floor,");
				else
					strcat(out_val, " 4 or 6 for floor,");
#endif
			}
		}

		/* Viewing floor */
		else if (command_wrk == (USE_FLOOR))
		{
			/* Begin the prompt */
#ifdef JP
			sprintf(out_val, "����:");
#else
			sprintf(out_val, "Floor:");
#endif


			if (!use_menu)
			{
				/* Build the prompt */
#ifdef JP
				sprintf(tmp_val, "%c-%c,'(',')',", n1, n2);
#else
				sprintf(tmp_val, " %c-%c,'(',')',", n1, n2);
#endif


				/* Append */
				strcat(out_val, tmp_val);
			}

			/* Indicate ability to "view" */
#ifdef JP
			if (!command_see && !use_menu) strcat(out_val, " '*'����,");
#else
			if (!command_see && !use_menu) strcat(out_val, " * to see,");
#endif


			if (use_menu)
			{
				if (allow_inven && allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4' ������, '6' ����ʪ,");
#else
					strcat(out_val, " 4 for Equip, 6 for Inven,");
#endif

				}
				else if (allow_inven)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' ����ʪ,");
#else
					strcat(out_val, " 4 or 6 for Inven,");
#endif

				}
				else if (allow_equip)
				{
#ifdef JP
					strcat(out_val, " '4'or'6' ������,");
#else
					strcat(out_val, " 4 or 6 for Equip,");
#endif

				}
			}
			/* Append */
			else if (allow_inven)
			{
#ifdef JP
				strcat(out_val, " '/' ����ʪ,");
#else
				strcat(out_val, " / for Inven,");
#endif

			}
			else if (allow_equip)
			{
#ifdef JP
				strcat(out_val, " '/'������,");
#else
				strcat(out_val, " / for Equip,");
#endif

			}

			/* Append */
			if (command_see && !use_menu)
			{
#ifdef JP
				strcat(out_val, " Enter ��,");
#else
				strcat(out_val, " Enter for scroll down,");
#endif
			}
		}

		/* Finish the prompt */
		strcat(out_val, " ESC");

		/* Build the prompt */
		sprintf(tmp_val, "(%s) %s", out_val, pmt);

		/* Show the prompt */
		prt(tmp_val, 0, 0);

		/* Get a key */
		which = inkey();

		if (use_menu)
		{
		int max_line = 1;
		if (command_wrk == USE_INVEN) max_line = max_inven;
		else if (command_wrk == USE_EQUIP) max_line = max_equip;
		else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
		switch (which)
		{
			case ESCAPE:
			case 'z':
			case 'Z':
			case '0':
			{
				done = TRUE;
				break;
			}

			case '8':
			case 'k':
			case 'K':
			{
				menu_line += (max_line - 1);
				break;
			}

			case '2':
			case 'j':
			case 'J':
			{
				menu_line++;
				break;
			}

			case '4':
			case 'h':
			case 'H':
			{
				/* Verify legality */
				if (command_wrk == (USE_INVEN))
				{
					if (allow_floor) command_wrk = USE_FLOOR;
					else if (allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (allow_inven) command_wrk = USE_INVEN;
					else if (allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_equip) command_wrk = USE_EQUIP;
					else if (allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if (command_wrk == USE_INVEN) max_line = max_inven;
				else if (command_wrk == USE_EQUIP) max_line = max_equip;
				else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case '6':
			case 'l':
			case 'L':
			{
				/* Verify legality */
				if (command_wrk == (USE_INVEN))
				{
					if (allow_equip) command_wrk = USE_EQUIP;
					else if (allow_floor) command_wrk = USE_FLOOR;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (allow_floor) command_wrk = USE_FLOOR;
					else if (allow_inven) command_wrk = USE_INVEN;
					else
					{
						bell();
						break;
					}
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_inven) command_wrk = USE_INVEN;
					else if (allow_equip) command_wrk = USE_EQUIP;
					else
					{
						bell();
						break;
					}
				}
				else
				{
					bell();
					break;
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Switch inven/equip */
				if (command_wrk == USE_INVEN) max_line = max_inven;
				else if (command_wrk == USE_EQUIP) max_line = max_equip;
				else if (command_wrk == USE_FLOOR) max_line = MIN(23, floor_num);
				if (menu_line > max_line) menu_line = max_line;

				/* Need to redraw */
				break;
			}

			case 'x':
			case 'X':
			case '\r':
			case '\n':
			{
				if (command_wrk == USE_FLOOR)
				{
					/* Special index */
					(*cp) = -get_item_label;
				}
				else
				{
					/* Validate the item */
					if (!get_item_okay(get_item_label))
					{
						bell();
						break;
					}

					/* Allow player to "refuse" certain actions */
					if (!get_item_allow(get_item_label))
					{
						done = TRUE;
						break;
					}

					/* Accept that choice */
					(*cp) = get_item_label;
				}

				item = TRUE;
				done = TRUE;
				break;
			}
		}
		if (menu_line > max_line) menu_line -= max_line;
		}
		else
		{
		/* Parse it */
		switch (which)
		{
			case ESCAPE:
			{
				done = TRUE;
				break;
			}

			case '*':
			case '?':
			case ' ':
			{
				/* Hide the list */
				if (command_see)
				{
					/* Flip flag */
					command_see = FALSE;

					/* Load screen */
					screen_load();
				}

				/* Show the list */
				else
				{
					/* Save screen */
					screen_save();

					/* Flip flag */
					command_see = TRUE;
				}
				break;
			}

			case '\n':
			case '\r':
			case '+':
			{
				int i, o_idx;
				cave_type *c_ptr = &cave[py][px];

				if (command_wrk != (USE_FLOOR)) break;

				/* Get the object being moved. */
				o_idx =	c_ptr->o_idx;

				/* Only rotate a pile of two or more objects. */
				if (!(o_idx && o_list[o_idx].next_o_idx)) break;

				/* Remove the first object from the list. */
				excise_object_idx(o_idx);

				/* Find end of the list. */
				i = c_ptr->o_idx;
				while (o_list[i].next_o_idx)
					i = o_list[i].next_o_idx;

				/* Add after the last object. */
				o_list[i].next_o_idx = o_idx;

				/* Re-scan floor list */ 
				floor_num = scan_floor(floor_list, py, px, 0x01);

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				break;
			}

			case '/':
			{
				if (command_wrk == (USE_INVEN))
				{
					if (!allow_equip)
					{
						bell();
						break;
					}
					command_wrk = (USE_EQUIP);
				}
				else if (command_wrk == (USE_EQUIP))
				{
					if (!allow_inven)
					{
						bell();
						break;
					}
					command_wrk = (USE_INVEN);
				}
				else if (command_wrk == (USE_FLOOR))
				{
					if (allow_inven)
					{
						command_wrk = (USE_INVEN);
					}
					else if (allow_equip)
					{
						command_wrk = (USE_EQUIP);
					}
					else
					{
						bell();
						break;
					}
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				/* Need to redraw */
				break;
			}

			case '-':
			{
				if (!allow_floor)
				{
					bell();
					break;
				}

				/*
				 * If we are already examining the floor, and there
				 * is only one item, we will always select it.
				 * If we aren't examining the floor and there is only
				 * one item, we will select it if floor_query_flag
				 * is FALSE.
				 */
				if (floor_num == 1)
				{
					if ((command_wrk == (USE_FLOOR)) || (!carry_query_flag))
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;

						break;
					}
				}

				/* Hack -- Fix screen */
				if (command_see)
				{
					/* Load screen */
					screen_load();

					/* Save screen */
					screen_save();
				}

				command_wrk = (USE_FLOOR);

				break;
			}

			case '0':
			case '1': case '2': case '3':
			case '4': case '5': case '6':
			case '7': case '8': case '9':
			{
				/* Look up the tag */
				if (!get_tag(&k, which))
				{
					bell();
					break;
				}

				/* Hack -- Validate the item */
				if ((k < INVEN_RARM) ? !inven : !equip)
				{
					bell();
					break;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}

#if 0
			case '\n':
			case '\r':
			{
				/* Choose "default" inventory item */
				if (command_wrk == (USE_INVEN))
				{
					k = ((i1 == i2) ? i1 : -1);
				}

				/* Choose "default" equipment item */
				else if (command_wrk == (USE_EQUIP))
				{
					k = ((e1 == e2) ? e1 : -1);
				}

				/* Choose "default" floor item */
				else if (command_wrk == (USE_FLOOR))
				{
					if (floor_num == 1)
					{
						/* Special index */
						k = 0 - floor_list[0];

						/* Allow player to "refuse" certain actions */
						if (!get_item_allow(k))
						{
							done = TRUE;
							break;
						}

						/* Accept that choice */
						(*cp) = k;
						item = TRUE;
						done = TRUE;
					}
					break;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					bell();
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
#endif

			default:
			{
				int ver;
				bool not_found = FALSE;

				/* Look up the alphabetical tag */
				if (!get_tag(&k, which))
				{
					not_found = TRUE;
				}

				/* Hack -- Validate the item */
				if ((k < INVEN_RARM) ? !inven : !equip)
				{
					not_found = TRUE;
				}

				/* Validate the item */
				if (!get_item_okay(k))
				{
					not_found = TRUE;
				}

				if (!not_found)
				{
					/* Accept that choice */
					(*cp) = k;
					item = TRUE;
					done = TRUE;
					break;
				}

				/* Extract "query" setting */
				ver = isupper(which);
				which = tolower(which);

				/* Convert letter to inventory index */
				if (command_wrk == (USE_INVEN))
				{
					if (which == '(') k = i1;
					else if (which == ')') k = i2;
					else k = label_to_inven(which);
				}

				/* Convert letter to equipment index */
				else if (command_wrk == (USE_EQUIP))
				{
					if (which == '(') k = e1;
					else if (which == ')') k = e2;
					else k = label_to_equip(which);
				}

				/* Convert letter to floor index */
				else if (command_wrk == USE_FLOOR)
				{
					if (which == '(') k = 0;
					else if (which == ')') k = floor_num - 1;
					else k = islower(which) ? A2I(which) : -1;
					if (k < 0 || k >= floor_num || k >= 23)
					{
						bell();
						break;
					}

					/* Special index */
					k = 0 - floor_list[k];
				}

				/* Validate the item */
				if ((command_wrk != USE_FLOOR) && !get_item_okay(k))
				{
					bell();
					break;
				}

				/* Verify the item */
#ifdef JP
				if (ver && !verify("������", k))
#else
				if (ver && !verify("Try", k))
#endif

				{
					done = TRUE;
					break;
				}

				/* Allow player to "refuse" certain actions */
				if (!get_item_allow(k))
				{
					done = TRUE;
					break;
				}

				/* Accept that choice */
				(*cp) = k;
				item = TRUE;
				done = TRUE;
				break;
			}
		}
		}
	}

	/* Fix the screen if necessary */
	if (command_see)
	{
		/* Load screen */
		screen_load();

		/* Hack -- Cancel "display" */
		command_see = FALSE;
	}


	/* Forget the item_tester_tval restriction */
	item_tester_tval = 0;

	/* Forget the item_tester_hook restriction */
	item_tester_hook = NULL;


	/* Clean up  'show choices' */
	/* Toggle again if needed */
	if (toggle) toggle_inven_equip();

	/* Update */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* Window stuff */
	window_stuff();


	/* Clear the prompt line */
	prt("", 0, 0);

	/* Warning if needed */
	if (oops && str) msg_print(str);

	if (item)
	{
		repeat_push(*cp);
		command_cmd = 0; /* Hack -- command_cmd is no longer effective */
	}


	/* Result */
	return (item);
}


static bool py_pickup_floor_aux(void)
{
	s16b this_o_idx;

	cptr q, s;

	int item;

	/* Restrict the choices */
	item_tester_hook = inven_carry_okay;

	/* Get an object */
#ifdef JP
	q = "�ɤ�򽦤��ޤ�����";
	s = "�⤦���å��ˤϾ��ˤ���ɤΥ����ƥ������ʤ���";
#else
	q = "Get which item? ";
	s = "You no longer have any room for the objects on the floor.";
#endif

	if (get_item(&item, q, s, (USE_FLOOR)))
	{
		this_o_idx = 0 - item;
	}
	else
	{
		return (FALSE);
	}

	/* Pick up the object */
	py_pickup_aux(this_o_idx);

	return (TRUE);
}


/*
 * Make the player carry everything in a grid
 *
 * If "pickup" is FALSE then only gold will be picked up
 *
 * This is called by py_pickup() when easy_floor is TRUE.
 */
void py_pickup_floor(int pickup)
{
	s16b this_o_idx, next_o_idx = 0;

	char o_name[MAX_NLEN];
	object_type *o_ptr;

	int floor_num = 0, floor_list[23], floor_o_idx = 0;

	int can_pickup = 0;

	/* Scan the pile of objects */
	for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		/* Access the object */
		o_ptr = &o_list[this_o_idx];

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Access the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Message */
#ifdef JP
			msg_format(" $%ld �β��ͤ�����%s�򸫤Ĥ�����",
			           (long)o_ptr->pval, o_name);
#else
			msg_format("You have found %ld gold pieces worth of %s.",
				(long) o_ptr->pval, o_name);
#endif


			/* Collect the gold */
			p_ptr->au[o_ptr->sval] += o_ptr->pval;

			/* Update gold */
			p_ptr->update |= (PU_GOLD);

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Check the next object */
			continue;
		}
		else if (o_ptr->marked & OM_NOMSG)
		{
			/* If 0 or 1 non-NOMSG items are in the pile, the NOMSG ones are
			 * ignored. Otherwise, they are included in the prompt. */
			o_ptr->marked &= ~(OM_NOMSG);
			continue;
		}

		/* Count non-gold objects that can be picked up. */
		if (inven_carry_okay(o_ptr))
		{
			can_pickup++;
		}

		/* Remember this object index */
		if (floor_num < 23)
			floor_list[floor_num] = this_o_idx;

		/* Count non-gold objects */
		floor_num++;

		/* Remember this index */
		floor_o_idx = this_o_idx;
	}

	/* There are no non-gold objects */
	if (!floor_num)
		return;

	/* Mention the number of objects */
	if (!pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Access the object */
			o_ptr = &o_list[floor_o_idx];

			/* Sense the object */
			sense_floor_object(floor_o_idx);

			/* Describe the object */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Message */
#ifdef JP
			msg_format("%s�����롣", o_name);
#else
			msg_format("You see %s.", o_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("%d �ĤΥ����ƥ�λ������롣", floor_num);
#else
			msg_format("You see a pile of %d items.", floor_num);
#endif
			/* Scan the pile of objects */
			for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Access the object */
				o_ptr = &o_list[this_o_idx];

				/* Sense the object */
				sense_floor_object(this_o_idx);

				/* Access the next object */
				next_o_idx = o_ptr->next_o_idx;
			}
		}

		/* Done */
		return;
	}

	/* The player has no room for anything on the floor. */
	if (!can_pickup)
	{
		/* One object */
		if (floor_num == 1)
		{
			/* Access the object */
			o_ptr = &o_list[floor_o_idx];

			/* Sense the object */
			sense_floor_object(floor_o_idx);

			/* Describe the object */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Message */
#ifdef JP
			msg_format("���å��ˤ�%s��������֤��ʤ���", o_name);
#else
			msg_format("You have no room for %s.", o_name);
#endif

		}

		/* Multiple objects */
		else
		{
			/* Message */
#ifdef JP
			msg_format("���å��ˤϾ��ˤ���ɤΥ����ƥ������ʤ���", o_name);
#else
			msg_print("You have no room for any of the objects on the floor.");
#endif

		}

		/* Done */
		return;
	}

	/* One object */
	if (floor_num == 1)
	{
		/* Hack -- query every object */
		if (carry_query_flag)
		{
			char out_val[MAX_NLEN+20];

			/* Access the object */
			o_ptr = &o_list[floor_o_idx];

			/* Sense the object */
			sense_floor_object(floor_o_idx);

			/* Describe the object */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Build a prompt */
#ifdef JP
			(void) sprintf(out_val, "%s�򽦤��ޤ���? ", o_name);
#else
			(void) sprintf(out_val, "Pick up %s? ", o_name);
#endif


			/* Ask the user to confirm */
			if (!get_check(out_val))
			{
				/* Done */
				return;
			}
		}

		/* Access the object */
		o_ptr = &o_list[floor_o_idx];

		/* Sense the object */
		sense_floor_object(floor_o_idx);

		/* Pick up the object */
		py_pickup_aux(floor_o_idx);
	}

	/* Allow the user to choose an object */
	else
	{
		while (can_pickup--)
		{
			if (!py_pickup_floor_aux()) break;
		}
	}
}
