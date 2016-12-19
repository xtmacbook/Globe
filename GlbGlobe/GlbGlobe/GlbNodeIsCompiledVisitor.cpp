#include "StdAfx.h"
#include "GlbNodeIsCompiledVisitor.h"
#include <osg/Geometry>

using namespace GlbGlobe;

CGlbNodeIsCompiledVisitor::CGlbNodeIsCompiledVisitor(void) :
	osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
	mpr_isCompiled(true)
{
}


CGlbNodeIsCompiledVisitor::~CGlbNodeIsCompiledVisitor(void)
{
}


void CGlbNodeIsCompiledVisitor::apply(osg::Geode &node)
{			
	for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
	{			
		osg::Drawable* drawable = node.getDrawable(i);
		if (drawable->getUseDisplayList() && drawable->getSupportsDisplayList())
		{// 显示列表
			//for(unsigned int j=0; j<= osg::GraphicsContext::getMaxContextID(); ++j)
			{
				GLuint globj = drawable->getDisplayList(0);
				// call the globj if already set otherwise compile and execute.
				if( globj != 0 )
				{// 显示列表已编译,直接返回
					//mpr_isCompiled = true;					
				}
				else
					mpr_isCompiled = false;
			}
		}
		else if (drawable->getUseVertexBufferObjects() && drawable->getUseVertexBufferObjects())
		{// VBO
			osg::Geometry* _geom = drawable->asGeometry();
			if (_geom)
			{
				osg::Geometry::PrimitiveSetList& primSets = _geom->getPrimitiveSetList();	
				for (osg::Geometry::PrimitiveSetList::iterator itr = primSets.begin(),
					end = primSets.end();
					itr != end;		++itr)
				{
					for(unsigned int j=0; j<= osg::GraphicsContext::getMaxContextID(); ++j)
					{
						osg::GLBufferObject* glBufferObject = (*itr)->getGLBufferObject(j);
						if (glBufferObject && glBufferObject->isDirty()==false)
						{// 已经编译完毕

						}
						else
						{// 当(*itr)->getBufferObject()为空或者(*itr)->dirty()时需要编译
							//mpr_isCompiled = true;
							
						}
					}
				}
			}
		}
	}
	traverse(node);
}

glbBool CGlbNodeIsCompiledVisitor::IsCompiled()
{
	return mpr_isCompiled;
}