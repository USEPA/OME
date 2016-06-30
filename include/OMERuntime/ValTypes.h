#pragma once
#include<limits>
#include<iomanip>

#define ARRAY_TYPE std::deque

//#define OME_SINGLE_PRECISION
#ifdef OME_SINGLE_PRECISION
	#define XMLVALQUERY(x,y,z) x->QueryFloatAttribute(y,z)
	#define XML_SCALAR XML_FLOAT
	#define OME_SCALAR float
	#define OME_SCALAR_NAN nanf("")
	#define OME_SCLR_STR	"float"
	#define OME_SCLR_TAG	"f"
	#define OME_SCLR_READSTR(x)	std::stof(x)
	#define OME_WRITE_PRECISION 6
	
	#define OMEABS(x)	fabs(x)
	#define OMEACOS(x)	std::acosf(x)
	#define OMEASIN(x)	std::asinf(x)
	#define OMEATAN(x)  std::atanf(x)
	#define OMEATAN2(x,y)  std::atan2f(x,y)
	#define OMECOS(x)	std::cosf(x)
	#define OMECOSH(x) std::coshf(x)
	#define OMEEXP(x)	std::expf(x)
	#define OMEFMOD(x,y)	std::fmodf(x,y)
	#define OMESQRT(x)	std::sqrtf(x)
	#define OMEPOW(x,y)	std::powf(x,y)
	#define OMELOG(x)	std::log(x)
	#define OMELOG10(x)	std::log10f(x)
	#define OMESIN(x)	std::sinf(x)
	#define OMESINH(x)	std::sinhf(x)
	#define OMETAN(x) std::tanf(x)
	#define OMETANH(x) std::tanhf(x)

	const OME_SCALAR OME_NOVAL = std::numeric_limits<float>::max();
	const OME_SCALAR OME_SCALAR_MAX = std::numeric_limits<float>::max();
	const OME_SCALAR OME_SCALAR_MIN = std::numeric_limits<float>::min();
	const OME_SCALAR OME_SCALAR_INF = std::numeric_limits<float>::infinity();
#else
	#define XMLVALQUERY(elem,tag,var) elem->QueryDoubleAttribute(tag,var)
	#define XML_SCALAR	XML_DOUBLE
	#define OME_SCALAR double
	#define OME_SCALAR_NAN nan("")
	#define OME_SCLR_STR	"double"
	#define OME_SCLR_TAG	""
	#define OME_SCLR_READSTR(x)	std::stod(x)
	#define OME_WRITE_PRECISION 15

	#define OMEABS(x)	std::abs(x)
	#define OMEACOS(x)	std::acos(x)
	#define OMEASIN(x)	std::asin(x)
	#define OMEATAN(x)  std::atan(x)
	#define OMEATAN2(x,y)  std::atan2(x,y)
	#define OMECOS(x)	std::cos(x)
	#define OMECOSH(x)	std::cosh(x)
	#define OMEEXP(x)	std::exp((long double)x)
	#define OMEFMOD(x,y)	std::fmod(x,y)
	#define OMESQRT(x)	std::sqrt(x)
	#define OMEPOW(x,y)	std::pow(x,y)
	#define OMELOG(x)	std::log(x)
	#define OMELOG10(x)	std::log10(x)
	#define OMESIN(x)	std::sin(x)
	#define OMESINH(x)	std::sinh(x)
	#define OMETAN(x) std::tan(x)
	#define OMETANH(x) std::tanh(x)

	const OME_SCALAR OME_NOVAL = std::numeric_limits<double>::max();
	const OME_SCALAR OME_SCALAR_MAX = std::numeric_limits<double>::max();
	const OME_SCALAR OME_SCALAR_MIN = std::numeric_limits<double>::min();
	const OME_SCALAR OME_SCALAR_INF = std::numeric_limits<double>::infinity();
#endif

