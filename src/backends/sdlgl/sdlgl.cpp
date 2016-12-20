#include "sdlgl.hpp"
#include <GL/glew.h>
#include <common/common.h>
namespace Org { namespace Infinideas { namespace NEWorld{
	namespace Backends
	{
		namespace SDLGL
		{
			GLuint gFBO = 0;
			void glBeginRenderToTexture(GLuint texture, int w, int h)
			{
				if (gFBO == 0)
				{
					glGenFramebuffers(1, &gFBO);
				}
				glBindFramebuffer(GL_FRAMEBUFFER, gFBO);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					errorstream << "FrameBuffer Creation Failure";
					Assert(false);
				}
				glViewport(0, 0, w, h);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			void glStopRenderToTexture()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}
	}
}}}

void bkBeginRenderToTexture(void * texture, int w, int h)
{
	using namespace Org::Infinideas::NEWorld::Backends::SDLGL;
	glBeginRenderToTexture(reinterpret_cast<GLuint>(texture), w, h);
}

void bkStopRenderToTexture()
{
	using namespace Org::Infinideas::NEWorld::Backends::SDLGL;
	glStopRenderToTexture();
}
