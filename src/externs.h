/* File: externs.h */

/* Purpose: extern declarations (variables and functions) */

/*
 * Note that some files have their own header files
 * (z-virt.h, z-util.h, z-form.h, term.h, random.h)
 */


/*
 * Automatically generated "variable" declarations
 */

extern int max_macrotrigger;
extern char *macro_template;
extern char *macro_modifier_chr;
extern char *macro_modifier_name[MAX_MACRO_MOD];
extern char *macro_trigger_name[MAX_MACRO_TRIG];
extern char *macro_trigger_keycode[2][MAX_MACRO_TRIG];


/* 
 *  List for auto-picker/destroyer entries
 */
extern int max_autopick;
extern autopick_type autopick_list[MAX_AUTOPICK];

/* tables.c */
extern s16b ddd[9];
extern s16b ddx[10];
extern s16b ddy[10];
extern s16b ddx_ddd[9];
extern s16b ddy_ddd[9];
extern s16b cdd[8];
extern s16b ddx_cdd[8];
extern s16b ddy_cdd[8];
extern char hexsym[16];
extern char listsym[];
extern cptr color_char;
extern byte adj_mag_study[];
extern byte adj_mag_mana[];
extern byte adj_mag_fail[];
extern byte adj_mag_stat[];
extern byte adj_chr_gold[];
extern byte adj_int_dev[];
extern byte adj_wis_sav[];
extern byte adj_dex_dis[];
extern byte adj_int_dis[];
extern byte adj_dex_ta[];
extern byte adj_str_td[];
extern byte adj_dex_th[];
extern byte adj_str_th[];
extern byte adj_str_wgt[];
extern byte adj_str_hold[];
extern byte adj_str_dig[];
extern byte adj_str_blow[];
extern byte adj_dex_blow[];
extern byte adj_dex_blow_bare_hand[];
extern byte adj_dex_safe[];
extern byte adj_con_fix[];
extern byte adj_con_mhp[];
extern byte adj_chr_chm[];
extern byte adj_chr_give[];
extern byte adj_throwing[];
extern byte blows_table[12][12];
extern s16b arena_monsters[MAX_ARENA_MONS + 4];
extern byte arena_object[MAX_ARENA_MONS + 4][2];
extern owner_type owners[MAX_STORES][MAX_OWNERS];
extern byte extract_energy[200];
extern s32b player_exp[PY_MAX_LEVEL];
extern player_sex sex_info[MAX_SEXES];
extern player_race race_info[MAX_RACES];
extern player_class class_info[MAX_CLASS];
extern u32b fake_spell_flags[MAX_REALM][3];
extern s32b realm_choices[];
extern cptr realm_names[];
#ifdef JP
extern cptr E_realm_names[];
#endif
extern cptr spell_names[MAX_REALM][32];
extern int chest_traps[64];
extern cptr player_title[MAX_CLASS][PY_MAX_LEVEL/5];
extern cptr color_names[16];
extern cptr stat_names[A_MAX];
extern cptr stat_names_reduced[A_MAX];
extern cptr window_flag_desc[32];
extern option_type option_info[];
extern martial_arts ma_blows[MAX_MA];
extern martial_arts octopus_ma_blows[MAX_OCTOPUS_MA];
extern cptr game_inscriptions[];
extern cptr skill_lev_str[MAX_SKILL_LEVEL];
extern cptr skill_unlearned_str;
extern s16b skill_lev_var[MAX_SKILL_LEVEL];
extern byte conv_terrain2feat[MAX_WILDERNESS];
extern cptr silly_attacks[MAX_SILLY_ATTACK];
#ifdef JP
extern cptr silly_attacks_other[MAX_SILLY_ATTACK];
#endif
extern cptr elem_names[];
#ifdef JP
extern cptr E_elem_names[];
#endif
extern weather_type weather_table[WEATHER_TYPE_NUM][WEATHER_LEVEL_NUM];
extern cptr ethnicity_names[ETHNICITY_NUM];
extern special_blow_type special_blow_info[MAX_SB];
extern special_blow_type temple_blow_info[MAX_TEMPLE_SB];
extern cptr wt_desc[];
extern tarot_type tarot_info[];
extern scratch_card_type scratch_card_info[];
extern int a_to_tr[A_MAX];
extern int ob_to_tr[OB_MAX];
extern int octopus_equip_order[INVEN_TOTAL - INVEN_RARM];
extern int octopus_equip_label_offset[INVEN_TOTAL - INVEN_RARM];

/* variable.c */
extern cptr copyright[5];
extern byte t_ver_major;
extern byte t_ver_minor;
extern byte t_ver_patch;
extern byte t_ver_extra;
extern byte sf_extra;
extern u32b sf_system;
extern byte t_major;
extern byte t_minor;
extern byte t_patch;
extern u32b sf_when;
extern u16b sf_lives;
extern u16b sf_saves;
extern bool arg_fiddle;
extern bool arg_wizard;
extern bool arg_sound;
extern byte arg_graphics;
extern bool arg_monochrome;
extern bool arg_force_original;
extern bool arg_force_roguelike;
extern bool arg_bigtile;
extern bool character_generated;
extern bool character_dungeon;
extern bool character_loaded;
extern bool character_saved;
extern bool character_icky;
extern bool character_xtra;
extern u32b seed_flavor;
extern u32b seed_town;
extern s16b command_cmd;
extern s16b command_arg;
extern s16b command_rep;
extern s16b command_dir;
extern s16b command_see;
extern s16b command_gap;
extern s16b command_wrk;
extern s16b command_new;
extern s16b energy_use;
extern bool msg_flag;
extern s16b running;
extern s16b resting;
extern s16b cur_hgt;
extern s16b cur_wid;
extern s16b dun_level;
extern s16b num_repro;
extern s16b object_level;
extern s16b monster_level;
extern s16b base_level;
extern s32b turn;
extern s32b dungeon_turn;
extern s32b old_turn;
extern s32b old_battle;
extern bool use_sound;
extern bool use_bigtile;
extern s16b signal_count;
extern bool inkey_base;
extern bool inkey_xtra;
extern bool inkey_scan;
extern bool inkey_flag;
extern bool inkey_special;
extern s16b coin_type;
extern bool opening_chest;
extern bool shimmer_monsters;
extern bool shimmer_objects;
extern bool repair_monsters;
extern bool repair_objects;
extern s16b inven_nxt;
extern s16b inven_cnt;
extern s16b equip_cnt;
extern s16b o_max;
extern s16b o_cnt;
extern s16b m_max;
extern s16b m_cnt;
extern s16b hack_m_idx;
extern s16b hack_m_idx_ii;
extern int total_friends;
extern s32b friend_align_gne;
extern s32b friend_align_lnc;
extern int leaving_quest;
extern bool reinit_wilderness;
extern bool multi_rew;
extern char summon_kin_type;
extern bool hack_mind;
extern bool show_item_graph;
extern bool rogue_like_commands;
extern bool quick_messages;
extern bool auto_more;
extern bool command_menu;
extern bool other_query_flag;
extern bool carry_query_flag;
extern bool always_pickup;
extern bool always_repeat;
extern bool use_old_target;
extern bool depth_in_feet;
extern bool compress_savefile;
extern bool hilite_player;
extern bool ring_bell;
extern bool find_ignore_stairs;
extern bool find_ignore_doors;
extern bool find_cut;
extern bool find_examine;
extern bool disturb_near;
extern bool disturb_high;
extern bool disturb_move;
extern bool disturb_panel;
extern bool disturb_state;
extern bool disturb_minor;
extern bool check_abort;
extern bool avoid_other;
extern bool flush_disturb;
extern bool flush_failure;
extern bool fresh_before;
extern bool fresh_after;
extern bool fresh_message;
extern bool alert_hitpoint;
extern bool disturb_trap_detect;
extern bool alert_trap_detect;
extern bool view_yellow_lite;
extern bool view_bright_lite;
extern bool view_granite_lite;
extern bool view_special_lite;
extern bool new_ascii_graphics;
extern bool use_fake_monochrome;
extern bool target_pet;
extern bool display_path;
extern bool plain_pickup;

