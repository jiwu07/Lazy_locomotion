using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class KeyBoardControll : MonoBehaviour
{

    public float moveSpeed = 5f; 
    public Animator animator; 

    void Update()
    {
        if (Input.GetMouseButton(0)) // mouse left button pressing
        {
            // move
            transform.Translate(Vector3.forward * moveSpeed * Time.deltaTime);
            animator.SetFloat("Speed", moveSpeed);
            animator.SetFloat("f", 1);

            return;
        }
        animator.SetFloat("Speed", 0f);

    }


}
