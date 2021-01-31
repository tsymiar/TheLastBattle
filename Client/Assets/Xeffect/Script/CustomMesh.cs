using UnityEngine;
using System.Collections;
using Xft;
using System.Collections.Generic;

namespace Xft
{
    public class CustomMesh
    {
        protected VertexPool.VertexSegment Vertexsegment;
        
        
        public Mesh MyMesh;
        
        public Vector3[] MeshVerts;

        public Color MyColor;
        public Vector3 MyPosition = Vector3.zero;
        public Vector2 MyScale = Vector2.one;
        public Quaternion MyRotation = Quaternion.identity;
        public  Vector3 MyDirection;
        
        
        Matrix4x4 LocalMat;
        Matrix4x4 WorldMat;
        float Fps = 0.016f;
        float ElapsedTime = 0f;
        
       // EffectNode Owner;

        public bool ColorChanged = false;
        
        
        public CustomMesh(VertexPool.VertexSegment segment, Mesh mesh,Vector3 dir, float maxFps,EffectNode owner)
        {
   
            MyMesh = mesh;
            
            //Owner = owner;
            
            MeshVerts =  new Vector3[mesh.vertices.Length];
            
            mesh.vertices.CopyTo(MeshVerts,0);

            Vertexsegment = segment;
            MyDirection = dir;
            Fps = 1f / maxFps;
            SetColor(Color.white);
            SetPosition(Vector3.zero);
            SetScale(1f,1f);
            InitVerts();
        }
        
        
        public void SetColor(Color c)
        {
            MyColor = c;
            ColorChanged = true;
        }

        public void SetPosition(Vector3 pos)
        {
            MyPosition = pos;
        }
        
        public void SetScale(float width, float height)
        {
            MyScale.x = width;
            MyScale.y = height;
        }
        
        public void SetRotation(float angle)
        {
            MyRotation = Quaternion.AngleAxis(angle, Vector3.up);
        }
 
        public void InitVerts()
        {
            VertexPool pool = Vertexsegment.Pool;
            int index = Vertexsegment.IndexStart;
            int vindex = Vertexsegment.VertStart;

            for (int i = 0; i < MeshVerts.Length; i++)
            {
                pool.Vertices[vindex + i] = MeshVerts[i];
            }
            
            
            int[] indices = MyMesh.triangles;
            for (int i = 0; i < Vertexsegment.IndexCount; i++)
            {
                pool.Indices[i + index] = indices[i] + vindex;
            }
            
            Vector2[] uvs = MyMesh.uv;
            for (int i = 0; i < uvs.Length; i++)
            {
                pool.UVs[i + vindex] = uvs[i];
            }
            
            Color[] colors = MyMesh.colors;
            for (int i = 0; i < colors.Length; i++)
            {
                pool.Colors[i + vindex] = colors[i];
            }
        }
        
        public void UpdateColor()
        {
            VertexPool pool = Vertexsegment.Pool;
            int index = Vertexsegment.VertStart;
            for (int i = 0; i < Vertexsegment.VertCount; i++)
            {
                pool.Colors[index + i] = MyColor;
            }
            Vertexsegment.Pool.ColorChanged = true;
        }
        
        
        public void Transform()
        {
            
            Quaternion rot = Quaternion.FromToRotation(Vector3.up,MyDirection);
            
            Vector3 scale = Vector3.one;
            scale.x = scale.z = MyScale.x;
            scale.y = MyScale.y;

            LocalMat.SetTRS(Vector3.zero, rot * MyRotation, scale);

            WorldMat.SetTRS(MyPosition, Quaternion.identity, Vector3.one);
            Matrix4x4 mat = WorldMat * LocalMat;

            VertexPool pool = Vertexsegment.Pool;

            for (int i = Vertexsegment.VertStart; i < Vertexsegment.VertStart + Vertexsegment.VertCount; i++)
            {
                pool.Vertices[i] = mat.MultiplyPoint3x4(MeshVerts[i - Vertexsegment.VertStart]);
            }
            
        }
        

        public void Update(bool force,float deltaTime)
        {
            ElapsedTime += deltaTime;
            if (ElapsedTime > Fps || force)
            {
                Transform();
                if (ColorChanged)
                    UpdateColor();
                ColorChanged = false;
                if (!force)
                    ElapsedTime -= Fps;
            }
        }
    }
}

