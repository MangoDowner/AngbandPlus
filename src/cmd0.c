/*
 * File: cmd0.c
 * Purpose: Deal with command processing.
 *
 * Copyright (c) 2007 Andrew Sidwell, Ben Harrison
 *
 * This work is free software; you can redistribute it and/or modify it
 * under the terms of either:
 *
 * a) the GNU General Public License as published by the Free Software
 *    Foundation, version 2, or
 *
 * b) the "Angband licence":
 *    This software may be copied and distributed for educational, research,
 *    and not for profit purposes provided that this copyright and statement
 *    are included in all such copies.  Other copyrights may also apply.
 */
#include "angband.h"
#include "wizard.h"
#include "cmds.h"
#include "ui-menu.h"


/*
 * This file contains (several) big lists of commands, so that they can be
 * easily maniuplated for e.g. help displays, or if a port wants to provide a
 * native menu containing a command list.
 *
 * Consider a two-paned layout for the command menus. XXX
 *
 * This file still needs some clearing up. XXX
 */

/*** Big list of commands ***/

/* Useful typedef */
typedef void do_cmd_type(void);


/* Forward declare these, because they're really defined later */
static do_cmd_type do_cmd_wizard, do_cmd_try_debug,
            do_cmd_quit, do_cmd_mouseclick, do_cmd_port,
			do_cmd_xxx_options, do_cmd_menu, do_cmd_monlist, do_cmd_itemlist;

#ifdef ALLOW_BORG
static do_cmd_type do_cmd_try_borg;
#endif

/*
 * Holds a generic command.
 */
typedef struct
{
	const char *desc;
	unsigned char key;
	do_cmd_type *hook;
} command_type;


/* Magic use */
static command_type cmd_magic[] =
{
/*	{ "Gain new spells or prayers", 'G', do_cmd_study }, */
	{ "Browse a book",              'b', do_cmd_browse },
	{ "Cast a spell",               'm', do_cmd_cast },
	{ "Pray a prayer",              'p', do_cmd_pray }
};

/* General actions */
static command_type cmd_action[] =
{
	{ "Search for traps/doors",     's', do_cmd_search },
	{ "Disarm a trap or chest",     'D', do_cmd_disarm },
	{ "Rest for a while",           'R', do_cmd_rest },
	{ "Look around",                'l', do_cmd_look },
	{ "Target monster or location", '*', do_cmd_target },
	{ "Dig a tunnel",               'T', do_cmd_tunnel },
	{ "Go up staircase",            '<', do_cmd_go_up },
	{ "Go down staircase",          '>', do_cmd_go_down },
	{ "Toggle search mode",         'S', do_cmd_toggle_search },
	{ "Open a door or a chest",     'o', do_cmd_open },
	{ "Close a door",               'c', do_cmd_close },
	{ "Jam a door shut",            'j', do_cmd_spike },
	{ "Bash a door open",           'B', do_cmd_bash }
};

/* Item use commands */
static command_type cmd_item_use[] =
{
	{ "Read a scroll",            'r', do_cmd_read_scroll },
	{ "Quaff a potion",           'q', do_cmd_quaff_potion },
	{ "Use a staff",              'u', do_cmd_use_staff },
	{ "Aim a wand",               'a', do_cmd_aim_wand },
	{ "Zap a rod",                'z', do_cmd_zap_rod },
	{ "Activate an object",       'A', do_cmd_activate },
	{ "Eat some food",            'E', do_cmd_eat_food },
	{ "Fuel your light source",   'F', do_cmd_refill },
	{ "Fire your missile weapon", 'f', do_cmd_fire },
	{ "Throw an item",            'v', do_cmd_throw }
};

/* Item management commands */
static command_type cmd_item_manage[]  =
{
	{ "Display equipment listing", 'e', do_cmd_equip },
	{ "Display inventory listing", 'i', do_cmd_inven },
	{ "Pick up objects",           'g', do_cmd_pickup },
	{ "Wear/wield an item",        'w', do_cmd_wield },
	{ "Take/unwield off an item",  't', do_cmd_takeoff },
	{ "Drop an item",              'd', do_cmd_drop },
	{ "Destroy an item",           'k', do_cmd_destroy },
	{ "Examine an item",           'I', do_cmd_observe },
	{ "Inscribe an object",        '{', do_cmd_inscribe },
	{ "Uninscribe an object",      '}', do_cmd_uninscribe }
};

