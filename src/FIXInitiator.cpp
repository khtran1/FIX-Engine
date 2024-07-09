#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"

#include "FIXApp.h"

int main(int argc, char** argv) {
  try {
    FIX::SessionSettings settings("config/fix.cfg");

    FIXApp application;
    FIX::FileStoreFactory storeFactory(settings);
    FIX::FileLogFactory logFactory(settings);
    FIX::SocketInitiator Initiator(application, storeFactory, settings, logFactory);
    
    Initiator.start();
    // while( condition == true ) { do something; }
    Initiator.stop();

    return 0;
  } catch (FIX::ConfigError& e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}