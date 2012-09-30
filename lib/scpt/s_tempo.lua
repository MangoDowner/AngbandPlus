-- Handles thhe temporal school


MAGELOCK = add_spell
{
	["name"] = 	"Magelock",
        ["school"] = 	{SCHOOL_TEMPORAL},
        ["level"] = 	1,
        ["mana"] = 	1,
        ["mana_max"] = 	35,
        ["fail"] = 	10,
        ["spell"] = 	function()
        		if get_level(MAGELOCK, 50) >= 30 then
	        		local ret, x, y, c_ptr

                		if get_level(MAGELOCK, 50) >= 40 then
        	                	ret, x, y = tgt_pt()
		                        if ret == FALSE then return end
                                else
                                	y = py
                                        x = px
                                end
                        	cave_set_feat(y, x, 3)
                        else
        	                ret, dir = get_aim_dir()
                                if ret == FALSE then return FALSE end
                                wizard_lock(dir)
                        end
	end,
	["info"] = 	function()
                       	return ""
	end,
        ["desc"] =	{
        		"Magicaly locks a door",
                        "At level 30 it creates a glyph of warding",
                        "At level 40 the glyph can be placed anywhere in the field of vision"
        }
}

SLOWMONSTER = add_spell
{
	["name"] = 	"Slow Monster",
        ["school"] = 	{SCHOOL_TEMPORAL},
        ["level"] = 	10,
        ["mana"] = 	10,
        ["mana_max"] = 	15,
        ["fail"] = 	10,
        ["spell"] = 	function()
               		local ret, dir

			ret, dir = get_aim_dir()
                        if ret == FALSE then return end
                        if get_level(SLOWMONSTER, 50) >= 20 then
                        	fire_ball(GF_OLD_SLOW, dir, 40 + get_level(SLOWMONSTER, 160), 1)
                        else
                        	fire_bolt(GF_OLD_SLOW, dir, 40 + get_level(SLOWMONSTER, 160))
                        end
	end,
	["info"] = 	function()
                        if get_level(SLOWMONSTER, 50) >= 20 then
	                       	return "power "..(40 + get_level(SLOWMONSTER, 160)).." rad 1"
                        else
	                       	return "power "..(40 + get_level(SLOWMONSTER, 160))
                        end
	end,
        ["desc"] =	{
                        "Magically slows down the passing of time around a monster",
                        "At level 20 it affects a zone"
        }
}

ESSENSESPEED = add_spell
{
	["name"] = 	"Essense of Speed",
        ["school"] = 	{SCHOOL_TEMPORAL},
        ["level"] = 	15,
        ["mana"] = 	20,
        ["mana_max"] = 	40,
        ["fail"] = 	10,
        ["spell"] = 	function()
        		if player.fast == 0 then set_fast(10 + randint(10) + get_level(ESSENSESPEED, 50), 5 + get_level(ESSENSESPEED, 20)) end
	end,
	["info"] = 	function()
                       	return "dur "..(10 + get_level(ESSENSESPEED, 50)).."+d10 speed "..(5 + get_level(ESSENSESPEED, 20))
	end,
        ["desc"] =	{
        		"Magicaly increases the passing of time around you",
        }
}

BANISHMENT = add_spell
{
	["name"] = 	"Banishment",
        ["school"] = 	{SCHOOL_TEMPORAL, SCHOOL_CONVEYANCE},
        ["level"] = 	30,
        ["mana"] = 	30,
        ["mana_max"] = 	40,
        ["fail"] = 	10,
        ["spell"] = 	function()
                        project_los(GF_AWAY_ALL, 40 + get_level(BANISHMENT, 160))
                        if get_level(SLOWMONSTER, 50) >= 15 then
                                project_los(GF_STASIS, 20 + get_level(BANISHMENT, 120))
                        end
	end,
	["info"] = 	function()
                     	return "power "..(40 + get_level(BANISHMENT, 160))
	end,
        ["desc"] =	{
        		"Disrupt the space/time continuum in your area and teleports all monsters away",
                        "At level 15 it also may lock them in a time bubble for some turns"
        }
}
