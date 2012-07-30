#include "angband.h"

void cause_wounds_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cause Light Wounds", "�ڽ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to damage a single foe.", "1�ΤΥ�󥹥����˾����᡼����Ϳ���롣�񹳤�����̵����"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(3, spell_power(8), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_ball_hide(GF_CAUSE_1, dir, spell_power(damroll(3, 8)), 0);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void cause_wounds_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cause Medium Wounds", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to damage a single foe.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(8, spell_power(8), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_ball_hide(GF_CAUSE_2, dir, spell_power(damroll(8, 8)), 0);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void cause_wounds_III_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cause Critical Wounds", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to damage a single foe.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(10, spell_power(15), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_ball_hide(GF_CAUSE_3, dir, spell_power(damroll(10, 15)), 0);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void cause_wounds_IV_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cause Mortal Wounds", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to damage a single foe.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(15, spell_power(15), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_ball_hide(GF_CAUSE_4, dir, spell_power(damroll(15, 15)), 0);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void clairvoyance_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Clairvoyance", "��Τ��"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Maps and lights whole dungeon level and gives telepathy for a while.", "���γ����Τ�ʵפ˾Ȥ餷�����󥸥���⤹�٤ƤΥ����ƥ���Τ��롣����ˡ�������֥ƥ�ѥ���ǽ�Ϥ����롣"));
		break;
	case SPELL_CAST:
		chg_virtue(V_KNOWLEDGE, 1);
		chg_virtue(V_ENLIGHTEN, 1);

		wiz_lite(p_ptr->tim_superstealth > 0);

		if (!p_ptr->telepathy)
			set_tim_esp(randint1(30) + 25, FALSE);

		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void clear_mind_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Clear Mind", "�����߿�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Player regains 3 + L/20 sp. This won't work if the player has any pets.");
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (total_friends)
		{
			msg_print(T("You need to concentrate on your pets now.", "���ϥڥåȤ���뤳�Ȥ˽��椷�Ƥ��ʤ��ȡ�"));
			return;
		}
		if (p_ptr->pclass == CLASS_RUNE_KNIGHT)
		{
			msg_print("Your mind remains cloudy.");
			return;
		}

		msg_print(T("You feel your head clear a little.", "����Ƭ���ϥå��ꤷ����"));

		sp_player(3 + p_ptr->lev/20);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void cold_touch_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cold Touch", "���μ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Freeze things with your icy fingers!");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(2 * p_ptr->lev)));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your hands get very cold.", "���ʤ���ξ��ϤȤƤ��䤿���ʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your hands warm up.", "�꤬�Ȥ����ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can freeze things with a touch.", "���ʤ���ʪ�򿨤ä���餻�뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		int x, y;
		cave_type *c_ptr;

		if (!get_rep_dir2(&dir))
		{
			var_set_bool(res, FALSE);
			break;
		}
		var_set_bool(res, TRUE);
		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];

		if (!c_ptr->m_idx)
		{
			msg_print(T("You wave your hands in the air.", "���ʤ��ϲ���ʤ����Ǽ�򿶤ä���"));
			break;
		}
		fire_bolt(GF_COLD, dir, spell_power(2 * p_ptr->lev));
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_cold_touch(void) { return cast_spell(cold_touch_spell); }

