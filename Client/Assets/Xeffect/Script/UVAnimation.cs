//----------------------------------------------
//            Xffect Editor
// Copyright © 2012- Shallway Studio
// http://shallway.net
//----------------------------------------------

using UnityEngine;
using System.Collections;
using System.Text;
using System.IO;
using Xft;

namespace Xft
{

    public struct EanFrame
    {
        public ushort X;
        public ushort Y;
        public ushort Width;
        public ushort Height;
    }

    public class EanAnimation
    {
        public int Offset;
        public int FrameCount;   //always = TotalCount ?
        public int MipWidth;      //always = CellWidth / 2 ?
        public int MipHeight;     //always = CellHeight / 2 ?
        public int StartX;
        public int StartY;
        public ushort TileCount;
        public ushort TotalCount;
        public ushort CellWidth;
        public ushort CellHeight;
        public EanFrame[] Frames;


        public void Load(BinaryReader br, FileStream fs)
        {
            Offset = br.ReadInt32();
            Offset += 16;
            FrameCount = br.ReadInt32();
            MipWidth = br.ReadInt32();
            MipHeight = br.ReadInt32();
            StartX = br.ReadInt32();
            StartY = br.ReadInt32();
            TileCount = br.ReadUInt16();
            TotalCount = br.ReadUInt16();
            CellWidth = br.ReadUInt16();
            CellHeight = br.ReadUInt16();
            Frames = new EanFrame[TotalCount];
            long curPos = fs.Position;
            fs.Seek((long)Offset, SeekOrigin.Begin);
            for (int i = 0; i < TotalCount; i++)
            {
                Frames[i].X = br.ReadUInt16();
                Frames[i].Y = br.ReadUInt16();
                Frames[i].Width = br.ReadUInt16();
                Frames[i].Height = br.ReadUInt16();
            }
            fs.Seek((long)curPos, SeekOrigin.Begin);
        }
    }
    class EanFile
    {
        public int Header;
        public int Version;
        public int Reserved;
        public int AnimCount;

        public EanAnimation[] Anims;

        public void Load(BinaryReader br, FileStream fs)
        {
            Header = br.ReadInt32();
            Version = br.ReadInt32();
            Reserved = br.ReadInt32();
            AnimCount = br.ReadInt32();

            Anims = new EanAnimation[AnimCount];

            for (int i = 0; i < AnimCount; i++)
            {
                Anims[i] = new EanAnimation();
                Anims[i].Load(br, fs);
            }
        }
    }



    public class UVAnimation
    {
        public Vector2[] frames;
        public Vector2[] UVDimensions;

        // Animation state vars:
        public int curFrame = 0;
        protected int stepDir = 1;
        //fixed ver 2.1.0, should be reset when finished.
        public int numLoops = 0;

        public string name;
        public int loopCycles = 0;                      // How many times to loop the animation (-1 loop infinitely)
        public bool loopReverse = false;

        public void Reset()
        {
            curFrame = 0;
            stepDir = 1;
            numLoops = 0;
        }

        // Sets the stepDir to -1 and sets the current frame to the end
        // so that the animation plays in reverse
        public void PlayInReverse()
        {
            stepDir = -1;
            curFrame = frames.Length - 1;
        }

        // Stores the UV of the next frame in 'uv', returns false if
        // we've reached the end of the animation (this will never
        // happen if it is set to loop infinitely)
        public bool GetNextFrame(ref Vector2 uv, ref Vector2 dm)
        {
            // See if we can advance to the next frame:
            if ((curFrame + stepDir) >= frames.Length || (curFrame + stepDir) < 0)
            {
                // See if we need to loop (if we're reversing, we don't loop until we get back to the beginning):
                if (stepDir > 0 && loopReverse)
                {
                    stepDir = -1;   // Reverse playback direction
                    curFrame += stepDir;

                    uv = frames[curFrame];
                    dm = UVDimensions[curFrame];
                }
                else
                {
                    // See if we can loop:
                    if (numLoops + 1 > loopCycles && loopCycles != -1)
                        return false;
                    else
                    {   // Loop the animation:
                        ++numLoops;
                        if (loopReverse)
                        {
                            stepDir *= -1;
                            curFrame += stepDir;
                        }
                        else
                            curFrame = 0;

                        uv = frames[curFrame];
                        dm = UVDimensions[curFrame];
                    }
                }
            }
            else
            {
                curFrame += stepDir;
                uv = frames[curFrame];
                dm = UVDimensions[curFrame];
            }
            return true;
        }


        public void BuildFromFile(string path, int index, float uvTime, Texture mainTex)
        {
            if (!File.Exists(path))
            {
                Debug.LogError("wrong ean file path!");
                return;
            }
            FileStream fs = new FileStream(path, FileMode.Open);
            BinaryReader br = new BinaryReader(fs);
            EanFile ean = new EanFile();
            ean.Load(br, fs);
            fs.Close();
            EanAnimation eanim = ean.Anims[index];
            frames = new Vector2[eanim.TotalCount];
            UVDimensions = new Vector2[eanim.TotalCount];
            int cols = eanim.TileCount;
            int rows = (eanim.TotalCount + cols - 1) / cols;
            int cellCount = 0;


            int tWidth = mainTex.width;
            int tHeight = mainTex.height;

            //Vector2 Oft = new Vector2((float)eanim.MipWidth / tWidth, (float)eanim.MipHeight / tHeight);

            for (int row = 0; row < rows; ++row)
            {
                for (int col = 0; col < cols && cellCount < eanim.TotalCount; ++col)
                {
                    Vector2 cellSize = Vector2.zero;
                    cellSize.x = (float)eanim.Frames[cellCount].Width / tWidth;
                    cellSize.y = (float)eanim.Frames[cellCount].Height / tHeight;
                    frames[cellCount].x = (float)eanim.Frames[cellCount].X / tWidth;
                    frames[cellCount].y = 1f - (float)eanim.Frames[cellCount].Y / tHeight;
                    UVDimensions[cellCount] = cellSize;
                    UVDimensions[cellCount].y = -UVDimensions[cellCount].y;
                    ++cellCount;
                }
            }
        }


        //top-left to bottom right.
        public Vector2[] BuildUVAnim(Vector2 start, Vector2 cellSize, int cols, int rows, int totalCells)
        {
            int cellCount = 0;
            frames = new Vector2[totalCells];
            UVDimensions = new Vector2[totalCells];
            frames[0] = start;
            for (int row = 0; row < rows; ++row)
            {
                for (int col = 0; col < cols && cellCount < totalCells; ++col)
                {
                    frames[cellCount].x = start.x + cellSize.x * ((float)col);
                    frames[cellCount].y = start.y + cellSize.y * ((float)row);
                    UVDimensions[cellCount] = cellSize;
                    //UVDimensions[cellCount].y = -UVDimensions[cellCount].y;
                    ++cellCount;
                }
            }
            return frames;
        }
        public void SetAnim(Vector2[] anim)
        {
            frames = anim;
        }
    }
}