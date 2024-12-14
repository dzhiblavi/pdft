#pragma once

#include "pdft/core/log.h"

struct fz_context;  // NOLINT

namespace pdft::media {

class MupdfContext {
 public:
    enum StorageSize : int {
        FZ_STORE_UNLIMITED = 0,
        FZ_STORE_DEFAULT = 256 << 20,
    };

    struct Builder {
        Builder& setStorageSize(int size);
        Builder& setLogger(log::LoggerPtr logger);

        MupdfContext build();

     private:
        int size_ = FZ_STORE_DEFAULT;
        log::LoggerPtr logger_ = log::loggerOrGlobal("mupdf");
    };

    MupdfContext(MupdfContext&& rhs) noexcept;
    MupdfContext& operator=(MupdfContext&&) = delete;
    MupdfContext(const MupdfContext&) = delete;
    MupdfContext& operator=(const MupdfContext&) = delete;

    ~MupdfContext() noexcept;

    fz_context* native() const;

    static MupdfContext& thisThreadInstance();

 private:
    explicit MupdfContext(fz_context* ctx) : context_(ctx) {}

    static MupdfContext& globalInstance();
    static MupdfContext clone(const MupdfContext& context);

    fz_context* context_ = nullptr;
};

}  // namespace pdft::media
