#include "angband.h"

void magic_mapping_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Magic Mapping");
		break;
	case SPELL_DESC:
		var_set_string(res, "Maps the dungeon in your vicinity.");
		break;
	case SPELL_CAST:
		map_area(DETECT_RAD_MAP);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_magic_mapping(void) { return cast_spell(magic_mapping_spell); }

void magic_missile_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Magic Missile", "�ޥ��å����ߥ�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a weak bolt of unresistable magic.", "�夤��ˡ��������ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(spell_power(3 + ((p_ptr->lev - 1) / 5)), 4, 0));
		break;
	case SPELL_CAST:
	{
		int dice = spell_power(3 + ((p_ptr->lev - 1) / 5));
		int sides = 4;
		int dir = 0;

		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_bolt_or_beam(beam_chance() - 10, GF_MISSILE, dir, damroll(dice, sides));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_magic_missile(void) { return cast_spell(magic_missile_spell); }

void mana_branding_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mana Branding", "��ˡ��"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Makes current weapon some elemental branded. You must wield weapons.", "������֡������䵤���ꡢ�ŷ⡢�����ǤΤ����줫��°����Ĥ��롣��������ʤ��ȻȤ��ʤ���"));
		break;
	case SPELL_CAST:
		var_set_bool(res, choose_ele_attack());
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void mana_bolt_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mana Bolt I", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a bolt of pure mana.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(1, spell_power(p_ptr->lev * 7 / 2), spell_power(50)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You cast a mana bolt.", "���Ϥ���μ�ʸ�򾧤�����"));
		fire_bolt(GF_MANA, dir, spell_power(randint1(p_ptr->lev * 7 / 2) + 50));

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void mana_bolt_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mana Bolt II", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a powerful bolt of pure mana.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(1, spell_power(p_ptr->lev * 7), spell_power(100)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You cast a mana bolt.", "���Ϥ���μ�ʸ�򾧤�����"));
		fire_bolt(GF_MANA, dir, spell_power(randint1(p_ptr->lev * 7) + 100));

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void mana_storm_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mana Storm I", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a large ball of pure mana.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(10, spell_power(10), spell_power(p_ptr->lev * 5)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You cast a mana storm.", "���Ϥ���μ�ʸ��ǰ������"));
		fire_ball(GF_MANA, dir, spell_power(p_ptr->lev * 5 + damroll(10, 10)), 4);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void mana_storm_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mana Storm II", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a large ball of pure mana.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(10, spell_power(10), spell_power(p_ptr->lev * 8 + 50)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You cast a mana storm.", "���Ϥ���μ�ʸ��ǰ������"));
		fire_ball(GF_MANA, dir, spell_power(p_ptr->lev * 8 + 50 + damroll(10, 10)), 4);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void massacre_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Massacre", "������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attack all adjacent monsters in a fit of wild, uncontrollable fury.", ""));
		break;
	case SPELL_CAST:
	{
		int              dir, x, y;
		cave_type       *c_ptr;
		monster_type    *m_ptr;

		for (dir = 0; dir < 8; dir++)
		{
			y = py + ddy_ddd[dir];
			x = px + ddx_ddd[dir];
			c_ptr = &cave[y][x];

			m_ptr = &m_list[c_ptr->m_idx];

			if (c_ptr->m_idx && (m_ptr->ml || cave_have_flag_bold(y, x, FF_PROJECT)))
				py_attack(y, x, 0);
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void mind_blast_spell(int cmd, variant *res)
{
	int dice = 3 + (p_ptr->lev - 1)/5;
	int sides = 3;
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mind Blast", "��������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempt to blast your opponent with psionic energy.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(spell_power(dice), sides, 0));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the power of Mind Blast.", "���������ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the power of Mind Blast.", "���������ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can Mind Blast your enemies.", "���ʤ���Ũ����������Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			msg_print(T("You concentrate...", "���椷�Ƥ���..."));
			fire_bolt(GF_PSI, dir, spell_power(damroll(dice, sides)));
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_mind_blast(void) { return cast_spell(mind_blast_spell); }

void orb_of_entropy_spell(int cmd, variant *res)
{
	int base;

	if (p_ptr->pclass == CLASS_MAGE || p_ptr->pclass == CLASS_BLOOD_MAGE || p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER)
		base = p_ptr->lev + p_ptr->lev / 2;
	else
		base = p_ptr->lev + p_ptr->lev / 4;

	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Orb of Entropy", "����ȥ�ԡ��ε�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a ball which damages living monsters.", "��̿�Τ���Ԥ˸��̤Τ��������ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(3, spell_power(6), spell_power(base)));
		break;
	case SPELL_CAST:
	{
		int dir;
		int rad = (p_ptr->lev < 30) ? 2 : 3;

		var_set_bool(res, FALSE);
		
		if (!get_aim_dir(&dir)) return;
		fire_ball(GF_OLD_DRAIN, dir, spell_power(damroll(3, 6) + base), rad);
		
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void panic_hit_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Panic Hit", "�ҥåȡ���������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack an adjacent monster and attempt a getaway.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You suddenly understand how thieves feel.", "������ť���ε�ʬ��ʬ����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer feel jumpy.", "����������ķ�٤뵤ʬ���ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can run for your life after hitting something.", "���ʤ��Ϲ��⤷����Ȥ��뤿��ƨ���뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		int x, y;

		var_set_bool(res, FALSE);
		if (!get_rep_dir2(&dir)) break;
		y = py + ddy[dir];
		x = px + ddx[dir];
		if (cave[y][x].m_idx)
		{
			py_attack(y, x, 0);
			if (randint0(p_ptr->skills.dis) < 7)
				msg_print(T("You failed to teleport.", "���ޤ�ƨ�����ʤ��ä���"));
			else 
				teleport_player(30, 0L);
	
			var_set_bool(res, TRUE);
		}
		else
		{
			msg_print(T("You don't see any monster in this direction", "���������ˤϥ�󥹥����Ϥ��ޤ���"));
			msg_print(NULL);
			/* No Charge for this Action ... */
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_panic_hit(void) { return cast_spell(panic_hit_spell); }

void pattern_mindwalk_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Pattern Mindwalking", "�ѥ����󡦥�������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Walk the pattern in your mind.  Restores life and stats.");
		break;
	case SPELL_CAST:
		msg_print(T("You picture the Pattern in your mind and walk it...", "���ʤ��ϡ֥ѥ�����פ򿴤������Ƥ��ξ���⤤��..."));

		set_poisoned(0, TRUE);
		set_image(0, TRUE);
		set_stun(0, TRUE);
		set_cut(0, TRUE);
		set_blind(0, TRUE);
		set_afraid(0, TRUE);
		do_res_stat(A_STR);
		do_res_stat(A_INT);
		do_res_stat(A_WIS);
		do_res_stat(A_DEX);
		do_res_stat(A_CON);
		do_res_stat(A_CHR);
		restore_level();
		
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void perception_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Perception", "�ռ�"));
		break;
	default:
		identify_spell(cmd, res);
		break;
	}
}

void phase_door_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Phase Door", "���硼�ȡ��ƥ�ݡ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, "A short range teleport.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the power of minor teleportation.", "���Υ�ƥ�ݡ��Ȥ�ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the power of minor teleportation.", "���Υ�ƥ�ݡ��Ȥ�ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can teleport yourself short distances.", "���ʤ���û����Υ��ƥ�ݡ��ȤǤ��롣"));
		break;
	case SPELL_CAST:
		teleport_player(10, 0);
		var_set_bool(res, TRUE);
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
bool cast_phase_door(void) { return cast_spell(phase_door_spell); }

