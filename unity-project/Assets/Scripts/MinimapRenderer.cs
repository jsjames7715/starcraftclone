using UnityEngine;
using System.Collections.Generic;

public class MinimapRenderer : MonoBehaviour
{
    public Camera minimapCamera;
    public RenderTexture minimapTexture;
    public RawImage minimapDisplay;
    public RectTransform minimapRect;
    public Material playerUnitMaterial;
    public Material enemyUnitMaterial;
    public Material buildingMaterial;

    [Header("Settings")]
    public float mapScale = 0.05f;
    public Color playerColor = Color.blue;
    public Color enemyColor = Color.red;
    public Color neutralColor = Color.yellow;
    public Color resourceColor = Color.cyan;

    [Header("Fog of War")]
    public bool showFogOfWar = true;
    public Color unexploredColor = Color.gray;

    private List<UnitIcon> unitIcons = new List<UnitIcon>();
    private List<BuildingIcon> buildingIcons = new List<BuildingIcon>();
    private Transform mapCenter;

    void Start()
    {
        if (minimapCamera != null)
        {
            minimapCamera.targetTexture = minimapTexture;
        }
    }

    void LateUpdate()
    {
        UpdateMinimap();
        UpdateCamera();
    }

    void UpdateMinimap()
    {
        if (minimapRect == null) return;

        // Get camera bounds
        Vector3 cameraPos = Camera.main.transform.position;
        float cameraHeight = Camera.main.transform.position.y;
        float cameraWidth = cameraHeight * Camera.main.aspect;

        // Calculate minimap position
        float mapX = (cameraPos.x / 200f) * minimapRect.rect.width + minimapRect.rect.width / 2f;
        float mapY = (cameraPos.z / 200f) * minimapRect.rect.height + minimapRect.rect.height / 2f;

        // Update camera
        if (minimapCamera != null)
        {
            Vector3 minimapCamPos = minimapCamera.transform.position;
            minimapCamPos.x = cameraPos.x;
            minimapCamPos.z = cameraPos.z;
            minimapCamera.transform.position = minimapCamPos;
        }
    }

    void UpdateCamera()
    {
        if (minimapDisplay == null || !Input.GetMouseButtonDown(1)) return;

        // Click on minimap to move camera
        Vector2 localPoint;
        RectTransformUtility.ScreenPointToLocalPointInRectangle(
            minimapRect, 
            Input.mousePosition, 
            null, 
            out localPoint
        );

        // Convert to world position
        float worldX = (localPoint.x / minimapRect.rect.width) * 200f;
        float worldZ = (localPoint.y / minimapRect.rect.height) * 200f;

        CameraController.Instance.CenterOnPosition(new Vector3(worldX, 0, worldZ));
    }

    public void RefreshUnits()
    {
        // Clear old icons
        foreach (UnitIcon icon in unitIcons)
        {
            if (icon.go != null) Destroy(icon.go);
        }
        unitIcons.Clear();

        // Create new icons
        Unit[] units = FindObjectsOfType<Unit>();
        foreach (Unit unit in units)
        {
            if (!unit.isAlive) continue;
            CreateUnitIcon(unit);
        }
    }

    void CreateUnitIcon(Unit unit)
    {
        GameObject iconGO = new GameObject("MinimapIcon_" + unit.name);
        iconGO.transform.SetParent(transform);
        
        RectTransform rect = iconGO.AddComponent<RectTransform>();
        rect.sizeDelta = new Vector2(8, 8);

        Image img = iconGO.AddComponent<Image>();
        img.color = GetUnitColor(unit.team);

        UnitIcon icon = new UnitIcon { go = iconGO, unit = unit, image = img, rect = rect };
        unitIcons.Add(icon);
    }

    Color GetUnitColor(Team team)
    {
        switch (team)
        {
            case Team.Player: return playerColor;
            case Team.Enemy: return enemyColor;
            default: return neutralColor;
        }
    }

    void UpdateUnitPositions()
    {
        foreach (UnitIcon icon in unitIcons)
        {
            if (icon.unit == null || !icon.unit.isAlive)
            {
                if (icon.go != null) Destroy(icon.go);
                continue;
            }

            Vector3 pos = icon.unit.transform.position;
            float x = pos.x * mapScale;
            float y = pos.z * mapScale;

            icon.rect.anchoredPosition = new Vector2(x, y);
        }
    }
}

public class UnitIcon
{
    public GameObject go;
    public Unit unit;
    public Image image;
    public RectTransform rect;
}

public class BuildingIcon
{
    public GameObject go;
    public Building building;
    public Image image;
    public RectTransform rect;
}
