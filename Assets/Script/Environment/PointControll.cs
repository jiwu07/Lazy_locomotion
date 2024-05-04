using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PointControll : MonoBehaviour
{
    private void OnTriggerEnter(Collider other)
    {
       //point disappear
            gameObject.SetActive(false);
        //play a sound ？
        
    }

    

}
