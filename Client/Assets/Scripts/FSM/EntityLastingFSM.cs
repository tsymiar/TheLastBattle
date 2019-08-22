namespace BlGame.FSM
{
	using BlGame.GameEntity;
    using UnityEngine;

    public class EntityLastingFSM : EntityFSM
	{
        public static readonly EntityFSM Instance = new EntityLastingFSM();
		
		public FsmState State
		{
			get
            {
                return FsmState.FSM_STATE_LASTING;
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
            entity.OnEntityLastingSkill();
		}
		
		public void Execute(Ientity entity)
        {

		}
		
		public void Exit(Ientity entity){
			
		}
	}
}