/* Information access commands */
static command_type cmd_info[] =
{
	{ "Full dungeon map",             'M', do_cmd_view_map },
	{ "Display visible item list",    ']', do_cmd_itemlist },
	{ "Display visible monster list", '[', do_cmd_monlist },
	{ "Locate player on map",         'L', do_cmd_locate },
	{ "Help",                         '?', do_cmd_help },
	{ "Identify symbol",              '/', do_cmd_query_symbol },
	{ "Character description",        'C', do_cmd_change_name },
	{ "Check knowledge",              '~', do_cmd_knowledge },
	{ "Repeat level feeling",   KTRL('F'), do_cmd_feeling },
	{ "Show previous message",  KTRL('O'), do_cmd_message_one },
	{ "Show previous messages", KTRL('P'), do_cmd_messages }
};

/* Utility/assorted commands */
static command_type cmd_util[] =
{
	{ "Interact with options",        '=', do_cmd_xxx_options },
	{ "Port-specific preferences",    '!', do_cmd_port },

	{ "Save and don't quit",  KTRL('S'), do_cmd_save_game },
	{ "Save and quit",        KTRL('X'), do_cmd_quit },
	{ "Quit (commit suicide)",      'Q', do_cmd_suicide },
	{ "Redraw the screen",    KTRL('R'), do_cmd_redraw },

	{ "Load \"screen dump\"",       '(', do_cmd_load_screen },
	{ "Save \"screen dump\"",       ')', do_cmd_save_screen }
};

/* Commands that shouldn't be shown to the user */ 
static command_type cmd_hidden[] =
{
	{ "Take notes",               ':', do_cmd_note },
	{ "Version info",             'V', do_cmd_version },
	{ "Load a single pref line",  '"', do_cmd_pref },
	{ "Mouse click",           (unsigned char) '\xff', do_cmd_mouseclick }, /* Does this work? Note that '\xff' is being interpretted as an int */
	{ "Enter a store",            '_', do_cmd_store },
	{ "Toggle windows",     KTRL('E'), toggle_inven_equip }, /* XXX */
	{ "Alter a grid",             '+', do_cmd_alter },
	{ "Walk",                     ';', do_cmd_walk },
	{ "Jump into a trap",         '-', do_cmd_jump },
	{ "Start running",            '.', do_cmd_run },
	{ "Stand still",              ',', do_cmd_hold },
	{ "Check knowledge",          '|', do_cmd_knowledge },
	{ "Display menu of actions", '\n', do_cmd_menu },
	{ "Display menu of actions", '\r', do_cmd_menu },

	{ "Toggle wizard mode",  KTRL('W'), do_cmd_wizard },

#ifdef ALLOW_DEBUG
	{ "Debug mode commands", KTRL('A'), do_cmd_try_debug },
#endif
#ifdef ALLOW_BORG
	{ "Borg commands",       KTRL('Z'), do_cmd_try_borg }
#endif
};

