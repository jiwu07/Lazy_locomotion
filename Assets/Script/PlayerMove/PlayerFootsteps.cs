using UnityEngine;

public class PlayerFootsteps : MonoBehaviour
{
    public AudioClip leftFootstepSound; 
    public AudioClip rightFootstepSound; 
    public AudioSource audioSource; 

    void Start()
    {
        if (audioSource == null)
        {
            audioSource = gameObject.AddComponent<AudioSource>();
        }
    }

    public void PlayLeftFootstepSound()
    {
        if (audioSource != null && leftFootstepSound != null )
        {
            audioSource.PlayOneShot(leftFootstepSound);
        }
    }

    public void PlayRightFootstepSound()
    {
        if (audioSource != null && rightFootstepSound != null)
        {
            audioSource.PlayOneShot(rightFootstepSound);
        }
    }
}
