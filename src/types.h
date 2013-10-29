/* File: types.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */


/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Note that certain data is saved in multiple places for efficient access,
 * and when modifying the data in one place it must also be modified in the
 * other places, to prevent the creation of inconsistant data.
 */



/**** Available Types ****/


/*
 * An array of 256 byte's
 */
typedef byte byte_256[256];


/*
 * An array of DUNGEON_WID byte's
 */
typedef byte byte_wid[DUNGEON_WID];

/*
 * An array of DUNGEON_WID s16b's
 */
typedef s16b s16b_wid[DUNGEON_WID];



/**** Available Structs ****/


typedef struct maxima maxima;
typedef struct feature_type feature_type;
typedef struct object_kind object_kind;
typedef struct artifact_type artifact_type;
typedef struct ego_item_type ego_item_type;
typedef struct monster_blow monster_blow;
typedef struct monster_race monster_race;
typedef struct monster_lore monster_lore;
typedef struct vault_type vault_type;
typedef struct object_type object_type;
typedef struct monster_type monster_type;
typedef struct alloc_entry alloc_entry;
typedef struct quest quest;
typedef struct owner_type owner_type;
typedef struct store_type store_type;
typedef struct magic_type magic_type;
typedef struct player_magic player_magic;
typedef struct spell_type spell_type;
typedef struct player_sex player_sex;
typedef struct player_race player_race;
typedef struct player_class player_class;
typedef struct hist_type hist_type;
typedef struct player_other player_other;
typedef struct player_type player_type;
typedef struct start_item start_item;
typedef struct autoinscription autoinscription;
typedef struct quiver_group_type quiver_group_type;


/**** Available structs ****/


/*
 * Information about maximal indices of certain arrays
 * Actually, these are not the maxima, but the maxima plus one
 */
struct maxima
{
	u32b fake_text_size;
	u32b fake_name_size;

	u16b f_max;		/* Max size for "f_info[]" */
	u16b k_max;		/* Max size for "k_info[]" */
	u16b a_max;		/* Max size for "a_info[]" */
	u16b e_max;		/* Max size for "e_info[]" */
	u16b r_max;		/* Max size for "r_info[]" */
	u16b v_max;		/* Max size for "v_info[]" */
	u16b p_max;		/* Max size for "p_info[]" */
	u16b h_max;		/* Max size for "h_info[]" */
	u16b b_max;		/* Max size per element of "b_info[]" */
	u16b c_max;		/* Max size for "c_info[]" */
	u16b flavor_max; /* Max size for "flavor_info[]" */
	u16b s_max;		/* Max size for "s_info[]" */

	u16b o_max;		/* Max size for "o_list[]" */
	u16b m_max;		/* Max size for "mon_list[]" */
};


/*
 * Information about terrain "features"
 */
struct feature_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte mimic;			/* Feature to mimic */

	byte extra;			/* Extra byte (unused) */

	s16b unused;		/* Extra bytes (unused) */

	byte d_attr;		/* Default feature attribute */
	char d_char;		/* Default feature character */

	byte x_attr;		/* Desired feature attribute */
	char x_char;		/* Desired feature character */
};


/*
 * Information about object kind, including player knowledge. (k_ptr)
 *
 * Only "aware" and "tried" are saved in the savefile
 */
struct object_kind
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Object type */
	byte sval;			/* Object sub type */

	s16b pval;			/* Object extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage dice/sides */
	byte sbdd, sbds;	/* 2nd blow damage dice/sides for double weapons */
	byte crc;			/* weapon crit chance (5 is +0) */
	s16b spdm;          /* weapon speed modifier (finesse) */

	s16b weight;		/* Weight */

	s32b cost;			/* Object "base cost" */

	u32b flags1;		/* Flags, set 1 */
	u32b flags2;		/* Flags, set 2 */
	u32b flags3;		/* Flags, set 3 */
	u32b flags4;		/* Flags, set 4 */

	byte locale[4];		/* Allocation level(s) */
	byte chance[4];		/* Allocation chance(s) */

	byte level;			/* Level */
	byte extra;			/* Difficulty level for magic items */


	byte d_attr;		/* Default object attribute */
	char d_char;		/* Default object character */


	byte x_attr;		/* Desired object attribute */
	char x_char;		/* Desired object character */

	byte charge_base;	/* Charge base */
	byte charge_dd, charge_ds;	/* Charge dice/sides */

	byte gen_mult_prob;     /* Probability of generating more than one */
	byte gen_dice;          /* Average number to generate - dice rolled */
	byte gen_side;          /* Average number to generate - dice sides */


	u16b flavor;		/* Special object flavor (or zero) */
	u16b note;          /* Autoinscription field (later) */

	bool aware;		/* The player is "aware" of the item's effects */
	bool tried;		/* The player has "tried" one of the items */

	byte squelch;		/* Squelch this item? */
	bool everseen;		/* Used to despoilify squelch menus */
};



/*
 * Information about "artifacts". (a_ptr)
 *
 * Note that the save-file only writes "cur_num" to the savefile.
 *
 * Note that "max_num" is always "1" (if that artifact "exists")
 */
