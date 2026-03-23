using UnityEngine;
using System.Collections.Generic;

public enum UnitType
{
    // Terran
    Worker, Marine, Medic, Tank, Fighter, Ghost, Vulture, Goliath,
    Wraith, Battlecruiser, ScienceVessel, Firebat, Dropship, Valkyrie,
    
    // Zerg
    Zergling, Ultralisk, Mutalisk, Hydralisk, ZerglingPack,
    Lurker, Defiler, Queen, Baneling, Roach, Infestor, SwarmHost,
    Broodling, Guardian, Devourer, Overlord, Overseer,
    
    // Protoss
    Probe, Zealot, Dragoon, DarkTemplar, Scout, Carrier, Reaver,
    Shuttle, Corsair, Arbiter, HighTemplar, Archon
}

public enum Team { Player, Enemy, Neutral }

public class Unit : MonoBehaviour
{
    [Header("Unit Info")]
    public UnitType unitType;
    public Team team;
    public string unitName;
    public Sprite unitIcon;

    [Header("Stats")]
    public int maxHealth = 100;
    public int currentHealth = 100;
    public int attackDamage = 10;
    public int armor = 0;
    public float attackRange = 5f;
    public float attackSpeed = 1f;
    public float moveSpeed = 3f;
    public float sightRange = 8f;

    [Header("Cost")]
    public int mineralCost = 100;
    public int gasCost = 0;
    public int supplyCost = 1;

    [Header("State")]
    public bool isAlive = true;
    public bool isSelected = false;
    public bool isMoving = false;
    public bool isAttacking = false;
    public bool isPatrolling = false;
    public bool isHoldingPosition = false;

    [Header("Combat")]
    public float attackCooldown = 0f;
    public Unit attackTarget;
    public Transform targetTransform;
    public Vector3 targetPosition;
    public List<Vector3> patrolPoints = new List<Vector3>();
    public int currentPatrolIndex = 0;

    [Header("Components")]
    public Transform healthBarAnchor;
    public GameObject healthBarPrefab;
    private GameObject healthBarInstance;
    public GameObject selectionCircle;
    public GameObject attackRangeIndicator;

    [Header("Abilities")]
    public List<Ability> abilities = new List<Ability>();

    protected virtual void Start()
    {
        currentHealth = maxHealth;
        CreateHealthBar();
        CreateSelectionCircle();
    }

    protected virtual void Update()
    {
        if (!isAlive) return;

        // Attack cooldown
        if (attackCooldown > 0)
        {
            attackCooldown -= Time.deltaTime;
        }

        // Movement
        if (isMoving && targetPosition != null)
        {
            MoveToTarget();
        }

        // Combat
        if (isAttacking && attackTarget != null)
        {
            AttackTarget();
        }
    }

    void MoveToTarget()
    {
        Vector3 direction = targetPosition - transform.position;
        direction.y = 0;

        if (direction.magnitude < 0.5f)
        {
            isMoving = false;
            return;
        }

        direction.Normalize();
        Vector3 newPosition = transform.position + direction * moveSpeed * Time.deltaTime;
        
        // Check collision with other units/buildings
        if (!IsPositionBlocked(newPosition))
        {
            transform.position = newPosition;
            transform.rotation = Quaternion.LookRotation(direction);
        }
    }

    bool IsPositionBlocked(Vector3 pos)
    {
        Collider[] colliders = Physics.OverlapSphere(pos, 0.5f);
        foreach (Collider col in colliders)
        {
            if (col.GetComponent<Unit>() != null && col.GetComponent<Unit>() != this)
                return true;
            if (col.GetComponent<Building>() != null)
                return true;
        }
        return false;
    }

    void AttackTarget()
    {
        if (attackTarget == null || !attackTarget.isAlive)
        {
            isAttacking = false;
            attackTarget = null;
            return;
        }

        float distance = Vector3.Distance(transform.position, attackTarget.transform.position);
        
        if (distance <= attackRange)
        {
            // In range, attack
            if (attackCooldown <= 0)
            {
                PerformAttack();
                attackCooldown = 1f / attackSpeed;
            }
        }
        else
        {
            // Move towards target
            targetPosition = attackTarget.transform.position;
            isMoving = true;
        }
    }

