#ifndef FIXAPP_H
#define FIXAPP_H

#include <iostream>
#include <unordered_map>
#include <optional>

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"

#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"
#include "quickfix/fix44/NewOrderSingle.h"
#include "quickfix/fix44/ExecutionReport.h"

#include "utils.h"

enum OrderType
{
  Market = FIX::OrdType_MARKET,
  Limit,
  Stop,
  StopLimit
};

enum Side
{
  Buy = FIX::Side_BUY,
  Sell
};

// https://www.onixs.biz/fix-dictionary/4.4/tagNum_59.html
enum TimeInForce
{
  DAY,
  GTC,
  OPG,
  IOC = FIX::TimeInForce_IMMEDIATE_OR_CANCEL,
  FOK,
  GTX,
  GTD,
  CLOSE
};

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
   * @param isIncremental If isIncremental is false, then target returns MDSnapshotFullRefresh (35=W). Else, MDIncrementalRefresh is returned (35=X).
   * @param sessionID
   */
  void sendMarketDataRequest44(std::string symbol, bool isIncremental, const FIX::SessionID &sessionID);
  void onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &sessionID);
  void onMessage(const FIX44::MarketDataIncrementalRefresh &message, const FIX::SessionID &sessionID);

  /**
   * @brief Gets the fields and processes them. For now, it outputs to console.
   * @tparam Group Accepts only FIX44::(MarketDataSnapshotFullRefresh or MarketDataIncrementalRefresh)::NoMDEntries
   * @param group Should hold Bid/Offer entries
   */
  template <typename Group>
  void outputMarketData(const Group &group)
  {
    FIX::MDEntryType mdEntryType;
    group.get(mdEntryType);

    FIX::MDEntryPx mdEntryPx;
    group.get(mdEntryPx);

    FIX::MDEntrySize mdEntrySize;
    group.get(mdEntrySize);

    char entryType = mdEntryType.getValue();
    double price = mdEntryPx.getValue();
    int size = mdEntrySize.getValue();

    if (entryType == '0')
    { // BID
      std::cout << "    BID: " << price << " x " << size << std::endl;
    }
    else if (entryType == '1')
    { // OFFER
      std::cout << "    OFFER: " << price << " x " << size << std::endl;
    }
  }

  /**
   * @brief Send a NewOrderSingle message (35=D).
   * @param symbol Tag 55.
   * @param orderType Tag 40. 1=Market, 2=Limit, 3=Stop, 4=Stop Limit
   * @param side Tag 54. 1=Buy, 2=Sell
   * @param qty Tag 38
   * @param timeInForce Tag 59. 0=DAY, 1=GTC, 2=OPG, 3=IOC, 4=FOK, 5=GTX, 6=GTD, 7=CLOSE
   * @param price Optional value to be used with non-market orders
   */

  void sendNewOrderSingle44(std::string symbol, OrderType type, Side side, int qty, TimeInForce timeInForce, const FIX::SessionID &sessionID, double price = -1.0);
  void onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID &sessionID);
  void handleExecReportError(const FIX44::ExecutionReport &message, const FIX::SessionID &sessionID);
};

#endif
