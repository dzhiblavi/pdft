#pragma once

#include "./impl/log.h"

namespace pdft::core::log {

using impl::Level;
using impl::LoggerPtr;

// Get current global logger
using impl::global;

// Get logger by name
LoggerPtr logger(const std::string& name);

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

}  // namespace pdft::core::log

namespace pdft::log {

using namespace core::log;

}  // namespace pdft::log
