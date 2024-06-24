using System.Collections;
using System.Collections.Generic;
using System.IO;

using TMPro;

using UnityEngine;
using UnityEngine.SceneManagement;
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
    string text2 = " trials: ";
    int count = 2;

    int distance;
    bool lightOn = true;

    Transform targetTransform;
    public GameObject taskObject;
    bool isPressed = false;

    public string filePath;

    int testCount = 1;
    int maxTestCount = 5;
    float nextTestCount = 0;

    public string nextSceneName = "EndScene";
    FadeToBlack FadeToBlack;
    Animator animator;

    int[] distanceList = {20,6,50,15 };

    void Start()
    {
        targetTransform = taskObject.transform;

        // Create CSV file and add headers if the file doesn't exist
        if (!File.Exists(filePath))
        {
            File.WriteAllText(filePath, "TestCount,Timestamp,Trial,LightOn,PlayerPositionZ,TargetPositionZ,DistanceDifference\n");
        }

        animator = player.GetComponent<Animator>();
        FadeToBlack = GetComponent<FadeToBlack>();
        

    }

    void Update()
    {
       // Debug.Log(" going normal");
        if(testCount > maxTestCount)
        {
           // Debug.Log("end");

            player.GetComponent<ArduinoComPort>().OnApplicationQuit();
            SceneManager.LoadScene(nextSceneName);
        }

        if(nextTestCount > 0) {
           // Debug.Log("next test");
            NextTest();
            return;
        }
        // let controller click turn light on/off
        if (Controller.inputDevice.IsPressed(Button, out bool pressed, Controller.axisToPressThreshold))
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
        

        if (!pressed)
            {
                isPressed = false;
            }
        }

        if (!lightOn)
        {  // light off
            if (FadeToBlack.isFinish)
            {
                roomOFFObject.SetActive(false);  // no need stuff off
                distanceText.transform.gameObject.SetActive(false);  // ui text off
                Camera.main.GetComponent<Camera>().cullingMask = LightOffMask;  // camera mask on
                player.GetComponent<SimplePlayerArduino>().UsingComPort = true;  // player cannot move

            }
            else
            {
                return;
            }
        }
        else
        {  // light on
            roomOFFObject.SetActive(true);  // no need stuff on
            Camera.main.GetComponent<Camera>().cullingMask = All;  // camera mask on
            distanceText.transform.gameObject.SetActive(true);  // ui text on
            player.GetComponent<SimplePlayerArduino>().UsingComPort = false;  // player cannot move
            animator.SetFloat("Forward", 0);
            animator.SetFloat("Pace", 0);
         
            CheckArrive();
        }


    }

    void CheckArrive()
    {
        distance = (int)(targetTransform.position.z - player.transform.position.z);

        if (distance == 0)
        {
            player.GetComponent<SimplePlayerArduino>().UsingComPort = false;  // player cannot move
            animator.SetFloat("Forward", 0);
            animator.SetFloat("Pace", 0);
            distanceText.text = "Congratulation! You arrived at the target point in " + ((count - 2) / 2).ToString() + " trials";
            nextTestCount = 5;
            testCount++;
        }
        else
        {
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
    void NextTest(){
        
        nextTestCount -= Time.deltaTime;
        distanceText.text = "Congratulation! You arrived at the target point in " + ((count - 2) / 2).ToString() + " trials";
        distanceText.text += "  Next test start in " + nextTestCount.ToString("N2");
        
        if(nextTestCount < 1 && nextTestCount > 0)
        {
            // Camera.main.GetComponent<Camera>().cullingMask = LightOffMask;  
            FadeToBlack.StartFade();
        }
        if (nextTestCount <= 0) {
            // move player to origin
            player.transform.position =  Vector3.zero;
            player.transform.rotation = Quaternion.identity;

            //active task object
            taskObject.SetActive(true);
            Camera.main.GetComponent<Camera>().cullingMask = All;  // camera mask on

            count = 2;
            //reset the fade
            FadeToBlack.ResetFade();
            //change target distance
            taskObject.transform.position = new Vector3(targetTransform.position.x, targetTransform.position.y, distanceList[testCount-2]);
        }
    }


}
