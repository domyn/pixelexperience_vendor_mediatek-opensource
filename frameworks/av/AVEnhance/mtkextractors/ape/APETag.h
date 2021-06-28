/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef APETAG_H_

#define APETAG_H_

#include <utils/RefBase.h>

namespace android
{

class DataSourceBase;
class String8;
class DataSourceHelper;

#define APE_VAL_MAX_LEN         244
#define APE_KEY_MAX_LEN         32
#define APE_KEY_MAX_NUM         32
#define APE_READ_HEADER_SIZE    (8+APE_KEY_MAX_LEN)
#define APE_READ_BUFFER_SIZE    META_TAG_FRAME_MAX_SIZE

#define META_TAG_ATTR_ARTIST        0x00000001
#define META_TAG_ATTR_AUTHOR        0x00000002
#define META_TAG_ATTR_ALBUM         0x00000004
#define META_TAG_ATTR_TITLE         0x00000008

#define META_TAG_ATTR_COPYRIGHT     0x00000010
#define META_TAG_ATTR_GENRE         0x00000020
#define META_TAG_ATTR_YEAR          0x00000040
#define META_TAG_ATTR_TRACKNUM      0x00000080

#define META_TAG_ATTR_DESCRIPTION   0x00000100
#define META_TAG_ATTR_SYNCLYRICS    0x00000200
#define META_TAG_ATTR_PICTURE       0x00000400
#define META_TAG_ATTR_DURATION      0x00000800

#define META_TAG_ATTR_BITRATE       0x00001000
#define META_TAG_ATTR_RATING        0x00002000


#define META_TAG_TEXT_ENCODING_UTF16       0xf1
#define META_TAG_TEXT_ENCODING_UTF16_BE    0xf2
#define META_TAG_TEXT_ENCODING_ISO_8859_1  0xf3


#define META_TAG_FRAME_MAX_SIZE 81*2
#define APE_TAG_FOOTER_SIZE     32
#define APE_TAG_MAGIC_SIZE      8
#define APE_TAG_MAGIC_ID        "APETAGEX"



struct APETAG
{
    APETAG(DataSourceHelper *source);
    APETAG(DataSourceBase *source);
    ~APETAG();

    bool isValid() const;
    ///    Version version() const;


    struct Iterator
    {
        Iterator(const APETAG &parent, const char *id, uint16_t len);
        ~Iterator();

        bool done() const;
        /// void getID(String8 *id) const;
        void getString(String8 *s) const;
        ///const uint8_t *getData(size_t *length) const;
        /// void next();

    private:
        const APETAG &mParent;
        char *mID;
        uint16_t mIDLen;
        size_t mOffset;

        const uint8_t *mFrameData;
        size_t mFrameSize;

        void findFrame();

        size_t getHeaderLength() const;

        Iterator(const Iterator &);
        Iterator &operator=(const Iterator &);
    };

private:
    bool mIsValid;
    uint8_t *mData;
    size_t mSize;
    size_t mFirstFrameOffset;
    ///Version mVersion;

    bool parsetag(DataSourceBase *source);
    ///void removeUnsynchronization();
    ///bool removeUnsynchronizationV2_4();

    ///static bool ParseSyncsafeInteger(const uint8_t encoded[4], size_t *x);

    APETAG(const APETAG &);
    APETAG &operator=(const APETAG &);
};

}  // namespace android

#endif  // APETAG_H_
