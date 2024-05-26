using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraMovement : MonoBehaviour
{
    public GameObject avatarHead;
    private Transform headPosition;
    


    void Start(){
        headPosition = avatarHead.transform;
        Transform cameraTransform = Camera.main.transform;
        transform.position = headPosition.position - cameraTransform.position;

    }
    // Update is called once per frame
    void Update()
    {
        transform.position = new Vector3(transform.position.x,headPosition.position.y, transform.position.z);
        
    }
}
