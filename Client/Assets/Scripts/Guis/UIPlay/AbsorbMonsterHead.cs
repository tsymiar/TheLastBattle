using System;
using System.Collections.Generic;
using System.Collections;
using UnityEngine;
using GameDefine;
using JT.FWW.Tools;
using BlGame.GameData;
using BlGame.GameEntity;
using BlGame.Effect;
using System.Linq;
using BlGame.Resource;

public class AbsorbMonsterHead : MonoBehaviour
    {
        public static AbsorbMonsterHead Instance = null;
        void Awake()
        {
            Instance = this;
        }
        public UISprite head1
        {
            private set;
            get;
        }

        public UISprite head2
        {
            private set;
            get;
        }
        private UISprite bar1;
        private UISprite bar2;
        private UISprite cha1;
        private UISprite cha2;
        public GameObject ShowEffect
        {
            get;
            set;
        }
        public GameObject RemoveEffect
        {
            set;
            get;
        }
        void OnEnable()
        {
            Transform trans = transform.FindChild("Creep1");
            //creep1 = trans.gameObject;
            bar1 = trans.FindChild("Bar").gameObject.GetComponent<UISprite>();
            cha1 = trans.FindChild("Cha").gameObject.GetComponent<UISprite>();
            ButtonOnPress press = trans.GetComponent<ButtonOnPress>();
            trans = trans.FindChild("Head");
            head1 = trans.GetComponent<UISprite>();         
            press.AddListener(0, OnHeadPressFunc);


            //
            trans = transform.FindChild("Creep2");
            bar2 = trans.FindChild("Bar").gameObject.GetComponent<UISprite>();
            cha2 = trans.FindChild("Cha").gameObject.GetComponent<UISprite>();
            //creep2 = trans.gameObject;
            press = trans.GetComponent<ButtonOnPress>();
            trans = trans.FindChild("Head");
            head2 = trans.GetComponent<UISprite>();           
            press.AddListener(1, OnHeadPressFunc);
            //
            head1.enabled = false;
            head2.enabled = false;

        }
        private void OnHeadPressFunc(int ie, bool isDown)
        {
            if (isDown)
            {
                return;
            }
            if (ie == 0)
            {
                if (head1.spriteName != "")
                {
                    //CGLCtrl_GameLogic.Instance.EmsgToss_AskCancelAbsorbSkill((uint)PlayerManager.Instance.LocalPlayer.AbsorbMonsterType[0]);
                }
            }
            else if (ie == 1)
            {
                if (head2.spriteName != "")
                {
                    //CGLCtrl_GameLogic.Instance.EmsgToss_AskCancelAbsorbSkill((uint)PlayerManager.Instance.LocalPlayer.AbsorbMonsterType[1]);
                }
            }
        }
        public void setHeadImg(int index, string spriteName)
        {
            if (index == 0)
            {

                head1.spriteName = spriteName;
                if (spriteName == "")
                {
                    head1.enabled = false;
                    bar1.enabled = false;
                    cha1.enabled = false;
                }
                else
                {
                    head1.enabled = true;
                    bar1.enabled = true;
                    cha1.enabled = true;
                }
            }
            else if (index == 1)
            {
                head2.spriteName = spriteName;
                if (spriteName == "")
                {
                    head2.enabled = false;
                    bar2.enabled = false;
                    cha2.enabled = false;
                }
                else
                {
                    head2.enabled = true;
                    bar2.enabled = true;
                    cha2.enabled = true;
                }
            }
        }
      public  void SetShowEffect(Transform tran,string name)
        {
            if (tran.childCount != 0)
            {
                for (int i = 0; i < tran.childCount; i++)
                {
                    DestroyImmediate(tran.GetChild(i).gameObject);
                }
            }
            //GameObject obj = Resources.Load("effect/ui_effect/" + name) as GameObject;
            ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate("effect/ui_effect/" + name, ResourceType.PREFAB);
            GameObject obj = objUnit.Asset as GameObject;
          
            if (obj == null)
            {
                //Debug.LogError("Res Not Found:" + "effect/ui_effect/soul_01");
            }
            
            ShowEffect = GameObject.Instantiate(obj) as GameObject;
            ShowEffect.transform.parent = tran;
            ShowEffect.transform.localPosition = Vector3.zero;
            ShowEffect.transform.localScale = Vector3.one;
            ShowEffect = null;
            DestroyImmediate(ShowEffect);
        }
       public void DestoryEffect(Transform tran, string name)
        {
           if (tran.childCount != 0){
               for (int i = 0; i < tran.childCount; i++)
               {
                   DestroyImmediate(tran.GetChild(i).gameObject);
               }
            }
            //GameObject obj = Resources.Load("effect/ui_effect/" + name) as GameObject;
           ResourceUnit objUnit = ResourcesManager.Instance.loadImmediate("effect/ui_effect/" + name, ResourceType.PREFAB);
           GameObject obj = objUnit.Asset as GameObject;

            if (obj == null)
            {
//Debug.LogError("Res Not Found:" + "effect/ui_effect/soul_death");
            }
          
            RemoveEffect = GameObject.Instantiate(obj) as GameObject;
            RemoveEffect.transform.parent = tran;
            RemoveEffect.transform.localPosition = Vector3.zero;
            RemoveEffect.transform.localScale = Vector3.one;
        }
    }

