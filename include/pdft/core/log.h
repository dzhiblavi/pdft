#pragma once

#include "pdft/core/log/common_adapters.h"
#include "pdft/core/log/impl/log.h"

namespace pdft::log {

using impl::Level;
using impl::LoggerPtr;

// Get current global logger
using impl::global;

// Get logger by name
LoggerPtr logger(const std::string& name);

// Get logger by name or global
LoggerPtr loggerOrGlobal(const std::string& name);

// Set logger by name
void setGlobalLogger(LoggerPtr logger);

// Helper functions
using impl::critical;
using impl::debug;
using impl::err;
using impl::info;
using impl::log;
using impl::trace;
using impl::warn;

}  // namespace pdft::log
