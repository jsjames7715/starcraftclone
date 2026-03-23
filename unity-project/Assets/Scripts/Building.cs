using UnityEngine;
using System.Collections.Generic;

public enum BuildingType
{
    // Terran
    CommandCenter, Barracks, Factory, Starport, SupplyDepot, Refinery,
    Academy, Armory, TechLab, ComSat, NuclearSilo, EngineeringBay,
    MissileTurret, Bunker, GhostAcademy, PhysicsLab, FusionCore,
    PlanetaryFortress, OrbitalCommand,
    
    // Zerg
    Hatchery, Lair, Hive, SpawningPool, EvolutionChamber, Extractor,
    HydraliskDen, LurkerAspect, Spire, GreaterSpire, SporeColony,
    SunkenColony, UltraliskCavern, NydusCanal, InfestationPit,
    
    // Protoss
    Nexus, Gateway, WarpGate, CyberneticsCore, Forge, Stargate,
    FleetBeacon, RoboticsBay, RoboticsFacility, TwilightCouncil,
    TemplarArchives, DarkShrine, PhotonCannon, ShieldBattery
}

public class Building : MonoBehaviour
{
    [Header("Building Info")]
    public BuildingType buildingType;
    public Team team;
    public string buildingName;
    public Sprite buildingIcon;
    public Vector2Int size = new Vector2Int(3, 3); // Grid size

    [Header("Stats")]
    public int maxHealth = 1000;
    public int currentHealth = 1000;
    public int armor = 0;
    public float sightRange = 8f;

    [Header("Cost")]
    public int mineralCost = 400;
    public int gasCost = 0;

    [Header("State")]
    public bool isAlive = true;
    public bool isSelected = false;
    public bool isUnderConstruction = false;
    public float constructionProgress = 0f;
    public bool isPowered = true;

    [Header("Production")]
    public List<UnitType> canProduce = new List<UnitType>();
    public List<UnitType> canResearch = new List<UnitType>();
    public float productionQueueTime = 0f;
    public UnitType queuedUnit;
    public bool isProducing = false;

    [Header("Rally Points")]
    public Transform rallyPoint;
    public Vector3 rallyPosition;
    public bool useRallyPoint = true;

    [Header("Components")]
    public GameObject selectionIndicator;
    public GameObject healthBarPrefab;
    private GameObject healthBarInstance;
    public Transform healthBarAnchor;
    public GameObject constructionProgressBar;

    [Header("Power")]
    public bool requiresPower = false;
    public float powerConsumption = 0f;

    protected virtual void Start()
    {
        currentHealth = maxHealth;
        CreateHealthBar();
        CreateSelectionIndicator();
        
        if (rallyPoint != null)
        {
            rallyPosition = rallyPoint.position;
        }
    }

    protected virtual void Update()
    {
        if (!isAlive) return;

        // Construction
        if (isUnderConstruction)
        {
            UpdateConstruction();
        }

        // Production
        if (isProducing && queuedUnit != null)
        {
            UpdateProduction();
        }
    }

    void UpdateConstruction()
    {
        // Construction is handled by builder units
        if (constructionProgress >= 100f)
        {
            isUnderConstruction = false;
            if (constructionProgressBar != null)
            {
                constructionProgressBar.SetActive(false);
            }
        }
    }

    void UpdateProduction()
    {
        // Get production time from unit data
        float productionTime = GetUnitProductionTime(queuedUnit);
        
        productionQueueTime -= Time.deltaTime * GameManager.Instance.gameSpeed;
        
        if (productionQueueTime <= 0)
        {
            CompleteProduction();
        }
    }

    float GetUnitProductionTime(UnitType unitType)
    {
        // Default production times
        switch (unitType)
        {
            case UnitType.Worker: return 17f;
            case UnitType.Marine: return 25f;
            case UnitType.Zergling: return 17f;
            case UnitType.Ghost: return 40f;
            case UnitType.Tank: return 60f;
            default: return 30f;
        }
    }

    void CompleteProduction()
    {
        // Spawn the unit at rally point
        Vector3 spawnPos = rallyPosition;
        if (!useRallyPoint)
        {
            spawnPos = transform.position + transform.forward * 3f;
        }

        Unit newUnit = Unit.CreateUnit(queuedUnit, spawnPos, team);
        
        isProducing = false;
        queuedUnit = null;
    }

    public bool CanProduce(UnitType unitType)
    {
        if (!isAlive || isUnderConstruction) return false;
        return canProduce.Contains(unitType);
    }

    public void QueueUnit(UnitType unitType)
    {
        if (!CanProduce(unitType)) return;

        UnitData data = UnitDatabase.GetUnitData(unitType);
        if (data == null) return;

        if (!GameManager.Instance.HasResources(data.mineralCost, data.gasCost))
        {
            return; // Not enough resources
        }

        GameManager.Instance.SpendMinerals(data.mineralCost);
        GameManager.Instance.SpendGas(data.gasCost);
        
        queuedUnit = unitType;
        productionQueueTime = GetUnitProductionTime(unitType);
        isProducing = true;
    }

