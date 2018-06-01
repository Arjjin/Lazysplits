using System;
using System.IO;
using System.Collections.Generic;

using LiveSplit.Lazysplits.Proto;

using NLog;
using Google.Protobuf;

namespace LiveSplit.Lazysplits.SharedData
{
    public class LzsCurrentGame
    {
        public bool bAvailable { get; private set; }
        //internal protobuf type
        private string GameInfoDir { get; set; }
        private GameInfo GameInfo { get; set; }
        private LzsSplitsData SplitsData { get; set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsCurrentGame()
        {
            bAvailable = false;
            GameInfoDir = "";
            GameInfo = new GameInfo();
        }

        public void SetUnavailable()
        {
            bAvailable = false;
            GameInfoDir = "";
            GameInfo = new GameInfo();
        }

        public string GetGameName()
        {
            return GameInfo.Name;
        }

        public void ParseFromDir( string gameInfoDir )
        {
            GameInfoDir = gameInfoDir;
            string GameInfoPath = GameInfoDir+"\\GameInfo.json";
            if( File.Exists(GameInfoPath) )
            {
                try
                {
                    TextReader FileReader = File.OpenText(GameInfoPath);
                    JsonParser Parser = new JsonParser(JsonParser.Settings.Default);
                    GameInfo = Parser.Parse<GameInfo>(FileReader);
                    bAvailable = true;
                    Log.Info(GameInfo.Name+" parsed from ("+GameInfoPath+")");

                }
                catch( InvalidJsonException e ){ Log.Error("error parsing GameInfo : "+e.Message); }
                catch( InvalidProtocolBufferException e ) {  Log.Error("error parsing GameInfo : "+e.Message); }
            }
            else
            {
                SetUnavailable();
                string PathString = String.IsNullOrEmpty(gameInfoDir) ? "empty path" : gameInfoDir;
                Log.Error("GameInfo.json not found in ["+PathString+"]!");
            }
        }

        public bool TryGetTarget( string targetName, ref TargetInfo targetInfoRef )
        {
            foreach( GameInfo.Types.TargetEntry targetEntry in GameInfo.Targets )
            {
                if( targetEntry.Name == targetName )
                {
                    string TargetInfoPath = GameInfoDir+targetEntry.RelativePath+"\\TargetInfo.json";
                    if( File.Exists(TargetInfoPath) )
                    {
                        try
                        {
                            TextReader FileReader = File.OpenText(TargetInfoPath);
                            JsonParser Parser = new JsonParser(JsonParser.Settings.Default);
                            targetInfoRef = Parser.Parse<TargetInfo>(FileReader);
                            return true;
                        }
                        catch( InvalidJsonException e ){
                            Log.Error("Error parsing Target : "+e.Message);
                            return false;
                        }
                        catch( InvalidProtocolBufferException e ){
                            Log.Error("Error parsing Target : "+e.Message);
                            return false;
                        }
                    }
                    else
                    {
                        Log.Error("TargetInfo.json not found in ["+TargetInfoPath+"]!");
                        return false;
                    }
                }
            }

            return false;
        }

    }
}
