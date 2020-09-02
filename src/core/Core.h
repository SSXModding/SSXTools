#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <type_traits>
#include <functional>
#include <cstdint>
#include <stdint.h>

namespace eagle {
	namespace core {

		/**
		 * Static assert helper for enforcing POD types while not being ugly.
		 * Forward-compatible with C++20's deprecation of is_pod<T>.
		 *
		 * \tparam T type to assert is POD.
		 */
		template<class T>
		struct PodStaticAssert {
#if __cplusplus >= 201704L
			static_assert(std::is_standard_layout<T>::value, ""Given type is not Plain Old Data");
#else
			static_assert(std::is_pod<T>::value, "Given type is not Plain Old Data");
#endif
		};

#if defined(__GNUC__) || defined(__clang__)
	#define EAGLE_UNUSED __attribute__((unused))
#elif defined(_MSC_VER)
	// msvc doesn't care about unused values
	// at least it doesnt screech
	#define EAGLE_UNUSED
#endif

		/**
		 * Macro to easily assert a type is POD using PodStaticAssert.
		 * Multiple EAGLE_REQUIRES_POD values can be stored.
		 */
#if defined(__GNUC__) || defined(__clang__)
	#define EAGLE_REQUIRES_POD(T) constexpr static __attribute__((unused)) PodStaticAssert<T> _##T;
#else
	#define EAGLE_REQUIRES_POD(T) constexpr static PodStaticAssert<T> _##T;
#endif

		typedef std::uint8_t byte;
		typedef std::int8_t sbyte;

		typedef std::int16_t int16;
		typedef std::uint16_t uint16;

		typedef std::int32_t int32;
		typedef std::uint32_t uint32;

		typedef std::int64_t int64;
		typedef std::uint64_t uint64;

		// TODO: Import StreamHelper and use it instead

		/**
		 * \defgroup FunTemp Function templates
		 * @{
		 */

		/** Reads value of T from a open stream
		 * T can be assumed, but, if you're doing
		 * ```cpp
		 * struct ThingHeader { things in the file ... }
		 * struct Thing : public ThingHeader { things not in the file ... }
		 * ```
		 * you can specify what T should be as well.
		 *
		 * \tparam T Type to read.
		 *
		 * \param[in] stream Stream to read from.
		 * \param[out] output Output variable.
		 */
		template<class T>
		inline bool ReadFromStream(std::istream& stream, T& output) {
			EAGLE_REQUIRES_POD(T);

			if(!stream)
				return false;

			stream.read((char*)&output, sizeof(T));
			return true;
		}

		/**
		 * Compare two objects of type T or 
		 * two objects of type T and T2.
		 * 
		 * \tparam T First type.
		 * \tparam T2 Second type.
		 *
		 * \param[in] a First object to compare.
		 * \param[in] b Second object to compare.
		 */
		template<typename T, typename T2>
		inline bool SizedCmp(T* a, T2* b) {
			// We require both operand types to be POD
			EAGLE_REQUIRES_POD(T);
			EAGLE_REQUIRES_POD(T2);

			return !memcmp(a, b, sizeof(T));
		}

		/**
		 * A non-owning wrapper object over a raw buffer sequence,
		 * that allows it to encode size in the same space.
		 * 
		 * \tparam T Type of the underlying memory.
		 */
		template<typename T>
		struct Span {
			constexpr Span() noexcept
				: memory_(nullptr),
				  size_(0) {
			}

			/**
			 * Construct a span with a valid memory address and size.
			 * 
			 * \param[in] memory The memory to wrap.
			 * \param[in] size The size of the buffer sequence.
			 */
			constexpr Span(T* memory, std::size_t size) noexcept
				: memory_(memory),
				  size_(size) {
			}

			/**
			 * Get the pointer stored in the span. 
			 */
			constexpr T* get() noexcept {
				return memory_;
			}

			/**
			 * Get the size of the buffer sequence stored in the span.
			 */
			constexpr std::size_t size() noexcept {
				return size_;
			}

		   private:
			T* memory_;
			std::size_t size_;
		};

		/**
		 * Constexpr function to make spans.
		 * 
		 * \tparam T The type of the span to create
		 */
		template<typename T>
		constexpr static Span<T> MakeSpan(T* buffer, std::size_t size) noexcept {
			return Span<T> { buffer, size };
		}

		/**
		 * std::max_element for spans
		 */
		template<typename T, class F>
		inline T MaxSpanElement(Span<T> memory, F comparision) {
			// a simple idiot check
			if(!memory.get() || memory.size() == 0)
				return {};

			// cache some frequently used things so that
			// we don't have to fetch them from the span so often
			T* ptr = memory.get();
			std::size_t size = memory.size();
			T* max = ptr;

			for(std::size_t i = 0; i < size; ++i) {
				if(comparision(*max, *ptr))
					max = ptr;
				++ptr;
			}

			return *max;
		}

		/**
		 * Read a value of type T from a buffer.
		 *
		 * Follows the same rules as ReadFromStream(),
		 * only that this is a buffer instead of a file, and
		 * each additional index travels a stride of sizeof(T) bytes.
		 *
		 * \tparam T Type to read.
		 *
		 * \param[in] buf Buffer to read from.
		 * \param[in] index Index in buffer.
		 * \param[out] output Output variable.
		 */
		template<typename T>
		inline bool ReadFromBuffer(Span<byte> buf, size_t index, T& out) {
			if(!buf.get() || buf.size() == 0)
				return false;

			if(buf.size() < index * sizeof(T))
				return false;

			memcpy(&buf[(index * sizeof(T))], &out, sizeof(T));
			return true;
		}

		/**
		 * }@
		 */

#undef EAGLE_REQUIRES_POD

	} // namespace core
} // namespace eagle
