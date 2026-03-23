using UnityEngine;

public class HealthBar : MonoBehaviour
{
    public Image fillImage;
    public Image backgroundImage;
    public float hideDelay = 3f;
    
    private float hideTimer = 0f;
    private bool isVisible = true;

    void Start()
    {
        if (fillImage != null)
        {
            fillImage.fillAmount = 1f;
        }
    }

    void Update()
    {
        if (!isVisible && fillImage != null && fillImage.fillAmount >= 1f)
        {
            hideTimer += Time.deltaTime;
            if (hideTimer >= hideDelay)
            {
                gameObject.SetActive(false);
            }
        }
    }

    public void UpdateHealth(float percentage)
    {
        if (fillImage != null)
        {
            fillImage.fillAmount = percentage;
        }

        // Show health bar when damaged
        if (percentage < 1f)
        {
            gameObject.SetActive(true);
            isVisible = true;
            hideTimer = 0f;
        }

        // Color based on health
        if (fillImage != null)
        {
            if (percentage > 0.5f)
            {
                fillImage.color = Color.green;
            }
            else if (percentage > 0.25f)
            {
                fillImage.color = Color.yellow;
            }
            else
            {
                fillImage.color = Color.red;
            }
        }
    }
}
