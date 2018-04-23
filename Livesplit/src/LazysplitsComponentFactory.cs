using System;
using LiveSplit.Model;
using LiveSplit.UI.Components;
using LiveSplit.Lazysplits;

using NLog;
using NLog.Targets;
using NLog.Config;

[assembly: ComponentFactory(typeof(LazysplitsComponentFactory))]

namespace LiveSplit.Lazysplits
{
    public class LazysplitsComponentFactory : IComponentFactory
    {
        public string ComponentName{ get { return "Lazysplits"; } }
        public string Description{ get { return "Lazysplits component"; } }
        public ComponentCategory Category{ get { return ComponentCategory.Control; } }
        
        public string UpdateName{ get {return ComponentName; } }
        public string XMLURL{ get; }
        public string UpdateURL{ get; }
        public Version Version{ get { return Version.Parse("1.0"); } }

        public IComponent Create(LiveSplitState state)
        {
            InitNLog();
            return new LazysplitsComponent(state);
        }

        private void InitNLog()
        {
            if( LogManager.Configuration == null )
            {
                LoggingConfiguration LogConfig = new LoggingConfiguration();

                //File log
                FileTarget LogFileTarget = new FileTarget();
                LogFileTarget.FileName = "${basedir}/Components/Lazysplits-log.txt";
                LogFileTarget.DeleteOldFileOnStartup = true;
                #if DEBUG
                    LogFileTarget.Layout = @"NLog|${date:format=HH\:mm\:ss.ff}|${pad:padding=5:inner=${level}}|${logger}|${message}";
                    LoggingRule FileRule = new LoggingRule( "*", LogLevel.Trace, LogFileTarget );
                #else
                    LogFileTarget.Layout = @"NLog|${date:format=HH\:mm\:ss.ff}|${logger}|${message}";
                    LoggingRule FileRule = new LoggingRule( "*", LogLevel.Info, LogFileTarget );
                #endif

                LogConfig.AddTarget( "File", LogFileTarget );
                LogConfig.LoggingRules.Add(FileRule);

                //console log
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
    }
} //namespace LiveSplit.Lazysplits
