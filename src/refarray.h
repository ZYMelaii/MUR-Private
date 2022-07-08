#ifndef REFARRAY_H
#define REFARRAY_H

#include "./common.h"
#include <type_traits>

/**
 * @brief 运行时定长数组封装
 * @author zymelaii
 * @date 2022-06-27
 */
template <typename T>
class RefArray {
public:
	using value_type = std::remove_reference_t<T>;
public:
	RefArray() = delete;
	RefArray(const RefArray &rhs) {
		array_ = rhs.array_;
		size_ = rhs.size_;
	}
	RefArray(T *array, size_t n) :
		array_(static_cast<value_type*>(array)), size_(n) {
	}
	template <typename G, std::enable_if_t<
		std::is_same_v<value_type, typename RefArray<G>::value_type>>>
	RefArray(RefArray<G> &&rhs) {
		array_ = rhs.array_;
		size_ = rhs.size_;
		rhs.array_ = nullptr;
		rhs.size_ = 0;
	}
	~RefArray() {
		array_ = nullptr;
		size_ = 0;
	}
public:
	template <typename G>
	bool operator==(const RefArray<G> &rhs) {
		using other_type = typename RefArray<G>::value_type;
		constexpr bool is_same_type = std::is_same_v<value_type, other_type>;
		return is_same_type && array_ == rhs.array_;
	}
	template <typename G>
	bool operator!=(const RefArray<G> &rhs) {
		using other_type = typename RefArray<G>::value_type;
		constexpr bool is_same_type = std::is_same_v<value_type, other_type>;
		return !is_same_type || array_ != rhs.array_;
	}
	value_type& operator[](int index) {
		assert(index >= 0 && index < size_);
		return array_[index];
	}
	const value_type& operator[](int index) const {
		assert(index >= 0 && index < size_);
		return array_[index];
	}
	size_t size() const {
		return size_;
	}
private:
	value_type *array_;
	size_t size_;
};

#endif /*REFARRAY_H*/