extern bool always_show_list;
extern bool powerup_home;
extern bool change_numeral;
extern bool allow_debug_opts;   /* Allow use of debug/cheat options */

extern bool display_mutations;     /* Skip mutations screen in 'C'haracter display */
extern bool plain_descriptions;
extern bool confirm_destroy;
extern bool confirm_wear;
extern bool confirm_quest;
extern bool disturb_pets;
extern bool view_perma_grids;
extern bool view_torch_grids;
extern bool view_unsafe_grids;
extern bool show_damage;
extern bool track_follow;
extern bool track_target;
extern bool stack_allow_items;
extern bool stack_force_notes;
extern bool stack_force_costs;
extern bool view_reduce_lite;
extern bool view_reduce_view;
extern bool manual_haggle;
extern bool auto_scum;
extern bool expand_look;
extern bool expand_list;
extern bool smart_learn;
extern bool smart_cheat;
extern bool show_labels;
extern bool show_weights;
extern bool show_choices;
extern bool cheat_peek;
extern bool cheat_hear;
extern bool cheat_room;
extern bool cheat_xtra;
extern bool cheat_know;
extern bool cheat_live;
extern bool cheat_save;
extern bool last_words;              /* Zangband options */
extern bool over_exert;
extern bool small_levels;
extern bool always_small_levels;
extern bool empty_levels;
extern bool player_symbols;
extern bool equippy_chars;
extern bool use_command;
extern bool center_player;
extern bool center_running;
extern bool destroy_items;
extern bool destroy_feeling;
extern bool destroy_identify;
extern bool leave_worth;
extern bool leave_equip;
extern bool leave_wanted;
extern bool leave_corpse;
extern bool leave_junk;
extern bool destroy_items;
extern bool leave_chest;
extern bool leave_special;
extern bool record_fix_art;
extern bool record_rand_art;
extern bool record_destroy_uniq;
extern bool record_fix_quest;
extern bool record_rand_quest;
extern bool record_maxdeapth;
extern bool record_stair;
extern bool record_buy;
extern bool record_sell;
extern bool record_danger;
extern bool record_arena;
extern bool record_ident;
extern bool record_named_pet;
extern char record_o_name[MAX_NLEN];
extern s32b record_turn;
extern byte hitpoint_warn;
extern byte delay_factor;
extern s16b autosave_freq;
extern bool autosave_t;
extern bool autosave_l;
extern byte feeling;
extern s16b rating;
extern bool good_item_flag;
extern bool closing_flag;
extern s16b panel_row_min, panel_row_max;
extern s16b panel_col_min, panel_col_max;
extern s16b panel_col_prt, panel_row_prt;
extern int py;
extern int px;
extern s16b target_who;
extern s16b target_col;
extern s16b target_row;
extern int player_uid;
extern int player_euid;
extern int player_egid;
extern char player_name[32];
extern char player_base[32];
extern char savefile[1024];
extern char savefile_base[40];
extern s16b lite_n;
extern s16b lite_y[LITE_MAX];
extern s16b lite_x[LITE_MAX];
extern s16b mon_lite_n;
extern s16b mon_lite_y[LITE_MAX];
extern s16b mon_lite_x[LITE_MAX];
extern s16b view_n;
extern byte view_y[VIEW_MAX];
extern byte view_x[VIEW_MAX];
extern s16b temp_n;
extern byte temp_y[TEMP_MAX];
extern byte temp_x[TEMP_MAX];
extern s16b macro__num;
extern cptr *macro__pat;
extern cptr *macro__act;
extern bool *macro__cmd;
extern char *macro__buf;
extern s16b quark__num;
extern cptr *quark__str;
extern u16b message__next;
extern u16b message__last;
extern u16b message__head;
extern u16b message__tail;
extern u16b *message__ptr;
extern char *message__buf;
extern u32b option_flag[8];
extern u32b option_mask[8];
extern u32b window_flag[8];
extern u32b window_mask[8];
extern term *angband_term[8];
extern char angband_term_name[8][16];
extern byte angband_color_table[256][4];
extern char angband_sound_name[SOUND_MAX][16];
extern cave_type *cave[MAX_HGT];
extern saved_floor_type saved_floors[MAX_SAVED_FLOORS];
extern s16b max_floor_id;
extern u32b saved_floor_file_sign;
extern object_type *o_list;
extern monster_type *m_list;
extern u16b max_towns;
extern town_type *town;
extern object_type *inventory;
extern s16b mw_old_weight;
extern s16b mw_diff_to_melee;
extern s16b alloc_kind_size;
extern alloc_entry *alloc_kind_table;
extern s16b alloc_race_size;
extern alloc_entry *alloc_race_table;
extern byte misc_to_attr[256];
extern char misc_to_char[256];
extern byte tval_to_attr[128];
extern char tval_to_char[128];
extern cptr keymap_act[KEYMAP_MODES][256];
extern player_type *p_ptr;
extern player_sex *sp_ptr;
extern player_race *rp_ptr;
extern player_class *cp_ptr;
extern player_magic *mp_ptr;
extern birther previous_char;
extern vault_type *v_info;
extern char *v_name;
extern char *v_text;
extern skill_table *s_info;
extern player_magic *m_info;
extern feature_type *f_info;
extern char *f_name;
extern object_kind *k_info;
extern char *k_name;
extern char *k_text;
extern artifact_type *a_info;
extern char *a_name;
extern char *a_text;
extern ego_item_type *e_info;
extern char *e_name;
extern char *e_text;
extern monster_race *r_info;
extern char *r_name;
extern char *r_text;
extern dungeon_info_type *d_info;
extern char *d_name;
extern char *d_text;
extern cptr ANGBAND_SYS;
extern cptr ANGBAND_KEYBOARD;
extern cptr ANGBAND_GRAF;
extern cptr ANGBAND_DIR;
extern cptr ANGBAND_DIR_APEX;
extern cptr ANGBAND_DIR_BONE;
extern cptr ANGBAND_DIR_DATA;
extern cptr ANGBAND_DIR_EDIT;
extern cptr ANGBAND_DIR_SCRIPT;
extern cptr ANGBAND_DIR_FILE;
extern cptr ANGBAND_DIR_HELP;
extern cptr ANGBAND_DIR_INFO;
extern cptr ANGBAND_DIR_PREF;
extern cptr ANGBAND_DIR_SAVE;
extern cptr ANGBAND_DIR_USER;
extern cptr ANGBAND_DIR_XTRA;
extern bool item_tester_full;
extern bool item_tester_no_ryoute;
extern byte item_tester_tval;
extern bool (*item_tester_hook)(object_type *o_ptr);
extern bool (*ang_sort_comp)(vptr u, vptr v, int a, int b);
extern void (*ang_sort_swap)(vptr u, vptr v, int a, int b);
extern monster_hook_type get_mon_num_hook;
extern monster_hook_type get_mon_num2_hook;
extern bool (*get_obj_num_hook)(int k_idx);
extern bool monk_armour_aux;
extern bool monk_notify_aux;
extern wilderness_type **wilderness;
extern building_type building[MAX_BLDG];
extern u16b max_quests;
extern u16b max_r_idx;
extern u16b max_k_idx;
extern u16b max_v_idx;
extern u16b max_f_idx;
extern u16b max_a_idx;
extern u16b max_e_idx;
extern u16b max_d_idx;
extern u16b max_o_idx;
extern u16b max_m_idx;
extern s32b max_wild_x;
extern s32b max_wild_y;
extern quest_type *quest;
extern char quest_text[10][80];
extern int quest_text_line;
extern s16b gf_color[MAX_GF];
extern int init_flags;
extern int highscore_fd;
extern int mutant_regenerate_mod;
extern bool easy_band;
extern bool ironman_shops;
extern bool ironman_small_levels;
extern bool ironman_forward;
extern bool ironman_autoscum;
extern bool ironman_empty_levels;
extern bool ironman_rooms;
extern bool ironman_nightmare;
extern bool left_hander;
extern bool preserve_mode;
extern bool can_save;
extern bool stop_the_time_monster;
extern bool stop_the_time_player;
extern int pet_t_m_idx;
extern int riding_t_m_idx;
extern s16b kubi_r_idx[MAX_KUBI];
extern s16b today_mon;
extern monster_type party_mon[21];
extern bool write_level;
extern u32b playtime;
extern u32b start_time;
extern bool mon_fight;
extern bool ambush_flag;
extern bool generate_encounter;
extern cptr screen_dump;
extern byte dungeon_type;
extern s16b *max_dlv;
extern byte feat_wall_outer;
extern byte feat_wall_inner;
extern byte feat_wall_solid;
extern byte floor_type[100], fill_type[100];
extern bool now_damaged;
extern s16b now_message;
extern bool use_menu;
extern s16b weather[WEATHER_TYPE_NUM];
extern s16b prev_weather[WEATHER_TYPE_NUM];
extern s16b weather_time_to_change;
extern s16b hack_elem_amount;
extern int hack_elem_mod_mode;
extern s16b num_anti_magic;
extern s16b *anti_magic_m_idx;
extern s16b num_fear_field;
extern s16b *fear_field_m_idx;
extern int penet_ac;
extern runeweapon_type runeweapon_list[MAX_RUNEWEAPON + 1];
extern byte runeweapon_num;
extern s16b chaos_frame[ETHNICITY_NUM];
extern byte fool_effect_status;
extern monster_type stock_mon[MAX_STOCK_MON];
extern bool astral_mode;
extern u32b misc_event_flags;
extern object_type *ancestor_inventory;
extern s16b ancestor_inven_cnt;