struct artifact_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage when hits */
	byte sbdd, sbds;	/* double weapon damage */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */

	u32b flags1;		/* Artifact Flags, set 1 */
	u32b flags2;		/* Artifact Flags, set 2 */
	u32b flags3;		/* Artifact Flags, set 3 */
	u32b flags4;		/* Flags, set 4 */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */
	byte maxlvl;		/* Maximum object level to create this artifact */
	s16b artrat;        /* Artifact rating */

	/* code for an item which gives race-specifix ESP: */
	/* 0-none, 1=orcs, 2=trolls, 3=giants, 4=undead, 5=dragons, 6=demons */
	/* 7=fairies, 8=bugs, 9=grepse, 10=(most)animals,.. */
	/* 70 = short-ranged ESP for everything */
	byte esprace;       /* race-specific ESP (see o_ptr declaration below) */

	byte cur_num;		/* Number created (0 or 1) */
	byte max_num;		/* Unused (should be "1") */

	byte activation;	/* Activation to use */
	u16b time;			/* Activation time */
	u16b randtime;		/* Activation time dice */
};


/*
 * Information about "ego-items". (e_ptr)
 */
struct ego_item_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	s32b cost;			/* Ego-item "cost" */

	u32b flags1;		/* Ego-Item Flags, set 1 */
	u32b flags2;		/* Ego-Item Flags, set 2 */
	u32b flags3;		/* Ego-Item Flags, set 3 */
	u32b flags4;		/* Flags, set 4 */

	byte level;			/* Minimum level */
	s16b maxlev;		/* maximum level for ego (soft cap) */
	byte rarity;		/* Object rarity */
	byte rating;		/* Level rating boost */
	s16b weight;		/* Extra weight */

	byte tval[EGO_TVALS_MAX]; /* Legal tval */
	byte min_sval[EGO_TVALS_MAX];	/* Minimum legal sval */
	byte max_sval[EGO_TVALS_MAX];	/* Maximum legal sval */

	s16b max_to_h;		/* Maximum to-hit bonus */
	s16b max_to_d;		/* Maximum to-dam bonus */
	s16b max_to_a;		/* Maximum to-ac bonus */
#if noxdice
#else
	s16b xdicec;        /* % chance for automatic +1 damage dice */
#endif
	s16b max_pval;		/* Maximum pval */

	/* holds how many random attributes in its set */
	byte randsus;		/* random sustain 0, 1 or 2 */
	byte randres;		/* random resist 0, 1, 2, or 3 */
	byte randpow;       /* random power 0, 1 or 2 */
	byte randslay;      /* random slay 0, 1 or 2 */
	byte randbon;       /* random bonus 0, 1 or 2 */
	byte randplu;       /* random plus (stats) 0, 1 or 2 */
	byte randdrb;       /* random drawback 0, 1 or 2 (3 special) */
	byte randlowr;      /* random low resist 0, 1, or 2 */
	byte randbran;      /* random brand 0 or 1 */
	byte randact;       /* random activation (for future) */
	
	byte esprace;       /* race-specific ESP (see o_ptr declaration below) */
	byte unused1;        /* saving a spot for future use */

	bool everseen;		/* Do not spoil squelch menus */
};




/*
 * Monster blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */
struct monster_blow
{
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};



/*
 * Monster "race" information, including racial memories (r_ptr)
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of monsters
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique monsters.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 * Maybe "x_attr", "x_char", "cur_num", and "max_num" should
 * be moved out of this array since they are not read from
 * "monster.txt".
 */
struct monster_race
{
	u32b name;				/* Name (offset) */
	u32b text;				/* Text (offset) */

	/* used to be bytes */
	s16b hdice;				/* Creatures hit dice count */
	s16b hside;				/* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	byte speed;				/* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	s16b power;				/* Monster power (this is the unused spot) */
	s16b stealth;           /* (DAJ) ..which is replaced by this */
	
#ifdef ALLOW_TEMPLATES_PROCESS

	s16b highest_threat;	/* Monster highest threat */

#endif /* ALLOW_TEMPLATES_PROCESS */

	byte freq_innate;		/* Innate spell frequency */
	byte freq_spell;		/* Other spell frequency */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */
	u32b flags7;			/* Flags 7 (new flags (not spells)) */

	monster_blow blow[MONSTER_BLOW_MAX]; /* Up to four blows per round */

	byte level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */
	
	s16b spr;               /* spell range */
	byte mrsize;            /* monster race size */
	s16b maxpop;            /* max total population (per game) */
	s16b curpop;            /* how many killed so far this game */
	/* curpop is only ever incremented when a monster is killed */
	/* cur_num keeps track of living monsters on the current level */
	/* so when you check population always check curpop + cur_num */
	s16b champ;             /* ~1 in N chance of becoming a pseudo-unique 'champion' */
	
	/* monster resistances */
	s16b Rfire;
	s16b Rcold; 
	s16b Relec; 
	s16b Racid;
	s16b Rpois;
	s16b Rlite;
	s16b Rdark;
	s16b Rwater;
	s16b Rnexus;
	s16b Rmissile;
	s16b Rchaos;
	s16b Rdisen;
	s16b Rsilver;
	s16b Rtaming;
	s16b R4later;
	s16b summex; /* used for S_EXTRA spell */

	byte d_attr;			/* Default monster attribute */
	char d_char;			/* Default monster character */

	byte x_attr;			/* Desired monster attribute */
	char x_char;			/* Desired monster character */

	byte max_num;			/* Maximum population allowed per level */
	byte cur_num;			/* Monster population on current level */
};


