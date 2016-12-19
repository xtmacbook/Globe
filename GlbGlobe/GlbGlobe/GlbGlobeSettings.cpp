#include "StdAfx.h"
#include "GlbGlobeSettings.h"
#include "GlbPath.h"
#include "GlbReference.h"

#include <sstream>
#include <time.h>

#define d2r(X) osg::DegreesToRadians(X)
#define r2d(X) osg::RadiansToDegrees(X)
#define nrad(X) { while( X > TWO_PI ) X -= TWO_PI; while( X < 0.0 ) X += TWO_PI; }
#define nrad2(X) { while( X <= -osg::PI ) X += TWO_PI; while( X > osg::PI ) X -= TWO_PI; }

static const double TWO_PI = (2.0*osg::PI);
static const double JD2000 = 2451545.0;


double getJulianDate( int year, int month, int date )
{
	if ( month <= 2 )
	{
		month += 12;
		year -= 1;
	}

	int A = int(year/100);
	int B = 2-A+(A/4);
	int C = int(365.25*(year+4716));
	int D = int(30.6001*(month+1));
	return B + C + D + date - 1524.5;
}

class Sun
{
public:
	// https://www.cfa.harvard.edu/~wsoon/JuanRamirez09-d/Chang09-OptimalTiltAngleforSolarCollector.pdf
	static osg::Vec3d getPosition(int year, int month, int date, double hoursUTC )
	{
		double JD = getJulianDate(year, month, date);
		double JD1 = (JD - JD2000);                         // julian time since JD2000 epoch
		double JC = JD1/36525.0;                            // julian century

		double mu = 282.937348 + 0.00004707624*JD1 + 0.0004569*(JC*JC);

		double epsilon = 280.466457 + 0.985647358*JD1 + 0.000304*(JC*JC);

		// orbit eccentricity:
		double E = 0.01670862 - 0.00004204 * JC;

		// mean anomaly of the perihelion
		double M = epsilon - mu;

		// perihelion anomaly:
		double v =
			M + 
			360.0*E*sin(d2r(M))/osg::PI + 
			900.0*(E*E)*sin(d2r(2*M))/4*osg::PI - 
			180.0*(E*E*E)*sin(d2r(M))/4.0*osg::PI;

		// longitude of the sun in ecliptic coordinates:
		double sun_lon = d2r(v - 360.0 + mu); // lambda
		nrad2(sun_lon);

		// angle between the ecliptic plane and the equatorial plane
		double zeta = d2r(23.4392); // zeta

		// latitude of the sun on the ecliptic plane:
		double omega = d2r(0.0);

		// latitude of the sun with respect to the equatorial plane (solar declination):
		double sun_lat = asin( sin(sun_lon)*sin(zeta) );
		nrad2(sun_lat);

		// finally, adjust for the time of day (rotation of the earth)
		double time_r = hoursUTC/24.0; // 0..1
		nrad(sun_lon); // clamp to 0..TWO_PI
		double sun_r = sun_lon/TWO_PI; // convert to 0..1

		// rotational difference between UTC and current time
		double diff_r = sun_r - time_r;
		double diff_lon = TWO_PI * diff_r;

		// apparent sun longitude.
		double app_sun_lon = sun_lon - diff_lon + osg::PI;
		nrad2(app_sun_lon);

#if 0
		OE_INFO
			<< "sun lat = " << r2d(sun_lat) 
			<< ", sun lon = " << r2d(sun_lon)
			<< ", time delta_lon = " << r2d(diff_lon)
			<< ", app sun lon = " << r2d(app_sun_lon)
			<< std::endl;
#endif

		return osg::Vec3d(
			cos(sun_lat) * cos(-app_sun_lon),
			cos(sun_lat) * sin(-app_sun_lon),
			sin(sun_lat) );
	}
};