/* autopick.c */
extern cptr autopick_line_from_entry(autopick_type *entry);
extern int is_autopick(object_type *o_ptr);
extern bool auto_inscribe_object(object_type *o_ptr, int idx);
extern void auto_inscribe_item(int item, int idx);
extern bool auto_destroy_object(object_type *o_ptr, int autopick_idx);
extern bool auto_destroy_item(int item, int autopick_idx);
extern void delayed_auto_destroy(void);
extern void auto_pickup_items(cave_type *c_ptr);
extern void init_autopicker(void);
extern errr process_pickpref_file_line(char *buf);

/* birth.c */
extern cptr class_jouhou[MAX_CLASS];
extern int adjust_stat(int value, int amount);
extern void player_birth(void);
extern void player_outfit(void);
extern void dump_yourself(FILE *fff);

/* cave.c */
extern int distance(int y1, int x1, int y2, int x2);
extern bool is_trap(int feat);
extern bool is_known_trap(cave_type *c_ptr);
extern bool is_closed_door(int feat);
extern bool is_hidden_door(cave_type *c_ptr);
extern bool los(int y1, int x1, int y2, int x2);
extern bool player_can_see_bold(int y, int x);
extern bool cave_valid_bold(int y, int x);
extern bool cave_valid_grid(cave_type *c_ptr);
extern bool no_lite(void);
extern s16b get_dominant_feature_elem(cave_type *c_ptr);
extern byte elem_attr(s16b elem);
#ifdef USE_TRANSPARENCY
extern void map_info(int y, int x, byte *ap, char *cp, byte *tap, char *tcp);
#else /* USE_TRANSPARENCY */
extern void map_info(int y, int x, byte *ap, char *cp);
#endif /* USE_TRANSPARENCY */
extern void bigtile_attr(char *cp, byte *ap, char *cp2, byte *ap2);
extern void move_cursor_relative(int row, int col);
extern void print_rel(char c, byte a, int y, int x);
extern void note_spot(int y, int x);
extern void display_dungeon(void);
extern void lite_spot(int y, int x);
extern void prt_map(void);
extern void prt_path(int y, int x);
extern void display_map(int *cy, int *cx);
extern void do_cmd_view_map(void);
extern void forget_lite(void);
extern void update_lite(void);
extern void forget_view(void);
extern void update_view(void);
extern void update_mon_lite(void);
extern void clear_mon_lite(void);
extern void forget_flow(void);
extern void update_flow(void);
extern void update_smell(void);
extern void map_area(int range);
extern void wiz_lite(bool ninja);
extern void wiz_dark(bool do_darken);
extern void apply_grid_effect(int y, int x, byte prev_feat, bool update);
extern void cave_set_feat(int y, int x, int feat);
extern void cave_force_set_floor(int y, int x);
extern bool is_glyph_grid(cave_type *c_ptr);
extern bool is_explosive_rune_grid(cave_type *c_ptr);
extern void mmove2(int *y, int *x, int y1, int x1, int y2, int x2);
extern bool projectable(int y1, int x1, int y2, int x2);
extern void scatter(int *yp, int *xp, int y, int x, int d, int mode);
extern void health_track(int m_idx);
extern void monster_race_track(int r_idx);
extern void object_kind_track(int k_idx);
extern void disturb(int stop_search, int flush_output);
extern void glow_floor_feature(void);
extern void apply_default_features_effect(void);
extern void apply_weather_effect(bool init);
extern void update_amfield(void);

/* cmd1.c */
extern bool test_hit_fire(int chance, int ac, int vis);
extern bool test_hit_norm(int chance, int ac, int vis);
extern s16b critical_shot(int weight, int plus, int dam);
extern s16b critical_norm(int weight, int plus, int dam, s16b meichuu, int mode);
extern void class_weapon_flags(u32b flgs[TR_FLAG_SIZE]);
extern s16b tot_dam_div(object_type *o_ptr, monster_type *m_ptr, bool in_hand);
extern s16b tot_dam_aux(object_type *o_ptr, int tdam, monster_type *m_ptr, bool in_hand);
extern void search(void);
extern void py_pickup_aux(int o_idx);
extern void carry(int pickup);
extern bool py_attack(int y, int x, int mode);
extern bool player_can_enter(byte feature);
extern void fall_into_air(void);
extern void move_player(int dir, int do_pickup);
extern void run_step(int dir);

/* cmd2.c */
extern void do_cmd_go_up(void);
extern void do_cmd_go_down(void);
extern void do_cmd_search(void);
extern void do_cmd_open(void);
extern void do_cmd_close(void);
extern void do_cmd_tunnel(void);
extern void do_cmd_disarm(void);
extern void do_cmd_bash(void);
extern void do_cmd_alter(void);
extern void do_cmd_spike(void);
extern void do_cmd_walk(int pickup);
extern void do_cmd_stay(int pickup);
extern void do_cmd_run(void);
extern void do_cmd_rest(void);
extern bool do_cmd_fire(bool direct);
extern bool do_cmd_fire_aux(int item, object_type *j_ptr, bool direct);
extern void do_cmd_throw(void);
extern bool do_cmd_throw_aux(int mult, u16b mode, int chosen_item);

/* cmd3.c */
extern void do_cmd_inven(void);
extern void do_cmd_equip(void);
extern void do_cmd_wield(void);
extern void do_cmd_takeoff(void);
extern void do_cmd_drop(void);
extern void do_cmd_destroy(void);
extern void do_cmd_observe(void);
extern void do_cmd_uninscribe(void);
extern void do_cmd_inscribe(void);
extern void do_cmd_refill(void);
extern void do_cmd_target(void);
extern void do_cmd_look(void);
extern void do_cmd_locate(void);
extern void do_cmd_query_symbol(void);
extern void research_mon_aux(int r_idx);
extern bool research_mon(void);
extern void kamaenaoshi(int item);
extern bool ang_sort_comp_hook(vptr u, vptr v, int a, int b);
extern void ang_sort_swap_hook(vptr u, vptr v, int a, int b);

