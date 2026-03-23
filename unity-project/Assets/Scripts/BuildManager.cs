using UnityEngine;
using System.Collections.Generic;

public class BuildManager : MonoBehaviour
{
    public static BuildManager Instance { get; private set; }

    [Header("Build Mode")]
    public bool isInBuildMode = false;
    public BuildingType selectedBuildingType;
    public GameObject ghostBuildingPrefab;
    public Material validPlacementMaterial;
    public Material invalidPlacementMaterial;

    [Header("Grid Settings")]
    public float gridSize = 2f;
    public bool snapToGrid = true;

    [Header("Placement Rules")]
    public float minBuildDistance = 10f;
    public LayerMask groundLayer;
    public LayerMask buildingLayer;
    public LayerMask unitLayer;

    private GameObject ghostInstance;
    private bool canPlace = false;

    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
    }

    void Update()
    {
        if (isInBuildMode)
        {
            UpdateGhostBuilding();
            HandleBuildInput();
        }
    }

    public void EnterBuildMode(BuildingType buildingType)
    {
        selectedBuildingType = buildingType;
        isInBuildMode = true;

        // Create ghost
        if (ghostBuildingPrefab != null)
        {
            ghostInstance = Instantiate(ghostBuildingPrefab);
            SetGhostColor(invalidPlacementMaterial);
        }

        Cursor.lockState = CursorLockMode.Confined;
    }

    public void ExitBuildMode()
    {
        isInBuildMode = false;
        selectedBuildingType = BuildingType.CommandCenter;

        if (ghostInstance != null)
        {
            Destroy(ghostInstance);
            ghostInstance = null;
        }

        Cursor.lockState = CursorLockMode.Locked;
    }

    void UpdateGhostBuilding()
    {
        if (ghostInstance == null) return;

        // Get mouse position on ground
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, 1000f, groundLayer))
        {
            Vector3 pos = hit.point;

            // Snap to grid
            if (snapToGrid)
            {
                pos.x = Mathf.Round(pos.x / gridSize) * gridSize;
                pos.z = Mathf.Round(pos.z / gridSize) * gridSize;
            }

            pos.y = 0;
            ghostInstance.transform.position = pos;

            // Check if valid placement
            canPlace = CheckPlacementValidity(pos);

            // Update color
            SetGhostColor(canPlace ? validPlacementMaterial : invalidPlacementMaterial);
        }
    }

    bool CheckPlacementValidity(Vector3 position)
    {
        // Check distance from existing buildings
        Building[] buildings = FindObjectsOfType<Building>();
        foreach (Building b in buildings)
        {
            float dist = Vector3.Distance(position, b.transform.position);
            if (dist < minBuildDistance)
            {
                return false;
            }
        }

        // Check collision with units
        Collider[] units = Physics.OverlapSphere(position, 1f, unitLayer);
        if (units.Length > 0)
        {
            return false;
        }

        // Check collision with other buildings
        Collider[] colliders = Physics.OverlapBox(position, Vector3.one, Quaternion.identity, buildingLayer);
        if (colliders.Length > 0)
        {
            return false;
        }

        return true;
    }

    void SetGhostColor(Material mat)
    {
        if (ghostInstance == null) return;

        Renderer[] renderers = ghostInstance.GetComponentsInChildren<Renderer>();
        foreach (Renderer r in renderers)
        {
            r.material = mat;
        }
    }

    void HandleBuildInput()
    {
        // Place building
        if (Input.GetMouseButtonDown(0) && canPlace)
        {
            PlaceBuilding(ghostInstance.transform.position);
        }

        // Cancel build mode
        if (Input.GetMouseButtonDown(1) || Input.GetKeyDown(KeyCode.Escape))
        {
            ExitBuildMode();
        }

        // Rotate building
        if (Input.GetKeyDown(KeyCode.R))
        {
            if (ghostInstance != null)
            {
                ghostInstance.transform.Rotate(0, 90f, 0);
            }
        }
    }

    void PlaceBuilding(Vector3 position)
    {
        // Check resources
        int cost = GetBuildingCost(selectedBuildingType);
        if (!GameManager.Instance.SpendMinerals(cost))
        {
            UIManager.Instance.ShowAlert("Not enough minerals!");
            return;
        }

        // Spawn building
        GameObject buildingPrefab = Resources.Load<GameObject>($"Buildings/{selectedBuildingType}");
        if (buildingPrefab != null)
        {
            GameObject buildingObj = Instantiate(buildingPrefab, position, ghostInstance.transform.rotation);
            Building building = buildingObj.GetComponent<Building>();
            
            if (building != null)
            {
                building.isUnderConstruction = true;
                GameManager.Instance.AddMinerals(cost); // Refund for now
            }
        }

        // Exit build mode
        ExitBuildMode();
    }

    int GetBuildingCost(BuildingType type)
    {
        switch (type)
        {
            case BuildingType.CommandCenter: return 400;
            case BuildingType.Barracks: return 150;
            case BuildingType.Factory: return 200;
            case BuildingType.Starport: return 250;
            case BuildingType.SupplyDepot: return 100;
            case BuildingType.Refinery: return 100;
            case BuildingType.EngineeringBay: return 125;
            case BuildingType.Armory: return 150;
            case BuildingType.MissileTurret: return 100;
            case BuildingType.Bunker: return 100;
            default: return 200;
        }
    }
}
