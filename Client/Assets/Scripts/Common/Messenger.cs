/*
 * Advanced C# messenger by Ilya Suzdalnitski. V1.0
 * 
 * Based on Rod Hyde's "CSharpMessenger" and Magnus Wolffelt's "CSharpMessenger Extended".
 * 
 * Features:
 	* Prevents a MissingReferenceException because of a reference to a destroyed message handler.
 	* Option to log all messages
 	* Extensive error detection, preventing silent bugs
 * 
 * Usage examples:
 	1. Messenger.AddListener<GameObject>("prop collected", PropCollected);
 	   Messenger.Broadcast<GameObject>("prop collected", prop);
 	2. Messenger.AddListener<float>("speed changed", SpeedChanged);
 	   Messenger.Broadcast<float>("speed changed", 0.5f);
 * 
 * Messenger cleans up its evenTable automatically upon loading of a new level.
 * 
 * Don't forget that the messages that should survive the cleanup, should be marked with Messenger.MarkAsPermanent(string)
 * 
 */
 
//#define LOG_ALL_MESSAGES
//#define LOG_ADD_LISTENER
//#define LOG_BROADCAST_MESSAGE
#define REQUIRE_LISTENER
 
using System;
using System.Collections.Generic;
using UnityEngine;
 
static internal class Messenger {
 
	//Disable the unused variable warning
#pragma warning disable 0414
	//Ensures that the MessengerHelper will be created automatically upon start of the game.
//	static private MessengerHelper mMessengerHelper = ( new GameObject("MessengerHelper") ).AddComponent< MessengerHelper >();
#pragma warning restore 0414
 
	static public Dictionary<EGameEvent, Delegate> mEventTable = new Dictionary<EGameEvent, Delegate>();
 
	//Message handlers that should never be removed, regardless of calling Cleanup
	static public List< EGameEvent > mPermanentMessages = new List< EGameEvent > ();

	
	//Marks a certain message as permanent.
	static public void MarkAsPermanent(EGameEvent eventType) {
#if LOG_ALL_MESSAGES
		Debug.Log("Messenger MarkAsPermanent \t\"" + eventType + "\"");
#endif
 
		mPermanentMessages.Add( eventType );
	}
 
 
	static public void Cleanup()
	{
#if LOG_ALL_MESSAGES
		Debug.Log("MESSENGER Cleanup. Make sure that none of necessary listeners are removed.");
#endif
 
		List< EGameEvent > messagesToRemove = new List<EGameEvent>();
 
		foreach (KeyValuePair<EGameEvent, Delegate> pair in mEventTable) {
			bool wasFound = false;
 
			foreach (EGameEvent message in mPermanentMessages) {
				if (pair.Key == message) {
					wasFound = true;
					break;
				}
			}
 
			if (!wasFound)
				messagesToRemove.Add( pair.Key );
		}
 
		foreach (EGameEvent message in messagesToRemove) {
			mEventTable.Remove( message );
		}
	}
 
	static public void PrEGameEventEventTable()
	{
		Debug.Log("\t\t\t=== MESSENGER PrEGameEventEventTable ===");
 
		foreach (KeyValuePair<EGameEvent, Delegate> pair in mEventTable) {
			Debug.Log("\t\t\t" + pair.Key + "\t\t" + pair.Value);
		}
 
		Debug.Log("\n");
	}
 
    static public void OnListenerAdding(EGameEvent eventType, Delegate listenerBeingAdded) {
#if LOG_ALL_MESSAGES || LOG_ADD_LISTENER
		Debug.Log("MESSENGER OnListenerAdding \t\"" + eventType + "\"\t{" + listenerBeingAdded.Target + " -> " + listenerBeingAdded.Method + "}");
#endif
 
        if (!mEventTable.ContainsKey(eventType)) {
            mEventTable.Add(eventType, null );
        }
 
        Delegate d = mEventTable[eventType];
        if (d != null && d.GetType() != listenerBeingAdded.GetType()) {
            throw new ListenerException(string.Format("Attempting to add listener with inconsistent signature for event type {0}. Current listeners have type {1} and listener being added has type {2}", eventType, d.GetType().Name, listenerBeingAdded.GetType().Name));
        }
    }
 
    static public void OnListenerRemoving(EGameEvent eventType, Delegate listenerBeingRemoved) {
#if LOG_ALL_MESSAGES
		Debug.Log("MESSENGER OnListenerRemoving \t\"" + eventType + "\"\t{" + listenerBeingRemoved.Target + " -> " + listenerBeingRemoved.Method + "}");
#endif
 
        if (mEventTable.ContainsKey(eventType)) {
            Delegate d = mEventTable[eventType];
 
            if (d == null) {
                throw new ListenerException(string.Format("Attempting to remove listener with for event type \"{0}\" but current listener is null.", eventType));
            } else if (d.GetType() != listenerBeingRemoved.GetType()) {
                throw new ListenerException(string.Format("Attempting to remove listener with inconsistent signature for event type {0}. Current listeners have type {1} and listener being removed has type {2}", eventType, d.GetType().Name, listenerBeingRemoved.GetType().Name));
            }
        } else {
            throw new ListenerException(string.Format("Attempting to remove listener for type \"{0}\" but Messenger doesn't know about this event type.", eventType));
        }
    }
 
