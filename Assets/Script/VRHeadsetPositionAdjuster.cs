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
     Vector3 offsetZ = new Vector3(0,0.02f,0.1f);



    void Start()
    {
        headPosition = avatarHead.transform;
        offset = headPosition.position - Camera.main.transform.position;
        //transform.position = headPosition.position - offset;
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
        if (isCalibrate)
        {
            Calibrate();
            isCalibrate = false;
        }

       // transform.position =new Vector3( transform.position.x, headPosition.position.y, transform.position.z);

    }



}
