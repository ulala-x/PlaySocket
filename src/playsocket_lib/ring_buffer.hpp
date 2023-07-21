#pragma once

#include <iostream>
#include <span>
#include <stdexcept>
#include <vector>

namespace Play
{

class RingBuffer
{
public:
    explicit RingBuffer(size_t capacity, size_t maxCapacity)
        : _buffer(capacity), _readerIndex(0), _headerIndex(0), _size(0),
          _maxCapacity(maxCapacity)
    {
        if (capacity > maxCapacity)
        {
            throw std::invalid_argument(
                "capacity cannot be greater than maxCapacity");
        }
    }

    RingBuffer(size_t capacity) : RingBuffer(capacity, capacity)
    {
    }

    size_t capacity() const
    {
        return _buffer.capacity();
    }

    size_t size() const
    {
        return _size;
    }

    void push(unsigned char item)
    {
        if (_size == capacity())
        {
            resizeBuffer(capacity() * 2);
        }

        _buffer[_headerIndex] = item;
        _headerIndex = nextIndex(_headerIndex);
        _size++;
    }

    void push(const std::vector<unsigned char> &data)
    {
        for (unsigned char b : data)
        {
            push(b);
        }
    }

    void resizeBuffer(size_t newCapacity)
    {
        if (newCapacity > _maxCapacity)
        {
            throw std::out_of_range("Queue has reached maximum capacity");
        }

        std::vector<unsigned char> newBuffer(newCapacity);
        size_t currentIndex = _readerIndex;

        for (size_t i = 0; i < _size; ++i)
        {
            newBuffer[i] = _buffer[currentIndex];
            currentIndex = nextIndex(currentIndex);
        }

        _buffer = std::move(newBuffer);
        _readerIndex = 0;
        _headerIndex = _size;
    }

    unsigned char pop()
    {
        if (_size == 0)
        {
            throw std::runtime_error("Queue is empty");
        }

        unsigned char item = _buffer[_readerIndex];
        _readerIndex = nextIndex(_readerIndex);
        _size--;
        return item;
    }

    unsigned char peek() const
    {
        if (_size == 0)
        {
            throw std::runtime_error("Queue is empty");
        }

        return _buffer[_readerIndex];
    }

    void clear()
    {
        // for (size_t i = 0; i < _buffer.size(); i++)
        // {
        //     _buffer[i] = 0;
        // }
        _readerIndex = 0;
        _headerIndex = 0;
        _size = 0;
    }

    void clear(size_t count)
    {
        if (count > _size)
        {
            throw std::invalid_argument("count exceeds queue size");
        }

        for (size_t i = 0; i < count; i++)
        {
            _readerIndex = nextIndex(_readerIndex);
        }

        _size -= count;
    }

    size_t read(unsigned char *buffer, size_t offset, size_t count)
    {
        size_t bytesRead = 0;

        while (bytesRead < count && _size > 0)
        {
            buffer[offset + bytesRead] = pop();
            bytesRead++;
        }

        return bytesRead;
    }

    void write(const unsigned char *buffer, size_t offset, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            push(buffer[offset + i]);
        }
    }

    void write(const std::span<const unsigned char> &buffer)
    {
        for (size_t i = 0; i < buffer.size(); i++)
        {
            push(buffer[i]);
        }
    }

    int8_t readInt8()
    {
        return pop();
    }

    int16_t readInt16()
    {
        int16_t data = peekInt16();
        clear(sizeof(int16_t));
        return data;
    }

    int32_t readInt32()
    {
        int32_t data = peekInt32();
        clear(sizeof(int32_t));
        return data;
    }

    int16_t peekInt16() const
    {
        return getInt16(_readerIndex);
    }

    int32_t peekInt32() const
    {
        return getInt32(_readerIndex);
    }

    void write(uint16_t value)
    {
        push(static_cast<unsigned char>((value >> 8) & 0xFF));
        push(static_cast<unsigned char>(value & 0xFF));
    }

    void write(uint32_t value)
    {
        push(static_cast<unsigned char>((value >> 24) & 0xFF));
        push(static_cast<unsigned char>((value >> 16) & 0xFF));
        push(static_cast<unsigned char>((value >> 8) & 0xFF));
        push(static_cast<unsigned char>(value & 0xFF));
    }

    void write(int16_t value)
    {
        write(static_cast<uint16_t>(value));
    }

    void write(int32_t value)
    {
        write(static_cast<uint32_t>(value));
    }

    void write(uint8_t value)
    {
        push(value);
    }

    void write(int8_t value)
    {
        push(value);
    }

private:
    std::vector<unsigned char> _buffer;
    size_t _readerIndex;
    size_t _headerIndex;
    size_t _size;
    size_t _maxCapacity;

    size_t nextIndex(size_t index) const
    {
        return (index + 1) % capacity();
    }

    bool isReadIndexValid(size_t index, size_t size) const
    {
        size_t tempIndex = index;
        for (size_t i = 0; i < size; i++)
        {
            if (tempIndex == _headerIndex || _size == 0)
            {
                return false;
            }
            tempIndex = nextIndex(tempIndex);
        }
        return true;
    }

    int16_t getInt16(size_t index) const
    {
        if (!isReadIndexValid(index, sizeof(int16_t)))
        {
            throw std::out_of_range("Index out of range");
        }

        return static_cast<int16_t>((_buffer[index] << 8) |
                                    _buffer[nextIndex(index)]);
    }

    int32_t getInt32(size_t index) const
    {
        if (!isReadIndexValid(index, sizeof(int32_t)))
        {
            throw std::out_of_range("Index out of range");
        }

        return (_buffer[index] << 24) | (_buffer[nextIndex(index)] << 16) |
               (_buffer[nextIndex(nextIndex(index))] << 8) |
               _buffer[nextIndex(nextIndex(nextIndex(index)))];
    }


    bool isWriteIndexValid(size_t size) const
    {
        size_t distance = (_headerIndex >= _readerIndex)
                              ? (_headerIndex - _readerIndex)
                              : (capacity() - _readerIndex + _headerIndex);
        return (distance + size) <= capacity();
    }
};
} // namespace Play
