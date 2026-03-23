-- barracks.lua – script for the Barracks building
-- Handles training of Marine units.

local script = {}

function script.StartTraining(unitID, unitDefID)
    Spring.PlaySoundFile("sounds/train.wav", 0.8)
end

function script.FinishTraining(unitID, newUnitID)
    -- Optionally give the newly trained Marine a rally point.
    local x,y,z = Spring.GetUnitPosition(unitID)
    Spring.GiveOrderToUnit(newUnitID, {"move", x+30, y, z}, {}, 0)
end

return script
