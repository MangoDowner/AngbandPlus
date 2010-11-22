#include "angband.h"

/*
void foo_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "");
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_INFO:
		var_set_string(res, "");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("", ""));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("", ""));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_CAST:
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_foo(void) { return cast_spell(foo_spell); }
*/

void berserk_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Berserk");
		break;
	case SPELL_DESC:
		var_set_string(res, "Enter a berserk frenzy, gaining great combat bonuses, but losing the ability to think clearly.");
		break;
	case SPELL_CAST:
		msg_print("Raaagh!  You feel like hitting something.");
		set_afraid(0, TRUE);
		set_shero(10 + randint1(p_ptr->lev), FALSE);
		hp_player(30);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_berserk(void) { return cast_spell(berserk_spell); }

void breathe_fire_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Breathe Fire", "��Υ֥쥹"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Breathes Fire at your opponent.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to breathe fire.", "�Ф��Ǥ�ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to breathe fire.", "��Υ֥쥹���Ǥ�ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can breathe fire (dam lvl * 2).", "���ʤ��ϱ�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(���᡼�� ��٥�X2)"));
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, 2 * p_ptr->lev));
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev);
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			racial_stop_mouth();
			msg_print(T("You breathe fire...", "���ʤ��ϲб�Υ֥쥹���Ǥ���..."));
			fire_ball(GF_FIRE, dir, 2 * p_ptr->lev, 1 + (p_ptr->lev / 20));
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_breathe_fire(void) { return cast_spell(breathe_fire_spell); }

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
	case SPELL_GAIN_MUT:
		msg_print(T("You smell filthy monsters.", "��󥹥����ν������̤�ʬ������褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer smell filthy monsters.", "�Է�ʥ�󥹥����ν������̤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can smell nearby monsters.", "���ʤ��϶᤯�Υ�󥹥�����¸�ߤ򤫤�ʬ���뤳�Ȥ��Ǥ��롣"));
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
	case SPELL_GAIN_MUT:
		msg_print(T("You smell a metallic odor.", "��°���������̤�ʬ������褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer smell a metallic odor.", "��°�ν������̤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can smell nearby precious metal.", "���ʤ��϶᤯�ˤ��뵮��°�򤫤�ʬ���뤳�Ȥ��Ǥ��롣"));
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


void hypnotic_gaze_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Hypnotic Gaze");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempt to charm a monster.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your eyes look mesmerizing...", "��̲���ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your eyes look uninteresting.", "���ʤ����ܤϤĤޤ�ʤ��ܤˤʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your gaze is hypnotic.", "���ʤ����ˤߤϺ�̲���̤��ġ�"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			msg_print(T("Your eyes look mesmerizing...", "���ʤ����ܤϸ���Ū�ˤʤä�..."));
			charm_monster(dir, p_ptr->lev);
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_hypnotic_gaze(void) { return cast_spell(hypnotic_gaze_spell); }

void light_area_spell(int cmd, variant *res)
{
	int dice = 2;
	int sides = spell_power(p_ptr->lev / 2);
	int rad = spell_power(p_ptr->lev / 10 + 1);

	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Light Area");
		break;
	case SPELL_DESC:
		var_set_string(res, "Lights up nearby area and the inside of a room permanently.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(dice, sides, 0));
		break;
	case SPELL_CAST:
		lite_area(spell_power(damroll(dice, sides)), rad);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_light_area(void) { return cast_spell(light_area_spell); }

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

void phase_door_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Phase Door");
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
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_phase_door(void) { return cast_spell(phase_door_spell); }

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

void spit_acid_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Spit Acid", "������"));
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 25)
			var_set_string(res, "Spits a bolt of acid.");
		else
			var_set_string(res, "Spits a ball of acid.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to spit acid.", "�����Ǥ�ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to spit acid.", "����᤭������ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can spit acid (dam lvl).", "���ʤ��ϻ���᤭�����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X1)"));
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, p_ptr->lev));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			racial_stop_mouth();
			msg_print(T("You spit acid...", "�����Ǥ�������..."));
			if (p_ptr->lev < 25) fire_bolt(GF_ACID, dir, p_ptr->lev);
			else fire_ball(GF_ACID, dir, p_ptr->lev, 2);
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_spit_acid(void) { return cast_spell(spit_acid_spell); }