void confusing_lights_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Confusing Lights", "���Ǥθ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Emits confusing lights, slowing, stunning, confusing, scaring and freezing nearby monsters.");
		break;
	case SPELL_CAST:
		msg_print(T("You glare nearby monsters with a dazzling array of confusing lights!", "�դ���ˤ��..."));
		slow_monsters();
		stun_monsters(p_ptr->lev * 4);
		confuse_monsters(p_ptr->lev * 4);
		turn_monsters(p_ptr->lev * 4);
		stasis_monsters(p_ptr->lev * 4);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void create_food_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Create Food", "��������"));
		break;
	case SPELL_DESC:
		if (p_ptr->prace == RACE_HOBBIT)
			var_set_string(res, "It's time for second breakfast!  Cook up a tasty meal.");
		else
			var_set_string(res, "Create a ration of tasty food.");
		break;
	case SPELL_CAST:
	{
		object_type forge;

		object_prep(&forge, lookup_kind(TV_FOOD, SV_FOOD_RATION));
		drop_near(&forge, -1, py, px);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_create_food(void) { return cast_spell(create_food_spell); }

void cure_poison_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cure Poison", "����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Cure poison status.", "������Ǥ��������"));
		break;
	case SPELL_CAST:
		set_poisoned(0, TRUE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void cure_wounds_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cure Light Wounds", "�ڽ��μ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Heals cut and HP a little.", "��������Ϥ򾯤����������롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(2, spell_power(10), 0));
		break;
	case SPELL_CAST:
		hp_player(spell_power(damroll(2, 10)));
		set_cut(p_ptr->cut - 10, TRUE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void cure_wounds_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cure Medium Wounds", "�Ž��μ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Heals cut and HP more.", "��������Ϥ������ٲ��������롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(4, spell_power(10), 0));
		break;
	case SPELL_CAST:
		hp_player(spell_power(damroll(4, 10)));
		set_cut((p_ptr->cut / 2) - 20, TRUE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void cure_wounds_III_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Cure Critical Wounds", "��̿���μ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Heals cut, stun and HP greatly.", "���Ϥ������˲��������������ۯ۰���֤��������롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_heal(8, spell_power(10), 0));
		break;
	case SPELL_CAST:
		hp_player(spell_power(damroll(8, 10)));
		set_stun(0, TRUE);
		set_cut(0, TRUE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void darkness_storm_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Darkness Storm", "�Ź�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a huge ball of darkness.", "����ʰŹ��ε�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(100 + p_ptr->lev * 2)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You invoke a darkness storm.", "�Ź�����μ�ʸ��ǰ������"));
		fire_ball(GF_DARK, dir, spell_power(100 + p_ptr->lev * 2), spell_power(4));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void darkness_storm_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Darkness Storm", "�Ź�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a huge ball of darkness of unmatched power", "����ʰŹ��ε�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(10, spell_power(10), spell_power(50 + p_ptr->lev * 8)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You invoke a darkness storm.", "�Ź�����μ�ʸ��ǰ������"));
		fire_ball(GF_DARK, dir, 
			spell_power(50 + p_ptr->lev * 8 + damroll(10, 10)), 
			spell_power(4));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void day_of_the_dove_spell(int cmd, variant *res)
{
	int power = spell_power(p_ptr->lev * 2);
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Day of the Dove", "��ι�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to charm all monsters in sight.", "�볦������ƤΥ�󥹥�����̥λ���롣�񹳤�����̵����"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_power(power));
		break;
	case SPELL_CAST:
		charm_monsters(power);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void dazzle_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Dazzle", "����"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Emits dazzling lights, stunning, confusing and scaring nearby monsters.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to emit dazzling lights.", "����������ȯ����ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to emit dazzling lights.", "�ޤФ椤������ȯ����ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can emit confusing, blinding radiation.", "���ʤ��Ϻ�������ܤ��������������ǽ��ȯ�����뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		stun_monsters(p_ptr->lev * 4);
		confuse_monsters(p_ptr->lev * 4);
		turn_monsters(p_ptr->lev * 4);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_dazzle(void) { return cast_spell(dazzle_spell); }

void demon_breath_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Breathe Fire/Nether", "�Ϲ�/�б�Υ֥쥹"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Breathe a powerful blast of either fire or nether at your opponent.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev * 3)));
		break;
	case SPELL_CAST:
	{
		int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		stop_mouth();

		msg_format(T("You breathe %s.", "���ʤ���%s�Υ֥쥹���Ǥ�����"),
			((type == GF_NETHER) ? T("nether", "�Ϲ�") : T("fire", "�б�")));

		fire_ball(type, dir, spell_power(p_ptr->lev * 3), -(p_ptr->lev / 15) - 1);
		var_set_bool(res, TRUE);
		break;
	}
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev/3);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void destruction_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Word of Destruction");
		break;
	case SPELL_DESC:
		var_set_string(res, "Destroys everything in your nearby vicinity ... except you, of course.");
		break;
	case SPELL_CAST:
		destroy_area(py, px, 12 + randint1(4), spell_power(4 * p_ptr->lev));
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_destruction(void) { return cast_spell(destruction_spell); }

void detect_curses_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Detect Curses", "��������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Detected cursed items in your inventory.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You can feel evil magics.", "�ٰ�����ˡ���ΤǤ���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You can no longer feel evil magics.", "�ٰ�����ˡ�򴶤����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can feel the danger of evil magic.", "���ʤ��ϼٰ�����ˡ�δ��򴶤��Ȥ뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int i;

		for (i = 0; i < INVEN_TOTAL; i++)
		{
			object_type *o_ptr = &inventory[i];

			if (!o_ptr->k_idx) continue;
			if (!object_is_cursed(o_ptr)) continue;

			o_ptr->feeling = FEEL_CURSED;
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_detect_curses(void) { return cast_spell(detect_curses_spell); }

void detect_doors_stairs_traps_spell(int cmd, variant *res)
{
	int rad = DETECT_RAD_DEFAULT;

	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Detect Doors & Traps", "�ɥ���� ����"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Detects doors, stairs, and traps in your vicinity.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_radius(rad));
		break;
	case SPELL_CAST:
		detect_traps(rad, TRUE);
		detect_doors(rad);
		detect_stairs(rad);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_detect_doors_stairs_traps(void) { return cast_spell(detect_doors_stairs_traps_spell); }

void detect_menace_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Detect Ferocity", "��������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Detects nearby menacing monsters.  Only intelligent monsters are detected.", ""));
		break;
	case SPELL_CAST:
		detect_monsters_mind(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void detect_monsters_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Detect Monsters");
		break;
	case SPELL_DESC:
		var_set_string(res, "Detects nearby monsters.");
		break;
	case SPELL_CAST:
		detect_monsters_normal(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_detect_monsters(void) { return cast_spell(detect_monsters_spell); }

void detect_objects_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Detect Objects");
		break;
	case SPELL_DESC:
		var_set_string(res, "Detects nearby objects.");
		break;
	case SPELL_CAST:
		detect_objects_normal(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_detect_objects(void) { return cast_spell(detect_objects_spell); }

void detect_traps_spell(int cmd, variant *res)
{
	int rad = DETECT_RAD_DEFAULT;

	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Detect Traps");
		break;
	case SPELL_DESC:
		var_set_string(res, "Detects traps in your vicinity.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_radius(rad));
		break;
	case SPELL_CAST:
		detect_traps(rad, TRUE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_detect_traps(void) { return cast_spell(detect_traps_spell); }

void detect_treasure_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Detect Treasure");
		break;
	case SPELL_DESC:
		var_set_string(res, "Detects nearby treasure.");
		break;
	case SPELL_CAST:
		detect_treasure(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_detect_treasure(void) { return cast_spell(detect_treasure_spell); }

void detection_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Detection", "������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Detects all monsters, traps, doors, stairs, treasures and items in your vicinity.", "�᤯�����ƤΥ�󥹥�����櫡��⡢���ʡ������������ƥ����ƥ���Τ��롣"));
		break;
	case SPELL_CAST:
		detect_all(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void dimension_door_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Dimension Door");
		break;
	case SPELL_DESC:
		var_set_string(res, "Open a portal to another dimension and step to a nearby location with great precision.");
		break;
	case SPELL_CAST:
		var_set_bool(res, dimension_door(p_ptr->lev / 2 + 10));
		break;
	case SPELL_ENERGY:
		if (mut_present(MUT_ASTRAL_GUIDE))
			var_set_int(res, 30);
		else
			default_spell(cmd, res);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_dimension_door(void) { return cast_spell(dimension_door_spell); }

void disintegrate_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Disintegrate", "����ʬ��"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a huge ball of disintegration.", "�����ʬ��ε�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev + 70)));
		break;
	case SPELL_CAST:
	{
		int dam = spell_power(p_ptr->lev + 70);
		int rad = 3 + p_ptr->lev / 40;
		int dir;
			
		var_set_bool(res, FALSE);

		if (!get_aim_dir(&dir)) return;
		fire_ball(GF_DISINTEGRATE, dir, dam, rad);
			
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void dispel_magic_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Dispel Magic", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Dispels all magics which is effecting a monster.",""));
		break;
	case SPELL_CAST:
	{
		int m_idx;

		var_set_bool(res, FALSE);
		if (!target_set(TARGET_KILL)) return;
		m_idx = cave[target_row][target_col].m_idx;
		if (!m_idx) return;

		var_set_bool(res, TRUE);
		if (!player_has_los_bold(target_row, target_col)) return;
		if (!projectable(py, px, target_row, target_col)) return;
		dispel_monster_status(m_idx);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void dispel_undead_spell(int cmd, variant *res)
{
	int dice = 1;
	int sides = spell_power(p_ptr->lev * 5);
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Dispel Undead", "����ǥå��໶"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Damages all undead monsters in sight.", "�볦������ƤΥ���ǥåɤ˥��᡼����Ϳ���롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(dice, sides, 0));
		break;
	case SPELL_CAST:
		if(project_hack(GF_DISP_UNDEAD, damroll(dice, sides)))
			chg_virtue(V_UNLIFE, -2);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void dominate_living_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Dominate a Living Thing", "��ʪ����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_ball_hide(GF_CONTROL_LIVING, dir, p_ptr->lev, 0);
		var_set_bool(res, TRUE);
		break;
	}
	case SPELL_COST_EXTRA:
		var_set_int(res, (p_ptr->lev+3)/4);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void dominate_living_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Dominate Living Things", "������ʪ����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_CAST:
		project_hack(GF_CONTROL_LIVING, p_ptr->lev);
		var_set_bool(res, TRUE);
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, (p_ptr->lev+20)/2);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void double_magic_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Double Magic", "Ϣ³��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (!can_do_cmd_cast()) return;
		handle_stuff();
		do_cmd_cast();
		handle_stuff();
		if (!p_ptr->paralyzed && can_do_cmd_cast())
			do_cmd_cast();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void draconian_breath_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Draconian Breath", "�֥쥹"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires an elemental, or perhaps special, breath at your foes.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev * 2)));
		break;
	case SPELL_CAST:
	{/* Sorry ... I made no effort to clean this up :( */
		int plev = p_ptr->lev;
		int dir = 0;
		int  Type = (one_in_(3) ? GF_COLD : GF_FIRE);
#ifdef JP
		cptr Type_desc = ((Type == GF_COLD) ? "�䵤" : "��");
#else
		cptr Type_desc = ((Type == GF_COLD) ? "cold" : "fire");
#endif

		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		if (randint1(100) < plev)
		{
			switch (p_ptr->pclass)
			{
				case CLASS_WARRIOR:
				case CLASS_MAULER:
				case CLASS_BERSERKER:
				case CLASS_RANGER:
				case CLASS_TOURIST:
				case CLASS_IMITATOR:
				case CLASS_ARCHER:
				case CLASS_SMITH:
					if (one_in_(3))
					{
						Type = GF_MISSILE;
#ifdef JP
						Type_desc = "�������";
#else
						Type_desc = "the elements";
#endif
					}
					else
					{
						Type = GF_SHARDS;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "shards";
#endif
					}
					break;
				case CLASS_MAGE:
				case CLASS_NECROMANCER:
				case CLASS_BLOOD_MAGE:
				case CLASS_WARRIOR_MAGE:
				case CLASS_HIGH_MAGE:
				case CLASS_SORCERER:
				case CLASS_MAGIC_EATER:
				case CLASS_RED_MAGE:
				case CLASS_BLUE_MAGE:
				case CLASS_MIRROR_MASTER:
					if (one_in_(3))
					{
						Type = GF_MANA;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "mana";
#endif
					}
					else
					{
						Type = GF_DISENCHANT;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "disenchantment";
#endif
					}
					break;
				case CLASS_CHAOS_WARRIOR:
					if (!one_in_(3))
					{
						Type = GF_CONFUSION;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "confusion";
#endif
					}
					else
					{
						Type = GF_CHAOS;
#ifdef JP
						Type_desc = "������";
#else
						Type_desc = "chaos";
#endif
					}
					break;
				case CLASS_MONK:
				case CLASS_SAMURAI:
				case CLASS_FORCETRAINER:
					if (!one_in_(3))
					{
						Type = GF_CONFUSION;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "confusion";
#endif
					}
					else
					{
						Type = GF_SOUND;
#ifdef JP
						Type_desc = "�첻";
#else
						Type_desc = "sound";
#endif
					}
					break;
				case CLASS_MINDCRAFTER:
				case CLASS_PSION:
					if (!one_in_(3))
					{
						Type = GF_CONFUSION;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "confusion";
#endif
					}
					else
					{
						Type = GF_PSI;
#ifdef JP
						Type_desc = "�������ͥ륮��";
#else
						Type_desc = "mental energy";
#endif
					}
					break;
				case CLASS_PRIEST:
				case CLASS_PALADIN:
					if (one_in_(3))
					{
						Type = GF_HELL_FIRE;
#ifdef JP
						Type_desc = "�Ϲ��ι��";
#else
						Type_desc = "hellfire";
#endif
					}
					else
					{
						Type = GF_HOLY_FIRE;
#ifdef JP
						Type_desc = "���ʤ��";
#else
						Type_desc = "holy fire";
#endif
					}
					break;
				case CLASS_ROGUE:
				case CLASS_SCOUT:
				case CLASS_NINJA:
					if (one_in_(3))
					{
						Type = GF_DARK;
#ifdef JP
						Type_desc = "�Ź�";
#else
						Type_desc = "darkness";
#endif
					}
					else
					{
						Type = GF_POIS;
#ifdef JP
						Type_desc = "��";
#else
						Type_desc = "poison";
#endif
					}
					break;
				case CLASS_BARD:
					if (!one_in_(3))
					{
						Type = GF_SOUND;
#ifdef JP
						Type_desc = "�첻";
#else
						Type_desc = "sound";
#endif
					}
					else
					{
						Type = GF_CONFUSION;
#ifdef JP
						Type_desc = "����";
#else
						Type_desc = "confusion";
#endif
					}
					break;
			}
		}

		stop_mouth();

#ifdef JP
		msg_format("���ʤ���%s�Υ֥쥹���Ǥ�����", Type_desc);
#else
		msg_format("You breathe %s.", Type_desc);
#endif

		fire_ball(Type, dir, spell_power(plev * 2),
			-(plev / 15) - 1);

		var_set_bool(res, TRUE);
		break;
	}
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev/3);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void earthquake_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Earthquake", "�Ͽ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "The walls will tremble and the gound will shake.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to wreck the dungeon.", "���󥸥����˲�����ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to wreck the dungeon.", "���󥸥������ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can bring down the dungeon around your ears.", "���ʤ��ϼ��ϤΥ��󥸥������������뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		earthquake(py, px, 10);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_earthquake(void) { return cast_spell(earthquake_spell); }

void eat_magic_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Eat Magic", "���Ͽ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Consumes magical devices to regain spell points.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your magic items look delicious.", "��ˡ�Υ����ƥब��̣�����˸����롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your magic items no longer look delicious.", "��ˡ�Υ����ƥ�Ϥ⤦��̣�������˸����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can consume magic energy for your own use.", "���ʤ�����ˡ�Υ��ͥ륮����ʬ��ʪ�Ȥ��ƻ��ѤǤ��롣"));
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (eat_magic(p_ptr->lev * 2))
			var_set_bool(res, TRUE);
		break;
	case SPELL_FAIL_MIN:
		var_set_int(res, 11);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_eat_magic(void) { return cast_spell(eat_magic_spell); }

void eat_rock_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Eat Rock", "�俩��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Consumes nearby rock.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("The walls look delicious.", "�ɤ���̣�������˸����롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("The walls look unappetizing.", "�ɤ���̣�������˸����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can consume solid rock.", "���ʤ��ϹŤ���򿩤٤뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int x, y;
		cave_type *c_ptr;
		feature_type *f_ptr, *mimic_f_ptr;
		int dir = 0;

		var_set_bool(res, FALSE);

		if (!get_rep_dir2(&dir)) break;
		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];
		f_ptr = &f_info[c_ptr->feat];
		mimic_f_ptr = &f_info[get_feat_mimic(c_ptr)];

		stop_mouth();

		if (!have_flag(mimic_f_ptr->flags, FF_HURT_ROCK))
		{
			msg_print(T("You cannot eat this feature.", "�����Ϸ��Ͽ��٤��ʤ���"));
			break;
		}
		else if (have_flag(f_ptr->flags, FF_PERMANENT))
		{
			msg_format(T("Ouch!  This %s is harder than your teeth!", "���Ƥá�����%s�Ϥ��ʤ��λ����Ť���"), 
				f_name + mimic_f_ptr->name);

			break;
		}
		else if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			msg_print(T("There's something in the way!", "���������⤷�Ƥ��ޤ���"));
			if (!m_ptr->ml || !is_pet(m_ptr)) py_attack(y, x, 0);
			break;
		}
		else if (have_flag(f_ptr->flags, FF_TREE))
		{
			msg_print(T("You don't like the woody taste!", "�ڤ�̣�Ϲ�������ʤ���"));
			break;
		}
		else if (have_flag(f_ptr->flags, FF_GLASS))
		{
			msg_print(T("You don't like the glassy taste!", "���饹��̣�Ϲ�������ʤ���"));
			break;
		}
		else if (have_flag(f_ptr->flags, FF_DOOR) || have_flag(f_ptr->flags, FF_CAN_DIG))
		{
			set_food(p_ptr->food + 3000);
		}
		else if (have_flag(f_ptr->flags, FF_MAY_HAVE_GOLD) || have_flag(f_ptr->flags, FF_HAS_GOLD))
		{
			set_food(p_ptr->food + 5000);
		}
		else
		{
			msg_format(T("This %s is very filling!", "����%s�ϤȤƤ⤪��������"), 
				f_name + mimic_f_ptr->name);
			set_food(p_ptr->food + 10000);
		}

		/* Destroy the wall */
		cave_alter_feat(y, x, FF_HURT_ROCK);

		/* Move the player */
		move_player_effect(y, x, MPE_DONT_PICKUP);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_eat_rock(void) { return cast_spell(eat_rock_spell); }

void evocation_spell(int cmd, variant *res)
{
	int power = spell_power(p_ptr->lev * 4);
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Evocation", "����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Dispels, scares and banishes all monsters in view.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, power));
		break;
	case SPELL_CAST:
		dispel_monsters(power);
		turn_monsters(power);
		banish_monsters(power);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void enchantment_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Enchantment", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to enchant a weapon, ammo or armor.");
		break;
	case SPELL_CAST:
	{
		int         item;
		bool        okay = FALSE;
		object_type *o_ptr;
		char        o_name[MAX_NLEN];

		var_set_bool(res, FALSE);

		item_tester_hook = object_is_weapon_armour_ammo;
		item_tester_no_ryoute = TRUE;

		if (!get_item(&item, "Enchant which item? ", "You have nothing to enchant.", (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

		if (item >= 0)
			o_ptr = &inventory[item];
		else
			o_ptr = &o_list[0 - item];

		object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

		if (object_is_weapon_ammo(o_ptr))
		{
			if (enchant(o_ptr, randint0(4) + 1, ENCH_TOHIT)) okay = TRUE;
			if (enchant(o_ptr, randint0(4) + 1, ENCH_TODAM)) okay = TRUE;
		}
		else
		{
			if (enchant(o_ptr, randint0(3) + 2, ENCH_TOAC)) okay = TRUE;			
		}
			

		msg_format("%s %s glow%s brightly!",
			   ((item >= 0) ? "Your" : "The"), o_name,
			   ((o_ptr->number > 1) ? "" : "s"));

		if (!okay)
		{
			if (flush_failure) flush();
			msg_print("The enchantment failed.");
			if (one_in_(3)) chg_virtue(V_ENCHANT, -1);
		}
		else
			chg_virtue(V_ENCHANT, 1);

		calc_android_exp();
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_enchantment(void) { return cast_spell(enchantment_spell); }

void enslave_undead_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Enslave Undead", "����ǥåɽ�°"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to enslave an undead monster.", "����ǥå�1�Τ�̥λ���롣�񹳤�����̵����"));
		break;
	case SPELL_CAST:
	{
		int power, dir;
		if (p_ptr->pclass == CLASS_NECROMANCER)
			power = spell_power(p_ptr->lev*3);
		else
			power = spell_power(p_ptr->lev);

		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		control_one_undead(dir, power);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void explosive_rune_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Explosive Rune", "��ȯ�Υ롼��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Sets a rune which will explode on a passing monster.");
		break;
	case SPELL_CAST:
		msg_print(T("You carefully set an explosive rune...", "��ȯ�Υ롼��򿵽Ť˻ųݤ���..."));
		explosive_rune();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void eye_for_an_eye_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("An Eye for an Eye", "�ܤˤ��ܤ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives special aura for a while. When you are attacked by a monster, the monster are injured with same amount of damage as you take.", "������֡���ʬ�����᡼����������Ȥ��˹����Ԥä���󥹥������Ф���Ʊ���Υ��᡼����Ϳ���롣"));
		break;
	case SPELL_CAST:
		set_tim_eyeeye(spell_power(randint1(10) + 10), FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void force_branding_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Force Branding", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Temporarily brands your weapon with force.", ""));
		break;
	case SPELL_CAST:
	{
		int base = spell_power(p_ptr->lev / 4);
		set_tim_force(base + randint1(base), FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void glyph_of_warding_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Glyph of Warding", "�볦�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.", "��ʬ�Τ��뾲�ξ�ˡ���󥹥������̤�ȴ�����꾤�����줿�ꤹ�뤳�Ȥ��Ǥ��ʤ��ʤ�롼���������"));
		break;
	case SPELL_CAST:
		warding_glyph();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void grow_mold_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Grow Mold", "����ȯ��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Surrounds yourself with moldy things.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel a sudden affinity for mold.", "�������Ӥ˿Ƥ��ߤ�Ф�����"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel a sudden dislike for mold.", "�������Ӥ������ˤʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can cause mold to grow near you.", "���ʤ��ϼ��Ϥ˥��Υ������䤹���Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int i;
		for (i = 0; i < 8; i++)
		{
			summon_specific(-1, py, px, p_ptr->lev, SUMMON_BIZARRE1, PM_FORCE_PET);
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_grow_mold(void) { return cast_spell(grow_mold_spell); }

