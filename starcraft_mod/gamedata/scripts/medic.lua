-- medic.lua – script for the Medic unit
-- Heals nearby friendly units over time.

local script = {}

function script.BeginMoving(unitID)
    Spring.SetUnitNoSelect(unitID, false)
end

function script.Update(unitID)
    local team = Spring.GetUnitTeam(unitID)
    local x, y, z = Spring.GetUnitPosition(unitID)
    -- Find friendly units within heal range (weaponDefs.heal.range = 120)
    local friends = Spring.GetUnitsInSphere(x, y, z, 120, team)
    for _, uid in ipairs(friends) do
        if uid ~= unitID then
            local _, maxHp, _, _, _, _, buildProgress = Spring.GetUnitHealth(uid)
            if maxHp and buildProgress == 1 then  -- only fully built units
                local hp = Spring.GetUnitHealth(uid)
                local healAmount = 2  -- heal per second (approx)
                if hp < maxHp then
                    Spring.AddUnitDamage(uid, -healAmount)  -- negative damage heals
                end
            end
        end
    end
end

return script