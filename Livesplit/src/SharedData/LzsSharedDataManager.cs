using System;

using NLog;

using LiveSplit.Model;

using LiveSplit.Lazysplits.Pipe;

namespace LiveSplit.Lazysplits.SharedData
{
    public class LzsSharedDataManager : LzsObservable
    {
        //private LazysplitsComponentSettings ParentSettings;
        
        private string RootDir { get; set; }
        private IRun CurrentRun { get; set; }
        public LzsGameList GameList { get; private set; }
        public LzsCurrentGame CurrentGame { get; private set; }

        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsSharedDataManager( IRun currentRun ) : base("Shared data manager")
        {
            CurrentRun = currentRun;
            GameList = new LzsGameList();
            CurrentGame = new LzsCurrentGame();

            if( currentRun.FilePath != String.Empty ){ CurrentGame.NewSplitsFile(currentRun.FilePath); }
        }

        public bool SetRootDir( string path )
        {
            if( RootDir != path )
            {
                RootDir = path;
                GameList.ParseFromDir(RootDir);
                if( GameList.GameExists(CurrentRun.GameName) )
                {
                    string GamePath = RootDir+GameList.GetGameDir(CurrentRun.GameName);
                    CurrentGame.ParseFromDir(GamePath);
                }

                return true;
            }
            return false;
        }

        public void RunChanged( IRun run )
        {
            CurrentRun = run;

            //check for new game
            if( !CurrentGame.bAvailable || CurrentGame.GameInfo.Name != CurrentRun.GameName )
            {
                if( GameList.GameExists(run.GameName) )
                {
                    string GamePath = RootDir+GameList.GetGameDir(CurrentRun.GameName);
                    CurrentGame.ParseFromDir(GamePath);
                    CurrentGame.NewSplitsFile(CurrentRun.FilePath);

                    NotifyAll("Splits changed");
                }
                else if(CurrentGame.bAvailable)
                {
                    CurrentGame.SetUnavailable();
                    NotifyAll("Splits changed");
                }
            }
            //if game is the same, check if splits file changed
            else if( CurrentGame.bAvailable && CurrentGame.GameInfo.Name == CurrentRun.GameName && CurrentGame.SplitsFile.Path != CurrentRun.FilePath )
            {
                CurrentGame.NewSplitsFile(CurrentRun.FilePath);

                //if ( CurrentGame.SplitsFile.bAvailable ){ NotifyAll("Splits changed"); }
                NotifyAll("Splits changed");
            }
        }

    }
}
