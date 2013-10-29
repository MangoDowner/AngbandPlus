#ifndef INCLUDED_OBJECT_H
#define INCLUDED_OBJECT_H

/*** Variables ***/

/** Maximum number of scroll titles generated */
#define MAX_TITLES    132 /* TODO Check if this works.  Was 50, spells are 121 */

/** The titles of scrolls, ordered by sval. */
/* HACK attempting to reuse scroll titles for spell pages */ 
extern char scroll_adj[MAX_TITLES][16];


/*** Constants ***/

/**
 * Modes for object_desc().
 */
typedef enum
{
	ODESC_BASE   = 0x00,   /*!< Only describe the base name */
	ODESC_COMBAT = 0x01,   /*!< Also show combat bonuses */
	ODESC_EXTRA  = 0x02,   /*!< Show charges/inscriptions/pvals */

	ODESC_FULL   = ODESC_COMBAT | ODESC_EXTRA,
	                       /*!< Show entire description */

	ODESC_STORE  = 0x04,   /*!< This is an in-store description */
	ODESC_PLURAL = 0x08,   /*!< Always pluralise */
	ODESC_SINGULAR  = 0x10, /*!< Always singular */
	ODESC_SPOIL  = 0x20    /*!< Display regardless of player knowledge */
} odesc_detail_t;


/**
 * Pseudo-ID markers.
 */
typedef enum
{
	INSCRIP_NULL = 0,            /*!< No pseudo-ID status */
	INSCRIP_STRANGE = 1,         /*!< Item that has mixed combat bonuses */
	INSCRIP_AVERAGE = 2,         /*!< Item with no interesting features */
	INSCRIP_MAGICAL = 3,         /*!< Item with combat bonuses */
	INSCRIP_SPLENDID = 4,        /*!< Obviously good item */
	INSCRIP_EXCELLENT = 5,       /*!< Ego-item */
	INSCRIP_SPECIAL = 6,         /*!< Artifact */
	INSCRIP_UNKNOWN = 7,

	INSCRIP_MAX                  /*!< Maximum number of pseudo-ID markers */
} obj_pseudo_t;

/*** Functions ***/

/* identify.c */
extern s32b object_last_wield;

void object_known(object_type *o_ptr);
void object_aware(object_type *o_ptr);
void object_tried(object_type *o_ptr);
void object_notice_on_attack(void);
void object_notice_slays(u32b known_f1);
void object_notice_flag(int flagset, u32b flag);
bool object_notice_curses(object_type *o_ptr);
void object_notice_on_wield(object_type *o_ptr);
obj_pseudo_t object_pseudo(const object_type *o_ptr);
void sense_inventory(void);

/* obj-desc.c */
void object_kind_name(char *buf, size_t max, s16b k_idx, bool easy_know);
size_t object_desc(char *buf, size_t max, const object_type *o_ptr, bool prefix, odesc_detail_t mode);

/* obj-info.c */
extern const slay_t slay_table[];
size_t num_slays(void);
void object_info_header(const object_type *o_ptr);

bool object_info(const object_type *o_ptr, bool full);
bool object_info_chardump(const object_type *o_ptr);
bool object_info_spoil(const object_type *o_ptr);

/* obj-make.c */
void free_obj_alloc(void);
bool init_obj_alloc(void);
s16b get_obj_num(s16b level, bool good);
void apply_magic(object_type *o_ptr, s16b lev, bool okay, bool good, bool great);
bool make_object(object_type *j_ptr, s16b lev, bool good, bool great);
void make_gold(object_type *j_ptr, s16b lev, int coin_type);

/* obj-ui.c */
void display_inven(void);
void display_equip(void);
void show_inven(void);
void show_equip(void);
void show_floor(const s16b *floor_list, int floor_num, bool gold);
bool verify_item(cptr prompt, int item);
bool get_item(int *cp, cptr pmt, cptr str, int mode);

