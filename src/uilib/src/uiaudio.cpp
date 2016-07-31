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
#include "uidataint.h"
#include <map>
#include <cmath>
#include <random>
#include <fstream>
#include <iostream>
#include <SDL2/SDL.h>
#include <float.h>
#include <limits.h>
#include <algorithm>

using namespace std;

#if defined( __WIN32__ ) || defined( _WIN32 )
    //#include <dsound.h>
#endif

namespace UI
{
    namespace Audio
    {
        size_t channels = 2;

        std::map<std::string, std::function<std::shared_ptr<Stream>(std::string)>> sourceLoaders;
        Listener thisListener;
        std::vector <std::shared_ptr<OutEffect>> outEffects;
        std::vector <std::shared_ptr<OutEffectEx>> outEffectExs;
        int32_t speedSampler(const Math::Vec3& pos);
        int32_t(*speedOfSoundSampler)(const Math::Vec3& pos) = speedSampler;
        unsigned char SampleNum = 2;
        std::vector<Source> sources;
        SDL_AudioDeviceID dev;
        int sampleFormat;

        // PlayData
        bool srcreleased = true;
        size_t size;
        //End PlayData

        std::mt19937 rand;

        //Formats
        class WAV : public Stream
        {
            unsigned int myChunkSize, mySubChunk1Size, length;
            short myFormat, myChannels, myBlockAlign, myBitsPerSample;
            double position;
            struct WAVChunk
            {
                int size, pos0, pos1;
                short data[32768];
            };
            int mySampleRate, myByteRate;
        public:
            WAVChunk* curChunk;
            //WAVChunk emptyChunk;
            ifstream inFile;
            std::vector<WAVChunk> chunks;
            int chunkID, count, filePos;
            double tellp() { return position; }

            void loadChunk(size_t chunk)
            {
                size_t lread = std::min(length - chunk * 32768, 32768ul) * sizeof(short);
                inFile.seekg(filePos + chunk * 32768 * sizeof(short), ios::beg);
                if (isStreaming)
                {
                    curChunk->pos0 = chunk * 32768;
                    curChunk->pos1 = chunk * 32768 + 32767;
                    curChunk->size = 32768;
                    inFile.read((char*)curChunk->data, lread);
                }
                else
                {
                    chunks[chunk].pos0 = chunk * 32768;
                    chunks[chunk].pos1 = chunk * 32768 + 32767;
                    chunks[chunk].size = 32768;

                    inFile.read((char*)(&chunks[chunk].data[0]), lread);
                }
            }

            void setp(double pos)
            {
                position = pos;
                chunkID = pos / 32768;
                if (isStreaming)
                {
                    if (curChunk != nullptr)
                    {
                        if (pos > curChunk->pos0 && pos < curChunk->pos1)
                            return;
                    }
                    loadChunk(chunkID);
                }
                else
                    curChunk = &chunks[chunkID];
            };

            void getNextChunk()
            {
                ++chunkID;
                if (chunkID == count)
                    if (looping)
                    {
                        position = 0;
                        chunkID = 0;
                        if (isStreaming)
                            inFile.clear();
                    }
                    else return;
                if (!isStreaming)
                    curChunk = &chunks[chunkID];
                else
                    loadChunk(chunkID);
            }
            int32_t get()
            {
                if (position > curChunk->pos1) getNextChunk();
                return curChunk->data[static_cast<size_t>((position++) - curChunk->pos0)];
            }
            int32_t peek()
            {
                if (position > curChunk->pos1) getNextChunk();
                return curChunk->data[static_cast<size_t>(position - curChunk->pos0)];
            }
            bool eof()
            {
                return (chunkID == count) && looping;
            }

