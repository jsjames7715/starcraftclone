-- mission.lua – simple win/lose condition for MiniStar
-- The game ends when the enemy Command Center is destroyed.

function gadget:GetInfo()
    return {
        name      = "Mission Win/Lose",
        desc      = "Ends the game when the enemy base is gone",
        author    = "MiniStar Dev",
        date      = "2026-03-23",
        license   = "GPL-3",
        layer     = 0,
        enabled   = true,
    }
end

local enemyCC = nil
local playerTeam = nil

function gadget:Initialize()
    playerTeam = Spring.GetMyTeamID()
    -- locate the enemy Command Center (the one not belonging to the player)
    for _, uid in ipairs(Spring.GetAllUnits()) do
        local ud = UnitDefs[Spring.GetUnitDefID(uid)]
        if ud and ud.name == "command_center" then
            if Spring.GetUnitTeam(uid) ~= playerTeam then
                enemyCC = uid
                break
            end
        end
    end
end

function gadget:UnitDestroyed(unitID, unitDefID, unitTeam)
    if unitID == enemyCC then
        Spring.Echo("All enemy bases destroyed – YOU WIN!")
        Spring.GameOver(true)   -- true = player victory
    end
end

function gadget:GameFrame(frame)
    -- optional: auto‑lose if player CC dies
    if not Spring.GetUnitHealth(Spring.GetUnitNearestEnemy(playerTeam, 0, 0, 0)) then
        -- placeholder – you could store player CC ID similarly to enemyCC
    end
end

return true
