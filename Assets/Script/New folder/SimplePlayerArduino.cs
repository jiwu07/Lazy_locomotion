using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SimplePlayerArduino : MonoBehaviour
{
    Animator animator;
    float spd;
    float phase;
    float offset;
    GameObject CamCarr;
    Vector3 CamPos;
    int Mode = 0;
    public bool UsingComPort = false;
    float w = 1f;
    float forw = 0f;

    // Use this for initialization
    void Start()
    {
        spd = 0;
        phase = 0;
        offset = -0.4f;
        animator = GetComponent<Animator>();
        //CamCarr = transform.Find("CamCarrier").gameObject;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        if (UsingComPort)
        {
            float PhDiff = ArduinoComPort.Phase - phase;
            forw = forw*(1-w) + PhDiff/Time.deltaTime*w;
            animator.SetFloat("Forward", forw);
            animator.SetFloat("Pace", ArduinoComPort.Pace*4);

            phase = (phase + forw * Time.deltaTime);
            //Debug.Log("Pace: " + ArduinoComPort.Pace.ToString());
            Debug.Log("Phase:  " + ArduinoComPort.Phase.ToString("N3")+ ", phsDiff:  " + PhDiff.ToString("N3") + ", Forw:  " + forw.ToString("N3") + ", PhaseCalc:  " + phase.ToString("N3"));
            Debug.Log("DeltaTime:  " + Time.deltaTime);

            /*
            if (Input.GetAxis("Vertical")>0)
            {
                offset = offset + 0.01f;
            }
            else if (Input.GetAxis("Vertical") < 0)
            {
                offset = offset - 0.01f;
            }
            */
            animator.SetFloat("Phase", phase);
            //float h = Input.GetAxis("Horizontal");
            //animator.SetFloat("Turn", h);
            //ofst = Saturation((ofst + Input.GetAxis("Vertical") * 0.01f));
            animator.SetFloat("Offset", offset);
        }
        else
        {
           // float h = Input.GetAxis("Horizontal");
            //float v = Input.GetAxis("Vertical");
            //Debug.Log("Speed: " + v);
            //animator.SetFloat("Turn", h);
            //animator.SetFloat("Speed", v * 2);
            //animator.SetFloat("Forward", v);
        }

    }

    void OnCollisionEnter(Collision col)
    {
        if (col.gameObject.CompareTag("Enemy"))
        {
            animator.SetTrigger("Die");
        }
    }

    float Saturation(float a)
    {
        if (a < 0)
            a = 0f;
        else if (a > 1)
            a = 1f;
        return a;
    }

}
