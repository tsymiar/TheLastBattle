using UnityEngine;
using System.Collections;
using Xft;

public class ExampleManager : MonoBehaviour 
{

    public XffectCache EffectCache;

    //loop xffet
    public CompositeXffect XLightBeams;
    protected bool ShowLightBeams = false;
    public XffectComponent XRaining;
    protected bool ShowRaining = false;
    public XftVolumeFogObject XVolumeFog;
    protected bool ShowVolumeFog = false;
    public XffectComponent XWaterfall;
    protected bool ShowWaterfall = false;
    public XffectComponent XPortalCone;
    protected bool ShowPortalCone = false;
    public XffectComponent XTadpoleGate;
    protected bool ShowTadpoleGate = false;
    public CompositeXffect XCrystalEnergy;
    protected bool ShowCrystalEnergy = false;
    public XffectComponent XSurroundSoul;
    protected bool ShowSurroundSoul = false;
    public XffectComponent XPhantomSwordSlash;
    protected bool ShowPhantomSwordSlash = false;
    public XffectComponent XPhantomSword;
    protected bool ShowPhantomSword = false;
    public XffectComponent XTransformSpell;
    protected bool ShowTransformSpell = false;
    public XffectComponent XSpreadSlash;
    protected bool ShowSpreadSlash = false;
    public XffectComponent XPinkSoul;
    protected bool ShowPinkSoul = false;
    public XffectComponent XSakura;
    protected bool ShowSakura = false;
    public CompositeXffect XWindowLight;
    protected bool ShowWindowLight = false;

    public Transform Colliders;
    public Transform CollisionGoal;
    //collision xffect
    public XffectComponent XCollisionTest1;
    public XffectComponent XCollisionTest2;

    //Missiles
    public XffectComponent XMissile1;
    public XffectComponent XMissile2;
    public XffectComponent XMissile3;
    public XffectComponent XMissile4;

    //Explode
    public XffectComponent XExplode1;
    public XffectComponent XExplode2;
    public CompositeXffect XExplode3;

    public Transform BackgroundWallBottom;

    //version 1.1.2
    public XffectComponent XBombAffector;


    public XffectComponent XLevelUp;

    public XffectComponent XIceImpact;
    protected bool ShowIceImpact = false;

    public CompositeXffect XDissolve;
    
    
    public XffectComponent XRadialEnergy;
    public XffectComponent XVolumetricLight1;
    protected bool ShowVolumetricLight1 = false;
    public XffectComponent XVolumetricLight2;
    
    public CompositeXffect XSuckBlood;
    protected bool ShowSuckBlood = false;
    
    protected bool ShowVolumetricLight2 = false;
    
    public XffectComponent Cyclone1;
    public XffectComponent Cyclone2;
    public XffectComponent Cyclone3;
    public XffectComponent DevilTrigger;
    
    protected bool ShowCyclone2 = false;
    protected bool ShowCyclone3 = false;
    protected bool ShowDevilTrigger = false;

    protected Vector2 ScrollPosition = Vector2.zero;
    protected float m_checkTime = 0f;
	
	void Update () 
    {

        if (ShowLightBeams)
            XLightBeams.Active();
        else
            XLightBeams.DeActive();
        if (ShowRaining)
            XRaining.Active();
        else
            XRaining.DeActive();

        if (ShowVolumeFog)
            XffectComponent.SetActive(XVolumeFog.gameObject,true);
        else
            XffectComponent.SetActive(XVolumeFog.gameObject,false);

        if (ShowWaterfall)
            XWaterfall.Active();
        else
            XWaterfall.DeActive();
        if (ShowPortalCone)
            XPortalCone.Active();
        else
            XPortalCone.DeActive();

        if (ShowTadpoleGate)
            XTadpoleGate.Active();
        else
            XTadpoleGate.DeActive();

        if (ShowCrystalEnergy)
            XCrystalEnergy.Active();
        else
            XCrystalEnergy.DeActive();

        if (ShowSurroundSoul)
            XSurroundSoul.Active();
        else
            XSurroundSoul.DeActive();

        if (ShowPhantomSwordSlash)
            XPhantomSwordSlash.Active();
        else
            XPhantomSwordSlash.DeActive();

        if (ShowPhantomSword)
            XPhantomSword.Active();
        else
            XPhantomSword.DeActive();

        if (ShowTransformSpell)
            XTransformSpell.Active();
        else
            XTransformSpell.DeActive();

        if (ShowSpreadSlash)
            XSpreadSlash.Active();
        else
            XSpreadSlash.DeActive();

        if (ShowPinkSoul)
            XPinkSoul.Active();
        else
            XPinkSoul.DeActive();

        if (ShowSakura)
            XSakura.Active();
        else
            XSakura.DeActive();

        if (ShowIceImpact)
            XIceImpact.Active();
        else
            XIceImpact.DeActive();

        if (ShowWindowLight)
            XWindowLight.Active();
        else
            XWindowLight.DeActive();
        
        if (ShowVolumetricLight1)
            XVolumetricLight1.Active();
        else
            XVolumetricLight1.DeActive();
        
        if (ShowVolumetricLight2)
            XVolumetricLight2.Active();
        else
            XVolumetricLight2.DeActive();
        
        if (ShowSuckBlood)
            XSuckBlood.Active();
        else
            XSuckBlood.DeActive();
        
        if (ShowCyclone2)
            Cyclone2.Active();
        else
            Cyclone2.DeActive();
        
        if (ShowCyclone3)
            Cyclone3.Active();
        else
            Cyclone3.DeActive();
        
        if (ShowDevilTrigger)
            DevilTrigger.Active();
        else
            DevilTrigger.DeActive();
	}

