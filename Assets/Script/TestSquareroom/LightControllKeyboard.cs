using System.Collections;
using System.Collections.Generic;

using TMPro;

using UnityEngine;

public class LightControllKeyboard : MonoBehaviour
{
    public TextMeshProUGUI lightText;
    public GameObject roomOFFObject;
    //public GameObject cameraMask;
    public GameObject player;

    public LayerMask LightOffMask;
    public LayerMask LightOnMask;


    string text = "Get Dark in ";
    float count = 9.0f;
    public bool isControll = true;
    public bool isCalibrate = false;



    // Update is called once per frame
    void Update()
    {
       
        if (count > 0 && isControll)
        {
            if (!isCalibrate)
            {
                lightText.text = "wait for calibrate the camera position";
                Camera.main.GetComponent<Camera>().cullingMask = LightOffMask;
                return;
            }
            Camera.main.GetComponent<Camera>().cullingMask = LightOnMask;


            //freez player movement
            player.transform.Find("Camera").GetComponent<KeyBoardControll>().enabled = false;
            player.transform.Find("Camera").GetComponent<Turn>().enabled = false;

            //light off count down
            int t = (int)count;
            lightText.text = text + t.ToString() + "s";
            count -= Time.deltaTime;
        }
        else if(isControll)
        {
            //Player can start move
            player.transform.Find("Camera").GetComponent<KeyBoardControll>().enabled = true;
            player.transform.Find("Camera").GetComponent<Turn>().enabled = true;
            //no need stuff off
            roomOFFObject.SetActive(false);
            //ui text off
            lightText.transform.gameObject.SetActive(false);
            //cameramask on
            Camera.main.GetComponent<Camera>().cullingMask = LightOffMask;
            isControll = false;
        }

        
        
    }
}
