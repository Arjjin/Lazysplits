using System;
using System.IO.Pipes;

using LiveSplit.Lazysplits.Threading;
using NLog;

namespace LiveSplit.Lazysplits.Pipe
{
    enum PipeStatus { None, NotCreated, Created, ConnectionPending, Connected };

    //main pipe client class
    class LzsPipeClient : LzsThread, ILzsObserver
    {
        private string PipeName;
        private NamedPipeClientStream PipeStream;
        private PipeTaskManager TaskManager;
        private LzsMessageQueue<byte[]> FromLivesplitQueue;
        private LzsMessageQueue<byte[]> ToLivesplitQueue;
    
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsPipeClient( string threadName, string pipeName, LzsMessageQueue<byte[]> fromLivesplitQueue, LzsMessageQueue<byte[]> toLivesplitQueue ) : base(threadName)
        {
            PipeName = pipeName;
            FromLivesplitQueue = fromLivesplitQueue;
            ToLivesplitQueue = toLivesplitQueue;

        }
        protected override void ThreadFuncInit()
        {
            base.ThreadFuncInit();
            
            try
            {
                PipeStream = new NamedPipeClientStream(
                    ".",
                    PipeName,
                    PipeAccessRights.ReadWrite,
                    PipeOptions.Asynchronous,
                    System.Security.Principal.TokenImpersonationLevel.None,
                    System.IO.HandleInheritability.None    
                );
            }
            catch( Exception e )
            {
                Log.Error( "Error creating pipe : {0}", e.ToString() );
                ThreadTerminate();
            }

            TaskManager = new PipeTaskManager( ref PipeStream );
            //attach self as observer to read queue, so we can wake up to parse messages that get added
            FromLivesplitQueue.AttachObserver(this);

            Log.Info("Pipe created");

        }
        protected override void ThreadFunc()
        {
            ThreadFuncInit();
            
            while( ThreadFuncShouldLoop() )
            {
                if( !PipeStream.IsConnected && !TaskManager.IsTaskInList( PipeTaskType.Connect ) )
                {
                    TaskManager.AddConnectTask(PipeStream);
                }
                else if(PipeStream.IsConnected)
                {
                    //process messages from queue
                    if( !FromLivesplitQueue.IsEmpty() )
                    {
                        while( FromLivesplitQueue.Count() > 0 )
                        {
                            byte[] serialized_protobuf;
                            if( FromLivesplitQueue.TryDequeue( out serialized_protobuf ) )
                            {
                                TaskManager.AddWriteTask( PipeStream, serialized_protobuf );
                            }
                            else
                            {
                                System.Threading.Thread.Sleep(50);
                            }
                        }
                    }
                    if( PipeStream.ReadMode != PipeTransmissionMode.Message ){ PipeStream.ReadMode = PipeTransmissionMode.Message; }
                    if( !TaskManager.IsTaskInList( PipeTaskType.Read ) ) { TaskManager.AddReadTask( PipeStream, ToLivesplitQueue ); }
                }

                //wait on tasks
                TaskManager.WaitOnTasks();
            }

            ThreadFuncCleanup();
        }
        protected override void ThreadFuncCleanup()
        {
            TaskManager.Dispose();
            //PipeStream.Close();
            PipeStream.Dispose();
            Log.Info("Pipe Closed");

            base.ThreadFuncCleanup();
        }
        public override void ThreadTerminate()
        {
            Log.Debug("Pipe client thread terminating");
            if( TaskManager != null && TaskManager.IsWaiting() ){  TaskManager.CancelWait(); }

            base.ThreadTerminate();
        }
        public void OnSubjectNotify()
        {
            //if our pipe is connected and waiting on tasks, wake it up to write new messages from queue
            if( PipeStream.IsConnected && TaskManager.IsWaiting() ){ TaskManager.CancelWait(); }
        }
    }
} //namespace LiveSplit.Lazysplits.Pipe
