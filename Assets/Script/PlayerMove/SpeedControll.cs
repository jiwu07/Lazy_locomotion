using UnityEngine;
using System.IO;
using System.IO.Ports;

using TMPro;
using UnityEditor.Animations;

public class SpeedControll : MonoBehaviour
{
    
    SerialPort serialPort;
    StreamWriter fileWriter;
    public string filePath;

    string portName = "COM4"; // Adjust the port name accordingly
    int baudRate = 9600; // Match with the baud rate set in Arduino

    float timeInterval = 0.02f;
    public int V = 0;
    //public int isJump = 0;
    private Animator animator;
    float offset = 0.6F;
    int  count = 0;

    //UI
    //public TextMeshProUGUI speedText;

    void Start()
    {
        //data reading
        serialPort = new SerialPort(portName, baudRate);
        serialPort.Open();

        //data writing
        fileWriter = new StreamWriter(filePath);
        fileWriter.WriteLine("Time,A0,A1,V"); // Adjust column headers as needed

        //animation
        animator = GetComponent<Animator>();
        animator.speed = 1f;

    }



    void Update()
    {
      
       string data = serialPort.ReadLine(); // Read data from serial port
       string[] dataArray = data.Split(',');

       // data consists of time, sensor1 data, and sensor2 data, velocity, isjump
       float time =count  * timeInterval;
	count ++; 
       int A0 = int.Parse(dataArray[0]) ; 
       int A1 = int.Parse(dataArray[1]) ; 
       V = int.Parse(dataArray[2]);
       

       //save in csv file
       string line = $"{time},{A0},{A1},{V}";
       fileWriter.WriteLine(line);

        
       //playwith the animation of avatar
       float speed = V  ; //map the speed to normal speed
       animator.SetFloat("Speed", speed);
        float f = 1;// V /40f; //map the speed to normal speed,
       animator.SetFloat("f", f);


        
      //controll the move speed of avatar
      transform.Translate(Vector3.forward * speed/5 * offset  * Time.deltaTime );

        //show on the UI
       // speedText.text = "Speed: " + speed.ToString();
      
    }

    
    
}