void poison_dart_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Poison Dart", "�ǤΥ�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Fires a poison dart at a single foe.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You throw a dart of poison.", "�ǤΥ����Ĥ��ꤲ����"));
		fire_bolt(GF_POIS, dir, p_ptr->lev);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void polish_shield_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Polish Shield");
		break;
	case SPELL_DESC:
		var_set_string(res, "Makes your shield reflect missiles and bolt spells.");
		break;
	case SPELL_CAST:
		polish_shield();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_polish_shield(void) {	return cast_spell(polish_shield_spell); }

void polymorph_colossus_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Polymorph Colossus", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Mimic a Colossus for a while. Loses abilities of original race and gets great abilities as a colossus.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(spell_power(15), spell_power(15)));
		break;
	case SPELL_CAST:
	{
		int base = spell_power(15);
		set_mimic(base + randint1(base), MIMIC_COLOSSUS, FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void polymorph_demon_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Polymorph Demon", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Mimic a demon for a while. Loses abilities of original race and gets abilities as a demon.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(spell_power(15), spell_power(15)));
		break;
	case SPELL_CAST:
	{
		int base = spell_power(10 + p_ptr->lev / 2);
		set_mimic(base + randint1(base), MIMIC_DEMON, FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void polymorph_demonlord_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Polymorph Demonlord", "�Ⲧ�Ѳ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Mimic a demon lord for a while. Loses abilities of original race and gets great abilities as a demon lord. Even hard walls can't stop your walking.", "����β����Ѳ����롣�Ѳ����Ƥ���֤�����μ�²��ǽ�Ϥ򼺤�������˰���β��Ȥ��Ƥ�ǽ�Ϥ������ɤ��˲����ʤ����⤯��"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(spell_power(15), spell_power(15)));
		break;
	case SPELL_CAST:
	{
		int base = spell_power(15);
		set_mimic(base + randint1(base), MIMIC_DEMON_LORD, FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void polymorph_self_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Polymorph", "�ѿ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Mutates yourself.  This can be dangerous!");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your body seems mutable.", "�Τ��Ѱۤ��䤹���ʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your body seems stable.", "���ʤ����Τϰ��ꤷ���褦�˸����롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can polymorph yourself at will.", "���ʤ��ϼ�ʬ�ΰջ֤��Ѳ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (get_check(T("You will polymorph yourself. Are you sure? ", "�ѿȤ��ޤ���������Ǥ�����")))
		{
			do_poly_self();
			var_set_bool(res, TRUE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_polymorph_self(void) { return cast_spell(polymorph_self_spell); }

void power_throw_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Throw Object", "�����ƥ��ꤲ"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Hurl an object with great force.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your throwing arm feels much stronger.", "���ʤ���ʪ���ꤲ���Ϥ��ʤ궯���ʤä��������롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your throwing arm feels much weaker.", "ʪ���ꤲ��꤬�夯�ʤä��������롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can hurl objects with great force.", "���ʤ��ϥ����ƥ���϶����ꤲ�뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev);
		break;
	case SPELL_CAST:
		var_set_bool(res, do_cmd_throw_aux(2 + p_ptr->lev / 40, FALSE, 0));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_power_throw(void) { return cast_spell(power_throw_spell); }

void probing_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Probe Monster", "��󥹥���Ĵ��"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Determines the abilities, strengths and weaknesses of nearby monsters.", ""));
		break;
	case SPELL_CAST:
		probing();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_probing(void) { return cast_spell(probing_spell); }

void protection_from_evil_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Protection from Evil");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to prevent evil monsters from attacking you.  When a weak evil monster melees you, it may be repelled by the forces of good.");
		break;
	case SPELL_CAST:
		set_protevil(randint1(3 * p_ptr->lev) + 25, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_protection_from_evil(void) { return cast_spell(protection_from_evil_spell); }

void radiation_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Emit Radiation", "����ǽ"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Generates a huge ball of radiation centered on you.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev)));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You start emitting hard radiation.", "���ʤ��϶�����������ȯ�����Ϥ᤿��"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You stop emitting hard radiation.", "���ʤ�������ǽ��ȯ�����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can emit hard radiation at will.", "���ʤ��ϼ�ʬ�ΰջפ�����ǽ��ȯ�����뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		msg_print(T("Radiation flows from your body!", "�Τ�������ǽ��ȯ��������"));
		fire_ball(GF_NUKE, 0, spell_power(p_ptr->lev * 2), 3 + (p_ptr->lev / 20));
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_radiation(void) { return cast_spell(radiation_spell); }

void ray_of_sunlight_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Ray of Sunlight", "���۸���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a beam of light which damages to light-sensitive monsters.", "���������ġ�����������󥹥����˸��̤����롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(6, 8, 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		lite_line(dir);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void recall_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Recall", "����"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Travel back and forth between the town and the dungeon.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel briefly homesick, but it passes.", "���������ۡ��ॷ�å��ˤʤä���������ľ�ä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel briefly homesick.", "�����δ֥ۡ��ॷ�å��ˤʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can travel between town and the depths.", "���ʤ��ϳ��ȥ��󥸥��δ֤�Ԥ��褹�뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (word_of_recall())
			var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_recall(void) { return cast_spell(recall_spell); }

void recharging_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Recharging");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to recharge staffs, wands or rods.  Items may be destroyed on failure.");
		break;
	case SPELL_CAST:
		var_set_bool(res, recharge(4 * p_ptr->lev));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_recharging(void) { return cast_spell(recharging_spell); }

void remove_curse_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Remove Curse");
		break;
	case SPELL_DESC:
		var_set_string(res, "Uncurses an item so that you may remove it.");
		break;
	case SPELL_CAST:
		if (remove_curse())
			msg_print(T("You feel as if someone is watching over you.", "ï���˸�����Ƥ���褦�ʵ������롣"));
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_remove_curse_I(void) { return cast_spell(remove_curse_I_spell); }

void remove_curse_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "*Remove Curse*");
		break;
	case SPELL_DESC:
		var_set_string(res, "Uncurses an item so that you may remove it.  Even heavily cursed items can be removed.");
		break;
	case SPELL_CAST:
		if (remove_all_curse())
			msg_print(T("You feel as if someone is watching over you.", "ï���˸�����Ƥ���褦�ʵ������롣"));
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_remove_curse_II(void) { return cast_spell(remove_curse_II_spell); }

void remove_fear_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Remove Fear", "���ݽ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_CAST:
		set_afraid(0, TRUE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_remove_fear(void) { return cast_spell(remove_fear_spell); }

void resistance_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Resistance", "������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives resistance to fire, cold, electricity, acid and poison for a while. These resistances can be added to which from equipment for more powerful resistances.", "������֡������ŷ⡢�ꡢ�䵤���Ǥ��Ф������������롣�����ˤ�����������Ѥ��롣"));
		break;
	case SPELL_CAST:
	{
		int base = spell_power(20);

		set_oppose_acid(randint1(base) + base, FALSE);
		set_oppose_elec(randint1(base) + base, FALSE);
		set_oppose_fire(randint1(base) + base, FALSE);
		set_oppose_cold(randint1(base) + base, FALSE);
		set_oppose_pois(randint1(base) + base, FALSE);
	
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void resist_elements_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Resist Elements", "�����������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Protect yourself from the ravages of the elements.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel like you can protect yourself.", "���ʤ��ϼ�ʬ���Ȥ���뵤�����롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel like you might be vulnerable.", "���Ĥ��פ��ʤä��������롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can harden yourself to the ravages of the elements.", "���ʤ��ϸ��Ǥι�����Ф��ƿȤ�Ť����뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_COST_EXTRA:
	{
		int n = 0;
		if (p_ptr->lev >= 20)
			n += 5;
		if (p_ptr->lev >= 30)
			n += 5;
		if (p_ptr->lev >= 40)
			n += 5;
		if (p_ptr->lev >= 50)
			n += 5;
		var_set_int(res, n);
		break;
	}
	case SPELL_CAST:
	{
		int num = p_ptr->lev / 10;
		int dur = randint1(20) + 20;

		if (randint0(5) < num)
		{
			set_oppose_acid(dur, FALSE);
			num--;
		}
		if (randint0(4) < num)
		{
			set_oppose_elec(dur, FALSE);
			num--;
		}
		if (randint0(3) < num)
		{
			set_oppose_fire(dur, FALSE);
			num--;
		}
		if (randint0(2) < num)
		{
			set_oppose_cold(dur, FALSE);
			num--;
		}
		if (num)
		{
			set_oppose_pois(dur, FALSE);
			num--;
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_resist_elements(void) { return cast_spell(resist_elements_spell); }

void resist_environment_spell(int cmd, variant *res)
{
	int base = spell_power(20);
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Resist Environment", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, "Gives resistance to fire, cold and electricity for a while. These resistances can be added to which from equipment for more powerful resistances.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(base, base));
		break;
	case SPELL_CAST:
		set_oppose_cold(randint1(base) + base, FALSE);
		set_oppose_fire(randint1(base) + base, FALSE);
		set_oppose_elec(randint1(base) + base, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void resist_heat_cold_spell(int cmd, variant *res)
{
	int base = spell_power(20);
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Resist Heat and Cold", "��Ǯ�Ѵ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives resistance to fire and cold. These resistances can be added to which from equipment for more powerful resistances.", "������֡��б���䵤���Ф������������롣�����ˤ�����������Ѥ��롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(base, base));
		break;
	case SPELL_CAST:
		set_oppose_cold(randint1(base) + base, FALSE);
		set_oppose_fire(randint1(base) + base, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void resist_poison_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Resist Poison", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, "Provides temporary resistance to poison.");
		break;
	case SPELL_CAST:
		set_oppose_pois(randint1(20) + 20, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void restore_life_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Restore Life");
		break;
	case SPELL_DESC:
		var_set_string(res, "Regain all lost experience.");
		break;
	case SPELL_CAST:
		restore_level();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_restore_life(void) { return cast_spell(restore_life_spell); }

void rocket_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Magic Rocket I", "�ޥ��å������å�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a magic rocket.", "���åȤ�ȯ�ͤ��롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(120 + p_ptr->lev * 2)));
		break;	
	case SPELL_CAST:
	{
		int dir = 0;
		int dam = spell_power(120 + p_ptr->lev * 2);
		int rad = 2;

		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You launch a rocket!", "���å�ȯ�͡�"));
		fire_rocket(GF_ROCKET, dir, dam, rad);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void rocket_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Magic Rocket II", "�ޥ��å������å�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a magic rocket of unsurpassable fire power.", "���åȤ�ȯ�ͤ��롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(500)));
		break;	
	case SPELL_CAST:
	{
		int dir = 0;
		int dam = spell_power(500);
		int rad = 2;

		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		msg_print(T("You launch a rocket!", "���å�ȯ�͡�"));
		fire_rocket(GF_ROCKET, dir, dam, rad);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void rush_attack_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Rush Attack", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Charge a nearby monster and attack with your weapons.", ""));
		break;
	case SPELL_CAST:
		var_set_bool(res, rush_attack(NULL));
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

