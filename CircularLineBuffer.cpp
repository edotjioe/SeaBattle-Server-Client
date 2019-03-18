//
// Created by Edo on 2/11/2019.
//

#include <iostream>
#include <string.h>
#include "CircularLineBuffer.h"

int CircularLineBuffer::freeSpace() {
    return bufferSize - count;
}

bool CircularLineBuffer::isFull() {
    return bufferSize == count;
}

bool CircularLineBuffer::isEmpty() {
    return count == 0;
}

int CircularLineBuffer::nextFreeIndex() {
    return (start + count) % bufferSize;
}

int CircularLineBuffer::findNewline() {
    for (int i = 1; i < bufferSize; ++i) {
        if(buffer[(start + i) % bufferSize] == '\n')
            return (start + i) % bufferSize;
    }
    return -1;
}

bool CircularLineBuffer::hasLine() {
    return findNewline() != -1 && !isEmpty();
}

bool CircularLineBuffer::_writeChars(const char *chars, size_t nchars) {
    if (isFull() || nchars > freeSpace())
        return false;

    for (int i = 0; i < nchars; ++i)
        buffer[(nextFreeIndex() + i) % bufferSize] = chars[i];

    count = count + nchars;

    return true;
}

std::string CircularLineBuffer::_readLine() {
    if (!isEmpty() && hasLine()){
        int len;

        start > findNewline() ?
                len = findNewline() + (bufferSize - start) + 1 : len = findNewline() - start + 1;

        char strl[len];
        memset(&strl, 0x00, sizeof(strl));

        for (int i = 0; i < len; ++i)
            strl[i] = buffer[(i + start) % bufferSize];

        strl[len] = '\0';

        start = (start + len) % bufferSize;
        count = count - len;

        std::string str(strl);

        return str;
    }
    return "";
}