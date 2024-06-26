using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class speed : MonoBehaviour
{
    public float smoothingFactor = 0.1f;  // Smoothing factor, between 0 and 1
    private float currentSpeed = 0f;  // Current speed
    public float h = 1.7f;
    float step_length;
    private void Start()
    {
         step_length = h * h / (1.72f * 1.52f * 1.72f * 1.52f); // WIP use 1.52 --GUDWIP using 0.157

    }
    void Update()
    {
        float f = ArduinoComPort.Pace;

        float  targetSpeed = f * f * step_length;
        // Apply a low-pass filter to smoothly adjust the current speed towards the target speed
        currentSpeed = Mathf.Lerp(currentSpeed, targetSpeed, smoothingFactor);

        // Move the player forward
        transform.Translate(Vector3.forward * currentSpeed * Time.deltaTime);

        Debug.Log(currentSpeed);
    }
}