/*
 * Monster "lore" information (l_ptr)
 *
 * Note that these fields are related to the "monster recall" and can
 * be scrapped if space becomes an issue, resulting in less "complete"
 * monster recall (no knowledge of spells, etc). XXX XXX XXX
 */
struct monster_lore
{
	s16b sights;			/* Count sightings of this monster */
	s16b deaths;			/* Count deaths from this monster */

	s16b pkills;			/* Count monsters killed in this life */
	s16b tkills;			/* Count monsters killed in all lives */

	byte wake;				/* Number of times woken up (?) */
	byte ignore;			/* Number of times ignored (?) */

	byte xtra1;				/* race has been probed (was unused) */
	byte xtra2;				/* Something (unused) */

	byte drop_gold;			/* Max number of gold dropped at once */
	byte drop_item;			/* Max number of item dropped at once */

	byte cast_innate;		/* Max number of innate spells seen */
	byte cast_spell;		/* Max number of other spells seen */

	byte blows[MONSTER_BLOW_MAX]; /* Number of times each blow type was seen */
	
	/* knowledge of monster resistances (because these aren't in RF? flags anymore) */
	/* these are just boolean whether the PC knows the level of the monster's resistance. */
	byte know_MRfire;
	byte know_MRcold;
	byte know_MRelec;
	byte know_MRacid;
	byte know_MRpois;
	byte know_MRlite;
	byte know_MRdark;
	byte know_MRwatr;
	byte know_MRnexu;
	byte know_MRmisl;
	/* X: line monster resistances */
	byte know_MRchao;
	byte know_MRdise;
	byte know_MRsilv;
	byte know_MRtame;
	byte know_R4latr;
	byte know_R4lat2;

	u32b flags1;			/* Observed racial flags */
	u32b flags2;			/* Observed racial flags */
	u32b flags3;			/* Observed racial flags */
	u32b flags4;			/* Observed racial flags */
	u32b flags5;			/* Observed racial flags */
	u32b flags6;			/* Observed racial flags */
	/* u32b flags7;			 Observed racial flags (new) */
};



/*
 * Information about "vault generation"
 */
struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */

	byte useMT;			/* can be used for empty vault */

	byte rare;			/* rarity of vault design (lower = more common) */
	byte vfloor;        /* possible alternate default floor terrain (usually water) */
	byte itheme;		/* ideal theme for vault design */
	byte ithemeb;		/* ideal theme for vault design */
	byte ithemec;		/* ideal theme for vault design */
	byte ithemed;		/* ideal theme for vault design */
};



/*
 * Object information, for a specific object. (object type, o_ptr)
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */
struct object_type
{
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s16b pval;			/* Item extra-parameter */
	s16b charges;       /* separate pval from charges so ego staves can have a pval */

	byte pseudo;		/* Pseudo-ID marker */
#ifdef instantpseudo
	byte hadinstant;    /* marks whether you've had a chance for instant pseudo */
#endif

	/* new origin-tracking stuff */
	s16b dlevel;		/* dungeon level it was created on */
	s16b drop_ridx;		/* race index of monster it was dropped by (or 0) */
	byte vcode;			/* 1= generated on a vault floor, 2= dropped by a monster in a vault */
		/* 3= generated in a rubble pile, 4= from a normal chest, */
		/* 5= from a normal chest which was generated in a vault */
		/* 6= from a silver-locked chest, 7= from a gold-locked chest */
		/* 8= generated in a rubble pile in a vault, 9= created by ?aquirement */
		/* 10= conjured by magic some other way, */
		/* 11= otherwise generated as a 'great' item (only used for cheat/testing options) */
		/* 12= bought from a store (besides the BM), 13= bought from the black market */

	byte number;		/* Number of items */

	s16b weight;		/* Item weight */

	byte name1;			/* Artifact type, if any */
	byte name2;			/* Ego-Item type, if any */

	char randego_name[40];	/* psuedo-randart names */
	/* holds indexes of random attribute in its set */
	byte randsus;		/* random sustain */
	byte randsus2;		/* 2nd random sustain */
	byte randres;		/* random resist */
	byte randres2;		/* 2nd random resist */
	byte randres3;		/* 3rd random resist */
	byte randpow;       /* random power */
	byte randpow2;      /* 2nd random power */
	/* the following are obvious if the object is known */
	/* (the only ones which are hidden are sustains, high resists, and powers) */
	byte randslay;      /* random slay */
	byte randslay2;     /* 2nd random slay */
	byte randslay3;     /* 2nd random slay */
	byte randbon;       /* random bonus */
	byte randbon2;      /* 2nd random bonus */
	byte randplu;       /* random plus (stats) */
	byte randplu2;      /* 2nd random plus (stats) */
	byte randdrb;       /* random drawback */
	byte randdrb2;      /* 2nd random drawback */
	byte randimm;       /* random immunity */
	byte randlowr;      /* random low resist */
	byte randlowr2;     /* 2nd random low resist */
	byte randbran;      /* random brand */
	byte randact;       /* random activation (for future) */
	
