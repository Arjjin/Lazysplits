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
        //private IRun CurrentRun;
        private LiveSplitState State;
        private List<CsMessage.Types.Target> Targets;
        private ISegment LastParsedActiveSegment;

        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsSplitsData( LiveSplitState state )
        {
            State = state;
            Targets = new List<CsMessage.Types.Target>();
            ParseTargets();
        }

        public void ParseTargets()
        {
            Targets.Clear();

            foreach( var SrcomVar in State.Run.Metadata.VariableValueNames )
            {
                var KeyArray = SrcomVar.Key.Split('|');
                if( KeyArray[0] == "LzsGlobalTarget" || State.CurrentSplit != null && KeyArray[0] == "LzsSplitTarget" && KeyArray[1] == State.CurrentSplit.Name )
                {
                    var NewTarget = new CsMessage.Types.Target();
                    var ValueArray = SrcomVar.Value.Split('|');
                    NewTarget.TargetName = ValueArray[0];

                    //if length greater than 1 watch variable(s) exist
                    if( ValueArray.Length > 1 )
                    {
                        for( int i = 1; i < ValueArray.Length; i++ )
                        {
                            var WatchVarArray = ValueArray[i].Split(',');
                            if( WatchVarArray.Length == 3 )
                            {
                                var NewWatchVariable = new CsMessage.Types.WatchVariable();

                                //try and parse an int and head to next watchvar if it fails
                                uint index;
                                if( uint.TryParse( WatchVarArray[0], out index) ){ NewWatchVariable.Index = index; }
                                else { continue; }
                                
                                NewWatchVariable.Name = WatchVarArray[1];
                                NewWatchVariable.Value = WatchVarArray[2];
                                
                                NewTarget.WatchVariables.Add(NewWatchVariable);
                            }
                        }
                    }

                    Targets.Add(NewTarget);
                }
            }

            LastParsedActiveSegment = State.CurrentSplit;
        }

        public bool IsTargets(){
            return Targets.Count > 0;
        }
        public List<CsMessage.Types.Target> GetTargets(){
            if( LastParsedActiveSegment != State.CurrentSplit ){ ParseTargets(); }
            return Targets;
        }
    }
}
