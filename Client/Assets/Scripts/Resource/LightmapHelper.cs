using UnityEngine;
using System.Collections;

namespace BlGame.Resource
{
    public class LightmapHelper : MonoBehaviour
    {
        public int lightmapIndex;
        public float x;
        public float y;
        public float z;
        public float w;
        void Start()
        {
            MeshRenderer render = gameObject.GetComponent<MeshRenderer>();
            if (render != null)
            {
                render.lightmapIndex = lightmapIndex;
                render.lightmapTilingOffset = new Vector4(x, y, z, w);
            }
        }
    }
}
