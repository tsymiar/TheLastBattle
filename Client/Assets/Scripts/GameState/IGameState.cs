using UnityEngine;
using System.Collections;

namespace BlGame.GameState
{
    public interface IGameState
    {
        GameStateType GetStateType();
        void SetStateTo(GameStateType gsType);
        void Enter();
        GameStateType Update(float fDeltaTime);
        void FixedUpdate(float fixedDeltaTime);
        void Exit();
    }
}
