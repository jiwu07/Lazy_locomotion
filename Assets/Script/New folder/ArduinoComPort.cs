using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System.IO;

public class ArduinoComPort : MonoBehaviour
{

    SerialPort serialPort;
    StreamWriter fileWriter;
    public string filePath;

    public static int ComPortNum = 4; // Should match the Com Port of the microcontroller
    SerialPort stream = new SerialPort("COM" + ComPortNum.ToString(), 115200);

    public static float Pace = 0f;
    public static float Phase = 0f;
    public static float a0 = 0f;
    public static float a1 = 0f;

    public static float time = 0f;
    public static float dt = 0.02f;

    int count = 0;

    // Start is called before the first frame update
    void Start()
    {
        stream.Open();

        //data writing
        fileWriter = new StreamWriter(filePath);
        fileWriter.WriteLine("Time,A0,A1,Pace,Phase");

    }

    // Update is called once per frame
    void Update()
    {
        if (stream.BytesToRead > 8)
        {
            Debug.Log("Stream Flushed!");
           // stream.ReadExisting();
        }
        string data = stream.ReadLine();
        string[] DataArr = data.Split(",");
        Debug.Log("Data: " + data);
        count++;
        time = count * dt;
        a0 = float.Parse(DataArr[0]);
        a1 = float.Parse(DataArr[1]);
        Pace = float.Parse(DataArr[2]);
        Phase = float.Parse(DataArr[3]);
        string line = $"{time},{a0},{a1},{Pace},{Phase}";
        fileWriter.WriteLine(line);
        //Debug.Log("Pace: " + Pace.ToString("N2") + ", Phase: " + Phase.ToString("N2"));
    }


    public void OnApplicationQuit()
    {
        stream.Close();
        stream.Dispose();
    }
}
