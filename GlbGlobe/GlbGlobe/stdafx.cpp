// stdafx.cpp : source file that includes just the standard includes
// GlbGlobe.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

osg::ref_ptr<osg::EllipsoidModel> g_ellipsmodel = new osg::EllipsoidModel(osg::WGS_84_RADIUS_EQUATOR,osg::WGS_84_RADIUS_POLAR);

//CGlbGlobeObjectFactory* g_objectFactory = new CGlbGlobeObjectFactory;

#include "agg_renderer_scanline.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_scanline_p.h"
#include "agg_pixfmt_rgb.h"
#include "agg_pixfmt_rgba.h"
#include "agg_renderer_base.h"
#include "agg_span_allocator.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_gradient_alpha.h"
#include "agg_span_image_filter_rgb.h"
#include "agg_span_converter.h"
#include "agg_conv_stroke.h"
#include "agg_conv_contour.h"

typedef agg::renderer_base<agg::pixfmt_bgra32> renderer_base_type;
typedef agg::span_allocator<agg::rgba8> span_allocator_type;
typedef agg::span_interpolator_linear<> interpolator_type; //插值器类型
typedef agg::span_image_filter_rgb_bilinear_clip<agg::pixfmt_bgra32, interpolator_type > span_gen_type; // 这个就是Span Generator 
typedef agg::renderer_base<agg::pixfmt_bgra32> renb_type; 

glbBool BlendImage(glbByte* src_img, glbByte* overlay_img, glbInt32 wid, glbInt32 hei, glbInt32 opacity, glbBool rgba_type)
{// 将overlay_img叠加到src_img上
	if (!src_img || !overlay_img || wid<=0 || hei<=0)
		return false;

#if 0
	agg::rendering_buffer rbuf(src_img,wid,hei,-wid * 4);
	//rbuf.attach(pm.buf(), pm.width(), pm.height(), -pm.stride());  
	agg::pixfmt_bgra32 pixf(rbuf);
	renb_type renb(pixf);  

	// Scanline Rasterizer “扫描线光栅化程序”
	agg::rasterizer_scanline_aa<> ras; 
	agg::scanline_u8 sl; 

	// pm_img里的图案作为填充来源 
	agg::rendering_buffer rbuf_img( overlay_img,wid,hei,-wid * 4 ); 
	agg::pixfmt_bgra32 pixf_img(rbuf_img);
	// 线段分配器 

	//分配器类型 
	span_allocator_type span_alloc; // span_allocator 

	// 插值器 
	agg::trans_affine img_mtx; // 变换矩阵 
	interpolator_type ip(img_mtx); // 插值器 

	// 线段生成器 
	span_gen_type span_gen(pixf_img, agg::rgba(1,1,1,1), ip); 

	// 组合成渲染器 
	agg::renderer_scanline_aa< renderer_base_type,span_allocator_type, span_gen_type > my_renderer(renb, span_alloc, span_gen); 

	// 插值器的矩阵变换 
	//img_mtx.scale(1); 
	//img_mtx.translate(0,0); 
	//img_mtx.invert(); //注意这里 会导致图像上下翻转!

	ras.move_to_d(0, 0); 
	ras.line_to_d(0, hei); 
	ras.line_to_d(wid, hei); 
	ras.line_to_d(wid, 0); 
	agg::render_scanlines(ras,sl,my_renderer);
#else
	opacity = min(opacity,100);
	opacity = max(opacity,0);
	
	float alpha = opacity/100.0f;
	for (int i = 0; i < wid*hei; i++)
	{
		float pixel_alpha = overlay_img[i*4+3]/255.0f * alpha;
		src_img[i*4] = overlay_img[i*4]*pixel_alpha + src_img[i*4] * (1-pixel_alpha);
		src_img[i*4+1] = overlay_img[i*4+1]*pixel_alpha + src_img[i*4+1] * (1-pixel_alpha);
		src_img[i*4+2] = overlay_img[i*4+2]*pixel_alpha + src_img[i*4+2] * (1-pixel_alpha);
		src_img[i*4+3] = src_img[i*4+3]; //255
	}
#endif

	return true;
}

