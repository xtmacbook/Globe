#include "StdAfx.h"
#include "GlbGlobeSceneManager.h"

#include "GlbPath.h"
#include "GlbWString.h"
#include "GlbString.h"

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>
#include "osg/Texture2D"
#include "osg/StateSet"


namespace GlbGlobe
{
	CGlbGlobeSceneManager::CGlbGlobeSceneManager(void)
	{
		mpr_funType=0;
		mpr_viewDepthTextureUnit=1;
	}

	CGlbGlobeSceneManager::~CGlbGlobeSceneManager(void)
	{
		mpr_viewSceneSS = NULL;
	}

	bool CGlbGlobeSceneManager::setFunType( EFunType funType )
	{
		if (funType&FT_Default)
		{
		}
		else if (funType&FT_ViewAnalysis)
		{
			initViewSceneStateSet();
		}
		mpr_funType=mpr_funType|funType;
		
		return true;
	}
	
	void CGlbGlobeSceneManager::traverse(osg::NodeVisitor& nv)
	{
		if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
		{
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(&nv);
			if (cv) 
				cull(*cv);
			else 
				osg::Group::traverse(nv);
		}
		else
		{
			osg::Group::traverse(nv);
		}
	}

	void CGlbGlobeSceneManager::cull(osgUtil::CullVisitor& cv)
	{
		mpr_ModelViewMat=*cv.getModelViewMatrix();
		if (mpr_funType==0)
		{
			//osg::StateSet *ss=mpr_defaultSS.get();
			osg::StateSet *ss=getStateSet();
			if (ss)
				cv.pushStateSet(ss);
			osg::Group::traverse(cv);
			if (ss)
				cv.popStateSet();
		}
		else if (mpr_funType==1)
		{
			if (mpr_viewSceneSS.valid())
			{
				osg::StateSet *ss=mpr_viewSceneSS.get();
				if (ss)
					cv.pushStateSet(ss);
				osg::Group::traverse(cv);
				if (ss)
					cv.popStateSet();
			}
		}		
	}

	osg::Shader* readShaderFile(const std::string& filename)
	{
		// The .vert and .frag extensions were added to the GLSL plugin in OSG 
		// 2.7.3, and the automatic setting of shader type depending on the 
		// extension was added in OSG 2.9.1. The code below lets us use OSG
		// 2.6 and still get the same behavior.
#if OPENSCENEGRAPH_MAJOR_VERSION > 2 || \
	(OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION > 9) || \
	(OPENSCENEGRAPH_MAJOR_VERSION == 2 && OPENSCENEGRAPH_MINOR_VERSION == 9 && OPENSCENEGRAPH_PATCH_VERSION >= 1)
		// This will search the registry's file path.
		return osgDB::readShaderFile(filename);
#else
		// Determine shader type from the extension
		osg::Shader::Type type = osg::Shader::UNDEFINED;
		if (filename.find("vert") == filename.length() - 4)
			type = osg::Shader::VERTEX;
		else if (filename.find("frag") == filename.length() - 4)
			type = osg::Shader::FRAGMENT;
		else
			return NULL;

		// Find the shader file in the osgDB data path list.
		std::string fullpath = osgDB::findDataFile(filename);
		if (fullpath.empty())
			return NULL;

		// Read the shader file.
		osg::Shader* shader = osg::Shader::readShaderFile(type, fullpath);
		return shader;
#endif
	}

