#ifndef ASYNC_TCP_IP
#define ASYNC_TCP_IP

#include "types.h"
#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <deque>
#include <utility>

namespace tinch_pp {

// Design decision: we make this class specific for the node_connections, although I believe 
// it could be more general. Let's see later...

namespace utils {
  class msg_lexer;
}

// The client registers callbacks to be invoked upon successfull completion of the 
// requested operation.
typedef boost::function<void (utils::msg_lexer&)> message_read_fn;
typedef boost::function<void ()> message_written_fn;

// All errors detected by this class are reported to a provided error handler.
// This allows the client to add some context before possibly throwing an exception.
typedef boost::function<void (const boost::system::error_code& /* reason */)> error_handler_fn;

class node_async_tcp_ip : boost::noncopyable
{
public:
  node_async_tcp_ip(boost::asio::ip::tcp::socket& connection,
		                  const error_handler_fn& error_handler);

  virtual void trigger_read(const message_read_fn& callback, utils::msg_lexer* received_msgs);

  virtual void trigger_write(const msg_seq& msg, const message_written_fn& callback);

private:
  void checked_read(const message_read_fn& callback,
		                  utils::msg_lexer* received_msgs,
		                  const boost::system::error_code& error,
		                  size_t bytes_transferred);

  void checked_write(const boost::system::error_code& error,
		                   size_t bytes_transferred);

private:
  boost::asio::ip::tcp::socket& connection;
  error_handler_fn error_handler;

  // Buffers used to handle reads and writes (the memory must be maintained during the whole 
  // asynchronous operation).
  msg_seq read_buffer;

  // Remembers the requested callback for each written message.
  typedef std::pair<msg_seq, message_written_fn> msg_and_callback;
  typedef std::deque<msg_and_callback> write_queue_type;
  write_queue_type write_queue;
};

}

#endif
