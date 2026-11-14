/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <vector>
#include <string>
#include <stdint.h>
/// <summary>
/// This class represents a vector of bytes and provides some convenience functions to work with that data
/// </summary>

class BinaryData
{
public:
	typedef std::vector<uint8_t> Container;

	/// <summary>
	/// Default constructor.
	/// </summary>

	BinaryData(void);

	/// <summary>
	/// Constructor initializing the object from a memory pointer.
	/// </summary>
	///
	/// <param name="data">
	/// The data pointer.
	/// </param>
	/// <param name="size">
	/// The data size.
	/// </param>

	BinaryData(const uint8_t* data, size_t size);

	/// <summary>
	/// Constructor initializing the object from a C string
	/// </summary>
	///
	/// <param name="data">
	/// The zero terminated string.
	/// </param>

	BinaryData(const char* s);

	/// <summary>
	/// Constructor initializing the object from a byte vector
	/// </summary>
	///
	/// <param name="data">
	/// The data.
	/// </param>

	BinaryData(const std::vector<uint8_t>& data);

	/// <summary>
	/// Constructor initializing the object from an 8bit value.
	/// </summary>
	///
	/// <param name="v">
	/// The value.
	/// </param>

	BinaryData( uint8_t v);

	/// <summary>
	/// Constructor initializing the object from a 16bit value.
	/// </summary>
	///
	/// <param name="v">
	/// The value.
	/// </param>

	BinaryData( uint16_t v);

	/// <summary>
	/// Constructor initializing the object from a 32bit value.
	/// </summary>
	///
	/// <param name="v">
	/// The value.
	/// </param>

	BinaryData( uint32_t v);

	/// <summary>
	/// Destructor
	/// </summary>

	virtual ~BinaryData(void);

	/// <summary>
	/// Gets an 8 bit value. If reading beyond the size of this object, 0 is returned.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	///
	/// <returns>
	/// The 8 bit value.
	/// </returns>

	uint8_t GetUInt8Value( size_t index )const;

	/// <summary>
	/// Gets a 16 bit value. Bytes beyond the size of this object are assumed as 0. Most significant byte is first.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	///
	/// <returns>
	/// The 16 bit value.
	/// </returns>

	uint16_t GetUInt16Value( size_t index )const;

	/// <summary>
	/// Gets a 24 bit value. Bytes beyond the size of this object are assumed as 0. Most significant byte is first.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	///
	/// <returns>
	/// The 24 bit value.
	/// </returns>

	uint32_t GetUInt24Value( size_t index )const;

	/// <summary>
	/// Gets a 32 bit value. Bytes beyond the size of this object are assumed as 0. Most significant byte is first.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	///
	/// <returns>
	/// The 32 bit value.
	/// </returns>

	uint32_t GetUInt32Value( size_t index )const;

	/// <summary>
	/// Gets a string value from index up to the first 0 character
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	///
	/// <returns>
	/// The string value.
	/// </returns>

	std::string GetStringValue( size_t index )const;

	/// <summary>
	/// Gets a range of bytes from this object and returns it as new BinaryData object.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based start index.
	/// </param>
	/// <param name="size">
	/// The size.
	/// </param>
	///
	/// <returns>
	/// The range of bytes.
	/// </returns>

	BinaryData GetRange( size_t index, size_t size )const;

	/// <summary>
	/// Copies binary data into a user-supplied buffer.
	/// </summary>
	///
	/// <param name="buffer">
	/// [in,out] If non-null, the buffer.
	/// </param>
	/// <param name="index">
	/// Zero-based start index.
	/// </param>
	/// <param name="size">
	/// The data size.
	/// </param>

	void GetBinaryData( uint8_t* buffer, size_t index, size_t size )const;

	/// <summary>
	/// Sets an 8 bit value. If necessary, the size is increased accordingly.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="value">
	/// The 8 bit value.
	/// </param>

	void SetUInt8Value( size_t index, uint8_t value );

	/// <summary>
	/// Sets a 16 bit value. If necessary, the size is increased accordingly. Most significant byte is stored first.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="value">
	/// The 16 bit value.
	/// </param>