#if 0
/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 */
static void process_command(void)
{
dlog(DEBUGKEYS,"dungeon.c: process_command: command_cmd %d ('%c')\n", (int)p_ptr->command_cmd, p_ptr->command_cmd);
   /* Parse the command */
   switch (p_ptr->command_cmd)
   {
      /* commands that are not always allowed: */
#ifdef ALLOW_WIZARD
      /* Special "debug" commands */
      case KTRL('A'): do_cmd_wizard(); break;
#endif
#ifdef ALLOW_BORG

      /* Special "borg" commands */
      case KTRL('B'):
      {
         if (verify_borg_mode()) do_cmd_borg();
         break;
      }

#endif


#ifndef VERIFY_SAVEFILE
      /* Hack -- Save and don't quit */
      case KTRL('S'):
      case ('$'):
         do_cmd_save_game(TRUE);
         break;

      case KTRL('L'):
      case KTRL('Q'): signals_handle_tstp(); quit(NULL); /* quit without saving */
#endif

      /* Ignore */
      case ESCAPE:
      case ' ':
      case '\r':
      case '\n':
      case '\t':   break;

	  /* MAGIC */
      case 'b': do_cmd_browse();                     break; /* Browse a book                        */

	  case '#': do_cmd_check();                      break; /* Check artifacts/uniques/whatever     */
      case '(': do_cmd_load_screen();                break; /* Load "screen dump"                   */
      case ')': do_cmd_save_screen();                break; /* Save "screen dump"                   */
      case '*': do_cmd_target();                     break; /* Target monster or location           */
      case '+': do_cmd_tunnel();                     break; /* Dig a tunnel                         */
      case ',': do_cmd_hold();                       break; /* Stay still (usually do not pick up)  */
      case '-': do_cmd_walk(!always_pickup);         break; /* Move (usually do not pick up)        */
      case '.': do_cmd_run();                        break; /* Begin Running -- Arg is Max Distance */
      case '/': do_cmd_query_symbol();               break; /* Identify symbol                      */
      case ':': do_cmd_note();                       break; /* Take notes                           */
      case ';': do_cmd_walk(always_pickup);          break; /* Move (usually pick up things)        */
      case '<': do_cmd_go_up();                      break; /* Go up staircase                      */
      case '>': do_cmd_go_down();                    break; /* Go down staircase                    */
      case '?': do_cmd_help("help.hlp");             break; /* Help                                 */
      case '@': do_cmd_system_command();             break; /* Interact with the options etc.       */
      case 'A': do_cmd_activate();                   break; /* Activate an artifact                 */
      case 'B': do_cmd_bash();                       break; /* Bash a door                          */
      case 'C': do_cmd_change_name();                break; /* Character description                */
      case 'D': do_cmd_disarm();                     break; /* Disarm a trap or chest               */
      case 'E': do_cmd_eat_food();                   break; /* Eat some food                        */
      case 'F': do_cmd_refill();                     break; /* Fuel your lantern/torch              */
      case 'I': do_cmd_observe();                    break; /* Identify an object                   */
      case 'L': do_cmd_locate();                     break; /* Locate player on map                 */
      case 'M': do_cmd_view_map(TRUE);               break; /* scalable dungeon map                 */
      case 'Q': do_cmd_suicide();                    break; /* Quit (commit suicide)                */
      case 'R': do_cmd_rest();                       break; /* Rest -- Arg is time                  */
      case 'S': do_cmd_toggle_search();              break; /* Toggle search mode                   */
      case 'T': do_cmd_tunnel();                     break; /* Dig a tunnel                         */
      case 'V': do_cmd_version();                    break; /* Version info                         */
      case '_': do_cmd_store();                      break; /* Enter store                          */
      case 'c': do_cmd_close();                      break; /* Close a door                         */
      case 'd': do_cmd_drop();                       break; /* Drop an item                         */
      case 'e': do_cmd_equip();                      break; /* Equipment list                       */
      case 'f': do_cmd_fire();                       break; /* Fire an item                         */
      case 'g': do_cmd_stay();                       break; /* Stay still (usually do not pick up)  */
      case 'i': do_cmd_inven();                      break; /* Inventory list                       */
      case 'j': do_cmd_spike();                      break; /* Jam a door with spikes               */
      case 'k': do_cmd_destroy();                    break; /* Destroy an item                      */
      case 'l': do_cmd_look();                       break; /* Look around                          */
      case 'm': do_cmd_cast_spell();                 break; /* use a spell                          */
      case 'o': do_cmd_open();                       break; /* Open a door or chest                 */
      case 'q': do_cmd_quaff_potion();               break; /* Quaff a potion                       */
      case 'r': do_cmd_read_scroll();                break; /* Read a scroll                        */
      case 's': do_cmd_search();                     break; /* Search for traps/doors               */
      case 't': do_cmd_takeoff();                    break; /* Take off equipment                   */
      case 'v': do_cmd_throw();                      break; /* Throw an item                        */
      case 'w': do_cmd_wield();                      break; /* Wear/wield equipment                 */
      case 'z': do_cmd_zap();                        break; /* Zap a rod                            */
      case '{': do_cmd_inscribe();                   break; /* Inscribe an object                   */
      case '}': do_cmd_uninscribe();                 break; /* Uninscribe an object                 */

      /* control key sequences: */

      case KTRL('F'): do_cmd_feeling();            break; /* Repeat level feeling */
      case KTRL('O'): do_cmd_message_one();        break; /* Show previous message */
      case KTRL('P'): do_cmd_messages();           break; /* Show previous messages */
      case KTRL('R'): do_cmd_redraw();             break; /* Redraw the screen */
      case KTRL('X'): alive = FALSE;               break; /* Save and quit */
      case KTRL('W'): do_cmd_toggle_wizard_mode(); break; /* Toggle Wizard Mode */

      default:
      {
         if (show_key_help && get_check("Unknown key! Display keyboard Usage Reminder? "))
         {
            show_file("quickkey.txt", "A Quick keyboard usage reminder");
         }
         else
         {
            prt("Press ? for help.", 0, MESSAGE_ROW); return;      /* Hack -- Unknown command */
         }
      }
   }
}
#endif



