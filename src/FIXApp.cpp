// FIXApp.cpp

#include "FIXApp.h"

void FIXApp::onCreate(const FIX::SessionID &sessionID)
{
  std::cout << "Session created: " << sessionID << std::endl;
  this->latch.countUp();
}

void FIXApp::onLogon(const FIX::SessionID &sessionID)
{
  std::cout << "Logon - Session: " << sessionID << std::endl;
  this->latch.countDown();
}

void FIXApp::onLogout(const FIX::SessionID &sessionID)
{
  std::cout << "Logout - Session: " << sessionID << std::endl;
}

void FIXApp::toAdmin(FIX::Message &message, const FIX::SessionID &sessionID)
{
  std::cout << "To Admin - Session: " << sessionID << std::endl;

  FIX::MsgType msgtype;
  message.getHeader().getField(msgtype);

  if (msgtype == FIX::MsgType_Logon)
  {
    std::cout << "Attempting Logon: " << message << std::endl;

    // This assumes that senderCompID IS ALWAYS the username...
    std::string sender = sessionID.getSenderCompID();
    std::string password = this->creds[sender];
    std::string target = sessionID.getTargetCompID();

    message.setField(FIX::Username(sender));
    message.setField(FIX::Password(password));
  }
  else
  {
    std::cout << "    " << message << std::endl;
  }
}

void FIXApp::toApp(FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::DoNotSend)
{
  std::cout << "To App - Session: " << sessionID << std::endl;
  std::cout << "    " << message << std::endl;
}

