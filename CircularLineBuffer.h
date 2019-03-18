#ifndef CPP_CHAT_CLIENT_CIRCULARBUFFER_H
#define CPP_CHAT_CLIENT_CIRCULARBUFFER_H

#include <cstdlib>
#include <string>
#include <mutex>

/**
 * Assignment 3
 *
 * See the lab manual for more details.
 */
class CircularLineBuffer {
private:
    /**
     * You may increase the size of the buffer, if you want. :)
     * Reducing the size of the buffer allows for easier debugging.
     */
    static const int bufferSize = 4096; //4096
    char buffer[bufferSize] = {0};

    /**
     * You may ignore this variable.
     */
    std::mutex mtx;
    /**
     * Use 'start' to keep track of the start of the buffer.
     * Use 'count' to keep track of the current number of characters in the buffer.
     */
    int start = 0, count = 0;

    /**
     * This method writes the given number of characters into the buffer,
     * starting at the next free location.
     * If there is not enough space left in the buffer, it writes nothing into the buffer and returns false.
     *
     * @param chars Pointer to the characters to write into the buffer.
     * @param nchars The number of characters to write.
     * @return False if there was not enough space in the buffer. True otherwise.
     */
    bool _writeChars(const char *chars, size_t nchars);

    /**
     * This method reads a line from the buffer,
     * starting from location 'start'.
     * If there is no complete line (no '\n') in the buffer, this method returns an empty string.
     *
     * @return The next string from the buffer. Returns an empty string if the buffer is empty.
     */
    std::string _readLine();

public:
    /**
     * @return The amount of free space in the buffer in number of characters.
     */
    int freeSpace();

    /**
     *
     * @return true if and only if (iff) the buffer is full.
     */
    bool isFull();

    /**
     *
     * @return true if and only if (iff) the buffer is empty.
     */
    bool isEmpty();

    /**
     * This method should return the next free spot in the buffer as seen from the current value of 'start'.
     *
     * For example, consider the following buffer:
     *  S
     * [H,E,L,L,O,\n,-,-]
     * Here 'S' points to the start of the buffer contents, and '-' indicates an empty space in the buffer.
     * For this buffer, nextFreeIndex should return 6,
     * because it is the first free position in the buffer.
     *
     * If the buffer is full, the behavior is undefined.
     *
     * @return The index of the first free position in the buffer.
     */
    int nextFreeIndex();

    /**
     * The position of the next newline character (\n), as seen from the current value of 'start'.
     *
     * For example, consider the following buffer:
     *     S
     * [\n,H,I,\n,-,-]
     * Here 'S' points to the start of the buffer contents, and '-' indicates an empty space in the buffer.
     * For this buffer, findNewline should return 3, because,
     * when starting from S, index 3 contains the first '\n' character.
     *
     * @return The position of the next newline character (\n), as seen from the current value of 'start'.
     */
    int findNewline();

    /**
     * Checks if there is a complete line in the buffer.
     * You can make your life easier by implementing this method using the method above.
     *
     * @return true if and only if (iff) there is at least one complete line in the buffer.
     */
    bool hasLine();

    /**
     * This method writes the given number of characters into the buffer.
     * It forwards its input to _writeChars, which you will implement.
     *
     * You don't have to read, understand, or edit the calls to 'mtx'. It prevents concurrent modification errors,
     * which you will discuss in another course.
     *
     * @param chars Pointer to the characters to write into the buffer.
     * @param nchars The number of characters to write.
     * @return False if there was not enough space in the buffer. True otherwise.
     */
    inline bool writeChars(const char *chars, size_t nchars) {
        mtx.lock();
        auto res = _writeChars(chars, nchars);
        mtx.unlock();
        return res;
    }

    /**
     * This method reads a line from the buffer.
     * It forwards its input to _readLine, which you will implement.
     *
     * You don't have to read, understand, or edit the calls to 'mtx'. It prevents concurrent modification errors,
     * which you will discuss in another course.
     *
     * @return The next string from the buffer. Returns an empty string if the buffer is empty.
     */
    inline std::string readLine() {
        mtx.lock();
        auto res = _readLine();
        mtx.unlock();
        return res;
    }
};


#endif //CPP_CHAT_CLIENT_CIRCULARBUFFER_H