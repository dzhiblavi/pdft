#include "pdft/core/result/type/error.h"
#include "pdft/core/result/type/result.h"
#include "pdft/core/result/type/status.h"
#include "pdft/core/result/type/unit.h"

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include "pdft/core/result/combine/and_then.h"
#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/combine/map.h"
#include "pdft/core/result/combine/map_err.h"
#include "pdft/core/result/combine/or_else.h"
#include "pdft/core/result/combine/unwrap.h"

#include "pdft/core/result/syntax/pipe.h"

#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"
