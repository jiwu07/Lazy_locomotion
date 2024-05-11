using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using Valve.VR;


public class LearningTestTurn : MonoBehaviour
{


    public SteamVR_Action_Boolean turnBackAction; // 定义一个 SteamVR_Action_Boolean 用于检测前进的输入动作


    public bool needMark = false;

    public GameObject player;
    public GameObject turnPreFab;


    void Update()
    {

        if (turnBackAction.GetState(SteamVR_Input_Sources.Any))
        {
            Turn();
            if (needMark)
            {
                Instantiate(turnPreFab, transform.position, Quaternion.identity);
            }
        }

        Turn();
    }

    //  1 turn back
    void Turn()
    {
        player.transform.Rotate(0f, 180f, 0f); ;
    }
}

