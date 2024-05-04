using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LearningTestTurn : MonoBehaviour
{
    float rotationSpeed = 45f; 
    private bool isRotating = false; 
    float targetRotation = 0f;

    public bool needMark = false;

    int direction = -1;

    void Update()
    {

        //mouse right button pressed, leave a mark if need
        if (Input.GetMouseButtonDown(1))
        {
            direction = 1;
        }

        Turn();
    }

    //  1 turn back
    void Turn()
    {
        if (direction == 1 && !isRotating)
        {
            targetRotation += 180f;
            isRotating = true;
        }
        else if (direction == 0 && !isRotating)
        {
            targetRotation -= 180f;
            isRotating = true;
        }

        if (isRotating)
        {
            float step = rotationSpeed * Time.deltaTime;
            //turn
            transform.rotation = Quaternion.RotateTowards(transform.rotation, Quaternion.Euler(0f, targetRotation, 0f), step);
            if (Quaternion.Angle(transform.rotation, Quaternion.Euler(0f, targetRotation, 0f)) < 0.1f)
            {
                transform.rotation = Quaternion.Euler(0f, targetRotation, 0f);
                isRotating = false;
                direction = -1;
            }
        }
    }
}

