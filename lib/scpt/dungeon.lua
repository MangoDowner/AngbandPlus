-- Internal lua file in charge of dungeon stuff

function place_dungeon(y, x, d_idx)
	if d_idx then
        	wild_map(y, x).entrance = 1000 + d_idx
       	else
        	wild_map(y, x).entrance = 0
        end
end

function dungeon(d_idx)
        return d_info[1 + d_idx]
end

function wild_feat(wild)
        return wf_info[1 + wild.feat]
end

function explode_dir(dir)
        return ddy[dir + 1], ddx[dir + 1]
end

-- Check if the map is a filename or directly a map
function load_map(map, y, x)
        if strsub(map, 1, 5) == "#!map" then
                -- Print to a temporary file and parse it, then delet the file
                make_temp_file()
                print_hook(map)
                close_temp_file()
                %load_map(get_temp_name(), y, x)
                end_temp_file()
        else
                %load_map(map, y, x)
        end
end

-- Place a trap for a specific level
function place_trap(y, x, level)
	local old_dun = dun_level
        dun_level = level
        %place_trap(y, x)
        dun_level = old_dun
end
