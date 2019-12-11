#ifndef HANDLER_TEMPLATE_HPP
#define HANDLER_TEMPLATE_HPP

#include <functional>
#include <type_traits>

namespace HandlerTemplate {

template <typename RET_T, typename... ARGS_T>
class HandlerClass {
private:
	std::function<RET_T(ARGS_T...)> handler_;

public:
	constexpr HandlerClass(std::function<RET_T(ARGS_T...)>&& handler) noexcept : handler_(handler) {};
	RET_T operator()(ARGS_T... args) { return handler_(args...); };
};

template <typename, typename = void>
struct LambdaChecker : std::false_type {};

template <typename T>
struct LambdaChecker<T, std::void_t<decltype(&T::operator())>> : std::true_type {};

template <typename T>
struct LambdaType { using type = T; };

template <typename RET_T, typename CLASS_T, typename... ARGS_T>
struct LambdaType<RET_T(CLASS_T::*)(ARGS_T...) const> {
	using type = std::function<RET_T(ARGS_T...)>;
};

template <typename CALLABLE_T, typename = void>
struct FunctionizedType {
	using type = std::function<std::remove_pointer_t<CALLABLE_T>>;
};

template <typename CALLABLE_T>
struct FunctionizedType<CALLABLE_T, typename std::enable_if<LambdaChecker<CALLABLE_T>::value>::type>
{
	using type = typename LambdaType<decltype(&CALLABLE_T::operator())>::type;
};

template <typename CALLABLE_T>
auto newHandlerClass(CALLABLE_T&& logic)
{
	typename FunctionizedType<std::remove_reference_t<CALLABLE_T>>::type&& handler = logic;
	return HandlerClass(std::move(handler));
}

template <typename RET_T, typename CLASS_T, typename... ARGS_T>
auto newHandlerClass(RET_T(CLASS_T::*logic)(ARGS_T...)) {
	std::function<RET_T(CLASS_T*, ARGS_T...)>&& handler = logic;
	return HandlerClass(std::move(handler));
}

}

#endif /* HANDLER_TEMPLATE_HPP */