    void LateUpdate()
    {
        // if no collision needed, hide the colliders.
        if (!XffectComponent.IsActive(XCollisionTest1.gameObject) && !XffectComponent.IsActive(XCollisionTest2.gameObject))
        {
            if (Colliders.gameObject == true)
            {
                foreach (Transform child in Colliders)
                {
                    XffectComponent.SetActive(child.gameObject,false);
                }
            }
            XffectComponent.SetActive(Colliders.gameObject,false);
        }
        
        //auto deactive background
        m_checkTime += Time.deltaTime;
        if (m_checkTime > 5f)
        {
            XffectComponent.SetActive(BackgroundWallBottom.gameObject,false);
        }
    }

    void OnGUI()
    {
        GUI.Label(new Rect(150, 0, 350, 25), "left button to rotate, middle button to pan, wheel to zoom.");
        GUI.Label(new Rect(200, 18, 200, 25), "xffect pro version 2.5.0");

        ScrollPosition = GUI.BeginScrollView(new Rect(0, 0, 140, 600), ScrollPosition, new Rect(0, 0, 140, 750));
        ShowLightBeams = GUI.Toggle(new Rect(10, 0, 80, 20), ShowLightBeams, "LightBeams");
        ShowRaining = GUI.Toggle(new Rect(10, 20, 80, 20), ShowRaining, "Raining");
        ShowVolumeFog = GUI.Toggle(new Rect(10, 40, 80, 20), ShowVolumeFog, "VolumeFog");
        ShowWaterfall = GUI.Toggle(new Rect(10, 60, 80, 20), ShowWaterfall, "Waterfall");
        ShowPortalCone = GUI.Toggle(new Rect(10, 100, 80, 20), ShowPortalCone, "PortalCone");
        ShowTadpoleGate = GUI.Toggle(new Rect(10, 120, 80, 20), ShowTadpoleGate, "TadpoleGate");
        ShowCrystalEnergy = GUI.Toggle(new Rect(10, 140, 120, 20), ShowCrystalEnergy, "CrystalEnergy");
        ShowSurroundSoul = GUI.Toggle(new Rect(10, 160, 120, 20), ShowSurroundSoul, "SwingAround");
  
        if (GUI.Button(new Rect(10, 80, 120, 20), "RadialEnergy"))
        {
            XffectComponent.SetActive(BackgroundWallBottom.gameObject,true);
            m_checkTime = 0f;
            XRadialEnergy.Active();
        }
        
        
        if (GUI.Button(new Rect(10, 190, 80, 20), "collision1"))
        {
            XCollisionTest1.Active();
            //show colliders.
            foreach (Transform child in Colliders)
            {
                XffectComponent.SetActive(child.gameObject,true);
            }
            XffectComponent.SetActive(Colliders.gameObject,true);
        }

        if (GUI.Button(new Rect(10, 210, 80, 20), "collision2"))
        {
            XCollisionTest2.Active();
            //set collision goal.
            XCollisionTest2.SetCollisionGoalPos(CollisionGoal);
            //show colliders.
            foreach (Transform child in Colliders)
            {
                XffectComponent.SetActive(child.gameObject,true);
            }
            XffectComponent.SetActive(Colliders.gameObject,true);
        }

        if (GUI.Button(new Rect(10, 240, 80, 20), "missile1"))
        {
            XMissile1.Active();
            XMissile1.transform.position = new Vector3(0, 0, 50);
            SimpleMissile smile = XMissile1.transform.GetComponent<SimpleMissile>();
            smile.Reset();
        }
        if (GUI.Button(new Rect(10, 260, 80, 20), "missile2"))
        {
            XMissile2.Active();
            XMissile2.transform.position = new Vector3(0, 0, 40);
            SimpleMissile smile = XMissile2.transform.GetComponent<SimpleMissile>();
            smile.Reset();
        }
        if (GUI.Button(new Rect(10, 280, 80, 20), "missile3"))
        {
            XMissile3.Active();
            XMissile3.transform.position = new Vector3(0, 0, 40);
            SimpleMissile smile = XMissile3.transform.GetComponent<SimpleMissile>();
            smile.Reset();
        }
        if (GUI.Button(new Rect(10, 300, 80, 20), "missile4"))
        {
            XMissile4.transform.position = new Vector3(0, 0, 40);
            XMissile4.Active();
            SimpleMissile smile = XMissile4.transform.GetComponent<SimpleMissile>();
            smile.Reset();
        }


        if (GUI.Button(new Rect(10, 330, 80, 20), "explosion1"))
        {
            XffectComponent.SetActive(BackgroundWallBottom.gameObject,true);
            m_checkTime = 0f;
            XExplode1.Active();
        }
        if (GUI.Button(new Rect(10, 350, 80, 20), "explosion2"))
        {
            XExplode2.Active();
        }
        if (GUI.Button(new Rect(10, 370, 80, 20), "explosion3"))
        {
            XExplode3.Active();
        }

        if (GUI.Toggle(new Rect(10, 390, 120, 20), ShowPhantomSwordSlash, "PhantomSlash"))
        {
            ShowPhantomSwordSlash = true;
            m_checkTime = 0f;
            XffectComponent.SetActive(BackgroundWallBottom.gameObject,true);
        }
        else
        {
            ShowPhantomSwordSlash = false;
        }
        ShowPhantomSword = GUI.Toggle(new Rect(10, 410, 120, 20), ShowPhantomSword, "PhantomSword");

        ShowTransformSpell = GUI.Toggle(new Rect(10, 430, 120, 20), ShowTransformSpell, "TransformSpell");

        if (GUI.Button(new Rect(10, 450, 100, 20), "BombAffector"))
        {
            XBombAffector.Active();
        }
        //ver 1.2.1
        ShowSpreadSlash = GUI.Toggle(new Rect(10, 470, 120, 20), ShowSpreadSlash, "SpreadSlash");
        ShowPinkSoul = GUI.Toggle(new Rect(10, 490, 120, 20), ShowPinkSoul, "PinkSoul");
        ShowSakura = GUI.Toggle(new Rect(10, 510, 120, 20), ShowSakura, "Sakura");

        //ver 1.2.2
        if (GUI.Button(new Rect(10, 530, 100, 20), "LevelUp"))
        {
            XLevelUp.Active();
        }

        //ver 1.2.3
        ShowIceImpact = GUI.Toggle(new Rect(10, 550, 120, 20), ShowIceImpact, "IceImpact");


        //ver 1.3.1
        if (GUI.Button(new Rect(10, 570, 80, 20), "Dissolve"))
        {
            XffectComponent.SetActive(BackgroundWallBottom.gameObject,true);
            m_checkTime = 0f;
            XDissolve.Active();
        }

        ShowWindowLight = GUI.Toggle(new Rect(10, 590, 120, 20), ShowWindowLight, "WindowLight");
        ShowVolumetricLight1 = GUI.Toggle(new Rect(10, 610, 120, 20), ShowVolumetricLight1, "VolumetricLight1");
        ShowVolumetricLight2 = GUI.Toggle(new Rect(10, 630, 120, 20), ShowVolumetricLight2, "VolumetricLight2");
        
        ShowSuckBlood = GUI.Toggle(new Rect(10, 650, 120, 20), ShowSuckBlood, "SuckBlood");
        
        if (GUI.Button(new Rect(10, 670, 120, 20), "Cyclone1"))
        {
            Cyclone1.Active();
        }
        
        ShowCyclone2 = GUI.Toggle(new Rect(10, 690, 120, 20), ShowCyclone2, "Cyclone2");
        ShowCyclone3 = GUI.Toggle(new Rect(10, 710, 120, 20), ShowCyclone3, "Cyclone3");
        ShowDevilTrigger = GUI.Toggle(new Rect(10, 730, 120, 20), ShowDevilTrigger, "Devil Trigger");

        GUI.EndScrollView();
    }

    //collisions
    void OnSpreadHit(Xft.CollisionParam param)
    {
        EffectCache.ReleaseEffect("exp_small", param.CollidePos);
    }

    void OnConcentrateHit(Xft.CollisionParam param)
    {
        EffectCache.ReleaseEffect("exp_small", param.CollidePos);
    }
}
