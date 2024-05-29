using System.Collections;
using System.Collections.Generic;
using System.IO;
using TMPro;

using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

public class LearningTest_LightControllKeyboard : MonoBehaviour
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
    bool lightOn = true;

    Transform targetTransform;
    public GameObject taskObject;

    bool isPressed = false;

    public string filePath;
    void Start()
    {
        targetTransform = taskObject.transform;

        // Create CSV file and add headers if the file doesn't exist
        if (!File.Exists(filePath))
        {
            File.WriteAllText(filePath, "Timestamp,Trial,LightOn,PlayerPositionZ,TargetPositionZ,DistanceDifference\n");
        }
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
                // Log the time, player position, and distance difference
                LogLightSwitch();
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
            player.transform.Find("Camera").GetComponent<KeyBoardControll>().enabled = true;

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
            player.transform.Find("Camera").GetComponent<KeyBoardControll>().enabled = false;

        }

        CheckArrive();
    }

    void CheckArrive()
    {
        distance = (int)(targetTransform.position.z - player.transform.position.z) ;

        if (distance == 0)
        {
            distanceText.text = " Congratulation! You arrived target point in " + ((count-2) / 2).ToString() + " trials";
            player.GetComponent<SpeedControll>().enabled = true;
            //todo
        }
        else
        {
            //show distance and trials number
            distanceText.text = distance.ToString() + text + text2 + (count / 2).ToString();
        }
    }

    void LogLightSwitch()
    {
        string timestamp = System.DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
        Vector3 playerPosition = player.transform.position;
        float distanceDifference = (int)(targetTransform.position.z - player.transform.position.z);
        string logEntry = string.Format("{0},{1},{2},{3},{4},{5}\n", timestamp, count / 2, lightOn, playerPosition.z, targetTransform.position.z, distanceDifference);

        File.AppendAllText(filePath, logEntry);
    }
}
