using UnityEngine;
using System.Collections;
using Unity.XR.CoreUtils;
using Unity.VisualScripting;

public class VRHeadsetPositionAdjuster : MonoBehaviour
{
    public XROrigin xROrigin;
     Vector3 offset;
    public GameObject avatarHead;
    public Transform headPosition;

    public bool isCalibrate;
     Vector3 offsetZ = new Vector3(0f,0.035f,0.5f);
    float startTime;



    void Start()
    {
        headPosition = avatarHead.transform;
        offset = headPosition.position - Camera.main.transform.position;
        //transform.position = headPosition.position - offset;
        float startTime = Time.time;

        Calibrate();

    }
    void Calibrate()
    {
        //xROrigin.CameraYOffset += offset.y;
        transform.position += offset;
        transform.position += offsetZ;

    }

    void Update()
    {
        offset = headPosition.position - Camera.main.transform.position;
        if (isCalibrate && Time.time-startTime >1)
        {
            Calibrate();
            isCalibrate = false;
        }

       // transform.position =new Vector3( transform.position.x, headPosition.position.y, transform.position.z);

    }



}
