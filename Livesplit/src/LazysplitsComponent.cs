using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Threading;

using NLog;

using Google.Protobuf;

using LiveSplit.Model;
using LiveSplit.UI;
using LiveSplit.UI.Components;

using LiveSplit.Lazysplits.Pipe;
using LiveSplit.Lazysplits.Proto;
using LiveSplit.Lazysplits.SharedData;

namespace LiveSplit.Lazysplits
{
    public class LazysplitsComponent : IComponent, ILzsObserver
    {
        public LazysplitsComponentSettings Settings { get; set; }
        public string ComponentName{ get { return "Lazysplits"; } }
        public IDictionary<string, Action> ContextMenuControls{ get; protected set; }
        private LiveSplitState State;
        private LzsTimerModel Timer;

        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        //pipe stuff
        private LzsPipeClient PipeClient;
        private bool bPipeConnected;
        private LzsMessageQueue<LsMsg> MsgQueue;

        private LzsSharedDataManager SharedDataManager;

        public float VerticalHeight { get; set; }
        public float MinimumHeight { get; set; }
        public float HorizontalWidth { get; set; }
        public float MinimumWidth { get; set; }

        public float PaddingTop { get; set; }
        public float PaddingLeft { get; set; }
        public float PaddingBottom { get; set; }
        public float PaddingRight { get; set; }

        public LazysplitsComponent(LiveSplitState state)
        {
            Settings = new LazysplitsComponentSettings();
            State = state;
            Timer = new LzsTimerModel();
            Timer.CurrentState = State;

            ContextMenuControls = (IDictionary<string, Action>) new Dictionary<string, Action>();
            //ContextMenuControls.Add("Lazysplits : Start pipe", new Action(this.StartPipeClient));

            PipeClient = new LzsPipeClient( "Pipe client thread", "lazysplits_pipe", this );
            bPipeConnected = false;
            MsgQueue = new LzsMessageQueue<LsMsg>("LazysplitsComponent message queue");
            MsgQueue.AttachObserver(this);

            SharedDataManager = new LzsSharedDataManager(State.Run);
            SharedDataManager.AttachObserver(this);

            Settings.SharedDataRootDirChanged += OnRootDirChanged;
            State.RunManuallyModified += OnRunChanged;

            PipeClient.ThreadCreate();
            //if( SharedDataManager.CurrentGame.bAvailable ){ SharedDataManager.}

            VerticalHeight = 10f;
            HorizontalWidth = 10f;
        }

        public Control GetSettingsControl(LayoutMode mode)
        {
            return this.Settings;
        }
        public System.Xml.XmlNode GetSettings(System.Xml.XmlDocument document)
        {
            return this.Settings.GetSettings(document);
        }
        public void SetSettings(System.Xml.XmlNode settings)
        {
            this.Settings.SetSettings(settings);
        }

        public void Update(IInvalidator invalidator, Model.LiveSplitState state, float width, float height, LayoutMode mode)
        {
        }
        
        private void DrawBase(Graphics g, Model.LiveSplitState state, float width, float height, LayoutMode mode)
        {
                var gradientBrush = new LinearGradientBrush(
                    new PointF(0, 0),
                    new PointF(0, height),
                    Color.Aqua,
                    Color.DarkRed
                );
            
                g.FillRectangle(gradientBrush, 0, 0, width, height);
        }
        public void DrawHorizontal(Graphics g, Model.LiveSplitState state, float height, Region clipRegion)
        {
            DrawBase(g, state, HorizontalWidth, height, LayoutMode.Horizontal);
        }

        public void DrawVertical(System.Drawing.Graphics g, Model.LiveSplitState state, float width, Region clipRegion)
        {
            DrawBase(g, state, width, VerticalHeight, LayoutMode.Vertical);
        }

        public void OnSubjectNotify( string subjectName, string message )
        {
            switch(subjectName)
            {
                case "LazysplitsComponent message queue":
                    CheckMsgQueue();
                break;
                case "Shared data manager":
                    if( message == "Splits changed" && bPipeConnected )
                    {
                        SendTargetData();
                    }
                break;
            }
        }

        /* pipe methods */

        public void StartPipeClient()
        {
            if( !PipeClient.ThreadIsLive() )
            {
                PipeClient.ThreadCreate();
            }
            ContextMenuControls.Remove("Lazysplits : Start pipe");
            ContextMenuControls.Add( "Lazysplits : Stop pipe", StopPipeClient );
            ContextMenuControls.Add( "Lazysplits : Reset pipe", ResetPipeClient );
        }
        public void StopPipeClient()
        {
            if( PipeClient.ThreadIsLive() )
            {
                PipeClient.ThreadTerminate();
                PipeClient.ThreadJoin();
            }
            ContextMenuControls.Remove("Lazysplits : Reset pipe");
            ContextMenuControls.Remove("Lazysplits : Stop pipe");
            ContextMenuControls.Add( "Lazysplits : Start pipe", StartPipeClient );
        }
        public void ResetPipeClient()
        {
            if( PipeClient.ThreadIsLive() )
            {
                StopPipeClient();
                StartPipeClient();
            }
        }

