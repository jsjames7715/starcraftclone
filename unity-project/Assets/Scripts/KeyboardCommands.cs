using UnityEngine;

public class KeyboardCommands : MonoBehaviour
{
    [Header("Command Keys")]
    public KeyCode stopKey = KeyCode.S;
    public KeyCode attackMoveKey = KeyCode.A;
    public KeyCode patrolKey = KeyCode.P;
    public KeyCode holdPositionKey = KeyCode.H;
    public KeyCode buildKey = KeyCode.B;
    public KeyCode cancelKey = KeyCode.Escape;

    void Update()
    {
        if (GameManager.Instance.isPaused) return;

        // Unit commands
        if (Input.GetKeyDown(stopKey))
        {
            StopSelected();
        }

        if (Input.GetKeyDown(attackMoveKey))
        {
            AttackMove();
        }

        if (Input.GetKeyDown(patrolKey))
        {
            EnterPatrolMode();
        }

        if (Input.GetKeyDown(holdPositionKey))
        {
            HoldPositionSelected();
        }

        // Build shortcuts
        if (Input.GetKeyDown(KeyCode.C))
        {
            BuildManager.Instance.EnterBuildMode(BuildingType.CommandCenter);
        }

        if (Input.GetKeyDown(KeyCode.Q))
        {
            // Queue selected building
            QueueSelectedBuilding(UnitType.Marine);
        }

        // Cancel / back
        if (Input.GetKeyDown(cancelKey))
        {
            if (BuildManager.Instance.isInBuildMode)
            {
                BuildManager.Instance.ExitBuildMode();
            }
            else
            {
                SelectionManager.Instance.ClearSelection();
            }
        }

        // Quick select
        if (Input.GetKeyDown(KeyCode.Tab))
        {
            SelectAllCombatUnits();
        }

        // Number keys for quick unit selection
        for (int i = 1; i <= 9; i++)
        {
            if (Input.GetKeyDown(KeyCode.Alpha1 + i - 1))
            {
                SelectUnitByIndex(i - 1);
            }
        }
    }

    void StopSelected()
    {
        if (SelectionManager.Instance == null) return;

        foreach (Unit unit in SelectionManager.Instance.selectedUnits)
        {
            if (unit != null)
            {
                unit.Stop();
            }
        }
    }

    void HoldPositionSelected()
    {
        if (SelectionManager.Instance == null) return;

        foreach (Unit unit in SelectionManager.Instance.selectedUnits)
        {
            if (unit != null)
            {
                unit.HoldPosition();
            }
        }
    }

    void AttackMove()
    {
        if (SelectionManager.Instance == null) return;

        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, 1000f))
        {
            foreach (Unit unit in SelectionManager.Instance.selectedUnits)
            {
                if (unit != null)
                {
                    unit.AttackMoveTo(hit.point);
                }
            }
        }
    }

    void EnterPatrolMode()
    {
        if (SelectionManager.Instance == null) return;

        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
        RaycastHit hit;

        if (Physics.Raycast(ray, out hit, 1000f))
        {
            foreach (Unit unit in SelectionManager.Instance.selectedUnits)
            {
                if (unit != null)
                {
                    unit.Patrol(new System.Collections.Generic.List<Vector3> { hit.point });
                }
            }
        }
    }

    void QueueSelectedBuilding(UnitType unitType)
    {
        foreach (Building building in SelectionManager.Instance.selectedBuildings)
        {
            if (building != null && building.CanProduce(unitType))
            {
                building.QueueUnit(unitType);
            }
        }
    }

    void SelectAllCombatUnits()
    {
        SelectionManager.Instance.ClearSelection();

        Unit[] units = FindObjectsOfType<Unit>();
        foreach (Unit unit in units)
        {
            if (unit.team == Team.Player && unit.isAlive)
            {
                if (unit.unitType != UnitType.Worker)
                {
                    SelectionManager.Instance.SelectUnit(unit);
                }
            }
        }
    }

    void SelectUnitByIndex(int index)
    {
        Unit[] units = FindObjectsOfType<Unit>();
        int count = 0;

        foreach (Unit unit in units)
        {
            if (unit.team == Team.Player && unit.isAlive)
            {
                if (count == index)
                {
                    SelectionManager.Instance.ClearSelection();
                    SelectionManager.Instance.SelectUnit(unit);
                    CameraController.Instance.FocusOnUnit(unit.transform);
                    break;
                }
                count++;
            }
        }
    }
}
