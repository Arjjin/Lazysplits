using System;
using System.Collections.Generic;
using System.Xml;

using LiveSplit.Model;

using System.IO;
using NLog;
using Google.Protobuf;
using LiveSplit.Lazysplits.Proto;

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
                }
                catch( InvalidJsonException e ){ Log.Error("protobuf parsing error : "+e.Message); }
                catch( InvalidProtocolBufferException e ) {  Log.Error("protobuf parsing error : "+e.Message); }
            }
            else
            {
                GameList = new GameList();
                string PathString = String.IsNullOrEmpty(gameListDir) ? "empty path" : gameListDir;
                Log.Error("GameList.json not found in ["+PathString+"]!");
            }
        }
    }

    public class LzsCurrentGame
    {
        public bool bAvailable { get; private set; }
        //internal protobuf type
        public GameInfo GameInfo { get; private set; }
        public LzsSplitsFile SplitsFile { get; private set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsCurrentGame()
        {
            bAvailable = false;
            GameInfo = new GameInfo();
            SplitsFile = new LzsSplitsFile();
        }

        public void SetUnavailable()
        {
            bAvailable = false;
            GameInfo = new GameInfo();
            SplitsFile.SetUnavailable();
        }

        public void NewSplitsFile( string path )
        {
            SplitsFile.SetXml(path);
        }

        public void ParseFromDir( string gameInfoDir )
        {
            string GameInfoPath = gameInfoDir+"\\GameInfo.json";
            if( File.Exists(GameInfoPath) )
            {
                try
                {
                    TextReader FileReader = File.OpenText(GameInfoPath);
                    JsonParser Parser = new JsonParser(JsonParser.Settings.Default);
                    GameInfo = Parser.Parse<GameInfo>(FileReader);
                    bAvailable = true;
                    Log.Debug(GameInfo.Name+" parsed from ["+GameInfoPath+"]");

                }
                catch( InvalidJsonException e ){ Log.Error("protobuf parsing error : "+e.Message); }
                catch( InvalidProtocolBufferException e ) {  Log.Error("protobuf parsing error : "+e.Message); }
            }
            else
            {
                SetUnavailable();
                string PathString = String.IsNullOrEmpty(gameInfoDir) ? "empty path" : gameInfoDir;
                Log.Error("GameInfo.json not found in ["+PathString+"]!");
            }
        }
    }

    public class LzsSplitsFile
    {
        public bool bAvailable { get; private set; }
        public string Path { get; private set; }
        private XmlDocument Xml;

        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsSplitsFile()
        {
            SetUnavailable();
        }

        public void SetUnavailable(){
            bAvailable = false;
            Path = "";
            Xml = new XmlDocument();
        }

        public void SetXml( string path )
        {
            try
            {
                Xml.Load(path);
                Path = path;
                bAvailable = true;
            }
            catch( Exception e )
            {
                SetUnavailable();

                Log.Debug("Error parsing XML : "+e.Message);
            }
        }

        public List<string> GetGlobalTargets()
        {
            List<string> TargetList = new List<string>();

            if( bAvailable )
            {
                try
                {
                    foreach( XmlNode LazysplitsNode in Xml.GetElementsByTagName("Lazysplits") )
                    {
                        if( LazysplitsNode.ParentNode.Name == "Run" )
                        {
                            if( LazysplitsNode["GlobalTargets"] != null )
                            {
                                foreach( XmlNode Target in LazysplitsNode["GlobalTargets"].ChildNodes )
                                {
                                    if( Target["TargetName"] != null ){ TargetList.Add(Target["TargetName"].InnerText); }
                                }
                            }
                        }               
                    }
                }
                catch( Exception e )
                {
                    Log.Debug("Error parsing XML : "+e.Message);
                }
            }

            return TargetList;
        }
    }
}
