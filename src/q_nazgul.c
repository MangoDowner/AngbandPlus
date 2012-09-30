#undef cquest
#define cquest (quest[QUEST_NAZGUL])

bool quest_nazgul_gen_hook(int small)
{
        int x, y, try = 10000;

        if ((cquest.status != QUEST_STATUS_TAKEN) || (small) || (p_ptr->town_num != 1)) return (FALSE);

        /* Find a good position */
        while(try)
        {
                /* Get a random spot */
                y = randint(cur_hgt - 4) + 2;
                x = randint(cur_wid - 4) + 2;

                /* Is it a good spot ? */
                /* Not in player los */
                if ((!los(py, px, y, x)) && cave_empty_bold(y, x)) break;

                /* One less try */
                try--;
        }

        /* Place the nazgul */
        hack_allow_special = TRUE;
        place_monster_one(y, x, test_monster_name("Uvatha the Horseman"), 0, FALSE, MSTATUS_ENEMY);
        hack_allow_special = FALSE;

        return FALSE;
}
bool quest_nazgul_finish_hook(int q_idx)
{
        object_type forge, *q_ptr;

        if (q_idx != QUEST_NAZGUL) return FALSE;

        c_put_str(TERM_YELLOW, "I believe he will not come back! Thanks you.", 8, 0);
        c_put_str(TERM_YELLOW, "Some time ago a ranger gave me this,", 9, 0);
        c_put_str(TERM_YELLOW, "I believe this will help you on your quest.", 10, 0);

        q_ptr = &forge;
        object_prep(q_ptr, lookup_kind(TV_FOOD, SV_FOOD_ATHELAS));
        q_ptr->number = 6;
        object_aware(q_ptr);
        object_known(q_ptr);
        q_ptr->ident |= IDENT_STOREB;
        (void)inven_carry(q_ptr, FALSE);

        /* End the plot */
        *(quest[q_idx].plot) = QUEST_NULL;

        del_hook(HOOK_QUEST_FINISH, quest_nazgul_finish_hook);
        process_hooks_restart = TRUE;

        return TRUE;
}
bool quest_nazgul_dump_hook(int q_idx)
{
        if (cquest.status >= QUEST_STATUS_COMPLETED)
        {
                fprintf(hook_file, "\n You saved Bree from a dreadful Nazgul.");
        }
        return (FALSE);
}
bool quest_nazgul_forbid_hook(int q_idx)
{
        if (q_idx != QUEST_NAZGUL) return (FALSE);

        if (p_ptr->lev < 30)
        {
                c_put_str(TERM_WHITE, "I fear you are not ready for the next quest, come back later.", 8, 0);
                return (TRUE);
        }
        return (FALSE);
}
bool quest_nazgul_death_hook(int m_idx)
{
        int r_idx = m_list[m_idx].r_idx;

        if (cquest.status != QUEST_STATUS_TAKEN) return (FALSE);
        if (r_idx != test_monster_name("Uvatha the Horseman")) return (FALSE);

        cquest.status = QUEST_STATUS_COMPLETED;

        del_hook(HOOK_MONSTER_DEATH, quest_nazgul_death_hook);
        process_hooks_restart = TRUE;

        return (FALSE);
}
bool quest_nazgul_init_hook(int q_idx)
{
        if ((cquest.status >= QUEST_STATUS_TAKEN) && (cquest.status < QUEST_STATUS_FINISHED))
        {
                add_hook(HOOK_MONSTER_DEATH, quest_nazgul_death_hook, "nazgul_death");
                add_hook(HOOK_WILD_GEN, quest_nazgul_gen_hook, "nazgul_gen");
                add_hook(HOOK_QUEST_FINISH, quest_nazgul_finish_hook, "nazgul_finish");
        }
        add_hook(HOOK_CHAR_DUMP, quest_nazgul_dump_hook, "nazgul_dump");
        add_hook(HOOK_INIT_QUEST, quest_nazgul_forbid_hook, "nazgul_forbid");
        return (FALSE);
}
