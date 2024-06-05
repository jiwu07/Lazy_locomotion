using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;

public class ArduinoComPort : MonoBehaviour
{
    public static int ComPortNum = 4; // Should match the Com Port of the microcontroller
    SerialPort stream = new SerialPort("COM" + ComPortNum.ToString(), 115200);

    public static float Pace = 0f;
    public static float Phase = 0f;

    // Start is called before the first frame update
    void Start()
    {
        stream.Open();

}

    // Update is called once per frame
    void Update()
    {
        if (stream.BytesToRead > 8)
        {
            Debug.Log("Stream Flushed!");
            stream.ReadExisting();
        }
        string data = stream.ReadLine();
        string[] DataArr = data.Split(",");
        Debug.Log("Data: " + data);
        Pace = float.Parse(DataArr[0]);
        Phase = float.Parse(DataArr[1]);
        //Debug.Log("Pace: " + Pace.ToString("N2") + ", Phase: " + Phase.ToString("N2"));
    }


    void OnApplicationQuit()
    {
        stream.Close();
        stream.Dispose();
    }
}