/* cmd4.c */
extern errr do_cmd_write_nikki(int type, int num, cptr note);
extern void do_cmd_nikki(void);
extern void do_cmd_redraw(void);
extern void do_cmd_change_name(void);
extern void do_cmd_message_one(void);
extern void do_cmd_messages(int num_now);
extern void do_cmd_options_aux(int page, cptr info);
extern void do_cmd_options(void);
extern void do_cmd_pref(void);
extern void do_cmd_pickpref(void);
extern void do_cmd_macros(void);
extern void do_cmd_visuals(void);
extern void do_cmd_colors(void);
extern void do_cmd_note(void);
extern void do_cmd_version(void);
extern void do_cmd_feeling(void);
extern void do_cmd_load_screen(void);
extern void do_cmd_save_screen_html_aux(char *filename, int message);
extern void do_cmd_save_screen(void);
extern void do_cmd_knowledge(void);
extern void plural_aux(char * Name);
extern void do_cmd_checkquest(void);
extern void do_cmd_time(void);
extern void do_cmd_give_money(void);
extern void do_cmd_skill_level(void);

/* cmd5.c */
extern cptr spell_categoly_name(int tval);
extern void do_cmd_browse(void);
extern void stop_singing(void);
extern void do_cmd_cast(void);
extern void do_cmd_pray(void);
extern bool rakuba(int dam, bool force);
extern bool do_riding(bool force);
extern int calculate_upkeep(void);
extern void do_cmd_pet_dismiss(void);
extern void do_cmd_pet(void);

/* cmd6.c */
extern void do_cmd_eat_food(void);
extern void do_cmd_quaff_potion(void);
extern void do_cmd_read_scroll(void);
extern void do_cmd_aim_wand(void);
extern void do_cmd_use_staff(void);
extern void do_cmd_zap_rod(void);
extern void do_cmd_activate(void);
extern void do_cmd_rerate(void);
extern void ring_of_power(int dir);
extern void do_cmd_use(void);

/* dungeon.c */
extern void sense_floor_object(int o_idx);
extern void leave_quest_check(void);
extern void play_game(bool new_game);
extern void leave_level(int level);
extern void enter_level(int level);
extern void load_all_pref_files(void);
extern void init_realm_table(void);
extern s32b turn_real(s32b hoge);


/* files.c */
extern void safe_setuid_drop(void);
extern void safe_setuid_grab(void);
extern s16b tokenize(char *buf, s16b num, char **tokens, int mode);
extern void display_player(int mode);
extern errr make_character_dump(FILE *fff);
extern errr file_character(cptr name);
extern errr process_pref_file_command(char *buf);
extern errr process_pref_file(cptr name);
extern errr process_pickpref_file(cptr name);
extern void print_equippy(void);
extern errr check_time_init(void);
extern errr check_load_init(void);
extern errr check_time(void);
extern errr check_load(void);
extern bool show_file(bool show_version, cptr name, cptr what, int line, int mode);
extern void do_cmd_help(void);
extern void process_player_name(bool sf);
extern void get_name(void);
extern void do_cmd_suicide(void);
extern void do_cmd_save_game(int is_autosave);
extern void do_cmd_save_and_exit(void);
extern long total_points(void);
extern void close_game(void);
extern void exit_game_panic(void);
extern void signals_ignore_tstp(void);
extern void signals_handle_tstp(void);
extern void signals_init(void);
extern errr get_rnd_line(cptr file_name, int entry, char *output);

#ifdef JP
extern errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count);
#endif

/* flavor.c */
extern void get_table_name(char *out_string);
extern void flavor_init(void);
extern char *object_desc_kosuu(char *t, object_type *o_ptr);
extern void construct_bonus_list(object_type *o_ptr, bonus_list_type bonus_list[A_MAX + OB_MAX + 1],
                                 cptr bonus_desc[A_MAX + OB_MAX], cptr to_all_stats_desc);
extern void object_desc(char *buf, object_type *o_ptr, int pref, int mode);

/* floors.c */
extern void init_saved_floors(void);
extern void clear_saved_floor_files(void);
extern saved_floor_type *get_sf_ptr(s16b floor_id);
extern s16b get_new_floor_id(void);
extern void prepare_change_floor_mode(u32b mode);
extern void leave_floor(void);
extern void change_floor(void);
extern void stair_creation(void);

/* generate.c */
extern void place_closed_door(int y, int x);
extern void place_quest_monsters(void);
extern void clear_cave(void);
extern void generate_cave(void);

/* init1.c */
extern byte color_char_to_attr(char c);
extern errr process_dungeon_file(cptr name, int ymin, int xmin, int ymax, int xmax);

/* init2.c */
extern void init_file_paths(char *path);
extern cptr err_str[PARSE_ERROR_MAX];
extern errr init_v_info(void);
extern errr init_buildings(void);
extern void init_angband(void);

/* load.c */
extern errr rd_savefile_new(void);
extern bool load_floor(saved_floor_type *sf_ptr, u32b mode);

/* melee1.c */
/* melee2.c */
extern bool mspell_check_path(monster_type *m_ptr, int *yp, int *xp, int dist,
                              int *do_disi_type, u32b *f4p, u32b *f5p, u32b *f6p, u32b *fap);
extern bool make_attack_normal(int m_idx);
extern bool make_attack_spell(int m_idx);
extern void process_monsters(void);
extern u32b get_curse(int power, object_type *o_ptr);
extern void curse_equipment(int chance, int heavy_chance);
extern void mon_take_hit_mon(bool force_damage, int m_idx, int dam, bool *fear, cptr note, int who);
extern bool process_stop_the_time(int num, bool vs_player);
extern void monster_gain_exp(int m_idx, int s_idx);

/* monster1.c */
extern void roff_top(int r_idx);
extern void screen_roff(int r_idx, int mode);
extern void display_roff(int r_idx);
extern void output_monster_spoiler(int r_idx, void (*roff_func)(byte attr, cptr str));
extern void create_name(int type, char *name);
/* monster1.c (was in monster3.c ??) */
extern bool monster_quest(int r_idx);
extern bool monster_dungeon(int r_idx);
extern bool monster_ocean(int r_idx);
extern bool monster_shore(int r_idx);
extern bool monster_town(int r_idx);
extern bool monster_wood(int r_idx);
extern bool monster_volcano(int r_idx);
extern bool monster_mountain(int r_idx);
extern bool monster_grass(int r_idx);
extern bool monster_deep_water(int r_idx);
extern bool monster_shallow_water(int r_idx);
extern bool monster_lava(int r_idx);
extern bool monster_tundra(int r_idx);
extern bool monster_deep_sea(int r_idx);
extern monster_hook_type get_monster_hook(void);
extern monster_hook_type get_monster_hook2(int y, int x);
extern void set_friendly(monster_type *m_ptr);
extern void set_pet(monster_type *m_ptr);
extern void set_hostile(monster_type *m_ptr);
extern void anger_monster(monster_type *m_ptr);
extern bool monster_can_cross_terrain(byte feat, monster_race *r_ptr);
extern bool monster_can_enter(int y, int x, monster_race *r_ptr);
extern bool are_enemies(monster_type *m_ptr1, monster_type *m_ptr2);
extern bool monster_has_hostile_alignment(monster_type *m_ptr, monster_race *r_ptr);
extern bool monster_living(monster_race *r_ptr);