	void SetUInt16Value( size_t index, uint16_t value );

	/// <summary>
	/// Sets a 24 bit value. If necessary, the size is increased accordingly. Most significant byte is stored first.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="value">
	/// The 24 bit value.
	/// </param>

	void SetUInt24Value( size_t index, uint32_t value );

	/// <summary>
	/// Sets a 32 bit value. If necessary, the size is increased accordingly. Most significant byte is stored first.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="value">
	/// The 32 bit value.
	/// </param>

	void SetUInt32Value( size_t index, uint32_t value );

	/// <summary>
	/// Sets a range of bytes in this object. If necessary, the size is increased accordingly.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="range">
	/// The BinaryData object containing the bytes to set.
	/// </param>

	void SetRange( size_t index, const BinaryData& range );

	/// <summary>
	/// Sets a range of bytes in this object to a string. If necessary, the size is increased accordingly.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="s">
	/// The string containing the bytes to set.
	/// </param>

	void SetStringValue( size_t index, const std::string& s );

	/// <summary>
	/// Copies binary data to a variable.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="obj">
	/// The object to copy the binary data into.
	/// </param>
	///
	/// <returns>
	/// true if it succeeds, false if it fails.
	/// </returns>

	template <class T>
	bool CopyToVariable( size_t index, T& obj )const
	{
		if( size() < (index + sizeof(T)))return false;
		for( size_t i=0; i<sizeof(T); i++ )
		{
			((uint8_t*)&obj)[i] = at(i+index);
		}
		return true;
	}

	/// <summary>
	/// Copies binary data from a variable.
	/// </summary>
	///
	/// <param name="index">
	/// Zero-based index.
	/// </param>
	/// <param name="obj">
	/// The object to copy into the binary data.
	/// </param>
	///
	/// <returns>
	/// true if it succeeds, false if it fails.
	/// </returns>

	template <class T>
	bool CopyFromVariable( size_t index, const T& obj )
	{
		if( size() < (index + sizeof(T)))resize(index + sizeof(T));
		for( size_t i=0; i<sizeof(T); i++ )
		{
			at(i+index) = ((uint8_t*)&obj)[i];
		}
		return true;
	}

	/// <summary>
	/// Convert this object into a string representation for debugging or logging.
	/// </summary>
	///
	/// <returns>
	/// A string representation of this object.
	/// </returns>

	std::string ToString()const;

	/// <summary>
	/// Fills a range of bytes from this object with a constant value.
	/// </summary>
	///
	/// <param name="start">
	/// The start index.
	/// </param>
	/// <param name="length">
	/// The length of the range to be filled.
	/// </param>
	/// <param name="c">
	/// The constant value to fill the range with.
	/// </param>

	void Fill( size_t start, size_t length, uint8_t c );


	////////////////////////////////////////////////////////////////////////
	//                        Bridge to the vector                        //
	////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Returns the number of elements.
	/// </summary>
	///
	/// <returns>
	/// The number of elements in the container.
	/// </returns>
	inline size_t size() const
	{
		return _data.size();
	}

	/// <summary>
	/// Returns whether the container is empty.
	/// </summary>
	///
	/// <returns>
	/// true if the container size is 0, false otherwise.
	/// </returns>
	inline bool empty() const
	{
		return _data.empty();
	}

	/// <summary>
	/// Returns the size of allocated storage capacity.
	/// </summary>
	///
	/// <returns>
	/// The number of allocated storage capacity.
	/// </returns>
	inline size_t capacity() const
	{
		return _data.capacity();
	}

	/// <summary>
	/// Resizes the container.
	/// </summary>
	///
	/// <param name="newsize">
	/// The new number of elements in the container.
	/// </param>
	inline void resize(size_t newsize)
	{
		_data.resize(newsize);
	}

	/// <summary>
	/// Returns the element at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The index of the element to get.
	/// </param>
	/// <returns>
	/// The const element at the specified position.
	/// </returns>
	inline const uint8_t& at(size_t position) const
	{
		return _data.at(position);
	}

	/// <summary>
	/// Returns the element at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The index of the element to get.
	/// </param>
	/// <returns>
	/// The element at the specified position.
	/// </returns>
	inline uint8_t& at(size_t position)
	{
		return _data.at(position);
	}

