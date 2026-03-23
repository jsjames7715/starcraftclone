-- resourceBar.lua – very simple UI panel for MiniStar
-- This script draws the player's mineral count, gas and supply on screen.
-- It is loaded by the engine via the UI configuration (see uiConfig.lua).

local widget = {}

function widget:Initialize()
    self.font = gl.LoadFont("FreeSans", 16)  -- uses a standard system font
    self.metal = 0
    self.gas = 0
    self.supplyUsed = 0
    self.supplyMax = 0
end

function widget:Update()
    local team = Spring.GetMyTeamID()
    self.metal = Spring.GetTeamResources(team, "metal") or 0
    self.gas = Spring.GetTeamResources(team, "gas") or 0
    local used, max = Spring.GetTeamResources(team, "supply") or 0, 0
    self.supplyUsed = used or 0
    self.supplyMax = max or 0
end

function widget:DrawScreen()
    local viewY = Spring.GetViewGeometry()
    local x = 10
    local y = viewY - 30
    local w = 200
    local h = 20
    gl.Color(0,0,0,0.6)
    gl.Rect(x, y - h*3, x + w, y)
    gl.Color(1,1,1,1)
    self.font:Print(string.format("Metal: %d", self.metal), x + 5, y - h*2 + 2, 1)
    self.font:Print(string.format("Gas: %d", self.gas), x + 5, y - h*1 + 2, 1)
    self.font:Print(string.format("Supply: %d/%d", self.supplyUsed, self.supplyMax), x + 5, y + 2, 1)
end

return widget
