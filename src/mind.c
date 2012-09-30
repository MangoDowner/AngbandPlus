/* File: mind.c */

/* Purpose: Mindcrafter code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#define MAX_MINDCRAFT_POWERS  13

static const char *mind_tips[MAX_MINDCRAFT_POWERS] =
{
#ifdef JP
	"�����������ŷ�ε�����ġ�",
	"�᤯�����Ƥθ������󥹥������Τ��롣��٥�5���/�⡢15��Ʃ���ʥ�󥹥�����30�Ǻ����ȥ����ƥ���ΤǤ���褦�ˤʤ롣��٥�20�Ǽ��դ��Ϸ����Τ���45�Ǥ��γ����Τ�ʵפ˾Ȥ餷�����󥸥����Τ��٤ƤΥ����ƥ���Τ��롣��٥�25�ǰ�����֥ƥ�ѥ��������롣",
	"���Υ�Υƥ�ݡ��Ȥ򤹤롣��٥�40�ʾ��û��Υ��λ��ꤷ�����˥ƥ�ݡ��Ȥ��롣",
	"���Υ�Υƥ�ݡ��Ȥ򤹤롣",
	"��٥�30̤���ǡ���󥹥�����ۯ۰�����𤫶��ݤ����������ġ���٥�30�ʾ�ǻ볦������ƤΥ�󥹥�����̥λ���롣�񹳤�����̵����",
	"�첻�ε�����ġ�",
	"������֡�AC��徺�����롣��٥뤬�夬��ȡ������ꡢ�䵤���ŷ⡢�Ǥ������������롣",
	"��٥�25̤���ǡ������ƥ��ʷ�ϵ����Τ롣��٥�25�ʾ�ǡ������ƥ����ꤹ�롣",
	"��٥�25̤���ǡ���ʬ���濴�Ȥ�����������ε��ȯ�������롣��٥�25�ʾ�ǡ��볦������ƤΥ�󥹥������Ф������������Ԥ���",
	"���ݤ�ۯ۰������������ҡ�����ʬ�Ǥⶸ��β����֤Ǥ��®���֤Ǥ�ʤ����HP�������������롣����ˡ���٥�35̤���ʤ�а�����֥ҡ�����ʬ�ˡ���٥�35�ʾ�Ƕ���β�������®���롣",
	"�����ƥ��ʬ��­���ذ�ư�����롣",
	"��������ε�����ġ���󥹥�����̿�椹��ȡ�0��1.5��������񤹤롣�񹳤���ʤ���С�MP���������롣",
	"̵�������ڤ��������ʥ��ͥ륮���Υӡ�������ġ�",
#else
	"Fires a ball of lightning.",
	"Detects visible monsters in your vicinity and more and more. Detects traps and doors at level 5, invisible monsters at level 15, items at level 30. And magic mapping at level 20. Lights and know the whole level at level 45. Gives telepathy at level 25.",
	"Teleport short distance. Or teleport to given location at level 40.",
	"Teleport long distance.",
	"Stuns, confuses or scares a monster. Or attempts to charm all monsters in sight at level 30.",
	"Fires a ball of sound.",
	"Gives stone skin and some resistance to elements for a while. The level increased, the more number of resistances given.",
	"Gives feeling of an item. Or identify an item at level 25.",
	"Generate a ball centered on you which inflict monster with PSI damage. Or inflict all monsters with PSI damage at level 25.",
	"Removes fear and stun. Gives heroism and speed. Heals HP a little unless you already have heroism and temporal speed boost.",
	"Pulls a distant item close to you.",
	"Fires a ball which damages monsters and absorbs monsters' mind power. Absorbing is takes more turns which from 0 to 1.5.",
	"Fires a beam of pure energy which penetrate the invulnerability barrier.",
#endif
};

mindcraft_power mindcraft_powers[MAX_MINDCRAFT_POWERS] =
{
	/* Level gained,  cost,  %fail,  name */
