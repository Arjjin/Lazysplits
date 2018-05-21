// <auto-generated>
//     Generated by the protocol buffer compiler.  DO NOT EDIT!
//     source: LzsPipeProtoCs.proto
// </auto-generated>
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
namespace LiveSplit.Lazysplits.Proto {

  /// <summary>Holder for reflection information generated from LzsPipeProtoCs.proto</summary>
  public static partial class LzsPipeProtoCsReflection {

    #region Descriptor
    /// <summary>File descriptor for LzsPipeProtoCs.proto</summary>
    public static pbr::FileDescriptor Descriptor {
      get { return descriptor; }
    }
    private static pbr::FileDescriptor descriptor;

    static LzsPipeProtoCsReflection() {
      byte[] descriptorData = global::System.Convert.FromBase64String(
          string.Concat(
            "ChRMenNQaXBlUHJvdG9Dcy5wcm90bxIaTGl2ZVNwbGl0LkxhenlzcGxpdHMu",
            "UHJvdG8i9AEKCkNwcE1lc3NhZ2USCgoCaWQYASABKAUSQAoEdHlwZRgCIAEo",
            "DjIyLkxpdmVTcGxpdC5MYXp5c3BsaXRzLlByb3RvLkNwcE1lc3NhZ2UuTWVz",
            "c2FnZVR5cGUSFwoPc2hhcmVkX2RhdGFfZGlyGAMgASgJEhEKCWdhbWVfbmFt",
            "ZRgEIAEoCRITCgt0YXJnZXRfbmFtZRgFIAEoCRIYChB0YXJnZXRfdGltZXN0",
            "YW1wGAYgASgEIj0KC01lc3NhZ2VUeXBlEggKBE5PTkUQABISCg5SRVFVRVNU",
            "X1JFU1lOQxABEhAKDFRBUkdFVF9GT1VORBACItECCglDc01lc3NhZ2USCgoC",
            "aWQYASABKAUSPwoEdHlwZRgCIAEoDjIxLkxpdmVTcGxpdC5MYXp5c3BsaXRz",
            "LlByb3RvLkNzTWVzc2FnZS5NZXNzYWdlVHlwZRIXCg9zaGFyZWRfZGF0YV9k",
            "aXIYAyABKAkSEQoJZ2FtZV9uYW1lGAQgASgJEhMKC3RhcmdldF9uYW1lGAUg",
            "ASgJEkwKD3dhdGNoX3ZhcmlhYmxlcxgGIAMoCzIzLkxpdmVTcGxpdC5MYXp5",
            "c3BsaXRzLlByb3RvLkNzTWVzc2FnZS5XYXRjaFZhcmlhYmxlGiwKDVdhdGNo",
            "VmFyaWFibGUSDAoEbmFtZRgBIAEoCRINCgV2YWx1ZRgCIAEoCSI6CgtNZXNz",
            "YWdlVHlwZRIICgROT05FEAASEQoNQ0xFQVJfVEFSR0VUUxABEg4KCk5FV19U",
            "QVJHRVQQAmIGcHJvdG8z"));
      descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
          new pbr::FileDescriptor[] { },
          new pbr::GeneratedClrTypeInfo(null, new pbr::GeneratedClrTypeInfo[] {
            new pbr::GeneratedClrTypeInfo(typeof(global::LiveSplit.Lazysplits.Proto.CppMessage), global::LiveSplit.Lazysplits.Proto.CppMessage.Parser, new[]{ "Id", "Type", "SharedDataDir", "GameName", "TargetName", "TargetTimestamp" }, null, new[]{ typeof(global::LiveSplit.Lazysplits.Proto.CppMessage.Types.MessageType) }, null),
            new pbr::GeneratedClrTypeInfo(typeof(global::LiveSplit.Lazysplits.Proto.CsMessage), global::LiveSplit.Lazysplits.Proto.CsMessage.Parser, new[]{ "Id", "Type", "SharedDataDir", "GameName", "TargetName", "WatchVariables" }, null, new[]{ typeof(global::LiveSplit.Lazysplits.Proto.CsMessage.Types.MessageType) }, new pbr::GeneratedClrTypeInfo[] { new pbr::GeneratedClrTypeInfo(typeof(global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable), global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable.Parser, new[]{ "Name", "Value" }, null, null, null)})
          }));
    }
    #endregion

  }
  #region Messages
  public sealed partial class CppMessage : pb::IMessage<CppMessage> {
    private static readonly pb::MessageParser<CppMessage> _parser = new pb::MessageParser<CppMessage>(() => new CppMessage());
    private pb::UnknownFieldSet _unknownFields;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pb::MessageParser<CppMessage> Parser { get { return _parser; } }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pbr::MessageDescriptor Descriptor {
      get { return global::LiveSplit.Lazysplits.Proto.LzsPipeProtoCsReflection.Descriptor.MessageTypes[0]; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    pbr::MessageDescriptor pb::IMessage.Descriptor {
      get { return Descriptor; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CppMessage() {
      OnConstruction();
    }

    partial void OnConstruction();

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CppMessage(CppMessage other) : this() {
      id_ = other.id_;
      type_ = other.type_;
      sharedDataDir_ = other.sharedDataDir_;
      gameName_ = other.gameName_;
      targetName_ = other.targetName_;
      targetTimestamp_ = other.targetTimestamp_;
      _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CppMessage Clone() {
      return new CppMessage(this);
    }

    /// <summary>Field number for the "id" field.</summary>
    public const int IdFieldNumber = 1;
    private int id_;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int Id {
      get { return id_; }
      set {
        id_ = value;
      }
    }

    /// <summary>Field number for the "type" field.</summary>
    public const int TypeFieldNumber = 2;
    private global::LiveSplit.Lazysplits.Proto.CppMessage.Types.MessageType type_ = 0;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public global::LiveSplit.Lazysplits.Proto.CppMessage.Types.MessageType Type {
      get { return type_; }
      set {
        type_ = value;
      }
    }

    /// <summary>Field number for the "shared_data_dir" field.</summary>
    public const int SharedDataDirFieldNumber = 3;
    private string sharedDataDir_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string SharedDataDir {
      get { return sharedDataDir_; }
      set {
        sharedDataDir_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "game_name" field.</summary>
    public const int GameNameFieldNumber = 4;
    private string gameName_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string GameName {
      get { return gameName_; }
      set {
        gameName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "target_name" field.</summary>
    public const int TargetNameFieldNumber = 5;
    private string targetName_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string TargetName {
      get { return targetName_; }
      set {
        targetName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "target_timestamp" field.</summary>
    public const int TargetTimestampFieldNumber = 6;
    private ulong targetTimestamp_;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public ulong TargetTimestamp {
      get { return targetTimestamp_; }
      set {
        targetTimestamp_ = value;
      }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override bool Equals(object other) {
      return Equals(other as CppMessage);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public bool Equals(CppMessage other) {
      if (ReferenceEquals(other, null)) {
        return false;
      }
      if (ReferenceEquals(other, this)) {
        return true;
      }
      if (Id != other.Id) return false;
      if (Type != other.Type) return false;
      if (SharedDataDir != other.SharedDataDir) return false;
      if (GameName != other.GameName) return false;
      if (TargetName != other.TargetName) return false;
      if (TargetTimestamp != other.TargetTimestamp) return false;
      return Equals(_unknownFields, other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override int GetHashCode() {
      int hash = 1;
      if (Id != 0) hash ^= Id.GetHashCode();
      if (Type != 0) hash ^= Type.GetHashCode();
      if (SharedDataDir.Length != 0) hash ^= SharedDataDir.GetHashCode();
      if (GameName.Length != 0) hash ^= GameName.GetHashCode();
      if (TargetName.Length != 0) hash ^= TargetName.GetHashCode();
      if (TargetTimestamp != 0UL) hash ^= TargetTimestamp.GetHashCode();
      if (_unknownFields != null) {
        hash ^= _unknownFields.GetHashCode();
      }
      return hash;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override string ToString() {
      return pb::JsonFormatter.ToDiagnosticString(this);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void WriteTo(pb::CodedOutputStream output) {
      if (Id != 0) {
        output.WriteRawTag(8);
        output.WriteInt32(Id);
      }
      if (Type != 0) {
        output.WriteRawTag(16);
        output.WriteEnum((int) Type);
      }
      if (SharedDataDir.Length != 0) {
        output.WriteRawTag(26);
        output.WriteString(SharedDataDir);
      }
      if (GameName.Length != 0) {
        output.WriteRawTag(34);
        output.WriteString(GameName);
      }
      if (TargetName.Length != 0) {
        output.WriteRawTag(42);
        output.WriteString(TargetName);
      }
      if (TargetTimestamp != 0UL) {
        output.WriteRawTag(48);
        output.WriteUInt64(TargetTimestamp);
      }
      if (_unknownFields != null) {
        _unknownFields.WriteTo(output);
      }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int CalculateSize() {
      int size = 0;
      if (Id != 0) {
        size += 1 + pb::CodedOutputStream.ComputeInt32Size(Id);
      }
      if (Type != 0) {
        size += 1 + pb::CodedOutputStream.ComputeEnumSize((int) Type);
      }
      if (SharedDataDir.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(SharedDataDir);
      }
      if (GameName.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(GameName);
      }
      if (TargetName.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(TargetName);
      }
      if (TargetTimestamp != 0UL) {
        size += 1 + pb::CodedOutputStream.ComputeUInt64Size(TargetTimestamp);
      }
      if (_unknownFields != null) {
        size += _unknownFields.CalculateSize();
      }
      return size;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(CppMessage other) {
      if (other == null) {
        return;
      }
      if (other.Id != 0) {
        Id = other.Id;
      }
      if (other.Type != 0) {
        Type = other.Type;
      }
      if (other.SharedDataDir.Length != 0) {
        SharedDataDir = other.SharedDataDir;
      }
      if (other.GameName.Length != 0) {
        GameName = other.GameName;
      }
      if (other.TargetName.Length != 0) {
        TargetName = other.TargetName;
      }
      if (other.TargetTimestamp != 0UL) {
        TargetTimestamp = other.TargetTimestamp;
      }
      _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(pb::CodedInputStream input) {
      uint tag;
      while ((tag = input.ReadTag()) != 0) {
        switch(tag) {
          default:
            _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
            break;
          case 8: {
            Id = input.ReadInt32();
            break;
          }
          case 16: {
            type_ = (global::LiveSplit.Lazysplits.Proto.CppMessage.Types.MessageType) input.ReadEnum();
            break;
          }
          case 26: {
            SharedDataDir = input.ReadString();
            break;
          }
          case 34: {
            GameName = input.ReadString();
            break;
          }
          case 42: {
            TargetName = input.ReadString();
            break;
          }
          case 48: {
            TargetTimestamp = input.ReadUInt64();
            break;
          }
        }
      }
    }

    #region Nested types
    /// <summary>Container for nested types declared in the CppMessage message type.</summary>
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static partial class Types {
      public enum MessageType {
        [pbr::OriginalName("NONE")] None = 0,
        [pbr::OriginalName("REQUEST_RESYNC")] RequestResync = 1,
        [pbr::OriginalName("TARGET_FOUND")] TargetFound = 2,
      }

    }
    #endregion

  }

  public sealed partial class CsMessage : pb::IMessage<CsMessage> {
    private static readonly pb::MessageParser<CsMessage> _parser = new pb::MessageParser<CsMessage>(() => new CsMessage());
    private pb::UnknownFieldSet _unknownFields;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pb::MessageParser<CsMessage> Parser { get { return _parser; } }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pbr::MessageDescriptor Descriptor {
      get { return global::LiveSplit.Lazysplits.Proto.LzsPipeProtoCsReflection.Descriptor.MessageTypes[1]; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    pbr::MessageDescriptor pb::IMessage.Descriptor {
      get { return Descriptor; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CsMessage() {
      OnConstruction();
    }

    partial void OnConstruction();

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CsMessage(CsMessage other) : this() {
      id_ = other.id_;
      type_ = other.type_;
      sharedDataDir_ = other.sharedDataDir_;
      gameName_ = other.gameName_;
      targetName_ = other.targetName_;
      watchVariables_ = other.watchVariables_.Clone();
      _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CsMessage Clone() {
      return new CsMessage(this);
    }

    /// <summary>Field number for the "id" field.</summary>
    public const int IdFieldNumber = 1;
    private int id_;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int Id {
      get { return id_; }
      set {
        id_ = value;
      }
    }

    /// <summary>Field number for the "type" field.</summary>
    public const int TypeFieldNumber = 2;
    private global::LiveSplit.Lazysplits.Proto.CsMessage.Types.MessageType type_ = 0;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public global::LiveSplit.Lazysplits.Proto.CsMessage.Types.MessageType Type {
      get { return type_; }
      set {
        type_ = value;
      }
    }

    /// <summary>Field number for the "shared_data_dir" field.</summary>
    public const int SharedDataDirFieldNumber = 3;
    private string sharedDataDir_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string SharedDataDir {
      get { return sharedDataDir_; }
      set {
        sharedDataDir_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "game_name" field.</summary>
    public const int GameNameFieldNumber = 4;
    private string gameName_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string GameName {
      get { return gameName_; }
      set {
        gameName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "target_name" field.</summary>
    public const int TargetNameFieldNumber = 5;
    private string targetName_ = "";
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public string TargetName {
      get { return targetName_; }
      set {
        targetName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
      }
    }

    /// <summary>Field number for the "watch_variables" field.</summary>
    public const int WatchVariablesFieldNumber = 6;
    private static readonly pb::FieldCodec<global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable> _repeated_watchVariables_codec
        = pb::FieldCodec.ForMessage(50, global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable.Parser);
    private readonly pbc::RepeatedField<global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable> watchVariables_ = new pbc::RepeatedField<global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable>();
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public pbc::RepeatedField<global::LiveSplit.Lazysplits.Proto.CsMessage.Types.WatchVariable> WatchVariables {
      get { return watchVariables_; }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override bool Equals(object other) {
      return Equals(other as CsMessage);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public bool Equals(CsMessage other) {
      if (ReferenceEquals(other, null)) {
        return false;
      }
      if (ReferenceEquals(other, this)) {
        return true;
      }
      if (Id != other.Id) return false;
      if (Type != other.Type) return false;
      if (SharedDataDir != other.SharedDataDir) return false;
      if (GameName != other.GameName) return false;
      if (TargetName != other.TargetName) return false;
      if(!watchVariables_.Equals(other.watchVariables_)) return false;
      return Equals(_unknownFields, other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override int GetHashCode() {
      int hash = 1;
      if (Id != 0) hash ^= Id.GetHashCode();
      if (Type != 0) hash ^= Type.GetHashCode();
      if (SharedDataDir.Length != 0) hash ^= SharedDataDir.GetHashCode();
      if (GameName.Length != 0) hash ^= GameName.GetHashCode();
      if (TargetName.Length != 0) hash ^= TargetName.GetHashCode();
      hash ^= watchVariables_.GetHashCode();
      if (_unknownFields != null) {
        hash ^= _unknownFields.GetHashCode();
      }
      return hash;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override string ToString() {
      return pb::JsonFormatter.ToDiagnosticString(this);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void WriteTo(pb::CodedOutputStream output) {
      if (Id != 0) {
        output.WriteRawTag(8);
        output.WriteInt32(Id);
      }
      if (Type != 0) {
        output.WriteRawTag(16);
        output.WriteEnum((int) Type);
      }
      if (SharedDataDir.Length != 0) {
        output.WriteRawTag(26);
        output.WriteString(SharedDataDir);
      }
      if (GameName.Length != 0) {
        output.WriteRawTag(34);
        output.WriteString(GameName);
      }
      if (TargetName.Length != 0) {
        output.WriteRawTag(42);
        output.WriteString(TargetName);
      }
      watchVariables_.WriteTo(output, _repeated_watchVariables_codec);
      if (_unknownFields != null) {
        _unknownFields.WriteTo(output);
      }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int CalculateSize() {
      int size = 0;
      if (Id != 0) {
        size += 1 + pb::CodedOutputStream.ComputeInt32Size(Id);
      }
      if (Type != 0) {
        size += 1 + pb::CodedOutputStream.ComputeEnumSize((int) Type);
      }
      if (SharedDataDir.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(SharedDataDir);
      }
      if (GameName.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(GameName);
      }
      if (TargetName.Length != 0) {
        size += 1 + pb::CodedOutputStream.ComputeStringSize(TargetName);
      }
      size += watchVariables_.CalculateSize(_repeated_watchVariables_codec);
      if (_unknownFields != null) {
        size += _unknownFields.CalculateSize();
      }
      return size;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(CsMessage other) {
      if (other == null) {
        return;
      }
      if (other.Id != 0) {
        Id = other.Id;
      }
      if (other.Type != 0) {
        Type = other.Type;
      }
      if (other.SharedDataDir.Length != 0) {
        SharedDataDir = other.SharedDataDir;
      }
      if (other.GameName.Length != 0) {
        GameName = other.GameName;
      }
      if (other.TargetName.Length != 0) {
        TargetName = other.TargetName;
      }
      watchVariables_.Add(other.watchVariables_);
      _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(pb::CodedInputStream input) {
      uint tag;
      while ((tag = input.ReadTag()) != 0) {
        switch(tag) {
          default:
            _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
            break;
          case 8: {
            Id = input.ReadInt32();
            break;
          }
          case 16: {
            type_ = (global::LiveSplit.Lazysplits.Proto.CsMessage.Types.MessageType) input.ReadEnum();
            break;
          }
          case 26: {
            SharedDataDir = input.ReadString();
            break;
          }
          case 34: {
            GameName = input.ReadString();
            break;
          }
          case 42: {
            TargetName = input.ReadString();
            break;
          }
          case 50: {
            watchVariables_.AddEntriesFrom(input, _repeated_watchVariables_codec);
            break;
          }
        }
      }
    }

    #region Nested types
    /// <summary>Container for nested types declared in the CsMessage message type.</summary>
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static partial class Types {
      public enum MessageType {
        [pbr::OriginalName("NONE")] None = 0,
        [pbr::OriginalName("CLEAR_TARGETS")] ClearTargets = 1,
        [pbr::OriginalName("NEW_TARGET")] NewTarget = 2,
      }

      public sealed partial class WatchVariable : pb::IMessage<WatchVariable> {
        private static readonly pb::MessageParser<WatchVariable> _parser = new pb::MessageParser<WatchVariable>(() => new WatchVariable());
        private pb::UnknownFieldSet _unknownFields;
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public static pb::MessageParser<WatchVariable> Parser { get { return _parser; } }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public static pbr::MessageDescriptor Descriptor {
          get { return global::LiveSplit.Lazysplits.Proto.CsMessage.Descriptor.NestedTypes[0]; }
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        pbr::MessageDescriptor pb::IMessage.Descriptor {
          get { return Descriptor; }
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public WatchVariable() {
          OnConstruction();
        }

        partial void OnConstruction();

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public WatchVariable(WatchVariable other) : this() {
          name_ = other.name_;
          value_ = other.value_;
          _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public WatchVariable Clone() {
          return new WatchVariable(this);
        }

        /// <summary>Field number for the "name" field.</summary>
        public const int NameFieldNumber = 1;
        private string name_ = "";
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public string Name {
          get { return name_; }
          set {
            name_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
          }
        }

        /// <summary>Field number for the "value" field.</summary>
        public const int ValueFieldNumber = 2;
        private string value_ = "";
        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public string Value {
          get { return value_; }
          set {
            value_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
          }
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public override bool Equals(object other) {
          return Equals(other as WatchVariable);
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public bool Equals(WatchVariable other) {
          if (ReferenceEquals(other, null)) {
            return false;
          }
          if (ReferenceEquals(other, this)) {
            return true;
          }
          if (Name != other.Name) return false;
          if (Value != other.Value) return false;
          return Equals(_unknownFields, other._unknownFields);
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public override int GetHashCode() {
          int hash = 1;
          if (Name.Length != 0) hash ^= Name.GetHashCode();
          if (Value.Length != 0) hash ^= Value.GetHashCode();
          if (_unknownFields != null) {
            hash ^= _unknownFields.GetHashCode();
          }
          return hash;
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public override string ToString() {
          return pb::JsonFormatter.ToDiagnosticString(this);
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public void WriteTo(pb::CodedOutputStream output) {
          if (Name.Length != 0) {
            output.WriteRawTag(10);
            output.WriteString(Name);
          }
          if (Value.Length != 0) {
            output.WriteRawTag(18);
            output.WriteString(Value);
          }
          if (_unknownFields != null) {
            _unknownFields.WriteTo(output);
          }
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public int CalculateSize() {
          int size = 0;
          if (Name.Length != 0) {
            size += 1 + pb::CodedOutputStream.ComputeStringSize(Name);
          }
          if (Value.Length != 0) {
            size += 1 + pb::CodedOutputStream.ComputeStringSize(Value);
          }
          if (_unknownFields != null) {
            size += _unknownFields.CalculateSize();
          }
          return size;
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public void MergeFrom(WatchVariable other) {
          if (other == null) {
            return;
          }
          if (other.Name.Length != 0) {
            Name = other.Name;
          }
          if (other.Value.Length != 0) {
            Value = other.Value;
          }
          _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
        }

        [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
        public void MergeFrom(pb::CodedInputStream input) {
          uint tag;
          while ((tag = input.ReadTag()) != 0) {
            switch(tag) {
              default:
                _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
                break;
              case 10: {
                Name = input.ReadString();
                break;
              }
              case 18: {
                Value = input.ReadString();
                break;
              }
            }
          }
        }

      }

    }
    #endregion

  }

  #endregion

}

#endregion Designer generated code