	/* code for an item which gives race-specifix ESP: */
	/* 0-none, 1=orcs, 2=trolls, 3=giants, 4=undead, 5=dragons, 6=demons */
	/* 7=fairies, 8=bugs, 9=grepse, 10=(most)animals,.. */
	/* 11=constructs (golems), 12=dwarves, 13=dark elves */
	/* 30 = 35% chance of ESP for (most) animals */
	/* 31 = 35% chance of ESP for orcs */
	/* 34 = 35% chance of ESP for undead (54 = 10%) */
	/* 35 = 35% chance of ESP for dragons, 36 = 35% chance of ESP for demons (56 = 10%) */
	/* 37 = 35% chance of ESP for fairies */
	/* 46 = 35% chance of ESP for HURT_SILV (werebeasts & such), 16 = 100% HURT_SILV */
	/* 40 = random esprace (43 = 35% chance of random esprace) */
	/* 41 = matches one of the random slays (42 = 35% chance of that) */
	/* 70 = short ranged ESP of everything (artifacts only) */
	/* 15 = special for Sting (orcs and spiders) */
	/* 71 = special for Ratagast (ESP for wizards) */
	/* 75 = special for Thranduil */
    byte esprace;       /* */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	byte dd, ds;		/* Damage dice/sides */
	byte sbdd, sbds;	/* 2nd blow damage dice/sides for double weapons */
	byte crc;			/* weapon crit chance (5 is +0) */
#if nextbreaksave
	s16b spdm;          /* weapon speed modifier */
#endif
	
	s16b blessed;       /* temporary blessing */
	s16b enhance;		/* alchemically enhanced magic items */
	s16b enhancenum;	/* alchemically enhanced items in a stack */

	s16b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags */

	byte marked;		/* Object is marked */
	byte hidden;		/* Object is buried in rubble */

	/* still need to code the temporary branding spell */
    s16b timedbrand;    /* temporary branding spell duration */
	byte thisbrand;     /* which brand for temporary brand */
	/* thisbrand:  1=frost, 2=fire, 3=elec, 4=acid, 5=poison */
	/* 6=acid coat(x2) */
	s16b extra1;        /* to prevent breaking savefiles later */
	s16b extra2;        /* to prevent breaking savefiles later */

	u16b note;			/* Inscription index */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */
};



/*
 * Monster information, for a specific monster. (m_ptr)
 *
 * Note: fy, fx constrain dungeon size to 256x256
 *
 * The "hold_o_idx" field points to the first object of a stack
 * of objects (if any) being carried by the monster (see above).
 */
struct monster_type
{
	s16b r_idx;			/* Monster race index */

	byte fy;			/* Y location on map */
	byte fx;			/* X location on map */

	s16b hp;			/* Current Hit points */
	s16b maxhp;			/* Max Hit points */

	s16b csleep;		/* Inactive counter */
	bool display_sleep;  /* is it displayed as asleep? */

	s16b roaming;       /* monster is awake but hasn't noticed the player */
	/* roaming: 0 = not roaming */
	/* roaming: 1 to 9 roaming alone */
	/* roaming: 11 to 19 roaming in a group */
	/* roaming: 30 temporary roaming to keep from repeatedly running into a wall */
	byte roam_to_y;     /* random destination for roaming monsters */
	byte roam_to_x;     /* random destination for roaming monsters */

	byte mspeed;		/* Monster "speed" */
	byte energy;		/* Monster "energy" */
	
	/* some monster races are sometimes but not always evil now */
    byte evil;
    s16b meet;          /* have you met this monster? */

	byte stunned;		/* Monster is stunned */
	byte confused;		/* Monster is confused */
	byte monfear;		/* Monster is afraid */
    bool bold;		    /* Monster has become bold */
    /* hack: tinvis is also used for the NONMONSTER statue for its description. */
    /* since normally NONMONSTERS can never use tinvis */
	s16b tinvis;        /* temporary invisibility */
	bool charmed;       /* monster is charmed */
	byte silence;       /* monster is silenced (stops summoning spells) */
	byte truce;			/* for paladin truce spell */
	s16b disguised;		/* monster disguised as another monster or object */
	/* (disguised is extracted for now, may need to add to savefiles later) */
	s16b hear_noise;    /* counts the noise level that the monster hears */
	byte pcloud;        /* 1 if the PC made the loudest noise, otherwise 0 */

	byte cdis;			/* Current distance from player */

	byte mflag;			/* Extra monster flags */

	byte monseen;       /* monster has been seen (>=2) or heard(==1) */
	bool ml;			/* Monster is "visible" */
	bool heard;         /* Monster has been heard recently */
	
	s16b temp_death;    /* Monsters is only temporarily dead */
	s16b ninelives;     /* counts how many times the monster has died temporarily */
	s16b extra2;        /* for new hallucenation-> monster is an imaginary monster */
	/* extra2 was either yes or no, now has different levels: */
	/* 1= semi-real illusory monster (slightly more potent than normal illusion) */
	/* 2= a semi-real illusory clone */
	/* 3= normal illusory monsters (from hallucenation or summon illusion(s) spell) */
	/* 4= an illsory clone of the monster who cast ILLUSION_CLONE (these cannot dissipate without true seeing) */
	/* 9= dissipated illusory monsters (these have been recognised by the PC as an illusion) */
	/* 10= static illusory monster (these never wake up -currently unused) */

	/* extra3 is now used to hold the m_idx of the monster whose clone this is (for extra2 == 2 or 4) */
	s16b extra3; /* that way it can clone aspects of the individual monster */
	
