using UnityEngine;
using System.Collections;

namespace BlGame.Resource
{
    //这是场景的环境光和雾效等设定
    public class EnviromentSetting : MonoBehaviour
    {
        public bool isFogOn;
        public Color fogColor;
        public FogMode fogMode;
        public float fogDensity;
        public float linearFogStart;
        public float linearFogEnd;
        public Color ambientLight;
        public Material skyboxMaterial;
        public float haloStrength;
        public float flareStrength;
        public float flareFadeSpeed;
        public Texture haloTexture;
        public Texture spotCookie;

        public void setValueByRenderSetting()
        {
            isFogOn = RenderSettings.fog;
            fogColor = RenderSettings.fogColor;
            fogMode = RenderSettings.fogMode;
            fogDensity = RenderSettings.fogDensity;
            linearFogStart = RenderSettings.fogStartDistance;
            linearFogEnd = RenderSettings.fogEndDistance;
            ambientLight = RenderSettings.ambientLight;
            skyboxMaterial = RenderSettings.skybox;
            haloStrength = RenderSettings.haloStrength;
            flareStrength = RenderSettings.flareStrength;
            flareFadeSpeed = RenderSettings.flareFadeSpeed;
        }

        // Use this for initialization
        void Start()
        {
            RenderSettings.fog = isFogOn;
            RenderSettings.fogColor = fogColor;
            RenderSettings.fogMode = fogMode;
            RenderSettings.fogDensity = fogDensity;
            RenderSettings.fogStartDistance = linearFogStart;
            RenderSettings.fogEndDistance = linearFogEnd;
            RenderSettings.ambientLight = ambientLight;
            RenderSettings.skybox = skyboxMaterial;
            RenderSettings.haloStrength = haloStrength;
            RenderSettings.flareStrength = flareStrength;
            RenderSettings.flareFadeSpeed = flareFadeSpeed;
            //RenderSettings.
        }

        // Update is called once per frame
        void Update()
        {

        }
    }
}
