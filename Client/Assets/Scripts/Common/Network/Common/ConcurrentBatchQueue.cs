using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace SuperSocket.ClientEngine
{
    //////////////////////////////////////////////////////////////////////////

    public class MyBatchQueue<T> : IBatchQueue<T>
    {
        public object mSyncCs = new object();
        public List<T> mAllItems = new List<T>();

        public MyBatchQueue()
        {

        }
        /// <summary>
        /// Enqueues the specified item.
        /// </summary>
        /// <param name="item">The item.</param>
        public bool Enqueue(T item)
        {
            lock (mSyncCs)
            {
                mAllItems.Add(item);
            }
            return true;
        }

        /// <summary>
        /// Enqueues the specified items.
        /// </summary>
        /// <param name="items">The items.</param>
        public bool Enqueue(IList<T> items)
        {
            lock (mSyncCs)
            {
                mAllItems.AddRange(items);
            }
            return true;
        }

        /// <summary>
        /// Tries to dequeue all items in the queue into the output list.
        /// </summary>
        /// <param name="outputItems">The output items.</param>
        /// <returns></returns>
        public bool TryDequeue(IList<T> outputItems)
        {
            lock (mSyncCs)
            {
                foreach (T one in mAllItems)
                {
                    outputItems.Add(one);
                }
                mAllItems.Clear();
            }
            return outputItems.Count>0?true:false;
        }

        /// <summary>
        /// Gets a value indicating whether this instance is empty.
        /// </summary>
        /// <value>
        ///   <c>true</c> if this instance is empty; otherwise, <c>false</c>.
        /// </value>
        public bool IsEmpty 
        { 
            get { return mAllItems.Count<=0;}
        }

        /// <summary>
        /// Gets the count.
        /// </summary>
        public int Count
        {
            get { return mAllItems.Count; }
        }
    }
}
