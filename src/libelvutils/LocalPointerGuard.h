/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * PointerGuard.h
 *  This template class offers a simple guard functionality to be used in local scope.
 *  The intention is to ensure deletion of given pointer automatically when leaving local scope.
 *  The given pointer must not be deleted outside as long as the release method has not been called before.
 */

#ifndef LOCALPOINTERGUARD_H_
#define LOCALPOINTERGUARD_H_

//#include <dllexport.h>

template<typename T> class LocalPointerGuard {
public:

	/** \brief Constructor.
	* \param objPointer Pointer to encapsulate.
	* \param arrayPtr Pass true if passed pointer points to an array. Pass false if pointer points to single value.
	*/
	LocalPointerGuard(T* objPointer, bool arrayPtr) {
		this->objPtr = objPointer;
		this->arrayPtr = arrayPtr;
	}

	/**\brief Destructor.
	 * \details Deletes the pointer.
	 */
	virtual ~LocalPointerGuard() {

		if(objPtr != 0) {
			if(arrayPtr) {
				delete[] objPtr;
			}
			else {
				delete objPtr;
			}

		}
	}

	/**\brief Returns the pointer for direct method/member access.
	 * \return The pointer.
	 * */
	T* operator->() const {
		return objPtr;
	}

	/**\brief Returns the pointer for passing it as argument.
	 * \return The pointer.*/
	T* get() const {
		return objPtr;
	}

	/**\brief Returns the pointer for passing it as argument.
	 * \details Same as .get(), but shorter.
	 * \return The pointer.
	 * */
	T* operator*() const {
		return objPtr;
	}

	/** \brief Releases the pointer.
	 * \details Releases ownership of the pointer. Sets the internal pointer to NULL, such that it won't be deleted on destruction.
	 */
	void release() {
		objPtr = NULL;
	}


private:
	//unsigned int refCnt;
	T* objPtr;
	bool arrayPtr;


	/* Private copy constructor.*/
	LocalPointerGuard(LocalPointerGuard const &);
	LocalPointerGuard<T>& operator=(LocalPointerGuard<T> const &);
};


#endif /* LOCALPOINTERGUARD_H_ */
