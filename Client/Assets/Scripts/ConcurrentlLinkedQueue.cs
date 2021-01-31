using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;
using JT.FWW.Tools;
using System.Text;


public class ConcurrentLinkedQueue<T> 
{
    private class Node<K>
    {
        internal K Item;
        internal Node<K> Next;

        public Node(K item, Node<K> next)
        {
            this.Item = item;
            this.Next = next;
        }
    }

    private Node<T> _head;
    private Node<T> _tail;

    public ConcurrentLinkedQueue()
    {
        _head = new Node<T>(default(T), null);
        _tail = _head;
    }

    public bool IsEmpty
    {
        get { return (_head.Next == null); }
    }

    public void Enqueue(T item)
    {
        Node<T> newNode = new Node<T>(item, null);
        while (true)
        {
            Node<T> curTail = _tail;
            Node<T> residue = curTail.Next;

            //判断_tail是否被其他process改变
            if (curTail == _tail)
            {
                //A 有其他process执行C成功，_tail应该指向新的节点
                if (residue == null) 
                {
                    //C 如果其他process改变了tail.next节点，需要重新取新的tail节点
                    if (Interlocked.CompareExchange<Node<T>>(
                        ref curTail.Next, newNode, residue) == residue) 
                    {
                        //D 尝试修改tail
                        Interlocked.CompareExchange<Node<T>>(ref _tail, newNode, curTail); 
                        return;
                    }
                }
                else
                {
                    //B 帮助其他线程完成D操作
                    Interlocked.CompareExchange<Node<T>>(ref _tail, residue, curTail); 
                }
            }
        }
    }

    public bool TryDequeue(out T result)
    {
        Node<T> curHead;
        Node<T> curTail;
        Node<T> next;
        do
        {
            curHead = _head;
            curTail = _tail;
            next = curHead.Next;
            if (curHead == _head)
            {
                if (next == null)  //Queue为空
                {
                    result = default(T);
                    return false;
                }
                if (curHead == curTail) //Queue处于Enqueue第一个node的过程中
                {
                    //尝试帮助其他Process完成操作
                    Interlocked.CompareExchange<Node<T>>(ref _tail, next, curTail); 
                }
                else
                {
                    //取next.Item必须放到CAS之前
                    result = next.Item; 
                    //如果_head没有发生改变，则将_head指向next并退出
                    if (Interlocked.CompareExchange<Node<T>>(ref _head, 
                        next, curHead) == curHead)
                        break;
                }
            }
        }
        while (true);
        return true;
    }
}