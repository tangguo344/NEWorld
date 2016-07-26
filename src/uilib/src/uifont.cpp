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

#include <uifont.h>
#include <uilogger.h>
#include <GL/glew.h>
#include <freetype/ftsizes.h>
#include <boost/filesystem.hpp>
#include <boost/locale/encoding_utf.hpp>
namespace UI
{
    namespace Font
    {
        using ubyte = unsigned char;
        FT_Library library;
        double stretch = 1.0;

        struct FNTT
        {
            float geo[72]; //vertex, tex, color
        };

        std::vector<FNTT> vtrary = {};
        unsigned int tex;
        size_t layer = 0;
        UnicodeChar::UnicodeChar() : tex(0), xpos(0), ypos(0), width(0), height(0), advance(0)
        {
        }

#define MByteToWChar(x) boost::locale::conv::utf_to_utf<wchar_t>(x)
#define WCharToMByte(x) boost::locale::conv::utf_to_utf<char>(x)

        unsigned int newTex(size_t x)
        {
            unsigned int ret;
            glGenTextures(1, &ret);
            glBindTexture(GL_TEXTURE_2D, ret);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, x, x, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
            return ret;
        }

        UnicodeChar FontOfSize::GetChar(const wchar_t uc)
        {
            auto it = chars.find(uc);
            if (texBuffers.empty())
            {
                int tsize = 0;
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tsize);
                if (tsize > 1024) tsize = 1024;
                maxBufferX = tsize / (int)pow(2, ceil(log2(height)));
                texBuffers.push_back(tex = curBufferID = newTex(maxBufferX*(int)pow(2, ceil(log2(height)))));
                curX = 0, curY = 0;
            }
            if(it != chars.end())
            {
                return it->second;
            }
            else
            {
                UnicodeChar c;
                size_t index = FT_Get_Char_Index(fontface, static_cast<FT_ULong>(uc));
                FT_Activate_Size(size);
                FT_Load_Glyph(fontface, index, FT_LOAD_DEFAULT);
                FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
                FT_Bitmap* bitmap = &(slot->bitmap);
                ubyte *Texsrc = bitmap->buffer, *Tex = new ubyte[wid * wid];

                for(size_t i = 0; i < bitmap->rows; i++)
                    for(size_t j = 0; j < bitmap->width; j++)
                        Tex[i * wid + j] = *(Texsrc++);

                glBindTexture(GL_TEXTURE_2D, c.tex = curBufferID);
                glTexSubImage2D(GL_TEXTURE_2D, 0, curX * wid, curY * wid, wid, wid, GL_ALPHA, GL_UNSIGNED_BYTE, Tex);
                c.twid = 1.0 / maxBufferX;
                c.xzero = (1.0 / maxBufferX)*curX;
                c.yzero = (1.0 / maxBufferX)*curY;
                curX++;
                if (curX >= maxBufferX)
                {
                    curX = 0;
                    curY++;
                }
 
                delete[] Tex;
                c.width = bitmap->width;
                c.height = bitmap->rows;
                c.advance = slot->advance.x >> 6;
                c.xpos = slot->bitmap_left;
                c.ypos = slot->bitmap_top;
                chars.insert({ uc, c });
                return c;
            }
        }

        FontOfSize::FontOfSize(const int _height, FT_Face _face) :
            height(_height), fontface(_face)
        {
            FT_New_Size(fontface, &size);
            FT_Activate_Size(size);
            FT_Set_Pixel_Sizes(fontface, static_cast<FT_UInt>(height), static_cast<FT_UInt>(height));
            slot = fontface->glyph;
            wid = (int)pow(2, ceil(log2(height)));
        }

        FontRenderer::FontRenderer(FontOfSize & _fos, Color col)
            : fos(_fos), color(col)
        {
        }

