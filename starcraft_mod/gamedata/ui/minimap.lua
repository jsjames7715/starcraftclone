-- minimap.lua – tiny minimap widget for MiniStar
-- Shows a scaled‑down view of the world and a rectangle for the camera.

local widget = {}
local mapSize = 2000  -- match map dimensions in units
local miniSize = 200   -- pixel size of minimap
local offsetX = 0
local offsetY = 0

function widget:Initialize()
    self.font = gl.LoadFont("FreeSans", 12)
end

function widget:DrawScreen()
    local view = Spring.GetViewGeometry()
    offsetX = view - miniSize - 10
    offsetY = 10
    gl.Color(0,0,0,0.6)
    gl.Rect(offsetX, offsetY, offsetX+miniSize, offsetY+miniSize)
    gl.Color(1,1,1,1)
    -- draw units as dots
    for _, uid in ipairs(Spring.GetAllUnits()) do
        local ux,uy,uz = Spring.GetUnitPosition(uid)
        local px = offsetX + (ux / mapSize) * miniSize
        local py = offsetY + (uz / mapSize) * miniSize
        gl.Point(px, py)
    end
    -- draw camera rectangle (approximate)
    local camX, camY = Spring.GetCameraPosition()
    local camW, camH = Spring.GetViewGeometry()
    local camRectX = offsetX + (camX / mapSize) * miniSize
    local camRectY = offsetY + (camY / mapSize) * miniSize
    local camRectW = (camW / mapSize) * miniSize
    local camRectH = (camH / mapSize) * miniSize
    gl.Color(1,1,0,0.8)
    gl.Rect(camRectX, camRectY, camRectX+camRectW, camRectY+camRectH)
end

return widget
