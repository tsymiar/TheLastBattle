namespace BlGame.FSM
{
	using BlGame.GameEntity;
    using UnityEngine;

    public class EntityIdleFSM : EntityFSM
	{
        public static readonly EntityFSM Instance = new EntityIdleFSM();
		
		public FsmState State
		{
			get
            {
                return FsmState.FSM_STATE_IDLE;
			}
		}
		
		public bool CanNotStateChange{
			set;get;
		}
		
		public bool StateChange(Ientity entity , EntityFSM fsm)
        {
			return CanNotStateChange;
		}
		
		public void Enter(Ientity entity , float last)
        {
            entity.OnEnterIdle();
		}
		
		public void Execute(Ientity entity)
        {
            if (EntityStrategyHelper.IsTick(entity, 3.0f))
            {
                entity.OnFSMStateChange(EntityFreeFSM.Instance);
            }
		}
		
		public void Exit(Ientity entity){
			
		}
	}
}