void summon_tree_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Summon Tree");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to summon a tree.");
		break;
	case SPELL_CAST:
	{
		int attempts = 0;
		int x, y, dir;

		for (;;)
		{
			if (attempts > 4)
			{
				msg_print("No trees arrive.");
				break;
			}

			dir = randint0(9);
			if (dir == 5) continue;

			attempts++;
			y = py + ddy[dir];
			x = px + ddx[dir];

			if (!in_bounds(y, x)) continue;
			if (!cave_naked_bold(y, x)) continue;
			if (player_bold(y, x)) continue;

			cave_set_feat(y, x, feat_tree);
			break;
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_summon_tree(void) { return cast_spell(summon_tree_spell); }

void telekinesis_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Telekinesis", "ǰư��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to fetch a distant object.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to move objects telekinetically.", "ʪ�Τ�ǰư�Ϥ�ư����ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to move objects telekinetically.", "ǰư�Ϥ�ʪ��ư����ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are telekinetic.", "���ʤ���ǰư�Ϥ��äƤ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			fetch(dir, p_ptr->lev * 10, TRUE);
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_telekinesis(void) { return cast_spell(telekinesis_spell); }

void teleport_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Teleport", "�ƥ�ݡ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Escape to a distant location.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the power of teleportation at will.", "��ʬ�ΰջפǥƥ�ݡ��Ȥ���ǽ�Ϥ�������"));
		mut_lose(MUT_TELEPORT_RND);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the power of teleportation at will.", "��ʬ�ΰջפǥƥ�ݡ��Ȥ���ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can teleport at will.", "���ʤ��ϼ�ʬ�ΰջפǥƥ�ݡ��ȤǤ��롣"));
		break;
	case SPELL_CAST:
		teleport_player(10 + 4 * p_ptr->lev, 0);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_teleport(void) { return cast_spell(teleport_spell); }

void vampirism_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Vampiric Drain", "�۷�ɥ쥤��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Suck blood from an adjacent monster, gaining hp in the process.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, p_ptr->lev * 2));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become vampiric.", "��̿�Ϥ�ۼ��Ǥ���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer vampiric.", "�۷��ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can drain life from a foe like a vampire.", "���ʤ��ϵ۷쵴�Τ褦��Ũ������̿�Ϥ�ۼ����뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int x, y, dummy;
		cave_type *c_ptr;
		int dir = 0;

		var_set_bool(res, FALSE);

		/* Only works on adjacent monsters */
		if (!get_rep_dir2(&dir)) break;

		var_set_bool(res, TRUE);

		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];

		racial_stop_mouth();

		if (!(c_ptr->m_idx))
		{
			msg_print(T("You bite into thin air!", "����ʤ����˳��ߤĤ�����"));
			break;
		}

		msg_print(T("You grin and bare your fangs...", "���ʤ��ϥ˥��Ȥ��Ʋ��त��..."));
		dummy = p_ptr->lev * 2;

		if (drain_life(dir, dummy))
		{
			/* No heal if we are "full" */
			if (p_ptr->food < PY_FOOD_FULL)
				hp_player(dummy);
			else
				msg_print(T("You were not hungry.", "���ʤ��϶�ʢ�ǤϤ���ޤ���"));

			/* Gain nutritional sustenance: 150/hp drained
			 * A Food ration gives 5000 food points (by contrast)
			 * Don't ever get more than "Full" this way
			 * But if we ARE Gorged,  it won't cure us 
			 */
			dummy = p_ptr->food + MIN(5000, 100 * dummy);
			if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
				set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
		}
		else
			msg_print(T("Yechh. That tastes foul.", "�������Ҥɤ�̣����"));

		break;
	}
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev / 3);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_vampirism(void) { return cast_spell(vampirism_spell); }
