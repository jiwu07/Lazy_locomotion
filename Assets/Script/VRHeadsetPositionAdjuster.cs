using UnityEngine;
using System.Collections;

public class VRHeadsetPositionAdjuster : MonoBehaviour
{
    public Transform targetObject;  // 拖动要参考的对象到这里
    private Vector3 initialOffset;  // 初始偏移量

    void Start()
    {
        if (targetObject != null)
        {
            // 启动协程等待一帧
            StartCoroutine(CalculateInitialOffset());
        }
        else
        {
            Debug.LogError("Target Object is not assigned.");
        }
    }

    IEnumerator CalculateInitialOffset()
    {
        // 等待一帧以确保VR头显初始化完成
        yield return null;

        // 抓取当前头显的位置
        Transform cameraTransform = Camera.main.transform;
        initialOffset = cameraTransform.position - targetObject.position;

        // 设置摄像头的位置
        cameraTransform.position = targetObject.position + initialOffset;

        // 使摄像头面向目标对象
        cameraTransform.LookAt(targetObject);
    }
}
