using System;
using System.IO;

using LiveSplit.Lazysplits.Proto;

using NLog;
using Google.Protobuf;

namespace LiveSplit.Lazysplits.SharedData
{
    public class LzsGameList
    {
        //internal protobuf type
        private GameList GameList { get; set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsGameList()
        {
            GameList = new GameList();
        }

        public bool GameExists( string gameName )
        {
            foreach( GameList.Types.GameEntry game in GameList.Games )
            {
                if( game.Name == gameName ){ return true; }
            }
            return false;
        }

        public string GetGameDir( string gameName )
        {
            foreach( GameList.Types.GameEntry game in GameList.Games )
            {
                if( game.Name == gameName ){ return game.RelativePath; }
            }
            return "";
        }

        public void ParseFromDir( string gameListDir )
        {
            string GameListPath = gameListDir+"\\GameList.json";
            if( File.Exists(GameListPath) )
            {
                try
                {
                    TextReader FileReader = File.OpenText(GameListPath);
                    JsonParser Parser = new JsonParser(JsonParser.Settings.Default);
                    GameList = Parser.Parse<GameList>(FileReader);
                    Log.Info("GameList parsed from ("+GameListPath+")");
                }
                catch( InvalidJsonException e ){ Log.Error("error parsing GameList : "+e.Message); }
                catch( InvalidProtocolBufferException e ) {  Log.Error("error parsing GameList : "+e.Message); }
            }
            else
            {
                GameList = new GameList();
                string PathString = String.IsNullOrEmpty(gameListDir) ? "empty path" : gameListDir;
                Log.Error("GameList.json not found in ["+PathString+"]!");
            }
        }
    }
}
