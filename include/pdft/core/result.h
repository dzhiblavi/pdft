#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/status.h"

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include "pdft/core/result/combine/and_then.h"
#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/combine/map.h"
#include "pdft/core/result/combine/map_err.h"
#include "pdft/core/result/combine/or_else.h"
#include "pdft/core/result/combine/unwrap.h"

#include "pdft/core/result/syntax/pipe.h"
