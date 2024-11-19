#pragma once

#include "pdft/media/pdf/context.h"

namespace app {

void setupLogging();

void setupMupdfLogging(const pdft::media::pdf::Context& context);

}  // namespace app
