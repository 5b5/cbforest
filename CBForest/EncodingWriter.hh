//
//  EncodingWriter.h
//  CBForest
//
//  Created by Jens Alfke on 1/26/15.
//  Copyright (c) 2015 Couchbase. All rights reserved.
//

#ifndef __CBForest__DataWriter__
#define __CBForest__DataWriter__

#include "Encoding.hh"
#include <ctime>
#include <iostream>
#include <unordered_map>

namespace forestdb {


    class dataWriter {
    public:
        dataWriter(std::ostream&,
                   const std::unordered_map<std::string, uint32_t> *externStrings = NULL);

        void writeNull();
        void writeBool (bool);

        void writeInt(int64_t);
        void writeUInt(uint64_t);
        void writeFloat(float);
        void writeDouble(double);
        void writeRawNumber(slice);
        void writeRawNumber(std::string str)        {writeRawNumber(slice(str));}

        void writeDate(std::time_t);

        void writeString(std::string);
        void writeString(slice);

        void writeData(slice);

        void beginArray(uint64_t count);
        void endArray()                             {popState();}

        void beginDict(uint64_t count);
        void writeKey(std::string);
        void writeKey(slice);
        void endDict();

        // Note: overriding <<(bool) would be dangerous due to implicit conversion
        dataWriter& operator<< (int64_t i)          {writeInt(i); return *this;}
        dataWriter& operator<< (double d)           {writeDouble(d); return *this;}
        dataWriter& operator<< (float f)            {writeFloat(f); return *this;}
        dataWriter& operator<< (std::string str)    {writeString(str); return *this;}
        dataWriter& operator<< (slice s)            {writeString(s); return *this;}

#ifdef __OBJC__
        void write(id);
#endif

    private:
        void _addTypeCode(value::typeCode code)     {_out.write((char*)&code, 1);}
        void addTypeCode(value::typeCode code)      {_addTypeCode(code); ++_state->i;}
        void addUVarint(uint64_t);

        struct state {
            uint64_t count;
            uint64_t i;
            uint64_t indexPos;
            uint16_t* hashes;
        };

        void pushState();
        void popState();
        void pushCount(uint64_t count);
        void writeHashes();

        std::ostream& _out;
        state* _state;
        std::vector<state> _states;
        std::unordered_map<std::string, uint32_t> _sharedStrings;
        const std::unordered_map<std::string, uint32_t> *_externStrings;
    };


}

#endif /* defined(__CBForest__DataWriter__) */
