/* 
   Socket.cpp

   Copyright (C) 2002-2004 Ren¨¦ Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   Ren¨¦ Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/


#include "SynSocketWrapper.h"
#include <iostream>

using namespace std;

int Socket::nofSockets_= 0;

void Socket::Start() {
  if (!nofSockets_) {
    WSADATA info;
    if (WSAStartup(MAKEWORD(2,0), &info)) {
      throw "Could not start WSA";
    }
  }
  ++nofSockets_;
}

void Socket::End() {
  WSACleanup();
}

Socket::Socket() : s_(0) {
  Start();
  // UDP: use SOCK_DGRAM instead of SOCK_STREAM
  s_ = socket(AF_INET,SOCK_STREAM,0);

  if (s_ == INVALID_SOCKET) {
    throw "INVALID_SOCKET";
  }

  refCounter_ = new int(1);
}

Socket::Socket(SOCKET s) : s_(s) {
  Start();
  refCounter_ = new int(1);
};

Socket::~Socket() {
  if (! --(*refCounter_)) {
    Close();
    delete refCounter_;
  }

  --nofSockets_;
  if (!nofSockets_) End();
}

Socket::Socket(const Socket& o) {
  refCounter_=o.refCounter_;
  (*refCounter_)++;
  s_         =o.s_;

  nofSockets_++;
}

Socket& Socket::operator=(Socket& o) {
  (*o.refCounter_)++;

  refCounter_=o.refCounter_;
  s_         =o.s_;

  nofSockets_++;

  return *this;
}

void Socket::Close() {
  closesocket(s_);
}

std::string Socket::ReceiveBytes() {
  std::string ret;
  char buf[1024];
 
  while (1) {
    u_long arg = 0;
    if (ioctlsocket(s_, FIONREAD, &arg) != 0)
      break;

    if (arg == 0)
      break;

    if (arg > 1024) arg = 1024;

    int rv = recv (s_, buf, arg, 0);
    if (rv <= 0) break;

    std::string t;

    t.assign (buf, rv);
    ret += t;
  }
 
  return ret;
}

std::string Socket::ReceiveLine() {
  std::string ret;
  while (1) {
    char r;

    switch(recv(s_, &r, 1, 0)) {
      case 0: // not connected anymore;
              // ... but last line sent
              // might not end in \n,
              // so return ret anyway.
        return ret;
      case -1:
        return "";
//      if (errno == EAGAIN) {
//        return ret;
//      } else {
//      // not connected anymore
//      return "";
//      }
    }

    ret += r;
    if (r == '\n')  return ret;
  }
}

void Socket::SendLine(std::string s) {
  s += '\n';
  send(s_,s.c_str(),s.length(),0);
}

void Socket::SendBytes(const std::string& s) {
  send(s_,s.c_str(),s.length(),0);
}


SocketClient::SocketClient(const std::string& host, int port) : Socket() {
  std::string error;

  hostent *he;
  if ((he = gethostbyname(host.c_str())) == 0) {
    error = strerror(errno);
    throw error;
  }

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr = *((in_addr *)he->h_addr);
  memset(&(addr.sin_zero), 0, 8); 

  if (::connect(s_, (sockaddr *) &addr, sizeof(sockaddr))) {
    error = strerror(WSAGetLastError());
    throw error;
  }
}