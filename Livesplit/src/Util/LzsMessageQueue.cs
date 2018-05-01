using System.Collections.Concurrent;

namespace LiveSplit.Lazysplits
{
    class LzsMessageQueue<T> : LzsObservable
    {
        public LzsMessageQueue( string subjectName ) : base(subjectName)
        {
            Queue = new ConcurrentQueue<T>();
        }
        
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
        public bool TryDequeue( out T data_out )
        {
            return Queue.TryDequeue( out data_out );
        }
        public bool TryPeek( out T data_out )
        {
            return Queue.TryPeek( out data_out);
        }
        public void Clear()
        {
            Queue = new ConcurrentQueue<T>();
        }

        private ConcurrentQueue<T> Queue;
    }
} //namespace LiveSplit.Lazysplits
