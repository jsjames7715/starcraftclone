-- terrainInfo.lua – movement class definitions for MiniStar
-- Each class defines speed modifiers for different ground types.
-- Spring automatically reads this file if placed under gamedata/terrain/.

return {
    KBOT2 = {
        maxSlope = 30,
        maxWaterDepth = 0,
        speedMod = 1.0,
    },
    TANK2 = {
        maxSlope = 15,
        maxWaterDepth = 0,
        speedMod = 0.8,
    },
}
