using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;


public class Controllerpractise : MonoBehaviour
{
    public GameObject text;
    public string nextSceneName = "MainTestScene";
    float count = 5.0f;
    public GameObject player;

    

    void Update()
    {
        if (count > 0)
        {
            count -= Time.deltaTime;

        }
        else
        {
            text.SetActive(false);
        }
    }

    void OnCollisionEnter(Collision collision)
    {
        //Debug.Log("next scene");
        if (collision.transform.gameObject.tag == "Player")
        {
            LoadNextScene();
        }
        
    }

    void LoadNextScene()
    {
        player.GetComponent<ArduinoComPort>().OnApplicationQuit();
        Animator animator = player.GetComponent<Animator>();
        animator.SetFloat("Forward", 0);
        animator.SetFloat("Pace", 0);
        SceneManager.LoadScene(nextSceneName);
    }



}
