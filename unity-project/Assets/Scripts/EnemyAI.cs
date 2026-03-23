using UnityEngine;
using System.Collections.Generic;

public class EnemyAI : MonoBehaviour
{
    [Header("AI Settings")]
    public float thinkInterval = 1f;
    public float aggressionLevel = 1f;
    public float defendRadius = 20f;

    [Header("Economy")]
    public int workersPerBase = 4;
    public int maxWorkers = 24;
    public float expandThreshold = 0.8f;

    [Header("Combat")]
    public float attackThreshold = 0.7f;
    public float retreatThreshold = 0.2f;

    private float thinkTimer = 0f;
    private List<Unit> combatUnits = new List<Unit>();
    private List<Building> enemyBuildings = new List<Building>();
    private Building mainBase;
    private int currentState = 0; // 0=eco, 1=expand, 2=attack, 3=defend

    void Start()
    {
        InvokeRepeating("FindMainBase", 1f, 5f);
    }

    void Update()
    {
        thinkTimer += Time.deltaTime;
        if (thinkTimer >= thinkInterval)
        {
            thinkTimer = 0f;
            Think();
        }
    }

    void FindMainBase()
    {
        Building[] buildings = FindObjectsOfType<Building>();
        foreach (Building b in buildings)
        {
            if (b.team == Team.Enemy && b.buildingType == BuildingType.CommandCenter)
            {
                mainBase = b;
                break;
            }
        }
    }

    void Think()
    {
        if (mainBase == null || !mainBase.isAlive) return;

        // Update unit list
        Unit[] allUnits = FindObjectsOfType<Unit>();
        combatUnits.Clear();
        enemyBuildings.Clear();

        foreach (Unit u in allUnits)
        {
            if (u.team == Team.Enemy && u.isAlive)
            {
                enemyBuildings.Add(u.GetComponent<Building>());
            }
            else if (u.team == Team.Enemy && u.isAlive && IsCombatUnit(u))
            {
                combatUnits.Add(u);
            }
        }

        // Check for threats
        bool underAttack = IsUnderAttack();
        float ecoStrength = GetEconomicStrength();
        float militaryStrength = GetMilitaryStrength();

        // Decide state
        if (underAttack)
        {
            currentState = 3; // Defend
        }
        else if (ecoStrength < expandThreshold)
        {
            currentState = 1; // Expand
        }
        else if (militaryStrength > attackThreshold)
        {
            currentState = 2; // Attack
        }
        else
        {
            currentState = 0; // Economy
        }

        // Execute state
        switch (currentState)
        {
            case 0: BuildEconomy(); break;
            case 1: Expand(); break;
            case 2: Attack(); break;
            case 3: Defend(); break;
        }
    }

    bool IsCombatUnit(Unit unit)
    {
        return unit.unitType != UnitType.Worker && 
               unit.unitType != UnitType.Probe;
    }

    bool IsUnderAttack()
    {
        foreach (Building b in enemyBuildings)
        {
            if (b != null && b.currentHealth < b.maxHealth * 0.8f)
            {
                return true;
            }
        }
        return false;
    }

    float GetEconomicStrength()
    {
        // Compare our workers to optimal
        int workerCount = 0;
        foreach (Unit u in combatUnits)
        {
            if (u.unitType == UnitType.Worker) workerCount++;
        }
        return (float)workerCount / (workersPerBase * 2);
    }

    float GetMilitaryStrength()
    {
        float totalPower = 0f;
        foreach (Unit u in combatUnits)
        {
            if (IsCombatUnit(u))
            {
                totalPower += u.attackDamage * u.currentHealth / u.maxHealth;
            }
        }
        return Mathf.Clamp01(totalPower / (1000f * aggressionLevel));
    }

    void BuildEconomy()
    {
        // Queue workers if needed
        if (mainBase != null && mainBase.CanProduce(UnitType.Worker))
        {
            // Check if we have enough workers
            int workerCount = 0;
            foreach (Unit u in combatUnits)
            {
                if (u.unitType == UnitType.Worker) workerCount++;
            }

            if (workerCount < maxWorkers)
            {
                // Workers auto-produced for now
            }
        }

        // Build supply if needed
        int supplyUsed = GameManager.Instance.supplyUsed;
        int supplyMax = GameManager.Instance.supplyMax;
        if (supplyMax - supplyUsed < 10)
        {
            // Queue supply depot
        }
    }

    void Expand()
    {
        // Find expansion location
        // Build new base
        // Defend expansion
    }

    void Attack()
    {
        if (mainBase == null) return;

        // Gather all combat units
        Vector3 attackPos = mainBase.transform.position;

        foreach (Unit u in combatUnits)
        {
            if (!IsCombatUnit(u)) continue;
            if (u.isAttacking) continue;

            // Move toward player base
            Building[] playerBuildings = FindObjectsOfType<Building>();
            Building target = null;
            float nearestDist = float.MaxValue;

            foreach (Building b in playerBuildings)
            {
                if (b.team == Team.Player && b.isAlive)
                {
                    float dist = Vector3.Distance(u.transform.position, b.transform.position);
                    if (dist < nearestDist)
                    {
                        nearestDist = dist;
                        target = b;
                    }
                }
            }

            if (target != null)
            {
                u.targetPosition = target.transform.position;
                u.isMoving = true;
                u.isAttacking = true;
            }
        }
    }

    void Defend()
    {
        if (mainBase == null) return;

        // Find threats
        Unit[] playerUnits = FindObjectsOfType<Unit>();
        Unit nearestThreat = null;
        float nearestDist = float.MaxValue;

        foreach (Unit u in playerUnits)
        {
            if (u.team == Team.Player && u.isAlive)
            {
                float dist = Vector3.Distance(mainBase.transform.position, u.transform.position);
                if (dist < defendRadius && dist < nearestDist)
                {
                    nearestDist = dist;
                    nearestThreat = u;
                }
            }
        }

        // Move combat units to defend
        if (nearestThreat != null)
        {
            foreach (Unit u in combatUnits)
            {
                if (!IsCombatUnit(u)) continue;
                u.AttackUnit(nearestThreat);
            }
        }
        else
        {
            // Return to base
            foreach (Unit u in combatUnits)
            {
                if (!IsCombatUnit(u)) continue;
                u.MoveTo(mainBase.transform.position);
            }
        }
    }
}
