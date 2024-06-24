using UnityEngine;
using UnityEngine.UI;

public class FadeToBlack : MonoBehaviour
{
    public Image overlayImage; // Reference to the Image with the black overlay
    public float fadeDuration ; // Duration over which the fade occurs

    private Color initialColor;
    private float timer = 0;
    public bool isStart = false;
    public bool isFinish = false;

    void Start()
    {
        initialColor = overlayImage.color;
        initialColor.a = 0f;
        overlayImage.color = initialColor; // Ensure the image starts fully transparent
    }

    void Update()
    {
        if (isStart && !isFinish)
        {
            if (timer < fadeDuration)
            {
                timer += Time.deltaTime;
                float normalizedTime = timer / fadeDuration;
                initialColor.a = Mathf.Lerp(0f, 1f, normalizedTime);
                overlayImage.color = initialColor;
            }
            else
            {
                initialColor.a = 1f;
                overlayImage.color = initialColor;
                isFinish = true;
                isStart = false;
            }
        }
    }

    public void StartFade()
    {
        if (!isStart)
        {
            timer = 0;
            isStart = true;
            isFinish = false;
        }
    }

    public void ResetFade()
    {
        timer = 0;
        initialColor.a = 0f;
        overlayImage.color = initialColor;
        isStart = false;
        isFinish = false;
    }
}
