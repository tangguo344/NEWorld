/*
UILib - A Open-Source UI-Library

Copyright(C) 2016 Infinideastudio-UITeam

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and / or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/

#include <uiaudio.h>
#include <uilogger.h>
#include <limits.h>
#include <float.h>
#include "uidataint.h"
#include <map>
#include <random>

#if defined( __WIN32__ ) || defined( _WIN32 )
//#include <dsound.h>
#endif

namespace UI
{
    namespace Audio
    {
        std::map < std::string, std::function<bool(const std::string&, maxiSample&) >> sourceLoaders;
        Listener thisListener;
        std::vector <std::shared_ptr<OutEffect>> outEffects;
        std::vector <std::shared_ptr<OutEffectEx>> outEffectExs;
        double speedSampler(const Math::Vec3& pos);
        double(*speedOfSoundSampler)(const Math::Vec3& pos) = speedSampler;
        unsigned char SampleNum = 2;
        std::vector<Source> sources;
        RtAudio device(RtAudio::WINDOWS_DS);

        // PlayData
        bool srcreleased = true;
        size_t size;
        double* vals = nullptr;
        //End PlayData

        std::mt19937 rand;

        double speedSampler(const Math::Vec3& pos)
        {
            return 343.3;
        }

        inline double mix(double* iter, double* end)
        {
            double s = 0.0;

            for(; iter < end; ++iter) s += *iter;

            return s;
        };

        void play(double* data)
        {
            for (auto &s : sources)
                if (!s.isStopped()) s.Sample();

            for(size_t i = 0; i < maxiSettings::channels; ++i)
            {
                for(size_t j = 0; j < size; j++)
                    vals[j] = sources[j](i);

                data[i] = mix(vals, vals + size);

                for(auto&x : outEffects)
                    data[i] = (*x)(data[i], i);
            }

            for(auto&x : outEffectExs)
                (*x)(data, maxiSettings::channels);
        }

#ifdef MAXIMILIAN_PORTAUDIO
        int routing(const void *inputBuffer, void *outputBuffer, unsigned long nBufferFrames,
                    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags status, void *userData)
        {
            float *buffer = (float *)outputBuffer;
#elif defined(MAXIMILIAN_RT_AUDIO)
        int routing(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                    double streamTime, RtAudioStreamStatus status, void *userData)
        {
            double *buffer = (double *)outputBuffer;
#endif
            double *lastValues = (double *)userData;
            unsigned int i;
            int j;

            //  double currentTime = (double) streamTime; Might come in handy for control
            if(status) logwarning("Stream underflow detected!");

            if(srcreleased)
            {
                if(vals) delete[]vals;

                size = sources.size();
                vals = new double[size];

                for(auto iter = sources.begin(); iter < sources.end(); iter++)
                    if(iter->canRelease()) sources.erase(iter);

                srcreleased = false;
            }

            // Write interleaved audio data.
            for(i = 0; i < nBufferFrames; i++)
            {
                play(lastValues);

                for(j = 0; j < maxiSettings::channels; j++)
                    *buffer++ = lastValues[j];
            }

            return 0;
        }
        void init()
        {
            if(device.getDeviceCount() < 1)
            {
                logerror("No audio devices found!");
            }

            RtAudio::StreamParameters SP;
            SP.deviceId = device.getDefaultOutputDevice();
            SP.nChannels = maxiSettings::channels;
            SP.firstChannel = 0;
            unsigned int SampleRate = maxiSettings::sampleRate;
            unsigned int BufferFrames = maxiSettings::bufferSize;
            vector<double> Data(maxiSettings::channels, 0);
            device.openStream(&SP, NULL, RTAUDIO_FLOAT64, SampleRate, &BufferFrames, &routing, Data.data());
            //Push Two Default Loaders
            sourceLoaders.insert({ "wav", [](const std::string & f, maxiSample & s)->bool { return s.load(f); } });
            sourceLoaders.insert({ "ogg", [](const std::string & f, maxiSample & s)->bool { return s.loadOgg(f); } });
        }
        SourceHandler load(const std::string & fileName)
        {
            std::shared_ptr<maxiSample> sample(new maxiSample());
            auto iter = sourceLoaders.find(UI::Data::strtolower(fileName.substr(fileName.find_last_of('.') + 1)));

            if(iter != sourceLoaders.end())
            {
                if(iter->second(fileName, *sample))
                {
                    sources.push_back(Source(sample));
                    return SourceHandler(&sources.back());
                }
                else
                {
                    logerror("The audio file:" + fileName + " is danmaged and unloadable.This MAY be caused by incorrect suffixs");
                }
            }
            else
            {
                logerror("The audio file:" + fileName + " has a format that is invalid or not supported.");
            }

            return SourceHandler(nullptr);
        }
        bool bind(const std::shared_ptr<maxiSample>& sample, SourceHandler & handler)
        {
            if(sample)
            {
                sources.push_back(Source(sample));
                handler = SourceHandler(&sources.back());
                return true;
            }

            return false;
        }
        void openStream()
        {
            device.startStream();
        }
        void unInit()
        {
            device.stopStream();
            if(device.isStreamOpen())
                device.closeStream();
        }
        void Source::play(bool loop_)
        {
            if(isPlay)
            {
                logwarning("The Source is Playing.");
            }

            loop = loop_;
            isPlay = true;
        }
        void Source::stop()
        {
            if(!isPlay)
            {
                logwarning("The Source is Stopped.");
            }

            isPlay = false;
        }
        double Source::setPos(double pos_)
        {
            double tmp = samplePos / sample->length;
            samplePos = pos_ * sample->length;
            return tmp;
        }
        double Source::operator()(size_t channel)
        {
            if(isPlay && sample)
            {
                double value = sampleValue;

                for(auto& func : effectsExs)
                {
                    value = (*func)(value, channel, this);
                }

                return value;
            }

            return 0.0;
        }
        void Source::Sample()
        {
            double lastPos = samplePos;
            sample->position = samplePos;
            sampleValue = sample->play(realFreq * freq) * gain;
            samplePos = sample->position;

            for(auto& func : effects)
            {
                sampleValue = (*func)(sampleValue, this);
            }

            isPlay = (samplePos >= lastPos) || loop;
        }
        Math::Vec3& Source::getPos()
        {
            return pos;
        }
        Math::Vec3& Source::getMove()
        {
            return move;
        }
        double Source::getFreq()
        {
            return freq;
        }
        void Source::setFreq(double newFreq)
        {
            freq = newFreq;
        }
        void Source::setGain(double newGain)
        {
            gain = newGain;
        }
        double Source::getGain()
        {
            return gain;
        }
        void Source::setRealFreq(double newFreq)
        {
            realFreq = newFreq;
        }
        void Source::addRef()
        {
            refcount++;
        }
        void Source::releaseRef()
        {
            refcount--;
        }
        bool Source::canRelease()
        {
            return refcount <= 0;
        }
        vector<shared_ptr<UI::Audio::Effect>>& Source::getEffects()
        {
            return effects;
        }
        vector<shared_ptr<UI::Audio::EffectEx>>& Source::getEffectExs()
        {
            return effectsExs;
        }
        bool Source::isStopped()
        {
            return (!isPlay) || (sample == nullptr);
        }
        Source::~Source()
        {
        }
        Source::Source() :
            sample(nullptr), isPlay(false), loop(false), refcount(0), samplePos(0.0),
            freq(1.0), gain(1.0), realFreq(1.0), sampleValue(0.0)
        {
        }
        Source::Source(std::shared_ptr<maxiSample> sample_) :
            sample(sample_), isPlay(false), loop(false), refcount(0), samplePos(0.0),
            freq(1.0), gain(1.0), realFreq(1.0), sampleValue(0.0)
        {
        }
        Source::Source(const Source & source_) :
            sample(source_.sample), isPlay(false), loop(false), refcount(0), samplePos(0.0),
            effects(source_.effects), effectsExs(source_.effectsExs), freq(1.0), realFreq(1.0), sampleValue(0.0)
        {
        }
        Math::Vec3& Listener::getPos()
        {
            return pos;
        }
        Math::Vec3 Listener::operator[](size_t channel)
        {
            if(channel >= pos2.size())
            {
                pos2.resize(channel + 1);
            }

            return pos + pos2[channel];
        }
        Math::Vec3& Listener::getChannelPos(size_t channel)
        {
            if(channel >= pos2.size())
            {
                pos2.resize(channel + 1);
            }

            return pos2[channel];
        }
        Math::Vec3& Listener::getMove()
        {
            return move;
        }
        double Effect::operator()(double input, Source * thisSource)
        {
            return input;
        }
        double EffectEx::operator()(double input, size_t channel, Source * thisSource)
        {
            return input;
        }
        double OutEffect::operator()(double input, size_t channel)
        {
            return input;
        }
        void OutEffectEx::operator()(double * input, size_t channel)
        {
        }
        SourceHandler::SourceHandler(Source * src) :
            msrc(src)
        {
            msrc->addRef();
            srcreleased = true;
        }
        SourceHandler::~SourceHandler()
        {
            msrc->releaseRef();

            if(msrc->canRelease())
                srcreleased = true;
        }
        Source& SourceHandler::get() const
        {
            return *msrc;
        }
        namespace Effects
        {
            double Balance::operator()(double input, size_t channel)
            {
                return input * ((channel < factors.size()) ? factors[channel] : 1.0);
            }
            void Balance::setFactor(size_t channel, double value)
            {
                if(channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                factors[channel] = value;
            }
            double Balance::getFactor(size_t channel)
            {
                if(channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                return factors[channel];
            }
            double Damping::operator()(double input, size_t channel)
            {
                if(channel >= values.size())
                {
                    values.resize(channel + 1);
                }

                values[channel] = input + values[channel] * ((channel < factors.size()) ? factors[channel] : 0.5);
                return values[channel];
            }
            void Damping::setFactor(size_t channel, double value)
            {
                if(channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                factors[channel] = value;
            }
            double Damping::getFactor(size_t channel)
            {
                if(channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                return factors[channel];
            }
            double EQ::operator()(double input, size_t channel)
            {
                if(channel >= samplePoints.size())
                {
                    samplePoints.resize(channel + 1);
                    freqs.resize(channel + 1);
                }

                if(samplePoints[channel].value < 0 && input > 0)
                {
                    if(samplePoints[channel].end)
                    {
                        freqs[channel] = 1000000000.0 / (std::chrono::system_clock::now() - samplePoints[channel].time).count();
                        samplePoints[channel].time = std::chrono::system_clock::now();
                        samplePoints[channel].end = false;
                        samplePoints[channel].value = input;
                    }
                    else
                    {
                        samplePoints[channel].end = true;
                    }
                }
                else
                {
                    if(samplePoints[channel].value < 0 && input > 0)
                    {
                        if(samplePoints[channel].end)
                        {
                            freqs[channel] = 1000000000.0 / (std::chrono::system_clock::now() - samplePoints[channel].time).count();
                            samplePoints[channel].time = std::chrono::system_clock::now();
                            samplePoints[channel].end = false;
                            samplePoints[channel].value = input;
                        }
                        else
                        {
                            samplePoints[channel].end = true;
                        }
                    }
                }

                //Interval
                const double interval[11][2] =
                {
                    { DBL_MIN, 31.0 },
                    { 31.0, 62.0 },
                    { 125.0, 250.0 },
                    { 250.0, 500.0 },
                    { 500.0, 1000.0 },
                    { 1000.0, 2000.0 },
                    { 2000.0, 4000.0 },
                    { 4000.0, 8000.0 },
                    { 8000.0, 16000.0 },
                    { 16000.0, DBL_MAX }
                };

                for(size_t i = 0; i < 11; i++)
                {
                    if((interval[i][0] < freqs[channel]) && (freqs[channel] <= interval[i][1]))
                    {
                        if((i == 0) || (i == 10))
                        {
                            return input + add + EQFactor[i <= 9 ? i : 9] / 32767.0;
                        }
                        else
                        {
                            double a = (freqs[channel] - interval[i][0]) / (interval[i][1] - interval[i][0]);
                            return input + add + EQFactor[i - 1] + a * (EQFactor[i] - EQFactor[i - 1]) / 32767.0;
                        }
                    }
                }

                return input + add;
            }

            double EQ::operator()(double input, Source * thisSource)
            {
                size_t channel = 0;

                if(channel >= samplePoints.size())
                {
                    samplePoints.resize(channel + 1);
                    freqs.resize(channel + 1);
                }

                if(samplePoints[channel].value < 0 && input > 0)
                {
                    if(samplePoints[channel].end)
                    {
                        freqs[channel] = 1000000000.0 / (std::chrono::system_clock::now() - samplePoints[channel].time).count();
                        samplePoints[channel].time = std::chrono::system_clock::now();
                        samplePoints[channel].end = false;
                        samplePoints[channel].value = input;
                    }
                    else
                    {
                        samplePoints[channel].end = true;
                    }
                }
                else
                {
                    if(samplePoints[channel].value < 0 && input > 0)
                    {
                        if(samplePoints[channel].end)
                        {
                            freqs[channel] = 1000000000.0 / (std::chrono::system_clock::now() - samplePoints[channel].time).count();
                            samplePoints[channel].time = std::chrono::system_clock::now();
                            samplePoints[channel].end = false;
                            samplePoints[channel].value = input;
                        }
                        else
                        {
                            samplePoints[channel].end = true;
                        }
                    }
                }

                //Interval
                const double interval[11][2] =
                {
                    { DBL_MIN, 31.0 },
                    { 31.0, 62.0 },
                    { 125.0, 250.0 },
                    { 250.0, 500.0 },
                    { 500.0, 1000.0 },
                    { 1000.0, 2000.0 },
                    { 2000.0, 4000.0 },
                    { 4000.0, 8000.0 },
                    { 8000.0, 16000.0 },
                    { 16000.0, DBL_MAX }
                };

                for(size_t i = 0; i < 11; i++)
                {
                    if((interval[i][0] < freqs[channel]) && (freqs[channel] <= interval[i][1]))
                    {
                        if((i == 0) || (i == 10))
                        {
                            return input + add + EQFactor[i <= 9 ? i : 9] / 32767.0;
                        }
                        else
                        {
                            double a = (freqs[channel] - interval[i][0]) / (interval[i][1] - interval[i][0]);
                            return input + add + EQFactor[i - 1] + a * (EQFactor[i] - EQFactor[i - 1]) / 32767.0;
                        }
                    }
                }

                return input + add;
            }
            void EQ::setFactors(Factors newFactors, double newAdd)
            {
                add = newAdd;

                for(size_t i = 0; i < 10; i++)
                {
                    EQFactor[i] = newFactors.facs[i];
                }
            }
            std::pair<double, EQ::Factors> EQ::getFactors()
            {
                Factors facs;

                for(size_t i = 0; i < 10; i++)
                {
                    facs.facs[i] = EQFactor[i];
                }

                return make_pair(add, facs);
            }
            void Amplifier::operator()(double* input, size_t channel)
            {
                double Sum = 0.0;

                for(size_t i = 0; i < channel; i++)
                {
                    Sum += input[i];
                }

                Sum /= channel;
                Sum *= factor;

                for(size_t i = 0; i < channel; i++)
                {
                    input[i] += input[i] * factor - Sum;
                }
            }
            double Amplifier::getFactor()
            {
                return factor;
            }
            void Amplifier::setFactor(double newFactor)
            {
                factor = newFactor;
            }
            double Stereo::operator()(double input, size_t channel, Source* thisSource)
            {
                double gain = 1.0;
                double distance = (thisSource->getPos() - thisListener[channel]).length();

                //formulas from << OpenAL Programmer's Guide >>
                switch(dampMode)
                {
                    case INVERSE_DISTANCE:
                        gain = referenceDistance / (referenceDistance + rolloffFactor * (distance - referenceDistance));
                        break;

                    case INVERSE_DISTANCE_CLAMPED:
                        distance = max(distance, referenceDistance);
                        distance = min(distance, maxDistance);
                        gain = referenceDistance / (referenceDistance + rolloffFactor * (distance - referenceDistance));
                        break;

                    case LINEAR_DISTANCE:
                        distance = min(distance, maxDistance); // avoid negative gain
                        gain = (1 - rolloffFactor * (distance - referenceDistance) / (maxDistance - referenceDistance));
                        break;

                    case LINEAR_DISTANCE_CLAMPED:
                        distance = max(distance, referenceDistance);
                        distance = min(distance, maxDistance); // avoid negative gain
                        gain = (1 - rolloffFactor * (distance - referenceDistance) / (maxDistance - referenceDistance));
                        break;

                    case EXPONENT_DISTANCE:
                        gain = pow(distance / referenceDistance, -rolloffFactor);
                        break;

                    case EXPONENT_DISTANCE_CLAMPED:
                        distance = max(distance, referenceDistance);
                        distance = min(distance, maxDistance);
                        gain = pow(distance / referenceDistance, -rolloffFactor);
                        break;

                    default:
                        break;
                }

                return input * gain;
            }
            void Stereo::getSettings(Mode & newMode, double & newRolloffFactor, double & newReferenceDistance, double & newMaxDistance)
            {
                newMode = dampMode;
                newRolloffFactor = rolloffFactor;
                newReferenceDistance = referenceDistance;
                newMaxDistance = maxDistance;
            }
            void Stereo::set(Mode newMode, double newRolloffFactor, double newReferenceDistance, double newMaxDistance)
            {
                dampMode = newMode;
                rolloffFactor = newRolloffFactor;
                referenceDistance = newReferenceDistance;
                maxDistance = newMaxDistance;
            }
            double Doppler::operator()(double input, Source * thisSource)
            {
                double v0 = thisListener.getMove().length();
                double vs = thisSource->getMove().length();
                double v = 0.0;
                double add = 1.0 / SampleNum;

                for(size_t i = 0; i < SampleNum; i++)
                {
                    v += speedOfSoundSampler(thisListener.getPos() + (thisSource->getPos() - thisListener.getPos()) * add * i);
                }

                v /= SampleNum;

                if((thisSource->getPos() - thisListener.getPos()).length() >
                        ((thisSource->getPos() + thisSource->getMove()) - (thisListener.getPos() + thisListener.getMove())).length()
                  )
                {
                    thisSource->setRealFreq((v + v0) / (v - vs));
                }
                else
                {
                    thisSource->setRealFreq((v - v0) / (v + vs));
                }

                return input;
            }
            Noise::Noise()
            {
                rand = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
            }
            double Noise::operator()(double input, Source * thisSource)
            {
                switch(noiseMode)
                {
                    case Add:
                        return input + rand() / double(UINT_MAX) * factor * 2.0 - factor;
                        break;

                    case Factor:
                        return input + rand() / double(UINT_MAX) * input * factor * 2.0 - input * factor;
                        break;

                    default:
                        return input;
                        break;
                }
            }
            void Noise::set(NoiseMode newMode, double newFactor)
            {
                noiseMode = newMode;
                factor = newFactor;
            }
            double Compressor::operator()(double input, Source * thisSource)
            {
                return compressor.compress(input);
            }
            maxiDyn & Compressor::getCompressor()
            {
                return compressor;
            }
        }
    }
}
