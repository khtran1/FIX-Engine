#include "FIXApp.h"
#include <iostream>

void FIXApp::onCreate(const FIX::SessionID& sessionID) {
  std::cout << "Session created: " << sessionID << std::endl;
}

void FIXApp::onLogon(const FIX::SessionID& sessionID) {
  std::cout << "Logon - Session: " << sessionID << std::endl;
}

void FIXApp::onLogout(const FIX::SessionID& sessionID) {
  std::cout << "Logout - Session: " << sessionID << std::endl;
}

void FIXApp::toAdmin(FIX::Message& message, const FIX::SessionID& sessionID) {
  std::cout << "To Admin - Session: " << sessionID << std::endl;
  std::cout << "    " << message << std::endl;
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
