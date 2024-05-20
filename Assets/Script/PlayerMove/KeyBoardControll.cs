using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

public class KeyBoardControll : MonoBehaviour
{

    public float moveSpeed = 5f; 
    public Animator animator;

    public XRController moveController;
    public InputHelpers.Button moveButton;


    public GameObject player;
  

    void Update()
    {
        if (moveController.inputDevice.IsPressed(moveButton, out bool pressed, moveController.axisToPressThreshold)) // mouse left button pressing
        {
            // turn 
            if (pressed)
            {
                transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);
                animator.SetFloat("Speed", moveSpeed);
                animator.SetFloat("f", 1);
            }
            // move
            

            return;
        }
        animator.SetFloat("Speed", 0f);

    }


}
