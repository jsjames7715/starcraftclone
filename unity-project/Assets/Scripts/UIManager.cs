using UnityEngine;
using UnityEngine.UI;
using TMPro;
using System.Collections.Generic;

public class UIManager : MonoBehaviour
{
    public static UIManager Instance { get; private set; }

    [Header("Resource Display")]
    public TextMeshProUGUI mineralText;
    public TextMeshProUGUI gasText;
    public TextMeshProUGUI supplyText;
    public Image mineralIcon;
    public Image gasIcon;
    public Image supplyIcon;

    [Header("Game Info")]
    public TextMeshProUGUI timeText;
    public TextMeshProUGUI waveText;
    public TextMeshProUGUI waveTimerText;

    [Header("Selection Panel")]
    public GameObject selectionPanel;
    public Image unitPortrait;
    public TextMeshProUGUI unitNameText;
    public TextMeshProUGUI unitHealthText;
    public Image healthBarFill;
    public TextMeshProUGUI unitEnergyText;
    public Image energyBarFill;
    public GridLayoutGroup abilityGrid;
    public List<Image> abilityIcons = new List<Image>();
    public List<TextMeshProUGUI> hotkeyTexts = new List<TextMeshProUGUI>();

    [Header("Minimap")]
    public RawImage minimapImage;
    public Camera minimapCamera;
    public RectTransform minimapPanel;
    public Image[] unitIcons; // Icons on minimap

    [Header("Alert Panel")]
    public GameObject alertPanel;
    public TextMeshProUGUI alertText;
    public Image alertIcon;

    [Header("Production Panel")]
    public GameObject productionPanel;
    public Image productionIcon;
    public TextMeshProUGUI productionProgress;
    public Slider productionSlider;
    public TextMeshProUGUI queueText;

    [Header("Build Panel")]
    public GameObject buildPanel;
    public GridLayoutGroup buildGrid;

    [Header("Chat Panel")]
    public GameObject chatPanel;
    public TextMeshProUGUI chatText;
    public InputField chatInput;

    [Header("Pause Menu")]
    public GameObject pauseMenu;
    public GameObject gameOverPanel;
    public GameObject victoryPanel;

    [Header("Settings")]
    public float alertDuration = 3f;
    private float alertTimer = 0f;

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
        if (pauseMenu != null) pauseMenu.SetActive(false);
        if (gameOverPanel != null) gameOverPanel.SetActive(false);
        if (victoryPanel != null) victoryPanel.SetActive(false);
        if (alertPanel != null) alertPanel.SetActive(false);
    }

    void Update()
    {
        UpdateResourceDisplay();
        UpdateGameInfo();
        UpdateSelectionInfo();
        UpdateAlertTimer();
    }

    void UpdateResourceDisplay()
    {
        if (GameManager.Instance == null) return;

        if (mineralText != null)
            mineralText.text = GameManager.Instance.minerals.ToString();
        if (gasText != null)
            gasText.text = GameManager.Instance.gas.ToString();
        if (supplyText != null)
            supplyText.text = $"{GameManager.Instance.supplyUsed}/{GameManager.Instance.supplyMax}";
    }

    void UpdateGameInfo()
    {
        if (GameManager.Instance == null) return;

        float minutes = Mathf.Floor(GameManager.Instance.gameTime / 60f);
        float seconds = Mathf.Floor(GameManager.Instance.gameTime % 60f);
        
        if (timeText != null)
            timeText.text = $"{minutes:00}:{seconds:00}";
        
        if (waveText != null)
            waveText.text = $"Wave {GameManager.Instance.waveNumber}";
        
        if (waveTimerText != null)
            waveTimerText.text = $"Next: {GameManager.Instance.waveTimer:F0}s";
    }

    void UpdateSelectionInfo()
    {
        if (SelectionManager.Instance == null) return;

        bool hasSelection = SelectionManager.Instance.selectedUnits.Count > 0 || 
                           SelectionManager.Instance.selectedBuildings.Count > 0;

        if (selectionPanel != null)
            selectionPanel.SetActive(hasSelection);

        if (hasSelection)
        {
            // Update with first selected unit
            if (SelectionManager.Instance.selectedUnits.Count > 0)
            {
                Unit unit = SelectionManager.Instance.selectedUnits[0];
                UpdateUnitInfo(unit);
            }
            else if (SelectionManager.Instance.selectedBuildings.Count > 0)
            {
                Building building = SelectionManager.Instance.selectedBuildings[0];
                UpdateBuildingInfo(building);
            }
        }
    }

    void UpdateUnitInfo(Unit unit)
    {
        if (unit == null) return;

        if (unitNameText != null)
            unitNameText.text = unit.unitName;
        
        if (unitHealthText != null)
            unitHealthText.text = $"{unit.currentHealth}/{unit.maxHealth}";
        
        if (healthBarFill != null)
            healthBarFill.fillAmount = (float)unit.currentHealth / unit.maxHealth;
    }

    void UpdateBuildingInfo(Building building)
    {
        if (building == null) return;

        if (unitNameText != null)
            unitNameText.text = building.buildingName;
        
        if (unitHealthText != null)
            unitHealthText.text = $"{building.currentHealth}/{building.maxHealth}";
        
        if (healthBarFill != null)
            healthBarFill.fillAmount = (float)building.currentHealth / building.maxHealth;

        // Production panel
        if (productionPanel != null)
            productionPanel.SetActive(building.isProducing);

        if (building.isProducing && productionSlider != null)
        {
            // Calculate progress
            float progress = 1f - (building.productionQueueTime / 60f);
            productionSlider.value = progress;
        }
    }

    public void ShowAlert(string message, Sprite icon = null)
    {
        if (alertPanel != null)
        {
            alertPanel.SetActive(true);
            if (alertText != null)
                alertText.text = message;
            if (alertIcon != null && icon != null)
                alertIcon.sprite = icon;
        }
        alertTimer = alertDuration;
    }

    void UpdateAlertTimer()
    {
        if (alertTimer > 0)
        {
            alertTimer -= Time.deltaTime;
            if (alertTimer <= 0 && alertPanel != null)
            {
                alertPanel.SetActive(false);
            }
        }
    }

    public void ShowPauseMenu()
    {
        if (pauseMenu != null)
            pauseMenu.SetActive(true);
        Time.timeScale = 0f;
    }

    public void HidePauseMenu()
    {
        if (pauseMenu != null)
            pauseMenu.SetActive(false);
        Time.timeScale = 1f;
    }

    public void ShowGameOver()
    {
        if (gameOverPanel != null)
            gameOverPanel.SetActive(true);
    }

    public void ShowVictory()
    {
        if (victoryPanel != null)
            victoryPanel.SetActive(true);
    }

    public void AddChatMessage(string sender, string message)
    {
        if (chatText != null)
        {
            chatText.text += $"\n[{sender}]: {message}";
        }
    }

    public void UpdateMinimap()
    {
        if (minimapCamera == null || minimapImage == null) return;

        // Render minimap from top-down camera
        RenderTexture currentRT = RenderTexture.active;
        RenderTexture.active = minimapCamera.targetTexture;
        minimapCamera.Render();
    }
}
