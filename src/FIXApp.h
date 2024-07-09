#ifndef FIXAPP_H
#define FIXAPP_H

#include "quickfix/Application.h"
#include "quickfix/MessageCracker.h"

class FIXApp : public FIX::Application, public FIX::MessageCracker {
  public:
    void onCreate(const FIX::SessionID&) override;
    void onLogon(const FIX::SessionID&) override;
    void onLogout(const FIX::SessionID&) override;
    void toAdmin(FIX::Message&, const FIX::SessionID&) override;
    void toApp(FIX::Message&, const FIX::SessionID&) throw (FIX::DoNotSend) override;
    void fromAdmin(const FIX::Message&, const FIX::SessionID&) throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon) override;
    void fromApp(const FIX::Message&, const FIX::SessionID&) throw (FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType) override;
};

#endif