/* monster2.c */
extern cptr horror_desc[MAX_SAN_HORROR];
extern cptr funny_desc[MAX_SAN_FUNNY];
extern cptr funny_comments[MAX_SAN_COMMENT];
extern void set_target(monster_type *m_ptr, int y, int x);
extern void reset_target(monster_type *m_ptr);
extern monster_race *real_r_ptr(monster_type *m_ptr);
extern void delete_monster_idx(int i);
extern void delete_monster(int y, int x);
extern void compact_monsters(int size);
extern void wipe_m_list(void);
extern s16b m_pop(void);
extern errr get_mon_num_prep(monster_hook_type monster_hook, monster_hook_type monster_hook2);
extern s16b get_mon_num(int level);
extern void monster_desc(char *desc, monster_type *m_ptr, int mode);
extern void lore_do_probe(int m_idx);
extern void lore_treasure(int m_idx, int num_item, int num_gold);
extern void sanity_blast(monster_type *m_ptr, bool necro);
extern void update_mon(int m_idx, bool full);
extern void update_monsters(bool full);
extern bool place_monster_aux(int who, int y, int x, int r_idx, u32b mode);
extern bool place_monster(int y, int x, u32b mode);
extern bool alloc_horde(int y, int x);
extern bool alloc_guardian(u32b mode);
extern bool alloc_monster(int dis, u32b mode);
extern bool summon_specific(int who, int y1, int x1, int lev, int type, u32b mode);
extern bool summon_named_creature (int who, int oy, int ox, int r_idx, u32b mode);
extern bool multiply_monster(int m_idx, bool clone, u32b mode);
extern void update_smart_learn(int m_idx, int what);
extern bool place_monster_one(int who, int y, int x, int r_idx, u32b mode);
extern bool player_place(int y, int x);
extern void monster_drop_carried_objects(monster_type *m_ptr);
extern bool is_anti_magic_grid(int who, int y, int x);
extern int is_fear_field_grid(int who, int y, int x);
extern bool create_runeweapon(int specific);
extern void verify_runeweapon(void);
extern void remove_runeweapon(int specific);
extern bool monster_is_runeweapon(int r_idx);

/* object1.c */
extern s16b m_bonus(int max, int level);

extern void reset_visuals(void);
extern void object_flags(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE]);
extern void object_flags_known(object_type *o_ptr, u32b flgs[TR_FLAG_SIZE]);
extern int get_object_ethnicity(object_type *o_ptr);
extern byte get_weapon_type(object_kind *k_ptr);
extern void object_desc_store(char *buf, object_type *o_ptr, int pref, int mode);
extern cptr item_activation(object_type *o_ptr);
extern bool screen_object(object_type *o_ptr, FILE *fff, bool real);
extern int real_inventory_slot(int slot);
extern char index_to_label(int i, bool use_offset);
extern s16b label_to_inven(int c);
extern s16b label_to_equip(int c);
extern s16b wield_slot(object_type *o_ptr);
extern cptr mention_use(int i);
extern cptr describe_use(int i);
extern bool check_book_realm(const byte book_tval, const byte book_sval);
extern bool item_tester_okay(object_type *o_ptr);
extern void display_inven(void);
extern void display_equip(void);
extern int show_inven(int target_item);
extern int show_equip(int target_item);
extern void toggle_inven_equip(void);
extern int get_tag(int *cp, char tag);
extern bool can_get_item(void);
extern bool get_item(int *cp, cptr pmt, cptr str, int mode);

/* object2.c */
extern void excise_object_idx(int o_idx);
extern void delete_object_idx(int o_idx);
extern void delete_object(int y, int x);
extern void compact_objects(int size);
extern void wipe_o_list(void);
extern s16b o_pop(void);
extern errr get_obj_num_prep(void);
extern s16b get_obj_num(int level);
extern void object_known(object_type *o_ptr);
extern void object_aware(object_type *o_ptr);
extern void object_tried(object_type *o_ptr);
extern s32b object_value(object_type *o_ptr);
extern s32b object_value_real(object_type *o_ptr);
extern bool can_player_destroy_object(object_type *o_ptr);
extern void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt);
extern void reduce_charges(object_type *o_ptr, int amt);
extern bool object_similar(object_type *o_ptr, object_type *j_ptr);
extern void object_absorb(object_type *o_ptr, object_type *j_ptr);
extern s16b lookup_kind(int tval, int sval);
extern void object_wipe(object_type *o_ptr);
extern void object_prep(object_type *o_ptr, int k_idx);
extern void object_copy(object_type *o_ptr, object_type *j_ptr);
extern bool shooting_star_generation_okay(object_type *o_ptr);
extern bool baldar_generation_okay(object_type *o_ptr);
extern void dragon_resist(object_type *o_ptr);
extern void apply_magic(object_type *o_ptr, int lev, u32b am_flags);
extern bool make_object(object_type *j_ptr, u32b am_flags);
extern void place_object(int y, int x, u32b am_flags);
extern bool make_gold(object_type *j_ptr);
extern void place_gold(int y, int x);
extern s16b drop_near(object_type *o_ptr, int chance, int y, int x);
extern void acquirement(int y1, int x1, int num, bool great, bool known);
extern byte choose_random_trap(void);
extern void disclose_grid(int y, int x);
extern void place_trap(int y, int x);
extern void inven_item_charges(int item);
extern void inven_item_describe(int item);
extern void inven_item_increase(int item, int num);
extern void inven_item_optimize(int item);
extern void floor_item_charges(int item);
extern void floor_item_describe(int item);
extern void floor_item_increase(int item, int num);
extern void floor_item_optimize(int item);
extern bool inven_carry_okay(object_type *o_ptr);
extern s16b inven_carry(object_type *o_ptr);
extern s16b inven_takeoff(int item, int amt);
extern void inven_drop(int item, int amt);
extern void combine_pack(void);
extern void reorder_pack(void);
extern void display_koff(int k_idx);
extern object_type *choose_warning_item(void);
extern bool process_warning(int xx, int yy);
extern bool item_tester_hook_metal(object_type *o_ptr);
extern void do_cmd_gunner(bool only_browse);
extern void process_runeweapon_list(void);

/* racial.c */
extern void do_cmd_racial_power(void);

/* save.c */
extern bool save_player(void);
extern bool load_player(void);
extern void remove_loc(void);
extern bool save_floor(saved_floor_type *sf_ptr, u32b mode);

/* spells1.c */
extern bool in_disintegration_range(int y1, int x1, int y2, int x2);
extern void breath_shape(u16b *path_g, int dist, int *pgrids, byte *gx, byte *gy, u16b *gm, int *pgm_rad, int rad, int y1, int x1, int y2, int x2, bool disint_ball, bool real_breath);
extern int take_hit(u32b damage_type, int damage, cptr kb_str);
extern u16b bolt_pict(int y, int x, int ny, int nx, int typ);
extern sint project_path(u16b *gp, int range, int y1, int x1, int y2, int x2, u32b flg);
extern int dist_to_line(int y, int x, int y1, int x1, int y2, int x2);
extern bool project(int who, int rad, int y, int x, int dam, int typ, u32b flg, int mod_elem_mode);
extern int project_length;

