#pragma once

struct fz_context;

namespace pdft::media::pdf {

class Context {
 public:
    enum StorageSize : int {
        FZ_STORE_UNLIMITED = 0,
        FZ_STORE_DEFAULT = 256 << 20,
    };

    struct Builder {
        Builder& setStorageSize(int size);

        Context build();

     private:
        int size_ = FZ_STORE_DEFAULT;
    };

    Context(Context&& rhs);
    Context& operator=(Context&&) = delete;
    ~Context() noexcept;

    fz_context* native() const;
    void registerDocumentHandlers() const;

 private:
    explicit Context(fz_context* context) : context_(context) {}

    fz_context* context_ = nullptr;
};

}  // namespace pdft::media::pdf
