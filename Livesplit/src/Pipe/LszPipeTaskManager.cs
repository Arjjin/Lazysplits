using System;
using System.Collections.Generic;
using System.Linq;

using System.Threading;
using System.Threading.Tasks;
using System.IO.Pipes;

using NLog;
using LiveSplit.Lazysplits;

namespace LiveSplit.Lazysplits.Pipe
{

    //class to manage adding, waiting on, and removing pipe tasks
    class PipeTaskManager : IDisposable
    {
        private List<IPipeTask> PipeTaskList;
        private CancellationTokenSource WaitTokenSource;
        private volatile bool bIsWaiting;
    
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public PipeTaskManager( ref NamedPipeClientStream pipeStream )
        {
            PipeTaskList = new List<IPipeTask>();
            WaitTokenSource = new CancellationTokenSource();
            bIsWaiting = false;
        }

        public bool AddConnectTask( NamedPipeClientStream pipeStream )
        {
            if( !IsTaskInList( PipeTaskType.Connect ) )
            {
                PipeTaskConnect ConnectTask = new PipeTaskConnect( pipeStream );
                if( ConnectTask.StartTask() )
                {
                    PipeTaskList.Add(ConnectTask);
                    Log.Trace("Pipe connection task added");
                    return true;
                }
            }

            return false;
        }
        public bool AddReadTask( NamedPipeClientStream pipeStream, LazysplitsComponent lzsComponent )
        {
            if( !IsTaskInList( PipeTaskType.Read ) )
            {
                PipeTaskRead ReadTask = new PipeTaskRead( pipeStream, lzsComponent );
                if( ReadTask.StartTask() )
                {
                    PipeTaskList.Add(ReadTask);
                    Log.Trace("Pipe read task added");
                    return true;
                }
            }

            return false;
        }
        public bool AddWriteTask(  NamedPipeClientStream pipeStream , byte[] writeBuffer )
        {
            PipeTaskWrite WriteTask = new PipeTaskWrite( pipeStream, writeBuffer );
            if( WriteTask.StartTask() )
            {
                PipeTaskList.Add(WriteTask);
                Log.Trace("Pipe write task added");
                return true;
            }

            return false;
        }
        public bool IsTaskInList( PipeTaskType pipeOp )
        {
            foreach( IPipeTask task in PipeTaskList )
            {
                if( task.GetTaskType() == pipeOp ){ return true; }
            }
            return false;
        }

        public void WaitOnTasks( LazysplitsComponent lzsComponent )
        {
            if( PipeTaskList.Count > 0 )
            {
                try
                {
                    //populate array of tasks to wait on
                    Task[] TaskArray = new Task[PipeTaskList.Count];
                    for( int i = 0; i < PipeTaskList.Count; i++ ){ TaskArray[i] = PipeTaskList[i].GetTask(); }

                    //refresh our cancellation token if needed
                    if(WaitTokenSource.Token.IsCancellationRequested)
                    {
                        WaitTokenSource.Dispose();
                        WaitTokenSource = new CancellationTokenSource();
                    }

                    //wait on any
                    Log.Trace( "waiting on {0} tasks", TaskArray.Length );
                    bIsWaiting = true;
                    int SignalledTaskIndex = Task.WaitAny( TaskArray.ToArray(), WaitTokenSource.Token );
                    bIsWaiting = false;
                    //wait finished, handle result
                    IPipeTask CurrentTask = PipeTaskList[SignalledTaskIndex];
                    CurrentTask.HandleTaskResult();
                    //handle status icons if enabled from our LiveSplit component...really ugly
                    if( lzsComponent.Settings.bStatusIconsEnabled )
                    {
                        if( CurrentTask.GetTask().Status == TaskStatus.RanToCompletion )
                        {
                            if( CurrentTask.GetTaskType() == PipeTaskType.Read ){ lzsComponent.MsgPipeData(LsPipeDataType.Received); }
                            else if( CurrentTask.GetTaskType() == PipeTaskType.Write ){ lzsComponent.MsgPipeData(LsPipeDataType.Sent); }
                        }
                    }

                    //dispose of task and remove from list
                    Log.Trace( "Removing {0} from list", Enum.GetName( typeof(PipeTaskType), CurrentTask.GetTaskType() ) );
                    PipeTaskList.RemoveAt(SignalledTaskIndex);
                }
                catch(OperationCanceledException){ Log.Trace("Task wait cancelled"); }
            }
        }
        public bool IsWaiting()
        {
            return bIsWaiting;
        }
        public void CancelWait()
        {
            if( WaitTokenSource.Token.CanBeCanceled ){ WaitTokenSource.Cancel(); }
        }
        
        //IDisposable
        public void Dispose()
        {
            WaitTokenSource.Dispose();
        }
    }

} //namespace LiveSplit.Lazysplits.Pipe