#include "FpsGuard.h"

FpsGuard::FpsGuard(std::function<void()> callback)
	: callback_{ std::move(callback) }
{
}

FpsGuard::~FpsGuard() {
	if (callback_) {
		callback_();
	}
}
