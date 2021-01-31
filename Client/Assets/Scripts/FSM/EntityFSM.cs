namespace BlGame.FSM
{
	using UnityEngine;
	using BlGame.GameEntity;
	using System.Collections;
	
	public interface EntityFSM
	{
		bool CanNotStateChange{set;get;}
		FsmState State { get; }
		void Enter(Ientity entity , float stateLast);
		bool StateChange(Ientity entity , EntityFSM state);
		void Execute(Ientity entity);
		void Exit(Ientity Ientity);
	}
} 