    public void CancelProduction()
    {
        if (queuedUnit != null)
        {
            // Refund half resources
            UnitData data = UnitDatabase.GetUnitData(queuedUnit);
            if (data != null)
            {
                GameManager.Instance.AddMinerals(data.mineralCost / 2);
                GameManager.Instance.AddGas(data.gasCost / 2);
            }
        }
        
        isProducing = false;
        queuedUnit = null;
        productionQueueTime = 0f;
    }

    public void SetRallyPoint(Vector3 position)
    {
        rallyPosition = position;
        if (rallyPoint != null)
        {
            rallyPoint.position = position;
        }
        useRallyPoint = true;
    }

    public void SetRallyPoint(Unit unit)
    {
        if (unit != null)
        {
            rallyPosition = unit.transform.position;
            useRallyPoint = false;
        }
        else
        {
            useRallyPoint = true;
        }
    }

    public void TakeDamage(int damage)
    {
        if (!isAlive) return;

        int actualDamage = Mathf.Max(1, damage - armor);
        currentHealth -= actualDamage;

        // Update health bar
        if (healthBarInstance != null)
        {
            HealthBar hb = healthBarInstance.GetComponent<HealthBar>();
            if (hb != null)
            {
                hb.UpdateHealth((float)currentHealth / maxHealth);
            }
        }

        if (currentHealth <= 0)
        {
            DestroyBuilding();
        }
    }

    public void Repair()
    {
        // Heal building
        currentHealth = Mathf.Min(maxHealth, currentHealth + 5);
    }

    void DestroyBuilding()
    {
        isAlive = false;
        
        // Death effect
        GameObject deathEffect = Resources.Load<GameObject>("Effects/BuildingExplosion");
        if (deathEffect != null)
        {
            Instantiate(deathEffect, transform.position, Quaternion.identity);
        }

        if (isSelected)
        {
            SelectionManager.Instance.DeselectBuilding(this);
        }

        Destroy(gameObject, 1f);
    }

    void CreateHealthBar()
    {
        if (healthBarPrefab != null && healthBarAnchor != null)
        {
            healthBarInstance = Instantiate(healthBarPrefab, healthBarAnchor.position, Quaternion.identity, healthBarAnchor);
        }
    }

    void CreateSelectionIndicator()
    {
        if (selectionIndicator != null)
        {
            selectionIndicator.SetActive(false);
        }
    }

    public void SetSelected(bool selected)
    {
        isSelected = selected;
        if (selectionIndicator != null)
        {
            selectionIndicator.SetActive(selected);
        }
    }

    void OnMouseDown()
    {
        if (team == Team.Player)
        {
            SelectionManager.Instance.SelectBuilding(this);
        }
    }
}

public static class UnitDatabase
{
    private static Dictionary<UnitType, UnitData> unitData = new Dictionary<UnitType, UnitData>();

    static UnitDatabase()
    {
        InitializeUnitData();
    }