	s16b champ;         /* marks this monster as a pseudo-unique 'champion' */
	char champion_name[40];	/* psuedo-unique names */

	s16b hold_o_idx;	/* Object being held (if any) */

	/* for testing, not in savefiles, defined in get_moves() */
	int headtox;
	int headtoy;

#ifdef DRS_SMART_OPTIONS

	u32b smart;			/* Field for "adult_ai_learn" */

#endif /* DRS_SMART_OPTIONS */

};




/*
 * An entry for the object/monster allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */
struct alloc_entry
{
	s16b index;		/* The actual index */

	byte level;		/* Base dungeon level */
	byte prob1;		/* Probability, pass 1 */
	byte prob2;		/* Probability, pass 2 */
	byte prob3;		/* Probability, pass 3 */

	u16b total;		/* Unused for now */
};



/*
 * Structure for the "quests"
 *
 * Hack -- currently, only the "level" parameter is set, with the
 * semantics that "one (QUEST) monster of that level" must be killed,
 * and then the "level" is reset to zero, meaning "all done".  Later,
 * we should allow quests like "kill 100 fire hounds", and note that
 * the "quest level" is then the level past which progress is forbidden
 * until the quest is complete.  Note that the "QUESTOR" flag then could
 * become a more general "never out of depth" flag for monsters.
 */
struct quest
{
	byte level;		/* Dungeon level */
	int r_idx;		/* Monster race */

	int cur_num;	/* Number killed (unused) */
	int max_num;	/* Number required (unused) */
};




/*
 * A store owner
 */
struct owner_type
{
	u32b owner_name;	/* Name (offset) */

	s32b max_cost;		/* Purse limit */

	byte inflate;		/* Inflation */

	byte owner_race;	/* Owner race */
};




/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
struct store_type
{
	byte owner;				/* Owner index */

	byte stock_num;			/* Stock -- Number of entries */
	s16b stock_size;		/* Stock -- Total Size of Array */
	object_type *stock;		/* Stock -- Actual stock items */

	s16b table_num;     /* Table -- Number of entries */
	s16b table_size;    /* Table -- Total Size of Array */
	s16b *table;        /* Table -- Legal item kinds */
};


/*
 * A structure to hold class-dependent information on spells. (s_ptr)
 * This is taken from p_class.txt (not spell.txt)
 */
struct magic_type
{
	byte slevel;		/* Required level (to learn) */
	byte smana;			/* Required mana (to cast) */
	byte sfail;			/* Minimum chance of failure */
	byte sexp;			/* Encoded experience bonus */
	/* s16b difficulty */               /* for later */
};


/*
 * Information about the player's "magic"
 *
 * Note that a player with a spell_book of zero can't cast spells.
 */
struct player_magic
{
	magic_type info[PY_MAX_SPELLS];	/* The available spells */
};


/*
 * And here's the structure for the fixed spell information (s_info, from spell.txt)
 */
struct spell_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

    /* 0 = mage, 1 = priest, (2=newm, 3=luck, 4=alchemy, 5=black, 6=mind) */
	byte realm;			
	byte tval;			/* Item type for book this spell is in */
	byte sval;			/* Item sub-type for book (= book number) */
	byte snum;			/* Position of spell within book */
	byte spell_noise;   /* how much noise does this spell make */
	s16b school;        /* spell type (for later) */

	byte spell_index;	/* Index into player_magic array */
};


/*
 * Player sex info
 */
struct player_sex
{
	cptr title;			/* Type of sex */

	cptr winner;		/* Name of winner */
};


/*
 * Player racial info
 */
struct player_race
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	s16b r_adj[A_MAX];	/* Racial stat bonuses */

	s16b r_dis;			/* disarming */
	s16b r_dev;			/* magic devices */
	s16b r_sav;			/* saving throw */
	s16b r_stl;			/* stealth */
	s16b r_srh;			/* search ability */
	s16b r_fos;			/* alertness / search frequency */
	s16b r_thn;			/* combat (normal) */
	s16b r_thb;			/* combat (shooting) */
	s16b r_tht;			/* combat (thrown) */

	byte r_mhp;			/* Race hit-dice modifier */
	byte r_exp;			/* Race experience factor */

	byte rsize;			/* race size */

	byte b_age;			/* base age */
	byte m_age;			/* mod age */

	byte m_b_ht;		/* base height (males) */
	byte m_m_ht;		/* mod height (males) */
	byte m_b_wt;		/* base weight (males) */
	byte m_m_wt;		/* mod weight (males) */

	byte f_b_ht;		/* base height (females) */
	byte f_m_ht;		/* mod height (females) */
	byte f_b_wt;		/* base weight (females) */
	byte f_m_wt;		/* mod weight (females) */

	u32b choice;		/* recommended class choices (changed from a byte) */

	s16b hist;			/* Starting history index */

                 /* this uses object flags */
	u32b flags1;		/* Racial Flags, set 1 */
	u32b flags2;		/* Racial Flags, set 2 */
	u32b flags3;		/* Racial Flags, set 3 */
	u32b flags4;		/* Flags, set 4 */
};


/*
 * Starting equipment entry
 */
struct start_item
{
	byte tval;	/* Item's tval */
	byte sval;	/* Item's sval */
	byte min;	/* Minimum starting amount */
	byte max;	/* Maximum starting amount */
};


