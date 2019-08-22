using UnityEngine;
using System.Collections;
using System.Text;

public class UIChat : MonoBehaviour {

    void OnEnable()
    {
        Input = this.transform.FindChild("Input").GetComponent<UIInput>();
        ButtonOnPress sendBtn = this.transform.FindChild("SendMsg").GetComponent<ButtonOnPress>();
        sendBtn.AddListener(OnSendMsg);
    }

    private UIInput Input;

    void OnDisable()
    {

    }

    private void OnSendMsg(int ie , bool pressed)
    {
        if (pressed)
        {
            return;
        }
        if (Input != null)
        {
            //byte[] utf8Bytes = Encoding.UTF8.GetBytes(Input.value);
            if (Input.value == "debug")
            {
                DebugManager.Switch();
            }
            else
            {
                CGLCtrl_GameLogic.Instance.EmsgToss_eMsgToGSToCSFromGC_GMCmd(Input.value);
            }

            Input.value = "";
        }
    }
}