/* obj-util.c */
void flavor_init(void);
void reset_visuals(bool unused);
extern s16b s_pop(void); /* TODO Check works! */
void object_flags(const object_type *o_ptr, u32b flags[OBJ_FLAG_N]);
void object_flags_known(const object_type *o_ptr, u32b flags[OBJ_FLAG_N]);
char index_to_label(int i);
s16b label_to_inven(int c);
s16b label_to_equip(int c);
bool wearable_p(const object_type *o_ptr);
s16b wield_slot(const object_type *o_ptr);
const char *mention_use(int slot);
cptr describe_use(int i);
bool item_tester_okay(const object_type *o_ptr);
int scan_floor(s16b *items, int max_size, s16b y, s16b x, int mode);
void excise_object_idx(int o_idx);
void delete_object_idx(int o_idx);
void delete_object(s16b y, s16b x);
void compact_objects(int size);
void wipe_o_list(void);
s16b o_pop(void);
object_type *get_first_object(s16b y, s16b x);
object_type *get_next_object(const object_type *o_ptr);
bool is_blessed(const object_type *o_ptr);
s32b object_value(const object_type *o_ptr, int qty);
bool object_similar(const object_type *o_ptr, const object_type *j_ptr);
void object_absorb(object_type *o_ptr, const object_type *j_ptr);
void object_wipe(object_type *o_ptr);
void object_copy(object_type *o_ptr, const object_type *j_ptr);
void object_prep(object_type *o_ptr, s16b k_idx);
s16b floor_carry(s16b y, s16b x, object_type *j_ptr);
void drop_near(object_type *j_ptr, int chance, s16b y, s16b x);
void acquirement(s16b y1, s16b x1, s16b level, int num, bool great);
void inven_item_charges(int item);
void inven_item_describe(int item);
void inven_item_increase(int item, int num);
void inven_item_optimize(int item);
void floor_item_charges(int item);
void floor_item_describe(int item);
void floor_item_increase(int item, int num);
void floor_item_optimize(int item);
bool inven_carry_okay(const object_type *o_ptr);
bool inven_stack_okay(const object_type *o_ptr);
s16b inven_carry(object_type *o_ptr);
s16b inven_takeoff(int item, int amt);
void inven_drop(int item, int amt);
void combine_pack(void);
void reorder_pack(void);
void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt);
void reduce_charges(object_type *o_ptr, int amt);
unsigned check_for_inscrip(const object_type *o_ptr, const char *inscrip);
s16b lookup_kind(int tval, int sval);
bool lookup_reverse(s16b k_idx, int *tval, int *sval);
int lookup_name(int tval, const char *name);
int lookup_artifact_name(const char *name);
int lookup_sval(int tval, const char *name);
int tval_find_idx(const char *name);
const char *tval_find_name(int tval);

/* obj-power.c and randart.c */
s32b object_power(const object_type *o_ptr, int verbose, ang_file *log_file);
/*
 * Some constants used in randart generation and power calculation
 * - thresholds for limiting to_hit, to_dam and to_ac
 * - fudge factor for rescaling ammo cost
 * (a stack of this many equals a weapon of the same damage output)
 */
#define INHIBIT_POWER       20000
#define HIGH_TO_AC             21
#define VERYHIGH_TO_AC         31
#define INHIBIT_AC             41
#define HIGH_TO_HIT            16
#define VERYHIGH_TO_HIT        26
#define HIGH_TO_DAM            16
#define VERYHIGH_TO_DAM        26
#define AMMO_RESCALER          20 /* this value is also used for torches */

#define sign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

#define LOG_PRINT(string) \
        do { if (verbose) \
                file_putf(log_file, (string)); \
        } while (0);

#define LOG_PRINT1(string, value) \
        do { if (verbose) \
                file_putf(log_file, (string), (value)); \
        } while (0);

#define LOG_PRINT2(string, val1, val2) \
        do { if (verbose) \
                file_putf(log_file, (string), (val1), (val2)); \
        } while (0);
	
#endif /* !INCLUDED_OBJECT_H */
