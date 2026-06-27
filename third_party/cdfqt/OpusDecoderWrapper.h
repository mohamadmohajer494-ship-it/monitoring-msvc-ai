#ifndef OPUSDECODERWRAPPER_H
#define OPUSDECODERWRAPPER_H

#include <QByteArray>
#include <opus/opus.h>
#include <speex/speex_resampler.h>

#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>

class OpusDecoderWrapper
{
public:
    OpusDecoderWrapper()
    {
        int err = 0;

        // ---- Opus decoder @ 16k (VOICE) ----
        decoder = opus_decoder_create(OPUS_RATE, CHANNELS, &err);
        if (!decoder || err != OPUS_OK)
            throw std::runtime_error("Opus decoder create failed");

        // ---- Resampler: 16k -> 32k ----
        resampler = speex_resampler_init(
            CHANNELS,
            OPUS_RATE,   // 16k in
            OUT_RATE,    // 32k out
            7,           // high quality for voice
            &err
            );
        if (!resampler || err != RESAMPLER_ERR_SUCCESS)
            throw std::runtime_error("Speex resampler init failed");

        speex_resampler_skip_zeros(resampler);

        opusFrame.resize(OPUS_FRAME_SIZE);
    }

    ~OpusDecoderWrapper()
    {
        if (decoder)   opus_decoder_destroy(decoder);
        if (resampler) speex_resampler_destroy(resampler);
    }

    // Input: [uint16 len][opus packet] stream
    // Output: PCM 16-bit mono @ 32kHz
    QByteArray decode(const QByteArray& encodedStream)
    {
        rxBuffer.append(encodedStream);
        QByteArray pcmOut;

        const char* data = rxBuffer.constData();
        int total = rxBuffer.size();
        int offset = 0;

        while (true)
        {
            if (total - offset < (int)sizeof(uint16_t))
                break;

            uint16_t packetLen = 0;
            std::memcpy(&packetLen, data + offset, sizeof(uint16_t));

            if (total - offset < (int)sizeof(uint16_t) + packetLen)
                break;

            offset += sizeof(uint16_t);

            const unsigned char* packetPtr =
                reinterpret_cast<const unsigned char*>(data + offset);

            offset += packetLen;

            // ---- Decode Opus @ 16k ----
            int decodedSamples = opus_decode_float(
                decoder,
                packetPtr,
                packetLen,
                opusFrame.data(),
                OPUS_FRAME_SIZE,
                0
                );

            if (decodedSamples <= 0)
                continue;

            // ---- Resample 16k -> 32k ----
            spx_uint32_t in_len  = decodedSamples;
            spx_uint32_t out_len = decodedSamples * 2;

            resampled32k.resize(out_len);

            int rerr = speex_resampler_process_float(
                resampler,
                0,
                opusFrame.data(),
                &in_len,
                resampled32k.data(),
                &out_len
                );

            if (rerr != RESAMPLER_ERR_SUCCESS)
                continue;

            // ---- float -> int16 ----
            QByteArray chunk;
            chunk.resize(out_len * sizeof(int16_t));

            int16_t* pcm16 = reinterpret_cast<int16_t*>(chunk.data());
            for (uint32_t i = 0; i < out_len; ++i)
            {
                float s = resampled32k[i];
                if (s > 1.0f)  s = 1.0f;
                if (s < -1.0f) s = -1.0f;
                pcm16[i] = static_cast<int16_t>(s * 32767.0f);
            }

            pcmOut.append(chunk);
        }

        if (offset > 0)
            rxBuffer.remove(0, offset);

        return pcmOut;
    }

private:
    static constexpr int OPUS_RATE       = 16000;
    static constexpr int OUT_RATE        = 32000;
    static constexpr int CHANNELS        = 1;
    static constexpr int OPUS_FRAME_SIZE = 320; // 20ms @ 16k

    OpusDecoder* decoder = nullptr;
    SpeexResamplerState* resampler = nullptr;

    QByteArray rxBuffer;

    std::vector<float> opusFrame;       // 16k decoded
    std::vector<float> resampled32k;    // 32k output
};

#endif // OPUSDECODERWRAPPER_H
