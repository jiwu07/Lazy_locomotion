using System.Collections;
using System.Collections.Generic;

using TMPro;

using Unity.VisualScripting;

using UnityEditor.SearchService;

using UnityEngine;
using UnityEngine.SceneManagement;


public class MainSceneControll : MonoBehaviour
{
    public string nextSceneName = "Room4x4";
    float count = 5.0f;
    bool isCount = false;
    public GameObject room;

    public TextMeshPro text;

    private void Start()
    {
        text.text = "Press any button to start the test";
    }




    void Update()
    {
        if ((Input.GetMouseButtonDown(0) || Input.GetMouseButtonDown(1)) && !isCount)
        {
            isCount = true;
        }

        if (isCount && count > 0)
        {
            //black vision
            room.SetActive(false);
            //count down
            count -= Time.deltaTime;
            int temp = (int)count;
            text.text = temp.ToString() + " s to get start the test";

        }
        else if(isCount && count < 0)
        {
            SceneManager.LoadScene(nextSceneName);
        }
    }


}