#ifdef JP
	{  1,  1,  15, "����å�" },
	{  2,  1,  20, "�ƥ�ѥ�" },
	{  3,  2,  25, "�֥��" },
	{  7,  6,  35, "�ƥ�ݡ���" },
	{  9,  7,  50, "��������" },
	{ 11,  7,  30, "���ͥ륮�����ܡ���" },
	{ 13, 12,  50, "�������å����Хꥢ" },
	{ 15, 12,  60, "��������ȥ꡼" },
	{ 18, 10,  45, "���������ե쥢" },
	{ 23, 15,  50, "���ɥ�ʥ�󡦥ɡ��ԥ�" },
	{ 26, 28,  60, "�ƥ쥭�ͥ���" },
	{ 28, 10,  40, "�������å����ɥ쥤��" },
	{ 35, 35,  75, "�����������ԥ�" },
#else
	{  1,  1,  15, "Shock" },
	{  2,  1,  20, "Precognition" },
	{  3,  2,  25, "Minor Displacement" },
	{  7,  6,  35, "Major Displacement" },
	{  9,  7,  50, "Domination" },
	{ 11,  7,  30, "Ball of Energy" },
	{ 13, 12,  50, "Character Armour" },
	{ 15, 12,  60, "Psychometry" },
	{ 18, 10,  45, "Mind Wave" },
	{ 23, 15,  50, "Adrenaline Channeling" },
	{ 26, 28,  60, "Telekinesis" },
	{ 28, 10,  40, "Psychic Drain" },
	{ 35, 35,  75, "Psycho-Spear" },
#endif
};


void mindcraft_info(char *p, int power)
{
	int plev = p_ptr->lev;

	strcpy(p, "");

	switch (power)
	{
#ifdef JP
	case 0:  sprintf(p, " »��:%dd%d", 3 + ((plev - 1) / 5), 4); break;
	case 2:  sprintf(p, " �ϰ�:%d", (plev < 25 ? 10 : plev + 2)); break;
	case 3:  sprintf(p, " �ϰ�:%d", plev * 5);  break;
	case 5:  sprintf(p, " »��:%dd8", 8 + ((plev - 5) / 4));  break;
	case 6:  sprintf(p, " ����:%d", plev);  break;
	case 8:
		if (plev < 25) sprintf(p, " »��:%d", plev * 3 / 2);
		else sprintf(p, " »��:d%d", plev * ((plev - 5) / 10 + 1));
		break;
	case 9: sprintf(p, " ����:10+d%d", (plev * 3) / 2); break;
	case 10: sprintf(p, " ����:%d.%dkg", lbtokg1(plev * 15), lbtokg2(plev * 15)); break;
	case 11: sprintf(p, " »��:%dd6", plev / 2);  break;
	case 12: sprintf(p, " »��:%d", plev * 5); break;
#else
	case 0:  sprintf(p, " dam %dd%d", 3 + ((plev - 1) / 5), 4); break;
	case 2:  sprintf(p, " range %d", (plev < 25 ? 10 : plev + 2)); break;
	case 3:  sprintf(p, " range %d", plev * 5);  break;
	case 5:  sprintf(p, " dam %dd8", 8 + ((plev - 5) / 4));  break;
	case 6:  sprintf(p, " dur %d", plev);  break;
	case 8:
		if (plev < 25) sprintf(p, " dam %d", plev * 3 / 2);
		else sprintf(p, " dam d%d", plev * ((plev - 5) / 10 + 1));
		break;
	case 9: sprintf(p, " dur 10+d%d", (plev * 3) / 2); break;
	case 10: sprintf(p, " wgt %d", plev * 15); break;
	case 11: sprintf(p, " dam %dd6", plev / 2);  break;
	case 12: sprintf(p, " dam %d", plev * 5); break;
#endif
	}
}