class Moon
{
public:
	static std::string radiansToHoursMinutesSeconds(double ra)
	{
		while (ra < 0) ra += (osg::PI * 2.0);
		//Get the total number of hours
		double hours = (ra / (osg::PI * 2.0) ) * 24.0;
		double minutes = hours - (int)hours;
		hours -= minutes;
		minutes *= 60.0;
		double seconds = minutes - (int)minutes;
		seconds *= 60.0;
		std::stringstream buf;
		buf << (int)hours << ":" << (int)minutes << ":" << (int)seconds;
		return buf.str();
	}

	static osg::Vec3d getPositionFromRADecl( double ra, double decl, double range )
	{
		return osg::Vec3(0,range,0) * 
			osg::Matrix::rotate( decl, 1, 0, 0 ) * 
			osg::Matrix::rotate( ra - osg::PI_2, 0, 0, 1 );
	}

	// From http://www.stjarnhimlen.se/comp/ppcomp.html
	static osg::Vec3d getPosition(int year, int month, int date, double hoursUTC )
	{
		//double julianDate = getJulianDate( year, month, date );
		//julianDate += hoursUTC /24.0;
		double d = 367*year - 7 * ( year + (month+9)/12 ) / 4 + 275*month/9 + date - 730530;
		d += (hoursUTC / 24.0);                     

		double ecl = osg::DegreesToRadians(23.4393 - 3.563E-7 * d);

		double N = osg::DegreesToRadians(125.1228 - 0.0529538083 * d);
		double i = osg::DegreesToRadians(5.1454);
		double w = osg::DegreesToRadians(318.0634 + 0.1643573223 * d);
		double a = 60.2666;//  (Earth radii)
		double e = 0.054900;
		double M = osg::DegreesToRadians(115.3654 + 13.0649929509 * d);

		double E = M + e*(180.0/osg::PI) * sin(M) * ( 1.0 + e * cos(M) );

		double xv = a * ( cos(E) - e );
		double yv = a * ( sqrt(1.0 - e*e) * sin(E) );

		double v = atan2( yv, xv );
		double r = sqrt( xv*xv + yv*yv );

		//Compute the geocentric (Earth-centered) position of the moon in the ecliptic coordinate system
		double xh = r * ( cos(N) * cos(v+w) - sin(N) * sin(v+w) * cos(i) );
		double yh = r * ( sin(N) * cos(v+w) + cos(N) * sin(v+w) * cos(i) );
		double zh = r * ( sin(v+w) * sin(i) );

		// calculate the ecliptic latitude and longitude here
		double lonEcl = atan2 (yh, xh);
		double latEcl = atan2(zh, sqrt(xh*xh + yh*yh));

		double xg = r * cos(lonEcl) * cos(latEcl);
		double yg = r * sin(lonEcl) * cos(latEcl);
		double zg = r * sin(latEcl);

		double xe = xg;
		double ye = yg * cos(ecl) -zg * sin(ecl);
		double ze = yg * sin(ecl) +zg * cos(ecl);

		double RA    = atan2(ye, xe);
		double Dec = atan2(ze, sqrt(xe*xe + ye*ye));

		//Just use the average distance from the earth            
		double rg = 6378137.0 + 384400000.0;

		// finally, adjust for the time of day (rotation of the earth)
		double time_r = hoursUTC/24.0; // 0..1            
		double moon_r = RA/TWO_PI; // convert to 0..1

		// rotational difference between UTC and current time
		double diff_r = moon_r - time_r;
		double diff_lon = TWO_PI * diff_r;

		RA -= diff_lon;

		nrad2(RA);

		return getPositionFromRADecl( RA, Dec, rg );
	}
};

