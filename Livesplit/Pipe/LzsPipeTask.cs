using System;

using System.Threading;
using System.Threading.Tasks;
using System.IO.Pipes;

using NLog;

namespace LiveSplit.Lazysplits.Pipe
{
    enum PipeTaskType { None, Connect, Read, Write };
    
    //interface for pipe tasks, to be able to add multiple task types to a single list in our task manager class
    interface IPipeTask
    {
        Task GetTask();
        PipeTaskType GetTaskType();
        bool StartTask();
        void HandleTaskResult();
    }
    
    abstract class PipeTaskBase : IPipeTask
    {
        public PipeTaskBase( PipeTaskType taskType, NamedPipeClientStream pipeStreamInstance )
        {
            TaskType = taskType;
            PipeStreamInstance = pipeStreamInstance;
        }

        public abstract Task GetTask();
        public PipeTaskType GetTaskType(){ return TaskType; }
        public abstract bool StartTask();
        public abstract void HandleTaskResult();

        public PipeTaskType TaskType { get; }
        protected NamedPipeClientStream PipeStreamInstance;
    }
    
    class PipeTaskConnect : PipeTaskBase
    {
        private Task ConnectTask;
    
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public PipeTaskConnect( NamedPipeClientStream pipeStreamInstance ) : base( PipeTaskType.Connect, pipeStreamInstance )
        {
        }
        public override bool StartTask()
        {
            if( PipeStreamInstance == null) { return false; }

            try
            {
                ConnectTask = PipeStreamInstance.ConnectAsync();
            }
            catch( Exception e ) {
                Log.Warn( "Error trying to connect to pipe {0}", e.ToString() );
                return false;
            }
            
            return true;
        }
        public override Task GetTask(){ return ConnectTask; }
        public override void HandleTaskResult()
        {
            if( ConnectTask.Status == TaskStatus.RanToCompletion )
            {
                Log.Debug("Connected to pipe");
            }
            else if( ConnectTask.Status == TaskStatus.Canceled )
            {
                Log.Trace("Pipe connection task was cancelled");
            }
        }
    }

    class PipeTaskRead : PipeTaskBase
    {
        private Task<Int32> ReadTask;
        byte[] ReadBuffer;
        LzsMessageQueue<byte[]> ToLivesplitQueue;
    
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public PipeTaskRead( NamedPipeClientStream pipeStreamInstance, LzsMessageQueue<byte[]> toLivesplitQueue ) : base( PipeTaskType.Read, pipeStreamInstance )
        {
            ToLivesplitQueue = toLivesplitQueue;
        }
        public override bool StartTask()
        {
            if( PipeStreamInstance == null) { return false; }

            try
            {
                ReadBuffer = new byte[PipeStreamInstance.InBufferSize];
                ReadTask = PipeStreamInstance.ReadAsync( ReadBuffer, 0, PipeStreamInstance.InBufferSize );
            }
            catch( Exception e ) {
                Log.Warn( "Error trying to read from pipe {0]", e.ToString() );
                return false;
            }
            
            return true;
        }
        public override Task GetTask(){ return ReadTask; }
        public override void HandleTaskResult()
        {
            if( ReadTask.Status == TaskStatus.RanToCompletion )
            {
                if( ReadTask.Result == 0 ){ Log.Debug("no bytes read from pipe (pipe broken during read?)"); }
                else
                {
                    byte[] MessageBuf = new byte[ReadTask.Result];
                    Array.Copy( ReadBuffer, MessageBuf, ReadTask.Result );
                    ToLivesplitQueue.Enqueue( MessageBuf );
                }
            }
            else if( ReadTask.Status == TaskStatus.Canceled )
            {
                Log.Trace("Pipe read task was cancelled");
            }
            
        }
    }

    class PipeTaskWrite : PipeTaskBase
    {
        private Task WriteTask;
        byte[] WriteBuffer;
    
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public PipeTaskWrite( NamedPipeClientStream pipeStreamInstance, byte[] writeBuffer ) : base( PipeTaskType.Write, pipeStreamInstance )
        {
            WriteBuffer = writeBuffer;
        }
        public override bool StartTask()
        {
            if( PipeStreamInstance == null) { return false; }

            try
            {
                WriteTask = PipeStreamInstance.WriteAsync( WriteBuffer, 0, WriteBuffer.Length );
            }
            catch( Exception e ) {
                Log.Warn( "Error trying to write to pipe ", e.ToString() );
                return false;
            }
            
            return true;
        }
        public override Task GetTask(){ return WriteTask; }
        public override void HandleTaskResult()
        {
            if( WriteTask.Status == TaskStatus.RanToCompletion )
            {
                Log.Debug("wrote to pipe");
            }
            else if( WriteTask.Status == TaskStatus.Canceled )
            {
                Log.Trace("Pipe write task was cancelled");
            }
        }
    }

} //namespace LiveSplit.Lazysplits.Pipe