void display_mind_list(void)
{
	int             i;
	int             y = 1;
	int             x = 1;
	int             plev = p_ptr->lev;
	int             chance;
	mindcraft_power spell;
	char            comment[80];
	char            psi_desc[80];

	/* Display a list of spells */
	prt("", y, x);
#ifdef JP
	put_str("̾��", y, x + 5);
	put_str("Lv   MP ��Ψ ����", y, x + 35);
#else
	put_str("Name", y, x + 5);
	put_str("Lv Mana Fail Info", y, x + 35);
#endif

	/* Dump the spells */
	for (i = 0; i < MAX_MINDCRAFT_POWERS; i++)
	{
		/* Access the available spell */
		spell = mindcraft_powers[i];
		if (spell.min_lev > plev) break;

		chance = get_spell_chance(spell.fail, spell.min_lev, spell.mana_cost);

		/* Get info */
		mindcraft_info(comment, i);

		/* Dump the spell */
		sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s",
			I2A(i), spell.name,
			spell.min_lev, spell.mana_cost, chance, comment);

		Term_putstr(x, y + i + 1, -1, TERM_WHITE, psi_desc);
	}
	return;
}


/*
 * Allow user to choose a mindcrafter power.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 *
 * nb: This function has a (trivial) display bug which will be obvious
 * when you run it. It's probably easy to fix but I haven't tried,
 * sorry.
 */
