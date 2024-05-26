
using TMPro;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.XR.Interaction.Toolkit;


public class MainSceneControll : MonoBehaviour
{
    public string nextSceneName = "Room4x4";
    float count = 3.0f;
    bool isCount = false;
    public GameObject room;

    public TextMeshPro text;

    public XRController Controller;
    public InputHelpers.Button moveButton;

    private void Start()
    {
        text.text = "";
    }




    void Update()
    {
        if (Controller.inputDevice.IsPressed(moveButton, out bool pressed, Controller.axisToPressThreshold)&& !isCount) 
        {
            isCount = pressed;
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
