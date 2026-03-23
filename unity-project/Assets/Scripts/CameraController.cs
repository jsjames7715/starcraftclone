using UnityEngine;

public class CameraController : MonoBehaviour
{
    public static CameraController Instance { get; private set; }

    [Header("Movement")]
    public float moveSpeed = 50f;
    public float edgeScrollSpeed = 10f;
    public float edgeScrollMargin = 20f;
    public float rotationSpeed = 100f;
    public float zoomSpeed = 20f;
    public float minZoom = 10f;
    public float maxZoom = 50f;

    [Header("Bounds")]
    public float minX = -100f;
    public float maxX = 100f;
    public float minZ = -100f;
    public float maxZ = 100f;
    public float minY = 10f;
    public float maxY = 50f;

    [Header("Keyboard Controls")]
    public KeyCode moveUp = KeyCode.W;
    public KeyCode moveDown = KeyCode.S;
    public KeyCode moveLeft = KeyCode.A;
    public KeyCode moveRight = KeyCode.D;
    public KeyCode rotateLeft = KeyCode.Q;
    public KeyCode rotateRight = KeyCode.E;
    public KeyCode zoomIn = KeyCode.R;
    public KeyCode zoomOut = KeyCode.F;

    [Header("Smooth Movement")]
    public bool useSmoothMovement = true;
    public float smoothTime = 0.1f;
    private Vector3 velocity;

    private float currentRotation = 0f;
    private float currentZoom = 30f;

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
        currentZoom = Camera.main.transform.position.y;
        currentRotation = Camera.main.transform.eulerAngles.y;
    }

    void Update()
    {
        HandleKeyboardInput();
        HandleMouseInput();
        HandleEdgeScrolling();
        HandleZoom();
        HandleRotation();
        ApplyMovement();
    }

    void HandleKeyboardInput()
    {
        Vector3 moveDirection = Vector3.zero;

        if (Input.GetKey(moveUp))
        {
            moveDirection += GetForwardDirection() * moveSpeed * Time.deltaTime;
        }
        if (Input.GetKey(moveDown))
        {
            moveDirection -= GetForwardDirection() * moveSpeed * Time.deltaTime;
        }
        if (Input.GetKey(moveLeft))
        {
            moveDirection -= GetRightDirection() * moveSpeed * Time.deltaTime;
        }
        if (Input.GetKey(moveRight))
        {
            moveDirection += GetRightDirection() * moveSpeed * Time.deltaTime;
        }

        if (useSmoothMovement)
        {
            transform.position += moveDirection * Time.deltaTime;
        }
        else
        {
            transform.position = Vector3.SmoothDamp(transform.position, transform.position + moveDirection, ref velocity, smoothTime);
        }
    }

    void HandleMouseInput()
    {
        // Middle mouse button rotation
        if (Input.GetMouseButton(2))
        {
            float rotationInput = Input.GetAxis("Mouse X") * rotationSpeed * Time.deltaTime;
            currentRotation -= rotationInput;
        }
    }

    void HandleEdgeScrolling()
    {
        Vector3 moveDirection = Vector3.zero;
        Vector2 mousePos = Input.mousePosition;

        // Left edge
        if (mousePos.x < edgeScrollMargin)
        {
            moveDirection -= GetRightDirection() * edgeScrollSpeed * Time.deltaTime;
        }
        // Right edge
        if (mousePos.x > Screen.width - edgeScrollMargin)
        {
            moveDirection += GetRightDirection() * edgeScrollSpeed * Time.deltaTime;
        }
        // Top edge
        if (mousePos.y > Screen.height - edgeScrollMargin)
        {
            moveDirection += GetForwardDirection() * edgeScrollSpeed * Time.deltaTime;
        }
        // Bottom edge
        if (mousePos.y < edgeScrollMargin)
        {
            moveDirection -= GetForwardDirection() * edgeScrollSpeed * Time.deltaTime;
        }

        transform.position += moveDirection;
    }

    void HandleZoom()
    {
        // Scroll wheel zoom
        float zoomInput = Input.GetAxis("Mouse ScrollWheel");
        if (zoomInput != 0)
        {
            currentZoom -= zoomInput * zoomSpeed;
            currentZoom = Mathf.Clamp(currentZoom, minZoom, maxZoom);
        }

        // Keyboard zoom
        if (Input.GetKey(zoomIn))
        {
            currentZoom -= zoomSpeed * Time.deltaTime;
        }
        if (Input.GetKey(zoomOut))
        {
            currentZoom += zoomSpeed * Time.deltaTime;
        }

        currentZoom = Mathf.Clamp(currentZoom, minZoom, maxZoom);
    }

    void HandleRotation()
    {
        if (Input.GetKey(rotateLeft))
        {
            currentRotation += rotationSpeed * Time.deltaTime;
        }
        if (Input.GetKey(rotateRight))
        {
            currentRotation -= rotationSpeed * Time.deltaTime;
        }
    }

    void ApplyMovement()
    {
        // Clamp position to bounds
        Vector3 newPosition = transform.position;
        newPosition.x = Mathf.Clamp(newPosition.x, minX, maxX);
        newPosition.z = Mathf.Clamp(newPosition.z, minZ, maxZ);
        newPosition.y = currentZoom;

        transform.position = newPosition;

        // Apply rotation
        transform.rotation = Quaternion.Euler(30f, currentRotation, 0f);
    }

    Vector3 GetForwardDirection()
    {
        return new Vector3(Mathf.Sin(currentRotation * Mathf.Deg2Rad), 0, Mathf.Cos(currentRotation * Mathf.Deg2Rad));
    }

    Vector3 GetRightDirection()
    {
        return new Vector3(Mathf.Cos(currentRotation * Mathf.Deg2Rad), 0, -Mathf.Sin(currentRotation * Mathf.Deg2Rad));
    }

    public void CenterOnPosition(Vector3 position)
    {
        Vector3 newPos = new Vector3(position.x, currentZoom, position.z + 20f);
        transform.position = newPos;
    }

    public void FocusOnUnit(Transform unit)
    {
        CenterOnPosition(unit.position);
    }
}
