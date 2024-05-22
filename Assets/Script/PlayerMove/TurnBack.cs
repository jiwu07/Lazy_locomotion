using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.XR.Interaction.Toolkit;

public class TurnBack : MonoBehaviour
{
    public XRController turnController;
    public InputHelpers.Button turnButton;
    bool isPressed = false;

    public GameObject player;
    public bool needMark;
    public GameObject turnPreFab;


    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (turnController.inputDevice.IsPressed(turnButton, out bool pressed, turnController.axisToPressThreshold)) // mouse left button pressing
        {
            // turn 
            if (pressed && !isPressed)
            {
                player.transform.Rotate(0, 180, 0);
                isPressed = true;
                if (needMark)
                {
                    Instantiate(turnPreFab, transform.position, Quaternion.identity);
                }
            }

            if (!pressed)
            {
                isPressed = false;
            }
        }
    }
}
