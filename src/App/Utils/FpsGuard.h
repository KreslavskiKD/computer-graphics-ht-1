#ifndef MORPHING_FPSGUARD
#define MORPHING_FPSGUARD

#include <functional>

class FpsGuard final {
public:
	explicit FpsGuard(std::function<void()> callback);
	~FpsGuard();

	FpsGuard(const FpsGuard &) = delete;
	FpsGuard(FpsGuard &&) = delete;

	FpsGuard & operator=(const FpsGuard &) = delete;
	FpsGuard & operator=(FpsGuard &&) = delete;

private:
	std::function<void()> callback_;
};

#endif // MORPHING_FPSGUARD