    static void InitializeUnitData()
    {
        // Terran Units
        unitData[UnitType.Worker] = new UnitData("SCV", 50, 0, 1, 60, 5, 0, 0, 3f, 5f);
        unitData[UnitType.Marine] = new UnitData("Marine", 50, 0, 1, 40, 6, 1, 0, 4f, 5f);
        unitData[UnitType.Medic] = new UnitData("Medic", 50, 0, 1, 60, 0, 0, 0, 3.75f, 6f);
        unitData[UnitType.Firebat] = new UnitData("Firebat", 75, 0, 2, 50, 8, 1, 0, 3.28f, 2f);
        unitData[UnitType.Ghost] = new UnitData("Ghost", 100, 25, 2, 45, 10, 0, 0, 3.94f, 7f);
        unitData[UnitType.Vulture] = new UnitData("Vulture", 75, 0, 2, 80, 20, 0, 0, 4.13f, 5f);
        unitData[UnitType.Goliath] = new UnitData("Goliath", 100, 0, 2, 125, 12, 1, 0, 3.16f, 6f);
        unitData[UnitType.Tank] = new UnitData("Siege Tank", 150, 100, 3, 150, 40, 2, 0, 2.06f, 11f);
        unitData[UnitType.Wraith] = new UnitData("Wraith", 100, 0, 2, 120, 8, 0, 0, 4.5f, 5f);
        unitData[UnitType.Fighter] = new UnitData("Wraith (Air)", 100, 0, 2, 100, 16, 0, 0, 4.5f, 5f);
        unitData[UnitType.Dropship] = new UnitData("Dropship", 100, 0, 2, 100, 0, 2, 0, 4.84f, 0f);
        unitData[UnitType.Battlecruiser] = new UnitData("Battlecruiser", 400, 300, 6, 500, 25, 3, 0, 1.88f, 11f);
        unitData[UnitType.ScienceVessel] = new UnitData("Science Vessel", 100, 225, 2, 200, 0, 1, 0, 3.66f, 0f);
        unitData[UnitType.Valkyrie] = new UnitData("Valkyrie", 225, 125, 3, 200, 14, 2, 0, 3.94f, 8f);

        // Zerg Units
        unitData[UnitType.Zergling] = new UnitData("Zergling", 50, 0, 1, 35, 5, 0, 0, 4.13f, 1f);
        unitData[UnitType.Baneling] = new UnitData("Baneling", 25, 0, 1, 30, 15, 0, 0, 4.13f, 1f);
        unitData[UnitType.Roach] = new UnitData("Roach", 75, 0, 2, 130, 12, 1, 0, 3.66f, 4f);
        unitData[UnitType.Hydralisk] = new UnitData("Hydralisk", 50, 0, 1, 80, 10, 0, 0, 3.66f, 5f);
        unitData[UnitType.Lurker] = new UnitData("Lurker", 100, 50, 2, 120, 20, 1, 0, 3.66f, 6f);
        unitData[UnitType.Mutalisk] = new UnitData("Mutalisk", 100, 100, 2, 120, 9, 0, 0, 4.5f, 3f);
        unitData[UnitType.Ultralisk] = new UnitData("Ultralisk", 300, 200, 4, 400, 20, 2, 0, 2.81f, 1f);
        unitData[UnitType.Queen] = new UnitData("Queen", 100, 100, 2, 120, 8, 0, 0, 2.81f, 7f);
        unitData[UnitType.Defiler] = new UnitData("Defiler", 100, 100, 2, 80, 0, 0, 0, 2.63f, 0f);
        unitData[UnitType.Infestor] = new UnitData("Infestor", 100, 150, 2, 90, 0, 0, 0, 2.63f, 0f);
        unitData[UnitType.Overlord] = new UnitData("Overlord", 100, 0, 0, 200, 0, 0, 0, 2.06f, 0f);
        unitData[UnitType.Overseer] = new UnitData("Overseer", 150, 50, 0, 200, 0, 0, 0, 2.44f, 0f);

        // Protoss Units
        unitData[UnitType.Probe] = new UnitData("Probe", 50, 0, 1, 40, 5, 0, 0, 3.94f, 5f);
        unitData[UnitType.Zealot] = new UnitData("Zealot", 100, 0, 2, 100, 8, 1, 0, 3.94f, 1f);
        unitData[UnitType.Dragoon] = new UnitData("Dragoon", 125, 50, 2, 100, 12, 1, 0, 3.28f, 6f);
        unitData[UnitType.DarkTemplar] = new UnitData("Dark Templar", 125, 100, 2, 80, 45, 1, 0, 3.94f, 1f);
        unitData[UnitType.HighTemplar] = new UnitData("High Templar", 100, 150, 2, 40, 0, 0, 0, 2.81f, 0f);
        unitData[UnitType.Archon] = new UnitData("Archon", 0, 0, 4, 340, 25, 0, 0, 2.81f, 3f);
        unitData[UnitType.Scout] = new UnitData("Scout", 75, 0, 2, 80, 8, 0, 0, 4.72f, 5f);
        unitData[UnitType.Corsair] = new UnitData("Corsair", 100, 100, 2, 100, 8, 0, 0, 4.5f, 5f);
        unitData[UnitType.Carrier] = new UnitData("Carrier", 350, 250, 6, 300, 6, 2, 0, 2.06f, 8f);
        unitData[UnitType.Reaver] = new UnitData("Reaver", 200, 100, 4, 200, 100, 0, 0, 1.5f, 8f);
        unitData[UnitType.Shuttle] = new UnitData("Shuttle", 100, 0, 2, 80, 0, 2, 0, 3.66f, 0f);
        unitData[UnitType.Arbiter] = new UnitData("Arbiter", 200, 200, 4, 200, 10, 1, 0, 2.63f, 6f);
    }

    public static UnitData GetUnitData(UnitType type)
    {
        if (unitData.ContainsKey(type))
        {
            return unitData[type];
        }
        return null;
    }
}

public class UnitData
{
    public string name;
    public int mineralCost;
    public int gasCost;
    public int supplyCost;
    public int maxHealth;
    public int attackDamage;
    public int armor;
    public int shields;
    public float moveSpeed;
    public float attackRange;

    public UnitData(string name, int minerals, int gas, int supply, int hp, int damage, int armor, int shields, float speed, float range)
    {
        this.name = name;
        this.mineralCost = minerals;
        this.gasCost = gas;
        this.supplyCost = supply;
        this.maxHealth = hp;
        this.attackDamage = damage;
        this.armor = armor;
        this.shields = shields;
        this.moveSpeed = speed;
        this.attackRange = range;
    }
}