void FIXApp::fromAdmin(const FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
{
  std::cout << "From Admin - Session: " << sessionID << std::endl;
  std::cout << "    " << message << std::endl;
}

void FIXApp::fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
{
  try
  {
    crack(message, sessionID);
  }
  catch (const FIX::UnsupportedMessageType &e)
  {
    std::cout << "Unsupported From App - Session: " << sessionID << std::endl;
    std::cout << "    " << message << std::endl;
  }
}

void FIXApp::sendMarketDataRequest44(std::string symbol, bool isIncremental, const FIX::SessionID &sessionID)
{
  FIX44::MarketDataRequest message(
      // fields 262, 263, 264 respectively
      FIX::MDReqID(symbol + "-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count())),
      FIX::SubscriptionRequestType('1'),
      FIX::MarketDepth(1));

  // if isTypeX = false, then request 35=W. Else, request 35=X
  message.set(FIX::MDUpdateType(isIncremental));

  // Bid part of tag 269
  FIX44::MarketDataRequest::NoMDEntryTypes groupBid;
  groupBid.set(FIX::MDEntryType(FIX::MDEntryType_BID));
  message.addGroup(groupBid);

  // Offer part of tag 269
  FIX44::MarketDataRequest::NoMDEntryTypes groupOffer;
  groupOffer.set(FIX::MDEntryType(FIX::MDEntryType_OFFER));
  message.addGroup(groupOffer);

  // Symbol, 55=symbol
  FIX44::MarketDataRequest::NoRelatedSym groupSymbol;
  groupSymbol.set(FIX::Symbol(symbol));
  message.addGroup(groupSymbol);

  // Send message to target
  FIX::Session::sendToTarget(message, sessionID);
}

void FIXApp::onMessage(const FIX44::MarketDataSnapshotFullRefresh &message, const FIX::SessionID &sessionID)
{
  FIX::MDReqID reqID;
  FIX::Symbol symbol;
  FIX::SendingTime time;
  FIX44::MarketDataSnapshotFullRefresh::NoMDEntries group;

  message.get(reqID);
  message.get(symbol);
  message.getHeader().get(time);

  std::cout << symbol << " - " << time << std::endl;

  FIX::NoMDEntries noMDEntries;
  message.get(noMDEntries);

  for (int i = 1; i <= noMDEntries; i++)
  {
    message.getGroup(i, group);
    outputMarketData(group);
  }

  std::cout << "    ReqID: " << reqID << std::endl;
  std::cout << "    35=W" << std::endl
            << std::endl;
}

void FIXApp::onMessage(const FIX44::MarketDataIncrementalRefresh &message, const FIX::SessionID &sessionID)
{
  FIX::MDReqID reqID;
  FIX::Symbol symbol;
  FIX::SendingTime time;
  FIX44::MarketDataIncrementalRefresh::NoMDEntries group;

  message.get(reqID);
  message.getField(symbol);
  message.getHeader().get(time);

  std::cout << symbol << " - " << time << std::endl;

  FIX::NoMDEntries noMDEntries;
  message.get(noMDEntries);

  // Read each market data group (usually bid and offer groups) and parse
  for (int i = 1; i <= noMDEntries; i++)
  {
    message.getGroup(i, group);
    outputMarketData(group);
  }

  std::cout << "    ReqID: " << reqID << std::endl;
  std::cout << "    35=X" << std::endl
            << std::endl;
}

void FIXApp::sendNewOrderSingle44(std::string symbol, OrderType type, Side side, int qty, TimeInForce timeInForce, const FIX::SessionID &sessionID, double price)
{
  // Ensure price is valid if needed (limit orders)
  if (type != Market)
  {
    if (price == -1.0)
    {
      std::cerr << "NewOrderSingle: Tried sending non-market order without stop/limit price" << std::endl;
      return;
    }
  }

  // Create a NewOrderSingle and set necessary fields
  FIX44::NewOrderSingle message(
      FIX::ClOrdID(getRandomID()),
      side,
      FIX::TransactTime(),
      FIX::OrdType(type));

  message.set(FIX::Symbol(symbol));
  message.set(FIX::OrderQty(qty));
  message.set(FIX::TimeInForce(timeInForce));

  // Non-Market orders require their respective price fields to be set
  switch (type)
  {
  case Limit:
    message.set(FIX::Price(price));
    break;
  case Stop:
    message.set(FIX::StopPx(price));
    break;
  case StopLimit:
    message.set(FIX::StopPx(price));
    break;
  default:
    break;
  }

  FIX::Session::sendToTarget(message, sessionID);
}

void FIXApp::onMessage(const FIX44::ExecutionReport &message, const FIX::SessionID &sessionID)
{
  FIX::ClOrdID clOrdID;
  FIX::Side side;
  FIX::TransactTime transactTime;
  FIX::OrdType ordType;
  FIX::Symbol symbol;
  FIX::OrderQty orderQty;
  FIX::OrdStatus ordStatus;

  message.get(clOrdID);
  message.get(side);
  message.get(transactTime);
  message.get(ordType);
  message.get(symbol);
  message.get(orderQty);
  message.get(ordStatus);

  std::cout << "Execution Report" << std::endl;
  std::cout << "  Client Order ID: " << clOrdID << std::endl;
  std::cout << "  Transaction Time: " << transactTime << std::endl;

  switch (ordType)
  {
  case Market:
  {
    if (ordStatus == FIX::OrdStatus_FILLED)
    {
      if (side == Buy)
      {
        std::cout << "  Bought " << orderQty << " ";
      }
      else if (side == Sell)
      {
        std::cout << "  Sold " << orderQty << " ";
      }

      FIX::AvgPx avgPrice;
      message.get(avgPrice);

      std::cout << symbol << " at " << avgPrice << " for " << avgPrice * orderQty << std::endl;
    }
    else
    {
      handleExecReportError(message, sessionID);
    }
    break;
  }
  case Limit:
  {
    if (ordStatus == FIX::OrdStatus_FILLED)
    {
      if (side == Buy)
      {
        std::cout << "  Filled limit buy of " << orderQty << " ";
      }
      else if (side == Sell)
      {
        std::cout << "  Filled limit sell of " << orderQty << " ";
      }

      FIX::AvgPx avgPrice;
      message.get(avgPrice);

      std::cout << symbol << " at " << avgPrice << " for " << avgPrice * orderQty << std::endl;
      break;
    }
    else
    {
      handleExecReportError(message, sessionID);
    }
  }
  }
}

void FIXApp::handleExecReportError(const FIX44::ExecutionReport &message, const FIX::SessionID &sessionID)
{
  FIX::OrdStatus ordStatus;
  FIX::Text reason;

  message.get(ordStatus);
  message.get(reason);

  switch (ordStatus)
  {
  case FIX::OrdStatus_CANCELED:
  {
    std::cout << "  Order was cancelled: " << reason << std::endl;
    break;
  }
  default:
  {
    std::cout << "  Unimplemented ExecutionReport: " << reason << std::endl;
    std::cout << message << std::endl;
    break;
  }
  }
}