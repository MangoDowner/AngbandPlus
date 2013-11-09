/* File: do-spell.c */

/* Purpose: Do everything for each spell */

#include "angband.h"


/*
 * Generate dice info string such as "foo 2d10"
 */
static cptr info_string_dice(cptr str, int dice, int sides, int base)
{
	/* Fix value */
	if (!dice)
		return format("%s%d", str, base);

	/* Dice only */
	else if (!base)
		return format("%s%dd%d", str, dice, sides);

	/* Dice plus base value */
	else
		return format("%s%dd%d%+d", str, dice, sides, base);
}


/*
 * Generate damage-dice info string such as "dam 2d10"
 */
static cptr info_damage(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("»��:", dice, sides, base);
#else
	return info_string_dice("dam ", dice, sides, base);
#endif
}


/*
 * Generate duration info string such as "dur 20+1d20"
 */
static cptr info_duration(int base, int sides)
{
#ifdef JP
	return format("����:%d+1d%d", base, sides);
#else
	return format("dur %d+1d%d", base, sides);
#endif
}


/*
 * Generate range info string such as "range 5"
 */
static cptr info_range(int range)
{
#ifdef JP
	return format("�ϰ�:%d", range);
#else
	return format("range %d", range);
#endif
}


/*
 * Generate heal info string such as "heal 2d8"
 */
static cptr info_heal(int dice, int sides, int base)
{
#ifdef JP
	return info_string_dice("����:", dice, sides, base);
#else
	return info_string_dice("heal ", dice, sides, base);
#endif
}


/*
 * Generate delay info string such as "delay 15+1d15"
 */
static cptr info_delay(int base, int sides)
{
#ifdef JP
	return format("�ٱ�:%d+1d%d", base, sides);
#else
	return format("delay %d+1d%d", base, sides);
#endif
}


/*
 * Generate multiple-damage info string such as "dam 25 each"
 */
static cptr info_multi_damage(int dam)
{
#ifdef JP
	return format("»��:��%d", dam);
#else
	return format("dam %d each", dam);
#endif
}


/*
 * Generate multiple-damage-dice info string such as "dam 5d2 each"
 */
static cptr info_multi_damage_dice(int dice, int sides)
{
#ifdef JP
	return format("»��:��%dd%d", dice, sides);
#else
	return format("dam %dd%d each", dice, sides);
#endif
}


/*
 * Generate multiple-damage-dice-base info string such as "dam 10 + d10 each"
 */
static cptr info_multi_damage_dice_base(int base, int sides)
{
#ifdef JP
	return format("»��:��%d+d%d", base, sides);
#else
	return format("dam %d+d%d each", base, sides);
#endif
}


/*
 * Generate call_the_element info string such as "d (15 + d15)*3"
 */
static cptr info_call_the_elemental(int base, int sides, int attacks)
{
#ifdef JP
	return format("»:(%d + d%d)*%d", base, sides, attacks);
#else
	return format("d (%d + d%d)*%d", base, sides, attacks);
#endif
}


/*
 * Generate power info string such as "power 100"
 */
static cptr info_power(int power)
{
#ifdef JP
	return format("����:%d", power);
#else
	return format("power %d", power);
#endif
}


/*
 * Generate radius info string such as "rad 100"
 */
static cptr info_radius(int rad)
{
#ifdef JP
	return format("Ⱦ��:%d", rad);
#else
	return format("rad %d", rad);
#endif
}


void stop_singing(void)
{
	if (!p_ptr->singing && !p_ptr->restart_singing) return;

	if (p_ptr->restart_singing)
	{
		p_ptr->restart_singing = 0;
		return;
	}
	if (!p_ptr->singing) return;

	/* Hack -- if called from set_action(), avoid recursive loop */
	if (p_ptr->action == ACTION_SING) set_action(ACTION_NONE);

	switch (p_ptr->singing)
	{
	case MUSIC_SILENT:
		song_of_silence(0);
		break;
	}

	p_ptr->singing = MUSIC_NONE;
	p_ptr->song_start = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}


/*
 * Drop 10+1d10 disintegration ball at random places near the target
 */
static bool cast_wrath_of_the_god(int dam, int rad)
{
	int x, y, tx, ty;
	int dir, i;
	int b = 10 + randint1(10);

	if (!get_aim_dir(&dir)) return FALSE;
	range_restricted_target(dir, MAX_RANGE, &ty, &tx, TRUE);

	for (i = 0; i < b; i++)
	{
		int count = 20, d = 0;

		while (count--)
		{
			int dx, dy;

			x = tx - 5 + randint0(11);
			y = ty - 5 + randint0(11);

			dx = (tx > x) ? (tx - x) : (x - tx);
			dy = (ty > y) ? (ty - y) : (y - ty);

			/* Approximate distance */
			d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
			/* Within the radius */
			if (d < 5) break;
		}

		if (count < 0) continue;

		/* Cannot penetrate perm walls */
		if (!in_bounds(y,x) ||
		    cave_stop_disintegration(y,x) ||
		    !in_disintegration_range(ty, tx, y, x))
			continue;

		project(0, rad, y, x, dam, GF_DISINTEGRATE, PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, MODIFY_ELEM_MODE_MAGIC);
	}

	return TRUE;
}


static bool cast_stop_the_time(void)
{
	if (stop_the_time_player)
	{
#ifdef JP
		msg_print("���˻��ϻߤޤäƤ��롣");
#else
		msg_print("Time is already stopped.");
#endif
		return FALSE;
	}
	stop_the_time_player = TRUE;
#ifdef JP
	msg_print("����ߤ᤿��");
#else
	msg_print("You stopped the time!");
#endif
	msg_print(NULL);

	/* Hack */
	p_ptr->energy_need -= 1000 + (100 * (4 + damroll(2, 2)))*TURNS_PER_TICK/10;

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	handle_stuff();

	return TRUE;
}


static bool cast_melt_weapon(int power)
{
	int tx, ty;

#ifdef JP
	msg_print("ï�ι����Ϥ���뤫���ꤷ�Ʋ�������");
#else
	msg_print("Choose the target to weaken.");
#endif
	if (!tgt_pt(&tx, &ty, FALSE)) return FALSE;
	if (!player_has_los_bold(ty, tx))
	{
#ifdef JP
		msg_print("���ξ�����ꤹ�뤳�ȤϤǤ��ޤ���");
#else
		msg_print("You can't specify that place.");
#endif
		return FALSE;
	}

	if (!cave[ty][tx].m_idx)
	{
#ifdef JP
		msg_print("�����ˤϥ�󥹥��������ޤ���");
#else
		msg_print("There is no monster.");
#endif
		return FALSE;
	}

	if (cave[ty][tx].m_idx)
	{
		monster_type *m_ptr = &m_list[cave[ty][tx].m_idx];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
		if (r_ptr->level > randint1((power - 10) < 1 ? 1 : (power - 10)) + 10)
		{
#ifdef JP
			msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
			msg_format("%^s is unaffected!", m_name);
#endif
		}
		else if (!MON_MELT_WEAPON(m_ptr))
		{
#ifdef JP
			msg_format("%^s�ι����Ϥ���ޤä��褦����", m_name);
#else
			msg_format("Damage of %^s is seems to weakened.", m_name);
#endif
			(void)set_monster_melt_weapon(cave[ty][tx].m_idx, power);
		}
	}
	return TRUE;
}


static bool cast_dispel_magic(void)
{
	int tx, ty;
	int dir;

#ifdef JP
	msg_print("ï�����Ϥ�õ�뤫���ꤷ�Ʋ�������");
#else
	msg_print("Choose the target to dispel.");
#endif
	if (!get_aim_dir(&dir)) return FALSE;
	range_restricted_target(dir, MAX_RANGE, &ty, &tx, FALSE);

	if (!((ty == py) && (tx == px)) && !cave[ty][tx].m_idx)
	{
#ifdef JP
		msg_print("�����ˤ�ï�⤤�ޤ���");
#else
		msg_print("There is no one.");
#endif
		return FALSE;
	}

	if ((ty == py) && (tx == px))
	{
		dispel_player();

		if (p_ptr->riding) dispel_monster_status(p_ptr->riding);
	}
	else if (cave[ty][tx].m_idx)
	{
		dispel_monster_status(cave[ty][tx].m_idx);
	}
	return TRUE;
}


static bool cast_change_element(int power)
{
	int tx, ty;
	int dir;

#ifdef JP
	msg_print("ï�Υ�����Ȥ�ȿž�����뤫���ꤷ�Ʋ�������");
#else
	msg_print("Choose the target to revert elements.");
#endif
	if (!get_aim_dir(&dir)) return FALSE;
	range_restricted_target(dir, MAX_RANGE, &ty, &tx, FALSE);

	if (!((ty == py) && (tx == px)) && !cave[ty][tx].m_idx)
	{
#ifdef JP
		msg_print("�����ˤ�ï�⤤�ޤ���");
#else
		msg_print("There is no one.");
#endif
		return FALSE;
	}

	if ((ty == py) && (tx == px))
	{
		if (!p_ptr->opposite_pelem) set_opposite_pelem(power);
	}
	else if (cave[ty][tx].m_idx)
	{
		monster_type *m_ptr = &m_list[cave[ty][tx].m_idx];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		char m_name[80];

		monster_desc(m_name, m_ptr, 0);
		if (r_ptr->level > randint1((power - 10) < 1 ? 1 : (power - 10)) + 10)
		{
#ifdef JP
			msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
			msg_format("%^s is unaffected!", m_name);
#endif
		}
		else if (!MON_OPPOSITE_ELEM(m_ptr))
		{
#ifdef JP
			msg_format("%^s�Υ�����Ȥ�ȿž������", m_name);
#else
			msg_format("Elements of %^s are reverted.", m_name);
#endif
			(void)set_monster_opposite_elem(cave[ty][tx].m_idx, power);
			if (p_ptr->action == ACTION_ELEMSCOPE) lite_spot(m_ptr->fy, m_ptr->fx);
		}
	}
	return TRUE;
}


static cptr do_magery_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_MAGERY]/10;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ޥ��å����ߥ�����";
		if (desc) return "�夤��ˡ�Υܥ�Ȥ����ġ�";
#else
		if (name) return "Magic Missile";
		if (desc) return "Fires a weak bolt of magic.";
