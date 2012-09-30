-- handle the nature school

GROWTREE = add_spell
{
	["name"] = 	"Grow Trees",
        ["school"] = 	{SCHOOL_NATURE, SCHOOL_TEMPORAL},
        ["level"] = 	6,
        ["mana"] = 	6,
        ["mana_max"] = 	30,
        ["fail"] = 	20,
        ["spell"] = 	function()
        		grow_trees(2 + get_level(GROWTREE, 7))
	end,
	["info"] = 	function()
			return "rad "..(2 + get_level(GROWTREE, 7))
	end,
        ["desc"] =	{
        		"Makes trees grow extremely quickly around you",
        }
}

HEALING = add_spell
{
	["name"] = 	"Healing",
        ["school"] = 	{SCHOOL_NATURE},
        ["level"] = 	10,
        ["mana"] = 	15,
        ["mana_max"] = 	50,
        ["fail"] = 	20,
        ["spell"] = 	function()
        		hp_player(player.mhp * (15 + get_level(HEALING, 35)) / 100)
	end,
	["info"] = 	function()
			return "heal "..(15 + get_level(HEALING, 35)).."% = "..(player.mhp * (15 + get_level(HEALING, 35)) / 100).."hp"
	end,
        ["desc"] =	{
        		"Heals a percent of hitpoints",
        }
}

RECOVERY = add_spell
{
	["name"] = 	"Recovery",
        ["school"] = 	{SCHOOL_NATURE},
        ["level"] = 	15,
        ["mana"] = 	10,
        ["mana_max"] = 	25,
        ["fail"] = 	20,
        ["spell"] = 	function()
        		set_poisoned(player.poisoned / 2)
                        if get_level(RECOVERY, 50) >= 5 then
                        	set_poisoned(0)
                                set_cut(0)
                        end
                        if get_level(RECOVERY, 50) >= 10 then
				do_res_stat(A_STR)
				do_res_stat(A_CON)
				do_res_stat(A_DEX)
				do_res_stat(A_WIS)
				do_res_stat(A_INT)
				do_res_stat(A_CHR)
                        end
                        if get_level(RECOVERY, 50) >= 15 then
                        	restore_level()
                        end
	end,
	["info"] = 	function()
			return ""
	end,
        ["desc"] =	{
        		"Reduces the length of time that you are poisoned",
                        "At level 5 it cures poison and cuts",
                        "At level 10 it restores drained stats",
                        "At level 15 it restores lost experience"
        }
}

REGENERATION = add_spell
{
	["name"] = 	"Regeneration",
        ["school"] = 	{SCHOOL_NATURE},
        ["level"] = 	20,
        ["mana"] = 	30,
        ["mana_max"] = 	55,
        ["fail"] = 	20,
        ["spell"] = 	function()
        		if player.tim_regen == 0 then set_tim_regen(randint(10) + 5 + get_level(REGENERATION, 50), 300 + get_level(REGENERATION, 700)) end
	end,
	["info"] = 	function()
			return "dur "..(5 + get_level(REGENERATION, 50)).."+d10 power "..(300 + get_level(REGENERATION, 700))
	end,
        ["desc"] =	{
        		"Increases your body's regeneration rate",
        }
}


SUMMONANNIMAL = add_spell
{
        ["name"] =      "Summon Animal",
        ["school"] = 	{SCHOOL_NATURE},
        ["level"] = 	25,
        ["mana"] = 	25,
        ["mana_max"] = 	50,
        ["fail"] = 	20,
        ["spell"] = 	function()
        		summon_specific_level = 25 + get_level(SUMMONANNIMAL, 50)
        		summon_monster(py, px, dun_level, TRUE, SUMMON_ANIMAL)
	end,
	["info"] = 	function()
			return "level "..(25 + get_level(SUMMONANNIMAL, 50))
	end,
        ["desc"] =	{
        		"Summons a leveled animal to your aid",
        }
}
