using System.Collections;
using System.Collections.Generic;

using TMPro;

using UnityEngine;
using UnityEngine.SceneManagement;
using System.IO;
using UnityEngine.XR.Interaction.Toolkit;

public class ControllerTestLoco : MonoBehaviour
{

    int turnCount = 0;
    public string nextSceneName = "MainTestScene";
    float count = 5.0f;
    public TextMeshProUGUI text;

    public GameObject point1;
    public GameObject point2;
    public GameObject point3;
    public GameObject point4;
    public GameObject player;

    bool isStart = false;

    public XRController turnController;
    public InputHelpers.Button turnButton;
    bool isPressed = false;


    public string csvFilePath ; 
    //record correct path(point on each corner)
    void Start()
    {
        //save the point data
        if (!File.Exists(csvFilePath))
        {
            File.WriteAllText(csvFilePath, "N,X,Y,Z\n");
        }
        //write point inside
        Vector3 position = point1.transform.position;
        string pointData = string.Format("{0},{1},{2},{3}\n", "point 1", position.x, position.y, position.z);
        File.AppendAllText(csvFilePath, pointData);

         position = point2.transform.position;
         pointData = string.Format("{0},{1},{2},{3}\n", "point 2", position.x, position.y, position.z);
        File.AppendAllText(csvFilePath, pointData);

         position = point3.transform.position;
         pointData = string.Format("{0},{1},{2},{3}\n", "point 3", position.x, position.y, position.z);
        File.AppendAllText(csvFilePath, pointData);

         position = point4.transform.position;
         pointData = string.Format("{0},{1},{2},{3}\n", "point 4", position.x, position.y, position.z);
        File.AppendAllText(csvFilePath, pointData);

      
    }


    void Update()
    {
        //if lightoff
        isStart = GetComponent<LightControllLoco>().isControll;
        //turn right, start counting after light off
        if (turnController.inputDevice.IsPressed(turnButton, out bool pressed, turnController.axisToPressThreshold) && isStart)
        {
            if (pressed && !isPressed)
            {
                turnCount++;
                //record the turn point 
                Vector3 position = player.transform.position;
                string positionData = string.Format("{0},{1},{2},{3}\n", "turn " + turnCount, (int)position.x, (int)position.y, (int)position.z);
                File.AppendAllText(csvFilePath, positionData);
                isPressed = true;
            }
            if (!pressed)
            {
                isPressed = false;
            }

        }

        //in total need to turn right 4 time to reach the start state
        if (turnCount >= 4)
        {
            //this turn end go to next room
            //count down to next room
            if (count > 0)
            {
                //count down
                count -= Time.deltaTime;
                int temp = (int)count;
                text.transform.gameObject.SetActive(true);
                text.text = temp.ToString() + " s to get start the next test";
                //freez player
                player.GetComponent<SpeedControll>().enabled = false;
                player.transform.Find("Camera").GetComponent<Turn>().enabled = false;

            }
            else
            {
                SceneManager.LoadScene(nextSceneName);
            }
        }
    }

  
}
