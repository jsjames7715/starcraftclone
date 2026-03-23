-- simple_ai.lua – very naive AI for MiniStar
-- This script is loaded by the engine as a Lua AI module.
-- It periodically spawns a marine from the enemy barracks and orders it to move toward the nearest player command centre.

local AI = {}

function AI:Initialize()
    self.frame = 0
    self.spawnCooldown = 300   -- frames (~10 seconds at 30 FPS)
    self.commanders = {}
    self:FindEnemyBuildings()
end

function AI:FindEnemyBuildings()
    self.commanders = {}
    for _, unit in ipairs(Spring.GetAllUnits()) do
        local udid = Spring.GetUnitDefID(unit)
        local ud = UnitDefs[udid]
        if ud and ud.name == "barracks" and Spring.GetUnitTeam(unit) ~= Spring.GetMyTeamID() then
            table.insert(self.commanders, unit)
        end
    end
end

function AI:GameFrame(frame)
    self.frame = frame
    if frame % self.spawnCooldown == 0 then
        self:SpawnMarine()
    end
end

function AI:SpawnMarine()
    if #self.commanders == 0 then return end
    local barracks = self.commanders[1]
    local team = Spring.GetMyTeamID()
    local x,y,z = Spring.GetUnitPosition(barracks)
    Spring.GiveOrderToUnit(barracks, {"build", "marine"}, {}, 0)
    -- After a short delay, command the new marine to attack the player's command centre
    Spring.CallCOBScript(barracks, "SetFireState", 2) -- optional: set attack mode
end

return AI
