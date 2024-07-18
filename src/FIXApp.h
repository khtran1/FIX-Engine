#ifndef FIXAPP_H
#define FIXAPP_H

#include <unordered_map>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"

#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"
#include "quickfix/fix44/NewOrderSingle.h"

#include "utils.h"

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

  /**
   * @brief Send a MarketDataRequest message.
   * @param symbol Tag 55
   * @param isTypeX If isTypeX is false, then target returns MDSnapshotFullRefresh (35=W). Else, MDIncrementalRefresh is returned (35=X).
   * @param sessionID
   */
  void sendMarketDataRequest44(std::string symbol, bool isTypeX, const FIX::SessionID &sessionID);
  void onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &sessionID);
  void onMessage(const FIX44::MarketDataIncrementalRefresh &message, const FIX::SessionID &sessionID);

  /**
   * @brief Send a NewOrderSingle message (35=D).
   * @param symbol Tag 55.
   * @param orderType Tag 40. 1=Market, 2=Limit, 3=Stop, 4=Stop Limit
   * @param side Tag 54. 1=Buy, 2=Sell
   * @param qty Tag 38
   * @param timeInForce Tag 59. 0=DAY, 1=GTC, 2=OPG, 3=IOC, 4=FOK, 5=GTX, 6=GTD, 7=CLOSE
   */
  void sendNewOrderSingle44(std::string symbol, char orderType, char side, int qty, char timeInForce, const FIX::SessionID &sessionID);
  void onMessage(const FIX44::NewOrderSingle &message, const FIX::SessionID &sessionID);
};

#endif