/*
 * Player class info
 */
struct player_class
{
	u32b name;			/* Name (offset) */

	u32b title[10];		/* Titles - offset */

	s16b c_adj[A_MAX];	/* Class stat modifier */

	s16b c_dis;			/* class disarming */
	s16b c_dev;			/* class magic devices */
	s16b c_sav;			/* class saving throws */
	s16b c_stl;			/* class stealth */
	s16b c_srh;			/* class searching ability */
	s16b c_fos;			/* class alertness / searching frequency */
	s16b c_thn;			/* class to hit (normal) */
	s16b c_thb;			/* class to hit (bows) */
	s16b c_tht;			/* class to hit (thrown) */

	s16b x_dis;			/* extra disarming */
	s16b x_dev;			/* extra magic devices */
	s16b x_sav;			/* extra saving throws */
	s16b x_stl;			/* extra stealth */
	s16b x_srh;			/* extra searching ability */
	s16b x_fos;			/* extra alertness / searching frequency */
	s16b x_thn;			/* extra to hit (normal) */
	s16b x_thb;			/* extra to hit (bows) */
	s16b x_tht;			/* extra to hit (thrown) */

	s16b c_mhp;			/* Class hit-dice adjustment */
	s16b c_exp;			/* Class experience factor */
	
	s16b calert;         /* now used for social class */

	u32b flags;			/* Class Flags */

	u16b max_attacks;	/* Maximum possible attacks */
	u16b min_weight;	/* Minimum weapon weight for calculations */
	u16b att_multiply;	/* Multiplier for attack calculations */

	byte spell_book;	/* Tval of spell books (if any) */
	u16b spell_stat;	/* Stat for spells (if any) */
	u16b spell_first;	/* Level of first spell */
	u16b spell_weight;	/* Weight that hurts spells */
	s16b useless_books; /* svals of books which this class can't use */

	u32b sense_base;	/* Base pseudo-id value */
	u16b sense_div;		/* Pseudo-id divisor (now unused) */

	start_item start_items[MAX_START_ITEMS];/* The starting inventory */

	player_magic spells; /* Magic spells */
};


/*
 * Player background information
 */
struct hist_type
{
	u32b text;			    /* Text (offset) */

	byte roll;			    /* Frequency of this entry */
	byte chart;			    /* Chart index */
	byte next;			    /* Next chart index */
	byte bonus;			    /* Social Class Bonus + 50 */
};



/*
 * Some more player information
 *
 * This information is retained across player lives
 */
struct player_other
{
	char full_name[32];		/* Full name */
	char base_name[32];		/* Base name */

	bool opt[OPT_MAX];		/* Options */

	u32b window_flag[ANGBAND_TERM_MAX];	/* Window flags */

	byte hitpoint_warn;		/* Hitpoint warning (0 to 9) */

	byte delay_factor;		/* Delay factor (0 to 9) */
};


/*
 * Most of the "player" information goes here. (p_ptr)
 *
 * This stucture gives us a large collection of player variables.
 *
 * This entire structure is wiped when a new character is born.
 *
 * This structure is more or less laid out so that the information
 * which must be saved in the savefile precedes all the information
 * which can be recomputed as needed.
 */
struct player_type
{
	s16b py;			/* Player location */
	s16b px;			/* Player location */

	byte psex;			/* Sex index */
	byte prace;			/* Race index */
	byte pclass;		/* Class index */

	byte theme;			/* themed level */

	byte hitdie;		/* Hit dice (sides) */
	byte expfact;		/* Experience factor */

	s16b age;			/* Character's age (also used to count wizmode deaths) */
	s16b ht;			/* Height */
	s16b wt;			/* Weight */
	s16b sc;			/* Social Class */

	s32b au;			/* Current Gold */

	s16b max_depth;		/* Max depth */
	s16b depth;			/* Cur depth */

	s16b max_lev;		/* Max level */
	s16b lev;			/* Cur level */

	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u16b exp_frac;		/* Cur exp frac (times 2^16) */

	s16b mhp;			/* Max hit pts */
	s16b chp;			/* Cur hit pts */
	u16b chp_frac;		/* Cur hit frac (times 2^16) */

	s16b msp;			/* Max mana pts */
	s16b csp;			/* Cur mana pts */
	u16b csp_frac;		/* Cur mana frac (times 2^16) */

	s16b stat_max[A_MAX];	/* Current "maximal" stat values */
	s16b stat_cur[A_MAX];	/* Current "natural" stat values */

	s16b timed[TMD_MAX];	/* Timed effects */

	s16b word_recall;	/* Word of recall counter */

	s16b energy;		/* Current energy */

	s16b food;			/* Current nutrition */
	
	s16b silver;        /* DJA: silver poison */
	s16b slime;         /* DJA: slimed state */
	s16b luck;          /* DJA: luck */
	s16b maxluck;		/* DJA: maximum luck (so can restore lost luck) */
	byte corrupt;       /* level of corruption */
	bool corrupting;    /* weilding a corrupting item (extracted, not in savefiles) */
	s16b spadjust;       /* amount of nonstandard speed adjustment */
	byte learnedcontrol; /* teleport control skill */
	s16b mimmic;		/* wand/rod sval to mimmic, +100 if rod (alchemy spell) */
	s16b menhance;		/* mimmiced wand is enhanced */

