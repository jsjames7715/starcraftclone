using UnityEngine;
using System.Collections.Generic;
using TMPro;

public class GameManager : MonoBehaviour
{
    public static GameManager Instance { get; private set; }

    [Header("Resources")]
    public int minerals = 500;
    public int gas = 0;
    public int supplyUsed = 0;
    public int supplyMax = 20;

    [Header("Game State")]
    public float gameTime = 0f;
    public int waveNumber = 0;
    public float waveTimer = 60f;
    public bool isPaused = false;
    public bool isGameOver = false;
    public bool isVictory = false;

    [Header("Settings")]
    public float gameSpeed = 1f;
    public int startingMinerals = 500;

    void Awake()
    {
        if (Instance != null && Instance != this)
        {
            Destroy(gameObject);
            return;
        }
        Instance = this;
    }

    void Start()
    {
        ResetGame();
    }

    void Update()
    {
        if (isPaused || isGameOver || isVictory) return;

        gameTime += Time.deltaTime * gameSpeed;

        // Wave timer
        waveTimer -= Time.deltaTime * gameSpeed;
        if (waveTimer <= 0)
        {
            SpawnWave();
            waveTimer = 45f;
        }

        // Check win/lose conditions
        CheckGameConditions();
    }

    public void ResetGame()
    {
        minerals = startingMinerals;
        gas = 0;
        supplyUsed = 0;
        supplyMax = 20;
        gameTime = 0f;
        waveNumber = 0;
        waveTimer = 60f;
        isPaused = false;
        isGameOver = false;
        isVictory = false;
    }

    public bool SpendMinerals(int amount)
    {
        if (minerals >= amount)
        {
            minerals -= amount;
            return true;
        }
        return false;
    }

    public bool SpendGas(int amount)
    {
        if (gas >= amount)
        {
            gas -= amount;
            return true;
        }
        return false;
    }

    public bool HasResources(int mineralsRequired, int gasRequired = 0)
    {
        return minerals >= mineralsRequired && gas >= gasRequired;
    }

    public void AddMinerals(int amount)
    {
        minerals += amount;
    }

    public void AddGas(int amount)
    {
        gas += amount;
    }

    public void SpawnWave()
    {
        waveNumber++;
        WaveSpawner.Instance.SpawnWave(waveNumber);
    }

    void CheckGameConditions()
    {
        // Check if player base is destroyed
        bool playerBaseAlive = false;
        Building[] buildings = FindObjectsOfType<Building>();
        foreach (Building b in buildings)
        {
            if (b.buildingType == BuildingType.CommandCenter && b.team == Team.Player && b.isAlive)
            {
                playerBaseAlive = true;
                break;
            }
        }

        if (!playerBaseAlive)
        {
            isGameOver = true;
        }
    }

    public void TogglePause()
    {
        isPaused = !isPaused;
        Time.timeScale = isPaused ? 0f : gameSpeed;
    }
}
