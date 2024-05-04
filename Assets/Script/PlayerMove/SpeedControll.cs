using UnityEngine;
using System.IO;
using System.IO.Ports;

using TMPro;
using UnityEditor.Animations;

public class SpeedControll : MonoBehaviour
{
    
    SerialPort serialPort;
    StreamWriter fileWriter;

    string portName = "COM5"; // Adjust the port name accordingly
    int baudRate = 9600; // Match with the baud rate set in Arduino
    public string filePath ;

    float timeInterval = 0.02f;
    public int V = 0;
    //public int isJump = 0;
    private Animator animator;
    float stepLength = 0.8F;
    int  count = 0;

    //UI
    //public TextMeshProUGUI speedText;

    void Start()
    {
        //data reading
        serialPort = new SerialPort(portName, baudRate);
        //serialPort.Open();

        //data writing
        fileWriter = new StreamWriter(filePath);
        fileWriter.WriteLine("Time,A0,A1,V, isJump"); // Adjust column headers as needed

        //animation
        animator = GetComponent<Animator>();
        Debug.Log(animator);
        animator.speed = 1f;

    }



    void Update()
    {
      /*
       string data = serialPort.ReadLine(); // Read data from serial port
       string[] dataArray = data.Split(',');

       // data consists of time, sensor1 data, and sensor2 data, velocity, isjump
       float time =count  * timeInterval;
	   count ++; 
       int A0 = int.Parse(dataArray[0]) ; 
       int A1 = int.Parse(dataArray[1]) ; 
       V = int.Parse(dataArray[2]);
       isJump = int.Parse(dataArray[3]);

       //save in csv file
       string line = $"{time},{A0},{A1},{V},{isJump}";
       fileWriter.WriteLine(line);

        */
       //playwith the animation of avatar
       float speed = V /6.0f ; //map the speed to normal speed
       animator.SetFloat("Speed", speed);
       float f = V /20f; //map the speed to normal speed,normal walking as speed 4
       animator.SetFloat("f", f);
        
       //controll the move speed of avatar
       transform.Translate(Vector3.forward * speed * stepLength* Time.deltaTime );

       //jump try
       /* AnimatorStateInfo stateInfo = animator.GetCurrentAnimatorStateInfo(0);
       if (isJump == 1 && !animator.GetBool("JumpAir") )
       {
            animator.SetBool("JumpAir", true);
            isJump = 0;
       }*/

        //show on the UI
       // speedText.text = "Speed: " + speed.ToString();
      
    }

    
    
}
