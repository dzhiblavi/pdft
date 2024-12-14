#pragma once

#define PDFT_FZ_VERIFY(...)                                         \
    [&] {                                                           \
        fz_try(ctx) {                                               \
            __VA_ARGS__;                                            \
        }                                                           \
        fz_catch(ctx) {                                             \
            ::pdft::panic("fz (mupdf) call failed: " #__VA_ARGS__); \
        }                                                           \
    }()
