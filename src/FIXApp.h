#ifndef FIXAPP_H
#define FIXAPP_H

#include <unordered_map>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"

#include "CountdownLatch.h"

class FIXApp : public FIX::Application, public FIX::MessageCracker
{
public:
  CountdownLatch latch;
  std::unordered_map<std::string, std::string> creds;

  void onCreate(const FIX::SessionID &) override;
  void onLogon(const FIX::SessionID &) override;
  void onLogout(const FIX::SessionID &) override;
  void toAdmin(FIX::Message &, const FIX::SessionID &) override;
  void toApp(FIX::Message &, const FIX::SessionID &) throw(FIX::DoNotSend) override;
  void fromAdmin(const FIX::Message &, const FIX::SessionID &) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) override;
  void fromApp(const FIX::Message &, const FIX::SessionID &) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) override;

  // 35=V
  void sendMarketDataRequest44(std::string symbol, bool isTypeX, const FIX::SessionID &sessionID);
};

#endif
