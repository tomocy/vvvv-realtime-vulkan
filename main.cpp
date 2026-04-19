#include <iostream>

#include "error.h"

namespace {
vvvv::Error run();
} // namespace

int main()
{
    const auto err = run();
    if (err.has()) {
        std::cerr << "Error: " << err.message() << "\n";
        return 1;
    }
    return 0;
}

namespace {
vvvv::Error run()
{
    std::cout << "Completed\n";
    return vvvv::Error::none();
}
} // namespace