/*
 * A categorised list of all the command lists.
 */
typedef struct
{
	const char *name;
	command_type *list;
	size_t len;
} command_list;

static command_list cmds_all[] =
{
	{ "Use magic/Pray",  cmd_magic,       N_ELEMENTS(cmd_magic) },
	{ "Action commands", cmd_action,      N_ELEMENTS(cmd_action) },
	{ "Use item",        cmd_item_use,    N_ELEMENTS(cmd_item_use) },
	{ "Manage items",    cmd_item_manage, N_ELEMENTS(cmd_item_manage) },
	{ "Information",     cmd_info,        N_ELEMENTS(cmd_info) },
	{ "Utility",         cmd_util,        N_ELEMENTS(cmd_util) },
	{ "Hidden",          cmd_hidden,      N_ELEMENTS(cmd_hidden) }
};





/*
 * Toggle wizard mode
 */
static void do_cmd_wizard(void)
{
	/* Verify first time */
	if (!(p_ptr->noscore & NOSCORE_WIZARD))
	{
		/* Mention effects */
		msg_print("You are about to enter 'wizard' mode for the very first time!");
		msg_print("This is a form of cheating, and your game will not be scored!");
		message_flush();

		/* Verify request */
		if (!get_check("Are you sure you want to enter wizard mode? "))
			return;

		/* Mark savefile */
		p_ptr->noscore |= NOSCORE_WIZARD;
	}

	/* Toggle mode */
	if (p_ptr->wizard)
	{
		p_ptr->wizard = FALSE;
		msg_print("Wizard mode off.");
	}
	else
	{
		p_ptr->wizard = TRUE;
		msg_print("Wizard mode on.");
	}

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw "title" */
	p_ptr->redraw |= (PR_TITLE);
}




#ifdef ALLOW_DEBUG

/*
 * Verify use of "debug" mode
 */
static void do_cmd_try_debug(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & NOSCORE_DEBUG))
	{
		/* Mention effects */
		msg_print("You are about to use the dangerous, unsupported, debug commands!");
		msg_print("Your machine may crash, and your savefile may become corrupted!");
		message_flush();

		/* Verify request */
		if (!get_check("Are you sure you want to use the debug commands? "))
			return;

		/* Mark savefile */
		p_ptr->noscore |= NOSCORE_DEBUG;
	}

	/* Okay */
	do_cmd_debug();
}

#endif /* ALLOW_DEBUG */



#ifdef ALLOW_BORG

/*
 * Verify use of "borg" mode
 */
