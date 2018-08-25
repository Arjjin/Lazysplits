using System;
using System.IO;
using System.Collections.Generic;
using System.Xml;

using LiveSplit.Model;
using LiveSplit.Lazysplits.Proto;

using NLog;

namespace LiveSplit.Lazysplits.SharedData
{
    public class LzsSplitsData
    {
        private LiveSplitState State;
        private List<KeyValuePair<string,CsMessage.Types.Target>> Targets;
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsSplitsData( LiveSplitState state )
        {
            State = state;
            State.Run.PropertyChanged += OnPropertyChanged;
            Targets = new List<KeyValuePair<string,CsMessage.Types.Target>>();
            ParseTargets();
        }
        void OnPropertyChanged( Object sender, EventArgs e )
        {
            ParseTargets();
        }

        public void ParseTargets()
        {
            Targets.Clear();

            foreach( ISegment segment in State.Run )
            {
                if( segment.Name.Contains("$lzs") && segment.Name.Contains("[") && segment.Name.Contains("]") )
                {
                    string Data = segment.Name.Substring( segment.Name.IndexOf("[") + 1, segment.Name.IndexOf("]") - segment.Name.IndexOf("[") - 1 );
                    foreach( string target in Data.Split(';') )
                    {
                        if( target.Length > 0 )
                        {
                            string[] targetData = target.Split('|');
                            //if( targetData[0] == "GlobalTarget" || State.CurrentSplit != null && targetData[0] == "SplitTarget" && segment.Name == State.CurrentSplit.Name )
                            if( targetData.Length >= 2 && targetData[0] == "GlobalTarget" || targetData[0] == "SplitTarget" )
                            {
                                string TargetKey = ( targetData[0] == "GlobalTarget" ) ? "Global" : segment.Name;
                                var ProtoTarget = new CsMessage.Types.Target();
                                ProtoTarget.TargetName = targetData[1];

                                //if length is greater than 2, watch var/vars exist
                                if( targetData.Length > 2 )
                                {
                                    for( int i = 2; i < targetData.Length; i++ )
                                    {
                                        string[] WatchVarData = targetData[i].Split(',');
                                        if( WatchVarData.Length == 3 )
                                        {
                                            var NewWatchVariable = new CsMessage.Types.WatchVariable();
                                
                                            NewWatchVariable.EntryLabel = WatchVarData[0];
                                            NewWatchVariable.Name = WatchVarData[1];
                                            NewWatchVariable.Value = WatchVarData[2];
                                
                                            ProtoTarget.WatchVariables.Add(NewWatchVariable);
                                        }
                                    }
                                }

                                Targets.Add( new KeyValuePair<string, CsMessage.Types.Target>( TargetKey, ProtoTarget ) );
                            }
                        }
                    }
                }
            }

            Log.Debug("Targets parsed");
        }

        public bool IsTargets(){
            return Targets.Count > 0;
        }
        public List<CsMessage.Types.Target> GetCurrentTargets(){
            var CurrentTargets = new List<CsMessage.Types.Target>();

            foreach( var targetEntry in Targets )
            {
                if( targetEntry.Key == "Global" || State.CurrentSplit != null && State.CurrentSplit.Name == targetEntry.Key )
                {
                    CurrentTargets.Add(targetEntry.Value); 
                }
            }

            return CurrentTargets;
        }
    }
}