    void PerformAttack()
    {
        if (attackTarget == null) return;

        // Create attack effect
        GameObject attackEffect = Resources.Load<GameObject>("Effects/AttackEffect");
        if (attackEffect != null)
        {
            Instantiate(attackEffect, attackTarget.transform.position, Quaternion.identity);
        }

        // Deal damage
        int damage = Mathf.Max(1, attackDamage - attackTarget.armor);
        attackTarget.TakeDamage(damage);
    }

    public void MoveTo(Vector3 position)
    {
        targetPosition = position;
        isMoving = true;
        isAttacking = false;
        attackTarget = null;
        isPatrolling = false;
    }

    public void AttackMoveTo(Vector3 position)
    {
        targetPosition = position;
        isMoving = true;
        isPatrolling = false;
    }

    public void AttackUnit(Unit target)
    {
        attackTarget = target;
        isAttacking = true;
        isMoving = false;
        isPatrolling = false;
    }

    public void Stop()
    {
        isMoving = false;
        isAttacking = false;
        isPatrolling = false;
        attackTarget = null;
        targetPosition = transform.position;
    }

    public void HoldPosition()
    {
        isHoldingPosition = true;
        Stop();
    }

    public void Patrol(List<Vector3> points)
    {
        patrolPoints = points;
        isPatrolling = true;
        currentPatrolIndex = 0;
        MoveTo(patrolPoints[0]);
    }

    public void TakeDamage(int damage)
    {
        if (!isAlive) return;

        currentHealth -= damage;

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
            Die();
        }
    }

    public void Heal(int amount)
    {
        if (!isAlive) return;
        currentHealth = Mathf.Min(maxHealth, currentHealth + amount);
    }

    public void Repair(int amount)
    {
        currentHealth = Mathf.Min(maxHealth, currentHealth + amount);
    }

    void Die()
    {
        isAlive = false;
        
        // Death effect
        GameObject deathEffect = Resources.Load<GameObject>("Effects/DeathEffect");
        if (deathEffect != null)
        {
            Instantiate(deathEffect, transform.position, Quaternion.identity);
        }

        // Remove from selection
        if (isSelected)
        {
            SelectionManager.Instance.DeselectUnit(this);
        }

        Destroy(gameObject, 0.5f);
    }

    void CreateHealthBar()
    {
        if (healthBarPrefab != null && healthBarAnchor != null)
        {
            healthBarInstance = Instantiate(healthBarPrefab, healthBarAnchor.position, Quaternion.identity, healthBarAnchor);
        }
    }

    void CreateSelectionCircle()
    {
        if (selectionCircle != null)
        {
            selectionCircle.SetActive(false);
        }
    }

    public void SetSelected(bool selected)
    {
        isSelected = selected;
        if (selectionCircle != null)
        {
            selectionCircle.SetActive(selected);
        }
    }

    void OnMouseDown()
    {
        if (team == Team.Player)
        {
            SelectionManager.Instance.SelectUnit(this);
        }
    }

    void OnMouseOver()
    {
        // Show tooltip or highlight
    }

    // Static methods for unit creation
    public static Unit CreateUnit(UnitType type, Vector3 position, Team team)
    {
        GameObject prefab = Resources.Load<GameObject>($"Units/{type}");
        if (prefab == null)
        {
            Debug.LogError($"Unit prefab not found: Units/{type}");
            return null;
        }

        GameObject unitObj = Instantiate(prefab, position, Quaternion.identity);
        Unit unit = unitObj.GetComponent<Unit>();
        
        if (unit != null)
        {
            unit.unitType = type;
            unit.team = team;
        }

        return unit;
    }
}

[System.Serializable]
public class Ability
{
    public string abilityName;
    public KeyCode hotkey;
    public float cooldown;
    public float currentCooldown;
    public float energyCost;
    public Sprite icon;
    public GameObject effectPrefab;

    public bool CanUse()
    {
        return currentCooldown <= 0;
    }

    public void Use(Unit caster, Vector3 target)
    {
        if (!CanUse()) return;
        
        currentCooldown = cooldown;
        
        if (effectPrefab != null)
        {
            Instantiate(effectPrefab, target, Quaternion.identity);
        }
    }
}
