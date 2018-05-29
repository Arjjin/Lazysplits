using System;
using System.Collections.Generic;

using NLog;

using LiveSplit.Model;

using LiveSplit.Lazysplits.Pipe;
using LiveSplit.Lazysplits.Proto;

namespace LiveSplit.Lazysplits.SharedData
{
    public class LzsSharedDataManager
    {
        //private LazysplitsComponentSettings ParentSettings;

        private string RootDir { get; set; }
        private LiveSplitState State { get; set; }
        private LzsGameList GameList { get; set; }
        private LzsCurrentGame CurrentGame { get; set; }

        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsSharedDataManager( LiveSplitState state )
        {
            State = state;
            GameList = new LzsGameList();
            CurrentGame = new LzsCurrentGame();

            if( State.Run.FilePath != String.Empty ){ CurrentGame.NewSplitsFile(State.Run.FilePath); }
        }

        public bool SetRootDir( string path )
        {
            if( RootDir != path )
            {
                RootDir = path;
                Log.Info("New root directory ("+path+")");
                GameList.ParseFromDir(RootDir);
                if( GameList.GameExists(State.Run.GameName) )
                {
                    string GamePath = RootDir+GameList.GetGameDir(State.Run.GameName);
                    CurrentGame.ParseFromDir(GamePath);
                }
                return true;
            }
            return false;
        }

        public bool RunChanged()
        {
            //check for new game
            if( !CurrentGame.bAvailable || CurrentGame.GetGameName() != State.Run.GameName )
            {
                if( GameList.GameExists(State.Run.GameName) )
                {
                    string GamePath = RootDir+GameList.GetGameDir(State.Run.GameName);
                    CurrentGame.ParseFromDir(GamePath);
                    CurrentGame.NewSplitsFile(State.Run.FilePath);
                    
                    return true;
                }
                else if(CurrentGame.bAvailable)
                {
                    CurrentGame.SetUnavailable();
                    return true;
                }
            }
            //if game is the same, check if splits file changed
            else if( CurrentGame.bAvailable && CurrentGame.GetGameName() == State.Run.GameName && CurrentGame.GetSplitsFilePath() != State.Run.FilePath )
            {
                CurrentGame.NewSplitsFile(State.Run.FilePath);

                //if ( CurrentGame.SplitsFile.bAvailable ){ NotifyAll("Splits changed"); }
                return true;
            }
            return false;
        }
        
        public string GetCurrentGameName()
        {
            return CurrentGame.GetGameName();
        }
        public List<LzsSplitTarget> GetCurrentSplitTargets()
        {
            string CurrentSplitName = ( State.CurrentSplit != null ) ? State.CurrentSplit.Name : "";
            return CurrentGame.GetCurrentSplitsTargets(CurrentSplitName);
        }
        public bool TryGetTarget( string targetName, ref TargetInfo targetInfoRef )
        {
            return CurrentGame.TryGetTarget( targetName, ref targetInfoRef );
        }

    }
}
