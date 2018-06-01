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

        private string RootDir;
        private LiveSplitState State;
        private LzsGameList GameList;
        private LzsCurrentGame CurrentGame;
        private LzsSplitsData SplitsData;

        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsSharedDataManager( LiveSplitState state )
        {
            State = state;
            GameList = new LzsGameList();
            CurrentGame = new LzsCurrentGame();
            SplitsData = new LzsSplitsData(State);
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
                    SplitsData.ParseTargets();
                    
                    return true;
                }
                else if(CurrentGame.bAvailable)
                {
                    CurrentGame.SetUnavailable();

                    return true;
                }
            }
            //if game is the same, check if splits file changed
            else if( CurrentGame.bAvailable && CurrentGame.GetGameName() == State.Run.GameName )
            {
                SplitsData.ParseTargets();

                return true;
            }

            return false;
        }
        
        public bool IsCurrentGameAvailable()
        {
            return CurrentGame.bAvailable;
        }
        public string GetCurrentGameName()
        {
            return CurrentGame.GetGameName();
        }
        public List<CsMessage.Types.Target> GetCurrentSplitTargets()
        {
            return SplitsData.GetTargets();
        }
        public bool IsTargets()
        {
            return SplitsData.IsTargets();
        }
        public bool TryGetTarget( string targetName, ref TargetInfo targetInfoRef )
        {
            return CurrentGame.TryGetTarget( targetName, ref targetInfoRef );
        }

    }
}
