-- LuaProfiler
-- Copyright Kepler Project 2005-2007 (http://www.keplerproject.org/luaprofiler)
-- $Id: summary.lua,v 1.5 2007/08/22 21:05:13 carregal Exp $

-- Function that reads one profile file
function ReadProfile(file)

	local profile

	-- Check if argument is a file handle or a filename
	if io.type(file) == "file" then
		profile = file

	else
		-- Open profile
		profile = io.open(file)
		end

	-- Table for storing each profile's set of lines
	line_buffer = {}

	-- Get all profile lines
	local i = 1
	for line in profile:lines() do
		line_buffer[i] = line
		i = i + 1
    end

	-- Close file
	profile:close()
	return line_buffer
	end

-- Function that creates the summary info
function CreateSummary(lines, summary)

	local global_time = 0

	-- Note: ignore first line
	for i = 2, table.getn(lines) do
            stack_level, funcname, cost_time, sourcename = string.match(lines[i], "(%d*)%s*(%a+%d*%a+)%s*(%d*)%s*(.*)");
		if stack_level ~= nil then
			if summary[funcname] == nil then
				summary[funcname] = {};
				summary[funcname]["calls"] = 1;
				summary[funcname]["costs"] = cost_time;
				summary[funcname]["func"] =  funcname;
				summary[funcname]["source"] = sourcename;
			else
				summary[funcname]["calls"] = summary[funcname]["calls"] + 1
				summary[funcname]["costs"] = summary[funcname]["costs"] + cost_time;
			end

			if stack_level == "0" then
				global_time = global_time + cost_time;
			end

		end
	end

    print("Global Total Time: " .. global_time)
    return global_time
end

-- Global time
global_t = 0

-- Summary table
profile_info = {}

-- Check file type
local verbose = false
local filename
if arg[1] == "-v" or arg[1] == "-V" then
  verbose = true
  filename = arg[2]
else
  filename = arg[1]
end
if filename then
  file = io.open(filename)
else
  print("Usage")
  print("-----")
  print("lua summary.lua [-v] <profile_log>")
  os.exit()
end
if not file then
  print("File " .. filename .. " does not exist!")
  os.exit()
end
firstline = file:read(11)

local lines = ReadProfile(file);
global_t = CreateSummary(lines, profile_info);

-- Sort table by total time
sorted = {}
for k, v in pairs(profile_info) do table.insert(sorted, v) end

table.sort(sorted, function (a, b)
    aval = tonumber(a["costs"]);
    bval = tonumber(b["costs"]);
    if aval == nil then return false end;
    if bval == nil then return true end; 
    return tonumber(a["costs"]) > tonumber(b["costs"]); 
end)

-- Output summary
print("Calls\tCost Time\t%Time\tAverage Per Call\tNodeName\t\t\t\tSource")

for k, v in pairs(sorted) do
    local average = v["costs"] / v["calls"];
    local percent = 100 * v["costs"] / global_t
        print(v["calls"] .. "\t" .. v["costs"] .. "       \t" .. string.format("%.2f", percent) .. "\t"
			.. string.format("%16.3f", average) .. "\t" .. string.format("%-32s", v["func"]) .. "\t" .. v["source"])
    end

