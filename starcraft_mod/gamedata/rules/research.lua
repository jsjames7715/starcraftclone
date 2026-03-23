-- research.lua – gadget that handles researching the Fighter unit
-- Requires 1000 metal, takes 30 seconds (900 frames at 30 FPS).
-- When complete, unlocks the Fighter for the Factory.

local gadget = {}

function gadget:GetInfo()
    return {
        name      = "Fighter Research",
        desc      = "Researches Fighter unit when enough metal is available",
        author    = "MiniStar Dev",
        date      = "2026-03-23",
        license   = "GPL-3",
        layer     = 0,
        enabled   = true,
    }
end

-- research state per team: { researching = bool, progress = framesLeft, unlocked = bool }
local research = {}

function gadget:Initialize()
    local team = Spring.GetMyTeamID()
    research[team] = { researching = false, progress = 0, unlocked = false }
end

function gadget:GameFrame(frame)
    local team = Spring.GetMyTeamID()
    local state = research[team]
    if not state then return end

    local metal = Spring.GetTeamResources(team, "metal") or 0

    -- start research if conditions met and not already researching/unlocked
    if not state.researching and not state.unlocked and metal >= 1000 then
        state.researching = true
        state.progress = 900  -- 30 seconds * 30 FPS
        Spring.Echo("Starting Fighter research...")
    end

    if state.researching then
        state.progress = state.progress - 1
        if state.progress <= 0 then
            state.researching = false
            state.unlocked = true
            Spring.SetTeamRulesParam(team, "fighterResearched", 1)
            Spring.Echo("Fighter research complete! Fighter now available.")
        end
    end
end

-- Helper for other gadgets/scripts to check if fighter is unlocked
function gadget:IsFighterUnlocked(team)
    local state = research[team]
    return state and state.unlocked or false
end

return gadget