static void do_cmd_try_borg(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & NOSCORE_BORG))
	{
		/* Mention effects */
		msg_print("You are about to use the dangerous, unsupported, borg commands!");
		msg_print("Your machine may crash, and your savefile may become corrupted!");
		message_flush();

		/* Verify request */
		if (!get_check("Are you sure you want to use the borg commands? "))
			return;

		/* Mark savefile */
		p_ptr->noscore |= NOSCORE_BORG;
	}

	/* Okay */
	do_cmd_borg();
}

#endif /* ALLOW_BORG */


/*
 * Quit the game.
 */
static void do_cmd_quit(void)
{
	/* Stop playing */
	p_ptr->playing = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Handle a mouseclick, using the horrible hack that is '\xff'.
 */
static void do_cmd_mouseclick(void)
{
	s16b x, y;

	if (!OPT(mouse_movement)) return;

	y = KEY_GRID_Y(p_ptr->command_cmd_ex);
	x = KEY_GRID_X(p_ptr->command_cmd_ex);

	/* Check for a valid location */
	if (!in_bounds_fully(y, x)) return;

	/* XXX We could try various things here like going up/down stairs */
	if ((p_ptr->py == y) && (p_ptr->px == x) /* && (p_ptr->command_cmd_ex.mousebutton) */)
	{
		do_cmd_rest();
	}
	else /* if (p_ptr->command_cmd_ex.mousebutton == 1) */
	{
		if (p_ptr->timed[TMD_CONFUSED])
		{
			do_cmd_walk();
		}
		else
		{
			do_cmd_pathfind(y, x);
		}
	}
	/*
	else if (p_ptr->command_cmd_ex.mousebutton == 2)
	{
		target_set_location(y, x);
		msg_print("Target set.");
	}
	*/
}


/*
 * Port-specific options
 *
 * Should be moved to the options screen. XXX
 */
static void do_cmd_port(void)
{
	(void)Term_user(0);
}


/*
 * Display the options and redraw afterward.
 */
static void do_cmd_xxx_options(void)
{
	do_cmd_options();
	do_cmd_redraw();
}


/*
 * Display the main-screen monster list.
 */
static void do_cmd_monlist(void)
{
	/* Save the screen and display the list */
	screen_save();
	display_monlist();

	/* Wait */
	anykey();

	/* Return */
	screen_load();
}


/*
 * Display the main-screen item list.
 */
static void do_cmd_itemlist(void)
{
	/* Save the screen and display the list */
	screen_save();
	display_itemlist();

	/* Wait */
	anykey();

	/* Return */
	screen_load();
}


/*
 * Invoked when the command isn't recognised.
 */
static void do_cmd_unknown(void)
{
	prt("Type '?' for help.", 0, 0);
}




/* List indexed by char */
do_cmd_type *converted_list[UCHAR_MAX+1];


/*** Menu functions ***/

/* Display an entry on a command menu */
static void cmd_sub_entry(menu_type *menu, int oid, bool cursor, s16b row, s16b col, s16b width)
{
	byte attr = (cursor ? TERM_L_BLUE : TERM_WHITE);
	const command_type *commands = menu->menu_data;

	(void)width;

	/* Write the description */
	Term_putstr(col, row, -1, attr, commands[oid].desc);

	/* Include keypress */
	Term_addch(attr, ' ');
	Term_addch(attr, '(');

	/* KTRL()ing a control character does not alter it at all */
	if (KTRL(commands[oid].key) == commands[oid].key)
	{
		Term_addch(attr, '^');
		Term_addch(attr, UN_KTRL(commands[oid].key));
	}
	else
	{
		Term_addch(attr, commands[oid].key);
	}

	Term_addch(attr, ')');
}


/* Handle user input from a command menu */
static bool cmd_sub_action(char cmd, void *db, int oid)
{
	(void)db;
	(void)oid;

	/* Only handle enter */
	if (cmd == '\n' || cmd == '\r')
		return TRUE;
	else
		return FALSE;
}

/*
 * Display a list of commands.
 */
static bool cmd_menu(command_list *list, void *selection_p)
{
	menu_type menu;
	menu_iter commands_menu = { NULL, NULL, cmd_sub_entry, cmd_sub_action };
	region area = { 23, 4, 37, 13 };

	ui_event_data evt;
	s16b cursor = 0;
	command_type *selection = selection_p;

	/* Set up the menu */
	WIPE(&menu, menu);
	menu.cmd_keys = "\x8B\x8C\n\r";
	menu.count = (s16b) list->len;
	menu.menu_data = list->list;
	menu_init(&menu, MN_SKIN_SCROLL, &commands_menu, &area);

	/* Set up the screen */
	screen_save();
	window_make(21, 3, 62, 17);

	/* Select an entry */
	evt = menu_select(&menu, &cursor, 0);

	/* Load de screen */
	screen_load();

	if (evt.type == EVT_SELECT)
	{
		*selection = list->list[evt.index];
	}

	if (evt.type == EVT_ESCAPE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}



static bool cmd_list_action(char cmd, void *db, int oid)
{
	if (cmd == '\n' || cmd == '\r' || cmd == '\xff')
	{
		return cmd_menu(&cmds_all[oid], db);
	}
	else
	{
		return FALSE;
	}
}

static void cmd_list_entry(menu_type *menu, int oid, bool cursor, s16b row, s16b col, s16b width)
{
	byte attr = (cursor ? TERM_L_BLUE : TERM_WHITE);
	(void)menu;
	(void)width;
	Term_putstr(col, row, -1, attr, cmds_all[oid].name);
}

/*
 * Display a list of command types, allowing the user to select one.
 */
static void do_cmd_menu(void)
{
	menu_type menu;
	menu_iter commands_menu = { NULL, NULL, cmd_list_entry, cmd_list_action };
	region area = { 21, 5, 37, 6 };

	ui_event_data evt;
	s16b cursor = 0;
	command_type chosen_command = { NULL, 0, NULL };

	/* Set up the menu */
	WIPE(&menu, menu);
	menu.cmd_keys = "\x8B\x8C\n\r";
	menu.count = N_ELEMENTS(cmds_all) - 1;
	menu.menu_data = &chosen_command;
	menu_init(&menu, MN_SKIN_SCROLL, &commands_menu, &area);

	/* Set up the screen */
	screen_save();
	window_make(19, 4, 58, 11);

	/* Select an entry */
	evt = menu_select(&menu, &cursor, 0);

	/* Load de screen */
	screen_load();

	/* If a command was chosen, do it. */
	if (chosen_command.hook)
	{
		chosen_command.hook();
	}
}


/*** Exported functions ***/

/*
 * Initialise the command list.
 */
void cmd_init(void)
{
	size_t i, j;

	/* Go through all the lists of commands */
	for (j = 0; j < N_ELEMENTS(cmds_all); j++)
	{
		command_type *commands = cmds_all[j].list;

		/* Fill everything in */
		for (i = 0; i < cmds_all[j].len; i++)
		{
			unsigned char key = commands[i].key;

			/* Note: at present converted_list is UCHAR_MAX + 1 
			   large, so 'key' is always a valid index. */
			converted_list[key] = commands[i].hook;
		}
	}

	/* Fill in the rest */
	for (i = 0; i < N_ELEMENTS(converted_list); i++)
	{
		switch (i)
		{
			/* Ignore */
			case ESCAPE:
			case ' ':
			case '\a':
			{
				break;
			}

			default:
			{
				if (!converted_list[i])
					converted_list[i] = do_cmd_unknown;
			}
		}		
	}
}


/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 */
void process_command(bool no_request)
{
	if (!no_request)
		request_command();

	/* Handle repeating the last command */
	repeat_check();

	/* Handle resize events XXX */
	if (p_ptr->command_cmd_ex.type == EVT_RESIZE)
	{
		do_cmd_redraw();
	}
	else
	{
		/* Within these boundaries, the cast to unsigned char will have the desired effect */
		assert(p_ptr->command_cmd >= CHAR_MIN && p_ptr->command_cmd <= CHAR_MAX);

		/* Execute the command */
		if (converted_list[(unsigned char) p_ptr->command_cmd])
			converted_list[(unsigned char) p_ptr->command_cmd]();
	}
}
