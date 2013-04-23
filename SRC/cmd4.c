/* File: cmd4.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Hack -- redraw the screen
 *
 * This command performs various low level updates, clears all the "extra"
 * windows, does a total redraw of the main window, and requests all of the
 * interesting updates and redraws that I can think of.
 *
 * This command is also used to "instantiate" the results of the user
 * selecting various things, such as graphics mode, so it must call
 * the "TERM_XTRA_REACT" hook before redrawing the windows.
 */
void do_cmd_redraw(void)
{
	int j;

	term *old = Term;


	/* Hack -- react to changes */
	Term_xtra(TERM_XTRA_REACT, 0);


	/* Verify the keymap */
	keymap_init();


	/* Combine and Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);


	/* Update torch */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Forget lite/view */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_MONSTER | PW_OBJECT);

	/* Hack -- update */
	handle_stuff();


	/* Redraw every window */
	for (j = 0; j < 8; j++)
	{
		/* Dead window */
		if (!angband_term[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw */
		Term_redraw();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- change name
 */
void do_cmd_change_name(void)
{
	char c;

	int mode = 0;

	char tmp[160];


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Forever */
	while (1)
	{
		/* Display the player */
		display_player(mode);

		/* Prompt */
		Term_putstr(2, 23, -1, TERM_WHITE,
			"['c' to change name, 'f' to file, 'h' to change mode, or ESC]");

		/* Query */
		c = inkey();

		/* Exit */
		if (c == ESCAPE) break;

		/* Change name */
		if (c == 'c')
		{
			get_name();
		}

		/* File dump */
		else if (c == 'f')
		{
			sprintf(tmp, "%s.txt", op_ptr->base_name);
			if (get_string("File name: ", tmp, 80))
			{
				if (tmp[0] && (tmp[0] != ' '))
				{
					file_character(tmp, FALSE);
				}
			}
		}

		/* Toggle mode */
		else if (c == 'h')
		{
			mode++;
		}

		/* Oops */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}


/*
 * Recall the most recent message
 */
void do_cmd_message_one(void)
{
	/* Recall one message XXX XXX XXX */
	prt(format("> %s", message_str(0)), 0, 0);
}


/*
 * Show previous messages to the user
 *
 * The screen format uses line 0 and 23 for headers and prompts,
 * skips line 1 and 22, and uses line 2 thru 21 for old messages.
 *
 * This command shows you which commands you are viewing, and allows
 * you to "search" for strings in the recall.
 *
 * Note that messages may be longer than 80 characters, but they are
 * displayed using "infinite" length, with a special sub-command to
 * "slide" the virtual display to the left or right.
 *
 * Attempt to only hilite the matching portions of the string.
 */
void do_cmd_messages(void)
{
	int i, j, k, n, q;

	char shower[80];
	char finder[80];


	/* Wipe finder */
	strcpy(finder, "");

	/* Wipe shower */
	strcpy(shower, "");


	/* Total messages */
	n = message_num();

	/* Start on first message */
	i = 0;

	/* Start at leftmost edge */
	q = 0;


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Dump up to 20 lines of messages */
		for (j = 0; (j < 20) && (i + j < n); j++)
		{
			cptr msg = message_str(i+j);

			/* Apply horizontal scroll */
			msg = (strlen(msg) >= q) ? (msg + q) : "";

			/* Dump the messages, bottom to top */
			Term_putstr(0, 21-j, -1, TERM_WHITE, msg);

			/* Hilite "shower" */
			if (shower[0])
			{
				cptr str = msg;

				/* Display matches */
				while ((str = strstr(str, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(str-msg, 21-j, len, TERM_YELLOW, shower);

					/* Advance */
					str += len;
				}
			}
		}

		/* Display header XXX XXX XXX */
		prt(format("Message Recall (%d-%d of %d), Offset %d",
					  i, i+j-1, n, q), 0, 0);

		/* Display prompt (not very informative) */
		prt("[Press 'p' for older, 'n' for newer, ..., or ESCAPE]", 23, 0);

		/* Get a command */
		k = inkey();

		/* Exit on Escape */
		if (k == ESCAPE) break;

		/* Hack -- Save the old index */
		j = i;

		/* Horizontal scroll */
		if (k == '4')
		{
			/* Scroll left */
			q = (q >= 40) ? (q - 40) : 0;

			/* Success */
			continue;
		}

		/* Horizontal scroll */
		if (k == '6')
		{
			/* Scroll right */
			q = q + 40;

			/* Success */
			continue;
		}

		/* Hack -- handle show */
		if (k == '=')
		{
			/* Prompt */
			prt("Show: ", 23, 0);

			/* Get a "shower" string, or continue */
			if (!askfor_aux(shower, 80)) continue;

			/* Okay */
			continue;
		}

		/* Hack -- handle find */
		if (k == '/')
		{
			int z;

			/* Prompt */
			prt("Find: ", 23, 0);

			/* Get a "finder" string, or continue */
			if (!askfor_aux(finder, 80)) continue;

			/* Show it */
			strcpy(shower, finder);

			/* Scan messages */
			for (z = i + 1; z < n; z++)
			{
				cptr msg = message_str(z);

				/* Search for it */
				if (strstr(msg, finder))
				{
					/* New location */
					i = z;

					/* Done */
					break;
				}
			}
		}

		/* Recall 1 older message */
		if ((k == '8') || (k == '\n') || (k == '\r'))
		{
			/* Go newer if legal */
			if (i + 1 < n) i += 1;
		}

		/* Recall 10 older messages */
		if (k == '+')
		{
			/* Go older if legal */
			if (i + 10 < n) i += 10;
		}

		/* Recall 20 older messages */
		if ((k == 'p') || (k == KTRL('P')) || (k == ' '))
		{
			/* Go older if legal */
			if (i + 20 < n) i += 20;
		}

		/* Recall 20 newer messages */
		if ((k == 'n') || (k == KTRL('N')))
		{
			/* Go newer (if able) */
			i = (i >= 20) ? (i - 20) : 0;
		}

		/* Recall 10 newer messages */
		if (k == '-')
		{
			/* Go newer (if able) */
			i = (i >= 10) ? (i - 10) : 0;
		}

		/* Recall 1 newer messages */
		if (k == '2')
		{
			/* Go newer (if able) */
			i = (i >= 1) ? (i - 1) : 0;
		}

		/* Hack -- Error of some kind */
		if (i == j) bell();
	}

	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}

#ifdef ALLOW_CHEATER
/*
 * Cheating options -- textual names
 */
static cptr cheating_text[CHEAT_MAX] =
{
	"cheat_peek",
	"cheat_hear",
	"cheat_room",
	"cheat_xtra",
	"cheat_know",
	"cheat_live"
};

/*
 * Cheating options -- descriptions
 */
static cptr cheating_desc[CHEAT_MAX] =
{
	"Peek into object creation",
	"Peek into monster creation",
	"Peek into dungeon creation",
	"Peek into something else",
	"Know complete monster info",
	"Allow player to avoid death"
};


/*
 * Interact with some options
 */
static void do_cmd_options_cheat(cptr info)
{
	char ch;

	int i, k = 0, n = CHEAT_MAX;

	char buf[80];


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
		sprintf(buf, "%s (RET to advance, y/n to set, ESC to accept) ", info);
		prt(buf, 0, 0);

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s  (%s)",
			cheating_desc[i],
			p_ptr->cheat[i] ? "yes" : "no ",
			cheating_text[i]);
			c_prt(a, buf, i + 2, 0);
		}

		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 't':
			case '5':
			{
				if (p_ptr->cheat[k])
				{
					p_ptr->cheat[k] = FALSE;
				}

				else
				{
					/* Mark player as a cheater */
					p_ptr->noscore |= (0x0100 << k);

					p_ptr->cheat[k] = TRUE;
				}

				break;
			}

			case 'y':
			case '6':
			{
				/* Mark player as a cheater */
				p_ptr->noscore |= (0x0100 << k);

				p_ptr->cheat[k] = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case '4':
			{
				p_ptr->cheat[k] = FALSE;
				k = (k + 1) % n;
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}
#endif

/*
 * Interact with some options
 */
static void do_cmd_options_aux(int page, cptr info)
{
	char ch;

	int i, k = 0;

	char buf[80];

	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
		sprintf(buf, "%s (RET to advance, y/n to set, ESC to accept) ", info);
		prt(buf, 0, 0);

		/* Display the options */
		for (i = 0; i < OPT_PAGE_LEN; i++)
		{
			byte a = TERM_WHITE;

			/* Ignore unused options */
			if (!opt_text[page][i]) continue;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s  (%s)",
					  opt_desc[page][i],
					  op_ptr->opt[page][i] ? "yes" : "no ",
					  opt_text[page][i]);
			c_prt(a, buf, i + 2, 0);
		}

		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				do
				{
					k = (OPT_PAGE_LEN + k - 1) % OPT_PAGE_LEN;
				} while (!opt_text[page][k]);

				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				do
				{
					k = (k + 1) % OPT_PAGE_LEN;
				} while (!opt_text[page][k]);

				break;
			}

			case 't':
			case '5':
			{
				op_ptr->opt[page][k] = !op_ptr->opt[page][k];
				break;
			}

			case 'y':
			case '6':
			{
				do
				{
					op_ptr->opt[page][k] = TRUE;
					k = (k + 1) % OPT_PAGE_LEN;
				} while (!opt_text[page][k]);

				break;
			}

			case 'n':
			case '4':
			{
				do
				{
					op_ptr->opt[page][k] = FALSE;
					k = (k + 1) % OPT_PAGE_LEN;
				} while (!opt_text[page][k]);

				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


/*
 * Modify the "window" options
 */
static void do_cmd_options_win(void)
{
	int i, j;

	int y = 0;
	int x = 0;

	char ch;

	bool go = TRUE;

	u32b old_flag[8];


	/* Memorize old flags */
	for (j = 0; j < 8; j++)
	{
		/* Acquire current flags */
		old_flag[j] = op_ptr->term_flag[j];
	}


	/* Clear screen */
	Term_clear();

	/* Interact */
	while (go)
	{
		/* Prompt */
		prt("Window flags (<dir> to move, 't' to toggle, or ESC)", 0, 0);

		/* Display the windows */
		for (j = 0; j < 8; j++)
		{
			byte a = TERM_WHITE;

			cptr s = angband_term_name[j];

			/* Use color */
			if (j == x) a = TERM_L_BLUE;

			/* Window name, staggered, centered */
			Term_putstr(35 + j * 5 - strlen(s) / 2, 2 + j % 2, -1, a, s);
		}

		/* Display the options */
		for (i = 0; i < MAX_TERM_FLAGS; i++)
		{
			byte a = TERM_WHITE;

			cptr str = term_flag_desc[i];

			/* Use color */
			if (i == y) a = TERM_L_BLUE;

			/* Unused option */
			if (!str) str = "(Unused option)";

			/* Flag name */
			Term_putstr(0, i + 5, -1, a, str);

			/* Display the windows */
			for (j = 0; j < 8; j++)
			{
				byte a = TERM_WHITE;

				char c = '.';

				/* Use color */
				if ((i == y) && (j == x)) a = TERM_L_BLUE;

				/* Active flag */
				if (op_ptr->term_flag[j] & (1L << i)) c = 'X';

				/* Flag value */
				Term_putch(35 + j * 5, i + 5, a, c);
			}
		}

		/* Place Cursor */
		Term_gotoxy(35 + x * 5, y + 5);

		/* Get key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			case 'q':
			{
				go = FALSE;
				break;
			}

			case 't':
			case '.':
			case '5':
			case '0':
			{
				/* Hack -- ignore the main window */
				if (x == 0)
				{
					bell();
					break;
				}

				/* Toggle flag */
				if (op_ptr->term_flag[x] & (1L << y))
				{
					op_ptr->term_flag[x] &= ~(1L << y);
				}
				else
				{
					op_ptr->term_flag[x] |= (1L << y);
				}

				break;
			}

			default:
			{
				int d = keymap_dirs[ch & 0x7F];

				x = (x + ddx[d] + 8) % 8;
				y = (y + ddy[d] + 16) % 16;

				if (!d) bell();
				
				break;
			}
		}
	}

	/* Notice changes */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* Dead window */
		if (!angband_term[j]) continue;

		/* Ignore non-changes */
		if (op_ptr->term_flag[j] == old_flag[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Erase */
		Term_clear();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}




/*
 * Set or unset various options.
 *
 * After using this command, a complete redraw is performed,
 * whether or not it is technically required.
 */
void do_cmd_options(void)
{
	int k;


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();


	/* Interact */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Why are we here */
		prt("RAngband options", 2, 0);

		/* Give some choices */
		prt("(1) User Interface Options", 4, 5);
		prt("(2) Disturbance Options", 5, 5);
		prt("(3) Game-Play Options", 6, 5);
		prt("(4) Efficiency Options", 7, 5);

#ifdef ALLOW_CHEATER
		/* Cheating */
		prt("(C) Cheating Options", 9, 5);
#endif

		/* Window flags */
		prt("(W) Window flags", 11, 5);

		/* Special choices */
		prt("(A) Autosave Delay", 13, 5);
		prt("(D) Base Delay Factor", 14, 5);
		prt("(H) Hitpoint Warning", 15, 5);
		prt("(K) Destruction Thresholds", 16, 5);

		/* Prompt */
		prt("Command: ", 18, 0);

		/* Get command */
		k = inkey();

		/* Exit */
		if (k == ESCAPE) break;

		/* Analyze */
		switch (k)
		{
			/* General Options */
			case '1':
			{
				/* Process the general options */
				do_cmd_options_aux(0, "User Interface Options");
				break;
			}

			/* Disturbance Options */
			case '2':
			{
				/* Spawn */
				do_cmd_options_aux(1, "Disturbance Options");
				break;
			}

			/* Inventory Options */
			case '3':
			{
				/* Spawn */
				do_cmd_options_aux(2, "Game-Play Options");
				break;
			}

			/* Efficiency Options */
			case '4':
			{
				/* Spawn */
				do_cmd_options_aux(3, "Efficiency Options");
				break;
			}

#ifdef ALLOW_CHEATER
			/* Cheating Options */
			case 'C':
			{
				/* Spawn */
				do_cmd_options_cheat("Cheaters never win (seriously!)");
				break;
			}
#endif

			/* Window flags */
			case 'W':
			case 'w':
			{
				/* Spawn */
				do_cmd_options_win();
				break;
			}

			/* Hack -- Autosave Delay */
			case 'A':
			case 'a':
			{
				char cmd;

            u16b t;

				/* Prompt */
				prt("Command: Autosave Delay", 18, 0);

				/* Get a new value */
				while (1)
				{
					prt(format("Current autosave delay: %d turns",
								  op_ptr->autosave_freq), 22, 0);
					prt("Autosave Delay (ESC to accept): ", 20, 0);

					/* Get a number */
					t = get_number(op_ptr->autosave_freq, -1, 20, 32, &cmd);

					/* Abort */
					if (cmd == ESCAPE) break;

					/* Protection */
					if (t < 10) continue;

					/* Set the new value */
					op_ptr->autosave_freq = t;
				}

				break;
			}

			/* Hack -- Delay Speed */
			case 'D':
			case 'd':
			{
				/* Prompt */
				prt("Command: Base Delay Factor", 18, 0);

				/* Get a new value */
				while (1)
				{
					int msec = op_ptr->delay_factor * op_ptr->delay_factor;
					prt(format("Current base delay factor: %d (%d msec)",
								  op_ptr->delay_factor, msec), 22, 0);
					prt("Delay Factor (0-9 or ESC to accept): ", 20, 0);
					k = inkey();
					if (k == ESCAPE) break;
					if (isdigit(k)) op_ptr->delay_factor = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- hitpoint warning factor */
			case 'H':
			case 'h':
			{
				/* Prompt */
				prt("Command: Hitpoint Warning", 18, 0);

				/* Get a new value */
				while (1)
				{
					prt(format("Current hitpoint warning: %2d%%",
								  op_ptr->hitpoint_warn * 10), 22, 0);
					prt("Hitpoint Warning (0-9 or ESC to accept): ", 20, 0);
					k = inkey();
					if (k == ESCAPE) break;
					if (isdigit(k)) op_ptr->hitpoint_warn = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- Destruction threshold */
			case 'K':
			case 'k':
			{
				char cmd;

				u32b t;

				/* Prompt */
				prt("Command: Destruction thresholds", 18, 0);

				/* Get a new value */
				prt(format("Destruction Threshold (valuable):   %lu",
							  op_ptr->destroy_good), 20, 0);

				/* Get a new value */
				prt(format("Destruction Threshold (worthless):  %lu",
							  op_ptr->destroy_junk), 21, 0);

				/* Get a number */
				t = get_number(op_ptr->destroy_good, -1, 20, 36, &cmd);

				/* Abort */
				if (cmd == ESCAPE) break;

				/* Set the new value */
				op_ptr->destroy_good = t;

				/* Get a number */
				t = get_number(op_ptr->destroy_junk, -1, 21, 36, &cmd);

				/* Abort */
				if (cmd == ESCAPE) break;

				/* Set the new value */
				op_ptr->destroy_junk = t;

				break;
			}

			/* Unknown option */
			default:
			{
				/* Oops */
				bell();
				break;
			}
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}



/*
 * Ask for a "user pref line" and process it
 *
 * XXX XXX XXX Allow absolute file names?
 */
void do_cmd_pref(void)
{
	char buf[80];

	/* Default */
	strcpy(buf, "");

	/* Ask for a "user pref command" */
	if (!get_string("Pref: ", buf, 80)) return;

	/* Process that pref command */
	(void)process_pref_file_aux(buf);
}


#ifdef ALLOW_MACROS

/*
 * Hack -- append all current macros to the given file
 */
static errr macro_dump(cptr fname)
{
	int i;

	FILE *fff;

	char buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) return (-1);


	/* Skip space */
	fprintf(fff, "\n\n");

	/* Start dumping */
	fprintf(fff, "# Automatic macro dump\n\n");

	/* Dump them */
	for (i = 0; i < macro__num; i++)
	{
		/* Start the macro */
		fprintf(fff, "# Macro '%d'\n\n", i);

		/* Extract the action */
		ascii_to_text(buf, macro__act[i]);

		/* Dump the macro */
		fprintf(fff, "A:%s\n", buf);

		/* Extract the action */
		ascii_to_text(buf, macro__pat[i]);

		/* Dump command macros */
		if (macro__cmd[i]) fprintf(fff, "C:%s\n", buf);

		/* Dump normal macros */
		else fprintf(fff, "P:%s\n", buf);

		/* End the macro */
		fprintf(fff, "\n\n");
	}

	/* Start dumping */
	fprintf(fff, "\n\n\n\n");


	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}


/*
 * Hack -- ask for a "trigger" (see below)
 *
 * Note the complex use of the "inkey()" function from "util.c".
 *
 * Note that both "flush()" calls are extremely important.
 */
static void do_cmd_macro_aux(char *buf)
{
	int i, n = 0;

	char tmp[1024];


	/* Flush */
	flush();

	/* Do not process macros */
	inkey_base = TRUE;

	/* First key */
	i = inkey();

	/* Read the pattern */
	while (i)
	{
		/* Save the key */
		buf[n++] = i;

		/* Do not process macros */
		inkey_base = TRUE;

		/* Do not wait for keys */
		inkey_scan = TRUE;

		/* Attempt to read a key */
		i = inkey();
	}

	/* Terminate */
	buf[n] = '\0';

	/* Flush */
	flush();


	/* Convert the trigger */
	ascii_to_text(tmp, buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);
}

#endif


/*
 * Interact with "macros"
 *
 * Note that the macro "action" must be defined before the trigger.
 *
 * XXX XXX XXX Need messages for success, plus "helpful" info
 */
void do_cmd_macros(void)
{
	int i;

	char tmp[160];

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Describe */
		prt("Interact with Macros", 2, 0);


		/* Describe that action */
		prt("Current action (if any) shown below:", 18, 0);

		/* Analyze the current action */
		ascii_to_text(buf, macro__buf);

		/* Display the current action */
		prt(buf, 20, 0);


		/* Selections */
		prt("(1) Load a user pref file", 4, 5);
#ifdef ALLOW_MACROS
		prt("(2) Dump macros", 5, 5);
		prt("(3) Enter a new action", 6, 5);
		prt("(4) Query a macro action", 7, 5);
		prt("(5) Create a command macro", 8, 5);
		prt("(6) Create a normal macro", 9, 5);
		prt("(7) Create an identity macro", 10, 5);
		prt("(8) Create an empty macro", 11, 5);
		prt("(9) Define a special keymap", 12, 5);
#endif /* ALLOW_MACROS */

		/* Prompt */
		prt("Command: ", 14, 0);

		/* Get a command */
		i = inkey();

		/* Leave */
		if (i == ESCAPE) break;

		/* Load a 'macro' file */
		else if (i == '1')
		{
			/* Prompt */
			prt("Command: Load a user pref file", 14, 0);

			/* Prompt */
			prt("File: ", 16, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Ask for a file */
			if (!askfor_aux(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);
		}

#ifdef ALLOW_MACROS

		/* Save a 'macro' file */
		else if (i == '2')
		{
			/* Prompt */
			prt("Command: Save a macro file", 14, 0);

			/* Prompt */
			prt("File: ", 16, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Ask for a file */
			if (!askfor_aux(tmp, 70)) continue;

			/* Drop priv's */
			safe_setuid_drop();

			/* Dump the macros */
			(void)macro_dump(tmp);

			/* Grab priv's */
			safe_setuid_grab();
		}

		/* Enter a new action */
		else if (i == '3')
		{
			/* Prompt */
			prt("Command: Enter a new action", 14, 0);

			/* Go to the correct location */
			Term_gotoxy(0, 20);

			/* Hack -- limit the value */
			tmp[80] = '\0';

			/* Get an encoded action */
			if (!askfor_aux(buf, 80)) continue;

			/* Extract an action */
			text_to_ascii(macro__buf, buf);
		}

		/* Query a macro action */
		else if (i == '4')
		{

#if 0
			/* Prompt */
			prt("Command: Query a macro action", 14, 0);

			/* Prompt */
			prt("Enter a macro trigger: ", 16, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);
#endif

			/* XXX XXX XXX */
			msg_print("Command not ready.");
		}

		/* Create a command macro */
		else if (i == '5')
		{
			/* Prompt */
			prt("Command: Create a command macro", 14, 0);

			/* Prompt */
			prt("Trigger: ", 16, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, macro__buf, TRUE);

			/* Message */
			msg_print("Created a new command macro.");
		}

		/* Create a normal macro */
		else if (i == '6')
		{
			/* Prompt */
			prt("Command: Create a normal macro", 14, 0);

			/* Prompt */
			prt("Trigger: ", 16, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, macro__buf, FALSE);

			/* Message */
			msg_print("Created a new normal macro.");
		}

		/* Create an identity macro */
		else if (i == '7')
		{
			/* Prompt */
			prt("Command: Create an identity macro", 14, 0);

			/* Prompt */
			prt("Trigger: ", 16, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, buf, FALSE);

			/* Message */
			msg_print("Created a new identity macro.");
		}

		/* Create an empty macro */
		else if (i == '8')
		{
			/* Prompt */
			prt("Command: Create an empty macro", 14, 0);

			/* Prompt */
			prt("Trigger: ", 16, 0);

			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, "", FALSE);

			/* Message */
			msg_print("Created a new empty macro.");
		}

		/* Define a keymap */
		else if (i == '9')
		{
			char i1, i2, i3;

			/* Flush the input */
			flush();

			/* Get the trigger */
			if (get_com("Type the trigger keypress: ", &i1) &&
			get_com("Type the resulting keypress: ", &i2) &&
			get_com("Type a direction (or zero): ", &i3))
			{
				/* Acquire the array index */
				int k = (i1 & 0x7F);
				int r = (i2 & 0x7F);
				int d = (i3 & 0x7F);

				/* Analyze the result key */
				keymap_cmds[k] = r;

				/* Hack -- Analyze the "direction" (always allow numbers) */
				keymap_dirs[k] = (isdigit(d) ? (d - '0') : keymap_dirs[d]);

				/* Success */
				msg_format("Keypress 0x%02x mapped to 0x%02x (direction %d)",
							  k, keymap_cmds[k], keymap_dirs[k]);
			}
		}

#endif /* ALLOW_MACROS */

		/* Oops */
		else
		{
			/* Oops */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Load the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}



/*
 * Interact with "visuals"
 */
void do_cmd_visuals(void)
{
	int i;

	FILE *fff;

	char tmp[160];

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
		prt("Interact with Visuals", 2, 0);

		/* Give some choices */
		prt("(1) Load a user pref file", 4, 5);
#ifdef ALLOW_VISUALS
		prt("(2) Dump monster attr/chars", 5, 5);
		prt("(3) Dump object attr/chars", 6, 5);
		prt("(4) Dump feature attr/chars", 7, 5);
		prt("(5) (unused)", 8, 5);
		prt("(6) Change monster attr/chars", 9, 5);
		prt("(7) Change object attr/chars", 10, 5);
		prt("(8) Change feature attr/chars", 11, 5);
		prt("(9) (unused)", 12, 5);
#endif
		prt("(0) Reset visuals", 13, 5);

		/* Prompt */
		prt("Command: ", 15, 0);

		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		/* Load a 'pref' file */
		else if (i == '1')
		{
			/* Prompt */
			prt("Command: Load a user pref file", 15, 0);

			/* Prompt */
			prt("File: ", 17, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Query */
			if (!askfor_aux(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);
		}

#ifdef ALLOW_VISUALS

		/* Dump monster attr/chars */
		else if (i == '2')
		{
			/* Prompt */
			prt("Command: Dump monster attr/chars", 15, 0);

			/* Prompt */
			prt("File: ", 17, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) continue;

			/* Start dumping */
			fprintf(fff, "\n\n");
			fprintf(fff, "# Monster attr/char definitions\n\n");

			/* Dump monsters */
			for (i = 0; i < RACE_MAX_NORMAL + cur_bones; i++)
			{
				monster_race *r_ptr = &r_info[i];

				/* Skip non-entries */
				if (!r_ptr->name) continue;

				/* Dump a comment */
				fprintf(fff, "# %s\n", (r_name + r_ptr->name));

				/* Dump the monster attr/char info */
				fprintf(fff, "R:%d:0x%02X:0x%02X\n\n", i,
				(byte)(r_ptr->x_attr), (byte)(r_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
			msg_print("Dumped monster attr/chars.");
		}

		/* Dump object attr/chars */
		else if (i == '3')
		{
			/* Prompt */
			prt("Command: Dump object attr/chars", 15, 0);

			/* Prompt */
			prt("File: ", 17, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) continue;

			/* Start dumping */
			fprintf(fff, "\n\n");
			fprintf(fff, "# Object attr/char definitions\n\n");

			/* Dump objects */
			for (i = 0; i < MAX_K_IDX; i++)
			{
				object_kind *k_ptr = &k_info[i];

				/* Skip non-entries */
				if (!k_ptr->name) continue;

				/* Dump a comment */
				fprintf(fff, "# %s\n", (k_name + k_ptr->name));

				/* Dump the object attr/char info */
				fprintf(fff, "K:%d:0x%02X:0x%02X\n\n", i,
				(byte)(k_ptr->x_attr), (byte)(k_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
			msg_print("Dumped object attr/chars.");
		}

		/* Dump feature attr/chars */
		else if (i == '4')
		{
			/* Prompt */
			prt("Command: Dump feature attr/chars", 15, 0);

			/* Prompt */
			prt("File: ", 17, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) continue;

			/* Start dumping */
			fprintf(fff, "\n\n");
			fprintf(fff, "# Feature attr/char definitions\n\n");

			/* Dump features */
			for (i = 0; i < MAX_F_IDX; i++)
			{
				feature_type *f_ptr = &f_info[i];

				/* Skip non-entries */
				if (!f_ptr->name) continue;

				/* Dump a comment */
				fprintf(fff, "# %s\n", (f_name + f_ptr->name));

				/* Dump the feature attr/char info */
				fprintf(fff, "F:%d:0x%02X:0x%02X\n\n", i,
				(byte)(f_ptr->z_attr), (byte)(f_ptr->z_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
			msg_print("Dumped feature attr/chars.");
		}

		/* Modify monster attr/chars */
		else if (i == '6')
		{
			static int r = 0;

			/* Prompt */
			prt("Command: Change monster attr/chars", 15, 0);

			/* Hack -- query until done */
			while (1)
			{
				monster_race *r_ptr = &r_info[r];

				int da = (byte)(r_ptr->d_attr);
				int dc = (byte)(r_ptr->d_char);
				int ca = (byte)(r_ptr->x_attr);
				int cc = (byte)(r_ptr->x_char);

				/* Label the object */
				Term_putstr(5, 17, -1, TERM_WHITE,
								format("Monster = %d, Name = %-40.40s",
							          r, (r_name + r_ptr->name)));

				/* Label the Default values */
				Term_putstr(10, 19, -1, TERM_WHITE,
							   format("Default attr/char = %3u / %3u", da, dc));
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

				/* Label the Current values */
				Term_putstr(10, 20, -1, TERM_WHITE,
							   format("Current attr/char = %3u / %3u", ca, cc));
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 20, ca, cc);

				/* Prompt */
				Term_putstr(0, 22, -1, TERM_WHITE,
							   "Command (n/N/a/A/c/C): ");

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze (ignore Undefined Ghosts) */
				if (i == 'n') r = (r + RACE_MAX_NORMAL + cur_bones + 1) % (RACE_MAX_NORMAL + cur_bones);
				if (i == 'N') r = (r + RACE_MAX_NORMAL + cur_bones - 1) % (RACE_MAX_NORMAL + cur_bones);
				if (i == 'a') r_ptr->x_attr = (byte)(ca + 1);
				if (i == 'A') r_ptr->x_attr = (byte)(ca - 1);
				if (i == 'c') r_ptr->x_char = (byte)(cc + 1);
				if (i == 'C') r_ptr->x_char = (byte)(cc - 1);
			}
		}

		/* Modify object attr/chars */
		else if (i == '7')
		{
			static int k = 0;

			/* Prompt */
			prt("Command: Change object attr/chars", 15, 0);

			/* Hack -- query until done */
			while (1)
			{
				object_kind *k_ptr = &k_info[k];

				int da = (byte)k_ptr->k_attr;
				int dc = (byte)k_ptr->k_char;
				int ca = (byte)k_ptr->x_attr;
				int cc = (byte)k_ptr->x_char;

				/* Label the object */
				Term_putstr(5, 17, -1, TERM_WHITE,
								format("Object = %d, Name = %-40.40s",
										 k, (k_name + k_ptr->name)));

				/* Label the Default values */
				Term_putstr(10, 19, -1, TERM_WHITE,
								format("Default attr/char = %3d / %3d", da, dc));
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

				/* Label the Current values */
				Term_putstr(10, 20, -1, TERM_WHITE,
							   format("Current attr/char = %3d / %3d", ca, cc));
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 20, ca, cc);

				/* Prompt */
				Term_putstr(0, 22, -1, TERM_WHITE,
							   "Command (n/N/a/A/c/C): ");

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') k = (k + MAX_K_IDX + 1) % MAX_K_IDX;
				if (i == 'N') k = (k + MAX_K_IDX - 1) % MAX_K_IDX;
				if (i == 'a') k_info[k].x_attr = (byte)(ca + 1);
				if (i == 'A') k_info[k].x_attr = (byte)(ca - 1);
				if (i == 'c') k_info[k].x_char = (byte)(cc + 1);
				if (i == 'C') k_info[k].x_char = (byte)(cc - 1);
			}
		}

		/* Modify feature attr/chars */
		else if (i == '8')
		{
			static int f = 0;

			/* Prompt */
			prt("Command: Change feature attr/chars", 15, 0);

			/* Hack -- query until done */
			while (1)
			{
				feature_type *f_ptr = &f_info[f];

				int da = (byte)f_ptr->f_attr;
				int dc = (byte)f_ptr->f_char;
				int ca = (byte)f_ptr->z_attr;
				int cc = (byte)f_ptr->z_char;

				/* Label the object */
				Term_putstr(5, 17, -1, TERM_WHITE,
								format("Terrain = %d, Name = %-40.40s",
										 f, (f_name + f_ptr->name)));

				/* Label the Default values */
				Term_putstr(10, 19, -1, TERM_WHITE,
								format("Default attr/char = %3d / %3d", da, dc));
				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

				/* Label the Current values */
				Term_putstr(10, 20, -1, TERM_WHITE,
								format("Current attr/char = %3d / %3d", ca, cc));
				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 20, ca, cc);

				/* Prompt */
				Term_putstr(0, 22, -1, TERM_WHITE,
							   "Command (n/N/a/A/c/C): ");

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') f = (f + MAX_F_IDX + 1) % MAX_F_IDX;
				if (i == 'N') f = (f + MAX_F_IDX - 1) % MAX_F_IDX;
				if (i == 'a') f_info[f].z_attr = (byte)(ca + 1);
				if (i == 'A') f_info[f].z_attr = (byte)(ca - 1);
				if (i == 'c') f_info[f].z_char = (byte)(cc + 1);
				if (i == 'C') f_info[f].z_char = (byte)(cc - 1);
			}
		}

#endif

		/* Reset visuals */
		else if (i == '0')
		{
			/* Reset */
			reset_visuals(TRUE);

			/* Message */
			msg_print("Visual attr/char tables reset.");
		}

		/* Unknown option */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}


/*
 * Interact with "colors"
 */
void do_cmd_colors(void)
{
	int i;

	FILE *fff;

	char tmp[160];

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
		prt("Interact with Colors", 2, 0);

		/* Give some choices */
		prt("(1) Load a user pref file", 4, 5);
#ifdef ALLOW_COLORS
		prt("(2) Dump colors", 5, 5);
		prt("(3) Modify colors", 6, 5);
#endif

		/* Prompt */
		prt("Command: ", 8, 0);

		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		/* Load a 'pref' file */
		if (i == '1')
		{
			/* Prompt */
			prt("Command: Load a user pref file", 8, 0);

			/* Prompt */
			prt("File: ", 10, 0);

			/* Default file */
			sprintf(tmp, "user.prf");

			/* Query */
			if (!askfor_aux(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);

			/* Mega-Hack -- react to changes */
			Term_xtra(TERM_XTRA_REACT, 0);

			/* Mega-Hack -- redraw */
			Term_redraw();
		}

#ifdef ALLOW_COLORS

		/* Dump colors */
		else if (i == '2')
		{
			/* Prompt */
			prt("Command: Dump colors", 8, 0);

			/* Prompt */
			prt("File: ", 10, 0);

			/* Default filename */
			sprintf(tmp, "user.prf");

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) continue;

			/* Start dumping */
			fprintf(fff, "\n\n");
			fprintf(fff, "# Color redefinitions\n\n");

			/* Dump colors */
			for (i = 0; i < 256; i++)
			{
				int kv = angband_color_table[i][0];
				int rv = angband_color_table[i][1];
				int gv = angband_color_table[i][2];
				int bv = angband_color_table[i][3];

				cptr name = "unknown";

				/* Skip non-entries */
				if (!kv && !rv && !gv && !bv) continue;

				/* Extract the color name */
				if (i < 16) name = color_names[i];

				/* Dump a comment */
				fprintf(fff, "# Color '%s'\n", name);

				/* Dump the monster attr/char info */
				fprintf(fff, "V:%d:0x%02X:0x%02X:0x%02X:0x%02X\n\n",
				i, kv, rv, gv, bv);
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
			msg_print("Dumped color redefinitions.");
		}

		/* Edit colors */
		else if (i == '3')
		{
			static int a = 0;

			/* Prompt */
			prt("Command: Modify colors", 8, 0);

			/* Hack -- query until done */
			while (1)
			{
				cptr name;

				/* Clear */
				clear_from(10);

				/* Exhibit the normal colors */
				for (i = 0; i < 16; i++)
				{
					/* Exhibit this color */
					Term_putstr(i*4, 20, -1, a, "###");

					/* Exhibit all colors */
					Term_putstr(i*4, 22, -1, i, format("%3d", i));
				}

				/* Describe the color */
				name = ((a < 16) ? color_names[a] : "undefined");

				/* Describe the color */
				Term_putstr(5, 10, -1, TERM_WHITE,
								format("Color = %d, Name = %s", a, name));

				/* Label the Current values */
				Term_putstr(5, 12, -1, TERM_WHITE,
								format("K = 0x%02x / R,G,B = 0x%02x,0x%02x,0x%02x",
										 angband_color_table[a][0],
										 angband_color_table[a][1],
										 angband_color_table[a][2],
										 angband_color_table[a][3]));

				/* Prompt */
				Term_putstr(0, 14, -1, TERM_WHITE,
							   "Command (n/N/k/K/r/R/g/G/b/B): ");

				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') a = (byte)(a + 1);
				if (i == 'N') a = (byte)(a - 1);
				if (i == 'k') angband_color_table[a][0] = (byte)(angband_color_table[a][0] + 1);
				if (i == 'K') angband_color_table[a][0] = (byte)(angband_color_table[a][0] - 1);
				if (i == 'r') angband_color_table[a][1] = (byte)(angband_color_table[a][1] + 1);
				if (i == 'R') angband_color_table[a][1] = (byte)(angband_color_table[a][1] - 1);
				if (i == 'g') angband_color_table[a][2] = (byte)(angband_color_table[a][2] + 1);
				if (i == 'G') angband_color_table[a][2] = (byte)(angband_color_table[a][2] - 1);
				if (i == 'b') angband_color_table[a][3] = (byte)(angband_color_table[a][3] + 1);
				if (i == 'B') angband_color_table[a][3] = (byte)(angband_color_table[a][3] - 1);

				/* Hack -- react to changes */
				Term_xtra(TERM_XTRA_REACT, 0);

				/* Hack -- redraw */
				Term_redraw();
			}
		}

#endif

		/* Unknown option */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}


/*
 * Note something in the message recall
 */
void do_cmd_note(void)
{
	char buf[80];

	/* Default */
	strcpy(buf, "");

	/* Input */
	if (!get_string("Note: ", buf, 60)) return;

	/* Ignore empty notes */
	if (!buf[0] || (buf[0] == ' ')) return;

	/* Add the note to the message recall */
	msg_format("Note: %s", buf);
}


/*
 * Mention the current version
 */
void do_cmd_version(void)
{
	/* Silly message */
	msg_format("You are playing RAngband %d.%d.%d.  Type '?' for more info.",
				  VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}



/*
 * Array of feeling strings
 */
static cptr do_cmd_feeling_text[12] =
{
	"A chill runs down your spine...",
	"You feel there is something special about this level.",
	"Terrible visions of your death swim before you...",
	"You are gripped by a mounting fear...",
	"The air is thick with tension...",
	"You have a bad feeling about this...",
	"Something feels wrong...",
	"You feel nervous...",
	"You feel uneasy...",
	"This level looks reasonably safe...",
	"What a boring place..."
};


/*
 * Note that "feeling" is set to zero unless some time has passed.
 * Note that this is done when the level is GENERATED, not entered.
 */
void do_cmd_feeling(void)
{
	/* Verify the feeling */
	if (feeling < 0) feeling = 0;
	if (feeling > 10) feeling = 10;

	/* No useful feeling in town */
	if (!p_ptr->depth)
	{
		msg_print("You feel like venturing downstairs.");
		return;
	}

	/* Display the feeling */
	msg_print(do_cmd_feeling_text[feeling]);
}





/*
 * Encode the screen colors
 */
static char hack[17] = "dwsorgbuDWvyRGBU";


/*
 * Hack -- load a screen dump from a file
 */
void do_cmd_load_screen(void)
{
	int i, y, x;

	byte a = 0;
	char c = ' ';

	bool okay = TRUE;

	FILE *fff;

	char buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, "dump.txt");

	/* Append to the file */
	fff = my_fopen(buf, "r");

	/* Oops */
	if (!fff) return;


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Clear the screen */
	Term_clear();


	/* Load the screen */
	for (y = 0; okay && (y < 24); y++)
	{
		/* Get a line of data */
		if (my_fgets(fff, buf, 1024)) okay = FALSE;

		/* Show each row */
		for (x = 0; x < 79; x++)
		{
			/* Put the attr/char */
			Term_draw(x, y, TERM_WHITE, buf[x]);
		}
	}

	/* Get the blank line */
	if (my_fgets(fff, buf, 1024)) okay = FALSE;


	/* Dump the screen */
	for (y = 0; okay && (y < 24); y++)
	{
		/* Get a line of data */
		if (my_fgets(fff, buf, 1024)) okay = FALSE;

		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Look up the attr */
			for (i = 0; i < 16; i++)
			{
				/* Use attr matches */
				if (hack[i] == buf[x]) a = i;
			}

			/* Put the attr/char */
			Term_draw(x, y, a, c);
		}

		/* End the row */
		fprintf(fff, "\n");
	}


	/* Get the blank line */
	if (my_fgets(fff, buf, 1024)) okay = FALSE;


	/* Close it */
	my_fclose(fff);


	/* Message */
	msg_print("Screen dump loaded.");
	msg_print(NULL);


	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}


/*
 * Hack -- save a screen dump to a file
 */
void do_cmd_save_screen(void)
{
	int y, x;

	byte a = 0;
	char c = ' ';

	FILE *fff;

	char buf[1024];

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, "dump.txt");

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Hack -- drop permissions */
	safe_setuid_drop();

	/* Append to the file */
	fff = my_fopen(buf, "w");

	/* Hack -- grab permissions */
	safe_setuid_grab();

	/* Oops */
	if (!fff) return;


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();


	/* Dump the screen */
	for (y = 0; y < 24; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* Terminate */
		buf[x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	/* Skip a line */
	fprintf(fff, "\n");


	/* Dump the screen */
	for (y = 0; y < 24; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = hack[a&0x0F];
		}

		/* Terminate */
		buf[x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	/* Skip a line */
	fprintf(fff, "\n");


	/* Close it */
	my_fclose(fff);


	/* Message */
	msg_print("Screen dump saved.");
	msg_print(NULL);


	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}




/*
 * Display known artifacts
 */
static void do_cmd_knowledge_artifacts(void)
{
	int i, k, z, x, y;

	FILE *fff;

	char file_name[1024];

	char base_name[80];

	bool okay[MAX_A_IDX];


	/* Temporary file */
	if (path_temp(file_name, 1024)) return;

	/* Open a new file */
	fff = my_fopen(file_name, "w");

	/* Scan the artifacts */
	for (k = 0; k < MAX_A_IDX; k++)
	{
		artifact_type *a_ptr = &a_info[k];

		/* Default */
		okay[k] = FALSE;

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if (!a_ptr->cur_num) continue;

		/* Assume okay */
		okay[k] = TRUE;
	}

	/* Check the dungeon */
	for (y = 0; y < DUNGEON_HGT; y++)
	{
		for (x = 0; x < DUNGEON_WID; x++)
		{
			s16b this_o_idx, next_o_idx = 0;

			/* Scan all objects in the grid */
			for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Ignore non-artifacts */
				if (!artifact_p(o_ptr)) continue;

				/* Ignore known items */
				if (object_known_p(o_ptr)) continue;

				/* Note the artifact */
				okay[o_ptr->name1] = FALSE;
			}
		}
	}

	/* Check objects held by monsters */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Ignore non-artifacts */
		if (!artifact_p(o_ptr)) continue;

		/* Ignore known items */
		if (object_known_p(o_ptr)) continue;

		/* Monster */
		if (o_ptr->held_m_idx) okay[o_ptr->name1] = FALSE;
	}

	/* Check the inventory and equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Ignore non-objects */
		if (!o_ptr->k_idx) continue;

		/* Ignore non-artifacts */
		if (!artifact_p(o_ptr)) continue;

		/* Ignore known items */
		if (object_known_p(o_ptr)) continue;

		/* Note the artifact */
		okay[o_ptr->name1] = FALSE;
	}

	/* Scan the artifacts */
	for (k = 0; k < MAX_A_IDX; k++)
	{
		artifact_type *a_ptr = &a_info[k];

		/* List "dead" ones */
		if (!okay[k]) continue;

		/* Paranoia */
		strcpy(base_name, "Unknown Artifact");

		/* Obtain the base object type */
		z = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Real object */
		if (z)
		{
			object_type *i_ptr;
			object_type object_type_body;

			/* Get local object */
			i_ptr = &object_type_body;

			/* Create fake object */
			object_prep(i_ptr, z);

			/* Make it an artifact */
			i_ptr->name1 = k;

			/* Describe the artifact */
			object_desc_store(base_name, i_ptr, FALSE, 0);
		}

		/* Hack -- Build the artifact name */
		fprintf(fff, "     The %s\n", base_name);
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
	show_file(file_name, "Known artifacts", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known uniques
 *
 * Note that the player ghosts are ignored.  XXX XXX XXX
 */
static void do_cmd_knowledge_uniques(void)
{
	int k;

	FILE *fff;

	char file_name[1024];


	/* Temporary file */
	if (path_temp(file_name, 1024)) return;

	/* Open a new file */
	fff = my_fopen(file_name, "w");

	/* Scan the monster races */
	for (k = 1; k < RACE_MAX_NORMAL; k++)
	{
		monster_race *r_ptr = &r_info[k];
		monster_lore *l_ptr = &l_info[k];

		/* Only print Uniques */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			bool dead = (r_ptr->max_num == 0);

			/* Only display "known" uniques */
			if (dead || cheat_know || l_ptr->sights)
			{
				/* Print a message */
				fprintf(fff, "     %s is %s\n",
				(r_name + r_ptr->name),
				(dead ? "dead" : "alive"));
			}
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
	show_file(file_name, "Known Uniques", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known objects
 */
static void do_cmd_knowledge_objects(void)
{
	int k;

	FILE *fff;

	char o_name[80];

	char file_name[1024];


	/* Temporary file */
	if (path_temp(file_name, 1024)) return;

	/* Open a new file */
	fff = my_fopen(file_name, "w");

	/* Scan the object kinds */
	for (k = 1; k < MAX_K_IDX; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Hack -- skip artifacts */
		if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;

		/* List known flavored objects */
		if (k_ptr->has_flavor && k_ptr->aware)
		{
			object_type *i_ptr;
			object_type object_type_body;

			/* Get local object */
			i_ptr = &object_type_body;

			/* Create fake object */
			object_prep(i_ptr, k);

			/* Describe the object */
			object_desc_store(o_name, i_ptr, FALSE, 0);

			/* Print a message */
			fprintf(fff, "     %s\n", o_name);
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
	show_file(file_name, "Known Objects", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}


/* Pluralize a name */
static void plural_aux(char *name)
{
	 int len = strlen(name);

	 if (strstr(name, "coins"))
	 {
		  char buf[80];
		  strcpy (buf, "piles of ");
		  strcat (buf, name);
		  strcpy (name, buf);
		  return;
	 }
	 else if (strstr(name, "Manes"))
	 {
		  return;
	 }
	 else if (name[len-1]=='y')
	 {
		  strcpy(&(name[len-1]), "ies");
	 }
	 else if (streq(&(name[len-4]), "ouse"))
	 {
		  strcpy (&(name[len-4]), "ice");
	 }
	 else if (streq(&(name[len-3]), "man"))
	 {
		  strcpy (&(name[len-3]), "men");
	 }
	 else if (streq(&(name[len-2]), "ex"))
	 {
		  strcpy (&(name[len-2]), "ices");
	 }
	 else if (streq(&(name[len-3]), "olf"))
	 {
		  strcpy (&(name[len-3]), "olves");
	 }
	 else if ((streq(&(name[len-2]), "ch")) || (name[len-1] == 's'))
	 {
		  strcpy (&(name[len]), "es");
	 }
	 else
	 {
		  strcpy (&(name[len]), "s");
	 }
}


/*
 * Total kill count
 *
 * Ignore player ghosts.
 */
static void do_cmd_knowledge_kill_count(void)
{
	int k;

	FILE *fff;

	char buf[80];

	char file_name[1024];

	s16b t;
	u32b total = 0;

	/* Temporary file */
	if (path_temp(file_name, 1024)) return;

	/* Open a new file */
	fff = my_fopen(file_name, "w");

	/* Monsters slain */
	for (k = 1; k < RACE_MAX_NORMAL; k++)
	{
		monster_race *r_ptr = &r_info[k];
		monster_lore *l_ptr = &l_info[k];

		/* Count the number killed */
		t = l_ptr->pkills;

		if (t > 0)
		{
			total += t;
		}
	}

	if (total < 1)
	{
		fprintf(fff,"You have defeated no enemies yet.\n\n");
	}
	else if (total == 1)
	{
		fprintf(fff,"You have defeated one enemy.\n\n");
	}
	else
	{
		fprintf(fff,"You have defeated %lu enemies.\n\n", total);
	}

	/* Scan the monster races */
	for (k = 1; k < RACE_MAX_NORMAL; k++)
	{
		monster_race *r_ptr = &r_info[k];
		monster_lore *l_ptr = &l_info[k];

		if (!(r_ptr->flags1 & RF1_LIMIT) && (r_ptr->flags1 & RF1_UNIQUE))
		{
			/* Unique is dead */
			if (r_ptr->max_num == 0)
			{
				/* Print a message */
				fprintf(fff, "     %s\n", (r_name + r_ptr->name));
			}

			/* Count deaths by this monster */
			t = l_ptr->deaths;

			/* We've been killed */
			if (t > 0)
			{
				/* Choose correct pronoun */
				if (r_ptr->flags1 & RF1_MALE) strcpy(buf, "He");
				else if (r_ptr->flags1 & RF1_FEMALE) strcpy(buf, "She");
				else strcpy(buf, "It");

				/* Unique is alive */
				if (r_ptr->max_num != 0)
				{
					/* Print a message */
					fprintf(fff, "     %s is still alive.\n", (r_name + r_ptr->name));
				}

				fprintf(fff, "          %s %s slain %d of your ancestors.\n",
						  buf, ((r_ptr->max_num == 0) ? "had" : "has"), t);
			}
		}
		else
		{
			/* Count the number killed */
			t = l_ptr->pkills;

			if (t > 0)
			{
				if (t < 2)
				{
					/* Creeping coins have a special identifier */
					if (strstr(r_name + r_ptr->name, "coins"))
					{
						fprintf(fff, "     1 pile of %s\n", (r_name + r_ptr->name));
					}
					/* Normal monster */
					else
					{
						fprintf(fff, "     1 %s\n", (r_name + r_ptr->name));
					}
				}
				else
				{
					/* Pluralize */
					strcpy(buf, (r_name + r_ptr->name));
					plural_aux(buf);
					fprintf(fff, "     %d %s\n", t, buf);
				}
			}

			/* Count deaths by this monster */
			t = l_ptr->deaths;

			/* We've been killed */
			if (t > 0)
			{
				/* No kills at all */
				if (l_ptr->tkills == 0)
				{
					/* Pluralize */
					strcpy(buf, (r_name + r_ptr->name));
					plural_aux(buf);

					/* Print a message */
					fprintf(fff, "     No %s have been defeated.\n", buf);
				}
				/* Our ancestors killed some */
				else if (l_ptr->pkills == 0)
				{
					/* Pluralize */
					strcpy(buf, (r_name + r_ptr->name));
					if (l_ptr->tkills > 1) plural_aux(buf);

					/* Print a message */
					fprintf(fff, "     Your ancestors have defeated %d %s.\n",
							  l_ptr->tkills, buf);
				}

				fprintf(fff, "          They have slain %d of your ancestors.\n", t);
			}
		}
	}

	fprintf(fff,"----------------------------------------------\n");

	/* Don't display trivial totals */
	if (total > 1) fprintf(fff,"   Total: %lu creature%s killed.\n", total, ((total == 1) ? "" : "s"));

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
	show_file(file_name, "Kill Count", 0, 0);

	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Interact with "knowledge"
 */
void do_cmd_knowledge(void)
{
	int i;


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
		prt("Display current knowledge", 2, 0);

		/* Give some choices */
		prt("(1) Display known artifacts", 4, 5);
		prt("(2) Display known uniques", 5, 5);
		prt("(3) Display known objects", 6, 5);
		prt("(4) Display kill count", 7, 5);

		/* Prompt */
		prt("Command: ", 9, 0);

		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		/* Artifacts */
		if (i == '1')
		{
			/* Spawn */
			do_cmd_knowledge_artifacts();
		}

		/* Uniques */
		else if (i == '2')
		{
			/* Spawn */
			do_cmd_knowledge_uniques();
		}

		/* Objects */
		else if (i == '3')
		{
			/* Spawn */
			do_cmd_knowledge_objects();
		}

		/* Kill count */
		else if (i == '4')
		{
			/* Spawn */
			do_cmd_knowledge_kill_count();
		}

		/* Unknown option */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}