	s32b control_des;   /* stores destination while being controlled */
						/* (like monster roaming -didn't get it to work) */
	/* currently only used for caverns (during level generation) and gets lost when */
	/* the game is saved and exited. I'll add it to the savefile next time I break savefiles. */
	s16b speclev;		/* hold code for something special about the level (caverns, etc) */
	/* speclev: 1=cavern, ... */
	
	s16b danger_turn;   /* last time you got a danger feeling */
	s32b game_score;    /* game score, (redone, now counted as you play) */

	s16b danger_text;   /* last danger feeling (temporary, not in savefile) */
    byte warned;		/* has been warned (about drowning) */
	byte confusing;		/* Glowing hands */
	byte searching;		/* Currently searching */

	s16b extra1;         /* marks as in a space with a pit without being in the pit */
	s16b extra2;        /* to prevent breaking savefiles later */
	s16b extra3;        /* to prevent breaking savefiles later */
	
	s32b lastfullmoon;   /* turn of last full moon in town */
	s32b last_nap;

	byte spell_flags[PY_MAX_SPELLS]; /* Spell flags */

	byte spell_order[PY_MAX_SPELLS];	/* Spell order */

	s16b player_hp[PY_MAX_LEVEL];	/* HP Array */

	char died_from[80];		/* Cause of death */
	char history[250];	/* Initial history */

	u16b total_winner;		/* Total winner */
	u16b panic_save;		/* Panic save */

	u16b noscore;			/* Cheating flags */

	bool is_dead;			/* Player is dead */

	bool wizard;			/* Player is in wizard mode */


	/*** Temporary fields ***/

	bool playing;			/* True if player is playing */

	bool leaving;			/* True if player is leaving */
	bool seek_vault;        /* look for a vault on next level if TRUE */
	byte find_vault;        /* how likely to find a vault (in savefile) */
	int manafree;           /* activate staff of mana-free casting */
	/* room effects (ridx-1400) : 1=war, 2=pestilence, 3=famine, 4=flame, 5=ice, */
	/* 6=acid, 7=lightning, 8=wind(no missiles), 9=darkness, 10=sundial, 11=hallu, */
	/* 12=magic, 13=skull, 14=static, 15=cuts, 16=silence, 17=Titanium door, */
	/* 18=rising dead, 19=blinker, 20=drainXP, 21=purewater, 22=grepse, 23=prison */
	/* 24=healing, 25=blind 2nd sight (see end of monster.txt) */
	s16b roomeffect;
	/* byte roomeffectb; */

	s16b held_m_idx;		/* Monster holding you (if any) */

	bool create_up_stair;	/* Create up stair on next level */
	bool create_down_stair;	/* Create down stair on next level */

	s32b total_weight;		/* Total weight being carried */

	s16b inven_cnt;			/* Number of items in inventory */
	s16b equip_cnt;			/* Number of items in equipment */
	s16b pack_size_reduce;	/* Number of inventory slots used by the quiver */

	s16b target_set;		/* Target flag */
	s16b target_who;		/* Target identity */
	s16b target_row;		/* Target location y */
	s16b target_col;		/* Target location x */

	s16b health_who;		/* Health bar trackee */

	s16b monster_race_idx;	/* Monster race trackee */

	s16b object_kind_idx;	/* Object kind trackee */

	s16b energy_use;		/* Energy use this turn */

	s16b resting;			/* Resting counter */
	s16b running;			/* Running counter */
	bool running_withpathfind;      /* Are we using the pathfinder ? */

	s16b run_cur_dir;		/* Direction we are running */
	s16b run_old_dir;		/* Direction we came from */
	bool run_unused;		/* Unused (padding field) */
	bool run_open_area;		/* Looking for an open area */
	bool run_break_right;	/* Looking for a break (right) */
	bool run_break_left;	/* Looking for a break (left) */

	bool auto_pickup_okay;      /* Allow automatic pickup */

	s16b command_cmd;		/* Gives identity of current command */
	s16b command_arg;		/* Gives argument of current command */
	s16b command_rep;		/* Gives repetition of current command */
	s16b command_dir;		/* Gives direction of current command */
	event_type command_cmd_ex; /* Gives additional information of current command */

	s16b command_see;		/* See "cmd1.c" */
	s16b command_wrk;		/* See "cmd1.c" */

	s16b command_new;		/* Hack -- command chaining XXX XXX */

	s16b new_spells;		/* Number of spells available */

	bool cumber_armor;	/* Mana draining armor */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield;	/* Heavy weapon */
	bool heavy_shoot;	/* Heavy shooter */
	bool icky_wield;	/* Icky weapon */

	s16b cur_lite;		/* Radius of lite (if any) */

	u32b notice;		/* Special Updates (bit flags) */
	u32b update;		/* Pending Updates (bit flags) */
	u32b redraw;		/* Normal Redraws (bit flags) */
	u32b window;		/* Window Redraws (bit flags) */

	s16b stat_use[A_MAX];	/* Current modified stats */
	s16b stat_top[A_MAX];	/* Maximal modified stats */

	/*** Extracted fields ***/

	s16b stat_add[A_MAX];	/* Equipment stat bonuses */
	s16b stat_ind[A_MAX];	/* Indexes into stat tables */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */

