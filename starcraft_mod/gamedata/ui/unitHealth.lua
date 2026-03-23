-- unitHealth.lua – draws a small health bar above every unit

local widget = {}

function widget:Initialize()
    self.font = gl.LoadFont("FreeSans", 10)
end

function widget:DrawWorld()
    for _, uid in ipairs(Spring.GetAllUnits()) do
        local hp, maxHp = Spring.GetUnitHealth(uid)
        if hp and maxHp and maxHp > 0 then
            local x, y, z = Spring.GetUnitPosition(uid)
            local healthPct = hp / maxHp
            local barWidth = 30 * healthPct
            gl.Color(0,0,0,0.8)
            gl.Rect(x-15, y+20, x+15, y+23)
            gl.Color(0,1,0,0.8)
            gl.Rect(x-15, y+20, x-15+barWidth, y+23)
            gl.Color(1,1,1,1)
        end
    end
end

return widget
