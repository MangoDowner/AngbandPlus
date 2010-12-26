#include "angband.h"

void _precognition_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Precognition", "���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Detects visible monsters in your vicinity and more and more. Detects traps and "
							  "doors at level 5, invisible monsters at level 15, items at level 30. And magic "
							  "mapping at level 20. Lights and know the whole level at level 45. Gives telepathy "
							  "at level 25.",
							  "�᤯�����Ƥθ������󥹥������Τ��롣��٥�5���/�⡢15��Ʃ���ʥ�󥹥�����30�Ǻ����ȥ����ƥ���ΤǤ���褦�ˤʤ롣��٥�20�Ǽ��դ��Ϸ����Τ���45�Ǥ��γ����Τ�ʵפ˾Ȥ餷�����󥸥����Τ��٤ƤΥ����ƥ���Τ��롣��٥�25�ǰ�����֥ƥ�ѥ��������롣"));
		break;
	case SPELL_CAST:
	{
		int b = 0;
		if (p_ptr->lev > 44)
		{
			chg_virtue(V_KNOWLEDGE, 1);
			chg_virtue(V_ENLIGHTEN, 1);
			wiz_lite(p_ptr->tim_superstealth > 0);
		}
		else if (p_ptr->lev > 19)
			map_area(DETECT_RAD_MAP);

		if (p_ptr->lev < 30)
		{
			b = detect_monsters_normal(DETECT_RAD_DEFAULT);
			if (p_ptr->lev > 14) b |= detect_monsters_invis(DETECT_RAD_DEFAULT);
			if (p_ptr->lev > 4)  {
				b |= detect_traps(DETECT_RAD_DEFAULT, TRUE);
				b |= detect_doors(DETECT_RAD_DEFAULT);
			}
		}
		else
		{
			b = detect_all(DETECT_RAD_DEFAULT);
		}

		if ((p_ptr->lev > 24) && (p_ptr->lev < 40))
			set_tim_esp(p_ptr->lev, FALSE);

		if (!b) msg_print(T("You feel safe.", "�����ʵ������롣"));

		var_set_bool(res, TRUE);
		break;
	}
	case SPELL_COST_EXTRA:
	{
		int n = 0;

		/* Base is 1.  Lets give reasonable costs based on powers.  Note
		   some powers become obsolete, like Telepathy, Magic Mapping and Detect Items */
		if (p_ptr->lev >= 45)
			n += 39;
		else if (p_ptr->lev >= 30)
			n += 19;
		else if (p_ptr->lev >= 25)
			n += 14;
		else if (p_ptr->lev >= 20)
			n += 9;
		else if (p_ptr->lev >= 15)
			n += 4;
		else if (p_ptr->lev >= 5)
			n += 1;

		var_set_int(res, n);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _neural_blast_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Neural Blast", "���й���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a beam or ball which inflicts psionic damage.", "��������Υӡ���ޤ��ϵ�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(spell_power(3 + ((p_ptr->lev - 1) / 4)), (3 + p_ptr->lev / 15), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		int dice = 3 + ((p_ptr->lev - 1) / 4);
		int sides = (3 + p_ptr->lev / 15);
		var_set_bool(res, FALSE);

		if (!get_aim_dir(&dir)) return;

		if (randint1(100) < p_ptr->lev * 2)
			fire_beam(GF_PSI, dir, spell_power(damroll(dice, sides)));
		else
			fire_ball(GF_PSI, dir, spell_power(damroll(dice, sides)), 0);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _minor_displacement_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		if (p_ptr->lev >= 45)
			var_set_string(res, T("Dimension Door", ""));
		else
			var_set_string(res, T("Minor Displacement", "�����ν֤�"));
		break;
	case SPELL_DESC:
		if (p_ptr->lev >= 45)
			var_set_string(res, T("Attempt to teleport to a specific location.", ""));
		else
			var_set_string(res, T("Teleport short distance.", "���Υ�Υƥ�ݡ��Ȥ򤹤롣"));
		break;
	case SPELL_CAST:
	{
		if (p_ptr->lev >= 45)
			var_set_bool(res, dimension_door());
		else
		{
			teleport_player(10, 0L);
			var_set_bool(res, TRUE);
		}
		break;
	}
	case SPELL_COST_EXTRA:
	{
		int n = 0;
		if (p_ptr->lev >= 45)
			n += 40;
		var_set_int(res, n);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _major_displacement_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Major Displacement", "�����θ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Teleport long distance.", "���Υ�Υƥ�ݡ��Ȥ򤹤롣"));
		break;
	case SPELL_CAST:
	{
		teleport_player(p_ptr->lev * 5, 0L);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _domination_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Domination", "��������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Stuns, confuses or scares a monster. Or attempts to charm all monsters in sight at level 30.",
							  "��٥�30̤���ǡ���󥹥�����ۯ۰�����𤫶��ݤ����������ġ���٥�30�ʾ�ǻ볦������ƤΥ�󥹥�����̥λ���롣�񹳤�����̵����"));
		break;
	case SPELL_CAST:
	{
		var_set_bool(res, FALSE);
		if (p_ptr->lev < 30)
		{
			int dir = 0;
			if (!get_aim_dir(&dir)) return;

			fire_ball(GF_DOMINATION, dir, spell_power(p_ptr->lev), 0);
		}
		else
		{
			charm_monsters(spell_power(p_ptr->lev * 2));
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _pulverise_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Pulverise", "ǰư�׷���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a ball which hurts monsters with telekinesis.", "�ƥ쥭�ͥ����ε�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(spell_power(8 + ((p_ptr->lev - 5) / 4)), 8, 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		int dice = 8 + ((p_ptr->lev - 5) / 4);
		int sides = 8;
		int rad = p_ptr->lev > 20 ? spell_power((p_ptr->lev - 20) / 8 + 1) : 0;

		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		fire_ball(GF_TELEKINESIS, dir, spell_power(damroll(dice, sides)), rad);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _character_armor_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Character Armour", "����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives stone skin and some resistance to elements for a while. The level "
							  "increased, the more number of resistances given.",
							  "������֡�AC��徺�����롣��٥뤬�夬��ȡ������ꡢ�䵤���ŷ⡢�Ǥ������������롣"));
		break;
	case SPELL_CAST:
	{
		set_shield(spell_power(p_ptr->lev), FALSE);
		if (p_ptr->lev > 14) set_oppose_acid(spell_power(p_ptr->lev), FALSE);
		if (p_ptr->lev > 19) set_oppose_fire(spell_power(p_ptr->lev), FALSE);
		if (p_ptr->lev > 24) set_oppose_cold(spell_power(p_ptr->lev), FALSE);
		if (p_ptr->lev > 29) set_oppose_elec(spell_power(p_ptr->lev), FALSE);
		if (p_ptr->lev > 34) set_oppose_pois(spell_power(p_ptr->lev), FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _psychometry_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Psychometry", "��������ȥ꡼"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Gives feeling of an item. Or identify an item at level 25.", "��٥�25̤���ǡ������ƥ��ʷ�ϵ����Τ롣��٥�25�ʾ�ǡ������ƥ����ꤹ�롣"));
		break;
	case SPELL_CAST:
	{
		if (p_ptr->lev < 25)
			var_set_bool(res, psychometry());
		else
			var_set_bool(res, ident_spell(FALSE));
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _mind_wave_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Mind Wave", "������ư"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Generate a ball centered on you which inflict monster with PSI damage. "
							  "Or inflict all monsters with PSI damage at level 25.",
							  "��٥�25̤���ǡ���ʬ���濴�Ȥ�����������ε��ȯ�������롣��٥�25�ʾ�ǡ��볦������ƤΥ�󥹥������Ф������������Ԥ���"));
		break;

	case SPELL_INFO:
		if (p_ptr->lev < 25)
			var_set_string(res, format("dam %d", spell_power(p_ptr->lev * 3 / 2)));
		else
			var_set_string(res, format("dam d%d", spell_power(p_ptr->lev * ((p_ptr->lev - 5) / 10 + 1))));
		break;
	case SPELL_CAST:
	{
		msg_print(T("Mind-warping forces emanate from your brain!", "������Ǳ���ʤ�����ư��ȯ����������"));

		if (p_ptr->lev < 25)
		{
			project(0, 2 + p_ptr->lev / 10, py, px,
						spell_power(p_ptr->lev * 3), GF_PSI, PROJECT_KILL, -1);
		}
		else
			mindblast_monsters(spell_power(randint1(p_ptr->lev * ((p_ptr->lev - 5) / 10 + 1))));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _adrenaline_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Adrenaline Channeling", "���ɥ�ʥ�󡦥ɡ��ԥ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Removes fear and stun. Gives heroism and speed. Heals HP a little unless "
							  "you already have heroism and temporary speed boost.",
							  "���ݤ�ۯ۰������������ҡ�����ʬ���Ĳ�®���֤Ǥʤ����HP�������������롣����ˡ�������֥ҡ�����ʬ�ˤʤꡢ��®���롣"));
		break;
	case SPELL_CAST:
	{
		int dur = spell_power(10 + randint1((p_ptr->lev * 3) / 2));
		bool heal = !IS_FAST() || !IS_HERO(); /* Prevent spamming this as a weak healing spell */

		set_afraid(0, TRUE);
		set_stun(0, TRUE);

		set_hero(dur, FALSE);
		set_fast(dur, FALSE);

		if (heal) /* Heal after granting Heroism to fill the +10 mhp */
			hp_player(p_ptr->lev);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _telekinesis_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Telekinesis", "�ƥ쥭�ͥ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Pulls a distant item close to you.", "�����ƥ��ʬ��­���ذ�ư�����롣"));		
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		fetch(dir, p_ptr->lev * 15, FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _psychic_drain_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Psychic Drain", "�������å����ɥ쥤��"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a ball which damages monsters and absorbs monsters' mind power. "
							  "Absorbing takes from 0 to 1.5 more turns.",
							  "��������ε�����ġ���󥹥�����̿�椹��ȡ�0��1.5��������񤹤롣�񹳤���ʤ���С�MP���������롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(spell_power(p_ptr->lev/2), 6, 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		int dam = spell_power(damroll(p_ptr->lev / 2, 6));
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;

		/* Only charge extra energy if the drain succeeded */
		if (fire_ball(GF_PSI_DRAIN, dir, dam, 0))
			p_ptr->energy_need += randint1(150);

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void psycho_spear_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Psycho-Spear", "���η�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a beam of pure energy which penetrate the invulnerability barrier.",
							  "̵�������ڤ��������ʥ��ͥ륮���Υӡ�������ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(1, spell_power(p_ptr->lev * 4), spell_power(p_ptr->lev * 4)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		fire_beam(GF_PSY_SPEAR, dir, spell_power(randint1(p_ptr->lev*4)+p_ptr->lev*4));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void _the_world_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("The World", "����������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Stops time. Consumes all of your SP. The more consumes SP, the longer duration of spell.",
							  "����ߤ�롣��MP����񤷡����񤷤�MP�˱�����Ĺ������ߤ�Ƥ����롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, format(T("%ld acts.", "��ư:%ld��"), (p_ptr->csp + 100-p_ptr->energy_need - 50)/100));
		break;
	case SPELL_CAST:
	{
		var_set_bool(res, FALSE);
		if (world_player)
		{
			msg_print(T("Time is already stopped.", "���˻��ϻߤޤäƤ��롣"));
			return;
		}

		world_player = TRUE;
		msg_print(T("You yell 'Time!'", "�ֻ��衪��"));
		msg_print(NULL);

		/* Note: We pay the casting cost up front these days.  So, add back the 150
		   to figure the starting sp, and then bash sp down to 0. We can't use the 
		   SPELL_COST_EXTRA mechanism here ... */
		p_ptr->energy_need -= 1000 + (100 + (p_ptr->csp + 150) - 50)*TURNS_PER_TICK/10;
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		p_ptr->redraw |= (PR_MAP);
		p_ptr->update |= (PU_MONSTERS);
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		handle_stuff();

		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}


/****************************************************************
 * Spell Table and Exports
 ****************************************************************/

#define MAX_MINDCRAFTER_SPELLS	14

static spell_info _spells[MAX_MINDCRAFTER_SPELLS] = 
{
    /*lvl cst fail spell */
    { 1,   1,  15, _precognition_spell},
    { 2,   1,  20, _neural_blast_spell},
    { 3,   2,  25, _minor_displacement_spell},
    { 7,   6,  35, _major_displacement_spell},
    { 9,   7,  50, _domination_spell},
    { 11,  7,  30, _pulverise_spell},
    { 13, 12,  50, _character_armor_spell},
    { 15, 12,  60, _psychometry_spell},
    { 18, 10,  45, _mind_wave_spell},
    { 23, 15,  50, _adrenaline_spell},
    { 26, 28,  60, _telekinesis_spell},
    { 28, 10,  40, _psychic_drain_spell},
    { 35, 35,  75, psycho_spear_spell},
    { 45,150,  85, _the_world_spell},
};

static int _get_spells(spell_info* spells, int max)
{
	int i;
	int ct = 0;
	int stat_idx = p_ptr->stat_ind[A_WIS];
	
	for (i = 0; i < MAX_MINDCRAFTER_SPELLS; i++)
	{
		spell_info *base = &_spells[i];
		if (ct >= max) break;
		if (base->level <= p_ptr->lev)
		{
			spell_info* current = &spells[ct];
			current->fn = base->fn;
			current->level = base->level;
			current->cost = base->cost;

			current->fail = calculate_fail_rate(base->level, base->fail, stat_idx);			
			ct++;
		}
	}
	return ct;
}

static int _get_powers(spell_info* spells, int max)
{
	int ct = 0;

	spell_info* spell = &spells[ct++];
	spell->level = 15;
	spell->cost = 0;
	spell->fail = calculate_fail_rate(spell->level, 30, p_ptr->stat_ind[A_WIS]);
	spell->fn = clear_mind_spell;

	return ct;
}

static void _calc_bonuses(void)
{
	if (p_ptr->lev >  9) p_ptr->resist_fear = TRUE;
	if (p_ptr->lev > 19) p_ptr->sustain_wis = TRUE;
	if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
	if (p_ptr->lev > 39) p_ptr->telepathy = TRUE;
}

void _on_fail(const spell_info *spell)
{
	if (randint1(100) < (spell->fail / 2))
	{
		/* Backfire */
		int b = randint1(100);

		if (b < 5)
		{
			msg_print(T("Oh, no! Your mind has gone blank!", "�ʤ�Ƥ��ä���Ƭ���椬������ˤʤä���"));
			lose_all_info();
		}
		else if (b < 15)
		{
			msg_print(T("Weird visions seem to dance before your eyes...", "��̯�ʸ��ʤ��ܤ������٤äƤ���..."));
			set_image(p_ptr->image + 5 + randint1(10), FALSE);
		}
		else if (b < 45)
		{
			msg_print(T("Your brain is addled!", "���ʤ���Ƭ�Ϻ��𤷤���"));
			set_confused(p_ptr->confused + randint1(8), FALSE);
		}
		else if (b < 90)
		{
			set_stun(p_ptr->stun + randint1(8), FALSE);
		}
		else
		{
			/* Mana storm */
			msg_print(T("Your mind unleashes its power in an uncontrollable storm!", "���Ϥ�����Ǥ��ʤ���ή�ȤʤäƲ������줿��"));

			project(PROJECT_WHO_UNCTRL_POWER, 2 + p_ptr->lev / 10, py, px, p_ptr->lev * 2,
				GF_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM, -1);
			p_ptr->csp = MAX(0, p_ptr->csp - p_ptr->lev * MAX(1, p_ptr->lev / 10));
		}
	}
}

static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = T("mindcraft", "Ķǽ��");
		me.use_sp = TRUE;
		me.on_fail = _on_fail;
		me.options = CASTER_ALLOW_DEC_MANA;
		init = TRUE;
	}
	return &me;
}

class_t *mindcrafter_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	/* static info never changes */
	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 30,  33,  38,   3,  22,  16,  50,  40 };
	skills_t xs = { 10,  11,  10,   0,   0,   0,  14,  18 };

		me.name = "Mindcrafter";
		me.desc = "The Mindcrafter is a unique class that uses the powers of the mind "
		          "instead of magic.  These powers are unique to Mindcrafters, and "
				  "vary from simple extrasensory powers to mental domination of others.  "
				  "Since these powers are developed by the practice of certain "
				  "disciplines, a Mindcrafter requires no spellbooks to use them.  "
				  "The available powers are simply determined by the character's "
				  "level.  Wisdom determines a Mindcrafter's ability to use mind powers.";
		me.stats[A_STR] = -1;
		me.stats[A_INT] =  0;
		me.stats[A_WIS] =  3;
		me.stats[A_DEX] = -1;
		me.stats[A_CON] = -1;
		me.stats[A_CHR] =  2;
		me.base_skills = bs;
		me.extra_skills = xs;
		me.calc_bonuses = _calc_bonuses;
		me.caster_info = _caster_info;
		me.get_spells = _get_spells;
		me.get_powers = _get_powers;
		init = TRUE;
	}

	return &me;
}