	/** Creates a shader program using either the given strings as shader 
	*  source directly, or as filenames to load the shaders from disk, 
	*  depending on the value of the \c loadFromFiles parameter.
	*/
	osg::Program* createShaderProgramEx( const std::string& name,const std::string& vertexSrc, const std::string& fragmentSrc, bool loadFromFiles )
	{
		osg::ref_ptr<osg::Shader> vShader = NULL;
		osg::ref_ptr<osg::Shader> fShader = NULL;

		if (loadFromFiles)
		{
			vShader = readShaderFile(vertexSrc);
			if (!vShader)
			{
				//osg::notify(osg::WARN) << "Could not read shader from file " << vertexSrc << std::endl;
				//return NULL;
			}

			fShader = readShaderFile(fragmentSrc);
			if (!fShader)
			{
				//osg::notify(osg::WARN) << "Could not read shader from file " << fragmentSrc << std::endl;
				//return NULL;
			}
		}
		else
		{
			if (!vertexSrc.empty())
			{
				vShader = new osg::Shader( osg::Shader::VERTEX, vertexSrc );
			}
			if (!fragmentSrc.empty())
			{
				fShader = new osg::Shader( osg::Shader::FRAGMENT, fragmentSrc );
			}
		}

		osg::Program* program = new osg::Program;
		program->setName(name);

		if (vShader.valid())
		{
			vShader->setName(name+"_vertex_shader");
			program->addShader( vShader.get() );
		}
		if (fShader.valid())
		{
			fShader->setName(name+"_fragment_shader");
			program->addShader( fShader.get() );
		}

		return program;
	}
	bool CGlbGlobeSceneManager::initViewSceneStateSet()
	{
		if (!mpr_viewDepthTexture.valid())
		{
			mpr_viewDepthTexture = new osg::Texture2D;
			mpr_viewDepthTexture->setTextureSize(1920, 1920);
			mpr_viewDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT);
			mpr_viewDepthTexture->setShadowTextureMode(osg::Texture2D::LUMINANCE);
			mpr_viewDepthTexture->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::NEAREST);
			mpr_viewDepthTexture->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::NEAREST);
			mpr_viewDepthTexture->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_BORDER);
			mpr_viewDepthTexture->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_BORDER);
			mpr_viewDepthTexture->setBorderColor(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
			mpr_viewDepthTexture->setShadowComparison(true);
			mpr_viewDepthTexture->setShadowCompareFunc( osg::Texture::LESS );
		}
		
		if (!mpr_viewSceneSS.valid())
		{
			mpr_viewSceneSS=new osg::StateSet();
			mpr_viewSceneSS->setTextureAttributeAndModes(mpr_viewDepthTextureUnit,mpr_viewDepthTexture.get(),osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
			mpr_viewSceneSS->setTextureMode(mpr_viewDepthTextureUnit,GL_TEXTURE_GEN_S,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
			mpr_viewSceneSS->setTextureMode(mpr_viewDepthTextureUnit,GL_TEXTURE_GEN_T,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
			mpr_viewSceneSS->setTextureMode(mpr_viewDepthTextureUnit,GL_TEXTURE_GEN_R,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);
			mpr_viewSceneSS->setTextureMode(mpr_viewDepthTextureUnit,GL_TEXTURE_GEN_Q,osg::StateAttribute::ON| osg::StateAttribute::OVERRIDE);


			osg::Uniform *shadowTextureSampler = new osg::Uniform("osgShadow_shadowTexture",(int)mpr_viewDepthTextureUnit);
			mpr_viewSceneSS->addUniform(shadowTextureSampler);


			HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
			CGlbWString workdir = CGlbPath::GetModuleDir(hmd);
			CGlbWString viewVertexShader = workdir +L"";  
			CGlbWString viewFragmentShader = workdir +L"\\res\\view_depthmap.frag";  
			osg::ref_ptr<osg::Program> _program = createShaderProgramEx("viewShader", viewVertexShader.ToString(), viewFragmentShader.ToString(), true);
			mpr_viewSceneSS->setAttribute(_program,osg::StateAttribute::ON|osg::StateAttribute::OVERRIDE);
		}

		return true;  
		{ 
			// fake texture for baseTexture, add a fake texture 
			// we support by default at least one texture layer
			// without this fake texture we can not support
			// textured and not textured scene 

			// TODO: at the moment the PSSM supports just one texture layer in the GLSL shader, multitexture are
			//       not yet supported !

			//osg::Image* image = new osg::Image; 
			//// allocate the image data, noPixels x 1 x 1 with 4 rgba floats - equivilant to a Vec4!
			//int noPixels = 1; 
			//image->allocateImage(noPixels,1,1,GL_RGBA,GL_FLOAT);
			//image->setInternalTextureFormat(GL_RGBA);
			//// fill in the image data. 
			//osg::Vec4* dataPtr = (osg::Vec4*)image->data(); 
			//osg::Vec4 color(1,1,1,1); 
			//*dataPtr = color; 
			//// make fake texture
			//osg::Texture2D* fakeTex = new osg::Texture2D;
			//fakeTex->setWrap(osg::Texture2D::WRAP_S,osg::Texture2D::CLAMP_TO_EDGE);
			//fakeTex->setWrap(osg::Texture2D::WRAP_T,osg::Texture2D::CLAMP_TO_EDGE);
			//fakeTex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
			//fakeTex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
			//fakeTex->setImage(image);
			//// add fake texture
			//mpr_viewSceneSS->setTextureAttribute(mpr_baseTextureUnit,fakeTex,osg::StateAttribute::ON);
			//mpr_viewSceneSS->setTextureMode(mpr_baseTextureUnit,GL_TEXTURE_2D,osg::StateAttribute::ON);
			//mpr_viewSceneSS->setTextureMode(mpr_baseTextureUnit,GL_TEXTURE_3D,osg::StateAttribute::OFF);
		}
		return true;
	}

}