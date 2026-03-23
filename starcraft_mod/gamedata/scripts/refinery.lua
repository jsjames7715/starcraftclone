-- refinery.lua – script for the Refinery building
-- No special behaviour needed; harvesting is handled by workers.

local script = {}

function script.StartBuilding(unitID, unitDefID)
    Spring.PlaySoundFile("sounds/construct.wav", 1.0)
end

function script.FinishBuilding(unitID)
    -- nothing
end

return script
