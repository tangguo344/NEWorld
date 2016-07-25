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

#ifndef UIAUDIO_H_
#define UIAUDIO_H_

#include "uimath.h"
#include <map>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <tuple>

namespace UI
{

    namespace Audio
    {
        class UILIB_API Source;

        class UILIB_API Effect
        {
        public:
            Effect() = default;
            virtual ~Effect() = default;
            virtual void operator()(int32_t& input, Source* thisSource) {};
        };

        class UILIB_API EffectEx
        {
        public:

            EffectEx() = default;
            virtual ~EffectEx() = default;
            virtual void operator()(int32_t& input, size_t channel, Source* thisSource) {};
        };

        class UILIB_API Stream
        {
        public:
            bool looping;
            size_t length, sampleRate, channels;
            double speed, deviceSampleRate;
            Stream() = default;
            virtual ~Stream() = default;
            virtual double tellp() = 0;
            virtual void setp(double) = 0;
            virtual int32_t get() = 0;
            virtual int32_t peek() = 0;
            virtual bool eof() = 0;
        protected:
            bool isStreaming;
        };

        class UILIB_API Source
        {
        private:
            Math::Vec3 pos;
            bool isPlay;
            std::shared_ptr<Stream> sample;
            Math::Vec3 move;
            size_t refcount;
            std::vector<std::shared_ptr<UI::Audio::Effect>> effects;
            std::vector<std::shared_ptr<UI::Audio::EffectEx>> effectsExs;
        public:
            Source();
            Source(const Source& source_);
            Source(std::shared_ptr<Stream> sample_);
            ~Source();
            bool isStopped();
            void play(bool loop);
            void stop();
            void setPos(double Pos_ = 0.0);
            int32_t operator()(size_t channel);
            int32_t Sample();
            Math::Vec3& getPos();
            Math::Vec3& getMove();
            double freq;//for User
            double realFreq;//for Effects
            double gain;
            void addRef();
            void releaseRef();
            bool canRelease();
            std::vector<std::shared_ptr<UI::Audio::Effect>>& getEffects();
            std::vector<std::shared_ptr<UI::Audio::EffectEx>>& getEffectExs();
        };

        class UILIB_API SourceHandler
        {
        public:
            SourceHandler() = default;
            SourceHandler(Source* src);
            ~SourceHandler();
            Source& get() const;
        private:
            Source* msrc;
        };

        class UILIB_API Listener
        {
        private:
            Math::Vec3 pos;
            std::vector<Math::Vec3> pos2;
            Math::Vec3 move;
        public:

            Listener() = default;
            ~Listener() = default;
            //operator Math::Vec3&();
            Math::Vec3& getPos();
            Math::Vec3 operator[](size_t channel);
            Math::Vec3& getChannelPos(size_t channel);
            Math::Vec3& getMove();
        };

        class UILIB_API OutEffect
        {
        public:
            OutEffect() = default;
            virtual ~OutEffect() = default;
            virtual void operator()(int32_t& input, size_t channel) {};
        };

        class UILIB_API OutEffectEx
        {
        public:
            OutEffectEx() = default;
            virtual ~OutEffectEx() = default;
            virtual void operator()(int32_t* input, size_t channel) {};
        };
        
        UILIB_API extern std::map<std::string, std::function<std::shared_ptr<Stream>(std::string)>> sourceLoaders;

        UILIB_API extern std::vector <std::shared_ptr<UI::Audio::OutEffect>> outEffects;

        UILIB_API extern std::vector <std::shared_ptr<UI::Audio::OutEffectEx>> outEffectExs;

        UILIB_API extern Listener thisListener;

        UILIB_API extern int32_t(*speedOfSoundSampler)(const Math::Vec3& pos);

        UILIB_API extern unsigned char SampleNum;

        UILIB_API void init();

        UILIB_API SourceHandler load(const std::string& fileName);

        UILIB_API void unInit();