            WAV() = default;
            void load(std::string filename, bool streaming)
            {
                isStreaming = streaming;
                int myDataSize;
                inFile = std::ifstream(filename, ios::binary);
                if (inFile.good())
                {

                    bool datafound = false;
                    inFile.seekg(4, ios::beg);
                    inFile.read((char*)&myChunkSize, sizeof(int)); // read the ChunkSize

                    inFile.seekg(16, ios::beg);
                    inFile.read((char*)&mySubChunk1Size, sizeof(int)); // read the SubChunk1Size

                    inFile.read((char*)&myFormat, sizeof(short)); // read the file format.  This should be 1 for PCM
                    inFile.read((char*)&myChannels, sizeof(short)); // read the # of channels (1 or 2)
                    inFile.read((char*)&mySampleRate, sizeof(int)); // read the samplerate
                    inFile.read((char*)&myByteRate, sizeof(int)); // read the byterate
                    inFile.read((char*)&myBlockAlign, sizeof(short)); // read the blockalign
                    inFile.read((char*)&myBitsPerSample, sizeof(short)); // read the bitspersample

                    //ignore any extra chunks
                    char chunkID[5] = "";
                    chunkID[4] = 0;
                    filePos = 20 + mySubChunk1Size;
                    while (!datafound && !inFile.eof())
                    {
                        inFile.seekg(filePos, ios::beg);
                        inFile.read((char*)&chunkID, sizeof(char) * 4);
                        inFile.seekg(filePos + 4, ios::beg);
                        inFile.read((char*)&myDataSize, sizeof(int)); // read the size of the data
                        filePos += 8;
                        if (strcmp(chunkID, "data") == 0)
                            datafound = true;
                        else
                            filePos += myDataSize;
                    }
                    length = myDataSize;
                    count = (size_t)((double)myDataSize / (sizeof(short) * 32768) + 1.0);
                    if (!isStreaming)
                    {
                        for (size_t c = 0; c < count; ++c)
                        {
                            chunks.push_back(WAVChunk());
                            loadChunk(c);
                        }
                        inFile.close();
                    }
                    else
                    {
                        curChunk = new WAVChunk();
                    }
                    setp(0.0);
                }
                else
                    cout<<"Could not load sample."<<endl;
            }
            ~WAV() = default;
        };

        int32_t speedSampler(const Math::Vec3& pos)
        {
            return 343.3;
        }

        //Put Temp Vars Together To Reduce Allocation Costs
        int32_t *buffer, *end;
        size_t i;
        void routing(void *, Uint8 * stream, int)
        {
            buffer = reinterpret_cast<int32_t*>(stream);
            end = buffer + 8192;
            if(srcreleased)
            {
                size = sources.size();
                for(auto iter = sources.begin(); iter < sources.end(); iter++)
                    if(iter->canRelease()) sources.erase(iter);

                srcreleased = false;
            }

            memset(buffer, 0, 8192 * sizeof(int32_t));
            for (; buffer < end;)
            {
                for (i = 0; i < channels; ++i)
                {
                    for (auto& s : sources) *buffer += s(i) * 65536;
                    for (auto& x : outEffects) (*x)(*buffer, i);
                    ++buffer;
                }
                for (auto&x : outEffectExs)
                    (*x)(buffer - channels, channels);
            }
        }

