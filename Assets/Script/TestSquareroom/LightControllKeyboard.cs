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

    string text = "Light off in ";
    float count = 10.0f;
    public bool isControll = true;


    // Update is called once per frame
    void Update()
    {
        if (count > 0 && isControll)
        {
            //light off count down
            int t = (int)count;
            lightText.text = text +  t.ToString() + "s";
            count -= Time.deltaTime;
            //freez player movement
            player.GetComponent<KeyBoardControll>().enabled = false;
            player.GetComponent<Turn>().enabled = false;

        }
        else if(isControll)
        {
            //Player can start move
            player.GetComponent<KeyBoardControll>().enabled = true;
            player.GetComponent<Turn>().enabled = true;
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
