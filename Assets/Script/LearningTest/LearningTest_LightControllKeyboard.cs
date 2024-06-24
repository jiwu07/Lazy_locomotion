using System.Collections;
using System.Collections.Generic;
using System.IO;
using TMPro;

using UnityEngine;
using UnityEngine.SceneManagement;
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
    int testCount = 1;
    int maxTestCount = 5;
    float nextTestCount = 0;
    public string nextSceneName = "EndScene";
    int[] distanceList = { 20, 6, 50, 15 };
    FadeToBlack FadeToBlack;

    public string filePath;
    void Start()
    {
        targetTransform = taskObject.transform;

        // Create CSV file and add headers if the file doesn't exist
        if (!File.Exists(filePath))
        {
            File.WriteAllText(filePath, "TestCount,Timestamp,Trial,LightOn,PlayerPositionZ,TargetPositionZ,DistanceDifference\n");
        }
        FadeToBlack = GetComponent<FadeToBlack>();
    }


    // Update is called once per frame
    void Update()
    {
        // Debug.Log(" going normal");
        if (testCount > maxTestCount)
        {
            // Debug.Log("end");
            SceneManager.LoadScene(nextSceneName);
        }

        if (nextTestCount > 0)
        {
            // Debug.Log("next test");
            NextTest();
            return;
        }
        //left mouse click turn light on/off
        if (Controller.inputDevice.IsPressed(Button, out bool pressed, Controller.axisToPressThreshold)) 
        {
            if (pressed && !isPressed)
            {                    
                // Log the time, player position, and distance difference         
                if (lightOn)
                {
                    lightOn = !lightOn;
                    count++;
                    isPressed = true;
                    FadeToBlack.StartFade();
                    LogLightSwitch();
                }
                else
                {
                    if (FadeToBlack.isFinish)
                    {
                        lightOn = !lightOn;
                        count++;
                        isPressed = true;
                        FadeToBlack.ResetFade();
                    }
                    
                }
            }

            if (!pressed)
            {
                isPressed = false;
            }
            
        }


        if (!lightOn)
        {  // light off

            if (FadeToBlack.isFinish)
            {
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
                return;
            }
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
            CheckArrive();
        }

      
    }

    void CheckArrive()
    {
        distance = (int)(targetTransform.position.z - player.transform.position.z) ;

        if (distance == 0)
        {
            distanceText.text = " Congratulation! You arrived target point in " + ((count-2) / 2).ToString() + " trials";
            player.transform.Find("Camera").GetComponent<KeyBoardControll>().enabled = false;
            //todo
            nextTestCount = 5;
            testCount++;
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
        float distanceDifference = targetTransform.position.z - player.transform.position.z;
        string logEntry = string.Format("{0},{1},{2},{3},{4},{5},{6}\n", testCount.ToString(), timestamp, count / 2, lightOn, playerPosition.z, targetTransform.position.z, distanceDifference);

        File.AppendAllText(filePath, logEntry);
    }

    /// <summary>
    /// count down, move player back to origin and start the next test
    /// </summary>
    void NextTest()
    {

        nextTestCount -= Time.deltaTime;
        distanceText.text = "Congratulation! You arrived at the target point in " + ((count - 2) / 2).ToString() + " trials";
        distanceText.text += "  Next test start in " + nextTestCount.ToString("N2");

        if (nextTestCount < 1 && nextTestCount > 0)
        {
            //Camera.main.GetComponent<Camera>().cullingMask = LightOffMask;
            FadeToBlack.StartFade();
        }
        if (nextTestCount <= 0)
        {
            // move player to origin
            player.transform.position = Vector3.zero;
            player.transform.rotation = Quaternion.identity;

            //active task object
            taskObject.SetActive(true);
            Camera.main.GetComponent<Camera>().cullingMask = All;  // camera mask on
            //reset the fade
            FadeToBlack.ResetFade();
            count = 2;
            //change target distance
            taskObject.transform.position = new Vector3(targetTransform.position.x, targetTransform.position.y, distanceList[testCount-2]);
        }
    }
}
