#pragma once

struct fz_context;

namespace pdft::media {

class MupdfContext {
 public:
    enum StorageSize : int {
        FZ_STORE_UNLIMITED = 0,
        FZ_STORE_DEFAULT = 256 << 20,
    };

    struct Builder {
        Builder& setStorageSize(int size);

        MupdfContext build();

     private:
        int size_ = FZ_STORE_DEFAULT;
    };

    MupdfContext(MupdfContext&& rhs);
    MupdfContext& operator=(MupdfContext&&) = delete;
    MupdfContext(const MupdfContext&) = delete;
    MupdfContext& operator=(const MupdfContext&) = delete;

    ~MupdfContext() noexcept;

    fz_context* native() const;
    void registerDocumentHandlers() const;

    static MupdfContext& thisThreadInstance();

 private:
    explicit MupdfContext(fz_context* context) : context_(context) {}

    fz_context* context_ = nullptr;
};

}  // namespace pdft::media
