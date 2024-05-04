using System.Collections;
using System.Collections.Generic;

using TMPro;

using UnityEngine;

public class LearningTest_LightControll : MonoBehaviour
{
    public TextMeshProUGUI distanceText;
    public GameObject roomOFFObject;
    public GameObject player;

    public LayerMask LightOffMask;
    public LayerMask All;


    string text = " to task point ";
    string text2 = " trails: ";
    int count = 2;

    int distance;
    public bool lightOn = false;

    Transform targetTransform;
    public GameObject taskObject;

    void Start()
    {
        targetTransform = taskObject.transform;
    }


    // Update is called once per frame
    void Update()
    {
        //left mouse click turn light on/off
        if (Input.GetMouseButtonDown(0))
        {
            lightOn = !lightOn;
            count++;
        }


        if (!lightOn)
        {  // light off
            //no need stuff off
            roomOFFObject.SetActive(false);
            //ui text off
            distanceText.transform.gameObject.SetActive(false);
            //cameramask on
            Camera.main.GetComponent<Camera>().cullingMask = LightOffMask;
            // player move
            player.GetComponent<SpeedControll>().enabled = true;

        }
        else
        {
            //light on
            //no need stuff on
            roomOFFObject.SetActive(true);
            //cameramask on
            Camera.main.GetComponent<Camera>().cullingMask = All;
            //show feedback
            //ui text on
            distanceText.transform.gameObject.SetActive(true);
            //Player can not  move
            player.GetComponent<SpeedControll>().enabled = false;

        }

        CheckArrive();
    }

    void CheckArrive()
    {
        distance = (int)(targetTransform.position.z - player.transform.position.z) ;

        if (distance == 0)
        {
            distanceText.text = " Congratulation! You arrived target point in " + ((count-2) / 2).ToString() + " trials";
            // player move
            player.GetComponent<SpeedControll>().enabled = true;
        }
        else
        {
            //show distance and trials number
            distanceText.text = distance.ToString() + text + text2 + (count / 2).ToString();
        }
    }
}