    static public void OnListenerRemoved(EGameEvent eventType) {
        if (mEventTable[eventType] == null) {
            mEventTable.Remove(eventType);
        }
    }
 
    static public void OnBroadcasting(EGameEvent eventType) {
#if REQUIRE_LISTENER
        if (!mEventTable.ContainsKey(eventType)) {
            //throw new BroadcastException(string.Format("Broadcasting message \"{0}\" but no listener found. Try marking the message with Messenger.MarkAsPermanent.", eventType));
        }
#endif
    }
 
    static public BroadcastException CreateBroadcastSignatureException(EGameEvent eventType) {
        return new BroadcastException(string.Format("Broadcasting message \"{0}\" but listeners have a different signature than the broadcaster.", eventType));
    }
 
    public class BroadcastException : Exception {
        public BroadcastException(string msg)
            : base(msg) {
        }
    }
 
    public class ListenerException : Exception {
        public ListenerException(string msg)
            : base(msg) {
        }
    }
 
	//No parameters
    static public void AddListener(EGameEvent eventType, Callback handler) {
        OnListenerAdding(eventType, handler);
        mEventTable[eventType] = (Callback)mEventTable[eventType] + handler;
    }
 
	//Single parameter
	static public void AddListener<T>(EGameEvent eventType, Callback<T> handler) {
        OnListenerAdding(eventType, handler);
        mEventTable[eventType] = (Callback<T>)mEventTable[eventType] + handler;
    }
 
	//Two parameters
	static public void AddListener<T, U>(EGameEvent eventType, Callback<T, U> handler) {
        OnListenerAdding(eventType, handler);
        mEventTable[eventType] = (Callback<T, U>)mEventTable[eventType] + handler;
    }
 
	//Three parameters
	static public void AddListener<T, U, V>(EGameEvent eventType, Callback<T, U, V> handler) {
        OnListenerAdding(eventType, handler);
        mEventTable[eventType] = (Callback<T, U, V>)mEventTable[eventType] + handler;
    }
	
	//Four parameters
	static public void AddListener<T, U, V, X>(EGameEvent eventType, Callback<T, U, V, X> handler) {
        OnListenerAdding(eventType, handler);
        mEventTable[eventType] = (Callback<T, U, V, X>)mEventTable[eventType] + handler;
    }
	
//	//four parameters
//	static public void AddListener<T, U, V>(EGameEvent eventType, Callback<T, U, V, X> handler) {
//        OnListenerAdding(eventType, handler);
//        mEventTable[eventType] = (Callback<T, U, V, X>)mEventTable[eventType] + handler;
//    }
//	
//	//five parameters
//	static public void AddListener<T, U, V>(EGameEvent eventType, Callback<T, U, V, X, Y> handler) {
//        OnListenerAdding(eventType, handler);
//        mEventTable[eventType] = (Callback<T, U, V, X, Y>)mEventTable[eventType] + handler;
//    }
//	
//	//six parameters
//	static public void AddListener<T, U, V>(EGameEvent eventType, Callback<T, U, V, X, Y, Z> handler) {
//        OnListenerAdding(eventType, handler);
//        mEventTable[eventType] = (Callback<T, U, V, X, X, Y, Z>)mEventTable[eventType] + handler;
//    }
 
	//No parameters
    static public void RemoveListener(EGameEvent eventType, Callback handler) {
        OnListenerRemoving(eventType, handler);   
        mEventTable[eventType] = (Callback)mEventTable[eventType] - handler;
        OnListenerRemoved(eventType);
    }
 
	//Single parameter
	static public void RemoveListener<T>(EGameEvent eventType, Callback<T> handler) {
        OnListenerRemoving(eventType, handler);
        mEventTable[eventType] = (Callback<T>)mEventTable[eventType] - handler;
        OnListenerRemoved(eventType);
    }
 
	//Two parameters
	static public void RemoveListener<T, U>(EGameEvent eventType, Callback<T, U> handler) {
        OnListenerRemoving(eventType, handler);
        mEventTable[eventType] = (Callback<T, U>)mEventTable[eventType] - handler;
        OnListenerRemoved(eventType);
    }
 
	//Three parameters
	static public void RemoveListener<T, U, V>(EGameEvent eventType, Callback<T, U, V> handler) {
        OnListenerRemoving(eventType, handler);
        mEventTable[eventType] = (Callback<T, U, V>)mEventTable[eventType] - handler;
        OnListenerRemoved(eventType);
    }
	
