

namespace BlGame.FSM
{
    using BlGame.GameEntity;
    class EntitySingFSM : EntityFSM
    {
        public static readonly EntitySingFSM Instance = new EntitySingFSM();
        public FsmState State
        {
            get
            {
                return FsmState.FSM_STATE_RUN;
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
            entity.OnEnterSing();
        }

        public void Execute(Ientity entity)
        {
        }

        public void Exit(Ientity entity)
        {
            entity.OnExitSing();
        }
    }
}
