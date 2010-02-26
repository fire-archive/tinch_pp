#ifndef NODE_MSG_H
#define NODE_MSG_H

#include "boost/utility.hpp"
#include <vector>

namespace tinch_pp {

class node_msg : boost::noncopyable {

public:
  node_msg(const std::vector<char> a_raw_msg);

  void print() const;

 private:
  mutable std::vector<char> raw_msg; // we need to get non-const iterators
};

}

#endif
