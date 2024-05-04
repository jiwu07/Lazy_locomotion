using System.Collections;
using System.Collections.Generic;

using TMPro;

using UnityEngine;
using UnityEngine.SceneManagement;
using System.IO;

public class ControllerTest : MonoBehaviour
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

        //if lightoff
        isStart = GetComponent<LightControll>().isControll;
    }


    void Update()
    {
        //turn right, start counting after light off
        if (Input.GetMouseButtonDown(1) && isStart)
        {

            turnCount++;
            //record the turn point 
            Vector3 position = player.transform.position;
            string positionData = string.Format("{0},{1},{2},{3}\n","turn "+ turnCount, (int) position.x, (int)position.y, (int)position.z);
            File.AppendAllText(csvFilePath, positionData);
        }

        //in case user press wrong button
        if (Input.GetMouseButtonDown(0) && isStart)
        {
            turnCount--;
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

            }
            else
            {
                SceneManager.LoadScene(nextSceneName);
            }
        }
    }

  
}
