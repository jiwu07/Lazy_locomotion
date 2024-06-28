using UnityEngine;
using UnityEngine.InputSystem; // Ensure you have the new Input System package installed
using System.IO;
using UnityEngine.XR.Interaction.Toolkit;
using Unity.VisualScripting;

public class PlayerPositionRecordKeyboad : MonoBehaviour
{
    private StreamWriter writer;
    public string fileName = "PlayerPositionRecords.csv";
    public XRController Controller;
    public InputHelpers.Button turnButton;
    //public XRController moveController;
    public InputHelpers.Button moveButton;
    void Start()
    {
        // Open the file for writing and write the header
        writer = new StreamWriter(fileName);
        writer.WriteLine("Time,X,Z,movePressed,turnPressed");
    }
    void Update()
    {

        bool turn = Controller.inputDevice.IsPressed(turnButton, out bool turnPressed, Controller.axisToPressThreshold);
        bool move =Controller.inputDevice.IsPressed(moveButton, out bool movePressed, Controller.axisToPressThreshold);
        // Record the player's position and time
        float time = Time.time;
        Vector2 position = new Vector2(transform.position.x, transform.position.z);

        // Write the data to the CSV file
        writer.WriteLine($"{time},{position.x},{position.y},{move},{turn}");
        writer.Flush();  // Ensure data is written to the file immediately
    }

    void OnApplicationQuit()
    {
        // Close the StreamWriter when the application quits
        writer.Close();
    }
}