        namespace Effects
        {
            /*
            Balance
            名称:声道平衡
            作用:调节声道音量,或者作为Stereo的替代品.
            */
            class UILIB_API Balance : public OutEffect
            {
            private:
                std::vector<double> factors;
            public:
                Balance() = default;
                ~Balance() = default;
                virtual void operator()(int32_t& input, size_t channel);
                void setFactor(size_t channel, double value);
                double getFactor(size_t channel);
            };

            /*
            Damping
            名称:衰减
            作用:计算声音衰减,不可作为回声的替代品.
            */
            class UILIB_API Damping : public OutEffect
            {
            private:
                std::vector<double> values;
                std::vector<double> factors;
            public:
                Damping() = default;
                ~Damping() = default;
                virtual void operator()(int32_t& input, size_t channel);
                void setFactor(size_t channel, double value);
                double getFactor(size_t channel);
            };

            /*
            EQ
            名称:EQ均衡器
            作用:用于播放BGM或空间感渲染.参数以16bit为准.
            */
            class UILIB_API EQ : public OutEffect, public Effect
            {

            private:
                struct Point
                {
                    std::chrono::time_point<std::chrono::system_clock> time = std::chrono::system_clock::now();
                    double value = 0.0;
                    bool end = false;
                };
                std::vector<Point> samplePoints;
                std::vector<double> freqs;
                double EQFactor[10];
                double add = 0.0;
            public:
                struct Factors
                {
                    double facs[10];
                };
                //Preset from http://blog.sina.com.cn/s/blog_4cb5837d01018rax.html
                //                                 31   62  125 250 500 1K  2K  4K   8K  16K
                const Factors bass = { { 6.0, 4.0, -5.0, 2.0, 3.0, 4.0, 4.0, 5.0, 5.0, 6.0 } };
                const Factors rock = { { 6.0, 4.0, 0.0, -2.0, -6.0, 1.0, 4.0, 6.0, 7.0, 9.0 } };
                const Factors vocal = { { 4.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 4.0, 3.0, 3.0 } };


                EQ() = default;
                ~EQ() = default;
                virtual void operator()(int32_t& input, size_t channel);
                virtual void operator()(int32_t& input, Source* thisSource);
                void setFactors(Factors newFactors, double newAdd = 0.0);
                std::pair<double, Factors> getFactors();
            };

            /*
            Amplifier
            名称:差异放大器
            作用:放大各声道的差异,提高立体感,作为Stereo的补偿.
            */
            class UILIB_API Amplifier : public OutEffectEx
            {
            private:
                double factor = 1.0;
            public:
                Amplifier() = default;
                ~Amplifier() = default;
                virtual void operator()(int32_t* input, size_t channel);
                double getFactor();
                void setFactor(double newFactor = 1.0);
            };

            /*
            Stereo
            名称:3D效果器
            作用:通过声音衰减模型计算音量,是立体感的基础,与Listener的pos2搭配使用.
            */
            class UILIB_API Stereo : public EffectEx
            {
            public:
                enum Mode
                {
                    INVERSE_DISTANCE,
                    INVERSE_DISTANCE_CLAMPED,
                    LINEAR_DISTANCE,
                    LINEAR_DISTANCE_CLAMPED,
                    EXPONENT_DISTANCE,
                    EXPONENT_DISTANCE_CLAMPED
                };
                Stereo() = default;
                ~Stereo() = default;
                virtual int32_t operator()(int32_t input, size_t channel, Source* thisSource);
                void getSettings(Mode& newMode, double& newRolloffFactor, double& newReferenceDistance, double& newMaxDistance);
                void set(Mode newMode = EXPONENT_DISTANCE_CLAMPED, double newRolloffFactor = 1.0, double newReferenceDistance = 1.0, double newMaxDistance = 100.0);
            private:
                double rolloffFactor = 1.0;
                double referenceDistance = 1.0;
                double maxDistance = 100.0;
                Mode dampMode = EXPONENT_DISTANCE_CLAMPED;
            };