	bool resist_acid;	/* Resist acid */
	bool resist_elec;	/* Resist lightning */
	bool resist_fire;	/* Resist fire */
	bool resist_cold;	/* Resist cold */
	bool resist_pois;	/* Resist poison */
	bool weakresist_pois;	/* partial poison resistance */
	bool breath_shield; /* damage reduction against monster breath */

	bool resist_charm;	/* Resist charm */
	bool resist_fear;	/* Resist fear */
	bool resist_lite;	/* Resist light */
	bool resist_dark;	/* Resist darkness */
	bool resist_blind;	/* Resist blindness */
	bool resist_confu;	/* Resist confusion */
	bool resist_sound;	/* Resist sound */
	bool resist_shard;	/* Resist shards */
	bool resist_nexus;	/* Resist nexus */
	bool resist_nethr;	/* Resist nether */
	bool resist_chaos;	/* Resist chaos */
	bool resist_disen;	/* Resist disenchant */
	s16b resist_static; /* Resist static (drain charges) */
	bool resist_silver; /* Resist silver (grepse) poison */
	bool resist_slime;  /* Resist slime */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	bool slow_digest;	/* Slower digestion */
	bool ffall;			/* Feather falling */
	bool regenerate;	/* Regeneration */
	bool telepathy;		/* Telepathy */
	bool see_inv;		/* See invisible */
	bool free_act;		/* Free action */
	bool hold_life;		/* Hold life */
	bool darkvis;       /* darkvision */
	bool nice;          /* nice */
	bool peace;         /* less agressive */
	bool listening;     /* much more likely to hear unseen monsters */
	s16b throwmult;		/* multiplier for throwing weapons (uses pval (max4, min2)) */

	/* branding damage from elemental rings: (these are now x2, not x3) */
	bool brand_cold;	/* */
	bool brand_acid;	/* */
	bool brand_elec;	/* */
	bool brand_fire;	/* */
	bool brand_pois;	/* */

	bool accident;      /* accidents happen with your weapon */
    bool stopregen;     /* Prevents HP regeneration */
	bool impact;		/* Earthquake blows */
	bool aggravate;		/* Aggravate monsters */
	bool teleport;		/* Random teleporting */
	bool exp_drain;		/* Experience draining */
	bool blinker;       /* involuntary blinking */

	bool telecontrol;   /* teleport control */

	bool bless_blade;	/* Blessed blade */

	s16b dis_to_h;		/* Known bonus to hit */
	s16b dis_to_d;		/* Known bonus to dam */
	s16b dis_to_a;		/* Known bonus to ac */

	s16b dis_ac;		/* Known base ac */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to dam */
	s16b to_a;			/* Bonus to ac */

	s16b ac;			/* Base ac */

	/* has nothing to do with infravision anymore */
	s16b see_infra;		/* stat modified by !temporary boost */

	s16b skills[SKILL_MAX];	/* Skills */
	
	s16b palert;        /* Player alertness based on FOS skill (used to be global) */
	u32b oppnoise;			/* Derived from stealth */

	s16b num_blow;		/* Number of blows */
	s16b num_fire;		/* Number of shots */
	s16b extra_blows;	/* number of extra blows */

	byte ammo_mult;		/* Ammo multiplier */
	byte ammo_tval;		/* Ammo variety */
	byte bow_range;     /* */

	bool cursed_quiver;	/* The quiver is cursed */

	s16b pspeed;		/* Current speed */
	
	/* temporary thing for knowledge menu (the only way I could figure how to do it) */
	bool fakehome;      /* for home inventory in knowledge menu */
};


/*
 * Semi-Portable High Score List Entry (128 bytes)
 *
 * All fields listed below are null terminated ascii strings.
 *
 * In addition, the "number" fields are right justified, and
 * space padded, to the full available length (minus the "null").
 *
 * Note that "string comparisons" are thus valid on "pts".
 */

typedef struct high_score high_score;

struct high_score
{
	char what[8];		/* Version info (string) */

	char pts[10];		/* Total Score (number) */

	char gold[10];		/* Total Gold (number) */

	char turns[10];		/* Turns Taken (number) */

	char day[10];		/* Time stamp (string) */

	char who[16];		/* Player Name (string) */

	char uid[8];		/* Player UID (number) */

	char sex[2];		/* Player Sex (string) */
	char p_r[3];		/* Player Race (number) */
	char p_c[3];		/* Player Class (number) */

	char cur_lev[4];		/* Current Player Level (number) */
	char cur_dun[4];		/* Current Dungeon Level (number) */
	char max_lev[4];		/* Max Player Level (number) */
	char max_dun[4];		/* Max Dungeon Level (number) */

	char how[32];		/* Method of death (string) */
};


typedef struct flavor_type flavor_type;

struct flavor_type
{
	u32b text;      /* Text (offset) */
	
	byte tval;      /* Associated object type */
	byte sval;      /* Associated object sub-type */

	byte d_attr;    /* Default flavor attribute */
	char d_char;    /* Default flavor character */

	byte x_attr;    /* Desired flavor attribute */
	char x_char;    /* Desired flavor character */
};

/* Information for object auto-inscribe */
struct autoinscription
{
	s16b kind_idx;
	s16b inscription_idx;
};

/*
 * Info used to manage quiver groups
 */
struct quiver_group_type
{
	char cmd;		/* The command used to perform an action with the objects in the group */
	byte color;		/* The color of the pseudo-tag used for the group */
};
