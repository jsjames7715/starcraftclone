-- command_center.lua – script for the Command Center building
-- Allows training Workers.

local script = {}

function script.StartBuilding(unitID, unitDefID)
    -- When a worker builds a Command Center, play a sound (if defined).
    Spring.PlaySoundFile("sounds/construct.wav", 1.0)
end

function script.FinishBuilding(unitID)
    -- Nothing special for now.
end

return script