        /* message queue stuff */

        public void MsgPipeStatus(bool connected)
        {
            MsgQueue.Enqueue( new LsPipeStatusMsg(connected) );
        }

        public void MsgProtobuf(byte[] serializedProtobuf)
        {
            MsgQueue.Enqueue( new LsProtobufMsg(serializedProtobuf) );
        }

        private void CheckMsgQueue()
        {
            while ( !MsgQueue.IsEmpty() )
            {
                LsMsg Msg;
                if( MsgQueue.TryDequeue( out Msg ) )
                {
                    if( Msg.Type == LsMsgType.PipeStatus )
                    {
                        LsPipeStatusMsg PipeStatusMsg = (LsPipeStatusMsg)Msg;
                        /*
                        if( bPipeConnected = PipeStatusMsg.Connected == true )
                        {
                            SendTargetData();
                        }
                        */
                        bPipeConnected = PipeStatusMsg.Connected;
                        Log.Debug("Pipe "+ (bPipeConnected ? "Connected" : "Disconnected") );
                    }
                    else if ( Msg.Type == LsMsgType.Protobuf )
                    {
                        LsProtobufMsg ProtobufMsg = (LsProtobufMsg) Msg;
                        CppMessage ObsMsg;
                        try
                        {
                            ObsMsg = CppMessage.Parser.ParseFrom(ProtobufMsg.SerializedProtobuf);
                            HandleProtobufMsg(ObsMsg);
                        }
                        catch (InvalidProtocolBufferException ex)
                        {
                            Log.Warn("Error deserializing protobuf : " + ex.Message);
                        }
                    }
                }
                else
                {
                    Log.Warn("failed to dequeue message, waiting 50ms");
                    Thread.Sleep(50);
                }
            }
        }

        void HandleProtobufMsg( CppMessage msg )
        {
            switch(msg.Type)
            {
                case CppMessage.Types.MessageType.RequestResync:
                    SendTargetData();
                break;
                case CppMessage.Types.MessageType.TargetFound:
                    string SharedDirsMatch = ( msg.SharedDataDir == Settings.SharedDataRootDir ) ? "match" : "don't match";
                    string GamesMatch = ( msg.GameName == SharedDataManager.CurrentGame.GameInfo.Name ) ? "match" : "don't match";

                    Log.Debug("Target found - id : "+msg.Id + ", type : "+msg.Type.ToString()+", games : "+GamesMatch+
                        ", timestamp : "+msg.TargetTimestamp );
                    ulong epoch_ms = (ulong)DateTime.Now.ToUniversalTime().Subtract( new DateTime( 1970, 1, 1) ).TotalMilliseconds;
                    ulong total_offset = ( epoch_ms - msg.TargetTimestamp ) + msg.TargetOffsetMs;
                    Log.Debug( msg.TargetOffsetMs );
                    Timer.Reset();
                    Timer.Start(
                        new TimeSpan( 0, 0, 0, 0, (int)( epoch_ms - msg.TargetTimestamp + msg.TargetOffsetMs ) )
                    );
                    SendTargetData();
                    //Timer.Start();
                break;
            }
            Log.Debug("Deserialized message - id : "+msg.Id + ", type : "+msg.Type.ToString());
        }
        
        /* shared data stuff */

        private void SendTargetData()
        {
            PipeClient.MsgSerializedProtobuf( LzsProtoHelper.ClearTargetMsg() );
            if( SharedDataManager.CurrentGame.SplitsFile.bAvailable )
            {
                List<string> TargetNames = SharedDataManager.CurrentGame.SplitsFile.GetGlobalTargets();

                foreach( string TargetName in TargetNames )
                {
                    byte[] SerializedMsg = LzsProtoHelper.NewTargetMsg( Settings.SharedDataRootDir, SharedDataManager.CurrentGame.GameInfo.Name, TargetName );
                    PipeClient.MsgSerializedProtobuf(SerializedMsg);
                }
            }
        }

        //event handlers
        public void OnRootDirChanged( object sender, EventArgs e )
        {
            if ( SharedDataManager.SetRootDir(Settings.SharedDataRootDir) )
            {
                SendTargetData();
            }
        }

        public void OnRunChanged(object sender, EventArgs e)
        {
            SharedDataManager.RunChanged(State.Run);
        }

        //IDisposable
        public void Dispose()
        {
            if( PipeClient.ThreadIsLive() )
            {
                PipeClient.ThreadTerminate();
                PipeClient.ThreadJoin();
            }
            Log.Debug("Component disposed");
        }
    }
} //namespace LiveSplit.Lazysplits
