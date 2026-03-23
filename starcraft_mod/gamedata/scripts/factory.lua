-- factory.lua – script for the Factory building
-- Handles training of Tank units and Fighter (if researched).

local script = {}

-- Helper to check if fighter is researched for the team
local function fighterUnlocked(teamID)
    -- Access the research gadget's table via Spring.GetTeamRulesParam?
    -- Simpler: use a global variable set by research gadget.
    -- We'll store a team rule param: "fighterResearched" (0/1)
    local val = Spring.GetTeamRulesParam(teamID, "fighterResearched") or 0
    return val == 1
end

function script.StartTraining(unitID, unitDefID)
    Spring.PlaySoundFile("sounds/train.wav", 0.8)
end

function script.FinishTraining(unitID, newUnitID)
    local x,y,z = Spring.GetUnitPosition(unitID)
    local team = Spring.GetUnitTeam(unitID)
    local udid = Spring.GetUnitDefID(newUnitID)
    local ud = UnitDefs[udid]
    if ud and ud.name == "fighter" then
        -- Fighters spawn slightly above ground
        Spring.GiveOrderToUnit(newUnitID, {"move", x, y+10, z}, {}, 0)
    else
        -- Tanks and others
        Spring.GiveOrderToUnit(newUnitID, {"move", x+40, y, z}, {}, 0)
    end
end

-- Called when the factory receives a build order; we can intercept unwanted builds.
function script.QueryBuildInfo(unitID, unitDefID)
    local team = Spring.GetUnitTeam(unitID)
    local ud = UnitDefs[unitDefID]
    if ud and ud.name == "fighter" then
        if not fighterUnlocked(team) then
            -- Return false to disallow building this unit
            return false
        end
    end
    return true
end

return script