-- worker.lua – script for the Worker unit
-- Handles building and resource gathering.

local script = {}

function script.BeginMoving(unitID)
    -- Called when the unit receives a move order.
    Spring.SetUnitNoSelect(unitID, false)
end

function script.BeginConstruction(unitID, buildingDefID)
    -- Called when the worker starts constructing a building.
    Spring.SetUnitNoSelect(unitID, true)
end

function script.FinishConstruction(unitID, buildingID)
    Spring.SetUnitNoSelect(unitID, false)
end

function script.Update(unitID)
    local team = Spring.GetUnitTeam(unitID)
    local x, _, z = Spring.GetUnitPosition(unitID)

    -- Gather metal from the map (simple constant rate)
    Spring.AddTeamResource(team, "metal", 0.1)

    -- Gather gas if near a refinery
    for _, uid in ipairs(Spring.GetTeamUnits(team)) do
        local udid = Spring.GetUnitDefID(uid)
        local ud = UnitDefs[udid]
        if ud and ud.name == "refinery" then
            local fx, _, fz = Spring.GetUnitPosition(uid)
            local dist = math.sqrt((fx - x)^2 + (fz - z)^2)
            if dist < 120 then  -- refinery harvest range
                Spring.AddTeamResource(team, "gas", 0.05)
                break
            end
        end
    end
end

return script
