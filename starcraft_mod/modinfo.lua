-- modinfo.lua – basic metadata for the Spring RTS StarCraft‑like mod
return {
    ai = {"advanced_ai"},
    name            = "MiniStar",
    shortname       = "ministar",
    version         = "0.1",
    description     = "A very small StarCraft‑style RTS built on Spring",
    mutator        = false,
    game            = "Spring",
    modtype         = "game",
    shortdescription = "StarCraft clone demo",
    fogOfWar        = true,
    -- No external dependencies – all assets are generated at runtime
}
