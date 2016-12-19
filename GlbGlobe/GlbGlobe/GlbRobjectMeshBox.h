/********************************************************************
* Copyright (c) 2014 ������ά������Ϣ�������޹�˾
* All rights reserved.
* @file    ClbRobjectMeshBox.h
* @brief   
* @version 1.0
* @author  xt
* @date    2016-08-31 15:30
*********************************************************************/
#pragma once

#ifndef __GLB_MESH_BOX_H__
#define __GLB_MESH_BOX_H__

#include "GlbExtent.h"
#include "GlbGlobeRObject.h"
#include "IGlbGeometry.h"
#include "GlbTin.h"

#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Node>
#include <vector>

namespace GlbGlobe
{
	struct GlbTinSymbolInfo;

	//������Tin����ָ���
	struct  GLB_DLLCLASS_EXPORT GridCell
	{
	public:
		GridCell();

		osg::ref_ptr<osg::Node> node; //�ø����µ�OSG�ڵ�
		CGlbExtent extend;  //���ӵ�����

		osg::ref_ptr <osg::Vec3Array> verticesArrays;
	};

	struct XYIndex
	{
		XYIndex()
		{
			xindex = yindex = zindex = 0;
		}
		XYIndex(unsigned int x,unsigned int y)
		{
			xindex = x;
			yindex = y;
		}
		XYIndex (unsigned int x,unsigned int y,unsigned int z)
		{
			xindex = x;
			yindex = y;
			zindex = z;
		}
		unsigned int xindex;
		unsigned int yindex;
		unsigned int zindex;
	};


	/************************************************************************/
	/* 
		���ӽڵ�ľ����ϵ:
		
			C��������ϵ
			mt ����
			Root----> parentM -----> localCenter ----> node
	 ƽ��:	mt0	 C0	  mt1        C1     mt2      C2     
	 ����:
	*/
	/************************************************************************/

//#define USE_Z_AXIS

#ifdef USE_Z_AXIS
	#define CellPointer GridCell*** 
#else
	#define CellPointer GridCell**
#endif

	class GLB_DLLCLASS_EXPORT ClbRobjectMeshBox
	{
	public:

		ClbRobjectMeshBox();
		/*
			����Box
		*/
		glbBool buildMeshBox(CGlbGlobeRObject * obj,IGlbGeometry *geo);
		
		/************************************************************************/
		/* ����Grids                                                             
		* @parentMatrix :offset scale����
		* @ glbeMode  :�Ƿ�������ģʽ
		/************************************************************************/
		void	  updateGrids(const osg::Matrix parentMatrix,const GlbTinSymbolInfo * tinSmblInfo,
					glbref_ptr<CGlbFeature> feature,GLboolean globeMode);

		/************************************************************************/
		/* �󽻼�                                                              */
		/************************************************************************/

		virtual glbBool intersect(const osg::Vec3d start,const osg::Vec3d end,osg::Vec3d&intersectPoint);

		/************************************************************************/
		/* ��ʱobj�����Ƿ��Ѿ����ָ��� 
		* @ return �Ѿ����ӻ�����true
		/************************************************************************/
		glbBool valid()						{return !_boxs.empty();}

		/************************************************************************/
		/* �������																 */
		/************************************************************************/

		void		clearGrids();

		/************************************************************************/
		/* ���Ը��� �ӵ�������													*/
		/************************************************************************/

		osg::Node * testShowGridNode();


	private:

		/************************************************************************/
		/* ��ȡ���Ӹ��ڵ����
		/************************************************************************/
		osg::Matrix getParentNodeMatrix(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

		/************************************************************************/
		/* ��ȡ���Ӿֲ�ƽ�Ƶ�ƽ�� ����ڶ��������
		/************************************************************************/
		osg::Vec3d  getLocalCenter(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

		/************************************************************************/
		/* ���ӻ�������¼�,�������µ�osg��֯һ���ڵ� 
		*@XcellNum: �ֲ�����ϵX���¸��Ӹ���
		*@YcellNum: �ֲ�����ϵY�� ���Ӹ���
		*@parentM :ƽ�� ���ž���
		*@localCenter:�ֲ��������ĵ�
		/************************************************************************/

		void  buildOver(const osg::Matrix&parentM,const osg::Matrixd&localCenter,
			unsigned int *axisNum, CellPointer grids);
		


		/************************************************************************/
		/* ��ʼ������ 
		  ����cellSize:���Ӵ�С
		 *@axisNum: �ֲ�����ϵXY���¸��Ӹ��� 
		 *@grids : ��ʼ����������
		 *@OBB8Points: ��ʼ�����Ӻ󣬸����ھֲ��ռ���������С�� ������
		/************************************************************************/
		CellPointer  initGrids(GLdouble & cellSize,unsigned int * axisNum,
			std::vector<osg::Vec3d>&OBB8Points);

		/************************************************************************/
		/* 		��ʼ��AABB(�ֲ�����ϵ)	
			��ȡ�ֲ�����ϵ���� �Ѿ��ֲ�����ϵ�µ�AABB
		*@ OBB8Points ��ʼ�����Ӻ󣬸����ھֲ��ռ���������С�� ������
		*@ local: true ����,false :ƽ��
		*@
		/************************************************************************/
		 void  initLocalAABB(CGlbGlobeRObject * obj, IGlbGeometry *  geo,std::vector<osg::Vec3d>&OBB8Points,
			glbBool local = false);
		
		/************************************************************************/
		/*  ��Object�����л�ȡ������Ƭ ����������
		*@geo:����
		*@parentMatrix:ƽ�����ž���
		*@localCenter: �ֲ�ƽ��
		*@cellSize:��ǰ���Ӵ�С
		*@axisNum:��ǰ����X�����
		*@grids :grid����
		/************************************************************************/
		void   getAddTinTriangle(CGlbGlobeRObject * obj,CGlbTin * geo,osg::Vec3d& localCenter,
			GLdouble cellSize,unsigned int* axisNum,
			const osg::Matrixd&parentMatrix,CellPointer grids, 
			std::vector<osg::Vec3d>&OBB8Points);

		
		/************************************************************************/
		/* 	��ȡ������Ƭ���ڸ�������	
		*@ v0 v1 v2 ��������Ƭ������
		*@cellSize:��ǰ���Ӵ�С
		*@XCellNum:��ǰ����X�����
		*@YCellNum����ǰ����Y����� 
		*@OBB8Points: ��ʼ�����Ӻ󣬸����ھֲ��ռ���������С�� ������

		/************************************************************************/
		void   getGridIndex(const osg::Vec3d* v0,const osg::Vec3d* v1,const osg::Vec3d*v2
			,XYIndex & idx,GLdouble cellSize,unsigned int* axisNum,
			std::vector<osg::Vec3d>&OBB8Points);

		void  addTriangleToBox(const osg::Matrix&parentM,const osg::Matrixd&localCenter,const GridCell&cell, CellPointer grids);

		glbBool buildPlaneTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);
		glbBool buildPlaneMultTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

		glbBool buildGlobeTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);
		glbBool buildGlobeMultTinMeshBoxs(CGlbGlobeRObject * obj, IGlbGeometry *  geo);

	private:
		osg::Matrixd _worldToAABBSpace; //�������굽�ֲ�AABB����ϵ�ľ���
		osg::Matrixd         _localMatrix;

		std::vector<GridCell> _boxs; //ƽ�����ŵľֲ�����ϵ��AABB����ϵ�ľ���
		
		GLdouble zCellWidth;
	};
}
#endif
