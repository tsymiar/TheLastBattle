using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using BlGame.GameEntity;

public class SceneSoundManager
{

    Dictionary<AudioSource, GameObject> sceneSound = new Dictionary<AudioSource, GameObject>();
    public static readonly SceneSoundManager Instance = new SceneSoundManager();



    public void addSound(AudioSource audioSource, GameObject obj)
    {
        sceneSound[audioSource] = obj;
    }
    public void remove(AudioSource audiosource)
    {
        audiosource.volume = 0.0f;
        sceneSound.Remove(audiosource);
        audiosource = null;
    }

    public void Update()
    {
        Vector3 AudioPos;
        Vector3 playerPos;
        List<AudioSource> removeList = new List<AudioSource>();
        foreach (KeyValuePair<AudioSource, GameObject> p in sceneSound)
        {
            if (p.Value != null && p.Key != null)
            {
                AudioPos = p.Value.transform.position;
              
                playerPos = Vector3.zero;
                if (PlayerManager.Instance.LocalPlayer != null && PlayerManager.Instance.LocalPlayer.RealEntity != null)
                {
                    playerPos = PlayerManager.Instance.LocalPlayer.RealEntity.transform.position;
                }
                else if (UIViewerPersonInfo.Instance != null && UIViewerPersonInfo.Instance.SetCurrClickPlayer != null)
                {
                    playerPos = UIViewerPersonInfo.Instance.SetCurrClickPlayer.RealEntity.transform.position;
                }

                float distance = Vector3.Distance(AudioPos, playerPos);
                if (distance > 30)
                {
                    distance = 30.0f;
                }
                p.Key.volume = (30.0f - distance) / 30.0f;
               
            }
            else
            {
                 removeList.Add(p.Key);
            }
        }
        foreach (AudioSource a in removeList)
        {
            remove(a);
        }
    }
}

