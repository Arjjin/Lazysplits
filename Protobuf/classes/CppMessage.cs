// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CppMessage.proto
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
namespace Lazysplits.Proto {

  /// <summary>Holder for reflection information generated from CppMessage.proto</summary>
  public static partial class CppMessageReflection {

    #region Descriptor
    /// <summary>File descriptor for CppMessage.proto</summary>
    public static pbr::FileDescriptor Descriptor {
      get { return descriptor; }
    }
    private static pbr::FileDescriptor descriptor;

    static CppMessageReflection() {
      byte[] descriptorData = global::System.Convert.FromBase64String(
          string.Concat(
            "ChBDcHBNZXNzYWdlLnByb3RvEhBMYXp5c3BsaXRzLlByb3RvIvkBCgpDcHBN",
            "ZXNzYWdlEhIKCm1lc3NhZ2VfaWQYASABKAUSPgoMbWVzc2FnZV90eXBlGAIg",
            "ASgOMiguTGF6eXNwbGl0cy5Qcm90by5DcHBNZXNzYWdlLk1lc3NhZ2VUeXBl",
            "EhYKDnBpcGVfY29ubmVjdGVkGAMgASgIEhQKDHRhcmdldF9pbmRleBgEIAEo",
            "DRIYChB0YXJnZXRfdGltZXN0YW1wGAUgASgEIk8KC01lc3NhZ2VUeXBlEggK",
            "BE5PTkUQABINCglQSVBFX0lORk8QARIVChFSRVFVRVNUX0NWX1RBUkdFVBAC",
            "EhAKDFRBUkdFVF9GT1VORBADYgZwcm90bzM="));
      descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
          new pbr::FileDescriptor[] { },
          new pbr::GeneratedClrTypeInfo(null, new pbr::GeneratedClrTypeInfo[] {
            new pbr::GeneratedClrTypeInfo(typeof(global::Lazysplits.Proto.CppMessage), global::Lazysplits.Proto.CppMessage.Parser, new[]{ "MessageId", "MessageType", "PipeConnected", "TargetIndex", "TargetTimestamp" }, null, new[]{ typeof(global::Lazysplits.Proto.CppMessage.Types.MessageType) }, null)
          }));
    }
    #endregion

  }
  #region Messages
  public sealed partial class CppMessage : pb::IMessage<CppMessage> {
    private static readonly pb::MessageParser<CppMessage> _parser = new pb::MessageParser<CppMessage>(() => new CppMessage());
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pb::MessageParser<CppMessage> Parser { get { return _parser; } }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public static pbr::MessageDescriptor Descriptor {
      get { return global::Lazysplits.Proto.CppMessageReflection.Descriptor.MessageTypes[0]; }
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
      messageId_ = other.messageId_;
      messageType_ = other.messageType_;
      pipeConnected_ = other.pipeConnected_;
      targetIndex_ = other.targetIndex_;
      targetTimestamp_ = other.targetTimestamp_;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public CppMessage Clone() {
      return new CppMessage(this);
    }

    /// <summary>Field number for the "message_id" field.</summary>
    public const int MessageIdFieldNumber = 1;
    private int messageId_;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int MessageId {
      get { return messageId_; }
      set {
        messageId_ = value;
      }
    }

    /// <summary>Field number for the "message_type" field.</summary>
    public const int MessageTypeFieldNumber = 2;
    private global::Lazysplits.Proto.CppMessage.Types.MessageType messageType_ = 0;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public global::Lazysplits.Proto.CppMessage.Types.MessageType MessageType {
      get { return messageType_; }
      set {
        messageType_ = value;
      }
    }

    /// <summary>Field number for the "pipe_connected" field.</summary>
    public const int PipeConnectedFieldNumber = 3;
    private bool pipeConnected_;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public bool PipeConnected {
      get { return pipeConnected_; }
      set {
        pipeConnected_ = value;
      }
    }

    /// <summary>Field number for the "target_index" field.</summary>
    public const int TargetIndexFieldNumber = 4;
    private uint targetIndex_;
    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public uint TargetIndex {
      get { return targetIndex_; }
      set {
        targetIndex_ = value;
      }
    }

    /// <summary>Field number for the "target_timestamp" field.</summary>
    public const int TargetTimestampFieldNumber = 5;
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
      if (MessageId != other.MessageId) return false;
      if (MessageType != other.MessageType) return false;
      if (PipeConnected != other.PipeConnected) return false;
      if (TargetIndex != other.TargetIndex) return false;
      if (TargetTimestamp != other.TargetTimestamp) return false;
      return true;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override int GetHashCode() {
      int hash = 1;
      if (MessageId != 0) hash ^= MessageId.GetHashCode();
      if (MessageType != 0) hash ^= MessageType.GetHashCode();
      if (PipeConnected != false) hash ^= PipeConnected.GetHashCode();
      if (TargetIndex != 0) hash ^= TargetIndex.GetHashCode();
      if (TargetTimestamp != 0UL) hash ^= TargetTimestamp.GetHashCode();
      return hash;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public override string ToString() {
      return pb::JsonFormatter.ToDiagnosticString(this);
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void WriteTo(pb::CodedOutputStream output) {
      if (MessageId != 0) {
        output.WriteRawTag(8);
        output.WriteInt32(MessageId);
      }
      if (MessageType != 0) {
        output.WriteRawTag(16);
        output.WriteEnum((int) MessageType);
      }
      if (PipeConnected != false) {
        output.WriteRawTag(24);
        output.WriteBool(PipeConnected);
      }
      if (TargetIndex != 0) {
        output.WriteRawTag(32);
        output.WriteUInt32(TargetIndex);
      }
      if (TargetTimestamp != 0UL) {
        output.WriteRawTag(40);
        output.WriteUInt64(TargetTimestamp);
      }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public int CalculateSize() {
      int size = 0;
      if (MessageId != 0) {
        size += 1 + pb::CodedOutputStream.ComputeInt32Size(MessageId);
      }
      if (MessageType != 0) {
        size += 1 + pb::CodedOutputStream.ComputeEnumSize((int) MessageType);
      }
      if (PipeConnected != false) {
        size += 1 + 1;
      }
      if (TargetIndex != 0) {
        size += 1 + pb::CodedOutputStream.ComputeUInt32Size(TargetIndex);
      }
      if (TargetTimestamp != 0UL) {
        size += 1 + pb::CodedOutputStream.ComputeUInt64Size(TargetTimestamp);
      }
      return size;
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(CppMessage other) {
      if (other == null) {
        return;
      }
      if (other.MessageId != 0) {
        MessageId = other.MessageId;
      }
      if (other.MessageType != 0) {
        MessageType = other.MessageType;
      }
      if (other.PipeConnected != false) {
        PipeConnected = other.PipeConnected;
      }
      if (other.TargetIndex != 0) {
        TargetIndex = other.TargetIndex;
      }
      if (other.TargetTimestamp != 0UL) {
        TargetTimestamp = other.TargetTimestamp;
      }
    }

    [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
    public void MergeFrom(pb::CodedInputStream input) {
      uint tag;
      while ((tag = input.ReadTag()) != 0) {
        switch(tag) {
          default:
            input.SkipLastField();
            break;
          case 8: {
            MessageId = input.ReadInt32();
            break;
          }
          case 16: {
            messageType_ = (global::Lazysplits.Proto.CppMessage.Types.MessageType) input.ReadEnum();
            break;
          }
          case 24: {
            PipeConnected = input.ReadBool();
            break;
          }
          case 32: {
            TargetIndex = input.ReadUInt32();
            break;
          }
          case 40: {
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
        [pbr::OriginalName("PIPE_INFO")] PipeInfo = 1,
        [pbr::OriginalName("REQUEST_CV_TARGET")] RequestCvTarget = 2,
        [pbr::OriginalName("TARGET_FOUND")] TargetFound = 3,
      }

    }
    #endregion

  }

  #endregion

}

#endregion Designer generated code
