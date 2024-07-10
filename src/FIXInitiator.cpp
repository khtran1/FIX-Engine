// FIXInitiator.cpp

#include "FIXInitiator.h"
#include "FIXApp.h"

void loadCreds(std::unordered_map<std::string, std::string>& creds, std::string path) {
  std::ifstream credsFile(path);
  std::string line;

  while (std::getline(credsFile, line)) {
      auto pos = line.find('=');
      if (pos != std::string::npos) {
          std::string key = line.substr(0, pos);
          std::string value = line.substr(pos + 1);
          creds[key] = value;
      }
  }
}

int main(int argc, char** argv) {
  try {
    FIX::SessionSettings settings("config/fix.cfg");

    FIXApp application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::FileLogFactory logFactory(settings);
    FIX::SocketInitiator Initiator(application, storeFactory, settings, logFactory);
    
    loadCreds(application.creds, "config/creds.cfg");

    Initiator.start();

    std::cout << "Awaiting logon..." << std::endl;
    application.latch.await();
    std::cout << "Done." << std::endl;

    // Awaits user input before logging out
    std::cin.get();
    std::cout << "Logging out..." << std::endl;
    Initiator.stop();

    return 0;
  } catch (FIX::ConfigError& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}