	//Four parameters
	static public void RemoveListener<T, U, V, X>(EGameEvent eventType, Callback<T, U, V, X> handler) {
        OnListenerRemoving(eventType, handler);
        mEventTable[eventType] = (Callback<T, U, V, X>)mEventTable[eventType] - handler;
        OnListenerRemoved(eventType);
    }
	
//	//Four parameters
//	static public void RemoveListener<T, U, V>(EGameEvent eventType, Callback<T, U, V, X> handler) {
//        OnListenerRemoving(eventType, handler);
//        mEventTable[eventType] = (Callback<T, U, V, X>)mEventTable[eventType] - handler;
//        OnListenerRemoved(eventType);
//    }
//	
//	//Five parameters
//	static public void RemoveListener<T, U, V>(EGameEvent eventType, Callback<T, U, V, X, Y> handler) {
//        OnListenerRemoving(eventType, handler);
//        mEventTable[eventType] = (Callback<T, U, V, X, Y>)mEventTable[eventType] - handler;
//        OnListenerRemoved(eventType);
//    }
//	
//	//Six parameters
//	static public void RemoveListener<T, U, V>(EGameEvent eventType, Callback<T, U, V, X, Y, Z> handler) {
//        OnListenerRemoving(eventType, handler);
//        mEventTable[eventType] = (Callback<T, U, V, X, Y, Z>)mEventTable[eventType] - handler;
//        OnListenerRemoved(eventType);
//    }
//	
	//No parameters
    static public void Broadcast(EGameEvent eventType) {
#if LOG_ALL_MESSAGES || LOG_BROADCAST_MESSAGE
		Debug.Log("MESSENGER\t" + System.DateTime.Now.ToString("hh:mm:ss.fff") + "\t\t\tInvoking \t\"" + eventType + "\"");
#endif
        OnBroadcasting(eventType);
 
        Delegate d;
        if (mEventTable.TryGetValue(eventType, out d)) {
            Callback callback = d as Callback;
 
            if (callback != null) {
                callback();
            } else {
                throw CreateBroadcastSignatureException(eventType);
            }
        }
    }
 
	//Single parameter
    static public void Broadcast<T>(EGameEvent eventType, T arg1) {
#if LOG_ALL_MESSAGES || LOG_BROADCAST_MESSAGE
		Debug.Log("MESSENGER\t" + System.DateTime.Now.ToString("hh:mm:ss.fff") + "\t\t\tInvoking \t\"" + eventType + "\"");
#endif
        OnBroadcasting(eventType);
 
        Delegate d;
        if (mEventTable.TryGetValue(eventType, out d)) {
            Callback<T> callback = d as Callback<T>;
 
            if (callback != null) {
                callback(arg1);
            } else {
                throw CreateBroadcastSignatureException(eventType);
            }
        }
	}
 
	//Two parameters
    static public void Broadcast<T, U>(EGameEvent eventType, T arg1, U arg2) {
#if LOG_ALL_MESSAGES || LOG_BROADCAST_MESSAGE
		Debug.Log("MESSENGER\t" + System.DateTime.Now.ToString("hh:mm:ss.fff") + "\t\t\tInvoking \t\"" + eventType + "\"");
#endif
        OnBroadcasting(eventType);
 
        Delegate d;
        if (mEventTable.TryGetValue(eventType, out d)) {
            Callback<T, U> callback = d as Callback<T, U>;
 
            if (callback != null) {
                callback(arg1, arg2);
            } else {
                throw CreateBroadcastSignatureException(eventType);
            }
        }
    }
 
	//Three parameters
    static public void Broadcast<T, U, V>(EGameEvent eventType, T arg1, U arg2, V arg3) {
#if LOG_ALL_MESSAGES || LOG_BROADCAST_MESSAGE
		Debug.Log("MESSENGER\t" + System.DateTime.Now.ToString("hh:mm:ss.fff") + "\t\t\tInvoking \t\"" + eventType + "\"");
#endif
        OnBroadcasting(eventType);
 
        Delegate d;
        if (mEventTable.TryGetValue(eventType, out d)) {
            Callback<T, U, V> callback = d as Callback<T, U, V>;
 
            if (callback != null) {
                callback(arg1, arg2, arg3);
            } else {
                throw CreateBroadcastSignatureException(eventType);
            }
        }
    }
	
	//Four parameters
    static public void Broadcast<T, U, V, X>(EGameEvent eventType, T arg1, U arg2, V arg3, X arg4) {
#if LOG_ALL_MESSAGES || LOG_BROADCAST_MESSAGE
#endif
        OnBroadcasting(eventType);
 
        Delegate d;
        if (mEventTable.TryGetValue(eventType, out d)) {
            Callback<T, U, V, X> callback = d as Callback<T, U, V, X>;
 
            if (callback != null) {
                callback(arg1, arg2, arg3, arg4);
            } else {
                throw CreateBroadcastSignatureException(eventType);
            }
        }
    }	
}
 /*
//This manager will ensure that the messenger's mEventTable will be cleaned up upon loading of a new level.
public sealed class MessengerHelper : MonoBehaviour {
	void Awake ()
	{
		DontDestroyOnLoad(gameObject);	
	}
 
	//Clean up mEventTable every time a new level loads.
	public void OnDisable() {
		Messenger.Cleanup();
	}
}
*/