/* spells2.c */
extern void message_pain(int m_idx, int dam);
extern void self_knowledge(void);
extern bool detect_traps(int range, bool known);
extern bool detect_doors(int range);
extern bool detect_stairs(int range);
extern bool detect_treasure(int range);
extern bool detect_objects_gold(int range);
extern bool detect_objects_normal(int range);
extern bool detect_objects_magic(int range);
extern bool detect_monsters_normal(int range);
extern bool detect_monsters_invis(int range);
extern bool detect_monsters_evil(int range);
extern bool detect_monsters_string(int range, cptr);
extern bool detect_monsters_nonliving(int range);
extern bool detect_monsters_living(int range);
extern bool detect_monsters_thermal(int range);
extern bool detect_monsters_mind(int range);
extern bool detect_all(int range);
extern bool speed_monsters(void);
extern bool slow_monsters(void);
extern bool sleep_monsters(void);
extern void aggravate_monsters(int who);
extern bool symbol_genocide(int power, int player_cast);
extern bool mass_genocide(int power, int player_cast);
extern bool mass_genocide_undead(int power, int player_cast);
extern bool probing(void);
extern bool banish_evil(int dist);
extern bool dispel_evil(int dam);
extern bool dispel_good(int dam);
extern bool dispel_undead(int dam);
extern bool dispel_monsters(int dam);
extern bool crusade(void);
extern bool destroy_area(int y1, int x1, int r);
extern bool mon_fall_into_air(int m_idx);
extern bool earthquake(int cy, int cx, int r);
extern void lite_room(int y1, int x1);
extern void unlite_room(int y1, int x1);
extern bool lite_area(int dam, int rad);
extern bool unlite_area(int dam, int rad);
extern bool fire_ball(int typ, int dir, int dam, int rad, bool no_reduce);
extern bool fire_rocket(int typ, int dir, int dam, int rad, bool no_reduce);
extern bool fire_ball_hide(int typ, int dir, int dam, int rad, bool no_reduce);
extern bool fire_meteor(int who, int typ, int x, int y, int dam, int rad, bool no_reduce);
extern bool fire_bolt(int typ, int dir, int dam);
extern bool fire_blast(int typ, int dir, int dd, int ds, int num, int dev);
extern void call_chaos(void);
extern bool fire_beam(int typ, int dir, int dam);
extern bool fire_bolt_or_beam(int prob, int typ, int dir, int dam);
extern bool lite_line(int dir);
extern bool drain_life(int dir, int dam);
extern bool death_ray(int dir, int plev);
extern bool wall_to_mud(int dir);
extern bool destroy_door(int dir);
extern bool disarm_trap(int dir);
extern bool heal_monster(int dir, int dam);
extern bool speed_monster(int dir);
extern bool slow_monster(int dir);
extern bool sleep_monster(int dir);
extern bool stasis_monster(int dir);    /* Like sleep, affects undead as well */
extern bool stasis_evil(int dir);    /* Like sleep, affects undead as well */
extern bool confuse_monster(int dir, int plev);
extern bool stun_monster(int dir, int plev);
extern bool fear_monster(int dir, int plev);
extern bool poly_monster(int dir);
extern bool teleport_monster(int dir);
extern bool trap_creation(int y, int x);
extern bool tree_creation(void);
extern bool glyph_creation(void);
extern bool destroy_doors_touch(void);
extern bool animate_dead(int who, int y, int x);
extern bool sleep_monsters_touch(void);
extern bool activate_ty_curse(bool stop_ty, int *count);
extern int activate_hi_summon(int y, int x, bool can_pet);
extern int summon_cyber(int who, int y, int x);
extern void wall_breaker(void);
extern bool confuse_monsters(int dam);
extern bool charm_monsters(int dam);
extern bool charm_animals(int dam);
extern bool charm_males(int dam);
extern bool stun_monsters(int dam);
extern bool stasis_monsters(int dam);
extern bool banish_monsters(int dist);
extern bool turn_monsters(int dam);
extern bool charm_monster(int dir, int plev);
extern bool control_one_undead(int dir, int plev);
extern bool charm_animal(int dir, int plev);
extern bool charm_beast(int dir, int plev);
extern bool mindblast_monsters(int dam);
extern s32b flag_cost(object_type *o_ptr);
extern void report_magics(void);
extern bool teleport_swap(int dir);
extern bool item_tester_hook_recharge(object_type *o_ptr);
extern bool project_hook(int typ, int dir, int dam, u32b flg);
extern bool project_hack(int typ, int dam);
extern bool eat_magic(int power);
extern void discharge_minion(void);
extern bool fire_elem_ball(s16b elem, int dir, int rad, s16b amount);
extern void range_restricted_target(int dir, int range, int *y, int *x, bool stop_before_wall);
extern bool summon_god(int typ, int dam);
extern void cast_call_the_elemental(int typ, int dir, int dam_base, int dam_dice, int dam_side, int attacks_base, int use_stat);
extern void get_mon_project_point(int who, int *y, int *x);
extern bool mon_cast_call_the_elemental(int who, int y, int x, int dam_base, int dam_dice, int dam_side, int attacks_base, int typ);
extern bool project_hack_living(int typ, int dam);
extern bool project_hack_undead(int typ, int dam);
extern bool stone_gaze(int who);
extern bool inc_area_elem(int who, s16b elem, int amount, int rad, bool dec_opposite);
extern bool mermaid_water_flow(void);
extern void song_of_silence(int dam);
extern void song_of_temptation(void);
extern void knock_back(int who, int y, int x, int base_dam);

/* spells3.c */
extern bool teleport_away(int m_idx, int dis);
extern void teleport_to(int m_idx, int ty, int tx, int dis, int power, bool avoid_fall);
extern void teleport_player(int dis);
extern void teleport_player_to(int ny, int nx, bool no_tele, bool avoid_fall);
extern void teleport_monster_to(int m_idx, int ty, int tx);
extern void teleport_level(int m_idx);
extern bool recall_player(int turns);
extern bool word_of_recall(void);
extern bool reset_recall(void);
extern bool apply_disenchant(void);
extern void mutate_player(void);
extern void brand_weapon(int brand_type);
extern void call_the_(void);
extern void fetch(int dir, int wgt, bool require_los);
extern void alter_reality(void);
extern bool alter_with_flood(void);
extern bool alter_to_fate(void);
extern bool can_create_glyph_bold(int y, int x);
extern bool warding_glyph(void);
extern bool explosive_rune(void);
extern void identify_pack(bool mental);
extern bool remove_curse(void);
extern bool remove_all_curse(void);
extern bool alchemy(void);
extern bool item_tester_hook_weapon(object_type *o_ptr);
extern bool item_tester_hook_armour(object_type *o_ptr);
extern bool item_tester_hook_corpse(object_type *o_ptr);
extern bool item_tester_hook_weapon_armour(object_type *o_ptr);
extern bool enchant(object_type *o_ptr, int n, int eflag);
extern bool enchant_spell(int num_hit, int num_dam, int num_ac);
extern bool artifact_scroll(void);
extern bool ident_spell(bool only_equip);
extern bool mundane_spell(bool only_equip);
extern bool identify_item(object_type *o_ptr);
extern bool identify_fully(bool only_equip);
extern bool recharge(int num);
extern bool bless_weapon(void);
extern bool unholy_weapon(void);
extern bool potion_smash_effect(int who, int y, int x, int k_idx);
extern void display_spell_list(void);
extern int calc_use_mana(int spell, int realm);
extern s16b spell_chance(int spell,int realm);
extern bool spell_okay(int spell, int realm);
extern void print_spells(int target_spell, byte *spells, int num, int y, int x, int realm);
extern bool hates_acid(object_type *o_ptr);
extern bool hates_elec(object_type *o_ptr);
extern bool hates_fire(object_type *o_ptr);
extern bool hates_cold(object_type *o_ptr);
extern int set_acid_destroy(object_type *o_ptr);
extern int set_elec_destroy(object_type *o_ptr);
extern int set_fire_destroy(object_type *o_ptr);
extern int set_cold_destroy(object_type *o_ptr);
extern int inven_damage(inven_func typ, int perc);
extern int acid_dam(int dam, cptr kb_str);
extern int elec_dam(int dam, cptr kb_str);
extern int fire_dam(int dam, cptr kb_str);
extern int cold_dam(int dam, cptr kb_str);
extern bool curse_armor(void);
extern bool curse_weapon(bool force, int slot);
extern bool polymorph_monster(int y, int x, bool change_lnc);
extern bool dimension_door(void);
extern bool aqua_diving(void);
extern bool jump_wall(void);
extern bool summon_kin_player(int level, int y, int x, u32b mode);
extern void reincarnation(void);
extern void dispel_player(bool kill_sing);
extern void snap_dragon(void);
extern bool wish_object(cptr err_msg);
extern void break_decoy(void);
extern bool set_decoy(void);
extern bool ego_creation_scroll(void);
extern bool get_energy_from_corpse(void);

