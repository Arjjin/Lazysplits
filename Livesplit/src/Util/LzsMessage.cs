
namespace LiveSplit.Lazysplits
{
    public enum LsMsgType { None, PipeStatus, PipeData, Protobuf }
    public enum LsPipeDataType { None, Sent, Received }

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

    class LsPipeDataMsg : LsMsg
    {
        public LsPipeDataType DataType { get; private set; }
        public LsPipeDataMsg( LsPipeDataType dataType ) : base(LsMsgType.PipeData)
        {
            DataType = dataType;
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
