using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
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
        public GraphicsCache Cache { get; set; }

        public float VerticalHeight { get; set; }
        public float MinimumHeight { get; set; }
        public float HorizontalWidth { get; set; }
        public float MinimumWidth { get; set; }

        public float PaddingTop { get; set; }
        public float PaddingLeft { get; set; }
        public float PaddingBottom { get; set; }
        public float PaddingRight { get; set; }

        public LazysplitsComponentSettings Settings { get; set; }
        public string ComponentName{ get { return "Lazysplits"; } }
        public IDictionary<string, Action> ContextMenuControls{ get; protected set; }
        private LiveSplitState State;
        private LzsTimerModel Timer;
        private bool bSettingsShouldInit = true;
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();
        
        private LzsPipeClient PipeClient;
        private bool bPipeConnected;
        private LzsMessageQueue<LsMsg> MsgQueue;
        
        private LzsSharedDataManager SharedDataManager;
        private LzsStatusIcons StatusIcons;
        
        public LazysplitsComponent(LiveSplitState state)
        {
            VerticalHeight = 0;
            HorizontalWidth = 0;
            Cache = new GraphicsCache();

            Settings = new LazysplitsComponentSettings();
            State = state;
            Timer = new LzsTimerModel();
            Timer.CurrentState = State;

            ContextMenuControls = (IDictionary<string, Action>) new Dictionary<string, Action>();
            ContextMenuControls.Add( "Lazysplits : Start pipe", new Action(this.StartPipeClient) );

            PipeClient = new LzsPipeClient( "Pipe client thread", "lazysplits_pipe", this );
            bPipeConnected = false;
            MsgQueue = new LzsMessageQueue<LsMsg>("LazysplitsComponent message queue");
            MsgQueue.AttachObserver(this);
            
            SharedDataManager = new LzsSharedDataManager(State);
            StatusIcons = new LzsStatusIcons();

            Settings.SharedDataRootDirChanged += OnRootDirChanged;
            Settings.StatusIconsEnabledChanged += OnStatusIconsChanged;
            State.RunManuallyModified += OnRunChanged;

            State.OnReset += OnSplitReset;
            State.OnSplit += OnSplitControlChanged;
            State.OnStart += OnSplitControlChanged;
            State.OnSkipSplit += OnSplitControlChanged;
            State.OnUndoSplit += OnSplitControlChanged;
            
        }

        public Control GetSettingsControl(LayoutMode mode)
        {
            return Settings;
        }
        public System.Xml.XmlNode GetSettings(System.Xml.XmlDocument document)
        {
            Log.Debug("GetSettings");
            return Settings.GetSettings(document);
        }
        public void SetSettings(System.Xml.XmlNode settings)
        {
            Log.Debug("SetSettings");
            Settings.SetSettings(settings);
        }
        private void SettingsInit()
        {
            if( Settings.bOpenPipeOnStart )
            {
                StartPipeClient();
            }

            bSettingsShouldInit = false;
        }

        public void Update(IInvalidator invalidator, Model.LiveSplitState state, float width, float height, LayoutMode mode)
        {
            State = state;

            if(bSettingsShouldInit)
            {
                SettingsInit();
            }
            
            if( Settings.bStatusIconsEnabled )
            { 
                StatusIcons.Update();

                Cache.Restart();
                Cache["IconPadding"] = Settings.IconPadding;
                Cache["IconMargin"] = Settings.IconMargin;
                Cache["ConnectionIconColor"] = Settings.ConnectionIconColor;
                Cache["IncomingDataIconColor"] = Settings.IncomingDataIconColor;
                Cache["OutgoingDataIconColor"] = Settings.OutgoingDataIconColor;
                Cache["WarningIconColor"] = Settings.WarningIconColor;
                Cache["ErrorIconColor"] = Settings.ErrorIconColor;
                Cache["InactiveIconColor"] = Settings.InactiveIconColor;
            }
            
            /*
            if( invalidator != null && ( Cache.HasChanged || Settings.bStatusIconsEnabled && StatusIcons.CacheHasChanged() ) )
            {
               invalidator.Invalidate(0, 0, width, height);
            }
            */

            if( invalidator != null )
            {
                if( Cache.HasChanged )
                {
                    invalidator.Invalidate(0, 0, width, height);
                }
                else if( Settings.bStatusIconsEnabled )
                {
                    if( StatusIcons.CacheHasChanged() )
                    {
                        invalidator.Invalidate(0, 0, width, height);
                    }
                }
            }
        }
        
        /* drawing */

        private void DrawBase(Graphics g, Model.LiveSplitState state, float width, float height, LayoutMode mode)
        {
            DrawBackground(g, width, height);
            if( Settings.bStatusIconsEnabled ){
                if( mode == LayoutMode.Vertical )
                {
                    StatusIcons.DrawStatusVertical( g, Settings, width, VerticalHeight );
                }
                else
                {
                    StatusIcons.DrawStatusHorizontal( g, Settings, HorizontalWidth, height );
                }
            }
        }
        private void DrawBackground(Graphics g, float width, float height)
        {
            if (Settings.BackgroundColor.A > 0
                || Settings.BackgroundGradient != GradientType.Plain
                && Settings.BackgroundColor2.A > 0)
            {
                var gradientBrush = new LinearGradientBrush(
                            new PointF(0, 0),
                            Settings.BackgroundGradient == GradientType.Horizontal
                            ? new PointF(width, 0)
                            : new PointF(0, height),
                            Settings.BackgroundColor,
                            Settings.BackgroundGradient == GradientType.Plain
                            ? Settings.BackgroundColor
                            : Settings.BackgroundColor2);
                g.FillRectangle(gradientBrush, 0, 0, width, height);
            }
        }
        public void DrawHorizontal(Graphics g, Model.LiveSplitState state, float height, Region clipRegion)
        {
            DrawBase(g, state, HorizontalWidth, height, LayoutMode.Horizontal);
        }
        public void DrawVertical(System.Drawing.Graphics g, Model.LiveSplitState state, float width, Region clipRegion)
        {
            DrawBase( g, state, width, VerticalHeight, LayoutMode.Vertical );
        }

        /* observer */

        public void OnSubjectNotify( string subjectName, string message )
        {
            switch(subjectName)
            {
                case "LazysplitsComponent message queue":
                    CheckMsgQueue();
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
        
        public void MsgPipeStatus( bool connected )
        {
            MsgQueue.Enqueue( new LsPipeStatusMsg(connected) );
        }
        public void MsgPipeData( LsPipeDataType dataType )
        {
            MsgQueue.Enqueue( new LsPipeDataMsg(dataType) );
        }
        public void MsgProtobuf( byte[] serializedProtobuf )
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
                        var PipeStatusMsg = (LsPipeStatusMsg)Msg;
                        bPipeConnected = PipeStatusMsg.Connected;

                        if(bPipeConnected){ StatusIcons.Connected(); }
                        else{ StatusIcons.Disconnected(); }

                        Log.Debug("Pipe "+ (bPipeConnected ? "Connected" : "Disconnected") );
                    }
                    else if ( Msg.Type == LsMsgType.PipeData )
                    {
                        var PipeDataMsg = (LsPipeDataMsg)Msg;
                        if( PipeDataMsg.DataType == LsPipeDataType.Received )
                        {
                            StatusIcons.MessageReceived();
                            Log.Debug("Message received");
                        }
                        else if( PipeDataMsg.DataType == LsPipeDataType.Sent )
                        {
                            StatusIcons.MessageSent();
                            Log.Debug("Message sent");
                        }
                    }
                    else if ( Msg.Type == LsMsgType.Protobuf )
                    {
                        var ProtobufMsg = (LsProtobufMsg) Msg;
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
                    TargetInfo TargetInfo = new TargetInfo();
                    if( SharedDataManager.TryGetTarget( msg.TargetName, ref TargetInfo ) )
                    {
                        string SharedDirsMatch = ( msg.SharedDataDir == Settings.SharedDataRootDir ) ? "match" : "don't match";
                        string GamesMatch = ( msg.GameName == SharedDataManager.GetCurrentGameName() ) ? "match" : "don't match";
                        Log.Debug("Target '"+TargetInfo.Name+"' found - id : "+msg.Id + ", type : "+TargetInfo.Type.ToString()+", games : "+GamesMatch );
                        
                        double epoch_ms = DateTime.Now.ToUniversalTime().Subtract( new DateTime( 1970, 1, 1) ).TotalMilliseconds;
                        int total_offset = (int)( ( epoch_ms - msg.TargetTimestamp ) + TargetInfo.SplitOffsetMs );

                        switch( TargetInfo.Type )
                        {
                            case TargetType.TgtReset :
                                if( State.CurrentPhase != TimerPhase.NotRunning ){ Timer.Reset(); }
                            break;
                            case TargetType.TgtStart :
                                if( State.CurrentPhase != TimerPhase.NotRunning ){ Timer.Reset(); }
                                Timer.AdjustedStart(total_offset);
                            break;
                            case TargetType.TgtStandard :
                                Timer.AdjustedSplit(total_offset);
                            break;
                        }
                    }
                    else
                    {
                        Log.Warn("Could not find target "+msg.TargetName+" for game "+msg.GameName);
                    }
                break;
            }
        }
        
        /* shared data stuff */

        private void SendTargetData()
        {
            PipeClient.MsgSerializedProtobuf( LzsProtoHelper.ClearTargetMsg() );
            
            if( SharedDataManager.IsCurrentGameAvailable() && SharedDataManager.IsTargets() )
            {
                byte[] SerializedMsg = LzsProtoHelper.NewTargetMsg(
                    Settings.SharedDataRootDir,
                    SharedDataManager.GetCurrentGameName(),
                    SharedDataManager.GetCurrentSplitTargets()
                );
                PipeClient.MsgSerializedProtobuf(SerializedMsg);
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
        public void OnStatusIconsChanged( object sender, EventArgs e )
        {
            VerticalHeight = Settings.bStatusIconsEnabled ? 20 : 0;
            HorizontalWidth = Settings.bStatusIconsEnabled ? 20 : 0;
        }
        public void OnRunChanged( object sender, EventArgs e )
        {
            //if we successfully updated data and are connected, resend targets
            if( SharedDataManager.RunChanged() && bPipeConnected  )
            {
                SendTargetData();
            }
        }
        public void OnSplitReset( object sender, TimerPhase timerPhase )
        {
            //SharedDataManager.Parse
            SendTargetData();
        }
        public void OnSplitControlChanged( object sender, EventArgs e )
        {
            SendTargetData();
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

        public int GetSettingsHashCode()
        {
            return Settings.GetSettingsHashCode();
        }
    }
} //namespace LiveSplit.Lazysplits
