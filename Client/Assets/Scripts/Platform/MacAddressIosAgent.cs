using UnityEngine;
using System.Collections;
using System.Net.NetworkInformation;  
using System.Runtime.InteropServices;

public class MacAddressIosAgent
{
    //[DllImport("__Internal")]
    //public static extern string GetMacAddress();

    public static string GetMacAddressByDNet()
    {
        return "0";
        

        string physicalAddress = "";

        NetworkInterface[] nice = NetworkInterface.GetAllNetworkInterfaces();

        foreach (NetworkInterface adaper in nice)
        {

            Debug.Log(adaper.Description);

            if (adaper.Description == "en0")
            {
                physicalAddress = adaper.GetPhysicalAddress().ToString();
                break;
            }
            else
            {
                physicalAddress = adaper.GetPhysicalAddress().ToString();

                if (physicalAddress != "")
                {
                    break;
                };
            }
        }

        return physicalAddress;
    }  

}