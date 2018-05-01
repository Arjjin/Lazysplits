using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Google.Protobuf;

namespace LiveSplit.Lazysplits.Proto
{
    static class LzsProtoHelper
    {
        private static int MsgId = 0;

        private static int GetMsgId()
        {
            return MsgId++;
        }

        public static byte[] ClearTargetMsg()
        {
            CsMessage OutMsg = new CsMessage();
            OutMsg.Id = GetMsgId();
            OutMsg.Type = CsMessage.Types.MessageType.ClearTargets;

            return OutMsg.ToByteArray();
        }

        public static byte[] NewTargetMsg( string sharedDataDir, string gameName, string targetName )
        {
            CsMessage OutMsg = new CsMessage();
            OutMsg.Id = GetMsgId();
            OutMsg.Type = CsMessage.Types.MessageType.NewTarget;
            OutMsg.SharedDataDir = sharedDataDir;
            OutMsg.GameName = gameName;
            OutMsg.TargetName = targetName;

            return OutMsg.ToByteArray();
        }
    }
}
