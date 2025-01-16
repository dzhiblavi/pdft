#pragma once

#include "platform/module/module.h"
#include "platform/utils/pinned.h"

struct fz_context;  // NOLINT

namespace pdft::media {

class MupdfContext {
 public:
    virtual ~MupdfContext() = default;
    virtual fz_context* native() const = 0;

 protected:
    static void drop(fz_context* ctx) noexcept;

    using ContextDeleter = decltype(&MupdfContext::drop);
    using ContextHolder = std::unique_ptr<fz_context, ContextDeleter>;
};

class MupdfContextOwner
    : public MupdfContext,
      public platform::rt::Module,
      public platform::utils::Pinned,
      public std::enable_shared_from_this<MupdfContextOwner> {
 public:
    struct Config {
        // FZ_STORE_DEFAULT
        static constexpr int DefaultStorageSize = 256 << 20;
        static constexpr int UseDefault = 0;

        int size = DefaultStorageSize;
        int text_aa_level = UseDefault;
        int graphics_aa_level = UseDefault;
        bool register_as_global = false;
        bool enable_icc = true;
        bool setup_logging = true;
    };

    MupdfContextOwner(Config config);

    fz_context* native() const override;
    void start() override;

 private:
    Config config_;
    std::array<std::mutex, 3> locks_;

    // This thing must be destroyed BEFORE locks_ because
    // the destruction involves using them.
    ContextHolder context_{nullptr, &MupdfContext::drop};
};

class ClonedMupdfContext : public MupdfContext {
 public:
    ClonedMupdfContext(fz_context* context, platform::Arc<MupdfContext> parent);

    fz_context* native() const override;

    static platform::Arc<MupdfContext> clone(platform::Arc<MupdfContext> from);

 private:
    platform::Arc<MupdfContext> parent_;

    // This thing must be destroyed BEFORE parent_ because
    // the destruction may involve using shared resources.
    ContextHolder context_{nullptr, &MupdfContext::drop};
};

void setGlobalMupdfContext(platform::Arc<MupdfContext> ctx);

MupdfContext& thisThreadMupdfContext();

}  // namespace pdft::media