            /*
            Doppler
            名称:多普勒效应
            作用:通过多普勒效应模型计算频率,提高立体感.(要获得更好的体验请更换speedOfSoundSampler函数,并提高采样次数)
            */
            class UILIB_API Doppler : public Effect
            {
            public:
                Doppler() = default;
                ~Doppler() = default;
                virtual void operator()(int32_t& input, Source* thisSource);
            };

            /*
            Noise
            名称:噪音生成器
            作用:用于生成噪音并叠加在原波形上,用于播放自然声音.
            */
            class UILIB_API Noise : public Effect
            {
            public:
                Noise();
                ~Noise() = default;
                enum NoiseMode
                {
                    Add,
                    Factor
                };
                virtual void operator()(int32_t& input, Source* thisSource);
                void set(NoiseMode newMode = Factor, double newFactor = 0.1);
            private:
                NoiseMode noiseMode = Factor;
                double factor = 0.1;
            };
        }

        template<typename Base, typename ...ArgTs>
        class EffectFactory
        {
        private:
            std::tuple<ArgTs...> templates;
            std::shared_ptr<Base> base;
            bool _enable[std::tuple_size<std::tuple<ArgTs...>>::value];

            template<typename T>
            static auto R2V(T& ref)->T
            {
                return ref;
            }

            template <size_t i, typename _Base, typename ..._ArgTs>
            struct EF
            {
                static void make(std::vector<std::shared_ptr<_Base>>& effects,
                                 std::tuple<_ArgTs...>& _templates,
                                 std::shared_ptr<_Base>& _base,
                                 bool __enable[])
                {
                    effects[i] = __enable[i] ? static_cast<decltype(R2V(_base))>(std::make_shared<decltype(R2V(std::get<i>(_templates)))>(std::get<i>(_templates))) : _base;
                    EF < i - 1, _Base, _ArgTs... > ().make(effects, _templates, _base, __enable);
                }
            };

            template <typename _Base, typename ..._ArgTs>
            struct EF<0, _Base, _ArgTs...>
            {
                static void make(std::vector<std::shared_ptr<_Base>>& effects,
                                 std::tuple<_ArgTs...>& _templates,
                                 std::shared_ptr<_Base>& _base,
                                 bool __enable[])
                {
                    effects[0] = __enable[0] ? static_cast<decltype(R2V(_base))>(std::make_shared <decltype(R2V(std::get<0>(_templates)))>(std::get<0>(_templates))) : _base;
                }
            };

            template <class EffectT, class EffectT2, class...EffectTs>
            void safeCheck()
            {
                Base* bptr = (EffectT*)(nullptr);
                safeCheck<EffectT2, EffectTs...>();
            }
            template <class EffectT>
            void safeCheck()
            {
                Base* bptr = (EffectT*)(nullptr);
            }

            template<size_t i>
            void makeEffects(std::vector<std::shared_ptr<Base>>& effects)
            {
                EF<i, Base, ArgTs...>::make(effects, templates, base, _enable);
            }

        public:

            EffectFactory()
            {
                safeCheck<ArgTs...>();

                for(size_t i = 0; i < std::tuple_size<std::tuple<ArgTs...>>::value; i++)
                {
                    _enable[i] = true;
                }

                base = std::make_shared<Base>();
            }

            ~EffectFactory() = default;

            template<size_t i>
            auto& get()
            {
                return std::get<i>(templates);
            }

            auto genEffects()
            {
                std::vector<std::shared_ptr<Base>> effects(std::tuple_size<std::tuple<ArgTs...>>::value);
                makeEffects < std::tuple_size<std::tuple<ArgTs...>>::value - 1 > (effects);
                return effects;
            }

            template<size_t i>
            void enable()
            {
                static_assert(i < std::tuple_size<std::tuple<ArgTs...>>::value, "Out of range.");
                _enable[i] = true;
            }

            template<size_t i>
            void disable()
            {
                static_assert(i < std::tuple_size<std::tuple<ArgTs...>>::value, "Out of range.");
                _enable[i] = false;
            }
        };

    }
}

#endif