#endif
    
		{
			int dice = 3 + ((mlev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_MISSILE, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "��󥹥�������";
		if (desc) return "�᤯�����Ƥθ������󥹥������Τ��롣";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "���硼�ȡ��ƥ�ݡ���";
		if (desc) return "���Υ�Υƥ�ݡ��Ȥ򤹤롣";
#else
		if (name) return "Phase Door";
		if (desc) return "Teleport short distance.";
#endif
    
		{
			int range = 10;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "櫤��ⴶ��";
		if (desc) return "�᤯�����Ƥ�櫤���ȳ��ʤ��Τ��롣";
#else
		if (name) return "Detect Doors and Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�饤�ȡ����ꥢ";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τ�ʵפ����뤯���롣";
#else
		if (name) return "Light Area";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = mlev / 2;
			int rad = mlev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "櫡����˲�";
		if (desc) return "��ľ��������Ƥ�櫤�����˲����롣";
#else
		if (name) return "Trap & Door Destruction";
		if (desc) return "Fires a beam which destroy traps and doors.";
#endif
    
		{
			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				destroy_door(dir);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�ƥ�ݡ���";
		if (desc) return "���Υ�Υƥ�ݡ��Ȥ򤹤롣";
#else
		if (name) return "Teleport";
		if (desc) return "Teleport long distance.";
#endif
    
		{
			int range = mlev * 5;

			if (info) return info_range(range);

			if (cast)
			{
				teleport_player(range);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "��������";
		if (desc) return "��ˡ�ε�����ġ�";
#else
		if (name) return "Mana Burst";
		if (desc) return "Fires a ball of magic.";
#endif
    
		{
			int dice = 3;
			int sides = 5;
			int rad = (mlev < 30) ? 2 : 3;
			int base;

			if (p_ptr->pclass == CLASS_WIZARD ||
			    p_ptr->pclass == CLASS_SIRENE ||
			    p_ptr->pclass == CLASS_LICH ||
			    p_ptr->pclass == CLASS_ARCHMAGE)
				base = mlev + mlev / 2;
			else
				base = mlev + mlev / 4;


			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MISSILE, dir, damroll(dice, sides) + base, rad, FALSE);

				/*
				 * Shouldn't actually use GF_MANA, as
				 * it will destroy all items on the
				 * floor
				 */
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�����ϲ�";
		if (desc) return "�ɤ��Ϥ����ƾ��ˤ��롣";
#else
		if (name) return "Stone to Mud";
		if (desc) return "Turns one rock square to mud.";
#endif
    
		{
			int dice = 3 + ((mlev - 5) / 4);
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				wall_to_mud(dir);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "��ˡ���Ͽ�";
		if (desc) return "���դ��Ϸ����Τ��롣";
#else
		if (name) return "Magic Mapping";
		if (desc) return "Maps nearby area.";
#endif
    
		{
			int rad = DETECT_RAD_MAP;

			if (info) return info_radius(rad);

			if (cast)
			{
				map_area(rad);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "����";
		if (desc) return "�����ƥ��1�ļ��̤��롣��٥�35�ǥ����ƥ��ǽ�Ϥ������Τ뤳�Ȥ��Ǥ��롣";
#else
		if (name) return "Identify";
		if (desc) return "Identifies an item.";
#endif
    
		{
			if (cast)
			{
				if (mlev < 35)
				{
					if (!ident_spell(FALSE)) return NULL;
				}
				else
				{
					if (!identify_fully(FALSE)) return NULL;
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "��������";
		if (desc) return "������֡��ƥ�ѥ���ǽ�Ϥ����롣";
#else
		if (name) return "Sense Minds";
		if (desc) return "Gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_esp(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "���Ϥ���";
		if (desc) return "�������ϤΥܥ�Ȥ����ġ�";
#else
		if (name) return "Mana Bolt";
		if (desc) return "Fires a beam of pure mana.";
#endif
    
		{
			int dice = 11 + (mlev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_MANA, dir, damroll(dice, sides));
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "��ʢ��­";
		if (desc) return "��ʢ�ˤ��롣";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "���Ԥμ�ʸ";
		if (desc) return "�Ͼ�ˤ���Ȥ��ϥ��󥸥��κǿ����ء����󥸥��ˤ���Ȥ����Ͼ�ؤȰ�ư���롣";
#else
		if (name) return "Word of Recall";
		if (desc) return "Recalls player from dungeon to town, or from town to the deepest level of dungeon.";
#endif
    
		{
			int base = 15;
			int sides = 20;

			if (info) return info_delay(base, sides);

			if (cast)
			{
				if (!word_of_recall()) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "Ĵ��";
		if (desc) return "��󥹥�����°�����Ĥ����ϡ��������ϡ����ԡ��ɡ����Τ��Τ롣";
#else
		if (name) return "Probing";
		if (desc) return "Proves all monsters' alignment, HP, speed and their true character.";
#endif
    
		{
			if (cast)
			{
				probing();
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "���Ͻ�Ŷ";
		if (desc) return "��/��ˡ���ν�Ŷ��������䤹������Ŷ��Υ�åɤν�Ŷ���֤򸺤餹��";
#else
		if (name) return "Recharging";
		if (desc) return "Recharges staffs, wands or rods.";
#endif
    
		{
			int power;
			if ((p_ptr->pclass == CLASS_WITCH) || (p_ptr->pclass == CLASS_HIGHWITCH)) power = mlev * 8;
			else power = mlev * 4;

			if (info) return info_power(power);

			if (cast)
			{
				if (!recharge(power)) return NULL;
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "�ƥ�ݡ��ȡ���٥�";
		if (desc) return "�ֻ��˾夫���γ��˥ƥ�ݡ��Ȥ��롣";
#else
		if (name) return "Teleport Level";
		if (desc) return "Teleport to up or down stairs in a moment.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("������¾�γ��˥ƥ�ݡ��Ȥ��ޤ�����")) return NULL;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return NULL;
#endif
				teleport_level(0);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�ƥ�ݡ��ȡ���������";
		if (desc) return "��ľ����Υ�󥹥�����ƥ�ݡ��Ȥ����롣�񹳤�����̵����";
#else
		if (name) return "Teleport Away";
		if (desc) return "Teleports all monsters on the line away unless resisted.";
#endif
    
		{
			int power = mlev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_AWAY_ALL, dir, power);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "������";
		if (desc) return "�᤯�����ƤΥ�󥹥�����櫡��⡢���ʡ������������ƥ����ƥ���Τ��롣";
#else
		if (name) return "Detection True";
		if (desc) return "Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_all(rad);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "��ˡ���å�";
		if (desc) return "���åȤ�ȯ�ͤ��롣";
#else
		if (name) return "Magic Rocket";
		if (desc) return "Fires a magic rocket.";
#endif
    
		{
			int dam = 120 + mlev * 2;
			int rad = 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_print("���å�ȯ�͡�");
#else
				msg_print("You launch a rocket!");
#endif

				fire_rocket(GF_ROCKET, dir, dam, rad, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "��Τ��";
		if (desc) return "���γ����Τ�ʵפ˾Ȥ餷�����󥸥���⤹�٤ƤΥ����ƥ���Τ��롣����ˡ�������֥ƥ�ѥ���ǽ�Ϥ����롣";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			int base = 25;
			int sides = 30;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				wiz_lite(FALSE);

				if (!p_ptr->telepathy)
				{
					set_tim_esp(randint1(sides) + base, FALSE);
				}
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "���⾤��";
		if (desc) return "�����1�ξ������롣";
#else
		if (name) return "Summon Demon";
		if (desc) return "Summons a demon.";
#endif
    
		{
			if (cast)
			{
				u32b mode = 0L;
				bool pet = !one_in_(3);

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_NO_PET | PM_IGNORE_AMGRID);

				if (summon_specific((pet ? -1 : 0), py, px, (mlev * 3) / 2, SUMMON_DEMON, mode))
				{
#ifdef JP
					msg_print("β���ΰ���������������");
#else
					msg_print("The area fills with a stench of sulphur and brimstone.");
#endif

					if (pet)
					{
#ifdef JP
						msg_print("�֤��ѤǤ������ޤ�����������͡�");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("���ܤ����Ԥ衢�����β��ͤˤ��餺�� �����κ���ĺ��������");
#else
						msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif
					}
				}
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "����ư";
		if (desc) return "���ذ�ư���롣�Ͼ�ˤ���Ȥ������Ȥ��ʤ���";
#else
		if (name) return "Teleport to town";
		if (desc) return "Teleport to a town which you choose in a moment. Can only be used outdoors.";
#endif
    
		{
			if (cast)
			{
				if (!tele_town(TRUE)) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�����ζ����";
		if (desc) return "1�Τθ���ɥ饴��򾤴����롣";
#else
		if (name) return "Trump Ancient Dragon";
		if (desc) return "Summons an ancient dragon.";
#endif
    
		{
			if (cast)
			{
				if (!summon_specific(-1, py, px, mlev * 2 / 3 + randint1(mlev/2), SUMMON_HI_DRAGON, PM_FORCE_PET))
				{
#ifdef JP
					msg_print("����ɥ饴��ϸ���ʤ��ä���");
#else
					msg_print("No ancient dragons arrive.");
#endif
				}
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "���Ϥ���";
		if (desc) return "���˶��Ϥǵ���ʽ������Ϥε�����ġ�";
#else
		if (name) return "Mana Storm";
		if (desc) return "Fires an extremely powerful huge ball of pure mana.";
#endif
    
		{
			int dam = 300 + mlev * 4;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_MANA, dir, dam, rad, FALSE);
			}
		}
		break;

	}

	return "";
}


static cptr do_fire_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_FIRE]/10;
	int pstat = p_ptr->stat_use[A_INT];

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "���ѡ������ե���";
		if (desc) return "�ץ饺�ޤΥӡ�������ġ�";
#else
		if (name) return "Spark sphere";
		if (desc) return "Fires a bolt of plasma.";
#endif
    
		{
			int dice = 3 + (mlev - 1) / 5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_PLASMA, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�ǥե�����ͥ�";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������ΤβФΥ�����Ȥ򶯤ᡢ��Υ�����Ȥ���롣";
#else
		if (name) return "Deft-Zoshonell";
		if (desc) return "Increase FIRE, decrease AQUA nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = 2;
			int rad = (mlev / 10) + 1;

			if (pstat >= (18 + 100)) amount++;
			if (pstat >= (18 + 150)) amount++;
			if (pstat >= (18 + 200)) amount++;

			if (info) return info_radius(rad);

			if (cast)
			{
				inc_area_elem(0, ELEM_FIRE, amount, rad, TRUE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�ե��������ܥ��";
		if (desc) return "��Υܥ�Ȥ����ġ�";
#else
		if (name) return "Fire bolt";
		if (desc) return "Fires a bolt of fire.";
#endif
    
		{
			int dice = 8 + (mlev - 1) / 5;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_FIRE, dir, damroll(dice, sides));
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�����󥹥�����";
		if (desc) return "����ε�����ġ�";
#else
		if (name) return "Stun slaughter";
		if (desc) return "Fires a ball of stun.";
#endif
    
		{
			int power = mlev + 50;
			int rad = 1;
			if (pstat >= (18 + 100))
			{
				power = mlev + 60;
				rad++;
			}
			if (pstat >= (18 + 150))
			{
				power = mlev + 70;
				rad++;
			}

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_STASIS, dir, power, rad, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�ե��������ȡ���";
		if (desc) return "���ꤷ��������������ϰϤ˲б깶���Ԥ���";
#else
		if (name) return "Fire storm";
		if (desc) return "Fires a ball of fire.";
#endif
    
		{
			int sides = mlev;
			int base = 10 + mlev * 2;
			int rad = (pstat >= (18 + 150)) ? 3 : 2;

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_FIRE, dir, base + randint1(sides), rad, TRUE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "���ꥢ������";
		if (desc) return "ŷ�����ɲ������롣";
#else
		if (name) return "Clear sky";
		if (desc) return "weather to fine.";
#endif
    
		{
			int power = -2;
			if (pstat >= (18 + 150)) power--;
			if (pstat >= (18 + 200)) power--;

			if (cast)
			{
				msg_print("ŷ�������䤫�ˤʤäƤ���...");
				set_weather(power, power, power);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�ҡ��ȥ����ݥ�";
		if (desc) return "���˱��°����Ĥ��롣";
#else
		if (name) return "Heat weapon";
		if (desc) return "Makes current weapon fire branded.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(EGO_ZOSHONELL);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���Ǥ�����";
		if (desc) return "������֡����ֵ�ž�ؤ����������롣";
#else
		if (name) return "Immortal Body";
		if (desc) return "Gives resistance to time for a while.";
#endif
    
		{
			int sides = 20;
			int base = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_res_time(randint1(sides)+base, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "����ޥ����";
		if (desc) return "���ꤷ��������������ϰ���Υ�󥹥�����̵���̤˲б깶��򷫤��֤���";
#else
		if (name) return "Salamander";
		if (desc) return "Fires a bolt or beam of fire.";
#endif
    
		{
			int base = mlev;
			int attacks = 3;
			if (pstat >= (18 + 100)) attacks++;
			if (pstat >= (18 + 150)) attacks++;
			if (pstat >= (18 + 180)) attacks++;
			if (pstat >= (18 + 200)) attacks++;
			if (pstat >= (18 + 220)) attacks++;

			if (info) return info_call_the_elemental(base, base, attacks);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_format("�����ޥ�����򾤴�������");
#else
				msg_format("You called the Salamander.");
#endif
				cast_call_the_elemental(GF_FIRE, dir, base, 1, base, 3, A_INT);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "����ॾ��Ρ���";
		if (desc) return "�ץ饺�ޤΥ֥쥹���Ǥ���";
#else
		if (name) return "Crimson Naught";
		if (desc) return "Breath of plasma.";
#endif
    
		{
			int base = MIN(p_ptr->chp / 2, 350);

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_PLASMA, dir, base, -2, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "����Ȥζ���";
		if (desc) return "�������åȤμ��Ϥ�*�б�*�ε��¿��ȯ�������롣";
#else
		if (name) return "Fire of Surtr";
		if (desc) return "Fires balls of *fire*.";
#endif
    
		{
			int dam = mlev * 3 + 25;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				int x, y, tx, ty;
				int dir, i;
				int b = 5 + randint1(10);

				if (!get_aim_dir(&dir)) return NULL;

				range_restricted_target(dir, MAX_RANGE, &ty, &tx, TRUE);

				for (i = 0; i < b; i++)
				{
					int count = 20, d = 0;

					while (count--)
					{
						int dx, dy;

						x = tx - 5 + randint0(11);
						y = ty - 5 + randint0(11);

						dx = (tx > x) ? (tx - x) : (x - tx);
						dy = (ty > y) ? (ty - y) : (y - ty);

						/* Approximate distance */
						d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
						/* Within the radius */
						if (d < 5) break;
					}

					if (count < 0) continue;

					/* Cannot penetrate walls */
					if (!in_bounds(y, x) || !los(ty, tx, y, x)) continue;

					project(0, 2, y, x, dam, GF_PURE_FIRE, PROJECT_JUMP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, MODIFY_ELEM_MODE_MAGIC);
				}
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "���ȥ饤���Υ���";
		if (desc) return "�б������Τʤ���󥹥���1�Τ�����᡼����Ϳ���롣�б������������󥹥����ˤ�̵����";
#else
		if (name) return "Strike Nova";
		if (desc) return "";
#endif
    
		{
			int base = 999;

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_STRIKE_NOVA, dir, base, 0, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "������ͥ�βø�";
		if (desc) return "������֡��б�Υ�����Ȳб�ȥץ饺�ޤؤ��ȱ֤��դ���HP�����Ϥȴ��Ѥ����Ƿ⹶��ǽ�Ϥ��徺������®���뤬���䵤�ȿ��*��*�˼夯�ʤ롣";
#else
		if (name) return "Zoshonell protect";
		if (desc) return "Gives immunity to fire and plasma, fire aura,  for a while..";
#endif
    
		{
			int sides = 10;
			int base = 10;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_zoshonel_protect(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�����ѡ��Υ���";
		if (desc) return "�볦����ϰϤ��Ф��ƥץ饺�޹����Ԥ�����ʬ����᡼��������롣����ˡ�ŷ������������ˤ��롣�ؼ�����Ѥ���ԤϿͿ��򼺤���";
#else
		if (name) return "Super Nova";
		if (desc) return "Damage by plasma in sight, weather to fine.";
#endif
    
		{
			int base = mlev * 8;
			int sides = mlev * 5;

			if (info) return info_multi_damage_dice_base(base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_PLASMA, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "�����ѡ��Υ����μ�ʸ�δ���ź��");
#else
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "the strain of casting Super Nova");
#endif

				set_weather(-15, 0, -15);
				change_chaos_frame(ETHNICITY_WALSTANIAN, -1);
				change_chaos_frame(ETHNICITY_GARGASTAN, -1);
				change_chaos_frame(ETHNICITY_BACRUM, -1);
			}
		}
		break;


	}

	return "";
}


static cptr do_aqua_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_AQUA]/10;
	int pstat = p_ptr->stat_use[A_INT];

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ǥե��롼��";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Το�Υ�����Ȥ򶯤ᡢ�ФΥ�����Ȥ���롣";
#else
		if (name) return "Deft Gruza";
		if (desc) return "Increase FIRE, decrease AQUA nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = 2;
			int rad = (mlev / 10) + 1;

			if (pstat >= (18 + 100)) amount++;
			if (pstat >= (18 + 150)) amount++;
			if (pstat >= (18 + 200)) amount++;


			if (info) return info_radius(rad);

			if (cast)
			{
				inc_area_elem(0, ELEM_AQUA, amount, rad, TRUE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�˴�����";
		if (desc) return "�˴��Υܥ�Ȥ����ġ�";
#else
		if (name) return "Ice bolt";
		if (desc) return "Fires a bolt of ice.";
#endif
    
		{
			int dice = 6 + (mlev - 1) / 5;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ICE, dir, damroll(dice, sides));
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�������֥饹��";
		if (desc) return "���ꤷ��������������ϰϤ��䵤�����Ԥ���";
#else
		if (name) return "Ice Blast";
		if (desc) return "Fires a ball of cold.";
#endif
    
		{
			int sides = mlev;
			int base = 10 + mlev * 2;
			int rad = (pstat >= (18 + 150)) ? 3 : 2;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_COLD, dir, base + randint1(sides), rad, TRUE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���ꥢ�֥�å�";
		if (desc) return "����������������Ǥ��Τ��鴰���˼����������Ϥ򾯤����������롣";
#else
		if (name) return "Clear blood";
		if (desc) return "Heals all cut and poison status. Heals HP a little.";
#endif
    
		{
			int dice = 4;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				set_poisoned(0);
				set_cut(0);
				hp_player(damroll(dice, sides));
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�����ࡼ��";
		if (desc) return "��󥹥���1�Τ�®�����롣�񹳤�����̵����";
#else
		if (name) return "Slow Monster";
		if (desc) return "Attempts to slow a monster.";
#endif
    
		{
			int power = mlev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				slow_monster(dir, power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "��������";
		if (desc) return "������֡��䵤�Υ���������롣";
#else
		if (name) return "Absolute zero";
		if (desc) return "Gives aura of cold for a while.";
#endif
    
		{
			int sides = 25 + mlev;
			int base = mlev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_tim_sh_cold(randint1(sides)+base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�������������ܡ���";
		if (desc) return "��ε�����ġ�";
#else
		if (name) return "Water ball";
		if (desc) return "Fires a ball of water.";
#endif
    
		{
			int base = 100 + mlev * 3 / 2;
			int rad = (mlev / 12) + 1;

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_WATER, dir, base, rad, TRUE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�ݥ�����ϥ�����";
		if (desc) return "��ʬ���濴�Ȥ��������ϰϤ˿塢�ǡ����𹶷��Ԥ�����ǽ���⤤����������̿�ϵۼ������Ԥ���";
#else
		if (name) return "Poison hazard";
		if (desc) return "";
#endif
    
		{
			int rad = mlev / 10;
			int dam = 15 + mlev / 2;
			int attacks = 3;
			bool disenchant = FALSE;
			bool drain = FALSE;

			if (pstat >= (18 + 150))
			{
				disenchant = TRUE;
				attacks++;
			}
			if (pstat >= (18 + 200))
			{
				drain = TRUE;
				attacks++;
			}

#ifdef JP
			if (info) return format("»��:%d*%d", dam, attacks);
#else
			if (info) return format("dam %d*%d", dam, attacks);
#endif

			if (cast)
			{
				project(0, rad, py, px, dam, GF_WATER, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				project(0, rad, py, px, dam, GF_POIS, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				project(0, rad, py, px, dam, GF_CONFUSION, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				if (disenchant) project(0, rad, py, px, dam, GF_DISENCHANT, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				if (drain) project(0, rad, py, px, dam, GF_OLD_DRAIN, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "����";
		if (desc) return "��Τ����꤫���Τ�����ذ�ư���롣��Υ���󤹤���ȵ�Υ�˱����ƥ��᡼��������롣";
#else
		if (name) return "Diving";
		if (desc) return "Teleport to water spot from water spot.";
#endif
    
		{
			int range = (prace_is_(RACE_MERMAID)) ? mlev + 20 : mlev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
				if (!aqua_diving(mlev)) return NULL;
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�ե�����";
		if (desc) return "���ꤷ��������������ϰ���Υ�󥹥�����̵���̤��䵤����򷫤��֤���";
#else
		if (name) return "Fenrer";
		if (desc) return "";
#endif
    
		{
			int base = mlev;
			int attacks = 3;
			if (pstat >= (18 + 100)) attacks++;
			if (pstat >= (18 + 150)) attacks++;
			if (pstat >= (18 + 180)) attacks++;
			if (pstat >= (18 + 200)) attacks++;
			if (pstat >= (18 + 220)) attacks++;

			if (info) return info_call_the_elemental(base, base, attacks);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_format("����ե�����򾤴�������");
#else
				msg_format("You called the Fenrer.");
#endif
				cast_call_the_elemental(GF_COLD, dir, base, 1, base, 3, A_INT);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�ޡ������쥤��";
		if (desc) return "��ʬ���濴�Ȥ��������ϰϤ����Ϥ������˲��������������ǡ�ۯ۰�����𤫤����������뱫��ߤ餻�롣";
#else
		if (name) return "Mercy Rain";
		if (desc) return "";
#endif
    
		{
			int base = 500;

			if (info) return info_heal(0, 0, base);

			if (cast)
			{
				hp_player(base);
				project(0, 3, py, px, base, GF_OLD_HEAL, PROJECT_KILL | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_NONE);
				set_poisoned(0);
				set_confused(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�繿��";
		if (desc) return "���ߤγ���ƹ������롣�ƹ������줿���Ͽ忻���ˤʤ롣1�ٻȤ��Ȥ��Ф餯�δ֤ϻȤ��ʤ���";
#else
		if (name) return "Water flood";
		if (desc) return "Recreates current dungeon level by water flood.";
#endif
    
		{
			if (cast)
			{
				if (!alter_with_flood()) return NULL;
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�֥롼�����ѥ����";
		if (desc) return "���ϤǤ��������ʿ�ε塢�������������ϵۼ��ε�ǹ��⤹�롣���Ϥ���ĥ�󥹥��������MP��ۼ����롣";
#else
		if (name) return "Blue spiral";
		if (desc) return "Fires a ball of water.";
#endif
    
		{
			int dice = 10;
			int sides = mlev;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_WATER, dir, damroll(dice, sides), 0, FALSE);
				fire_ball(GF_DRAIN_MANA, dir, damroll(2, mlev), 0, FALSE);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�������쥯������";
		if (desc) return "�볦����ϰϤ��Ф��ƶ˴������Ԥ�����ʬ����᡼��������롣����ˡ�ŷ�������ˤ��롣�ؼ�����Ѥ���ԤϿͿ��򼺤���";
#else
		if (name) return "Ice Requiem";
		if (desc) return "Attack by ice in sight.";
#endif
    
		{
			int base = mlev * 8;
			int sides = mlev * 5;

			if (info) return info_multi_damage_dice_base(base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_ICE, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "�������쥯������μ�ʸ�δ���ź��");
#else
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "the strain of casting Ice Requiem");
#endif

				set_weather(15, 0, 15);
				change_chaos_frame(ETHNICITY_WALSTANIAN, -1);
				change_chaos_frame(ETHNICITY_GARGASTAN, -1);
				change_chaos_frame(ETHNICITY_BACRUM, -1);
			}
		}
		break;


	}

	return "";
}


static cptr do_earth_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_EARTH]/10;
	int pstat = p_ptr->stat_use[A_INT];

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "���������֥�����";
		if (desc) return "���������ʻ��ε�����ġ����ޤˤ��Υ�󥹥��������㤵�����ɲø��̤�ȯ�����롣";
#else
		if (name) return "Ache of Xorn";
		if (desc) return "Fires a ball of acid.";
#endif
    
		{
			int dice = 3 + ((mlev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ACID, dir, damroll(dice, sides), 0, FALSE);
				if (one_in_(3)) fire_ball(GF_STASIS, dir, 50 + mlev, 0, FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�ǥեС���";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τ��ϤΥ�����Ȥ򶯤ᡢ���Υ�����Ȥ���롣";
#else
		if (name) return "Deft Bertha";
		if (desc) return "Increase EARTH, decrease WIND nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = 2;
			int rad = (mlev / 10) + 1;

			if (pstat >= (18 + 100)) amount++;
			if (pstat >= (18 + 150)) amount++;
			if (pstat >= (18 + 200)) amount++;


			if (info) return info_radius(rad);

			if (cast)
			{
				inc_area_elem(0, ELEM_EARTH, amount, rad, TRUE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�����åɡ��ܥ��";
		if (desc) return "���Υܥ�Ȥ����ġ�";
#else
		if (name) return "Acid bolt";
		if (desc) return "Fires a bolt of acid.";
#endif
    
		{
			int dice = 8 + (mlev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ACID, dir, damroll(dice, sides));
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "������¤";
		if (desc) return "���Ϥ��ڤ���Ф���";
#else
		if (name) return "Forest Creation";
		if (desc) return "Creates trees in all adjacent squares.";
#endif
    
		{
			if (cast)
			{
				tree_creation();
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�����åɥ��饦��";
		if (desc) return "���ꤷ��������������ϰϤ˻������Ԥ���";
#else
		if (name) return "Acid Cloud";
		if (desc) return "Fires a ball of acid.";
#endif
    
		{
			int sides = mlev;
			int base = 10 + mlev * 2;
			int rad = (pstat >= (18 + 150)) ? 3 : 2;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ACID, dir, base + randint1(sides), rad, TRUE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�����ץ�������";
		if (desc) return "1�ޥ����ɤ��󥹥��������ӱۤ��롣�������������ǤϤʤ����ϲ��ⵯ���ʤ���";
#else
		if (name) return "Jump Wall";
		if (desc) return "";
#endif
    
		{
			if (cast)
			{
				if (!jump_wall()) return NULL;
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "ȩ�в�";
		if (desc) return "������֡��в��ؤ����������ơ�AC��徺�����롣";
#else
		if (name) return "Stone Skin";
		if (desc) return "Gives bonus to AC for a while.";
#endif
    
		{
			int sides = mlev / 2;
			int base = mlev / 2;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_shield(randint1(sides)+base, FALSE);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "��ε��";
		if (desc) return "���Υӡ��ࡢ���ҤΥӡ�������ġ���ǽ���⤤����̿�ϵۼ��Υӡ�����դ���Ϳ�������᡼���������Ϥ��������롣";
#else
		if (name) return "Earth-dragon spirits";
		if (desc) return "Fires a beam of acid and shard.";
#endif
    
		{
			int dice1 = 10;
			int sides1 = mlev / 3;
			int dice2 = 2;
			int sides2 = mlev;
			bool drain = (pstat >= (18 + 200)) ? TRUE : FALSE;
#ifdef JP
			static const char s_dam[] = "»��:";
#else
			static const char s_dam[] = "dam ";
#endif

			if (info)
			{
				if (drain) return format("%s%dd%d*2+%dd%d", s_dam, dice1, sides1, dice2, sides2);
				else return format("%s%dd%d*2", s_dam, dice1, sides1);
			}

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_ACID, dir, damroll(dice1, sides1));
				fire_beam(GF_SHARDS, dir, damroll(dice1, sides1));
				if (drain) fire_beam(GF_NEW_DRAIN, dir, damroll(dice2, sides2));
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "���Ϥ�ܸ";
		if (desc) return "������֡���ʬ�ʳ��ΰջ֤ǥƥ�ݡ��Ȥ��ʤ��ʤ롣����ˡ���ͷ��̵��������롣";
#else
		if (name) return "Earth spike";
		if (desc) return "";
#endif
    
		{
			int sides = 6;
			int base = 6;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_earth_spike(base + randint1(sides), FALSE);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�Ρ���";
		if (desc) return "���ꤷ��������������ϰ���Υ�󥹥�����̵���̤˻�����򷫤��֤���";
#else
		if (name) return "Gnome";
		if (desc) return "";
#endif
    
		{
			int base = mlev;
			int attacks = 3;
			if (pstat >= (18 + 100)) attacks++;
			if (pstat >= (18 + 150)) attacks++;
			if (pstat >= (18 + 180)) attacks++;
			if (pstat >= (18 + 200)) attacks++;
			if (pstat >= (18 + 220)) attacks++;

			if (info) return info_call_the_elemental(base, base, attacks);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_format("����Ρ���򾤴�������");
#else
				msg_format("You called the Gnome.");
#endif
				cast_call_the_elemental(GF_ACID, dir, base, 1, base, 3, A_INT);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "����å��ץ쥹";
		if (desc) return "���ꤷ��������������ϰϤ���й����Ԥ���";
#else
		if (name) return "Crack Breath";
		if (desc) return "Make a meteor ball.";
#endif
    
		{
			int sides = mlev * 2;
			int base = 100 + mlev;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_METEOR, dir, base + randint1(sides), 2, TRUE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�ڥȥ�֥쥹";
		if (desc) return "�в��Υ֥쥹���Ǥ���";
#else
		if (name) return "Petro breath";
		if (desc) return "Breath of stoning.";
#endif
    
		{
			int base = MIN(p_ptr->chp / 2, 350);

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;

				fire_ball(GF_STONE, dir, base, -2, FALSE);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "��������������";
		if (desc) return "�볦����ϰϤ��Ф������ҹ����Ԥ�����ʬ����᡼��������롣�ؼ�����Ѥ���ԤϿͿ��򼺤���";
#else
		if (name) return "Earthquake";
		if (desc) return "Attakcs by shards in sight.";
#endif
    
		{
			int base = mlev * 8;
			int sides = mlev * 5;

			if (info) return info_multi_damage_dice_base(base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_SHARDS, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "���������������μ�ʸ�δ���ź��");
#else
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "the strain of casting Earthquake");
#endif

				change_chaos_frame(ETHNICITY_WALSTANIAN, -1);
				change_chaos_frame(ETHNICITY_GARGASTAN, -1);
				change_chaos_frame(ETHNICITY_BACRUM, -1);
			}
		}
		break;


	}

	return "";
}


static cptr do_wind_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_WIND]/10;
	int pstat = p_ptr->stat_use[A_INT];

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ǥåɥ���å�";
		if (desc) return "�첻�Υӡ�������ġ�";
#else
		if (name) return "Deadshot";
		if (desc) return "Fires a bolt of sound.";
#endif
    
		{
			int dice = 3 + ((mlev - 1) / 5);
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_SOUND, dir, damroll(dice, sides));
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�ǥեϡ��ͥ�";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τ����Υ�����Ȥ򶯤ᡢ�ϤΥ�����Ȥ���롣";
#else
		if (name) return "Deft Hahnela";
		if (desc) return "Increase WIND, decrease EARTH nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = 2;
			int rad = (mlev / 10) + 1;

			if (pstat >= (18 + 100)) amount++;
			if (pstat >= (18 + 150)) amount++;
			if (pstat >= (18 + 200)) amount++;


			if (info) return info_radius(rad);

			if (cast)
			{
				inc_area_elem(0, ELEM_WIND, amount, rad, TRUE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "���θ��";
		if (desc) return "������֡��ͷ����򤹤�ǽ�Ϥ����롣��ǽ���⤤����ˡ�Υܥ�Ȥ���åȤ���򤹤롣";
#else
		if (name) return "Wind's gurad";
		if (desc) return "Protect from shoot for a while.";
#endif
    
		{
			int sides = mlev;
			int base = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_wind_guard(randint1(sides)+base, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "����������ܥ��";
		if (desc) return "�ŷ�Υܥ�Ȥ����ġ�";
#else
		if (name) return "Thunder bolt";
		if (desc) return "Fires a bolt of lightning.";
#endif
    
		{
			int dice = 8 + (mlev - 5) / 4;
			int sides = 8;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_bolt(GF_ELEC, dir, damroll(dice, sides));
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "��������ե쥢";
		if (desc) return "���ꤷ��������������ϰϤ��ŷ⹶���Ԥ���";
#else
		if (name) return "Thunder Flare";
		if (desc) return "Fires a ball of lightning.";
#endif
    
		{
			int sides = mlev;
			int base = 10 + mlev * 2;
			int rad = (pstat >= (18 + 150)) ? 3 : 2;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_ELEC, dir, base + randint1(sides), rad, TRUE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�����å��ࡼ��";
		if (desc) return "������֡���®���롣";
#else
		if (name) return "Quick Move";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = mlev;
			int sides = 25 + mlev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�����륹�ȡ���";
		if (desc) return "ŷ���򰭲������롣";
#else
		if (name) return "Call storm";
		if (desc) return "Weather to bad.";
#endif
    
		{
			int power = 2;

			if (pstat >= (18 + 150)) power++;
			if (pstat >= (18 + 200)) power++;

			if (cast)
			{
				msg_print("ŷ�����Ӥ�Ƥ���...");

				set_weather(power, power, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�ȥ�͡���";
		if (desc) return "��ʬ���濴�Ȥ���*��*�ε塢�첻�ε塢���Ϥε��ȯ�������롣";
#else
		if (name) return "Tornado";
		if (desc) return "";
#endif
    
		{
			int rad = mlev / 10;
			int dam1 = 40 + mlev / 2;
			int dam2 = 20 + mlev / 2;

#ifdef JP
			if (info) return format("»��:%d+%d*2", dam1, dam2);
#else
			if (info) return format("dam %d+%d*2", dam1, dam2);
#endif


			if (cast)
			{
				project(0, rad, py, px, dam1, GF_PURE_WIND, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, MODIFY_ELEM_MODE_MAGIC);
				project(0, rad, py, px, dam2, GF_SOUND, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, MODIFY_ELEM_MODE_MAGIC);
				project(0, rad, py, px, dam2, GF_GRAVITY, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, MODIFY_ELEM_MODE_MAGIC);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "��������С���";
		if (desc) return "���ꤷ��������������ϰ���Υ�󥹥�����̵���̤��ŷ⹶��򷫤��֤���";
#else
		if (name) return "Thunderbird";
		if (desc) return "";
#endif
    
		{
			int base = mlev;
			int attacks = 3;
			if (pstat >= (18 + 100)) attacks++;
			if (pstat >= (18 + 150)) attacks++;
			if (pstat >= (18 + 180)) attacks++;
			if (pstat >= (18 + 200)) attacks++;
			if (pstat >= (18 + 220)) attacks++;

			if (info) return info_call_the_elemental(base, base, attacks);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_format("���������С��ɤ򾤴�������");
#else
				msg_format("You called the Thunderbird.");
#endif
				cast_call_the_elemental(GF_ELEC, dir, base, 1, base, 3, A_INT);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�ϡ��ͥ������";
		if (desc) return "�첻�Υ֥쥹���Ǥ���";
#else
		if (name) return "Howl of Hahnela";
		if (desc) return "Breath of sound.";
#endif
    
		{
			int base = MIN(p_ptr->chp / 2, 350);

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;

				fire_ball(GF_SOUND, dir, base, -2, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "��������";
		if (desc) return "û��Υ��λ��ꤷ�����˥ƥ�ݡ��Ȥ��롣";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif
    
		{
			int range = mlev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�������⤬����������Ū�Ϥ�����ǲ�������");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(mlev)) return NULL;
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�����ꥢ�륯�饤";
		if (desc) return "�볦����ϰϤ��Ф��ƹ첻�����Ԥ�����ʬ����᡼��������롣����ˡ�ŷ����˽���ˤ��롣�ؼ�����Ѥ���ԤϿͿ��򼺤���";
#else
		if (name) return "Aerial Cry";
		if (desc) return "Attacks by sound.";
#endif
    
		{
			int base = mlev * 8;
			int sides = mlev * 5;

			if (info) return info_multi_damage_dice_base(base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_SHARDS, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "�����ꥢ�륯�饤�μ�ʸ�δ���ź��");
#else
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "the strain of casting Aerial Cry");
#endif

				set_weather(0, 15, 0);
				change_chaos_frame(ETHNICITY_WALSTANIAN, -1);
				change_chaos_frame(ETHNICITY_GARGASTAN, -1);
				change_chaos_frame(ETHNICITY_BACRUM, -1);
			}
		}
		break;


	}

	return "";
}


static cptr do_holy_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_HOLY]/10;
	int pstat = p_ptr->stat_use[A_WIS];

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ٰ�¸�ߴ���";
		if (desc) return "�᤯�μٰ��ʥ�󥹥������Τ��롣";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "��ʡ";
		if (desc) return "������֡�̿��Ψ��AC�˥ܡ��ʥ������롣";
#else
		if (name) return "Bless";
		if (desc) return "Gives bonus to hit and AC for a few turns.";
#endif
    
		{
			int base = 12;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_blessed(randint1(base) + base, FALSE);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "���ξ���";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τ�ʵפ����뤯���롣";
#else
		if (name) return "Call Light";
		if (desc) return "Lights up nearby area and the inside of a room permanently.";
#endif
    
		{
			int dice = 2;
			int sides = mlev / 2;
			int rad = mlev / 10 + 1;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				lite_area(damroll(dice, sides), rad);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�ҡ����";
		if (desc) return "���Ϥ������ٲ��������������ۯ۰���֤��������롣";
#else
		if (name) return "Cure Critical Wounds";
		if (desc) return "Heals cut, stun and HP.";
#endif
    
		{
			int dice = 8;
			int sides = 10;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				hp_player(damroll(dice, sides));
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "� & �����ⴶ��";
		if (desc) return "�᤯�����Ƥ�櫤���ȳ��ʤ��Τ��롣";
#else
		if (name) return "Detect Doors & Traps";
		if (desc) return "Detects traps, doors, and stairs in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
				detect_stairs(rad);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "��ʢ��­";
		if (desc) return "��ʢ�ˤ��롣";
#else
		if (name) return "Satisfy Hunger";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			if (cast)
			{
				set_food(PY_FOOD_MAX - 1);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���奢�ݥ�����";
		if (desc) return "������Ǥ��������";
#else
		if (name) return "Cure Poison";
		if (desc) return "Cure poison status.";
#endif
    
		{
			if (cast)
			{
				set_poisoned(0);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���奢������";
		if (desc) return "�����ƥ�ˤ����ä��夤�����������롣��٥뤬�夬��ȶ��Ϥʼ����������롣";
#else
		if (name) return "Remove Curse";
		if (desc) return "Removes normal curses from equipped items.";
#endif

		{
			if (cast)
			{
				if (mlev < 40)
				{
					if (remove_curse())
					{
#ifdef JP
						msg_print("ï���˸�����Ƥ���褦�ʵ������롣");
#else
						msg_print("You feel as if someone is watching over you.");
#endif
					}
				}
				else
				{
					if (remove_all_curse())
					{
#ifdef JP
						msg_print("ï���˸�����Ƥ���褦�ʵ������롣");
#else
						msg_print("You feel as if someone is watching over you.");
#endif
					}
				}
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "ͽ��";
		if (desc) return "���դ��Ϸ����Τ����᤯��櫡��⡢���ʡ����Ƥθ������󥹥������Τ��롣";
#else
		if (name) return "Divination";
		if (desc) return "Maps nearby area. Detects all monsters, traps, doors and stairs.";
#endif
    
		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

			if (info) return info_radius(MAX(rad1, rad2));

			if (cast)
			{
				map_area(rad1);
				detect_traps(rad2, TRUE);
				detect_doors(rad2);
				detect_stairs(rad2);
				detect_monsters_normal(rad2);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "������";
		if (desc) return "̵�������ڤ��������ʥ��ͥ륮���Υӡ�������ġ�";
#else
		if (name) return "Goddly spear";
		if (desc) return "Fires a bolt of pure energy.";
#endif
    
		{
			int sides = mlev * 2;
			int base = 100;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;

				fire_beam(GF_GODLY_SPEAR, dir, base + randint1(sides));
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�볦�����";
		if (desc) return "��ʬ�Τ��뾲�ξ�ˡ���󥹥������̤�ȴ�����꾤�����줿�ꤹ�뤳�Ȥ��Ǥ��ʤ��ʤ�롼���������";
#else
		if (name) return "Glyph of Warding";
		if (desc) return "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.";
#endif
    
		{
			if (cast)
			{
				warding_glyph();
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�ҡ���󥰥�����";
		if (desc) return "�ˤ�ƶ��Ϥʲ�����ʸ�ǡ������ۯ۰���֤��������롣";
#else
		if (name) return "Healing all";
		if (desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif
    
		{
			int heal = 1000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "���ꥢ���";
		if (desc) return "�ǡ��������������������β������ݡ����С������˿ʹԤ����в���ľ����";
#else
		if (name) return "Clearance";
		if (desc) return "heals poison, fear, stun and cut status.";
#endif
    
		{
			if (cast)
			{
				set_poisoned(0);
				set_cut(0);
				set_afraid(0);
				set_image(0);
				set_shero(0, TRUE);
				set_stoning(0);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�мٰ��볦";
		if (desc) return "�ٰ��ʥ�󥹥����ι�����ɤ��Хꥢ��ĥ�롣";
#else
		if (name) return "Protection from Evil";
		if (desc) return "Gives aura which protect you from evil monster's physical attack.";
#endif
    
		{
			int base = 3 * mlev;
			int sides = 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_protevil(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�ҡ���󥰥ץ饹";
		if (desc) return "��ʬ�ڤӼ�ʬ�μ��ϤΥ�󥹥��������Ϥ������˲������������ۯ۰���֤����������롣";
#else
		if (name) return "Healing plus";
		if (desc) return "Much powerful healing magic, and heals cut and stun completely.";
#endif
    
		{
			int heal = 300;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				project(0, 2, py, px, heal, GF_OLD_HEAL, PROJECT_KILL | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_NONE);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "������";
		if (desc) return "������֡������ŷ⡢�ꡢ�䵤���Ǥ��Ф������������롣�����ˤ�����������Ѥ��롣";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�۶��θ���";
		if (desc) return "�볦������ƤΥ�󥹥�����̥λ���롣�񹳤�����̵����";
#else
		if (name) return "Day of the Dove";
		if (desc) return "Attempts to charm all monsters in sight.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				charm_monsters(power);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "������";
		if (desc) return "���٤ƤΥ��ơ������ȷи��ͤ�������롣";
#else
		if (name) return "Restoration";
		if (desc) return "Restores all stats and experience.";
#endif
    
		{
			if (cast)
			{
				do_res_stat(A_STR);
				do_res_stat(A_INT);
				do_res_stat(A_WIS);
				do_res_stat(A_DEX);
				do_res_stat(A_CON);
				do_res_stat(A_CHR);
				restore_level();
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�����ʤ��";
		if (desc) return "���γ����Τ�ʵפ˾Ȥ餷�����󥸥���⤹�٤ƤΥ����ƥࡢ��󥹥������Τ��롣";
#else
		if (name) return "Clairvoyance";
		if (desc) return "Maps and lights whole dungeon level. Knows all objects location. And gives telepathy for a while.";
#endif
    
		{
			if (cast)
			{
				wiz_lite(FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�����˥��ե�����";
		if (desc) return "���ꤷ��������������ϰ���Υ�󥹥�����̵���̤����ʤ빶��򷫤��֤���";
#else
		if (name) return "Ignis Fatuus";
		if (desc) return "";
#endif
    
		{
			int base = mlev;
			int attacks = 1;
			if (pstat >= (18 + 100)) attacks++;
			if (pstat >= (18 + 150)) attacks++;
			if (pstat >= (18 + 180)) attacks++;
			if (pstat >= (18 + 200)) attacks++;
			if (pstat >= (18 + 220)) attacks++;

			if (info) return info_call_the_elemental(base, base, attacks);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_format("������˥��ե������򾤴�������");
#else
				msg_format("You called the Ignis Fatuus.");
#endif
				cast_call_the_elemental(GF_HOLY_FIRE, dir, base, 1, base, 1, A_WIS);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "ŷ�Ⱦ���";
		if (desc) return "ŷ�Ȥ�1�ξ������롣";
#else
		if (name) return "Summon Angel";
		if (desc) return "Summons an angel.";
#endif
    
		{
			if (cast)
			{
				bool pet = !one_in_(3);
				u32b mode = 0L;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_NO_PET | PM_IGNORE_AMGRID);
				if (!(pet && (mlev < 50))) mode |= PM_ALLOW_GROUP;

				if (summon_specific((pet ? -1 : 0), py, px, (mlev * 3) / 2, SUMMON_ANGEL, mode))
				{
					if (pet)
					{
#ifdef JP
						msg_print("�֤��ѤǤ������ޤ�����������͡�");
#else
						msg_print("'What is thy bidding... Master?'");
#endif
					}
					else
					{
#ifdef JP
						msg_print("�ֲ����β��ͤˤ��餺�� ���ԼԤ衢��������衪��");
#else
						msg_print("Mortal! Repent of thy impiousness.");
#endif
					}
				}
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�����볦";
		if (desc) return "��ʬ�Τ��뾲�ȼ���8�ޥ��ξ��ξ�ˡ���󥹥������̤�ȴ�����꾤�����줿�ꤹ�뤳�Ȥ��Ǥ��ʤ��ʤ�롼���������";
#else
		if (name) return "Warding True";
		if (desc) return "Creates glyphs in all adjacent squares and under you.";
#endif
    
		{
			int rad = 1;

			if (info) return info_radius(rad);

			if (cast)
			{
				warding_glyph();
				glyph_creation();
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�ѡ��ե����ȥҡ���";
		if (desc) return "�Ƕ��μ�������ˡ�ǡ�������Ǥ�������������β������ݡ����С������˿ʹԤ����в���ľ����";
#else
		if (name) return "Healing True";
		if (desc) return "The greatest healing magic. Heals all HP, cut and stun.";
#endif
    
		{
			int heal = 5000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				hp_player(heal);
				set_poisoned(0);
				set_cut(0);
				set_afraid(0);
				set_shero(0, TRUE);
				set_image(0);
				set_stoning(0);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�ꥶ�쥯�����";
		if (desc) return "������֡���˴���Ƥⴰ������Ǥ���ǽ�Ϥ����롣��MP����񤹤롣";
#else
		if (name) return "Resurrection";
		if (desc) return "Gives resurrection for a while.";
#endif
    
		{
			int base = 6;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_resurrection(base + randint1(base), FALSE);
			}
		}
		break;


	}

	return "";
}


static cptr do_death_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_DEATH]/10;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "����ǥåɽ�°";
		if (desc) return "����ǥå�1�Τ�̥λ���롣�񹳤�����̵����";
#else
		if (name) return "Enslave Undead";
		if (desc) return "Attempts to charm an undead monster.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				control_one_undead(dir, power);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�Ź�����";
		if (desc) return "���ܡ��ǡ�����ۯ۰���֡�������������������Ф�ľ����";
#else
		if (name) return "Dark Cure";
		if (desc) return "heals poison, fear, stun and cut status.";
#endif
    
		{
			if (cast)
			{
				set_blind(0);
				set_poisoned(0);
				set_confused(0);
				set_stun(0);
				set_cut(0);
				set_image(0);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "������Ǯ��";
		if (desc) return "���Τ��饨�ͥ륮����ۤ���ꡢ���Ϥ�������롣�ۤ����줿���Τϳ��ˤʤ롣";
#else
		if (name) return "Drain corpse";
		if (desc) return "drain energy from corpse.";
#endif
    
		{
			if (cast)
			{
				if (!get_energy_from_corpse()) return NULL;
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�Ǥα�";
		if (desc) return "�ٰ����Ϥ���ĵ�����ġ����ɤʥ�󥹥������Ф����礭�ʥ��᡼����Ϳ���롣";
#else
		if (name) return "Dark fire";
		if (desc) return "Fires a ball of hell fire.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int base = mlev + mlev / 2;
			int rad = (mlev < 30) ? 2 : 3;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, damroll(dice, sides) + base, rad, FALSE);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "���㡼�����ڥ�";
		if (desc) return "������֡���ǽ�ȸ�����徺������MP�β����Ϥ���������롣";
#else
		if (name) return "Charge Spell";
		if (desc) return "Gain Int and Wis for a while.";
#endif
    
		{
			int base = mlev;
			int sides = mlev + 25;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_chargespell(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�ʥ��ȥᥢ";
		if (desc) return "���������ʰŹ��ε�����ġ����Ф���̲��������������";
#else
		if (name) return "Nightmare";
		if (desc) return "Fire a ball of dark.";
#endif
    
		{
			int dice = 4 + (mlev - 10) / 5;
			int sides = 4;
			int base = 30 + mlev;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;

				fire_ball(GF_DARK, dir, base + damroll(dice, sides), 0, FALSE);
				if (!one_in_(3)) fire_ball_hide(GF_OLD_SLEEP, dir, mlev + 50, 0, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�����Ķ�";
		if (desc) return "�볦������ƤΥ�󥹥������Ф��ƽ��Ϲ����Ԥ���";
#else
		if (name) return "Distort";
		if (desc) return "Attacks by gravity in sight.";
#endif
    
		{
			int dice = mlev / 5 + 10;
			int sides = 10;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				project_hack(GF_GRAVITY, damroll(dice, sides));
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "��󥹥�������";
		if (desc) return "��󥹥���1�Τ�ä���롣�и��ͤ䥢���ƥ�ϼ������ʤ����񹳤�����̵����";
#else
		if (name) return "Genocide One";
		if (desc) return "Attempts to vanish a monster.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball_hide(GF_GENOCIDE, dir, power, 0, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "��Ծ���";
		if (desc) return "����ǥåɤ򾤴����롣��٥뤬�夬��Ⱦ�饢��ǥåɤ򾤴����롣";
#else
		if (name) return "Summon undead";
		if (desc) return "Summons an undead monster.";
#endif
    
		{
			if (cast)
			{
				int type;
				bool pet = one_in_(3);
				u32b mode = 0L;

				type = (mlev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);

				if (!pet || (pet && (mlev > 24) && one_in_(3)))
					mode |= PM_ALLOW_GROUP;

				if (pet) mode |= PM_FORCE_PET;
				else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET | PM_IGNORE_AMGRID);

				if (summon_specific((pet ? -1 : 0), py, px, (mlev * 3) / 2, type, mode))
				{
#ifdef JP
					msg_print("�䤿���������ʤ��μ���˿᤭�Ϥ᤿����������Խ��򱿤�Ǥ���...");
#else
					msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


					if (pet)
					{
#ifdef JP
						msg_print("�Ť��λऻ��Զ������ʤ��˻Ť��뤿���ڤ���ᴤä���");
#else
						msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif
					}
					else
					{
#ifdef JP
						msg_print("��Ԥ�ᴤä���̲���˸���뤢�ʤ���ȳ���뤿��ˡ�");
#else
						msg_print("'The dead arise... to punish you for disturbing them!'");
#endif
					}
				}
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "��ɥ��֥ڥ���";
		if (desc) return "��󥹥���1�Τ˸��ߤμ�ʬ�μ����Ƥ�����᡼����Ʊ�����᡼����Ϳ���롣";
#else
		if (name) return "Word of Pain";
		if (desc) return "";
#endif
    
		{
			int dam = MAX(p_ptr->mhp - p_ptr->chp, 0);

			if (info) info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return FALSE;

				fire_ball(GF_WORD_OF_PAIN, dir, 0, 0, FALSE);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�˲��θ���";
		if (desc) return "���դΥ����ƥࡢ��󥹥������Ϸ����˲����롣";
#else
		if (name) return "Word of Destruction";
		if (desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 13;
			int sides = 5;

			if (cast)
			{
				destroy_area(py, px, base + randint0(sides));
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "��������";
		if (desc) return "���Ϥλ��Τ���������֤���";
#else
		if (name) return "Animate dead";
		if (desc) return "Resurrects nearby corpse and skeletons. And makes these your pets.";
#endif
    
		{
			if (cast)
			{
				animate_dead(0, py, px);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�ڥå�����";
		if (desc) return "���ƤΥڥåȤ���Ū�����ˤ����롣";
#else
		if (name) return "Explode Pets";
		if (desc) return "Makes all pets explode.";
#endif
    
		{
			if (cast)
			{
				discharge_minion();
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�۷�ο�";
		if (desc) return "���˵۷��°����Ĥ��롣";
#else
		if (name) return "Vampiric Branding";
		if (desc) return "Makes current weapon Vampiric.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(EGO_VAMPIRIC);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "���Ԥ�����";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τμ�ʬ�Υ�����Ȱʳ��Υ�����Ȥ���롣";
#else
		if (name) return "Corrupted area";
		if (desc) return "Decrease all but your element nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = -4;
			int rad = -((mlev / 10) - 1);
			int pstat = p_ptr->stat_use[mp_ptr->spell_stat];

			if (pstat >= (18 + 100)) amount -= 2;
			if (pstat >= (18 + 150)) amount -= 2;
			if (pstat >= (18 + 200)) amount -= 2;

			if (info) return info_power(amount);

			if (cast)
			{
				int i;

				for (i = MIN_ELEM; i < ELEM_NUM; i++)
				{
					if (i == get_cur_pelem()) continue;
					inc_area_elem(0, i, amount, rad, FALSE);
				}
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "�饤�եե�����";
		if (desc) return "��󥹥���1�Τ�����̿�Ϥ�ۤ��Ȥ롣�ۤ��Ȥä���̿�Ϥˤ�ä����Ϥ��������롣";
#else
		if (name) return "Vampirism True";
		if (desc) return "Fires 3 bolts. Each of the bolts absorbs some HP from a monster and gives them to you.";
#endif
    
		{
			int dam = 100;

#ifdef JP
			if (info) return format("»��:3*%d", dam);
#else
			if (info) return format("dam 3*%d", dam);
#endif

			if (cast)
			{
				int i;

				if (!get_aim_dir(&dir)) return NULL;

				for (i = 0; i < 3; i++)
					fire_ball(GF_NEW_DRAIN, dir, dam, 0, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "��������";
		if (desc) return "���ꤷ��������������ϰ���Υ�󥹥�����̵���̤˼ٰ��ʹ���򷫤��֤���";
#else
		if (name) return "Dark lore";
		if (desc) return "";
#endif
    
		{
			int base = mlev;
			int attacks = 1;
			int pstat = p_ptr->stat_use[A_WIS];

			if (pstat >= (18 + 100)) attacks++;
			if (pstat >= (18 + 150)) attacks++;
			if (pstat >= (18 + 180)) attacks++;
			if (pstat >= (18 + 200)) attacks++;
			if (pstat >= (18 + 220)) attacks++;

			if (info) return info_call_the_elemental(base, base, attacks);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

#ifdef JP
				msg_format("����ե���ȥ�򾤴�������");
#else
				msg_format("You called the Phantom.");
#endif
				cast_call_the_elemental(GF_HELL_FIRE, dir, base, 1, base, 1, A_WIS);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "̽���ؤξ���";
		if (desc) return "�볦������ƤΥ�󥹥������Ф��Ƽٰ��ʹ����Ԥ���̲�餻�롣���ɤʥ�󥹥����ˤ��礭�ʥ��᡼����Ϳ���롣";
#else
		if (name) return "Invite to Hell";
		if (desc) return "";
#endif
    
		{
			int dam = mlev * 4;
			int power = mlev + 50;

			if (info) info_multi_damage(dam);

			if (cast)
			{
				project_hack_living(GF_HELL_FIRE, dam);
				project_hack_living(GF_OLD_SLEEP, power);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "����";
		if (desc) return "���ꤷ��ʸ���Υ�󥹥����򸽺ߤγ�����ä���롣�񹳤�����̵����";
#else
		if (name) return "Genocide";
		if (desc) return "Eliminates an entire class of monster, exhausting you.  Powerful or unique monsters may resist.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				symbol_genocide(power, TRUE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�Ź�����";
		if (desc) return "����ʰŹ��ε�����ġ�";
#else
		if (name) return "Darkness Storm";
		if (desc) return "Fires a huge ball of darkness.";
#endif
    
		{
			int dam = 100 + mlev * 4;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_DARK, dir, dam, rad, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�Ǥ��˿���";
		if (desc) return "������֡������Ϥ�̿��Ψ���������졢��郎��̿����¸�ߵڤ����ɤ�¸�ߤ˶����ʤ롣�����Ӥ����򳰤���̵����";
#else
		if (name) return "Dark ragnarok";
		if (desc) return "";
#endif
    
		{
			int base = mlev /2;

			if (info) info_duration(base, base);

			if (cast)
			{
				if (!buki_motteruka(INVEN_RARM))
				{
#ifdef JP
					msg_format("�����Ӥ���������ʤ��ȰǤ��˿����ϻȤ��ʤ���");
#else
					msg_format("You cannot use evil weapon with no main weapon.");
#endif
					return NULL;
				}

				set_evil_weapon(randint1(base) + base, FALSE, INVEN_RARM, FALSE);
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "��������";
		if (desc) return "��ʬ�μ��Ϥˤ����󥹥����򸽺ߤγ�����ä���롣�񹳤�����̵����";
#else
		if (name) return "Mass Genocide";
		if (desc) return "Eliminates all nearby monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				mass_genocide(power, TRUE);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�Ϲ��ι��";
		if (desc) return "���˶��Ϥǵ���ʼٰ��ʵ�����ġ����ɤʥ�󥹥����ˤ��礭�ʥ��᡼����Ϳ���롣";
#else
		if (name) return "Hellfire";
		if (desc) return "Fires a powerful ball of evil power. Hurts good monsters greatly.";
#endif
    
		{
			int dam = 666;
			int rad = 3;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HELL_FIRE, dir, dam, rad, FALSE);
#ifdef JP
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "�Ϲ��ι�Фμ�ʸ�򾧤�����ϫ");
#else
				take_hit(DAMAGE_USELIFE, 20 + randint1(30), "the strain of casting Hellfire");
#endif
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�ѥ������";
		if (desc) return "����ߤ�롣��MP����񤹤롣";
#else
		if (name) return "Paradigm";
		if (desc) return "Stops time. Consumes all of your SP.";
#endif
    
		{
			if (cast)
			{
				if (!cast_stop_the_time()) return NULL;
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�ǥåɥ����꡼��";
		if (desc) return "�볦����ϰϤ��Ф��ưŹ������Ԥ�����ʬ����᡼��������롣�ؼ�����Ѥ���ԤϿͿ��򼺤���";
#else
		if (name) return "Dead Scream";
		if (desc) return "";
#endif
    
		{
			int base = mlev * 8;
			int sides = mlev * 5;

			if (info) return info_multi_damage_dice_base(base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_DARK, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "�ǥåɥ����꡼��μ�ʸ�δ���ź��");
#else
				take_hit(DAMAGE_NOESCAPE, base + randint1(sides), "the strain of casting Dead Scream");
#endif

				change_chaos_frame(ETHNICITY_WALSTANIAN, -1);
				change_chaos_frame(ETHNICITY_GARGASTAN, -1);
				change_chaos_frame(ETHNICITY_BACRUM, -1);
			}
		}
		break;


	}

	return "";
}


static bool ang_sort_comp_pet(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	monster_type *m_ptr1 = &m_list[w1];
	monster_type *m_ptr2 = &m_list[w2];
	monster_race *r_ptr1 = &r_info[m_ptr1->r_idx];
	monster_race *r_ptr2 = &r_info[m_ptr2->r_idx];

	/* Unused */
	(void)v;

	if (m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if (m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if (!m_ptr1->parent_m_idx && m_ptr2->parent_m_idx) return TRUE;
	if (!m_ptr2->parent_m_idx && m_ptr1->parent_m_idx) return FALSE;

	if ((r_ptr1->flags1 & RF1_UNIQUE) && !(r_ptr2->flags1 & RF1_UNIQUE)) return TRUE;
	if ((r_ptr2->flags1 & RF1_UNIQUE) && !(r_ptr1->flags1 & RF1_UNIQUE)) return FALSE;

	if (r_ptr1->level > r_ptr2->level) return TRUE;
	if (r_ptr2->level > r_ptr1->level) return FALSE;

	if (m_ptr1->hp > m_ptr2->hp) return TRUE;
	if (m_ptr2->hp > m_ptr1->hp) return FALSE;
	
	return w1 <= w2;
}


static cptr do_symbiotic_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_SYMBIOTIC]/10;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "��󥹥�������";
		if (desc) return "�᤯�����Ƥθ������󥹥������Τ��롣";
#else
		if (name) return "Detect Monsters";
		if (desc) return "Detects all monsters in your vicinity unless invisible.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_normal(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "ưʪ��餷";
		if (desc) return "ưʪ1�Τ�̥λ���롣�񹳤�����̵����";
#else
		if (name) return "Taim Monster";
		if (desc) return "Attempts to charm a monster.";
#endif
    
		{
			int power = mlev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				charm_monster(dir, power);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "ͧ�ͼ���";
		if (desc) return "��󥹥���1�Τ����Ϥ�������������㡢���ݡ�ۯ۰��������Ť��롣";
#else
		if (name) return "Heal Monster";
		if (desc) return "Heal a monster.";
#endif
    
		{
			int dice = 4;
			int sides = 6;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				bool result;
				bool old_target_pet = target_pet;

				result = get_aim_dir(&dir);

				target_pet = old_target_pet;

				if (!result) return NULL;

				fire_ball(GF_OLD_HEAL, dir, damroll(4, 6), 0, FALSE);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�����";
		if (desc) return "�볦������ƤΥ�󥹥�����®�����롣�񹳤�����̵����";
#else
		if (name) return "Entangle";
		if (desc) return "Attempts to slow all monsters in sight.";
#endif
    
		{
			int power = mlev;

			if (info) return info_power(power);

			if (cast)
			{
				slow_monsters(power);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "��������";
		if (desc) return "��ʬ��­���˿�������Ф���";
#else
		if (name) return "Produce Food";
		if (desc) return "Produces a Ration of Food.";
#endif
    
		{
			if (cast)
			{
				object_type forge, *q_ptr = &forge;

#ifdef JP
				msg_print("����������������");
#else
				msg_print("A food ration is produced.");
#endif

				/* Create the food ration */
				object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_RATION));

				/* Drop the object from heaven */
				drop_near(q_ptr, -1, py, px);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�ڥåȽ���";
		if (desc) return "�ڥåȤ�ƤӴ󤻤롣";
#else
		if (name) return "Call pet";
		if (desc) return "Call all pets.";
#endif
    
		{
			if (cast)
			{
				int pet_ctr, i;
				u16b *who;
				int max_pet = 0;
				u16b dummy_why;

				/* Allocate the "who" array */
				C_MAKE(who, max_m_idx, u16b);

				/* Process the monsters (backwards) */
				for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
				{
					if (is_pet(&m_list[pet_ctr]) && (p_ptr->riding != pet_ctr))
					  who[max_pet++] = pet_ctr;
				}

				/* Select the sort method */
				ang_sort_comp = ang_sort_comp_pet;
				ang_sort_swap = ang_sort_swap_hook;

				ang_sort(who, &dummy_why, max_pet);

				/* Process the monsters (backwards) */
				for (i = 0; i < max_pet; i++)
				{
					pet_ctr = who[i];
					teleport_to(pet_ctr, py, px, 2, 100, TRUE);
				}

				/* Free the "who" array */
				C_KILL(who, max_m_idx, u16b);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "��­�þ���";
		if (desc) return "1�Τλ�­��('q'��ɽ������󥹥���)�򾤴����롣";
#else
		if (name) return "Summon Animal";
		if (desc) return "Summons an animal.";
#endif
    
		{
			if (cast)
			{
				summon_kin_type = 'q';
				if (!summon_specific(-1, py, px, mlev * 2 / 3 + randint1(mlev/2), SUMMON_KIN, PM_FORCE_PET))
				{
#ifdef JP
					msg_print("ưʪ�ϸ���ʤ��ä���");
#else
					msg_print("No animals arrive.");
#endif
				}
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "ͧ������Τ餻";
		if (desc) return "�᤯�����ƤΥ�󥹥�����櫡��⡢���ʡ������������ƥࡢ�������Ϸ����Τ��롣";
#else
		if (name) return "Nature Awareness";
		if (desc) return "Maps nearby area. Detects all monsters, traps, doors and stairs.";
#endif
    
		{
			int rad1 = DETECT_RAD_MAP;
			int rad2 = DETECT_RAD_DEFAULT;

			if (info) return info_radius(MAX(rad1, rad2));

			if (cast)
			{
				map_area(rad1);
				detect_all(rad2);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "���ԡ��ɡ���󥹥���";
		if (desc) return "��󥹥���1�Τ��®�����롣";
#else
		if (name) return "Haste Monster";
		if (desc) return "Hastes a monster.";
#endif
    
		{
			int power = mlev;

			if (info) info_power(power);

			if (cast)
			{
				bool result;

				/* Temporary enable target_pet option */
				bool old_target_pet = target_pet;
				target_pet = TRUE;

				result = get_aim_dir(&dir);

				/* Restore target_pet option */
				target_pet = old_target_pet;

				if (!result) return NULL;

				speed_monster(dir, power);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "ưʪͧ��";
		if (desc) return "�볦������Ƥ�ưʪ��̥λ���롣�񹳤�����̵����";
#else
		if (name) return "Animal Friendship";
		if (desc) return "Attempts to charm all animals in sight.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				charm_animals(power);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�����˻�";
		if (desc) return "���γ������������󥹥����������Ǥ��ʤ��ʤ롣";
#else
		if (name) return "Sterilization";
		if (desc) return "Prevents any breeders on current level from breeding.";
#endif
    
		{
			if (cast)
			{
				num_repro += MAX_REPRO;
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�ϥ���ɾ���";
		if (desc) return "1���롼�פΥϥ���ɤ򾤴����롣";
#else
		if (name) return "Summon Hounds";
		if (desc) return "Summons a group of hounds.";
#endif
    
		{
			if (cast)
			{
				u32b mode = PM_ALLOW_GROUP | PM_FORCE_PET;
				if (!summon_specific(-1, py, px, mlev * 2 / 3 + randint1(mlev/2), SUMMON_HOUND, mode))
				{
#ifdef JP
					msg_print("�ϥ���ɤϸ���ʤ��ä���");
#else
					msg_print("No hounds arrive.");
#endif
				}
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "ͧ�ؤΰ�";
		if (desc) return "��ʬ�μ��դΥڥåȤ����Ϥ�����������㡢���ݡ�ۯ۰��������Ť��롣";
#else
		if (name) return "Heal Monsters";
		if (desc) return "Heal nearby monsters.";
#endif
    
		{
			int heal = 200;

			if (info) info_heal(0, 0, heal);

			if (cast)
			{
				int     i, x, y;
				u32b    flg = PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE;
				monster_type *m_ptr;

				/* Affect all (nearby) monsters */
				for (i = 1; i < m_max; i++)
				{
					m_ptr = &m_list[i];

					/* Paranoia -- Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Location */
					y = m_ptr->fy;
					x = m_ptr->fx;

					/* Require line of sight */
					if (!player_has_los_bold(y, x)) continue;

					/* Skip distant monsters */
					if (distance(py, px, y, x) > 3) continue;

					/* Player's pets only */
					if (!is_pet(m_ptr)) continue;

					/* Jump directly to the target monster */
					project(0, 0, y, x, heal, GF_OLD_HEAL, flg, MODIFY_ELEM_MODE_NONE);
				}
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "ưʪ����";
		if (desc) return "ʣ����ưʪ�򾤴����롣";
#else
		if (name) return "Summon Animals";
		if (desc) return "Summons animals.";
#endif
    
		{
			if (cast)
			{
				int  i;
				bool fail = TRUE;

				for (i = 0; i < 1 + ((mlev - 15)/ 10); i++)
				{
					if (summon_specific(-1, py, px, mlev, SUMMON_ANIMAL, (PM_ALLOW_GROUP | PM_FORCE_PET)))
						fail = FALSE;
				}

				if (fail)
#ifdef JP
					msg_print("ưʪ�ϸ���ʤ��ä���");
#else
					msg_print("No animals arrive.");
#endif
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "����ͧ�ͼ���";
		if (desc) return "��󥹥���1�Τ����Ϥ������˲��������в������㡢���ݡ�ۯ۰��������Ť��롣";
#else
		if (name) return "*Heal Monster*";
		if (desc) return "Heal a monster.";
#endif
    
		{
			int heal = 1000;

			if (info) return info_heal(0, 0, heal);

			if (cast)
			{
				bool result;
				bool old_target_pet = target_pet;

				result = get_aim_dir(&dir);

				target_pet = old_target_pet;

				if (!result) return NULL;

				fire_ball(GF_STAR_HEAL, dir, heal, 0, FALSE);
			}
		}
		break;

	}

	return "";
}


static cptr do_witch_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int dir;
	int mlev = p_ptr->magic_exp[REALM_WITCH]/10;
	int pstat;
	s16b chosen_elem;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ֳ�������";
		if (desc) return "������֡��ֳ������Ϥ���������롣";
#else
		if (name) return "Infravision";
		if (desc) return "Gives infravision for a while.";
#endif
    
		{
			int base = 100;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_infra(base + randint1(base), FALSE);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "���㡼��";
		if (desc) return "̥λ�ε�����ġ�̥�Ϥ��⤤�Ȼ볦������ƤΥ�󥹥�����̥λ���롣�񹳤�����̵����";
#else
		if (name) return "Charm Monster";
		if (desc) return "Attempts to charm a monster.";
#endif
    
		{
			int power, rad;
			pstat = p_ptr->stat_use[A_CHR];

			if (pstat < (18 + 180))
			{
				if (pstat >= (18 + 150))
				{
					power = mlev + 70;
					rad = 4;
				}
				else if (pstat >= (18 + 100))
				{
					power = mlev + 60;
					rad = 3;
				}
				else
				{
					power = mlev + 50;
					rad = 2;
				}
			}
			else power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				if (pstat < (18 + 180))
				{
					if (!get_aim_dir(&dir)) return NULL;

					fire_ball(GF_CHARM, dir, power, rad, FALSE);
				}
				else
				{
					charm_monsters(power);
				}
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "����β�";
		if (desc) return "�볦������ƤΥ�󥹥������𤵤��롣�񹳤�����̵����";
#else
		if (name) return "Illusion Pattern";
		if (desc) return "Attempts to confuse all monsters in sight.";
#endif
    
		{
			int power = mlev + 50;

			if (info) return info_power(power);

			if (cast)
			{
				confuse_monsters(power);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "�ҡ�����ʬ";
		if (desc) return "������֡��ҡ�����ʬ�ˤʤ롣";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "�ǥե������";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τ�Ǥ�դΥ�����Ȥ򶯤ᡢ����ȿ�ФΥ�����Ȥ���롣";
#else
		if (name) return "Deft element";
		if (desc) return "Increase choose element, decrease opposite element nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = 2;
			int rad = (mlev / 10) + 1;
			pstat = p_ptr->stat_use[A_INT];

			if (pstat >= (18 + 100)) amount++;
			if (pstat >= (18 + 150)) amount++;
			if (pstat >= (18 + 200)) amount++;

			if (info) info_power(amount);

			if (cast)
			{
				chosen_elem = choose_elem();
				if (chosen_elem == NO_ELEM)
				{
#ifdef JP
					msg_print("������Ȥ������������Τ��᤿��");
#else
					msg_print("You cancel enhancing the elememt.");
#endif
					return NULL;
				}

				inc_area_elem(0, chosen_elem, amount, rad, TRUE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "���θ��";
		if (desc) return "������֡��ͷ����򤹤�ǽ�Ϥ����롣��ǽ���⤤����ˡ�Υܥ�Ȥ���åȤ���򤹤롣";
#else
		if (name) return "Wind's gurad";
		if (desc) return "Satisfies hunger.";
#endif
    
		{
			int sides = mlev;
			int base = 20;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_wind_guard(randint1(sides)+base, FALSE);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "���ꥢ�֥�å�";
		if (desc) return "����������������Ǥ��Τ��鴰���˼����������Ϥ򾯤����������롣";
#else
		if (name) return "Clear blood";
		if (desc) return "Heals all cut and poison status. Heals HP a little.";
#endif
    
		{
			int dice = 4;
			int sides = 8;

			if (info) return info_heal(dice, sides, 0);

			if (cast)
			{
				set_poisoned(0);
				set_cut(0);
				hp_player(damroll(dice, sides));
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "�����å��ࡼ��";
		if (desc) return "������֡���®���롣";
#else
		if (name) return "Quick Move";
		if (desc) return "Hastes you for a while.";
#endif
    
		{
			int base = mlev;
			int sides = 25 + mlev;

			if (info) return info_duration(base, sides);

			if (cast)
			{
				set_fast(randint1(sides) + base, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�����ࡼ��";
		if (desc) return "��󥹥���1�Τ�®�����롣�񹳤�����̵����";
#else
		if (name) return "Slow Monster";
		if (desc) return "Attempts to slow a monster.";
#endif
    
		{
			int power = mlev;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				slow_monster(dir, power);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "��©�μ�";
		if (desc) return "������������̿�ϵۼ��ε�����ġ����Ф������ۤ������������";
#else
		if (name) return "Choked hand";
		if (desc) return "Fire a ball of life drain.";
#endif
    
		{
			int dice = 3;
			int sides = 5;
			int base = mlev + mlev / 2;
			int power = mlev + 50;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				if (fire_ball(GF_OLD_DRAIN, dir, damroll(dice, sides) + base, 0, FALSE))
				{
					if (!one_in_(3)) fire_ball(GF_SILENT, dir, power, 0, FALSE);
				}
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�����󥹥�����";
		if (desc) return "����ε�����ġ�";
#else
		if (name) return "Stun slaughter";
		if (desc) return "Fires a ball of stun.";
#endif
    
		{
			int power = mlev + 50;
			int rad = 1;
			if (pstat >= (18 + 100))
			{
				power = mlev + 60;
				rad++;
			}
			if (pstat >= (18 + 150))
			{
				power = mlev + 70;
				rad++;
			}

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_STASIS, dir, power, rad, FALSE);
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "��ȯ�Υ롼��";
		if (desc) return "��ʬ�Τ��뾲�ξ�ˡ���󥹥������̤����ȯ���ƥ��᡼����Ϳ����롼���������";
#else
		if (name) return "Explosive Rune";
		if (desc) return "Sets a glyph under you. The glyph will explode when a monster moves on it.";
#endif
    
		{
			if (cast)
			{
				explosive_rune(mlev);
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�����ץ�������";
		if (desc) return "1�ޥ����ɤ��󥹥��������ӱۤ��롣�������������ǤϤʤ����ϲ��ⵯ���ʤ���";
#else
		if (name) return "Jump Wall";
		if (desc) return "";
#endif
    
		{
			if (cast)
			{
				if (!jump_wall()) return NULL;
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "�ݥ����󥯥饦��";
		if (desc) return "���ꤷ��������������ϰϤ��ǹ����Ԥ���";
#else
		if (name) return "Poison cloud";
		if (desc) return "Fire a ball of poison.";
#endif
    
		{
			int sides = mlev;
			int base = 10 + mlev * 2;
			int rad = (pstat >= (18 + 150)) ? 3 : 2;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_POIS, dir, base + randint1(sides), rad, TRUE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "���ߡ�";
		if (desc) return "��ʬ�ξ��򵽤�����οͷ���­�����֤���";
#else
		if (name) return "Dummy";
		if (desc) return "Sets a dummy under you.";
#endif
    
		{
			if (cast)
			{
				if (!set_decoy()) return NULL;
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "������";
		if (desc) return "������֡������ŷ⡢�ꡢ�䵤���Ǥ��Ф������������롣�����ˤ�����������Ѥ��롣";
#else
		if (name) return "Resistance";
		if (desc) return "Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_oppose_acid(randint1(base) + base, FALSE);
				set_oppose_elec(randint1(base) + base, FALSE);
				set_oppose_fire(randint1(base) + base, FALSE);
				set_oppose_cold(randint1(base) + base, FALSE);
				set_oppose_pois(randint1(base) + base, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "�ޥ����륦���ݥ�";
		if (desc) return "������֡������Ϥ�̿��Ψ���������졢���νŤ���ۤȤ�ɴ����ʤ��ʤꡢ�����䵤���ꡢ�ŷ⡢�����ǤΤ����줫��°����Ĥ��롣�����Ӥ����򳰤���̵����";
#else
		if (name) return "Magical weapon";
		if (desc) return "";
#endif
    
		{
			int base = mlev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				if (!choose_magical_weapon()) return NULL;
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���ȥ����ݥ�";
		if (desc) return "��󥹥���1�Τ�ľ�ܹ����Ϥ���롣";
#else
		if (name) return "Melt Weapon";
		if (desc) return "";
#endif
    
		{
			if (cast)
			{
				int power = mlev * 2;
				if (!cast_melt_weapon(power)) return NULL;
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "�ץ쥤�������";
		if (desc) return "�������Ȥ餷�Ƥ����ϰϤ��������Τ�Ǥ�դΥ�����Ȥ򶯤�롣";
#else
		if (name) return "Pray element";
		if (desc) return "Increase choose element nearby area and the inside of a room permanently.";
#endif
    
		{
			int amount = 4;
			int rad = (mlev / 10) + 1;
			pstat = p_ptr->stat_use[A_INT];

			if (pstat >= (18 + 100)) amount += 2;
			if (pstat >= (18 + 150)) amount += 2;
			if (pstat >= (18 + 200)) amount += 2;

			if (info) info_power(amount);

			if (cast)
			{
				chosen_elem = choose_elem();
				if (chosen_elem == NO_ELEM)
				{
#ifdef JP
					msg_print("������Ȥ������������Τ��᤿��");
#else
					msg_print("You cancel enhancing the elememt.");
#endif
					return NULL;
				}

				inc_area_elem(0, chosen_elem, amount, rad, FALSE);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�߱群��";
		if (desc) return "��ʬ�μ�²���б���������ܥ�Υ�󥹥����򾤴����롣";
#else
		if (name) return "Summon aid";
		if (desc) return "Summon player's race symbol monsters.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				msg_print("�緳�򾤴�������");
#else
				msg_print("You summon minions.");
#endif
				summon_kin_player(mlev * 2 / 3 + randint1(mlev/2), py, px, PM_FORCE_PET);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "*����*�ӡ���";
		if (desc) return "��ʬ�θ��ߤΥ�����Ȥ�Ʊ��*����*�Υӡ�������ġ�";
#else
		if (name) return "*element* beam";
		if (desc) return "Fire a beam of *element*.";
#endif
    
		{
			int sides = 100;
			int base = mlev * 2;

			if (info) return info_damage(1, sides, base);

			if (cast)
			{
				int pure_elem_typ = GF_GODLY_SPEAR;
				switch (get_cur_pelem())
				{
				case NO_ELEM:
					pure_elem_typ = GF_MANA;
					break;
				case ELEM_FIRE:
					pure_elem_typ = GF_PURE_FIRE;
					break;
				case ELEM_AQUA:
					pure_elem_typ = GF_PURE_AQUA;
					break;
				case ELEM_EARTH:
					pure_elem_typ = GF_PURE_EARTH;
					break;
				case ELEM_WIND:
					pure_elem_typ = GF_PURE_WIND;
					break;
				}

				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(pure_elem_typ, dir, base + randint1(sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "��ˡ�γ�";
		if (desc) return "������֡���ˡ�ɸ��Ϥ�AC���夬�ꡢ��������ܤ�������ȿ��ǽ�ϡ������Τ餺����ͷ�����롣";
#else
		if (name) return "Magical armor";
		if (desc) return "Gives resistance to magic, bonus to AC, resistance to confusion, blindness, reflection, free action and levitation for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_magicdef(randint1(base) + base, FALSE);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�ߥ����μ�";
		if (desc) return "�����ƥ�1�Ĥ򤪶���Ѥ��롣";
#else
		if (name) return "Midas Touch";
		if (desc) return "Turn object to gold.";
#endif
    
		{
			if (cast)
			{
				if (!alchemy()) return NULL;
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "���ɾ���";
		if (desc) return "ʣ���Υ��ɤ򾤴����롣";
#else
		if (name) return "Summon molds";
		if (desc) return "Summons some molds.";
#endif
    
		{
			if (cast)
			{
				int  i;
				bool fail = TRUE;

				for (i = 0; i < 1 + ((mlev - 15)/ 10); i++)
				{
					if (summon_specific(-1, py, px, mlev * 2 / 3 + randint1(mlev/2), SUMMON_MOLD, (PM_ALLOW_GROUP | PM_FORCE_PET)))
						fail = FALSE;
				}

				if (fail)
#ifdef JP
					msg_print("���ɤϸ���ʤ��ä���");
#else
					msg_print("No molds arrive.");
#endif
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "�����륹�ȡ���";
		if (desc) return "ŷ���򰭲������롣";
#else
		if (name) return "Call storm";
		if (desc) return "Weather to bad.";
#endif
    
		{
			int power = 2;

			if (pstat >= (18 + 150)) power++;
			if (pstat >= (18 + 200)) power++;

			if (cast)
			{
				msg_print("ŷ�����Ӥ�Ƥ���...");

				set_weather(power, power, power);
			}
		}
		break;

	case 25:
#ifdef JP
		if (name) return "���Ͼõ�";
		if (desc) return "��ʬ�ޤ��ϥ�󥹥���1�Τˤ����ä���ˡ�������롣";
#else
		if (name) return "Dispel magic";
		if (desc) return "";
#endif
    
		{
			if (cast)
			{
				if (!cast_dispel_magic()) return NULL;
			}
		}
		break;

	case 26:
#ifdef JP
		if (name) return "�ޡ������쥤��";
		if (desc) return "��ʬ���濴�Ȥ��������ϰϤ����Ϥ������˲��������������ǡ�ۯ۰�����𤫤����������뱫��ߤ餻�롣";
#else
		if (name) return "Mercy Rain";
		if (desc) return "";
#endif
    
		{
			int base = 500;

			if (info) return info_heal(0, 0, base);

			if (cast)
			{
				hp_player(base);
				project(0, 3, py, px, base, GF_OLD_HEAL, PROJECT_KILL | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_NONE);
				set_poisoned(0);
				set_confused(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 27:
#ifdef JP
		if (name) return "�ڥȥ��饦��";
		if (desc) return "���ꤷ��������������ϰϤ��в������Ԥ���";
#else
		if (name) return "Petro Cloud";
		if (desc) return "Fires a ball of stonning.";
#endif
    
		{
			int base = 200 + mlev * 3;
			pstat = p_ptr->stat_use[A_INT];

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				int rad = (pstat >= (18 + 150)) ? 3 : 2;

				if (!get_aim_dir(&dir)) return FALSE;

				fire_ball(GF_STONE, dir, base, rad, FALSE);
			}
		}
		break;

	case 28:
#ifdef JP
		if (name) return "��������";
		if (desc) return "û��Υ��λ��ꤷ�����˥ƥ�ݡ��Ȥ��롣";
#else
		if (name) return "Dimension Door";
		if (desc) return "Teleport to given location.";
#endif
    
		{
			int range = mlev / 2 + 10;

			if (info) return info_range(range);

			if (cast)
			{
#ifdef JP
				msg_print("�������⤬����������Ū�Ϥ�����ǲ�������");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				if (!dimension_door(mlev)) return NULL;
			}
		}
		break;

	case 29:
#ifdef JP
		if (name) return "�����󥸥������";
		if (desc) return "��ʬ�ޤ��ϥ�󥹥���1�ΤΥ�����Ȥ�ȿž�����롣";
#else
		if (name) return "Change element";
		if (desc) return "Change element of player or monster.";
#endif
    
		{
			if (cast)
			{
				int power = mlev * 2;
				if (!cast_change_element(power)) return NULL;
			}
		}
		break;

	case 30:
#ifdef JP
		if (name) return "ͩ�β�";
		if (desc) return "������֡��ɤ��̤�ȴ���뤳�Ȥ��Ǥ���������᡼�����ڸ������ͩ�Τξ��֤��ѿȤ��롣";
#else
		if (name) return "Wraithform";
		if (desc) return "Becomes wraith form which gives ability to pass walls and makes all damages half.";
#endif
    
		{
			int base = mlev / 2;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_wraith_form(randint1(base) + base, FALSE);
			}
		}
		break;
	}

	return "";
}


static cptr do_drakonite_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int mlev = p_ptr->magic_exp[REALM_DRAKONITE]/10;

#ifdef JP
	static const char s_dam[] = "»��:";
#else
	static const char s_dam[] = "dam ";
#endif

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ޡ��ƥ��饤��";
		if (desc) return "���ޤ��ݤ�����ˡ�������󥹥��������褵���롣�������Ȥǽи�������ˡ�������󥹥����ˤ�̵����";
#else
		if (name) return "Martyrs";
		if (desc) return "";
#endif
    
		{
			if (cast)
			{
				int i;
				bool *no_revive;
				bool revived = FALSE;

				/* Allocate the "no_revive" array */
				C_MAKE(no_revive, max_r_idx, bool);

				/* Scan the random quests */
				for (i = 1; i < max_quests; i++)
				{
					if (quest[i].r_idx) no_revive[quest[i].r_idx] = TRUE;
				}

				/* Scan the arena */
				for (i = 0; i < MAX_ARENA_MONS + 4; i++)
				{
					no_revive[arena_info[i].r_idx] = TRUE;
				}

				/* Scan the monster races */
				for (i = 0; i < max_r_idx; i++)
				{
					monster_race *r_ptr = &r_info[i];

					if (!r_ptr->name) continue;

					/* Unique monsters only */
					if (!(r_ptr->flags1 & RF1_UNIQUE)) continue;

					/* Questors don't revive */
					if (r_ptr->flags1 & RF1_QUESTOR) continue;

					/* Unique monster is alive, no need to revive */
					if (r_ptr->max_num) continue;

					/* This unique is stayed dead... */
					if (no_revive[i]) continue;

					/* The dead unique monster is come back!! */
					r_ptr->max_num = 1;
					revived = TRUE;
				}

				/* Free the "no_revive" array */
				C_KILL(no_revive, max_r_idx, bool);

				if (revived)
				{
	#ifdef JP
					msg_format("��ä���Ũã�����äƤ���Τ򴶤��롣");
	#else
					msg_format("You feel slain foes coming back.");
	#endif
					/* Window stuff */
					p_ptr->window |= (PW_MONSTER);

					if (p_ptr->materialize_cnt < MAX_SHORT) p_ptr->materialize_cnt++;
				}
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�ɥߥ˥���";
		if (desc) return "�볦������ƤΥ�󥹥������񹳤Ǥ����˳μ¤˸�®�����롣��ˡ�������󥹥����ϸ�®���ʤ���礬���롣";
#else
		if (name) return "Dominion";
		if (desc) return "";
#endif
    
		{
			int sides = mlev + 25;
			int base = mlev;
#ifdef JP
			if (info) return format("����:%d+d%d", base, sides);
#else
			if (info) return format("power %d+d%d", base, sides);
#endif

			if (cast)
			{
				project_hack(GF_NEW_SLOW, randint1(sides) + base);
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "�ƥ�ڥ���";
		if (desc) return "�볦����ϰϤ��Ф���*��*�����Ԥ�������ˡ�ŷ���򰭲������롣";
#else
		if (name) return "Tempest";
		if (desc) return "";
#endif
    
		{
			int sides = mlev * 5;
			int base = mlev * 6;

			if (info) return format("%s%d+d%d", s_dam, base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_PURE_WIND, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				set_weather(8, 8, 8);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���˥ҥ졼�����";
		if (desc) return "�볦����ϰϤ��Ф���*�б�*�����Ԥ�������ˡ�ŷ�����ɹ��������롣";
#else
		if (name) return "Annihilation";
		if (desc) return "";
#endif
    
		{
			int sides = mlev * 5;
			int base = mlev * 6;

			if (info) return format("%s%d+d%d", s_dam, base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_PURE_FIRE, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				set_weather(-8, -8, -8);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "��ƥ����ȥ饤��";
		if (desc) return "�볦����ϰϤ��Ф���*����*�����Ԥ������𤵤��롣";
#else
		if (name) return "Meteor Strike";
		if (desc) return "";
#endif
    
		{
			int sides = mlev * 5;
			int base = mlev * 6;
			int power = mlev + 50;

			if (info) return format("%s%d+d%d", s_dam, base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_PURE_EARTH, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				confuse_monsters(power);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "�ۥ磻�ȥߥ塼��";
		if (desc) return "�볦����ϰϤ��Ф���*��*�����Ԥ������㤵���롣";
#else
		if (name) return "White Mute";
		if (desc) return "";
#endif
    
		{
			int sides = mlev * 5;
			int base = mlev * 6;
			int power = mlev + 50;

			if (info) return format("%s%d+d%d", s_dam, base, sides);

			if (cast)
			{
				project(0, MAX_SIGHT, py, px, base + randint1(sides), GF_PURE_AQUA, PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
				stasis_monsters(power);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "�꡼�󥫡��ͥ���";
		if (desc) return "���Ƥ�ǽ�ϴ����͡����ϡ�MP����ǽ�򤤤��餫�γ��ǰ����Ѥ����ޤ޼�ʬ�κ����٥��1���᤹��";
#else
		if (name) return "Reincarnate";
		if (desc) return "Reincarnate.";
#endif
    
		{
			if (cast)
			{
#ifdef JP
				if (!get_check("��٥�1�Υ���饯����ž�����ޤ���������Ǥ����� ")) return NULL;
#else
				if (!get_check("Reincarnate as level 1 character. Are you sure? ")) return NULL;
#endif
				reincarnation();
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���ʥåץɥ饴��";
		if (desc) return "��ʬ��ǽ�Ϥ˱����������Ѥ��롣�����Τʤ��ץ쥤�䡼���ѿȤ������ϻ�¹��ȯ���Ǥ����ǽ�������롣";
#else
		if (name) return "Snapdragon";
		if (desc) return "Turn player to runeweapon.";
#endif
    
		{
			if (cast)
			{
				int i;

#ifdef JP
				if (!get_check("�����������Ѳ����ޤ�����")) return NULL;
#else
				if (!get_check("Do you really want to commit change into a weapon? ")) return NULL;
#endif
				/* Special Verification for Snap Dragon */
#ifdef JP
				prt("��ǧ�Τ��� '@' �򲡤��Ʋ�������", 0, 0);
#else
				prt("Please verify CHANGE by typing the '@' sign: ", 0, 0);
#endif

				flush();
				i = inkey();
				prt("", 0, 0);
				if (i != '@') return NULL;

				snap_dragon();
			}
		}
		break;

	}

	return "";
}


static cptr do_crusade_spell(int spell, int mode)
{
	bool name = (mode == SPELL_NAME) ? TRUE : FALSE;
	bool desc = (mode == SPELL_DESC) ? TRUE : FALSE;
	bool info = (mode == SPELL_INFO) ? TRUE : FALSE;
	bool cast = (mode == SPELL_CAST) ? TRUE : FALSE;

	int	dir;
	int mlev = p_ptr->magic_exp[REALM_CRUSADE]/10;
	int pstat = p_ptr->stat_use[A_WIS];;

	switch (spell)
	{
	case 0:
#ifdef JP
		if (name) return "�ٰ�¸�ߴ���";
		if (desc) return "�᤯�μٰ��ʥ�󥹥������Τ��롣";
#else
		if (name) return "Detect Evil";
		if (desc) return "Detects all evil monsters in your vicinity.";
#endif
    
		{
			int rad = DETECT_RAD_DEFAULT;

			if (info) return info_radius(rad);

			if (cast)
			{
				detect_monsters_evil(rad);
			}
		}
		break;

	case 1:
#ifdef JP
		if (name) return "�饤�ȥ˥󥰥ܥ�";
		if (desc) return "�����Υӡ�������ġ�";
#else
		if (name) return "Litghtning bow";
		if (desc) return "Fire a beam of lite.";
#endif
    
		{
			int dice = 3 + (mlev - 1) /5;
			int sides = 4;

			if (info) return info_damage(dice, sides, 0);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_LITE, dir, damroll(dice, sides));
			}
		}
		break;

	case 2:
#ifdef JP
		if (name) return "����";
		if (desc) return "���ܤ������ƤΥ�󥹥�����̲�餻�롣�񹳤�����̵����";
#else
		if (name) return "Sanctuary";
		if (desc) return "Attempts to sleep monsters in the adjacent squares.";
#endif
    
		{
			int power = mlev;

			if (info) return info_power(power);

			if (cast)
			{
				sleep_monsters_touch(power);
			}
		}
		break;

	case 3:
#ifdef JP
		if (name) return "���ξ���";
		if (desc) return "�����ǡ�ۯ۰�����������롣";
#else
		if (name) return "Purify";
		if (desc) return "Heals all cut, stun and poison status.";
#endif
    
		{
			if (cast)
			{
				set_cut(0);
				set_poisoned(0);
				set_stun(0);
			}
		}
		break;

	case 4:
#ifdef JP
		if (name) return "Ʃ����ǧ";
		if (desc) return "������֡�Ʃ���ʤ�Τ�������褦�ˤʤ롣";
#else
		if (name) return "See Invisible";
		if (desc) return "Gives see invisible for a while.";
#endif
    
		{
			int base = 24;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_invis(randint1(base) + base, FALSE);
			}
		}
		break;

	case 5:
#ifdef JP
		if (name) return "������������";
		if (desc) return "�������å��ն�������Υܥ�Ȥ�Ϣ�ͤ��롣";
#else
		if (name) return "Star Dust";
		if (desc) return "Fires many bolts of light near the target.";
#endif
    
		{
			int dice = 3 + (mlev - 1) / 9;
			int sides = 2;

			if (info) return info_multi_damage_dice(dice, sides);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_blast(GF_LITE, dir, dice, sides, 10, 3);
			}
		}
		break;

	case 6:
#ifdef JP
		if (name) return "����";
		if (desc) return "�ٰ��ʥ�󥹥�����ư����ߤ�롣";
#else
		if (name) return "Arrest";
		if (desc) return "Attempts to paralyze an evil monster.";
#endif
    
		{
			int power = mlev * 2;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				stasis_evil(dir, power);
			}
		}
		break;

	case 7:
#ifdef JP
		if (name) return "���ʤ����";
		if (desc) return "���ʤ��Ϥ�����������ġ��ٰ��ʥ�󥹥������Ф����礭�ʥ��᡼����Ϳ���뤬�����ɤʥ�󥹥����ˤϸ��̤��ʤ���";
#else
		if (name) return "Holy Orb";
		if (desc) return "Fires a ball with holy power. Hurts evil monsters greatly, but don't effect good monsters.";
#endif
    
		{
			int dice = 3;
			int sides = 6;
			int rad = (mlev < 30) ? 2 : 3;
			int base = mlev + mlev / 2;

			if (info) return info_damage(dice, sides, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_HOLY_FIRE, dir, damroll(dice, sides) + base, rad, FALSE);
			}
		}
		break;

	case 8:
#ifdef JP
		if (name) return "�ե�����";
		if (desc) return "�볦������ƤΥ���ǥåɤ˥��᡼����Ϳ�����ƥ�ݡ��Ȥ����롣";
#else
		if (name) return "Face";
		if (desc) return "Damages all undead and demons in sight, and scares all evil monsters in sight.";
#endif
    
		{
			int sides = mlev * 4;

			if (info) return info_damage(1, sides, 0);

			if (cast)
			{
				dispel_undead(randint1(sides));
				project_hack_undead(GF_AWAY_ALL, sides);
			}
		}
		break;

	case 9:
#ifdef JP
		if (name) return "�ۤ�����";
		if (desc) return "���Ϥ��ŷ�Υܥ�Ȥ����ġ�";
#else
		if (name) return "Judgment Thunder";
		if (desc) return "Fires a powerful bolt of lightning.";
#endif
    
		{
			int dam = mlev * 5;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_bolt(GF_ELEC, dir, dam);
			}
		}
		break;

	case 10:
#ifdef JP
		if (name) return "�����ܤ�";
		if (desc) return "�������åȤμ��Ϥ�ʬ��ε��¿����Ȥ���";
#else
		if (name) return "Wrath of the God";
		if (desc) return "Drops many balls of disintegration near the target.";
#endif
    
		{
			int dam = mlev * 3 + 25;
			int rad = 2;

			if (info) return info_multi_damage(dam);

			if (cast)
			{
				if (!cast_wrath_of_the_god(dam, rad)) return NULL;
			}
		}
		break;

	case 11:
#ifdef JP
		if (name) return "�ޥ��å��ܥ�";
		if (desc) return "��ʬ���濴�Ȥ��������ϰϤ����ʤ빶���Ԥ�����ʬ�����Ϥ�1/4�򼺤���";
#else
		if (name) return "Magic bomb";
		if (desc) return "Fires a huge ball of holy power nearby.";
#endif
    
		{
			int dam = p_ptr->chp;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				project(0, rad, py, px, dam, GF_HOLY_FIRE, PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_NO_REDUCE, MODIFY_ELEM_MODE_MAGIC);
#ifdef JP
				take_hit(DAMAGE_USELIFE, p_ptr->chp / 4, "����Ū�ʥޥ��å��ܥ�");
#else
				take_hit(DAMAGE_USELIFE, p_ptr->chp / 4, "a suicidal Magic Bomb");
#endif
			}
		}
		break;

	case 12:
#ifdef JP
		if (name) return "�ε�����";
		if (desc) return "������֡��ҡ�����ʬ�ˤʤ롣";
#else
		if (name) return "Heroism";
		if (desc) return "Removes fear, and gives bonus to hit and 10 more HP for a while.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_hero(randint1(base) + base, FALSE);
				hp_player(10);
				set_afraid(0);
			}
		}
		break;

	case 13:
#ifdef JP
		if (name) return "������������";
		if (desc) return "����ǥåɤ򸽺ߤγ�����ä���������ġ��񹳤�����̵����";
#else
		if (name) return "Exorcism";
		if (desc) return "Eliminates undead monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int dummy = (pstat >= (18 + 150)) ? 3 : 2;
			int power = mlev * dummy;

			if (info) return info_power(power);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_GENOCIDE_UNDEAD, dir, power, dummy, FALSE);
			}
		}
		break;

	case 14:
#ifdef JP
		if (name) return "�ȥ�󥭥饤��";
		if (desc) return "����β��������ݤ����롣";
#else
		if (name) return "Berserk";
		if (desc) return "Gives bonus to hit and HP, immunity to fear for a while. But decreases AC.";
#endif
    
		{
			int base = 25;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_shero(randint1(base) + base, FALSE);
				hp_player(30);
				set_afraid(0);
			}
		}
		break;

	case 15:
#ifdef JP
		if (name) return "���ʤ륪����";
		if (desc) return "������֡��ٰ��ʥ�󥹥�������Ĥ������ʤ륪��������롣";
#else
		if (name) return "Holy Aura";
		if (desc) return "Gives aura of holy power which injures evil monsters which attacked you for a while.";
#endif
    
		{
			int base = 20;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_sh_holy(randint1(base) + base, FALSE);
			}
		}
		break;

	case 16:
#ifdef JP
		if (name) return "���ʤ��";
		if (desc) return "�̾�������Ǽ٤�°����Ĥ��롣";
#else
		if (name) return "Holy Blade";
		if (desc) return "Makes current weapon especially deadly against evil monsters.";
#endif
    
		{
			if (cast)
			{
				brand_weapon(EGO_KILL_EVIL);
			}
		}
		break;

	case 17:
#ifdef JP
		if (name) return "���㥤�˥�";
		if (desc) return "��ʬ�����Ϥ�1/3�ΰ��Ϥ����ʤ빶���Ԥ����񹳤�����̵����";
#else
		if (name) return "Shining";
		if (desc) return "Eliminates all nearby undead monsters, exhausting you.  Powerful or unique monsters may be able to resist.";
#endif
    
		{
			int base = mlev + 60;

			if (info) return info_damage(0, 0, base);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;
				fire_ball(GF_SHINING, dir, base, 0, FALSE);
			}
		}
		break;

	case 18:
#ifdef JP
		if (name) return "���ʤ�����";
		if (desc) return "�볦��μٰ���¸�ߤ��礭�ʥ��᡼����Ϳ�������Ϥ���������ǡ����ݡ�ۯ۰���֡���������������롣";
#else
		if (name) return "Holy Word";
		if (desc) return "Damages all evil monsters in sight, heals HP somewhat, and completely heals poison, fear, stun and cut status.";
#endif
    
		{
			int dam_sides = mlev * 6;
			int heal = 100;

#ifdef JP
			if (info) return format("»:1d%d/��%d", dam_sides, heal);
#else
			if (info) return format("dam:d%d/h%d", dam_sides, heal);
#endif

			if (cast)
			{
				dispel_evil(randint1(dam_sides));
				hp_player(heal);
				set_afraid(0);
				set_poisoned(0);
				set_stun(0);
				set_cut(0);
			}
		}
		break;

	case 19:
#ifdef JP
		if (name) return "�������С�����";
		if (desc) return "����������ε�����ġ�";
#else
		if (name) return "Star Burst";
		if (desc) return "Fires a huge ball of powerful light.";
#endif
    
		{
			int dam = 100 + mlev * 2;
			int rad = 4;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_ball(GF_LITE, dir, dam, rad, FALSE);
			}
		}
		break;

	case 20:
#ifdef JP
		if (name) return "�ϥ�ޥ��ɥ�";
		if (desc) return "���դΥ����ƥࡢ��󥹥������Ϸ����˲����롣";
#else
		if (name) return "Armageddon";
		if (desc) return "Destroy everything in nearby area.";
#endif
    
		{
			int base = 13;
			int sides = 5;

			if (cast)
			{
				destroy_area(py, px, base + randint0(sides));
			}
		}
		break;

	case 21:
#ifdef JP
		if (name) return "�ܤˤ��ܤ�";
		if (desc) return "������֡���ʬ�����᡼����������Ȥ��˹����Ԥä���󥹥������Ф���Ʊ���Υ��᡼����Ϳ���롣";
#else
		if (name) return "An Eye for an Eye";
		if (desc) return "Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.";
#endif
    
		{
			int base = 10;

			if (info) return info_duration(base, base);

			if (cast)
			{
				set_tim_eyeeye(randint1(base) + base, FALSE);
			}
		}
		break;

	case 22:
#ifdef JP
		if (name) return "�ۡ��꡼���";
		if (desc) return "���ʤ�ӡ�������ġ�";
#else
		if (name) return "Holy lance";
		if (desc) return "Fires a huge beam of holy.";
#endif
    
		{
			int dam = 200 + mlev * 2;

			if (info) return info_damage(0, 0, dam);

			if (cast)
			{
				if (!get_aim_dir(&dir)) return NULL;

				fire_beam(GF_HOLY_FIRE, dir, dam);
			}
		}
		break;

	case 23:
#ifdef JP
		if (name) return "�������ƥ�����";
		if (desc) return "���ܤ����󥹥��������ʤ���᡼����Ϳ�����볦��Υ�󥹥����˥��᡼������®��ۯ۰�����𡢶��ݡ�̲���Ϳ���롣��������Ϥ�������롣";
#else
		if (name) return "Star Tiara";
		if (desc) return "Damages all adjacent monsters with holy power. Damages and attempt to slow, stun, confuse, scare and freeze all monsters in sight. And heals HP.";
#endif
    
		{
			int b_dam = mlev * 11;
			int d_dam = mlev * 4;
			int heal = 100;
			int power = mlev * 4;

#ifdef JP
			if (info) return format("��%d/»%d+%d", heal, d_dam, b_dam/2);
#else
			if (info) return format("h%d/dm%d+%d", heal, d_dam, b_dam/2);
#endif

			if (cast)
			{
				project(0, 1, py, px, b_dam, GF_HOLY_FIRE, PROJECT_KILL, MODIFY_ELEM_MODE_MAGIC);
				dispel_monsters(d_dam);
				slow_monsters(mlev);
				stun_monsters(power);
				confuse_monsters(power);
				turn_monsters(power);
				stasis_monsters(power);
				hp_player(heal);
			}
		}
		break;

	case 24:
#ifdef JP
		if (name) return "����";
		if (desc) return "�볦������ɤʥ�󥹥�����ڥåȤˤ��褦�Ȥ����ʤ�ʤ��ä����ڤ����ɤǤʤ���󥹥������ݤ����롣�����¿���β�®���줿���Τ򾤴������ҡ�������ʡ����®���мٰ��볦�����롣";
#else
		if (name) return "Crusade";
		if (desc) return "Attempts to charm all good monsters in sight, and scare all non-charmed monsters, and summons great number of knights, and gives heroism, bless, speed and protection from evil.";
#endif
    
		{
			if (cast)
			{
				int base = 25;
				int sp_sides = 20 + mlev;
				int sp_base = mlev;

				int i;
				crusade(mlev * 4);
				for (i = 0; i < 12; i++)
				{
					int attempt = 10;
					int my, mx;

					while (attempt--)
					{
						scatter(&my, &mx, py, px, 4, 0);

						/* Require empty grids */
						if (cave_empty_bold2(my, mx)) break;
					}
					if (attempt < 0) continue;
					summon_specific(-1, my, mx, mlev, SUMMON_HUMANS, (PM_ALLOW_GROUP | PM_FORCE_PET | PM_HASTE | PM_ALLOW_UNIQUE));
				}
				set_hero(randint1(base) + base, FALSE);
				set_blessed(randint1(base) + base, FALSE);
				set_fast(randint1(sp_sides) + sp_base, FALSE);
				set_protevil(randint1(base) + base, FALSE);
				set_afraid(0);
			}
		}
		break;
	}

	return "";
}


/*
 * Do everything for each spell
 */
cptr do_spell(int realm, int spell, int mode)
{
	switch (realm)
	{
	case REALM_MAGERY:     return do_magery_spell(spell, mode);
	case REALM_FIRE:  return do_fire_spell(spell, mode);
	case REALM_AQUA:   return do_aqua_spell(spell, mode);
	case REALM_EARTH:    return do_earth_spell(spell, mode);
	case REALM_WIND:    return do_wind_spell(spell, mode);
	case REALM_HOLY:   return do_holy_spell(spell, mode);
	case REALM_DEATH:    return do_death_spell(spell, mode);
	case REALM_SYMBIOTIC:  return do_symbiotic_spell(spell, mode);
	case REALM_WITCH:   return do_witch_spell(spell, mode);
	case REALM_DRAKONITE:    return do_drakonite_spell(spell, mode);
	case REALM_CRUSADE:  return do_crusade_spell(spell, mode);
	}

	return NULL;
}
