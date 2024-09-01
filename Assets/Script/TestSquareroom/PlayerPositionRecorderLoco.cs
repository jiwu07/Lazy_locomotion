using UnityEngine;
using System.IO;

public class PlayerPositionRecorderLoco : MonoBehaviour
{
    private StreamWriter writer;
    public string fileName = "PlayerPositionRecords.csv";

    void Start()
    {
        // Open the file for writing and write the header
        writer = new StreamWriter(fileName);
        writer.WriteLine("Time,X,Z");
    }

    void Update()
    {
        // Record the player's position and time
        float time = Time.time;
        Vector2 position = new Vector2(transform.position.x, transform.position.z);
        writer.WriteLine($"{time},{position.x},{position.y}");
        writer.Flush();  // Ensure data is written to the file immediately
    }

    void OnApplicationQuit()
    {
        // Close the StreamWriter when the application quits
        //writer.Close();
    }
}
