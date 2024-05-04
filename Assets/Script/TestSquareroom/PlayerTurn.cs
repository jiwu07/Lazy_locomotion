using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerTurn : MonoBehaviour
{
    float rotationSpeed = 45f; 
    private bool isRotating = false; 
    float targetRotation = 0f;

    public GameObject turnPreFab;
    public bool needMark;

    int direction = -1;
    
    // Update is called once per frame
    void Update()
    {
        //mouse left button pressed
        if (Input.GetMouseButtonDown(0))
        {
            direction = 0;
            //Instantiate(turnPreFab, transform.position, Quaternion.identity);
        }

        //mouse right button pressed, leave a mark if need
        if (Input.GetMouseButtonDown(1))
        {
            direction = 1;
            if (needMark)
            {
                Instantiate(turnPreFab, transform.position, Quaternion.identity);
            }
        }

        Turn();
    }

    // 0 turn left 1 turn right
    void Turn()
    {
        if (direction == 1 && !isRotating)
        {
            targetRotation += 90f;
            isRotating = true;
        }
        else if (direction == 0 && !isRotating)
        {
            targetRotation -= 90f;
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
