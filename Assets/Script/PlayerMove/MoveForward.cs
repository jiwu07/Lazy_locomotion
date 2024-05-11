using UnityEngine;

using Valve.VR;

public class MoveForward : MonoBehaviour
{
    public SteamVR_Action_Boolean moveForwardAction; // 定义一个 SteamVR_Action_Boolean 用于检测前进的输入动作
    public float moveSpeed = 3.0f; // 玩家移动速度

    private CharacterController characterController;

    void Start()
    {
        characterController = GetComponent<CharacterController>();
    }

    void Update()
    {
        // 检测前进输入动作是否被触发
        if (moveForwardAction.GetState(SteamVR_Input_Sources.Any))
        {
            Move();
        }
    }

    // 前进函数
    void Move()
    {
        // 获取玩家头部的正前方向
        Vector3 forward = transform.forward;
        // 设置y分量为0，保证只在水平方向移动
        forward.y = 0;
        // 根据移动速度和时间，计算移动向量
        Vector3 moveDirection = forward * moveSpeed * Time.deltaTime;
        // 应用移动向量到玩家角色上
        characterController.Move(moveDirection);
    }
}
