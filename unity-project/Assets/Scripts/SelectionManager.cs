using UnityEngine;
using System.Collections.Generic;

public class SelectionManager : MonoBehaviour
{
    public static SelectionManager Instance { get; private set; }

    [Header("Selection")]
    public List<Unit> selectedUnits = new List<Unit>();
    public List<Building> selectedBuildings = new List<Building>();
    public int maxSelection = 12;

    [Header("Selection Box")]
    public bool isSelecting = false;
    public Vector3 selectionStart;
    public Vector3 selectionEnd;
    public Material selectionBoxMaterial;
    public GameObject selectionBoxPrefab;
    private GameObject selectionBoxInstance;

    [Header("Multi-Selection")]
    public bool shiftHeld = false;
    public bool controlHeld = false;

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
        // Check for modifier keys
        shiftHeld = Input.GetKey(KeyCode.LeftShift) || Input.GetKey(KeyCode.RightShift);
        controlHeld = Input.GetKey(KeyCode.LeftControl) || Input.GetKey(KeyCode.RightControl);

        // Handle selection input
        HandleSelectionInput();
    }

    void HandleSelectionInput()
    {
        // Start selection box
        if (Input.GetMouseButtonDown(0))
        {
            isSelecting = true;
            selectionStart = Input.mousePosition;
        }

        // Update selection box
        if (isSelecting)
        {
            selectionEnd = Input.mousePosition;
            UpdateSelectionBox();
        }

        // End selection
        if (Input.GetMouseButtonUp(0))
        {
            EndSelection();
        }

        // Right click commands
        if (Input.GetMouseButtonDown(1))
        {
            ExecuteRightClickCommand();
        }
    }

    void UpdateSelectionBox()
    {
        if (selectionBoxInstance == null && selectionBoxPrefab != null)
        {
            selectionBoxInstance = Instantiate(selectionBoxPrefab);
        }

        if (selectionBoxInstance != null)
        {
            Vector3 center = (selectionStart + selectionEnd) / 2f;
            Vector3 size = new Vector3(
                Mathf.Abs(selectionEnd.x - selectionStart.x),
                0,
                Mathf.Abs(selectionEnd.y - selectionStart.y)
            );

            selectionBoxInstance.transform.position = center;
            selectionBoxInstance.transform.localScale = size / 100f;
        }
    }

    void EndSelection()
    {
        isSelecting = false;

        if (selectionBoxInstance != null)
        {
            Destroy(selectionBoxInstance);
        }

        if (!shiftHeld && !controlHeld)
        {
            ClearSelection();
        }

        // Raycast to select units/buildings
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit[] hits = Physics.RaycastAll(ray);

        bool clickedOnUnit = false;
        foreach (RaycastHit hit in hits)
        {
            Unit unit = hit.collider.GetComponent<Unit>();
            if (unit != null && unit.team == Team.Player && unit.isAlive)
            {
                clickedOnUnit = true;
                SelectUnit(unit);
                break;
            }
        }

        // If didn't click on unit and not shift/ctrl, select units in box
        if (!clickedOnUnit && !shiftHeld)
        {
            SelectUnitsInBox();
        }
    }

    void SelectUnitsInBox()
    {
        Vector3 worldStart = Camera.main.ScreenToWorldPoint(selectionStart);
        Vector3 worldEnd = Camera.main.ScreenToWorldPoint(selectionEnd);

        float minX = Mathf.Min(worldStart.x, worldEnd.x);
        float maxX = Mathf.Max(worldStart.x, worldEnd.x);
        float minZ = Mathf.Min(worldStart.z, worldEnd.z);
        float maxZ = Mathf.Max(worldStart.z, worldEnd.z);

        Unit[] allUnits = FindObjectsOfType<Unit>();
        foreach (Unit unit in allUnits)
        {
            if (unit.team == Team.Player && unit.isAlive)
            {
                Vector3 pos = unit.transform.position;
                if (pos.x >= minX && pos.x <= maxX && pos.z >= minZ && pos.z <= maxZ)
                {
                    if (selectedUnits.Count < maxSelection)
                    {
                        SelectUnit(unit);
                    }
                }
            }
        }
    }

    public void SelectUnit(Unit unit)
    {
        if (unit == null) return;

        if (selectedUnits.Count >= maxSelection && !selectedUnits.Contains(unit))
        {
            return; // Max selection reached
        }

        if (!selectedUnits.Contains(unit))
        {
            selectedUnits.Add(unit);
            unit.SetSelected(true);
        }
    }

    public void DeselectUnit(Unit unit)
    {
        if (unit == null) return;

        if (selectedUnits.Contains(unit))
        {
            selectedUnits.Remove(unit);
            unit.SetSelected(false);
        }
    }

    public void SelectBuilding(Building building)
    {
        ClearSelection();

        if (selectedBuildings.Count >= maxSelection) return;

        selectedBuildings.Add(building);
        building.SetSelected(true);
    }

    public void DeselectBuilding(Building building)
    {
        if (building == null) return;

        if (selectedBuildings.Contains(building))
        {
            selectedBuildings.Remove(building);
            building.SetSelected(false);
        }
    }

    public void ClearSelection()
    {
        foreach (Unit unit in selectedUnits)
        {
            if (unit != null)
            {
                unit.SetSelected(false);
            }
        }
        selectedUnits.Clear();

        foreach (Building building in selectedBuildings)
        {
            if (building != null)
            {
                building.SetSelected(false);
            }
        }
        selectedBuildings.Clear();
    }

    void ExecuteRightClickCommand()
    {
        if (selectedUnits.Count == 0) return;

        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, 1000f))
        {
            Vector3 targetPos = hit.point;

            // Check what we clicked on
            Unit targetUnit = hit.collider.GetComponent<Unit>();
            Building targetBuilding = hit.collider.GetComponent<Building>();
            ResourceNode resourceNode = hit.collider.GetComponent<ResourceNode>();

            if (targetUnit != null)
            {
                if (targetUnit.team == Team.Enemy)
                {
                    // Attack
                    foreach (Unit unit in selectedUnits)
                    {
                        if (unit != null)
                        {
                            unit.AttackUnit(targetUnit);
                        }
                    }
                }
                else if (targetUnit.team == Team.Neutral)
                {
                    // Move to neutral
                    MoveUnitsTo(targetPos);
                }
                else
                {
                    // Move to ally
                    MoveUnitsTo(targetPos);
                }
            }
            else if (targetBuilding != null)
            {
                if (targetBuilding.team == Team.Player)
                {
                    // Set rally point or move to
                    if (Input.GetKey(KeyCode.LeftShift))
                    {
                        // Set rally point for buildings
                        foreach (Building b in selectedBuildings)
                        {
                            b.SetRallyPoint(targetPos);
                        }
                    }
                    else
                    {
                        MoveUnitsTo(targetPos);
                    }
                }
                else if (targetBuilding.team == Team.Enemy)
                {
                    // Attack building
                    foreach (Unit unit in selectedUnits)
                    {
                        unit.AttackUnit(null); // Will attack the building when in range
                    }
                }
            }
            else if (resourceNode != null)
            {
                // Gather resources
                foreach (Unit unit in selectedUnits)
                {
                    if (unit.unitType == UnitType.Worker)
                    {
                        // Assign worker to resource
                        WorkerAI worker = unit.GetComponent<WorkerAI>();
                        if (worker != null)
                        {
                            worker.AssignToResource(resourceNode);
                        }
                    }
                }
            }
            else
            {
                // Move to position
                MoveUnitsTo(targetPos);
            }
        }
    }

    void MoveUnitsTo(Vector3 position)
    {
        // Formation movement
        Vector3 offset = Vector3.zero;
        int unitsPerRow = Mathf.CeilToInt(Mathf.Sqrt(selectedUnits.Count));

        int count = 0;
        foreach (Unit unit in selectedUnits)
        {
            if (unit == null) continue;

            int row = count / unitsPerRow;
            int col = count % unitsPerRow;

            offset = new Vector3(col * 1.5f, 0, row * 1.5f);
            Vector3 targetPos = position + offset - new Vector3(unitsPerRow * 0.75f, 0, unitsPerRow * 0.75f);

            unit.MoveTo(targetPos);

            // Attack-move (will attack enemies on the way)
            // unit.AttackMoveTo(position);

            count++;
        }
    }

    public List<Unit> GetSelectedUnitsOfType(UnitType type)
    {
        return selectedUnits.FindAll(u => u.unitType == type);
    }

    public bool HasUnitOfType(UnitType type)
    {
        return selectedUnits.Exists(u => u.unitType == type);
    }
}
