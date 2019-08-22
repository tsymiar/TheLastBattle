namespace BlGame.FSM
{
	using BlGame.GameEntity;
    using UnityEngine;

	public class EntityFreeFSM : EntityFSM
	{
		public static readonly EntityFSM Instance = new EntityFreeFSM();
		
		public FsmState State
		{
			get{
				return FsmState.FSM_STATE_FREE;
			}
		}
		
		public bool CanNotStateChange{
			set;get;
		}
		
		public bool StateChange(Ientity entity , EntityFSM fsm){
			return CanNotStateChange;
		}
		
		public void Enter(Ientity entity , float last){
            entity.OnEnterFree();
		}
		
		public void Execute(Ientity entity)
        {
            //if (FSMHelper.ExecuteDeviation(entity))
            //{
                entity.OnExecuteFree();
            //}
		}
		
		public void Exit(Ientity entity){
			
		}
	}
}

