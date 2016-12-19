/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    GlbCalculateNodeSizeVisitor.h
* @brief   �����ͷ�ļ����ĵ�����CGlbCalculateNodeSizeVisitor��
* @version 1.0
* @author  ChenPeng
* @date    2014-5-22 15:00
*********************************************************************/
#pragma once
#include <osg/NodeVisitor>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/Switch>
#include <osgTerrain/Terrain>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include "GlbCommTypes.h"

namespace GlbGlobe
{
	/**
	* @brief  ����node�ڵ㼰�������нڵ���ռ�ڴ��С��
	* @version 1.0
	* @author  ChenPeng
	* @date    2014-5-22 15:00
	* !!!!!!!! ���� ��Сͳ��׼ȷ �ǳ��ؼ���������������
	* ���Բ��� ��ͳ�ƣ�����
	* ��Ҳ���ܶ�ͳ��̫�࣡����
	*/
	class CGlbCalculateNodeSizeVisitor : public osg::NodeVisitor
	{
	public:
		/**
		* @brief ���캯��
		*/
		CGlbCalculateNodeSizeVisitor();
		/**
		* @brief ��������
		*/
		~CGlbCalculateNodeSizeVisitor();
		/**
		* @brief ���ʽڵ����
		*/
		virtual void apply(osg::Node& node)
		{
			if (node.getStateSet())
				apply(*(node.getStateSet()));

			if (node.asSwitch())
			{
				mpr_nodeMemSize+=sizeof(osg::Switch);
			}
			else if (node.asTerrain())
			{
				mpr_nodeMemSize+=sizeof(osgTerrain::Terrain);
			}
			else if (node.asTransform())
			{
				osg::MatrixTransform *mt=dynamic_cast<osg::MatrixTransform *>(&node);
				if (mt!=NULL)
					mpr_nodeMemSize+=sizeof(osg::MatrixTransform);
				else
					mpr_nodeMemSize+=sizeof(osg::Transform);
			}
			else if (node.asCamera())
			{
				mpr_nodeMemSize+=sizeof(osg::Camera);
			}
			else if (node.asGroup())
			{
				mpr_nodeMemSize+=sizeof(osg::Group);
			}
			else if (node.asGeode())
			{
				mpr_nodeMemSize+=sizeof(osg::Geode);
			}			
			else
			{
				osg::Node* _node = dynamic_cast<osg::Node*> (&node);
				if (_node)
					mpr_nodeMemSize+=sizeof(osg::Node);
				else
					printf("error! node type is unknown!\n");
			}

			traverse(node);
		}
		/**
		* @brief ���ʽڵ����
		*/
		virtual void apply(osg::Geode& node)
		{
			if (node.getStateSet())
				apply(*(node.getStateSet()));
			for(unsigned int i=0;i<node.getNumDrawables();++i)
			{
				osg::Drawable* drawable = node.getDrawable(i);
				if (drawable==NULL) continue;
				if (drawable && drawable->getStateSet()) 
					apply(*(drawable->getStateSet()));
				if (drawable->asGeometry())
				{
					osg::Geometry::ArrayList list;
					unsigned int geomSize = drawable->asGeometry()->getGLObjectSizeHint();
					mpr_nodeMemSize+=geomSize;
					//drawable->asGeometry()->getArrayList(list);
					//for (int il = 0;il<(long)list.size();il++)
					//{
					//	osg::Array* array = list[il];						
					//	//mpr_nodeMemSize+=array->getDataSize()*array->getNumElements();// �������������ͱ�����ռ���ֽ���
					//	mpr_nodeMemSize+=array->getTotalDataSize();
					//}
				}
				else
				{// ��osg::geometry��drawable �ض��� osg::DrawPixels��� osg::ShapeDrawable��

				}
			};
			mpr_nodeMemSize+=sizeof(osg::Geode);
			traverse(node);
		}
		/**
		* @brief ���ʽڵ����
		*/
		virtual void apply(osg::Billboard& node)
		{
			if (node.getStateSet())
				apply(*(node.getStateSet()));
			for(unsigned int i=0;i<node.getNumDrawables();++i)
			{
				osg::Drawable* drawable = node.getDrawable(i);
				if (drawable && drawable->getStateSet()) 
					apply(*(drawable->getStateSet()));
				if (drawable->asGeometry())
				{
					osg::Geometry::ArrayList list;
					drawable->asGeometry()->getArrayList(list);
					for (int il = 0;il<(long)list.size();il++)
					{
						osg::Array* array = list[il];
						//mpr_nodeMemSize+=array->getDataSize()*array->getNumElements();
						mpr_nodeMemSize+=array->getTotalDataSize();
					}					
				}
			}
			mpr_nodeMemSize+=sizeof(osg::Billboard);
			traverse(node);
		}
		/**
		* @brief ���ʽڵ����
		*/
		virtual void apply(osg::StateSet& stateset)
		{
			// search for the existence of any texture object attributes
			for(unsigned int i=0;i<stateset.getTextureAttributeList().size();++i)
			{
				osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i,osg::StateAttribute::TEXTURE));
				if (texture)
				{
					mpr_textureSet.insert(texture);
				}
			}
		}
		/**
		* @brief ��ȡ�ڵ������ڴ�ռ����
		*/
		glbInt32 getTextureSize()
		{
			/*
			* �������drawable������һ������Ļ���ͳ�Ƶ�����ռ���ڴ������ʵ��ռ������
			*/
			glbInt32 size = 0;
			for(TextureSet::iterator itr = mpr_textureSet.begin();
				itr != mpr_textureSet.end();
				++itr)
			{
				osg::Texture* texture = const_cast<osg::Texture*>(itr->get());

				osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(texture);
				osg::Texture3D* texture3D = dynamic_cast<osg::Texture3D*>(texture);

				size+=texture2D ? sizeof(osg::Texture2D) : sizeof(osg::Texture3D);

				osg::ref_ptr<osg::Image> image = texture2D ? texture2D->getImage() : (texture3D ? texture3D->getImage() : 0);
				if (image.valid())
				{						
					GLenum format = image->getPixelFormat();
					int _compress = 1;
					switch(format)
					{
					case(GL_COMPRESSED_RGB_S3TC_DXT1_EXT):
						_compress = 4;
						break;
					case(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT):
						_compress = 4;
						break;
					case(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT):
						_compress = 8;
						break;
					case(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT): 
						_compress = 8;
						break;
					default: break;
					}
					//size+=image->getTotalSizeInBytesIncludingMipmaps();
					size+=image->getImageSizeInBytes()+image->getTotalSizeInBytesIncludingMipmaps();
					size+=sizeof(osg::Image);
				}
			}
			return size;
		}
		/**
		* @brief ��ȡ�ڵ��ڴ�ռ����
		*/
		glbInt32 getNodeMemSize()
		{
			return mpr_nodeMemSize;
		}

	private:
		typedef std::set<osg::ref_ptr<osg::Texture>> TextureSet;
		TextureSet mpr_textureSet;
		glbInt32 mpr_nodeMemSize;
	};
}