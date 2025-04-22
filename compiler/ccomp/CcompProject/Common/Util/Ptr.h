/*
 * Ptr.h: Utility class for handling memory safety of pointers.
 */

#ifndef COMMON_UTIL_PTR_H_
#define COMMON_UTIL_PTR_H_

#include <cstdint>
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"

/*
 * @brief Class designed to handle safe access/deletion of allocated memory.
 *
 * Works similar to std::shard_ptr<>, though incorporates safety checks on
 * de-references. Tracks reference counts, freeing memory upon no references.
 *
 * For template reasons, all functionality is expressed in a header file.
 */
template<typename T> class Ptr {
public:
	/*
	 * @brief Origin constructor- ties new allocation to Ptr.
	 *
	 * Pointer MUST be only passed directly to one Ptr<> instance (use '='
	 * operator to pass to other Ptr<> instances). Pointer can be real or
	 * nullptr.
	 *
	 * @param ptr pointer to track/safeguard
	 */
	Ptr(T* const ptr) {
		// Init.
		m_ptr    = ptr;
		m_refCnt = new size_t(0);

		// Update.
		if (m_refCnt == nullptr) {
			Print_bug("ptr() bad ref alloc");
			Terminate_bug(); // Does NOT return
		}
		else if (m_ptr != nullptr) {*m_refCnt += 1;}
	}

	/*
	 * @brief Copy constructor- ties previous allocation to Ptr.
	 *
	 * Defined specifically for STL functionality (eg queue.push()). Works
	 * similar to copy assignment operator.
	 *
	 * @param ptr Ptr<> instance to copy
	 */
	Ptr(Ptr const& ptr) {
		// Copy.
		m_ptr    = ptr.m_ptr;
		m_refCnt = ptr.m_refCnt;
		if (m_ptr != nullptr) {*m_refCnt += 1;}
	}

	/*
	 * @brief Copy assignment- ties previous allocation to Ptr.
	 *
	 * Only accepts other Ptr<> instances to ensure memory safety. Effectively
	 * destructs/overwrites previous Ptr<> instance. Increments reference count
	 * of input Ptr<>.
	 *
	 * @param ptr Ptr<> instance to copy into this instance
	 * @return          reference to overwritten Ptr<> instance
	 */
	Ptr& operator=(Ptr const& ptr) {
		// Clean-up.
		this->clean();

		// Reuse code.
		return Ptr<T>(ptr);
	}

	/*
	 * @brief Overloaded '->' operator. Includes null ptr check/assert.
	 *
	 * @return pointer to templated object
	 */
	T* operator->(void) {
		// Assert.
		if (m_ptr == nullptr) {
			Print_bug("ptr->() with nullptr");
			Terminate_bug(); // Does NOT return
		}

		// Return.
		return m_ptr;
	}

	/*
	 * @brief Overloaded '*' operator. Includes null ptr check/assert.
	 *
	 * @return reference to templated object
	 */
	T& operator*(void) {
		// Assert.
		if (m_ptr == nullptr) {
			Print_bug("ptr*() with nullptr");
			Terminate_bug(); // Does NOT return
		}

		// Return.
		return *m_ptr;
	}

	/*
	 * @brief Getter function to check if stored pointer is null.
	 *
	 * @return true if pointer is nullptr, false otherwise
	 */
	bool isNull(void) {return (m_ptr == nullptr);}

	/*
	 * @brief Getter function to check reference count of pointed to object.
	 *
	 * @return reference count of object being pointed to
	 */
	std::size_t refCnt(void) {return *m_refCnt;}

	/*
	 * @brief Destructor- decrements reference count- deleting object as needed.
	 */
	~Ptr(void) {this->clean();}

private:
	// Raw "shared ptr" being represented.
	T*           m_ptr    = nullptr;
	std::size_t* m_refCnt = nullptr;

	// Helper function to keep internals clean.
	void clean(void) {
		// Destruct.
		if (*m_refCnt > 0) {*m_refCnt -= 1;}
		if (*m_refCnt == 0) {
			if (m_ptr != nullptr) {delete m_ptr;}
			delete m_refCnt;
		}
	}
};

#endif /* COMMON_UTIL_PTR_H_ */
