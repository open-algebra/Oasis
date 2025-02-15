#pragma once

#include <memory>

namespace Oasis {
    template <typename T>
    using UniquePtr = std::unique_ptr<T>;
}
