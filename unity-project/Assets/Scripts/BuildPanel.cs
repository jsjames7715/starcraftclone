using UnityEngine;
using UnityEngine.UI;
using TMPro;
using System.Collections.Generic;

public class BuildPanel : MonoBehaviour
{
    [Header("Panels")]
    public GameObject terranPanel;
    public GameObject zergPanel;
    public GameObject protossPanel;
    public GameObject unitsPanel;
    public GameObject buildingsPanel;
    public GameObject upgradesPanel;

    [Header("Content")]
    public GridLayoutGroup unitGrid;
    public GridLayoutGroup buildingGrid;
    public GridLayoutGroup upgradeGrid;

    [Header("Item Prefabs")]
    public GameObject unitButtonPrefab;
    public GameObject buildingButtonPrefab;
    public GameObject upgradeButtonPrefab;

    [Header("Buttons")]
    public Button unitsTab;
    public Button buildingsTab;
    public Button upgradesTab;
    public Button terranTab;
    public Button zergTab;
    public Button protossTab;

    private Dictionary<UnitType, BuildItem> unitItems = new Dictionary<UnitType, BuildItem>();
    private Dictionary<BuildingType, BuildItem> buildingItems = new Dictionary<BuildingType, BuildItem>();

    void Start()
    {
        // Hide all panels initially
        if (terranPanel != null) terranPanel.SetActive(false);
        if (zergPanel != null) zergPanel.SetActive(false);
        if (protossPanel != null) protossPanel.SetActive(false);

        // Set up tabs
        if (unitsTab != null) unitsTab.onClick.AddListener(() => ShowPanel(unitsPanel));
        if (buildingsTab != null) buildingsTab.onClick.AddListener(() => ShowPanel(buildingsPanel));
        if (upgradesTab != null) upgradesTab.onClick.AddListener(() => ShowPanel(upgradesPanel));

        if (terranTab != null) terranTab.onClick.AddListener(() => ShowFaction(terranPanel));
        if (zergTab != null) zergTab.onClick.AddListener(() => ShowFaction(zergPanel));
        if (protossTab != null) protossTab.onClick.AddListener(() => ShowFaction(protossPanel));

        // Build UI
        BuildUnitButtons();
        BuildBuildingButtons();
        BuildUpgradeButtons();

        // Show first panel
        ShowPanel(unitsPanel);
    }

    void ShowPanel(GameObject panel)
    {
        if (unitsPanel != null) unitsPanel.SetActive(false);
        if (buildingsPanel != null) buildingsPanel.SetActive(false);
        if (upgradesPanel != null) upgradesPanel.SetActive(false);

        if (panel != null) panel.SetActive(true);
    }

    void ShowFaction(GameObject panel)
    {
        if (terranPanel != null) terranPanel.SetActive(false);
        if (zergPanel != null) zergPanel.SetActive(false);
        if (protossPanel != null) protossPanel.SetActive(false);

        if (panel != null) panel.SetActive(true);
    }

    void BuildUnitButtons()
    {
        if (unitGrid == null) return;

        // Clear existing
        foreach (Transform child in unitGrid.transform)
        {
            Destroy(child.gameObject);
        }

        // Terran units
        AddUnitButton(UnitType.Worker, "SCV", 50, 0, "S");
        AddUnitButton(UnitType.Marine, "Marine", 50, 0, "M");
        AddUnitButton(UnitType.Medic, "Medic", 50, 0, "M");
        AddUnitButton(UnitType.Firebat, "Firebat", 75, 0, "F");
        AddUnitButton(UnitType.Ghost, "Ghost", 100, 25, "G");
        AddUnitButton(UnitType.Vulture, "Vulture", 75, 0, "V");
        AddUnitButton(UnitType.Goliath, "Goliath", 100, 0, "G");
        AddUnitButton(UnitType.Tank, "Siege Tank", 150, 100, "T");
        AddUnitButton(UnitType.Wraith, "Wraith", 100, 0, "W");
        AddUnitButton(UnitType.Battlecruiser, "Battlecruiser", 400, 300, "B");

        // Zerg units
        AddUnitButton(UnitType.Zergling, "Zergling", 50, 0, "Z");
        AddUnitButton(UnitType.Baneling, "Baneling", 25, 0, "B");
        AddUnitButton(UnitType.Roach, "Roach", 75, 0, "R");
        AddUnitButton(UnitType.Hydralisk, "Hydralisk", 50, 0, "H");
        AddUnitButton(UnitType.Mutalisk, "Mutalisk", 100, 100, "M");
        AddUnitButton(UnitType.Ultralisk, "Ultralisk", 300, 200, "U");

        // Protoss units
        AddUnitButton(UnitType.Probe, "Probe", 50, 0, "P");
        AddUnitButton(UnitType.Zealot, "Zealot", 100, 0, "Z");
        AddUnitButton(UnitType.Dragoon, "Dragoon", 125, 50, "D");
        AddUnitButton(UnitType.DarkTemplar, "Dark Templar", 125, 100, "D");
        AddUnitButton(UnitType.Carrier, "Carrier", 350, 250, "C");
    }

