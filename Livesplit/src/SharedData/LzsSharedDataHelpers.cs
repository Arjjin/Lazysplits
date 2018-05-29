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

    public class LzsCurrentGame
    {
        public bool bAvailable { get; private set; }
        //internal protobuf type
        private string GameInfoDir { get; set; }
        private GameInfo GameInfo { get; set; }
        private LzsSplitsFile SplitsFile { get; set; }
        
        //NLog
        private static Logger Log = LogManager.GetCurrentClassLogger();

        public LzsCurrentGame()
        {
            bAvailable = false;
            GameInfoDir = "";
            GameInfo = new GameInfo();
            SplitsFile = new LzsSplitsFile();
        }

        public void SetUnavailable()
        {
            bAvailable = false;
            GameInfoDir = "";
            GameInfo = new GameInfo();
            SplitsFile.SetUnavailable();
        }

        public string GetGameName()
        {
            return GameInfo.Name;
        }
        public string GetSplitsFilePath()
        {
            return SplitsFile.Path;
        }
        public List<LzsSplitTarget> GetCurrentSplitsTargets( string currentSplitName )
        {
            return SplitsFile.GetCurrentTargets(currentSplitName);
        }

        public void NewSplitsFile( string path )
        {
            SplitsFile.SetXml(path);
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

    public class LzsSplitTarget
    {
        public string Name { get; private set; }
        public List<KeyValuePair<string,string>> WatchVars { get; private set; }

        public LzsSplitTarget( string name )
        {
            Name = name;
            WatchVars = new List<KeyValuePair<string,string>>();
        }
        public void AddWatchVar( string varName, string varValue )
        {
            WatchVars.Add( new KeyValuePair<string,string>( varName, varValue ) );
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

                Log.Debug("Error parsing split XML : "+e.Message);
            }
        }
        
        //TODO clean up this mess
        public List<LzsSplitTarget> GetCurrentTargets( string currentSplitName )
        {
            List<LzsSplitTarget> TargetNames = new List<LzsSplitTarget>();

            if( bAvailable )
            {
                try
                {
                    //get global targets
                    XmlNodeList GlobalTargets = Xml.GetElementsByTagName("GlobalTargets");
                    if( GlobalTargets.Count == 1 )
                    {
                        foreach( XmlNode target in  GlobalTargets.Item(0).ChildNodes )
                        {
                            if( target["TargetName"] != null ){
                                LzsSplitTarget SplitTarget = new LzsSplitTarget( target["TargetName"].InnerText );
                                if( target["WatchVariables"] != null)
                                {
                                    foreach( XmlNode watchVariable in target["WatchVariables"].ChildNodes )
                                    {
                                        if( watchVariable["Name"] != null && watchVariable["Value"] != null )
                                        {
                                            SplitTarget.AddWatchVar( watchVariable["Name"].InnerText, watchVariable["Value"].InnerText );
                                        }
                                    }
                                }
                                TargetNames.Add(SplitTarget);
                            }
                        }
                    }

                    //get active split targets if applicable
                    if( currentSplitName != string.Empty )
                    {
                        XmlNodeList Segments = Xml.GetElementsByTagName("Segment");
                        foreach( XmlNode segment in Segments)
                        {
                            if( segment["Name"].InnerText == currentSplitName && segment["Lazysplits"] != null )
                            {
                                foreach( XmlNode target in  segment["Lazysplits"].ChildNodes )
                                {
                                    if( target["TargetName"] != null ){
                                        LzsSplitTarget SplitTarget = new LzsSplitTarget( target["TargetName"].InnerText );
                                        if( target["WatchVariables"] != null)
                                        {
                                            foreach( XmlNode watchVariable in target["WatchVariables"].ChildNodes )
                                            {
                                                if( watchVariable["Name"] != null && watchVariable["Value"] != null )
                                                {
                                                    SplitTarget.AddWatchVar( watchVariable["Name"].InnerText, watchVariable["Value"].InnerText );
                                                }
                                            }
                                        }
                                        TargetNames.Add(SplitTarget);
                                    }
                                }
                            }
                        }
                    }  
                      
                }
                catch( Exception e )
                {
                    Log.Debug("Error parsing split XML : "+e.Message);
                }
            }

            return TargetNames;
        }
    }
}