        void init()
        {
            if (SDL_GetNumAudioDrivers() < 1)
            {
                logerror("No audio devices found!");
            }

            SDL_AudioSpec want, have;

            SDL_memset(&want, 0, sizeof(want));
            want.freq = 44100;
            want.format = AUDIO_S32;
            want.channels = 2;
            want.samples = 4096;
            want.callback = routing;  // you wrote this function elsewhere.

            dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
            if (dev == 0) logerror("Failed to open audio:" + std::string(SDL_GetError()));
            if (have.format != want.format)
            {
                printf("We didn't get int32_t audio format, currently the playing will fail");
            }
            SDL_PauseAudioDevice(dev, 0); // start audio playing.
            sourceLoaders.insert({ "wav",[](std::string file)-> std::shared_ptr<Stream>
                {
                    auto c = std::make_shared<WAV>();
                    c->load(file, true);
                    return c;
                }
            });
        }
        SourceHandler load(const std::string & fileName)
        {
            std::shared_ptr<Stream> sample;
            auto iter = sourceLoaders.find(UI::Data::strtolower(fileName.substr(fileName.find_last_of('.') + 1)));

            if(iter != sourceLoaders.end())
            {
                if(sample = iter->second(fileName))
                {
                    sources.push_back(Source(sample));
                    srcreleased = true;
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
        bool bind(const std::shared_ptr<Stream>& sample, SourceHandler & handler)
        {
            if(sample)
            {
                sources.push_back(Source(sample));
                handler = SourceHandler(&sources.back());
                return true;
            }

            return false;
        }
        void unInit()
        {
            SDL_PauseAudioDevice(dev, 1); // start audio playing.
            SDL_CloseAudioDevice(dev);
        }
        void Source::play(bool loop_)
        {
            if(isPlay) logwarning("The Source is Playing.");

            sample->looping = loop_;
            isPlay = true;
        }
        void Source::stop()
        {
            if(!isPlay) logwarning("The Source is Stopped.");

            isPlay = false;
        }

        void Source::setPos(double pos_)
        {
            sample->setp(pos_ * sample->length);
        }

        int32_t Source::operator()(size_t channel)
        {
            if(isPlay && sample)
            {
                int32_t value = Sample();
                for(auto& func : effectsExs)
                    (*func)(value, channel, this);
                return value;
            }

            return 0.0;
        }
        int32_t Source::Sample()
        {
            int32_t sampleValue = sample->get() * gain;
            for(auto& func : effects)
                (*func)(sampleValue, this);

            isPlay = !sample->eof();
            return sampleValue;
        }
        Math::Vec3& Source::getPos()
        {
            return pos;
        }
        Math::Vec3& Source::getMove()
        {
            return move;
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
        std::vector<std::shared_ptr<UI::Audio::Effect>>& Source::getEffects()
        {
            return effects;
        }
        std::vector<std::shared_ptr<UI::Audio::EffectEx>>& Source::getEffectExs()
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
            sample(nullptr), isPlay(false), refcount(0),
            freq(1.0), gain(1.0), realFreq(1.0)
        {
        }
        Source::Source(std::shared_ptr<Stream> sample_) :
            sample(sample_), isPlay(false), refcount(0),
            freq(1.0), gain(1.0), realFreq(1.0)
        {
        }
        Source::Source(const Source & source_) :
            sample(source_.sample), isPlay(false), refcount(0),
            effects(source_.effects), effectsExs(source_.effectsExs), freq(1.0), realFreq(1.0)
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
            void Balance::operator()(int32_t& input, size_t channel)
            {
                input *= ((channel < factors.size()) ? factors[channel] : 1.0);
            }
            void Balance::setFactor(size_t channel, double value)
            {
                if (channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                factors[channel] = value;
            }
            double Balance::getFactor(size_t channel)
            {
                if (channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                return factors[channel];
            }
            void Damping::operator()(int32_t& input, size_t channel)
            {
                if (channel >= values.size())
                    values.resize(channel + 1);

                values[channel] = input += values[channel] * ((channel < factors.size()) ? factors[channel] : 0.5);
            }
            void Damping::setFactor(size_t channel, double value)
            {
                if (channel >= factors.size())
                    factors.resize(channel + 1);

                factors[channel] = value;
            }
            double Damping::getFactor(size_t channel)
            {
                if (channel >= factors.size())
                {
                    factors.resize(channel + 1);
                }

                return factors[channel];
            }
            void EQ::operator()(int32_t& input, size_t channel)
            {
                if (channel >= samplePoints.size())
                {
                    samplePoints.resize(channel + 1);
                    freqs.resize(channel + 1);
                }

                if (samplePoints[channel].value < 0 && input > 0)
                {
                    if (samplePoints[channel].end)
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
                    if (samplePoints[channel].value < 0 && input > 0)
                    {
                        if (samplePoints[channel].end)
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

                for (size_t i = 0; i < 11; i++)
                {
                    if ((interval[i][0] < freqs[channel]) && (freqs[channel] <= interval[i][1]))
                    {
                        if ((i == 0) || (i == 10))
                        {
                            input += add + EQFactor[i <= 9 ? i : 9] / 32767.0;
                            return;
                        }
                        else
                        {
                            double a = (freqs[channel] - interval[i][0]) / (interval[i][1] - interval[i][0]);
                            input += add + EQFactor[i - 1] + a * (EQFactor[i] - EQFactor[i - 1]) / 32767.0;
                            return;
                        }
                    }
                }

                input += add;
            }

            void EQ::operator()(int32_t& input, Source * thisSource)
            {
                size_t channel = 0;

                if (channel >= samplePoints.size())
                {
                    samplePoints.resize(channel + 1);
                    freqs.resize(channel + 1);
                }

                if (samplePoints[channel].value < 0 && input > 0)
                {
                    if (samplePoints[channel].end)
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
                    if (samplePoints[channel].value < 0 && input > 0)
                    {
                        if (samplePoints[channel].end)
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

                for (size_t i = 0; i < 11; i++)
                {
                    if ((interval[i][0] < freqs[channel]) && (freqs[channel] <= interval[i][1]))
                    {
                        if ((i == 0) || (i == 10))
                        {
                            input += add + EQFactor[i <= 9 ? i : 9] / 32767.0;
                            return;
                        }
                        else
                        {
                            double a = (freqs[channel] - interval[i][0]) / (interval[i][1] - interval[i][0]);
                            input += add + EQFactor[i - 1] + a * (EQFactor[i] - EQFactor[i - 1]) / 32767.0;
                            return;
                        }
                    }
                }

                input += add;
            }
            void EQ::setFactors(Factors newFactors, double newAdd)
            {
                add = newAdd;

                for (size_t i = 0; i < 10; i++)
                {
                    EQFactor[i] = newFactors.facs[i];
                }
            }
            std::pair<double, EQ::Factors> EQ::getFactors()
            {
                Factors facs;

                for (size_t i = 0; i < 10; i++)
                {
                    facs.facs[i] = EQFactor[i];
                }

                return std::make_pair(add, facs);
            }
            void Amplifier::operator()(int32_t* input, size_t channel)
            {
                int32_t Sum = 0.0;

                for (size_t i = 0; i < channel; i++)
                {
                    Sum += input[i];
                }

                Sum /= channel;
                Sum *= factor;

                for (size_t i = 0; i < channel; i++)
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
            int32_t Stereo::operator()(int32_t input, size_t channel, Source* thisSource)
            {
                double gain = 1.0;
                double distance = (thisSource->getPos() - thisListener[channel]).length();

                //formulas from << OpenAL Programmer's Guide >>
                switch (dampMode)
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
            void Doppler::operator()(int32_t& input, Source * thisSource)
            {
                double v0 = thisListener.getMove().length();
                double vs = thisSource->getMove().length();
                double v = 0.0;
                double add = 1.0 / SampleNum;
                Math::Vec3 rpos = thisSource->getPos() - thisListener.getPos();
                for (size_t i = 0; i < SampleNum; i++)
                    v += speedOfSoundSampler(thisListener.getPos() + rpos * add * i);

                v /= SampleNum;

                thisSource->realFreq = (v + v0) / ((rpos.lengthSqr() >(rpos + thisSource->getMove() - thisListener.getMove()).lengthSqr())
                                                   ? (v - vs) : (v + vs));
            }
            Noise::Noise()
            {
                rand = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
            }
            void Noise::operator()(int32_t& input, Source * thisSource)
            {
                switch (noiseMode)
                {
                case Add:
                    input += rand() / double(UINT_MAX) * factor * 2.0 - factor;
                    break;

                case Factor:
                    input += rand() / double(UINT_MAX) * input * factor * 2.0 - input * factor;
                    break;

                default:
                    break;
                }
            }
            void Noise::set(NoiseMode newMode, double newFactor)
            {
                noiseMode = newMode;
                factor = newFactor;
            }
        }
    }
}
