using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using System.Collections;
using System.Text;
using System.Security.Cryptography;
using System.Collections.Generic;

using JT.FWW.Tools;

namespace JT
{
    namespace FWW
    {
        namespace Tools
        {
			public enum MsgGCToGSToCS
			{
				GAME_PING = 1025,
				GAME_REGISTE = 1026,
			}
        }
    }
}