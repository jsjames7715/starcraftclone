using UnityEngine;

public class WaveSpawner : MonoBehaviour
{
    public static WaveSpawner Instance { get; private set; }

    [Header("Spawn Settings")]
    public Transform spawnPoint;
    public int baseEnemyCount = 5;
    public float spawnRadius = 10f;

    [Header("Wave Composition")]
    public int baseWaveSize = 3;
    public float waveScaling = 1.5f;

    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
    }

    public void SpawnWave(int waveNumber)
    {
        int enemyCount = Mathf.CeilToInt(baseWaveSize * Mathf.Pow(waveScaling, waveNumber - 1));

        for (int i = 0; i < enemyCount; i++)
        {
            SpawnEnemy(waveNumber);
        }

        // Show alert
        UIManager.Instance?.ShowAlert($"Wave {waveNumber} incoming! {enemyCount} enemies!");

        // Occasionally spawn elite
        if (waveNumber % 5 == 0)
        {
            SpawnElite(waveNumber);
        }

        // Boss every 10 waves
        if (waveNumber % 10 == 0)
        {
            SpawnBoss(waveNumber);
        }
    }

    void SpawnEnemy(int waveNumber)
    {
        Vector3 spawnPos = GetSpawnPosition();

        // Determine enemy type based on wave
        UnitType enemyType = GetEnemyType(waveNumber);

        Unit enemy = Unit.CreateUnit(enemyType, spawnPos, Team.Enemy);
        
        // Scale enemy stats
        float healthMultiplier = 1f + (waveNumber - 1) * 0.1f;
        enemy.maxHealth = Mathf.CeilToInt(enemy.maxHealth * healthMultiplier);
        enemy.currentHealth = enemy.maxHealth;
        enemy.attackDamage = Mathf.CeilToInt(enemy.attackDamage * healthMultiplier);
    }

    UnitType GetEnemyType(int waveNumber)
    {
        if (waveNumber < 3)
        {
            return UnitType.Marine;
        }
        else if (waveNumber < 5)
        {
            return Random.value > 0.5f ? UnitType.Marine : UnitType.Zergling;
        }
        else if (waveNumber < 8)
        {
            int r = Random.Range(0, 3);
            if (r == 0) return UnitType.Zergling;
            if (r == 1) return UnitType.Marine;
            return UnitType.Tank;
        }
        else if (waveNumber < 12)
        {
            int r = Random.Range(0, 4);
            if (r == 0) return UnitType.Zergling;
            if (r == 1) return UnitType.Marine;
            if (r == 2) return UnitType.Tank;
            return UnitType.Ghost;
        }
        else
        {
            int r = Random.Range(0, 6);
            switch (r)
            {
                case 0: return UnitType.Zergling;
                case 1: return UnitType.Marine;
                case 2: return UnitType.Tank;
                case 3: return UnitType.Ghost;
                case 4: return UnitType.Battlecruiser;
                case 5: return UnitType.Ultralisk;
                default: return UnitType.Marine;
            }
        }
    }

    void SpawnElite(int waveNumber)
    {
        Vector3 spawnPos = GetSpawnPosition();

        // Spawn elite version of random unit
        UnitType[] eliteTypes = { UnitType.Marine, UnitType.Tank, UnitType.Ghost, UnitType.Battlecruiser };
        UnitType eliteType = eliteTypes[Random.Range(0, eliteTypes.Length)];

        Unit elite = Unit.CreateUnit(eliteType, spawnPos, Team.Enemy);

        // Double stats
        elite.maxHealth *= 2;
        elite.currentHealth = elite.maxHealth;
        elite.attackDamage *= 2;

        UIManager.Instance?.ShowAlert("Elite enemy spawned!");
    }

    void SpawnBoss(int waveNumber)
    {
        Vector3 spawnPos = GetSpawnPosition();

        // Create boss
        GameObject bossPrefab = Resources.Load<GameObject>("Units/BossUnit");
        if (bossPrefab != null)
        {
            GameObject boss = Instantiate(bossPrefab, spawnPos, Quaternion.identity);
            Unit bossUnit = boss.GetComponent<Unit>();

            if (bossUnit != null)
            {
                bossUnit.maxHealth = 1000 + waveNumber * 200;
                bossUnit.currentHealth = bossUnit.maxHealth;
                bossUnit.attackDamage = 50 + waveNumber * 10;
                bossUnit.attackRange = 10f;
                bossUnit.sightRange = 15f;

                UIManager.Instance?.ShowAlert($"BOSS: {bossUnit.unitName} has appeared!");
            }
        }
    }

    Vector3 GetSpawnPosition()
    {
        // Find player base and spawn away from it
        Building[] buildings = FindObjectsOfType<Building>();
        Vector3 playerBase = Vector3.zero;

        foreach (Building b in buildings)
        {
            if (b.buildingType == BuildingType.CommandCenter && b.team == Team.Player)
            {
                playerBase = b.transform.position;
                break;
            }
        }

        // Random position around enemy spawn area
        float angle = Random.Range(0f, Mathf.PI * 2f);
        float distance = Random.Range(spawnRadius * 0.5f, spawnRadius);
        
        Vector3 spawnPos = playerBase + new Vector3(
            Mathf.Cos(angle) * distance,
            0,
            Mathf.Sin(angle) * distance
        );

        return spawnPos;
    }
}
