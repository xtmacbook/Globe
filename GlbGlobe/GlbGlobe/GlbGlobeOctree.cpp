#include "StdAfx.h"
#include "GlbAxisAlignedBox.h"
#include "GlbGlobeOctree.h"
#include "GlbGlobeRObject.h"
#include <algorithm>

using namespace GlbGlobe;

CGlbGlobeOctree::CGlbGlobeOctree(CGlbGlobeOctree* p) :  mpr_halfsize( 0, 0, 0 )
{
	//initialize all children to null.
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			for ( int k = 0; k < 2; k++ )
			{
				mpr_p_children[ i ][ j ][ k ] = 0;
			}
		}
	}

	mpr_p_parent = p;
	mpr_NumObjects = 0;
}

CGlbGlobeOctree::~CGlbGlobeOctree(void)
{
	//initialize all children to null.
	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			for ( int k = 0; k < 2; k++ )
			{
				if ( mpr_p_children[ i ][ j ][ k ] != 0 )
						delete mpr_p_children[ i ][ j ][ k ];
			}
		}
	}	
	mpr_p_parent = 0;
}

void CGlbGlobeOctree::AddObject(CGlbGlobeObject* obj)
{
	// 不能重复加同一个object
	if ( std::find( mpr_objects.begin(), mpr_objects.end(), obj ) != mpr_objects.end() )
	{// 已经存在此obj,可直接返回
		return;
	}
	// end
	CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(obj);
	if (robj)
	{
		mpr_objects.push_back( obj );
		robj->SetOctree(this);
	}
	//update total counts.
	_ref();
}

void CGlbGlobeOctree::RemoveObject(CGlbGlobeObject* obj)
{
	ObjectList::iterator itr = std::find( mpr_objects.begin(), mpr_objects.end(), obj );
	if (itr == mpr_objects.end())
		return;
	mpr_objects.erase( itr/*std::find( mpr_objects.begin(), mpr_objects.end(), obj )*/ );
	CGlbGlobeRObject* robj = dynamic_cast<CGlbGlobeRObject*>(obj);
	if (robj)
	{		
		robj->SetOctree( 0 );
	}

	//update total counts.
	_unref();
}

bool CGlbGlobeOctree::IsTwiceSize( const CGlbAxisAlignedBox &box ) const
{
	// infinite boxes never fit in a child - always root node
	if (box.IsInfinite())
		return false;

	osg::Vec3 halfMBoxSize = mpr_box.GetHalfSize();
	osg::Vec3 boxSize = box.GetSize();
	return ((boxSize.x() <= halfMBoxSize.x()) && (boxSize.y() <= halfMBoxSize.y()) && (boxSize.z() <= halfMBoxSize.z()));
}

void CGlbGlobeOctree::GetChildIndexes( CGlbAxisAlignedBox& box, long *x, long *y, long *z ) const
{
	osg::Vec3 center = mpr_box.GetCenter();// = mpr_box.GetMaximum().midPoint( mpr_box.GetMinimum() );

	osg::Vec3 ncenter = box.GetCenter(); // box.GetMaximum().midPoint( box.GetMinimum() );

	if ( ncenter.x() > center.x() )
		* x = 1;
	else
		*x = 0;

	if ( ncenter.y() > center.y() )
		* y = 1;
	else
		*y = 0;

	if ( ncenter.z() > center.z() )
		* z = 1;
	else
		*z = 0;

}
void CGlbGlobeOctree::GetBoundBox( CGlbAxisAlignedBox *b )
{
	b -> SetExtents( mpr_box.GetMinimum() - mpr_halfsize, mpr_box.GetMaximum() + mpr_halfsize );
}