#include "iconv.h"
#include <string.h>

//代码转换:从一种编码转为另一种编码  
int code_convert(char* from_charset, char* to_charset, char* inbuf,	int inlen, char* outbuf, int outlen)
{
	iconv_t cd;

	char** pin = &inbuf;  

	char** pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);  

	if(cd == 0)

		return -1;

	memset(outbuf,0,outlen);  

	size_t _inlen = inlen;
	size_t _outlen = outlen;

	//if(iconv(cd,(const char**)pin,(unsigned int /*size_t*/ *)&inlen,pout,(unsigned int /*size_t*/ *)&outlen)
	//	== -1)
	if(iconv(cd,(const char**)pin,&_inlen,pout,&_outlen)
		== -1)

		return -1;  

	iconv_close(cd);

	return 0;  

}

//UNICODE码转为GB2312码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* u2g(char *inbuf)  
{

	int nOutLen = 2 * strlen(inbuf) - 1;
	if(nOutLen<=0)return NULL;

	char* szOut = (char*)malloc(nOutLen);

	if (-1 == code_convert("utf-8","gb2312",inbuf,strlen(inbuf),szOut,nOutLen))

	{

		free(szOut);

		szOut = NULL;

	}

	return szOut;

}  

//GB2312码转为UNICODE码  
//成功则返回一个动态分配的char*变量，需要在使用完毕后手动free，失败返回NULL
char* g2u(char *inbuf)  
{
	int nOutLen = 2 * strlen(inbuf) - 1;
	if(nOutLen<=0)return NULL;

	char* szOut = (char*)malloc(nOutLen);

	if (-1 == code_convert("gb2312","utf-8",inbuf,strlen(inbuf),szOut,nOutLen))

	{

		free(szOut);

		szOut = NULL;

	}

	return szOut;

}  

//获取系统剩余内存
#include <Psapi.h>
glbInt64 GetUnusedMemory()
{
	glbInt64 mem = 0;
#ifdef _WIN64
	MEMORYSTATUS memstatus;
	memset(&memstatus,0,sizeof(MEMORYSTATUS));
	memstatus.dwLength =sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&memstatus);
	mem=memstatus.dwAvailPhys ;	
#else
	PROCESS_MEMORY_COUNTERS pmc;  
    if(GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
	{
		mem =  pmc.WorkingSetSize;//进程已使用内存
		mem += pmc.PagefileUsage; //加上进程使用的虚拟内存
		glbInt64 totalCanUse = 2*1024;
		totalCanUse = totalCanUse*1024*1024;
		mem = totalCanUse - mem;	  //进程可用剩余内存	
	}
#endif
	return mem;
}
/****************全局配置信息*********************/
glbBool   g_isMemCtrl   = false; //默认不再控制内存 2016.11.11
//#ifdef _WIN64
//	glbInt64  g_memThreshold= 3*1024*1024*1024;//3G,内存限额
//#else
//	glbInt64  g_memThreshold= 0.4*1024*1024*1024;//1G,内存限额
//#endif

glbBool   g_isDMemCtrl  = false; //默认不再控制显存
glbInt64  g_totalDMem   = 0.9*1024*1024*1024;//M

glbInt64  g_memThreshold = g_totalDMem;//* 0.5;
/****************全局配置信息*********************/
#include "GlbWString.h"
#include "GlbString.h"
GlbLic  g_srvLic;
CGlbWString g_srvIP  = L"";
glbInt32    g_srvPort= 8888;
GlbLicResult*  srvlicVery()//网络许可认证
{
	if(g_srvIP.length() == 0)
	{
		GlbLicResult* rt = new GlbLicResult();
		rt->code   ="err";
		rt->message="没有设置许可服务";
		return rt;
	}
	if(g_srvLic.isconnected()==false)
	{
		CGlbString srvIP = g_srvIP.ToString();
		g_srvLic.init(srvIP.c_str(),g_srvPort);
		if(g_srvLic.connect()==false)
		{
			GlbLicResult* rt = new GlbLicResult();
			rt->code   ="err";
			rt->message="连接许可服务失败";
			return rt;
		}
	}
	return g_srvLic.verylic("glbglobe");
}
