//! \file
//! \brief Main doxygen docs; also defines compiler signature exports.

#ifndef LANDSATLOOK_EXPORT_
#define LANDSATLOOK_EXPORT_

//! \mainpage LandsatLook Documentation
//!
//! \section intro Introduction
//!
//! Lorem ipsum dolor...
//!
//! \section usage A Quick Example
//!
//! \include docs/demo.cpp

#include <string>
#include <map>
#include <vector>
#include <gdal/gdal_priv.h>
//#include <gdal_alg.h>
//#include <gdal/gdal.h>
#include <ConfigFile.h>

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
    #if defined( LandsatLook_EXPORTS )
    #	define LANDSATLOOK_EXPORT __declspec(dllexport)
    #else
    #	define LANDSATLOOK_EXPORT __declspec(dllimport)
    #endif
#else
    #define LANDSATLOOK_EXPORT
#endif

namespace landsatlook {

class MTLParse
{
public:
	MTLParse( std::string &MTLName );
	~MTLParse();
	bool ParseSuccess( )	{ return m_Success; };
	bool FindKey( std::string &key, double &keyVal ) const;
	bool FindKey( std::string &key, int &keyVal ) const;

private:
	ConfigFile *m_Config;
	bool m_Success;
	std::string m_MTLName;

};

class MetaDataOwner
{
public:
	MetaDataOwner( int band) : m_Band( band ) {};
	int getBandNumber()	{ return m_Band; };

	std::map< std::string, double > m_KeyValues;
	int m_Band;
};

class TopOfAtmosphere : public MetaDataOwner
{
public:
	TopOfAtmosphere( const MTLParse *config, int band );
	double ComputeTOAReflectance( double uncorrectedNumber );

private:
	double m_SunElevation;
	double m_Multiplier;
	double m_BaseOffset;
};

class NormalizedIndex
{
public:
	NormalizedIndex( std::map< int, double > &bands, int *reqBands );
	double ComputeIndex( std::map< int, double > &bands );
	double ComputeIndex( double reflectance1, double reflectance2 );

	std::vector< int > m_reqBands;
};

class NDVI : public NormalizedIndex
{
public:
	NDVI( std::map< int, double > &bands );

};

class NDTI : public NormalizedIndex
{
public:
	NDTI( std::map< int, double > &bands );

};

#define INPUT_SIZE unsigned short int
#define INPUT_RASTER_TYPE GDT_UInt16

class ImageBandData
{
public:
	ImageBandData();
	ImageBandData( GDALDataset *gdalData, INPUT_SIZE *oneDataLine, TopOfAtmosphere *toa );
	~ImageBandData();
	void SetGDALData( GDALDataset *gdalData );
	void SetOneDataLine( INPUT_SIZE *oneDataLine ) { m_OneDataLine = oneDataLine; };
	void SetTOA( TopOfAtmosphere *toa ) { m_toa = toa; };
	bool RetrieveOneDataLine( int lineNumber, int startCol, int numCols );
	double TOACorrectOnePixel( int pixelNumber );

	GDALDataset *m_GdalData;
	GDALRasterBand *m_GdalBand;
	INPUT_SIZE *m_OneDataLine;
	TopOfAtmosphere *m_toa;
};

typedef std::map< int, ImageBandData * > ImageBandMap;

class LANDSATLOOK_EXPORT LandsatLook {
public:
	LandsatLook( bool exportNDVI, bool exportNDTI, std::string &landsatPath, std::string &landsatFileRoot,
		double &ULBoundsX, double &ULBoundsY, double &LRBoundsX, double &LRBoundsY );
	void InitGDAL() const;
	GDALDataset* OpenLandsatBand( std::string &landsatBandFileRoot, int bandNumber ) const;
	bool ComputeFarmCellBounds( const MTLParse *config, double &ULX, double &ULY, double &LRX, double &LRY, int &startCol, int &startRow, int &cols, int &rows ) const;
	void SetGeoRefFromLandsat( GDALDataset *targetDataset, ImageBandMap &imageBands, int startCol, int startRow ) const;
	bool Operate();
	bool ExportNDVI( MTLParse *parse );
	bool ExportNDTI( MTLParse *parse );

	bool m_exportNDVI;
	bool m_exportNDTI;
	std::string m_landsatPath;
	std::string m_landsatFileRoot;
	int m_ULCellX;
	int m_ULCellY;
	int m_LRCellX;
	int m_LRCellY;
	double m_ULBoundsX;
	double m_ULBoundsY;
	double m_LRBoundsX;
	double m_LRBoundsY;

};

int LANDSATLOOK_EXPORT function();

}

#endif

