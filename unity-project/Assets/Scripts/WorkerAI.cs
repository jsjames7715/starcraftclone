using UnityEngine;

public class ResourceNode : MonoBehaviour
{
    public enum ResourceType { Minerals, Gas }

    [Header("Resource Info")]
    public ResourceType resourceType;
    public int currentAmount = 1500;
    public int maxAmount = 1500;
    public float harvestRate = 5f;

    [Header("Visuals")]
    public GameObject depletedModel;
    public ParticleSystem harvestEffect;
    public Sprite resourceIcon;

    [Header("Depletion")]
    public bool isDepleted = false;
    public GameObject depletedIndicator;

    void Start()
    {
        if (depletedIndicator != null)
            depletedIndicator.SetActive(false);
    }

    public int Harvest(int amount)
    {
        if (isDepleted) return 0;

        int harvested = Mathf.Min(amount, currentAmount);
        currentAmount -= harvested;

        if (currentAmount <= 0)
        {
            Deplete();
        }

        return harvested;
    }

    void Deplete()
    {
        isDepleted = true;
        currentAmount = 0;

        if (depletedModel != null)
            depletedModel.SetActive(true);

        if (depletedIndicator != null)
            depletedIndicator.SetActive(true);

        gameObject.SetActive(false);
    }

    public float GetPercentageRemaining()
    {
        return (float)currentAmount / maxAmount;
    }
}

public class WorkerAI : MonoBehaviour
{
    public enum WorkerState { Idle, MovingToResource, Harvesting, Returning, Depositing }

    [Header("State")]
    public WorkerState currentState = WorkerState.Idle;
    public ResourceNode assignedResource;
    public Building targetBuilding;

    [Header("Settings")]
    public float harvestTime = 2f;
    public int carryAmount = 8;
    public int currentCarried = 0;

    [Header("References")]
    public Transform mineralDropPoint;

    private float harvestTimer = 0f;
    private Unit unit;

    void Start()
    {
        unit = GetComponent<Unit>();
    }

    void Update()
    {
        switch (currentState)
        {
            case WorkerState.Idle:
                FindResource();
                break;
            case WorkerState.MovingToResource:
                MoveToResource();
                break;
            case WorkerState.Harvesting:
                HarvestResource();
                break;
            case WorkerState.Returning:
                ReturnToBase();
                break;
            case WorkerState.Depositing:
                DepositResources();
                break;
        }
    }

    void FindResource()
    {
        // Find nearest resource node
        ResourceNode[] resources = FindObjectsOfType<ResourceNode>();
        ResourceNode nearest = null;
        float nearestDist = float.MaxValue;

        foreach (ResourceNode resource in resources)
        {
            if (resource.isDepleted) continue;
            
            float dist = Vector3.Distance(transform.position, resource.transform.position);
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearest = resource;
            }
        }

        if (nearest != null)
        {
            assignedResource = nearest;
            currentState = WorkerState.MovingToResource;
        }
    }

    void MoveToResource()
    {
        if (assignedResource == null || assignedResource.isDepleted)
        {
            currentState = WorkerState.Idle;
            return;
        }

        float distance = Vector3.Distance(transform.position, assignedResource.transform.position);
        
        if (distance <= 2f)
        {
            currentState = WorkerState.Harvesting;
            harvestTimer = 0f;
        }
        else
        {
            unit.MoveTo(assignedResource.transform.position);
        }
    }

    void HarvestResource()
    {
        if (assignedResource == null || assignedResource.isDepleted)
        {
            currentState = WorkerState.Idle;
            return;
        }

        harvestTimer += Time.deltaTime * GameManager.Instance.gameSpeed;

        if (harvestTimer >= harvestTime)
        {
            harvestTimer = 0f;
            int harvested = assignedResource.Harvest(carryAmount);
            currentCarried += harvested;

            if (currentCarried >= carryAmount || harvested == 0)
            {
                currentState = WorkerState.Returning;
            }
        }
    }

    void ReturnToBase()
    {
        // Find command center or depot
        if (targetBuilding == null)
        {
            Building[] buildings = FindObjectsOfType<Building>();
            foreach (Building b in buildings)
            {
                if (b.buildingType == BuildingType.CommandCenter && b.team == Team.Player)
                {
                    targetBuilding = b;
                    break;
                }
            }
        }

        if (targetBuilding != null)
        {
            float distance = Vector3.Distance(transform.position, targetBuilding.transform.position);
            
            if (distance <= 3f)
            {
                currentState = WorkerState.Depositing;
            }
            else
            {
                unit.MoveTo(targetBuilding.transform.position);
            }
        }
    }

    void DepositResources()
    {
        if (currentCarried > 0)
        {
            if (assignedResource.resourceType == ResourceNode.ResourceType.Minerals)
            {
                GameManager.Instance.AddMinerals(currentCarried);
            }
            else if (assignedResource.resourceType == ResourceNode.ResourceType.Gas)
            {
                GameManager.Instance.AddGas(currentCarried);
            }

            currentCarried = 0;
        }

        // Return to harvest
        currentState = WorkerState.MovingToResource;
    }

    public void AssignToResource(ResourceNode resource)
    {
        assignedResource = resource;
        currentState = WorkerState.MovingToResource;
    }

    public void StopHarvesting()
    {
        currentState = WorkerState.Idle;
        assignedResource = null;
        unit.Stop();
    }
}