    void AddUnitButton(UnitType type, string name, int minerals, int gas, string hotkey)
    {
        GameObject btn = Instantiate(unitButtonPrefab, unitGrid.transform);
        BuildItem item = btn.GetComponent<BuildItem>();
        if (item != null)
        {
            item.Initialize(type, name, minerals, gas, hotkey);
            unitItems[type] = item;
        }
    }

    void BuildBuildingButtons()
    {
        if (buildingGrid == null) return;

        foreach (Transform child in buildingGrid.transform)
        {
            Destroy(child.gameObject);
        }

        // Terran buildings
        AddBuildingButton(BuildingType.CommandCenter, "Command Center", 400, 0, "C");
        AddBuildingButton(BuildingType.Barracks, "Barracks", 150, 0, "B");
        AddBuildingButton(BuildingType.Factory, "Factory", 200, 0, "F");
        AddBuildingButton(BuildingType.Starport, "Starport", 250, 0, "S");
        AddBuildingButton(BuildingType.SupplyDepot, "Supply Depot", 100, 0, "D");
        AddBuildingButton(BuildingType.Refinery, "Refinery", 100, 0, "R");
        AddBuildingButton(BuildingType.EngineeringBay, "Engineering Bay", 125, 0, "E");
        AddBuildingButton(BuildingType.Armory, "Armory", 150, 0, "A");
        AddBuildingButton(BuildingType.MissileTurret, "Missile Turret", 100, 0, "T");
        AddBuildingButton(BuildingType.Bunker, "Bunker", 100, 0, "U");
    }

    void AddBuildingButton(BuildingType type, string name, int minerals, int gas, string hotkey)
    {
        GameObject btn = Instantiate(buildingButtonPrefab, buildingGrid.transform);
        BuildItem item = btn.GetComponent<BuildItem>();
        if (item != null)
        {
            item.Initialize(type, name, minerals, gas, hotkey);
            buildingItems[type] = item;
        }
    }

    void BuildUpgradeButtons()
    {
        if (upgradeGrid == null) return;

        foreach (Transform child in upgradeGrid.transform)
        {
            Destroy(child.gameObject);
        }

        // Add upgrade buttons
        AddUpgradeButton("Infantry Weapons", "Weapons Level 1", 100, 0, "1");
        AddUpgradeButton("Infantry Armor", "Armor Level 1", 100, 0, "2");
        AddUpgradeButton("Stim Pack", "Stim Pack", 100, 100, "S");
        AddUpgradeButton("Siege Mode", "Siege Tech", 150, 100, "T");
    }

    void AddUpgradeButton(string name, string description, int minerals, int gas, string hotkey)
    {
        GameObject btn = Instantiate(upgradeButtonPrefab, upgradeGrid.transform);
        BuildItem item = btn.GetComponent<BuildItem>();
        if (item != null)
        {
            item.Initialize(name, description, minerals, gas, hotkey);
        }
    }
}

public class BuildItem : MonoBehaviour
{
    public Image icon;
    public TextMeshProUGUI nameText;
    public TextMeshProUGUI costText;
    public TextMeshProUGUI hotkeyText;
    public Image resourceIcon;
    public GameObject disabledOverlay;

    private int mineralCost;
    private int gasCost;
    private UnitType? unitType;
    private BuildingType? buildingType;

    public void Initialize(UnitType type, string name, int minerals, int gas, string hotkey)
    {
        unitType = type;
        mineralCost = minerals;
        gasCost = gas;

        if (nameText != null) nameText.text = name;
        if (costText != null) costText.text = $"{minerals}";
        if (hotkeyText != null) hotkeyText.text = $"[{hotkey}]";

        UpdateAvailability();
    }

    public void Initialize(BuildingType type, string name, int minerals, int gas, string hotkey)
    {
        buildingType = type;
        mineralCost = minerals;
        gasCost = gas;

        if (nameText != null) nameText.text = name;
        if (costText != null) costText.text = $"{minerals}";
        if (hotkeyText != null) hotkeyText.text = $"[{hotkey}]";

        UpdateAvailability();
    }

    public void Initialize(string name, string description, int minerals, int gas, string hotkey)
    {
        mineralCost = minerals;
        gasCost = gas;

        if (nameText != null) nameText.text = name;
        if (costText != null) costText.text = $"{minerals}";
        if (hotkeyText != null) hotkeyText.text = $"[{hotkey}]";

        UpdateAvailability();
    }

    void Update()
    {
        UpdateAvailability();
    }

    void UpdateAvailability()
    {
        bool canAfford = GameManager.Instance.HasResources(mineralCost, gasCost);
        if (disabledOverlay != null) disabledOverlay.SetActive(!canAfford);
    }

    public void OnClick()
    {
        if (unitType.HasValue)
        {
            // Train unit
            Building[] buildings = FindObjectsOfType<Building>();
            foreach (Building b in buildings)
            {
                if (b.team == Team.Player && b.CanProduce(unitType.Value))
                {
                    b.QueueUnit(unitType.Value);
                    break;
                }
            }
        }
        else if (buildingType.HasValue)
        {
            // Enter build mode
            BuildManager.Instance.EnterBuildMode(buildingType.Value);
        }
    }
}
