#include "pdft/utils/background_pool.h"

namespace pdft::utils {

BackgroundPool::BackgroundPool(size_t num_threads) : pool_(num_threads) {}

BackgroundPool::~BackgroundPool() {
    requestStop();
    pool_.join();
}

void BackgroundPool::requestStop() {
    pool_.stop();
}

}  // namespace pdft::utils
