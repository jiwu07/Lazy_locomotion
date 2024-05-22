using System.Collections;
using System.Collections.Generic;

using TMPro;

using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

public class LearningTest_LightControllLoco : MonoBehaviour
{
    public TextMeshProUGUI distanceText;
    public GameObject roomOFFObject;
    public GameObject player;

    public LayerMask LightOffMask;
    public LayerMask All;


    public XRController Controller;
    public InputHelpers.Button Button;


    string text = " to task point ";
    string text2 = " trails: ";
    int count = 2;

    int distance;
    public bool lightOn = false;

    Transform targetTransform;
    public GameObject taskObject;
    bool isPressed = false;

    void Start()
    {
        targetTransform = taskObject.transform;
    }


    // Update is called once per frame
    void Update()
    {
        //left mouse click turn light on/off
        if (Controller.inputDevice.IsPressed(Button, out bool pressed, Controller.axisToPressThreshold)) 
        {
            if (pressed && !isPressed)
            {
                lightOn = !lightOn;
                count++;
                isPressed = true;
            }

            if (!pressed)
            {
                isPressed = false;
            }

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
            //todo
        }
        else
        {
            //show distance and trials number
            distanceText.text = distance.ToString() + text + text2 + (count / 2).ToString();
        }
    }
}
