﻿using System;
using System.Collections.Concurrent;

namespace LiveSplit.Lazysplits
{
    class LzsMessageQueue<T> : LzsObservable
    {
        public LzsMessageQueue()
        {
            Queue = new ConcurrentQueue<T>();
        }

        //wrapping our queue methods
        public void Enqueue( T data )
        {
            Queue.Enqueue(data);
            NotifyAll();
        }
        public int Count()
        {
            return Queue.Count;
        }
        public bool IsEmpty()
        {
            return Queue.IsEmpty;
        }
        //unsure about chaining outs here
        public bool TryDequeue( out T data_out )
        {
            return Queue.TryDequeue( out data_out );
        }
        public bool TryPeek( out T data_out )
        {
            return Queue.TryPeek( out data_out);
        }

        private ConcurrentQueue<T> Queue;
    }
} //namespace LiveSplit.Lazysplits
