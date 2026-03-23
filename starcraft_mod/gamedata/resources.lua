-- resources.lua – defines the two resources used by MiniStar
return {
    metal = {
        name = "Metal",
        description = "Minerals harvested from the ground",
        start = 500,
        max   = 9999,
    },
    gas = {
        name = "Gas",
        description = "Vespene gas harvested from refineries",
        start = 0,
        max   = 9999,
    },
    supply = {
        name = "Supply",
        description = "Population capacity provided by buildings",
        start = 0,
        max   = 9999,
    },
}