using System;

using NLog;
using NLog.Targets;
using NLog.Config;

namespace LiveSplit.Lazysplits
{
    public static class LzsLogging
    {
        
        public static event EventHandler<OnLogWarningOrErrorArgs> OnLogWarningOrError;
        public class OnLogWarningOrErrorArgs : EventArgs 
        {
            public string LogLevel { get; set; }
        }

        public static void TryInitNLog()
        {
            if( LogManager.Configuration == null )
            {
                LoggingConfiguration LogConfig = new LoggingConfiguration();

                //status icon warning/errors
                MethodCallTarget StatusIconTarget = new MethodCallTarget();
                StatusIconTarget.ClassName = typeof(LzsLogging).AssemblyQualifiedName;
                StatusIconTarget.MethodName = "LogWarningOrError";
                StatusIconTarget.Parameters.Add( new MethodCallParameter("${level}") );
                LoggingRule StatusIconRule = new LoggingRule( "*", LogLevel.Warn, StatusIconTarget );
                LogConfig.AddTarget( "StatusIcon", StatusIconTarget );
                LogConfig.LoggingRules.Add(StatusIconRule);

                //File log
                FileTarget LogFileTarget = new FileTarget();
                LogFileTarget.FileName = @"${basedir}/Components/Lazysplits/logs/log.txt";
                LogFileTarget.ArchiveNumbering = NLog.Targets.ArchiveNumberingMode.Date;
                LogFileTarget.ArchiveDateFormat = "yyyyMMddHHmmss";
                LogFileTarget.MaxArchiveFiles = 20;
                LogFileTarget.ArchiveOldFileOnStartup = true;
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

        public static void LogWarningOrError( string logLevel )
        {
            var args = new OnLogWarningOrErrorArgs();
            args.LogLevel = logLevel;
            OnLogWarningOrError?.Invoke( null, args );
        }
    }
}