	/// <summary>
	/// Returns the element at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The index of element to get.
	/// </param>
	/// <returns>
	/// The const element at the specified position.
	/// </returns>
	inline const uint8_t& operator[](size_t position) const
	{
		return _data[position];
	}

	/// <summary>
	/// Returns the element at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The index of element to get.
	/// </param>
	/// <returns>
	/// The element at the specified position.
	/// </returns>
	inline uint8_t& operator[](size_t position)
	{
		return _data[position];
	}

	/// <summary>
	/// Equality operator.
	/// </summary>
	///
	/// <param name="rhs">
	/// BinaryData to compare to.
	/// </param>
	/// <returns>
	/// true if objects are equal, false otherwise.
	/// </returns>
	inline bool operator==(const BinaryData& rhs) const
	{
		return (_data == rhs._data);
	}

	/// <summary>
	/// Inequality operator.
	/// </summary>
	///
	/// <param name="rhs">
	/// BinaryData to compare to.
	/// </param>
	/// <returns>
	/// true if objects are not equal, false otherwise.
	/// </returns>
	inline bool operator!=(const BinaryData& rhs) const
	{
		return (_data != rhs._data);
	}

	/// <summary>
	/// Assignment operator.
	/// </summary>
	///
	/// <param name="rhs">
	/// Another object to assign from.
	/// </param>
	/// <returns>
	/// A reference to itself.
	/// </returns>
	inline BinaryData& operator=(const BinaryData& rhs)
	{
		_data = rhs._data;
		return *this;
	}

	/// <summary>
	/// Clears the container.
	/// </summary>
	inline void clear()
	{
		_data.clear();
	}

	/// <summary>
	/// Returns the pointer to the internal array data.
	/// </summary>
	///
	/// <returns>
	/// Internal array data.
	/// </returns>
	inline const uint8_t* data() const
	{
		return _data.data();
	}

	/// <summary>
	/// Returns the pointer to the internal array data.
	/// </summary>
	///
	/// <returns>
	/// Internal array data.
	/// </returns>
	inline uint8_t* data()
	{
		return _data.data();
	}

	/// <summary>
	/// Appends a new element to the container.
	/// </summary>
	///
	/// <param name="value">
	/// The element to append.
	/// </param>
	inline void push_back(const uint8_t& value)
	{
		_data.push_back(value);
	}

	/// <summary>
	/// Removes the last element from the container.
	/// </summary>
	inline void pop_back()
	{
		_data.pop_back();
	}

	/// <summary>
	/// Inserts elements at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The position where to insert.
	/// </param>
	/// <param name="first">
	/// The first element to be inserted.
	/// </param>
	/// <param name="last">
	/// The last element where to stop the insertion.
	/// </param>
	template<typename Iter>
	inline void insert(Container::iterator position, Iter first, Iter last)
	{
		_data.insert(position, first, last);
	}

	/// <summary>
	/// Inserts elements at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The position where to insert.
	/// </param>
	/// <param name="count">
	/// The number of elements to insert.
	/// </param>
	/// <param name="value">
	/// The value to fill the new elements with.
	/// </param>
	inline void insert(Container::iterator position, size_t count, const uint8_t& value)
	{
		_data.insert(position, count, value);
	}

	/// <summary>
	/// Inserts a single element at the specified position.
	/// </summary>
	///
	/// <param name="position">
	/// The position where to insert.
	/// </param>
	/// <param name="value">
	/// The value to fill the new element with.
	/// </param>
	inline Container::iterator insert(Container::iterator position, const uint8_t& value)
	{
		return _data.insert(position, value);
	}

	/// <summary>
	/// Erases elements in the container.
	/// </summary>
	///
	/// <param name="first">
	/// The position where to begin.
	/// </param>
	/// <param name="last">
	/// The position where to stop erasing.
	/// </param>
	/// <returns>
	/// An iterator pointing to the new location after the last erased element.
	/// </returns>
	inline Container::iterator erase(Container::iterator first, Container::iterator last)
	{
		return _data.erase(first, last);
	}

