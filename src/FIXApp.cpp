#include "FIXApp.h"
#include <iostream>

void FIXApp::onCreate(const FIX::SessionID& sessionID) {
  std::cout << "Session created: " << sessionID << std::endl;
  this->latch.countUp();
}

void FIXApp::onLogon(const FIX::SessionID& sessionID) {
  std::cout << "Logon - Session: " << sessionID << std::endl;
  this->latch.countDown();
}

void FIXApp::onLogout(const FIX::SessionID& sessionID) {
  std::cout << "Logout - Session: " << sessionID << std::endl;
}

void FIXApp::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {
  std::cout << "To Admin - Session: " << sessionID << std::endl;

  FIX::MsgType msgtype;
  message.getHeader().getField(msgtype);

  if (msgtype == FIX::MsgType_Logon) {
    std::cout << "Attempting Logon: " << message << std::endl;

    // This assumes that senderCompID IS ALWAYS the username...
    std::string sender = sessionID.getSenderCompID();
    std::string password = this->creds[sender];
    std::string target = sessionID.getTargetCompID();

    message.setField(FIX::Username(sender));
    message.setField(FIX::Password(password));
  } else {
    std::cout << "    " << message << std::endl;
  }
}

void FIXApp::toApp(FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::DoNotSend) {
  std::cout << "To App - Session: " << sessionID << std::endl;
  std::cout << "    " << message << std::endl;
}

void FIXApp::fromAdmin(const FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) {
  std::cout << "From Admin - Session: " << sessionID << std::endl;
  std::cout << "    " << message << std::endl;
}

void FIXApp::fromApp(const FIX::Message& message, const FIX::SessionID& sessionID) throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) {
  std::cout << "From App - Session: " << sessionID << std::endl;
  std::cout << "    " << message << std::endl;
  
  crack(message, sessionID);
}
