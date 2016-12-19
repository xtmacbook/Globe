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
		{// ��ʾ�б�
			//for(unsigned int j=0; j<= osg::GraphicsContext::getMaxContextID(); ++j)
			{
				GLuint globj = drawable->getDisplayList(0);
				// call the globj if already set otherwise compile and execute.
				if( globj != 0 )
				{// ��ʾ�б��ѱ���,ֱ�ӷ���
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
						{// �Ѿ��������

						}
						else
						{// ��(*itr)->getBufferObject()Ϊ�ջ���(*itr)->dirty()ʱ��Ҫ����
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