	/// <summary>
	/// Erases a single element in the container.
	/// </summary>
	///
	/// <param name="position">
	/// The position of the element to erase.
	/// </param>
	/// <returns>
	/// An iterator pointing to the new location after the erased element.
	/// </returns>
	inline Container::iterator erase(Container::iterator position)
	{
		return _data.erase(position);
	}

	/// <summary>
	/// Assigns a fixed value to all the elements in the container.
	/// </summary>
	///
	/// <param name="value">
	/// The value to assign to all elements.
	/// </param>
	inline void assign(size_t count, const uint8_t& value)
	{
		_data.assign(count, value);
	}

	/// <summary>
	/// Assigns the container from a range of iterable objects.
	/// </summary>
	///
	/// <param name="first">
	/// The position of first object.
	/// </param>
	/// <param name="last">
	/// The position of last (excluded) object.
	/// </param>
	template<typename Iter>
	inline void assign(Iter first, Iter last)
	{
		_data.assign(first, last);
	}

	/// <summary>
	/// Returns an iterator pointing to the first element.
	/// </summary>
	///
	/// <returns>
	/// A const iterator pointing to the first element.
	/// </returns>
	inline Container::const_iterator begin() const
	{
		return _data.begin();
	}

	/// <summary>
	/// Returns an iterator pointing to the first element.
	/// </summary>
	///
	/// <returns>
	/// An iterator pointing to the first element.
	/// </returns>
	inline Container::iterator begin()
	{
		return _data.begin();
	}

	/// <summary>
	/// Returns an iterator pointing to the following element after the last one.
	/// </summary>
	///
	/// <returns>
	/// A const iterator pointing to the following element after the last one.
	/// </returns>
	inline Container::const_iterator end() const
	{
		return _data.end();
	}

	/// <summary>
	/// Returns an iterator pointing to the following element after the last one.
	/// </summary>
	///
	/// <returns>
	/// An iterator pointing to the following element after the last one.
	/// </returns>
	inline Container::iterator end()
	{
		return _data.end();
	}

	/// <summary>
	/// Returns a reverse iterator pointing to the last element.
	/// </summary>
	///
	/// <returns>
	/// A const reverse iterator pointing to the last element.
	/// </returns>
	inline Container::const_reverse_iterator rbegin() const
	{
		return _data.rbegin();
	}

	/// <summary>
	/// Returns a reverse iterator pointing to the last element.
	/// </summary>
	///
	/// <returns>
	/// A reverse iterator pointing to the last element.
	/// </returns>
	inline Container::reverse_iterator rbegin()
	{
		return _data.rbegin();
	}

	/// <summary>
	/// Returns a reverse iterator pointing to the element before the first one.
	/// </summary>
	///
	/// <returns>
	/// A const reverse iterator pointing to the element before the first one.
	/// </returns>
	inline Container::const_reverse_iterator rend() const
	{
		return _data.rend();
	}

	/// <summary>
	/// Returns a reverse iterator pointing to the element before the first one.
	/// </summary>
	///
	/// <returns>
	/// A reverse iterator pointing to the element before the first one.
	/// </returns>
	inline Container::reverse_iterator rend()
	{
		return _data.rend();
	}

	/// <summary>
	/// Returns the first element in the container.
	/// </summary>
	///
	/// <returns>
	/// A const reference to the first element in the container.
	/// </returns>
	inline const uint8_t& front() const
	{
		return _data.front();
	}

	/// <summary>
	/// Returns the first element in the container.
	/// </summary>
	///
	/// <returns>
	/// A reference to the first element in the container.
	/// </returns>
	inline uint8_t& front()
	{
		return _data.front();
	}

	/// <summary>
	/// Returns the last element in the container.
	/// </summary>
	///
	/// <returns>
	/// A const reference to the last element in the container.
	/// </returns>
	inline const uint8_t& back() const
	{
		return _data.back();
	}

	/// <summary>
	/// Returns the last element in the container.
	/// </summary>
	///
	/// <returns>
	/// A reference to the last element in the container.
	/// </returns>
	inline uint8_t& back()
	{
		return _data.back();
	}

private:
	Container _data;
};
