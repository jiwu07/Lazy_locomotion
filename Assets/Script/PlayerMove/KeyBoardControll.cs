using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

public class KeyBoardControll : MonoBehaviour
{

     float moveSpeed = 1f; 
    public Animator animator;

    public XRController moveController;
    public InputHelpers.Button moveButton;


    public GameObject player;
    float maxSpeed = 5;

   

    void Update()
    {
        if (moveController.inputDevice.IsPressed(moveButton, out bool pressed, moveController.axisToPressThreshold)) // 
        {
            
            if (pressed)
            {
                if(moveSpeed<1) { moveSpeed = 1; }
                moveSpeed *= moveSpeed * 1.001f;
                moveSpeed = Mathf.Min(moveSpeed, maxSpeed);
                animator.SetFloat("Speed",moveSpeed);
                animator.SetFloat("f", 1);
                player.transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);

                return;
            }
            // moveSpeed *= moveSpeed *0.8f;
            //animator.SetFloat("Speed", Mathf.Max(moveSpeed, 0));
            Debug.Log(moveSpeed);

            moveSpeed = moveSpeed * 0.95f;
            animator.SetFloat("Speed", Mathf.Max(moveSpeed, 0f));
            player.transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);


        }

    }


}
