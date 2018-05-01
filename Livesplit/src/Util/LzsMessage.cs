
namespace LiveSplit.Lazysplits
{
    enum LsMsgType { None, PipeStatus, Protobuf }

    abstract class LsMsg
    {
        public LsMsgType Type { get; private set; }
        public LsMsg( LsMsgType type ){ Type = type; }
    }

    class LsPipeStatusMsg : LsMsg
    {
        public bool Connected { get; private set; }
        public LsPipeStatusMsg( bool connected ) : base(LsMsgType.PipeStatus)
        {
            Connected = connected;
        }
    }

    class LsProtobufMsg : LsMsg
    {
        public byte[] SerializedProtobuf { get; private set; }
        public LsProtobufMsg( byte[] serializedProtobuf ) : base(LsMsgType.Protobuf)
        {
            SerializedProtobuf = serializedProtobuf;
        }
    }
}