/* store.c */
extern void do_cmd_store(void);
extern void store_shuffle(int which);
extern void store_maint(int town_num, int store_num);
extern void store_init(int town_num, int store_num);
extern void move_to_black_market(object_type * o_ptr);
extern void move_inventory_to_home(void);

/* bldg.c */
extern bool get_nightmare(int r_idx);
extern void have_nightmare(int r_idx);
extern void do_cmd_bldg(void);
extern void do_cmd_quest(void);
extern void quest_discovery(int q_idx);
extern int quest_number(int level);
extern int random_quest_number(int level);
extern bool tele_town(bool magic);

/* util.c */
extern errr path_parse(char *buf, int max, cptr file);
extern errr path_build(char *buf, int max, cptr path, cptr file);
extern FILE *my_fopen(cptr file, cptr mode);
extern FILE *my_fopen_temp(char *buf, int max);
extern errr my_fgets(FILE *fff, char *buf, huge n);
extern errr my_fputs(FILE *fff, cptr buf, huge n);
extern errr my_fclose(FILE *fff);
extern errr fd_kill(cptr file);
extern errr fd_move(cptr file, cptr what);
extern errr fd_copy(cptr file, cptr what);
extern int fd_make(cptr file, int mode);
extern int fd_open(cptr file, int flags);
extern errr fd_lock(int fd, int what);
extern errr fd_seek(int fd, huge n);
extern errr fd_chop(int fd, huge n);
extern errr fd_read(int fd, char *buf, huge n);
extern errr fd_write(int fd, cptr buf, huge n);
extern errr fd_close(int fd);
extern void flush(void);
extern void bell(void);
extern void sound(int num);
extern void move_cursor(int row, int col);
extern void text_to_ascii(char *buf, cptr str);
extern void ascii_to_text(char *buf, cptr str);
extern errr macro_add(cptr pat, cptr act);
extern sint macro_find_exact(cptr pat);
extern char inkey(void);
extern cptr quark_str(s16b num);
extern s16b quark_add(cptr str);
extern s16b message_num(void);
extern cptr message_str(int age);
extern void message_add(cptr msg);
extern void msg_print(cptr msg);
#ifndef SWIG
extern void msg_format(cptr fmt, ...);
#endif /* SWIG */
extern void screen_save(void);
extern void screen_load(void);
extern void c_put_str(byte attr, cptr str, int row, int col);
extern void put_str(cptr str, int row, int col);
extern void c_prt(byte attr, cptr str, int row, int col);
extern void prt(cptr str, int row, int col);
extern void c_roff(byte attr, cptr str);
extern void roff(cptr str);
extern void clear_from(int row);
extern bool askfor_aux(char *buf, int len);
extern bool get_string(cptr prompt, char *buf, int len);
extern bool get_check(cptr prompt);
extern bool get_check_strict(cptr prompt, int mode);
extern bool get_com(cptr prompt, char *command, bool z_escape);
extern s16b get_quantity(cptr prompt, int max);
extern void pause_line(int row);
extern void request_command(int shopping);
extern bool is_a_vowel(int ch);
extern int get_keymap_dir(char ch);
extern void roff_to_buf(cptr str, int wlen, char *tbuf, size_t bufsize);

#ifdef SORT_R_INFO
extern void tag_sort(tag_type elements[], int number);
#endif /* SORT_R_INFO */

#ifdef SUPPORT_GAMMA
extern byte gamma_table[256];
extern void build_gamma_table(int gamma);
#endif /* SUPPORT_GAMMA */

extern size_t my_strcpy(char *buf, const char *src, size_t bufsize);
extern size_t my_strcat(char *buf, const char *src, size_t bufsize);


/* xtra1.c */
extern bool is_daytime(void);
extern void extract_day_hour_min(int *day, int *hour, int *min);
extern void prt_time(void);
extern cptr map_name(void);
extern void cnv_stat(int val, char *out_val);
extern s16b modify_stat_value(int value, int amount);
extern void notice_stuff(void);
extern void update_stuff(void);
extern void redraw_stuff(void);
extern void window_stuff(void);
extern void handle_stuff(void);
extern byte empty_hands(void);
extern int inventory_to_hand(int slot);
extern int hand_to_inventory(int hand);
extern bool heavy_armor(void);
extern void calc_bonuses(void);
extern int number_of_quests(void);

/* effects.c */
extern void set_action(int typ);
extern void reset_tim_flags(void);
extern bool set_blind(int v);
extern bool set_confused(int v);
extern bool set_poisoned(int v);
extern bool set_afraid(int v);
extern bool set_paralyzed(int v);
extern bool set_image(int v);
extern bool set_stoning(int v);
extern bool set_opposite_pelem(int v);
extern bool set_fast(int v, bool do_dec);
extern bool set_slow(int v, bool do_dec);
extern bool set_shield(int v, bool do_dec);
extern bool set_magicdef(int v, bool do_dec);
extern bool set_blessed(int v, bool do_dec);
extern bool set_hero(int v, bool do_dec);
extern bool set_shero(int v, bool do_dec);
extern bool set_protevil(int v, bool do_dec);
extern bool set_invuln(int v, bool do_dec);
extern bool set_tim_invis(int v, bool do_dec);
extern bool set_tim_infra(int v, bool do_dec);
extern bool set_chargespell(int v, bool do_dec);
extern void set_aquatic_in_water(void);
extern bool set_tim_sh_fire(int v, bool do_dec);
extern bool set_tim_sh_elec(int v, bool do_dec);
extern bool set_tim_sh_cold(int v, bool do_dec);
extern bool set_tim_sh_holy(int v, bool do_dec);
extern bool set_tim_eyeeye(int v, bool do_dec);
extern bool set_tim_inc_blow(int v, bool do_dec);
extern bool set_tim_dec_blow(int v, bool do_dec);
extern bool set_zoshonel_protect(int v, bool do_dec);
extern bool set_tim_octopus_immunity(int v, bool do_dec);
extern bool set_multishadow(int v, bool do_dec);
extern bool set_dustrobe(int v, bool do_dec);
extern bool set_magical_weapon(u32b attack_type, int v, int item, bool item_disappear);
extern bool set_evil_weapon(int v, bool do_dec, int item, bool item_disappear);
extern bool set_the_immunity(u32b immune_type, int v);
extern bool set_oppose_acid(int v, bool do_dec);
extern bool set_oppose_elec(int v, bool do_dec);
extern bool set_oppose_fire(int v, bool do_dec);
extern bool set_oppose_cold(int v, bool do_dec);
extern bool set_oppose_pois(int v, bool do_dec);
extern bool set_stun(int v);
extern bool set_cut(int v);
extern bool set_food(int v);
extern bool inc_stat(int stat);
extern bool dec_stat(int stat, int amount, int permanent);
extern bool res_stat(int stat);
extern bool hp_player(int num);
extern bool do_dec_stat(int stat);
extern bool do_res_stat(int stat);
extern bool do_inc_stat(int stat);
extern bool restore_level(void);
extern bool lose_all_info(void);
extern void gain_exp(s32b amount);
extern void lose_exp(s32b amount);
extern void do_poly_self(void);
extern bool set_tim_res_time(int v, bool do_dec);
extern bool choose_magical_weapon(void);
extern bool choose_the_immunity(int turn);
extern bool set_wraith_form(int v, bool do_dec);
extern bool set_tim_esp(int v, bool do_dec);
extern bool set_earth_spike(int v, bool do_dec);
extern bool set_wind_guard(int v, bool do_dec);
extern bool set_tim_resurrection(int v, bool do_dec);
extern bool set_tim_immune_magic(int v, bool do_dec);