static int get_mindcraft_power(int *sn, bool only_browse)
{
	int             i;
	int             num = 0;
	int             y = 1;
	int             x = 20;
	int             minfail;
	int             plev = p_ptr->lev;
	int             chance;
	int             ask;
	char            choice;
	char            out_val[160];
	char            comment[80];
#ifdef JP
	cptr            p = "Ķǽ��";
#else
	cptr            p = "power";
#endif
	mindcraft_power spell;
	bool            flag, redraw;

	/* Assume cancelled */
	*sn = (-1);

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell */
		if (mindcraft_powers[*sn].min_lev <= plev)
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	for (i = 0; i < MAX_MINDCRAFT_POWERS; i++)
	{
		if (mindcraft_powers[i].min_lev <= plev)
		{
			num++;
		}
	}

	/* Build a prompt (accept all spells) */
	if (only_browse)
	{
#ifdef JP
		(void)strnfmt(out_val, 78, "(%^s %c-%c, '*'�ǰ���, ESC) �ɤ�%s�ˤĤ����Τ�ޤ�����",
#else
		(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
			      p, I2A(0), I2A(num - 1), p);
	}
	else
	{
#ifdef JP
		(void)strnfmt(out_val, 78, "(%^s %c-%c, '*'�ǰ���, ESC) �ɤ�%s��Ȥ��ޤ�����",
#else
		(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ",
#endif
			  p, I2A(0), I2A(num - 1), p);
	}

	/* Get a spell from the user */
	choice = always_show_list ? ESCAPE : 1;
	while (!flag)
	{
		if(choice == ESCAPE) choice = ' ';
		else if( !get_com(out_val, &choice) )break; 

		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Show the list */
			if (!redraw)
			{
				char psi_desc[80];

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				screen_save();

				/* Display a list of spells */
				prt("", y, x);
#ifdef JP
				put_str("̾��", y, x + 5);
				put_str("Lv   MP ��Ψ ����", y, x + 35);
#else
				put_str("Name", y, x + 5);
				put_str("Lv Mana Fail Info", y, x + 35);
#endif

				/* Dump the spells */
				for (i = 0; i < MAX_MINDCRAFT_POWERS; i++)
				{
					/* Access the spell */
					spell = mindcraft_powers[i];
					if (spell.min_lev > plev)   break;

					chance = spell.fail;

					/* Reduce failure rate by "effective" level adjustment */
					chance -= 3 * (plev - spell.min_lev);

					/* Reduce failure rate by INT/WIS adjustment */
					chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[mp_ptr->spell_stat]] - 1);

					/* Not enough mana to cast */
					if (spell.mana_cost > p_ptr->csp)
					{
						chance += 5 * (spell.mana_cost - p_ptr->csp);
					}

					/* Extract the minimum failure rate */
					minfail = adj_mag_fail[p_ptr->stat_ind[mp_ptr->spell_stat]];

					/* Minimum failure rate */
					if (chance < minfail) chance = minfail;

					/* Stunning makes spells harder */
					if (p_ptr->stun > 50) chance += 25;
					else if (p_ptr->stun) chance += 15;

					/* Always a 5 percent chance of working */
					if (chance > 95) chance = 95;

					/* Get info */
					mindcraft_info(comment, i);

					/* Dump the spell --(-- */
					sprintf(psi_desc, "  %c) %-30s%2d %4d %3d%%%s",
						I2A(i), spell.name,
						spell.min_lev, spell.mana_cost, chance, comment);
					prt(psi_desc, y + i + 1, x);
				}

				/* Clear the bottom line */
				prt("", y + i + 1, x);
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		/* Note verify */
		ask = isupper(choice);

		/* Lowercase */
		if (ask) choice = tolower(choice);

		/* Extract request */
		i = (islower(choice) ? A2I(choice) : -1);

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = mindcraft_powers[i];

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Prompt */
#ifdef JP
			(void) strnfmt(tmp_val, 78, "%s��Ȥ��ޤ�����", mindcraft_powers[i].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", mindcraft_powers[i].name);
#endif

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	/* Show choices */
	if (show_choices)
	{
		/* Update */
		p_ptr->window |= (PW_SPELL);

		/* Window stuff */
		window_stuff();
	}

	/* Abort if needed */
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = i;

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
static bool cast_mindcrafter_spell(int spell)
{
	int b;
	int dir;
	int plev = p_ptr->lev;


	/* spell code */
	switch (spell)
	{
	case 0:   /* Shock */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_ELEC, dir,
			  damroll(3 + ((plev - 1) / 5), 4), 0);  
		break;
	case 1:   /* Precog */
		if (plev > 44)
			wiz_lite();
		else if (plev > 19)
			map_area(DETECT_RAD_MAP);

		if (plev < 30)
		{
			b = detect_monsters_normal(DETECT_RAD_DEFAULT);
			if (plev > 14) b |= detect_monsters_invis(DETECT_RAD_DEFAULT);
			if (plev > 4)
			{
				b |= detect_traps(DETECT_RAD_DEFAULT);
				b |= detect_doors(DETECT_RAD_DEFAULT);
			}
		}
		else
		{
			b = detect_all(DETECT_RAD_DEFAULT);
		}

		if ((plev > 24) && (plev < 40))
			set_tim_esp(p_ptr->tim_esp + plev);

#ifdef JP
		if (!b) msg_print("�����ʵ������롣");
#else
		if (!b) msg_print("You feel safe.");
#endif
		break;
	case 2:
		/* Minor displace */
		if (plev < 40)
		{
			teleport_player(10);
		}
		else
		{
#ifdef JP
			msg_print("�������⤬����������Ū�Ϥ�����ǲ�������");
#else
			msg_print("You open a dimensional gate. Choose a destination.");
#endif
			return dimension_door();
		}
		break;
	case 3:
		/* Major displace */
		teleport_player(plev * 5);
		break;
	case 4:
		/* Domination */
		if (plev < 30)
		{
			if (!get_aim_dir(&dir)) return FALSE;

			fire_ball(GF_DOMINATION, dir, plev, 0);
		}
		else
		{
			charm_monsters(plev * 2);
		}
		break;
	case 5:
		/* Fist of Force  ---  not 'true' TK  */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_SOUND, dir, damroll(8 + ((plev - 5) / 4), 8),
			(plev > 20 ? (plev - 20) / 8 + 1 : 0));
		break;
	case 6:
		/* Character Armour */
		set_shield(p_ptr->shield + plev);
		if (plev > 14) set_oppose_acid(p_ptr->oppose_acid + plev);
		if (plev > 19) set_oppose_fire(p_ptr->oppose_fire + plev);
		if (plev > 24) set_oppose_cold(p_ptr->oppose_cold + plev);
		if (plev > 29) set_oppose_elec(p_ptr->oppose_elec + plev);
		if (plev > 34) set_oppose_pois(p_ptr->oppose_pois + plev);
		break;
	case 7:
		/* Psychometry */
		if (plev < 25) return psychometry();
		else return ident_spell();
	case 8:
		/* Mindwave */
#ifdef JP
		msg_print("������Ǳ���ʤ�����ư��ȯ����������");
#else
		msg_print("Mind-warping forces emanate from your brain!");
#endif
		if (plev < 25)
			project(0, 2 + plev / 10, py, px,
			(plev * 3) / 2, GF_PSI, PROJECT_KILL);
		else
			(void)mindblast_monsters(randint1(plev * ((plev - 5) / 10 + 1)));
		break;
	case 9:
		/* Adrenaline */
		set_afraid(0);
		set_stun(0);

		/*
		 * Only heal when Adrenalin Channeling is not active. We check
		 * that by checking if the player isn't fast and 'heroed' atm.
		 */
		if (!p_ptr->fast || !(p_ptr->hero || p_ptr->shero))
		{
			hp_player(plev);
		}

		b = 10 + randint1((plev * 3) / 2);
		if (plev < 35)
			set_hero(p_ptr->hero + b);
		else
			set_shero(p_ptr->shero + b);

		if (!p_ptr->fast)
		{
			/* Haste */
			(void)set_fast(b);
		}
		else
		{
			(void)set_fast(p_ptr->fast + randint1(5));
		}
		break;
	case 10:
		/* Telekinesis */
		if (!get_aim_dir(&dir)) return FALSE;

		fetch(dir, plev * 15, FALSE);
		break;
	case 11:
		/* Psychic Drain */
		if (!get_aim_dir(&dir)) return FALSE;

		b = damroll(plev / 2, 6);

		/* This is always a radius-0 ball now */
		if (fire_ball(GF_PSI_DRAIN, dir, b, 0))
			p_ptr->energy_need += randint1(150);
		break;
	case 12:
		/* Psycho spear */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_PSY_SPEAR, dir, plev * 5);
		break;
	default:
#ifdef JP
		msg_print("�ʤˡ�");
#else
		msg_print("Zap?");
#endif
	}

	return TRUE;
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_mindcraft(void)
{
	int             n = 0;
	int             chance;
	int             minfail;
	int             plev = p_ptr->lev;
	int             old_csp = p_ptr->csp;
	mindcraft_power spell;
	bool            cast;


	/* not if confused */
	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("���𤷤Ƥ��ƽ���Ǥ��ʤ���");
#else
		msg_print("You are too confused!");
#endif
		return;
	}

	/* get power */
	if (!get_mindcraft_power(&n, FALSE)) return;

	spell = mindcraft_powers[n];

	/* Verify "dangerous" spells */
	if (spell.mana_cost > p_ptr->csp)
	{
		/* Warning */
#ifdef JP
		msg_print("�ͣФ�­��ޤ���");
#else
		msg_print("You do not have enough mana to use this power.");
#endif

		if (!over_exert) return;

		/* Verify */
#ifdef JP
		if (!get_check("����Ǥ�ĩ�路�ޤ���? ")) return;
#else
		if (!get_check("Attempt it anyway? ")) return;
#endif
	}

	/* Spell failure chance */
	chance = spell.fail;

	/* Reduce failure rate by "effective" level adjustment */
	chance -= 3 * (plev - spell.min_lev);

	/* Reduce failure rate by INT/WIS adjustment */
	chance -= 3 * (adj_mag_stat[p_ptr->stat_ind[mp_ptr->spell_stat]] - 1);

	/* Not enough mana to cast */
	if (spell.mana_cost > p_ptr->csp)
	{
		chance += 5 * (spell.mana_cost - p_ptr->csp);
	}

	/* Extract the minimum failure rate */
	minfail = adj_mag_fail[p_ptr->stat_ind[mp_ptr->spell_stat]];

	/* Minimum failure rate */
	if (chance < minfail) chance = minfail;

	/* Stunning makes spells harder */
	if (p_ptr->stun > 50) chance += 25;
	else if (p_ptr->stun) chance += 15;

	/* Always a 5 percent chance of working */
	if (chance > 95) chance = 95;

	/* Failed spell */
	if (randint0(100) < chance)
	{
		if (flush_failure) flush();
#ifdef JP
		msg_format("�����ν���˼��Ԥ�����");
#else
		msg_format("You failed to concentrate hard enough!");
#endif
		sound(SOUND_FAIL);

		if (randint1(100) < (chance / 2))
		{
			/* Backfire */
			int b = randint1(100);
			if (b < 5)
			{
#ifdef JP
				msg_print("�ʤ�Ƥ��ä���Ƭ���椬������ˤʤä���");
#else
				msg_print("Oh, no! Your mind has gone blank!");
#endif
				lose_all_info();
			}
			else if (b < 15)
			{
#ifdef JP
				msg_print("��̯�ʸ��ʤ��ܤ������٤äƤ���...");
#else
				msg_print("Weird visions seem to dance before your eyes...");
#endif
				set_image(p_ptr->image + 5 + randint1(10));
			}
			else if (b < 45)
			{
#ifdef JP
				msg_print("���ʤ���Ƭ�Ϻ��𤷤���");
#else
				msg_print("Your brain is addled!");
#endif
				set_confused(p_ptr->confused + randint1(8));
			}
			else if (b < 90)
			{
				set_stun(p_ptr->stun + randint1(8));
			}
			else
			{
				/* Mana storm */
#ifdef JP
				msg_print("�������Ϥ�����Ǥ��ʤ���ή�ȤʤäƲ������줿��");
#else
				msg_print("Your mind unleashes its power in an uncontrollable storm!");
#endif
				project(1, 2 + plev / 10, py, px, plev * 2,
					GF_MANA, PROJECT_JUMP | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM);
				p_ptr->csp = MAX(0, p_ptr->csp - plev * MAX(1, plev / 10));
			}
		}
	}
	else
	{
		sound(SOUND_ZAP);

		/* Cast the spell */
		cast = cast_mindcrafter_spell(n);

		if (!cast) return;
	}

	/* Take a turn */
	energy_use = 100;

	/* Sufficient mana */
	if (spell.mana_cost <= old_csp)
	{
		/* Use some mana */
		p_ptr->csp -= spell.mana_cost;

		/* Limit */
		if (p_ptr->csp < 0) p_ptr->csp = 0;
	}

	/* Over-exert the player */
	else
	{
		int oops = spell.mana_cost - old_csp;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
#ifdef JP
		msg_print("�������椷�����Ƶ��򼺤äƤ��ޤä���");
#else
		msg_print("You faint from the effort!");
#endif

		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint1(5 * oops + 1));

		/* Damage WIS (possibly permanently) */
		if (randint0(100) < 50)
		{
			bool perm = (randint0(100) < 25);

			/* Message */
#ifdef JP
			msg_print("��ʬ�������򹶷⤷�Ƥ��ޤä���");
#else
			msg_print("You have damaged your mind!");
#endif

			/* Reduce constitution */
			(void)dec_stat(A_WIS, 15 + randint1(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
}

/*
 * do_cmd_cast calls this function if the player's class
 * is 'mindcrafter'.
 */
void do_cmd_mind_browse(void)
{
	int n = 0;
	int j, line;
	char temp[62 * 5];

	screen_save();

	while(1)
	{
		/* get power */
		if (!get_mindcraft_power(&n, TRUE))
		{
			screen_load();
			return;
		}

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(12, 21, 255);
		Term_erase(12, 20, 255);
		Term_erase(12, 19, 255);
		Term_erase(12, 18, 255);
		Term_erase(12, 17, 255);
		Term_erase(12, 16, 255);

		roff_to_buf(mind_tips[n], 62, temp, sizeof(temp));
		for(j = 0, line = 17; temp[j]; j += (1 + strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
	}
}
