#pragma once

#define _DETAIL_CONCAT2(x, y) x##y
#define _DETAIL_CONCAT(x, y) _DETAIL_CONCAT2(x, y)

// expands to name<some unique number>
#define ANON_VAR(name) _DETAIL_CONCAT(name, __COUNTER__)
