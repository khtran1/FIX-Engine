// FIXApp.cpp

#include "FIXApp.h"
#include <iostream>

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

void FIXApp::sendMarketDataRequest44(std::string symbol, bool isTypeX, const FIX::SessionID &sessionID)
{
  FIX44::MarketDataRequest message(
      // fields 262, 263, 264 respectively
      FIX::MDReqID(symbol + "-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count())),
      FIX::SubscriptionRequestType('1'),
      FIX::MarketDepth(1));

  // if isTypeX = false, then request 35=W. Else, request 35=X
  message.set(FIX::MDUpdateType(isTypeX));

  // Bid part of tag 269
  FIX44::MarketDataRequest::NoMDEntryTypes groupBid;
  groupBid.set(FIX::MDEntryType(FIX::MDEntryType_BID));
  message.addGroup(groupBid);

  // Offer art of tag 269
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
  message.get(reqID);

  FIX::Symbol symbol;
  message.get(symbol);

  FIX::SendingTime time;
  message.getHeader().get(time);

  std::cout << symbol << " - " << time << std::endl;

  FIX::NoMDEntries noMDEntries;
  message.get(noMDEntries);

  for (int i = 1; i <= noMDEntries; i++)
  {
    FIX44::MarketDataSnapshotFullRefresh::NoMDEntries group;
    message.getGroup(i, group);

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

  std::cout << "    ReqID: " << reqID << std::endl;

  std::cout << "    35=W" << std::endl;

  std::cout << std::endl;
}

void FIXApp::onMessage(const FIX44::MarketDataIncrementalRefresh &message, const FIX::SessionID &sessionID)
{
  FIX::MDReqID reqID;
  message.get(reqID);

  FIX::Symbol symbol;
  message.getField(symbol);

  FIX::SendingTime time;
  message.getHeader().get(time);

  std::cout << symbol << " - " << time << std::endl;

  FIX::NoMDEntries noMDEntries;
  message.get(noMDEntries);

  for (int i = 1; i <= noMDEntries; i++)
  {
    FIX44::MarketDataIncrementalRefresh::NoMDEntries group;
    message.getGroup(i, group);

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

  std::cout << "    ReqID: " << reqID << std::endl;

  std::cout << "    35=X" << std::endl;

  std::cout << std::endl;
}

void FIXApp::sendNewOrderSingle44(std::string symbol, char orderType, char side, int qty, char timeInForce, const FIX::SessionID &sessionID)
{
  FIX44::NewOrderSingle message(
      FIX::ClOrdID("1234"),
      FIX::Side(side),
      FIX::TransactTime(),
      FIX::OrdType(orderType));
}

void FIXApp::onMessage(const FIX44::NewOrderSingle &message, const FIX::SessionID &sessionID)
{
  std::cout << "NewOrderSingle" << std::endl;
  std::cout << message << std::endl;
}