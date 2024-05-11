using UnityEngine;

using Valve.VR;

public class TurnAction : MonoBehaviour
{
    public SteamVR_Action_Boolean turnRightAction; // 定义一个 SteamVR_Action_Boolean 用于检测前进的输入动作
    public SteamVR_Action_Boolean turnLeftAction; // 定义一个 SteamVR_Action_Boolean 用于检测前进的输入动作

    public float moveSpeed = 3.0f; // 玩家移动速度
    public bool needMark;
    public GameObject turnPreFab;

    int turnRightCount = 0;

    void Update()
    {
        // 检测前进输入动作是否被触发
        if (turnRightAction.GetState(SteamVR_Input_Sources.RightHand))
        {
            Turn(90);
            turnRightCount++;
        }
        if (turnLeftAction.GetState(SteamVR_Input_Sources.LeftHand))
        {
            Turn(-90);
        }
    }

    // 前进函数
    void Turn(int turn)
    {
        transform.Rotate(Vector3.up, turn);
        //leave a mark on the ground
        if (needMark)
        {
            Instantiate(turnPreFab, transform.position, Quaternion.identity);
        }
    }

    public int GetTurnRightCount()
    {
        return turnRightCount;
    }
}