/* xtra2.c */
extern void check_experience(void);
extern void check_quest_completion(monster_type *m_ptr);
extern cptr extract_note_dies(monster_race *r_ptr);
extern void monster_death(int m_idx, bool drop_item, bool is_stoned);
extern void get_exp_from_mon(int dam, monster_type *m_ptr);
extern bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note, bool is_stoned);
extern void get_screen_size(int *wid_p, int *hgt_p);
extern void panel_bounds_center(void);
extern void resize_map(void);
extern void redraw_window(void);
extern bool change_panel(int dy, int dx);
extern void verify_panel(void);
extern cptr look_mon_desc(int m_idx, u32b mode);
extern void ang_sort_aux(vptr u, vptr v, int p, int q);
extern void ang_sort(vptr u, vptr v, int n);
extern bool target_able(int m_idx);
extern bool target_okay(void);
extern bool target_set(int mode);
extern bool get_aim_dir(int *dp);
extern bool get_hack_dir(int *dp);
extern bool get_rep_dir(int *dp, bool under);
extern bool get_rep_dir2(int *dp);
extern bool activate_tarot_power(int effect);
extern void gain_level_reward(int chosen_reward);
extern bool tgt_pt(int *x, int *y, bool allow_player);
extern void do_poly_wounds(void);
extern int mon_damage_mod(monster_type *m_ptr, int dam, bool force_damage);
extern s16b gain_energy(void);
extern s16b bow_energy(object_type *o_ptr);
extern int bow_tmul(object_type *o_ptr);
extern int rocket_damage(object_type *o_ptr, int to_d);
extern cptr your_alignment_gne(void);
extern int get_your_alignment_gne(void);
extern cptr your_alignment_lnc(void);
extern int get_your_alignment_lnc(void);
extern void change_your_alignment_lnc(int amt);
extern void change_level99_quest(bool flip);
extern void process_chaos_frame(int ethnic);
extern void change_chaos_frame(int ethnic, int amt);
extern s16b get_cur_pelem(void);
extern s16b get_cur_melem(monster_type *m_ptr);
extern s16b get_elem_type(int typ);
extern bool are_opposite_elems(s16b atk_elem, s16b def_elem);
extern s16b get_opposite_elem(s16b elem);
extern s16b get_cur_weather_elem(int pick);
extern void set_weather(int inc_rain, int inc_wind, int inc_temp);
extern void change_grid_elem(cave_type *c_ptr, s16b elem, s16b amount);
extern int modify_dam_by_elem(int a_who, int d_who, int dam, int typ, int mode);
extern s16b choose_elem(void);
extern bool can_choose_class(byte new_class, byte mode);

/* mspells1.c */
extern bool clean_shot(int y1, int x1, int y2, int x2, bool friend, bool strict);
extern bool summon_possible(int who, int y1, int x1);
extern bool spell_is_inate(u16b spell);

/* mspells2.c */
extern bool monst_spell_monst(int m_idx, bool target_is_decoy);

/* artifact.c */
extern void one_sustain(object_type *o_ptr);
extern void one_high_resistance(object_type *o_ptr);
extern void one_lordly_high_resistance(object_type *o_ptr);
extern void one_ele_resistance(object_type *o_ptr);
extern void one_dragon_ele_resistance(object_type *o_ptr);
extern void one_resistance(object_type *o_ptr);
extern void one_ability(object_type *o_ptr);
extern bool create_artifact(object_type *o_ptr, bool a_scroll);
extern bool activate_random_artifact(object_type * o_ptr);
extern void random_artifact_resistance(object_type * o_ptr, artifact_type *a_ptr);
extern bool create_named_art(int a_idx, int y, int x);

/* scores.c */
extern void display_scores_aux(int from, int to, int note, high_score *score);
extern void display_scores(int from, int to);
extern void kingly(void);
extern errr top_twenty(void);
extern errr predict_score(void);
extern void race_legends(void);
extern void race_score(int race_num);
extern void show_highclass(void);
extern void survived_finish(void);

/* mind.c */
extern void do_cmd_mind(void);
extern void do_cmd_mind_browse(void);
extern void learn_mindcraft_power(bool is_unlearned);

/* mutation.c */
extern int count_bits(u32b x);
extern bool gain_random_mutation(int choose_mut, bool as_mutation);
extern bool lose_mutation(int choose_mut);
extern void dump_mutations(FILE *OutFile);
extern void do_cmd_knowledge_mutations(void);
extern int calc_mutant_regenerate_mod(void);
extern bool mutation_power_aux(u32b power);


/*
 * Hack -- conditional (or "bizarre") externs
 */

#ifdef SET_UID
/* util.c */
extern void user_name(char *buf, int id);
#endif

#if 0
#ifndef HAS_STRICMP
/* util.c */
extern int stricmp(cptr a, cptr b);
#endif
#endif

#ifndef HAVE_USLEEP
/* util.c */
extern int usleep(huge usecs);
#endif

#ifdef MACINTOSH
/* main-mac.c */
/* extern void main(void); */
#endif

#if defined(MAC_MPW)
/* Globals needed */
extern  u32b _ftype;
extern  u32b _fcreator;
#endif

#ifdef WINDOWS
/* main-win.c */
/* extern int FAR PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, ...); */
#endif


#ifdef ALLOW_REPEAT /* TNB */

/* util.c */
extern void repeat_push(int what);
extern bool repeat_pull(int *what);
extern void repeat_check(void);

#endif /* ALLOW_REPEAT -- TNB */

#ifdef ALLOW_EASY_OPEN /* TNB */

/* variable.c */
extern bool easy_open;

/* cmd2.c */
extern bool easy_open_door(int y, int x);

#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_DISARM /* TNB */

/* variable.c */
extern bool easy_disarm;

/* cmd2.c */
extern bool do_cmd_disarm_aux(int y, int x, int dir);

#endif /* ALLOW_EASY_DISARM -- TNB */


#ifdef ALLOW_EASY_FLOOR /* TNB */

/* object1.c */
extern int scan_floor(int *items, int y, int x, int mode);
extern int show_floor(int target_item, int y, int x, int *min_width);
extern bool get_item_floor(int *cp, cptr pmt, cptr str, int mode);
extern void py_pickup_floor(int pickup);

/* variable.c */
extern bool easy_floor;

#endif /* ALLOW_EASY_FLOOR -- TNB */

/* obj_kind.c */
extern errr k_info_alloc(void);
extern errr k_info_free(void);
extern object_kind *k_info_add(object_kind *k_info_entry);
extern int get_object_level(object_type *o_ptr);
extern s32b get_object_cost(object_type *o_ptr);
extern cptr get_object_name(object_type *o_ptr);
extern bool get_object_aware(object_type *o_ptr);
extern bool get_object_tried(object_type *o_ptr);
extern bool object_is_potion(object_type *o_ptr);
extern bool object_is_shoukinkubi(object_type *o_ptr);
extern bool object_is_runeweapon(object_type *o_ptr);
extern bool object_is_snapdragon_runeweapon(object_type *o_ptr);
extern bool object_is_astral_runeweapon(object_type *o_ptr);
extern errr init_object_alloc(void);
extern void k_info_reset(void);
extern bool buki_motteruka(int i);

/* wild.c */
extern void set_floor_and_wall(byte type);
extern void wilderness_gen(void);
extern void wilderness_gen_small(void);
extern errr init_wilderness(void);
extern void seed_wilderness(void);
extern errr parse_line_wilderness(char *buf, int xmin, int xmax, int *y, int *x);
extern bool change_wild_mode(void);

/* wizard1.c */
extern void spoil_random_artifact(cptr fname);

/* wizard2.c */
extern void strip_name(char *buf, int k_idx);

#ifdef JP
/* japanese.c */
extern void jverb1( const char *in , char *out);
extern void jverb2( const char *in , char *out);
extern void jverb3( const char *in , char *out);
extern void jverb( const char *in , char *out , int flag);
extern char* strstr_j(cptr str1, cptr str2);
extern void codeconv(char *str);
extern bool iskanji2(cptr s, int x);
#endif

/* report.c */
extern cptr make_screen_dump(void);
