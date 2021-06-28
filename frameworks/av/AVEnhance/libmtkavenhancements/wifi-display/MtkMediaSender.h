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

#ifndef MTKMEDIA_SENDER_H_

#define MTKMEDIA_SENDER_H_

#include "rtp/MtkRTPSender.h"

#include <media/stagefright/foundation/ABase.h>
#include <media/stagefright/foundation/AHandler.h>
#include <utils/Errors.h>
#include <utils/Vector.h>

namespace android
{

struct ABuffer;
struct MtkANetworkSession;
struct AMessage;
//struct IHDCP; remove HDCP
struct MtkTSPacketizer;

// This class facilitates sending of data from one or more media tracks
// through one or more RTP channels, either providing a 1:1 mapping from
// track to RTP channel or muxing all tracks into a single RTP channel and
// using transport stream encapsulation.
// Optionally the (video) data is encrypted using the provided hdcp object.
struct MtkMediaSender : public AHandler {
    enum {
        kWhatInitDone,
        kWhatError,
        kWhatNetworkStall,
        kWhatInformSender,
    };

    MtkMediaSender(
        const sp<MtkANetworkSession> &netSession,
        const sp<AMessage> &notify);

    //status_t setHDCP(const sp<IHDCP> &hdcp); remove HDCP

    enum FlagBits {
        FLAG_MANUALLY_PREPEND_SPS_PPS = 1,
    };
    ssize_t addTrack(const sp<AMessage> &format, uint32_t flags);

    // If trackIndex == -1, initialize for transport stream muxing.
    status_t initAsync(
        ssize_t trackIndex,
        const char *remoteHost,
        int32_t remoteRTPPort,
        MtkRTPSender::TransportMode rtpMode,
        int32_t remoteRTCPPort,
        MtkRTPSender::TransportMode rtcpMode,
        int32_t *localRTPPort);

    status_t queueAccessUnit(
        size_t trackIndex, const sp<ABuffer> &accessUnit);

protected:
    virtual void onMessageReceived(const sp<AMessage> &msg);
    virtual ~MtkMediaSender();

private:
    enum {
        kWhatSenderNotify,
    };

    enum Mode {
        MODE_UNDEFINED,
        MODE_TRANSPORT_STREAM,
        MODE_ELEMENTARY_STREAMS,
    };

    struct TrackInfo {
        sp<AMessage> mFormat;
        uint32_t mFlags;
        sp<MtkRTPSender> mSender;
        List<sp<ABuffer> > mAccessUnits;
        ssize_t mPacketizerTrackIndex;
        bool mIsAudio;
        bool mFirstAccessUnitIsValid;
    };

    sp<MtkANetworkSession> mNetSession;
    sp<AMessage> mNotify;

    //sp<IHDCP> mHDCP; remove HDCP

    Mode mMode;
    int32_t mGeneration;

    Vector<TrackInfo> mTrackInfos;

    sp<MtkTSPacketizer> mTSPacketizer;
    sp<MtkRTPSender> mTSSender;
    int64_t mPrevTimeUs;

    size_t mInitDoneCount;

    FILE *mLogFile;

    void onSenderNotify(const sp<AMessage> &msg);

    void notifyInitDone(status_t err);
    void notifyError(status_t err);
    void notifyNetworkStall(size_t numBytesQueued);

    status_t packetizeAccessUnit(
        size_t trackIndex,
        sp<ABuffer> accessUnit,
        sp<ABuffer> *tsPackets);

    DISALLOW_EVIL_CONSTRUCTORS(MtkMediaSender);

private:
    bool recordAudioOnly;
    bool mAllTracksHaveAccessUnit;
    bool allTracksHaveAccessUnit();
    bool avsync_enable;
    bool mWFDFrameLog;
    int64_t mFirstVideoTimeUs;

};

}  // namespace android

#endif  // MTK_MEDIA_SENDER_H_
