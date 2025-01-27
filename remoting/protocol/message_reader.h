// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_PROTOCOL_MESSAGE_READER_H_
#define REMOTING_PROTOCOL_MESSAGE_READER_H_

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/non_thread_safe.h"
#include "remoting/base/compound_buffer.h"
#include "remoting/protocol/message_decoder.h"

namespace net {
class IOBuffer;
}  // namespace net

namespace remoting {
namespace protocol {

class P2PStreamSocket;

// MessageReader reads data from the socket asynchronously and calls
// callback for each message it receives. It stops calling the
// callback as soon as the socket is closed, so the socket should
// always be closed before the callback handler is destroyed.
//
// In order to throttle the stream, MessageReader doesn't try to read
// new data from the socket until all previously received messages are
// processed by the receiver (|done_task| is called for each message).
// It is still possible that the MessageReceivedCallback is called
// twice (so that there is more than one outstanding message),
// e.g. when we the sender sends multiple messages in one TCP packet.
class MessageReader : public base::NonThreadSafe {
 public:
  typedef base::Callback<void(scoped_ptr<CompoundBuffer>, const base::Closure&)>
      MessageReceivedCallback;
  typedef base::Callback<void(int)> ReadFailedCallback;

  MessageReader();
  virtual ~MessageReader();

  // Sets the callback to be called for each incoming message.
  void SetMessageReceivedCallback(const MessageReceivedCallback& callback);

  // Starts reading from |socket|.
  void StartReading(P2PStreamSocket* socket,
                    const ReadFailedCallback& read_failed_callback);

 private:
  void DoRead();
  void OnRead(int result);
  void HandleReadResult(int result, bool* read_succeeded);
  void OnDataReceived(net::IOBuffer* data, int data_size);
  void RunCallback(scoped_ptr<CompoundBuffer> message);
  void OnMessageDone();

  ReadFailedCallback read_failed_callback_;

  P2PStreamSocket* socket_;

  // Set to true, when we have a socket read pending, and expecting
  // OnRead() to be called when new data is received.
  bool read_pending_;

  // Number of messages that we received, but haven't finished
  // processing yet, i.e. |done_task| hasn't been called for these
  // messages.
  int pending_messages_;

  bool closed_;
  scoped_refptr<net::IOBuffer> read_buffer_;

  MessageDecoder message_decoder_;

  // Callback is called when a message is received.
  MessageReceivedCallback message_received_callback_;

  base::WeakPtrFactory<MessageReader> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(MessageReader);
};

}  // namespace protocol
}  // namespace remoting

#endif  // REMOTING_PROTOCOL_MESSAGE_READER_H_
