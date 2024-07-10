#ifndef FIXINIT_H
#define FIXINIT_H

#include "quickfix/FileStore.h"
#include "quickfix/FileLog.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/Session.h"
#include "quickfix/SessionSettings.h"

#include "quickfix/fix44/MarketDataRequest.h"

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <chrono>

// Add creds file at path as mappings to outparam creds
void loadCreds(std::unordered_map<std::string, std::string> &creds, std::string path);

#endif