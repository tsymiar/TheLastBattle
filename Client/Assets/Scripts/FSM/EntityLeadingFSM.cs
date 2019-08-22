namespace BlGame.FSM
{
	using BlGame.GameEntity;
    using UnityEngine;

    public class EntityLeadingFSM : EntityFSM
	{
        public static readonly EntityFSM Instance = new EntityLeadingFSM();
		
		public FsmState State
		{
			get
            {
                return FsmState.FSM_STATE_LEADING;
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
            entity.OnEntityLeadingSkill();
		}
		
		public void Execute(Ientity entity)
        {

		}
		
		public void Exit(Ientity entity){
			
		}
	}
}