        void FontRenderer::renderStr(int x, int y, const Rect& r, const std::wstring & str) const
        {
            UnicodeChar c;
            int span = 0, tpos = fos.height - 1;

            for(wchar_t wc : str)
            {
                c = fos.GetChar(wc);
                float wid = pow(2, ceil(log2(fos.height)));
                float p1x = x + span + c.xpos, p1y = y + tpos - c.ypos,
                    p2x = x + span + c.xpos + c.width, p2y = y + tpos + c.height - c.ypos;
                float t1x = 0.0, t1y = 0.0,
                       t2x = c.width / wid * c.twid, t2y = c.height / wid * c.twid;
                span += c.advance;

                //Apply Clipping
                if (p1x > r.xmax) return;
                if (p2x < r.xmin) continue;   //already exceeds max or before the rect, clipped

                if(p1x < r.xmin)
                {
                    t1x = t2x * ((r.xmin - p1x) / c.width);
                    p1x = r.xmin;
                }

                if(p2x > r.xmax)
                {
                    t2x *= (1 - (p2x - r.xmax) / c.width);
                    p2x = r.xmax;
                }
               
                t1x += c.xzero; t2x += c.xzero;
                t1y += c.yzero; t2y += c.yzero;

                //tex:2 color:4, vtx:2
                vtrary.push_back(
                {
                t1x, t1y, 0.5, 0.5, 0.5, (float)color.t, p1x + 1.0f, p1y + 1.0f, (float)layer,
                t2x, t1y, 0.5, 0.5, 0.5, (float)color.t, p2x + 1.0f, p1y + 1.0f, (float)layer,
                t2x, t2y, 0.5, 0.5, 0.5, (float)color.t, p2x + 1.0f, p2y + 1.0f, (float)layer,
                t1x, t2y, 0.5, 0.5, 0.5, (float)color.t, p1x + 1.0f, p2y + 1.0f, (float)layer,

                t1x, t1y, (float)color.x, (float)color.y, (float)color.z, (float)color.t, p1x, p1y, (float)layer + 0.5f,
                t2x, t1y, (float)color.x, (float)color.y, (float)color.z, (float)color.t, p2x, p1y, (float)layer + 0.5f,
                t2x, t2y, (float)color.x, (float)color.y, (float)color.z, (float)color.t, p2x, p2y, (float)layer + 0.5f,
                t1x, t2y, (float)color.x, (float)color.y, (float)color.z, (float)color.t, p1x, p2y, (float)layer + 0.5f
                }
                );
                //layer += 1;
                tex = c.tex;
            }
        }

        void FontRenderer::renderStr(int x, int y, const Rect& r, const std::string & str) const
        {
            renderStr(x, y, r, MByteToWChar(str));
        }

        double FontRenderer::getStrWidth(const std::wstring & str) const
        {
            int res = 0;

            for(wchar_t wc : str)
                res += fos.GetChar(wc).advance;

            return res;
        }

        double FontRenderer::getStrWidth(const std::string & str) const
        {
            return getStrWidth(MByteToWChar(str));
        }

        FontBase::FontBase(std::string _path):
            path(_path)
        {
            FT_New_Face(library, path.c_str(), 0, &fontface);
        }

        std::shared_ptr<FontRenderer> FontBase::getRenderer(int height, Color col)
        {
            if(sizes.find(height) == sizes.end())
                sizes.insert({ height, FontOfSize(height, fontface) });
            return std::shared_ptr<FontRenderer>(new FontRenderer(sizes[height], col));
        }

        void FontBase::gc()
        {
        }

        Service::Service()
        {
            FT_Init_FreeType(&library);
        }

        void Service::addSearchPaths(std::initializer_list<std::string> _apths)
        {
            for(auto c : _apths)
                searchpaths.push_back(c);
        }

        std::shared_ptr<FontRenderer> Service::getRenderer(const std::string & font, int height, Color col)
        {
            using boost::filesystem::exists;
            if(fonts.find(font) == fonts.end())
            {
                std::string path;

                for(auto c : searchpaths)
                {
                    if(exists(c + "/" + font + ".ttf")) path = c + "/" + font + ".ttf";

                    if(exists(c + "/" + font + ".TTF")) path = c + "/" + font + ".TTF";

                    if(exists(c + "/" + font + ".otf")) path = c + "/" + font + ".otf";

                    if(exists(c + "/" + font + ".OTF")) path = c + "/" + font + ".OTF";
                }
                if(path.empty())
                {
                    logerror("Font: " + font + " not found!");
                }
                fonts.insert({ font, FontBase(path) });
            }

            return fonts[font].getRenderer(height, col);
        }

        void Service::gc()
        {
        }

        Service service;

        void flush()
        {
            layer = 1;
            glEnable(GL_TEXTURE_2D);
            glColor4f(0.0, 0.0, 0.0, 1.0);
            glBindTexture(GL_TEXTURE_2D, tex);
            glVertexPointer(3, GL_FLOAT, 9 * sizeof(float), reinterpret_cast<float*>(vtrary.data()) + 6 );
            glColorPointer(4, GL_FLOAT, 9 * sizeof(float), reinterpret_cast<float*>(vtrary.data()) + 2);
            glTexCoordPointer(2, GL_FLOAT, 9 * sizeof(float), reinterpret_cast<float*>(vtrary.data()));
            glDrawArrays(GL_QUADS, 0, vtrary.size() * 8);
            glDisable(GL_TEXTURE_2D);
            vtrary.clear();
        }
    }
}
