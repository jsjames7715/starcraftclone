-- tooltip.lua – shows unit name & health when mouse hovers over a unit

local widget = {}
local hoverUnit = nil

function widget:Update()
    local mx, my, _ = Spring.GetMouseState()
    local units = Spring.GetUnitsInRectangle(mx-5, my-5, mx+5, my+5)
    hoverUnit = #units > 0 and units[1] or nil
end

function widget:DrawScreen()
    if not hoverUnit then return end
    local name = UnitDefs[Spring.GetUnitDefID(hoverUnit)].humanName or "Unit"
    local hp, maxHp = Spring.GetUnitHealth(hoverUnit)
    local text = string.format("%s – %.0f/%.0f", name, hp or 0, maxHp or 0)
    local sx, sy = Spring.GetScreenCoordinates(Spring.GetUnitPosition(hoverUnit))
    gl.Color(0,0,0,0.7)
    gl.Rect(sx-60, sy+15, sx+60, sy+35)
    gl.Color(1,1,1,1)
    widget.font:Print(text, sx-55, sy+20, 1)
end

return widget