CGlbGlobeSettings::CGlbGlobeSettings(void)
{
	InitializeCriticalSection(&mpr_critical);	
	HMODULE hmd = GetModuleHandle(L"GlbGlobe.dll");
	CGlbWString exedir = CGlbPath::GetModuleDir(hmd);
	//初始化logofile
	mpr_logoFile = exedir+L"\\res\\logo.jpg";
	//初始化天空纹理
	mpr_skytexFile = exedir+L"\\res\\SkyBoxTexture.jpg";
	//初始化时区
	TIME_ZONE_INFORMATION timezoneinfo;
	GetTimeZoneInformation(&timezoneinfo);
	mpr_timeZone = timezoneinfo.StandardName;	
	//天气
	mpr_weather = 0.0;
	mpr_winddirection=0.0;
	mpr_windstrong=5.0;

	mpr_earthRadius=g_ellipsmodel->getRadiusEquator();
	mpr_innerRadius=mpr_earthRadius;
	mpr_outerRadius = mpr_earthRadius * 1.025f;
	mpr_sunDistance = mpr_earthRadius * 12000.0f;
	mpr_starRadius = 20000.0 * (mpr_sunDistance > 0.0 ? mpr_sunDistance : mpr_outerRadius);
	//mpr_minStarMagnitude=10;
	mpr_minStarMagnitude=-1;
	//UTC
	time_t timer;
	timer = time(NULL);
	tm *t_tm=gmtime(&timer);//localtime(&timer);
	SetDateTime(t_tm->tm_year+1900,t_tm->tm_mon+1,t_tm->tm_mday,t_tm->tm_hour);
}


CGlbGlobeSettings::~CGlbGlobeSettings(void)
{
	DeleteCriticalSection(&mpr_critical);
	std::vector<glbobserver_ptr<IGlbGlobeSettingListen>>::iterator itr = mpr_listens.begin();
	std::vector<glbobserver_ptr<IGlbGlobeSettingListen>>::iterator itrEnd = mpr_listens.end();
	for(itr;itr!=itrEnd;itr++)
	{
		glbref_ptr<IGlbGlobeSettingListen> p;
		(*itr).SafeGet(p);
		if(p.get())
			p->RemoveObserver(this);
	}
	mpr_listens.clear();
}

CGlbWString const &CGlbGlobeSettings::GetLogoFile()const
{
	return mpr_logoFile;
}
void CGlbGlobeSettings::SetLogoFile(const glbWChar* logoFile)
{
	if(mpr_logoFile == logoFile)return;
	if(logoFile == NULL)
		mpr_logoFile = L"";
	else
		mpr_logoFile = logoFile;
	notifyListens(LogoFile_Setting);
}
CGlbWString const &CGlbGlobeSettings::GetSkyTexFile()const
{
	return mpr_skytexFile;
}
void CGlbGlobeSettings::SetSkyTexFile(const glbWChar* skytexFile)
{
	if(mpr_skytexFile == skytexFile)return;
	if(skytexFile == NULL)
		mpr_skytexFile = L"";
	else
		mpr_skytexFile = skytexFile;
	notifyListens(SkyTexFile_Setting);
}
CGlbWString const &CGlbGlobeSettings::GetTimeZone()const
{
	return mpr_timeZone;
}
void CGlbGlobeSettings::SetTimeZone(const glbWChar* timezone)
{
	mpr_timeZone = timezone;
}
//////////////////////////////////////////////////////////////////////////
void CGlbGlobeSettings::SetDateTime( glbInt32 year,glbInt32 month,glbInt32 date,glbDouble houresUTC )
{
	if(mpr_year == year && mpr_month == month
		&& mpr_date == date && mpr_houresUTC == houresUTC)
		return;
	mpr_year=year;mpr_month=month;mpr_date=date;mpr_houresUTC=houresUTC;	
	mpr_sunPos=Sun::getPosition(year,month,date,houresUTC);
	mpr_sunPos.normalize();
	mpr_sunPos*=mpr_sunDistance;

	mpr_moonPos=Moon::getPosition(year,month,date,houresUTC);

	notifyListens(DateTime_Setting);
}

void CGlbGlobeSettings::GetDateTime( glbInt32 &year,glbInt32 &month,glbInt32 &date,glbDouble &houresUTC ) const
{
	year=mpr_year;month=mpr_month;date=mpr_date;houresUTC=mpr_houresUTC;
}

