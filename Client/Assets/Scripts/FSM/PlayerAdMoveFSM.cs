namespace BlGame.FSM
{
    using BlGame.GameEntity;
    using UnityEngine;

    public class PlayerAdMoveFSM : EntityFSM
    {
        public static readonly EntityFSM Instance = new PlayerAdMoveFSM();

        public FsmState State
        {
            get
            {
                return FsmState.FSM_STATE_ADMOVE;
            }
        }

        public bool CanNotStateChange
        {
            set;
            get;
        }

        public bool StateChange(Ientity entity, EntityFSM fsm)
        {
            return CanNotStateChange;
        }

        public void Enter(Ientity entity, float last)
        {
            entity.OnEnterEntityAdMove();
        }

        public void Execute(Ientity entity)
        {
            entity.OnExecuteEntityAdMove();
        }

        public void Exit(Ientity entity)
        {

        }
    }
}

