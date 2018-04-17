using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

using NLog;
using NLog.Config;
using NLog.Targets;

using LiveSplit.Model;
using LiveSplit.UI;
using LiveSplit.UI.Components;

using LiveSplit.Lazysplits.Pipe;
using LiveSplit.Lazysplits.Proto;

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
        private LzsMessageQueue<byte[]> LsToPipeQueue;
        private LzsMessageQueue<byte[]> PipeToLsQueue;

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
            State = state;
            Timer = new LzsTimerModel { CurrentState = state };
            Settings = new LazysplitsComponentSettings();
            
            ContextMenuControls = new Dictionary<string, Action>();
            ContextMenuControls.Add( "Lazysplits : Start pipe", StartPipeClient );
            
            InitNLog();
            
            LsToPipeQueue = new LzsMessageQueue<byte[]>();
            PipeToLsQueue = new LzsMessageQueue<byte[]>();
            PipeToLsQueue.AttachObserver(this);
            PipeClient = new LzsPipeClient( "Pipe client thread", "lazysplits_pipe", LsToPipeQueue, PipeToLsQueue );
            
            VerticalHeight = 10;
            HorizontalWidth = 10;
        }

        private void InitNLog()
        {
            if( LogManager.Configuration == null )
            {
                LoggingConfiguration LogConfig = new LoggingConfiguration();

                FileTarget LogFileTarget = new FileTarget();
                LogFileTarget.FileName = "${basedir}/Components/Lazysplits-log.txt";
                LogFileTarget.DeleteOldFileOnStartup = true;
                LogFileTarget.Layout = @"NLog|${date:format=HH\:mm\:ss.ff}|${pad:padding=5:inner=${level}}|${logger}|${message}";

                #if DEBUG
                    LoggingRule FileRule = new LoggingRule( "*", LogLevel.Trace, LogFileTarget );
                #else
                    LoggingRule FileRule = new LoggingRule( "*", LogLevel.Info, LogFileTarget );
                #endif

                LogConfig.AddTarget( "File", LogFileTarget );
                LogConfig.LoggingRules.Add(FileRule);

                #if DEBUG
                    TraceTarget LogTraceTarget = new TraceTarget();
                    LogTraceTarget.Layout = @"NLog|${date:format=HH\:mm\:ss.ff}|${pad:padding=5:inner=${level}}|${logger}|${message}";
                    LoggingRule TraceRule = new LoggingRule( "*", LogLevel.Trace, LogTraceTarget );
                    LogConfig.AddTarget( "File", LogTraceTarget );
                    LogConfig.LoggingRules.Add(TraceRule);
                #endif

                LogManager.Configuration = LogConfig;
            }
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

        public void StartPipeClient()
        {
            if( !PipeClient.ThreadIsLive() )
            {
                PipeClient.ThreadCreate();
                ContextMenuControls.Remove("Lazysplits : Start pipe");
                ContextMenuControls.Add( "Lazysplits : Stop pipe", StopPipeClient );
                ContextMenuControls.Add( "Lazysplits : Reset pipe", ResetPipeClient );
            }
        }
        public void StopPipeClient()
        {
            if( PipeClient.ThreadIsLive() )
            {
                PipeClient.ThreadTerminate();
                PipeClient.ThreadJoin();
                ContextMenuControls.Remove("Lazysplits : Reset pipe");
                ContextMenuControls.Remove("Lazysplits : Stop pipe");
                ContextMenuControls.Add( "Lazysplits : Start pipe", StartPipeClient );
            }
        }
        public void ResetPipeClient()
        {
            if( PipeClient.ThreadIsLive() )
            {
                StopPipeClient();
                StartPipeClient();
            }
        }

        public void OnSubjectNotify()
        {
            CheckReadQueue();
        }

        private void CheckReadQueue()
        {
            while( !PipeToLsQueue.IsEmpty() )
            {
                byte[] SerializedMessage;
                if( PipeToLsQueue.TryDequeue( out SerializedMessage ) )
                {
                    CppMessage CvMessage;
                    try
                    {
                        CvMessage = CppMessage.Parser.ParseFrom(SerializedMessage);
                    }
                    catch( Google.Protobuf.InvalidProtocolBufferException e )
                    {
                        Log.Warn("Error deserializing protobuf : "+e.Message);
                        return;
                    }
                    Log.Debug("Deserialized message - id : "+CvMessage.MessageId+", type : "+CvMessage.MessageType.ToString() );
                }
                else
                {
                    System.Threading.Thread.Sleep(50);
                }
            }
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