void CGlbGlobeSettings::SetWindDirection( glbDouble ws )
{
	if(ws != mpr_winddirection)
	{
		mpr_winddirection = ws;
		notifyListens(WindDirection_Setting);
	}
}

glbDouble CGlbGlobeSettings::GetWindDirection() const
{
	return mpr_winddirection;
}

void CGlbGlobeSettings::SetWindStrong( glbDouble ws )
{
	if(ws != mpr_windstrong)
	{
		mpr_windstrong = ws;
		notifyListens(WindStrong_Setting);
	}
}

glbDouble CGlbGlobeSettings::GetWindStrong() const
{
	return mpr_windstrong;
}

void CGlbGlobeSettings::SetWeather( glbDouble w )
{
	if(w != mpr_weather)
	{
		mpr_weather = w;
		notifyListens(Weather_Setting);
	}
}

glbDouble CGlbGlobeSettings::GetWeather() const
{
	return mpr_weather;
}

const osg::Vec3d & CGlbGlobeSettings::GetSunPostion() const
{
	return mpr_sunPos;
}

glbDouble CGlbGlobeSettings::GetSunDistance() const
{
	return mpr_sunDistance;
}

const osg::Vec3d & CGlbGlobeSettings::GetMoonPostion() const
{
	return mpr_moonPos;
}

glbDouble CGlbGlobeSettings::GetMoonDistance() const
{
	return mpr_moonPos.length();
}

glbDouble CGlbGlobeSettings::GetInnerAtmosphereRadius() const
{
	return mpr_innerRadius;
}

glbDouble CGlbGlobeSettings::GetOuterAtmosphereRadius() const
{
	return mpr_outerRadius;
}

glbDouble CGlbGlobeSettings::GetStarsRadius() const
{
	return mpr_starRadius;
}

glbDouble CGlbGlobeSettings::GetMinStarMagnitude() const
{
	return mpr_minStarMagnitude;
}

osg::EllipsoidModel * CGlbGlobeSettings::GetEarthEllipsoidModel() const
{
	return g_ellipsmodel;
}

void CGlbGlobeSettings::RegisterListen(IGlbGlobeSettingListen* listen)
{
	if(listen == NULL)return;

	EnterCriticalSection(&mpr_critical);
		listen->AddObserver(this);
		mpr_listens.push_back(listen);
	LeaveCriticalSection(&mpr_critical);
}
void CGlbGlobeSettings::UnRegisterListen(IGlbGlobeSettingListen* listen)
{
	if(listen == NULL)return;
	EnterCriticalSection(&mpr_critical);
		listen->RemoveObserver(this);
		std::vector<glbobserver_ptr<IGlbGlobeSettingListen>>::iterator itr = mpr_listens.begin();
		std::vector<glbobserver_ptr<IGlbGlobeSettingListen>>::iterator itrEnd = mpr_listens.end();
		for(itr;itr!=itrEnd;itr++)
		{
			if( listen == (*itr).get())
			{
				mpr_listens.erase(itr);
				break;
			}
		}
	LeaveCriticalSection(&mpr_critical);
}
void CGlbGlobeSettings::ObjectDeleted(void* listen)
{
	if(listen == NULL)return;
	UnRegisterListen((IGlbGlobeSettingListen*)listen);
}
//void CGlbGlobeSettings::notifyListens(const glbWChar* changedSetting)
void CGlbGlobeSettings::notifyListens(glbInt32 changedSetting)
{
	EnterCriticalSection(&mpr_critical);
		std::vector<glbobserver_ptr<IGlbGlobeSettingListen>>::iterator itr = mpr_listens.begin();
		std::vector<glbobserver_ptr<IGlbGlobeSettingListen>>::iterator itrEnd = mpr_listens.end();
		for(itr;itr!=itrEnd;itr++)
		{
			glbref_ptr<IGlbGlobeSettingListen> p;
			(*itr).SafeGet(p);
			if(p.get())
				p.get()->SettingUpdate(changedSetting);
			
		}
	LeaveCriticalSection(&mpr_critical);
}