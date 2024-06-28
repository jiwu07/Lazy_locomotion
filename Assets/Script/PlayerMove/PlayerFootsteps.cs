using UnityEngine;

public class PlayerFootsteps : MonoBehaviour
{
    public AudioClip leftFootstepSound; // Left footstep sound clip
    public AudioClip rightFootstepSound; // Right footstep sound clip
    public AudioSource audioSource; // AudioSource to play the sound

    void Start()
    {
        if (audioSource == null)
        {
            audioSource = gameObject.AddComponent<AudioSource>();
        }
    }

    // This method will be called from animation events for left footstep
    public void PlayLeftFootstepSound()
    {
        if (audioSource != null && leftFootstepSound != null )
        {
            audioSource.PlayOneShot(leftFootstepSound);
        }
    }

    // This method will be called from animation events for right footstep
    public void PlayRightFootstepSound()
    {
        if (audioSource != null && rightFootstepSound != null)
        {
            audioSource.PlayOneShot(rightFootstepSound);
        }
    }
}
