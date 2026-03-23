-- advanced_ai.lua – a very small but slightly smarter AI
-- This AI will:
--   1. Build a Barracks if it does not already have one.
--   2. Train Marines continuously when the Barracks is idle.
--   3. Once it has 3 Marines, issue an attack order toward the nearest player Command Center.

local AI = {}

function AI:Initialize()
    self.team = Spring.GetMyTeamID()
    self.barracks = nil
    self.marineCount = 0
    self.attackSent = false
    self.frame = 0
end

function AI:GameFrame(frame)
    self.frame = frame
    if frame % 30 ~= 0 then return end   -- run logic ~1× per second (30 FPS)
    self:UpdateState()
    self:MakeDecisions()
end

function AI:UpdateState()
    -- Find own Barracks (if any)
    self.barracks = nil
    for _, uid in ipairs(Spring.GetTeamUnits(self.team)) do
        local udid = Spring.GetUnitDefID(uid)
        local ud = UnitDefs[udid]
        if ud and ud.name == "barracks" then
            self.barracks = uid
            break
        end
    end
    -- Count own Marines
    self.marineCount = 0
    for _, uid in ipairs(Spring.GetTeamUnits(self.team)) do
        local udid = Spring.GetUnitDefID(uid)
        local ud = UnitDefs[udid]
        if ud and ud.name == "marine" then
            self.marineCount = self.marineCount + 1
        end
    end
end

function AI:MakeDecisions()
    if not self.barracks then
        -- Build a Barracks near the first building we own (usually a Command Center)
        local buildings = Spring.GetTeamUnits(self.team)
        if #buildings > 0 then
            local cmd = buildings[1]
            local x,y,z = Spring.GetUnitPosition(cmd)
            -- Issue a build order for a Barracks 8 units east of the command centre
            Spring.GiveOrderToUnit(cmd, {"build", "barracks"}, {}, 0)
        end
        return
    end

    -- If we have a Barracks, train Marines when idle
    local queue = Spring.GetCommandQueue(self.barracks, 1)
    if #queue == 0 and self.marineCount < 6 then
        Spring.GiveOrderToUnit(self.barracks, {"train", "marine"}, {}, 0)
    end

    -- Attack when we have enough forces
    if self.marineCount >= 3 and not self.attackSent then
        local target = self:FindNearestEnemyCommandCenter()
        if target then
            for _, uid in ipairs(Spring.GetTeamUnits(self.team)) do
                local udid = Spring.GetUnitDefID(uid)
                local ud = UnitDefs[udid]
                if ud and ud.name == "marine" then
                    Spring.GiveOrderToUnit(uid, {"attack", target}, {}, 0)
                end
            end
            self.attackSent = true
        end
    end

    -- *** New behaviours ***
    self:Scout()
    self:ExpandBase()
    self:TechTreeDecisions()
end

function AI:FindNearestEnemyCommandCenter()
    local myPos = Spring.GetTeamStartPosition(self.team)
    local closestDist = math.huge
    local closest = nil
    for _, uid in ipairs(Spring.GetAllUnits()) do
        local udid = Spring.GetUnitDefID(uid)
        local ud = UnitDefs[udid]
        if ud and ud.name == "command_center" then
            local ux,uy,uz = Spring.GetUnitPosition(uid)
            local dist = (ux - myPos[1])^2 + (uy - myPos[2])^2
            if dist < closestDist then
                closestDist = dist
                closest = uid
            end
        end
    end
    return closest
end

-- Scout behaviour: send a free worker to a random unexplored location
function AI:Scout()
    -- Find an idle worker
    for _, uid in ipairs(Spring.GetTeamUnits(self.team)) do
        local udid = Spring.GetUnitDefID(uid)
        local ud = UnitDefs[udid]
        if ud and ud.name == "worker" then
            local orders = Spring.GetCommandQueue(uid, 1)
            if #orders == 0 then
                -- random point within map bounds (assume 0-2000)
                local tx = math.random(0, 2000)
                local tz = math.random(0, 2000)
                Spring.GiveOrderToUnit(uid, {"move", tx, 0, tz}, {}, 0)
                break
            end
        end
    end
end

-- Expand base if we have enough minerals and no second command centre
function AI:ExpandBase()
    local minerals = Spring.GetTeamResources(self.team, "metal") or 0
    if minerals > 800 and not self.secondCC then
        -- Build a second Command Center near the first one (simple offset)
        local units = Spring.GetTeamUnits(self.team)
        if #units > 0 then
            local cmd = units[1]
            Spring.GiveOrderToUnit(cmd, {"build", "command_center"}, {}, 0)
            self.secondCC = true
        end
    end
end

-- Simple tech‑tree decision helper
function AI:TechTreeDecisions()
    -- After we have a Factory, start training Tanks
    local hasFactory = false
    local factoryID = nil
    for _, uid in ipairs(Spring.GetTeamUnits(self.team)) do
        local udid = Spring.GetUnitDefID(uid)
        local ud = UnitDefs[udid]
        if ud and ud.name == "factory" then
            hasFactory = true
            factoryID = uid
            break
        end
    end
    if hasFactory and factoryID then
        local queue = Spring.GetCommandQueue(factoryID, 1)
        if #queue == 0 then
            Spring.GiveOrderToUnit(factoryID, {"train", "tank"}, {}, 0)
        end
    end
end

return AI
