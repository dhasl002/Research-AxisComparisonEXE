/*		MRC CLASS
 *
 *		Kamal Al Nasr, Augest 2010  -- modified by Dong Si, fall 2010
 *		kal@cs.odu.edu
 *
 *		Old Dominion University
 *		Department of Computer Science
 *		Engineering & Computational Sciences Bldg,
 *		4700 Elkhorn Ave, Suite 3300, Norfolk, VA
 *
 *
 *
 *
 */

#ifndef MRC_H
#define MRC_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <math.h>
#include <limits.h>
#include "protein.h"
#include "constants.h"
#include "geometry.h"
#include "jacobi.h"
#include "skeleton_overall.h"

using namespace std;

/*
 *		This is an improtant aspect....you should change this according to the mode of the map file u r working on
 *		the default is 2 (float) but u can change it to any type
 */
typedef float vxlDataType;

/*
 *	The Maximum Size of the grid (map) we handle
 */
#define MAXLEN	2000



//data structure for deleting the small voxel groups in map
struct Position
{
    int x;
    int y;
    int z;

    //initializer
    Position() : x(0), y(0), z(0) {}
};
struct Node
{
    Position pos;
    float density;
    bool traveled;
    bool isHlxEnd;

    //initializer
    Node() : density(0.0), traveled(false), isHlxEnd(false){}
};




//local peaks counter in the density map
//for each cell store the coordinate and its local peak counter
struct peakCell{
	float peak;
	Coordinate pnt;
};
// Determine priority.
bool operator<(const peakCell &a, const peakCell &b)
{
	return a.peak  < b.peak ;
}



/*
 *		MRC HEADER SRTUCTURE
 *		source http://www2.mrc-lmb.cam.ac.uk/image2000.html
 */

/*
Map/Image Header Format

Length = 1024 bytes, organized as 56 LONG words followed
by space for 10 80 byte text labels.

1		NX number of columns (fastest changing in map)
2		NY number of rows
3		NZ number of sections (slowest changing in map)
4		MODE data type :	0 image : signed 8-bit bytes range -128 to 127
							1 image : 16-bit halfwords
							2 image : 32-bit reals
							3 transform : complex 16-bit integers
							4 transform : complex 32-bit reals

5		NXSTART number of first column in map (Default = 0)
6		NYSTART number of first row in map
7		NZSTART number of first section in map
8		MX number of intervals along X
9		MY number of intervals along Y
10		MZ number of intervals along Z
11-13	CELLA cell dimensions in angstroms
14-16	CELLB cell angles in degrees
17		MAPC axis corresp to cols (1,2,3 for X,Y,Z)
18		MAPR axis corresp to rows (1,2,3 for X,Y,Z)
19		MAPS axis corresp to sections (1,2,3 for X,Y,Z)
20		DMIN minimum density value
21		DMAX maximum density value
22		DMEAN mean density value
23		ISPG space group number 0 or 1 (default=0)
24		NSYMBT number of bytes used for symmetry data (0 or 80)
25-49	EXTRA extra space used for anything - 0 by default
50-52	ORIGIN origin in X,Y,Z used for transforms
53		MAP character string 'MAP ' to identify file type
54		MACHST machine stamp
55		RMS rms deviation of map from mean density
56		NLABL number of labels being used
57-256	LABEL(20,10) 10 80-character text labels
*/
struct MRC_HEADER {


  int    nx;            /* # of columns ( fastest changing in the map    */
  int    ny;            /* # of rows                                     */
  int    nz;            /* # of sections (slowest changing in the map    */

  int    mode;          /* data type
                              0 = image data in bytes
                              1 = image data in short integer
                              2 = image data in floats
                              3 = complex data in complex short integers
                              4 = complex data in complex reals          */

  int    nxstart;       /* number of first column in map (default = 0)   */
  int    nystart;       /* number of first row in map (default = 0)      */
  int    nzstart;       /* number of first ssection in map (default = 0) */

  int    mx;            /* number of intervals along X                   */
  int    my;            /* number of intervals along Y                   */
  int    mz;            /* number of intervals along Z                   */

  float  xlength;       /* cell dimensions in X (angstrom)               */
  float  ylength;       /* cell dimensions in Y (angstrom)               */
  float  zlength;       /* cell dimensions in Z (angstrom)               */

  float  alpha;         /* cell angles between Y and Z                   */
  float  beta;          /* cell angles between X and Z                   */
  float  gamma;         /* cell angles between X and Y                   */

  int    mapc;          /* number of axis corresponding to columns (X)   */
  int    mapr;          /* number of axis corresponding to rows (Y)      */
  int    maps;          /* number of axis corresponding to sections (Z)  */

  float  amin;          /* minimum density value                         */
  float  amax;          /* maximum density value                         */
  float  amean;         /* mean density value                            */

  int    ispg;          /* space group number (0 for images)             */
  int    nsymbt;        /* # of bytes for symmetry operators             */

  int    extra[25];     /* user defined storage space                    */

  float  xorigin;       /* X phase origin                                */
  float  yorigin;       /* Y phase origin                                */
  float	 zorigin;		/* Z phase origin								 */

  char map[4];			/* constant string "MAP "						*/

  int machineStamp;		/* machine stamp in ccp4 convention: big endian: 0x11110000, little endian 0x44440000 */

  float rms;			/* rms deviation of map from mean density		*/


  int    nlabl;         /* # of labels being used in the MRC header      */

  char   label[10][80]; /* actual text labels                            */

/*
 * NOTE: In some cases what follows the MRC header are symmetry records stored as
 *       text as in the International Tables operators. These are separated by
 *       a * and grouped into 'lines' of 80 characters.
 */

  //initializer
  MRC_HEADER() : nx(0), ny(0), nz(0), mode(2), nxstart(0), nystart(0), nzstart(0), mx(0), my(0), mz(0), xlength(0), \
				ylength(0), zlength(0), alpha(0), beta(0), gamma(0), mapc(0), mapr(0), maps(0), amin(0), amax(0),	\
				amean(0), ispg(0), nsymbt(0), xorigin(0.0), yorigin(0.0), zorigin(0.0), nlabl(0) {}

};

/*
 *		END of MRC HEADER STRUCTURE
 */

struct Gradient
{
  float dx;
  float dy;
  float dz;  //
  float da;  //

  //initializer
  Gradient() : dx(0.0), dy(0.0), dz(0.0), da(0.0) {}
};

struct Tensor
{
  vector<vector<float> > Hmatrix;      //Hessian matrix
  vector<float> Evalue;                //Eigenvalue
  vector<vector<float> > Evector;      //Eigenvector
};

struct Thickness
{
  float t1;
  float t2;
  float t3;

  //initializer
  Thickness() : t1(0.0), t2(0.0), t3(0.0) {}
};

/*
 *		DENSITY MAP : CLASS Definitioan
 */

class Map
{
public:

	MRC_HEADER	hdr;

	/*
	 *		actual data : The Body Of The Map
	 *
	 *		the cube where we store actual values of voxels
	 *		The occurence of the points on the cube
	 *		nx			the fastest point changes	....	but this would be correspondant to x coordinate from pdb
	 *		ny																  correspondant to y coordinate from pdb
	 *		nz			the slice...the slowest point changes				  correspondant to z coordinate from pdb
	 *
	 *					 nx	ny nz	 nx ny nz	 nx ny nz		nx ny nz	nx ny nz	 nx ny nz	 nx ny nz	nx ny nz	ny ny nz
	 *		i.e			[0, 0, 0]	[1, 0, 0]	[2, 0, 0].......[0, 1, 0]	[1, 1, 0]	[2, 1, 0]....[0, 0, 1] [1, 0, 1]	[2, 0, 1]
	 *
	 *		The map is structured in a way the first slice (nz) written first then the second slice....
	 *		the size of the cube will be nx * ny * nz (rows X cols X depth)
	 *		nx : Rows
	 *		ny : Cols
	 *		nz : Depths (slices)
	 *
	 */
    float apixX;								//Angstrom per pixel ratio for X direction
	float apixY;
	float apixZ;
	vector<vector<vector<vxlDataType> > >	cube;   // voxel density value
	vector<vector<vector<Gradient> > >	 grad;      // voxel gradient
	vector<vector<vector<Tensor> > >   tens;        // voxel tenser
	vector<vector<vector<Thickness> > >   thick;    // voxel thickness
	vector<vector<vector<float> > >   dt;           // Distance Transform
	vector<vector<vector<float> > >   dr;           // DT value of the Distance Ridge / Medial Axis
    //vector<vector<vector<float> > >   localThick;   // local thickness derived from DT and DR

    vector<vector<vector<Node> > >	node;   //vector for all the voxels in map, for filerting small groups


    void setApix();								//set Apix values
	void read(string);							//read the density map ... given the name of the density map.
	void write(string);							//write back the density on a given file
	void printInfo();                           //print mapp information
	void createCube(short, short, short);		//create the grid of the size by given dimensions	(rows, cols, slices)
	void filterize(float);						//filterize the map using a threshold
	short numRows();							//returns number of rows in grid3D
	short numCols();							//returns number of cols in grid3D
	short numSlcs();							//returns number of slices in grid3D (depth)
	void cleanVxls(vector<vector<Coordinate> >, float);	//given a set of sticks and a radius, clean the voxels around each stick within the given radius (in A)
	void traceDensity(Coordinate, Coordinate, short, vector<Coordinate>&);	//given two indeces in the Cryo-EM map and the length of the sequence (in terms of #AA) is expected between them
																			//it saves the trace points into a vector, if the sequence does not fit b/w the two points...
	void localPeaks(Coordinate, Coordinate, short, vector<Coordinate>&, float);			//find local Peaks b/w two points by applying a sphere around each voxel and calculate the average of density
																						//for each voxel inside that sphere has a density larger than the average will be (its counter) by 1
	void localPeaksMap(vector<Coordinate>&, vector<vector<Coordinate> >, float, float);		//find local peaks for he entire map


    /////////////////////////////////////////////--- added by Dong/////////////////////////////////////////////////
    void gauss_smooth();
    void buildGradient(int fast);               //build the gradient for each voxel
    void buildTensor();
    void buildThickness(float threshold);
    void normalize();
    void update_hdrInfo();                      //update the header info after modify the density map (ex. gauss)

    void EDT();                                 //Saito-Toriwaki algorithm for Euclidian Distance Transformation.
    void DR();                                  //detect the distance ridge/medial axis from the distance map
    vector<vector<int> > createTemplate(vector<int> distSqValues);          // Build template --- sub-function of DR()
    vector<int> scanCube(int dx, int dy, int dz, vector<int> distSqValues);   // scan Cube --- sub-function of DR();
    float LocalThickness(int x, int y, int z);                   //local thickness derived from DT and DR for a centain voxel

    void LocalPeakFilter(int divider);  //local peak filter for selecting backbone voxels. filter the voxels has LPC less than MAX_LPC/divider

    float dist(Node p1, Node p2); //calculate the distance btw to nodes based on voxel
    void AddNearbyNodes(Node d, vector<Node> & group);     //add nearby voxel into group
    void deleteSmallVxlGroup(float minLength, float minSize);  //delete small voxel group that has length smaller than minLength

    void Output_HLX(string pdbID, vector<Coordinate> curve_pnts);  //output helix stick files
    int findClosestNodeInHLX(vector<Node> Cluster, Node End);  //find closest node to the End node in a HLX node cluster

    void Output_SHT(string pdbID, vector<Coordinate> & curve_pnts);


private:
	short slcLen;		//how many cell in each slice
	short sizeOfVxl;	//the size of the voxel,	depends on (mode) if bytes, short, floats,....
};
/*
 *		DENSITY MAP : END of CLASS Definitioan
 */




/*
 *		DENSITY MAP : CLASS Implementation
 */
void Map::read (string mrcFname)
{
	ifstream inMapF;	//map file
	int iRow,
		iCol,
		iDepth;				//regular counters

	//open given mrc file
	inMapF.open (mrcFname.c_str (), ios::binary);

	if (!inMapF.is_open ()){
		cout<<"============================== in MRC::read (string) =========================="<<endl;
		cout<<"Can't open given Map file ( "<<mrcFname<<" ). "<<endl;
		cout<<"==============================================================================="<<endl;
		exit(1);
	}
	/*
	 *		Read Map Header
	 */
	inMapF.read ((char*)(&hdr), sizeof(MRC_HEADER));

    if ( hdr.nx <= 0 || hdr.nx >= MAXLEN ||
		hdr.ny <= 0 || hdr.ny >= MAXLEN ||
		hdr.nz <= 0 || hdr.nz >= MAXLEN )
	{
		cout<<"============================== in MRC::read (string) =========================="<<endl;
		cout<<"Strange header of the file. One of (nx,ny,nz) exceeds MAXLEN ( "<<MAXLEN<<" )."<<endl;
		cout<<"==============================================================================="<<endl;
		exit(1);
	}

	/*
	 *		Set Apix ratios
	 */
	setApix();

	createCube(hdr.nx, hdr.ny , hdr.nz);		//create the grid (rows , Cols, Depth) --> nx X ny X nz

	/*
	 *		Read map data (Voxels)
	 */

	for (iDepth=0; iDepth<hdr.nz; iDepth++)
		for (iCol=0; iCol<hdr.ny; iCol++)
			for (iRow=0; iRow<hdr.nx; iRow++)
				// Read one cell at a time
				inMapF.read ((char *) &cube[iRow][iCol][iDepth], sizeOfVxl);
}
/////////////////////////////////////////////////////////////////////////////////
void Map::write (string outFileName)
{

	ofstream outMapF;

	//open out file
	outMapF.open (outFileName.c_str (), ios::binary);

	if (!outMapF.is_open ()){
		cout<<"============================= in MRC::write (string) =========================="<<endl;
		cout<<"Can't open given Map file ( "<<outFileName<<" ). "<<endl;
		cout<<"==============================================================================="<<endl;
		exit(1);
	}
	/*
	 *		write the header first
	 */
	outMapF.write ((char *) &hdr, sizeof(MRC_HEADER));

	/*
	 *		write the grid
	 */
	int iRow,
		iCol,
		iDepth;		//regular counters

	for (iDepth = 0; iDepth <numSlcs(); iDepth++)
		for (iCol=0; iCol<numCols(); iCol++)
			for (iRow=0; iRow<numRows(); iRow++)
				//write cell at a time
				outMapF.write ((char *) &cube[iRow][iCol][iDepth], sizeOfVxl);

	outMapF.close();
}
///////////////////////////////////////////////////////////////////////////////////
void Map::printInfo()
{
	cout<<endl<<"		=================== Cryo-EM Info ======================="<<endl;
    cout<<"		n? : "<<hdr.nx<<" "<<hdr.ny<<" "<<hdr.nz<<endl;
    cout<<"		n?start : "<<hdr.nxstart<<" "<<hdr.nystart<<" "<<hdr.nzstart<<endl;
    cout<<"		# intervals : "<<hdr.mx<<" "<<hdr.my<<" "<<hdr.mz<<endl;
    cout<<"		length : "<<hdr.xlength<<" "<<hdr.ylength<<" "<<hdr.zlength<<endl;
    cout<<"		map? : "<<hdr.mapc<<" "<<hdr.mapr<<" "<<hdr.maps<<endl;
    cout<<"		origin : "<<hdr.xorigin<<" "<<hdr.yorigin<<" "<<hdr.zorigin<<endl;
    cout<<"		apixX: "<<apixX<<" , apixY: "<<apixY<<" , apixZ: "<<apixZ<<endl;
    cout<<"		========================================================="<<endl<<endl;
}
////////////////////////////////////////////////////////////////////////////////////
void Map::setApix ()
{
	apixX = hdr.xlength / hdr.mx;
	apixY = hdr.ylength / hdr.my;
	apixZ = hdr.zlength / hdr.mz;
}
///////////////////////////////////////////////////////////////////////////////////
void Map::createCube(short nX, short nY, short nZ)
{

	// set slice length
	slcLen = nX * nY;

	cube.resize(nX);			//resize rows according to nx

	for(int iRow=0; iRow<nX; iRow++){
		cube[iRow].resize(nY);		// resize cols according to ny ..  generate ny cols for each row

		for (int iCol=0; iCol<nY; iCol++)
			cube[iRow][iCol].resize(nZ);	//resize depth according to nz .. generate nz slices for each cell
	}

	//set the size of voxel (the size of data type used)
	sizeOfVxl = sizeof(cube[0][0][0]);
}
////////////////////////////////////////////////////////////////////////////////////////
// Delete densities around a stick within a given radius
// This function assumes that the Cryo-EM map and the list of points sent are aligned
// and no translation is needed
void Map::cleanVxls(vector<vector<Coordinate> > ssEdges, float radius)
{
    setApix();

	int i, j, irow,icol,islc;

	//re-scale xyz coordinate system to match Cryo-EM map
	for (i=0; i<ssEdges.size (); i++){
		for (j=0; j<ssEdges[i].size (); j++){
			ssEdges[i][j].x /= apixX;
			ssEdges[i][j].y /= apixY;
			ssEdges[i][j].z /= apixZ;
		}
	}
	//calculate new origin indx
	float originX = -hdr.xorigin/apixX;
	float originY = -hdr.yorigin/apixY;
	float originZ = -hdr.zorigin/apixZ;

	//convert radius in Angstrom to number of cells
	radius = radius/apixX + 0.5;

	cout<<"Deleting density around Sticks...";
	Coordinate nPnt, mapP;
	Coordinate sgmntStart, sgmntEnd;			//start and end points of the stick converted into map indeces
	Coordinate stkStart, stkEnd;				//start and end of the stick converted into map indeces

	short startIndx=-1, endIndx=-1;

	cout<<"Radius= "<<radius-0.5<<" in PDB= "<<(radius-0.5) * apixX<<endl;
	for (i=0; i<ssEdges.size (); i++){

		//initiate indeces
		startIndx = -1;
		endIndx = -1;

		vector<Coordinate> stkSegments;			//stkSegments to check density against

		//find two points close to ends where to remove density in  b/w...this would protect density around the end of SS directly

		for (j=1; j<ssEdges[i].size (); j++){
			if (getDistance(ssEdges[i][0], ssEdges[i][j]) > radius-1){
				startIndx = j;
				break;
			}
		}
		for (j=ssEdges[i].size ()-2; j>-1; j--){
			if (getDistance(ssEdges[i][ssEdges[i].size()-1], ssEdges[i][j]) > radius-1){
				endIndx = j;
				break;
			}
		}

		//for short SS....dont make allowance region
		if (getDistance(ssEdges[i][0], ssEdges[i][ssEdges[i].size ()-1]) < 3 * radius){
			startIndx = 0;
			endIndx = ssEdges[i].size ()-1;
		}

		stkStart.x = ssEdges[i][startIndx].x + originX;
		stkStart.y = ssEdges[i][startIndx].y + originY;
		stkStart.z = ssEdges[i][startIndx].z + originZ;
		stkEnd.x = ssEdges[i][endIndx].x + originX;
		stkEnd.y = ssEdges[i][endIndx].y + originY;
		stkEnd.z = ssEdges[i][endIndx].z + originZ;

		/*
		//first check if this stick is close to another stick....then reduce the radius
		Coordinate p1S1, p2S1, p1S2, p2S2;		//2 segments points
		for (j=0; j<ssEdges.size (); j++){
			if (j!=i){
				//find the shortest deistance b/w two line segments (represented by the sticks
				for (int k=0; k<ssEdges[i].size ()-1; k++){
						p1S1.x = ssEdges[i][k].x;
						p1S1.y = ssEdges[i][k].y;
						p1S1.z = ssEdges[i][k].z;
						p2S1.x = ssEdges[i][k+1].x;
						p2S1.y = ssEdges[i][k+1].y;
						p2S1.z = ssEdges[i][k+1].z;
						for (int l=0; l<ssEdges[j].size ()-1;l++){
							p1S2.x = ssEdges[j][l].x;
							p1S2.y = ssEdges[j][l].y;
							p1S2.z = ssEdges[j][l].z;
							p2S2.x = ssEdges[j][l+1].x;
							p2S2.y = ssEdges[j][l+1].y;
							p2S2.z = ssEdges[j][l+1].z;

							float shortestDist = getDistLines(p1S1, p2S1, p1S2, p2S2)/2;
							if (shortestDist<newR){
								newR = shortestDist;
								cout<<"Stick# "<<i+1<<"  and stick# "<<j+1<<" are close. radius will be changed to "<<newR*apixX<<endl;
							}
						}
				}
			}
		}
		*/


		//work only on some segments on the stick
		stkSegments.push_back (ssEdges[i][startIndx]);			//first point
		if (endIndx-startIndx>3){
			short nSegments = (endIndx-startIndx)/4;
			stkSegments.push_back(ssEdges[i][nSegments]);
			stkSegments.push_back (ssEdges[i][2*nSegments]);
			if (3*nSegments < endIndx){
				stkSegments.push_back (ssEdges[i][3*nSegments]);
				if (4*nSegments < endIndx)
					stkSegments.push_back (ssEdges[i][4*nSegments]);
			}
		}
		stkSegments.push_back (ssEdges[i][endIndx]);			//last point

		//cout<<endl<<endl<<"STK "<<i+1<<endl;
		//cout<<"Number of segments = "<<stkSegments.size ()<<endl;
		//cout<<"StartIndx= "<<startIndx<<"  EndIndx="<< endIndx<<endl;
		for (irow=0; irow<numRows(); irow++){
			mapP.x = irow;
			for (icol=0; icol<numCols(); icol++){
				mapP.y = icol;
				for (islc=0; islc<numSlcs(); islc++){
					mapP.z = islc;
					if (cube[irow][icol][islc] > 0){
						bool closeToOther = false;
						for (j=0; j<stkSegments.size ()-1; j++){
							//calculate indeces for the map
							sgmntStart.x = stkSegments[j].x + originX;
							sgmntStart.y = stkSegments[j].y + originY;
							sgmntStart.z = stkSegments[j].z + originZ;
							sgmntEnd.x = stkSegments[j+1].x + originX;
							sgmntEnd.y = stkSegments[j+1].y + originY;
							sgmntEnd.z = stkSegments[j+1].z + originZ;

							//remove local peaks within 3 Angstrom from the axis....exclude those peaks at both ends of the axis
							//if the projection of the local points inside the axis then it is not after one of the two ends
							//float proj2 = linePointIntersection(stkStart, stkEnd, mapP, nPnt);

							//float proj = linePointIntersection(sgmntStart, sgmntEnd, mapP, nPnt);

							if (getDistLineSegPoint(sgmntStart, sgmntEnd, mapP) < radius){
							//if (getDistance(mapP, nPnt) < radius &&
								//(proj2 > 0 && proj2 < 1)){
								//(proj > 0 && proj < 1)){

								//cout<<"stkSegment : "<<stkSegments[j].x*apixX<<" "<<stkSegments[j].y*apixY<<" "<<stkSegments[j].z*apixZ<<" & "<<stkSegments[j+1].x*apixX<<" "<<stkSegments[j+1].y*apixY<<" "<<stkSegments[j+1].z*apixZ<<endl;
								//cout<<"point : "<<irow<<" "<<icol<<" "<<islc<<" distance= "<<getDistance(mapP, nPnt)<<" proj= "<<proj<<endl;
								for (int k=0; k<ssEdges.size (); k++){
									if (k != i){
										for (int l=0; l<ssEdges[k].size (); l++){

											//point on other SS
											sgmntStart.x = ssEdges[k][l].x + originX;
											sgmntStart.y = ssEdges[k][l].y + originY;
											sgmntStart.z = ssEdges[k][l].z + originZ;

											if (getDistance(mapP, sgmntStart) < radius){
												closeToOther = true;
												break;
											}
										}
									}
									if (closeToOther)
										break;

								}

								//cout<<"  ["<<irow<<","<<icol<<","<<islc<<"] dist = "<<getDistance(mapP, nPnt)<<endl;
								if (!closeToOther){
									cube[irow][icol][islc] = 0;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	cout<<"  Done."<<endl;
	getchar();getchar();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::traceDensity (Coordinate sIndx, Coordinate eIndx, short nAA, vector<Coordinate> &pnts)
{
    setApix();

	float	straightDist = getDistance(sIndx, eIndx),
			seqDist = nAA * 3.8;

	if (seqDist - straightDist < 0)
		return;

	/*
	 *		find local maximum for all 26-neighborhood
	 */

	short ix, iy, iz, i, j, k;
	Coordinate pnt;
	for (ix=1; ix<numRows()-1; ix+=3) {
		for (iy=1; iy<numCols()-1; iy+=3) {
			for (iz=1; iz<numSlcs()-1; iz+=3) {

				pnt.x = ix;
				pnt.y = iy;
				pnt.z = iz;

				if (getDistance(sIndx, pnt)*apixX + getDistance(pnt, eIndx)*apixX < seqDist){
					float localMax = -9999.0;

					for (i=-1; i<=1; i++) {
						for (j=-1; j<=1; j++) {
							for (k=-1; k<=1; k++) {
								if (cube[ix+i][iy+j][iz+k] > 0){
									if (cube[ix+i][iy+j][iz+k]>localMax){
										localMax = cube[ix+i][iy+j][iz+k];
										pnt.x = (ix+i)*apixX + hdr.xorigin;
										pnt.y = (iy+j)*apixY + hdr.yorigin;
										pnt.z = (iz+k)*apixZ + hdr.zorigin;
									}
								}
							}
						}
					}
					if (localMax>-9999.0){
						pnts.push_back (pnt);
					}
				}
			}
		}
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//given point is in MAP indexing system not in XYZ
void Map::localPeaks(Coordinate sIndx, Coordinate eIndx, short nAA, vector<Coordinate>& peaks, float prcntg)
{
    setApix();

	float	straightDist = getDistance(sIndx, eIndx),
			seqDist = nAA * 3.8;

	if (seqDist - straightDist*apixX < 0)
		return;

	float sphereR = 3*apixX;							//sphere radius in Angstrom
	short sphereVxl = (short) (sphereR + 1.5*apixX);	//a variable used to estimate number of vxl to check around target voxel to form a asphere
	short ix, iy, iz, i, j, k;

	//define and initiate local counters
	short ***lCntrs;
	lCntrs = new short **[numRows()];
	for (i=0; i<numRows(); i++)
		lCntrs[i] = new short *[numCols()];
	for (i=0; i<numRows(); i++)
		for (j=0; j<numCols(); j++){
			lCntrs[i][j] = new short[numSlcs()];

			for (k=0; k<numSlcs(); k++)
				lCntrs[i][j][k] = 0;
		}

	/*
	 *		find local maximum for all voxels.....
	 *		for each voxel calculate the average density on the sphere around it...and then for each voxel inside the sphere larger than
	 *		avg density increment its local counter by 1
	 *		then sort all local peak counter and pick up top percentage
	 */
	Coordinate pnt;
	for (ix=0; ix<numRows(); ix++) {
		pnt.x = ix;
		for (iy=0; iy<numCols(); iy++) {
			pnt.y = iy;
			for (iz=0; iz<numSlcs(); iz++) {
				pnt.z = iz;
				if (getDistance(sIndx, pnt)*apixX + getDistance(pnt, eIndx)*apixX < seqDist){		//check if this point is accessible for the portion

					//calculate average density in the sphere
					float avgD=0;
					short cntr=1;		//points included counter

					Coordinate pnt_;

					for (i=-sphereVxl; i<=sphereVxl; i++) {
						if (ix+i>-1 && ix+i<numRows()){
							pnt_.x = ix+i;
							for (j=-sphereVxl; j<=sphereVxl; j++) {
								if (iy+j>-1 && iy+j<numCols()){
									pnt_.y = iy+j;
									for (k=-sphereVxl; k<=sphereVxl; k++) {
										if (iz+k>-1 && iz+k<numSlcs()){
											pnt_.z = iz+k;
											if (cube[ix+i][iy+j][iz+k]>0 && getDistance(pnt, pnt_)*apixX <= sphereR){
												avgD += cube[ix+i][iy+j][iz+k];
												cntr++;
											}
										}
									}
								}
							}
						}
					}
					avgD /= cntr;

					//increment local counters for those voxels who has density greater than the avg density
					for (i=-sphereVxl; i<=sphereVxl; i++) {
						if (ix+i>-1 && ix+i<numRows()){
							pnt_.x = ix+i;
							for (j=-sphereVxl; j<=sphereVxl; j++) {
								if (iy+j>-1 && iy+j<numCols()){
									pnt_.y = iy+j;
									for (k=-sphereVxl; k<=sphereVxl; k++) {
										if (iz+k>-1 && iz+k<numSlcs()){
											pnt_.z = iz+k;
											if (cube[ix+i][iy+j][iz+k]>avgD && getDistance(pnt, pnt_)*apixX <= sphereR){
												lCntrs[ix+i][iy+j][iz+k]++;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	peakCell tmpCell;
	priority_queue<peakCell> localPks;

	for (ix=0; ix<numRows(); ix++) {
		tmpCell.pnt.x = ix*apixX + hdr.xorigin;
		for (iy=0; iy<numCols(); iy++) {
			tmpCell.pnt.y = iy*apixY + hdr.yorigin;
			for (iz=0; iz<numSlcs(); iz++) {
				tmpCell.pnt.z = iz*apixZ + hdr.zorigin;
				if (lCntrs[ix][iy][iz] > 0){
					tmpCell.peak = lCntrs[ix][iy][iz];
					localPks.push (tmpCell);
				}
			}
		}
	}

	peaks.clear ();
	i=0;
	short nPeaks = localPks.size ()*prcntg;
	while (!localPks.empty() && i<nPeaks){
		peaks.push_back (localPks.top ().pnt);
		localPks.pop ();
		i++;
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Map::localPeaksMap(vector<Coordinate> &pnts, vector<vector<Coordinate> > ssEdges, float radius, float peakTHRg)
{
	int i, j;
	/*
	 *		find local peaks globally ... means after cleaning (or without cleaning) the density map, it finds local peaks for the remaining
	 *		densities ... it also removes densities close to (around) SSs
	 */

	/*
	 *	delete density around Sticks
	 */

	cleanVxls(ssEdges, radius);

	/*
	 *		find local peaks globally for the entire map
	 */
	Coordinate origin;
	short nAAloop = 10000;
	cout<<endl<<"Finding local Peaks for the entire map ...";
	localPeaks (origin, origin, nAAloop, pnts, peakTHRg);		//pnts are stored in XYZ system
	cout<<" Done."<<endl;


	//re-convert edges pointrs to xyz coordinate system
//	for (i=0; i<ssEdges.size (); i++){
//		for (j=0; j<ssEdges[i].size (); j++){
//			ssEdges[i][j].x *= apixX;
//			ssEdges[i][j].y *= apixY;
//			ssEdges[i][j].z *= apixZ;
//		}
//	}

	//remove peaks around other SSs (for each axis...remove all peaks around this axis for certain distance)
	cout<<"Removing local Peaks around SS ....";
	Coordinate nPnt;
	for (i=0; i<ssEdges.size (); i++){
		for (j=0; j<ssEdges[i].size ()-1; j++){
			int k=0;
			while (k<pnts.size ()){
				//remove local peaks within 3 Angstrom from the axis....exclude those peaks at both ends of the axis
				//if the projection of the local points inside the axis then it is not after one of the two ends
				float proj = linePointIntersection(ssEdges[i][0], ssEdges[i][ssEdges[i].size ()-1], pnts[k], nPnt);
				if (getDistance(pnts[k], ssEdges[i][j]) < 4 &&
					(proj > 0 && proj < 1)){
					pnts.erase (pnts.begin () + k);
					k -= 1;
				}
				k++;
			}
		}
	}

	cout<<" Done."<<endl;
}
////////////////////////////////////////////////////////////////////////////////////
void Map::filterize(float threshold)
{
    cout<<"Filtering the map ..."<<endl<<endl;

	int iRow, iCol, iDepth;

	for (iRow=0; iRow<numRows(); iRow++)
		for (iCol=0; iCol<numCols(); iCol++)
			for(iDepth=0; iDepth<numSlcs(); iDepth++)
				if (cube[iRow][iCol][iDepth] < threshold)
					cube[iRow][iCol][iDepth] = 0.0;
}
////////////////////////////////////////////////////////////////////////////////////
void Map::normalize()
{
    cout<<"Normalizing the map ..."<<endl<<endl;

	int iRow, iCol, iDepth;

	for (iRow=0; iRow<numRows(); iRow++)
		for (iCol=0; iCol<numCols(); iCol++)
			for(iDepth=0; iDepth<numSlcs(); iDepth++)
            {
                cube[iRow][iCol][iDepth] = cube[iRow][iCol][iDepth]/hdr.amax;
            }

}
////////////////////////////////////////////////////////////////////////////////////
short Map::numRows ()
{
	return 	cube.size();
}
////////////////////////////////////////////////////////////////////////////////////
short Map::numCols()
{
	if (cube.size ())
		return cube[0].size();
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////////////
short Map::numSlcs()
{
	if (cube.size ())
		return cube[0][0].size();
	else
		return 0;
}
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Added by Dong Si below///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// smoothing the mrc using Gaussian filter
void Map::gauss_smooth()
{
    cout<<"Gauss smoothing the map..."<<endl<<endl;

    int t;

    float wt[7]={0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006};

    // Convolving along x direction
    for (int k=3; k<numSlcs()-3; k++)
        for (int j=3; j<numCols()-3; j++)
            for (int i=3; i<numRows()-3; i++)
	        {
	            float temp=0;
	            for (t=-3; t<=3; t++)
                     temp += cube[i+t][j][k]*wt[t+3];

	            cube[i][j][k]=temp;
	        }

    // Convolving along y direction
    for (int k=3; k<numSlcs()-3; k++)
        for (int j=3; j<numCols()-3; j++)
            for (int i=3; i<numRows()-3; i++)
	        {
	            float temp=0;
	            for (t=-3; t<=3; t++)
                     temp += cube[i][j+t][k]*wt[t+3];

	            cube[i][j][k]=temp;
	        }

    // Convolving along z direction
    for (int k=3; k<numSlcs()-3; k++)
        for (int j=3; j<numCols()-3; j++)
            for (int i=3; i<numRows()-3; i++)
	        {
	            float temp=0;
                for (t=-3; t<=3; t++)
                    temp += cube[i][j][k+t]*wt[t+3];

	            cube[i][j][k]=temp;
	        }

    update_hdrInfo();
}
////////////////////////////////////////////////////////////////////////////////////
//update the header info after modify the density map (ex. gauss)
//update max, min, mean density value --- amin, amax, amean
void Map::update_hdrInfo()
{
    cout<<"Updating the map header ..."<<endl<<endl;

    float max=-999, min=999, mean=0;

    for (int k=0; k<numSlcs(); k++)
       for (int j=0; j<numCols(); j++)
          for (int i=0; i<numRows(); i++)
          {
              if (cube[i][j][k] > max)
                  max = cube[i][j][k];
              if (cube[i][j][k] < min)
                  min = cube[i][j][k];

              mean += cube[i][j][k];
          }

          hdr.amax = max;
          hdr.amin = min;
          hdr.amean = float (mean/(numSlcs()*numCols()*numRows()));
}
////////////////////////////////////////////////////////////////////////////////////
// calculate the gradient by using Sobel-like convolution masks
// when fast=0, use 3D matrix to find the gradient; when fast=1, use two neighbors to find the gradient
void Map::buildGradient(int fast)
{
    //resize the gradient vector

    grad.resize(numRows());			//resize rows

	for(int i=0; i<numRows(); i++){
		grad[i].resize(numCols());		// resize cols

		for (int j=0; j<numCols(); j++)
			grad[i][j].resize(numSlcs());	//resize depth
	}


    float globalmaxda=-999;  //global max gradient value
    float globalminda=999;  //global min gradient value

    //close to Sobel
    float mx[3][3][3];
    float my[3][3][3];
    float mz[3][3][3];

    // setup derivative masks
    for (int i=0;i<3;i++) {
       for (int j=0;j<3;j++) {
          for (int k=0;k<3;k++) {
              mx[i][j][k]=0;
              my[i][j][k]=0;
              mz[i][j][k]=0;
          }
       }
    }

    for (int i=0;i<3;i++) {
        for (int j=0;j<3;j++) {
            float val=2.0;
            if (i% 2+j %2 ==0) val=1.0;  //modified by Y. Lu
            if (i==1 && j ==1 ) val=4.0;

            //pattern
            // 121
            // 242
            // 121

            mx[0][i][j]=val;
            mx[2][i][j]=-val;

            my[i][0][j]=val;
            my[i][2][j]=-val;

            mz[i][j][0]=val;
            mz[i][j][2]=-val;

        }
    }


    cout<<"Building gradient..."<<endl;
    cout<<endl<<endl;

    for (int k=1; k<numSlcs()-1; k++)
       for (int j=1; j<numCols()-1; j++)
          for (int i=1; i<numRows()-1; i++)
          {
                 //gradient
                 if (fast)
                 {
                     grad[i][j][k].dx=cube[i-1][j][k]-cube[i+1][j][k];
                     grad[i][j][k].dy=cube[i][j-1][k]-cube[i][j+1][k];
                     grad[i][j][k].dz=cube[i][j][k-1]-cube[i][j][k+1];
                     //cout<<grad[i][j][k].dx;
                 }
                 else
                 {
                     grad[i][j][k].dx=0;
                     grad[i][j][k].dy=0;
                     grad[i][j][k].dz=0;

                     for (int l=-1;l<2;l++)
                         for (int m=-1;m<2;m++)
                             for (int n=-1;n<2;n++)
                             {
                                  grad[i][j][k].dx+=mx[l+1][m+1][n+1]*cube[i+l][j+m][k+n];
                                  grad[i][j][k].dy+=my[l+1][m+1][n+1]*cube[i+l][j+m][k+n];
                                  grad[i][j][k].dz+=mz[l+1][m+1][n+1]*cube[i+l][j+m][k+n];
                             }
                 }

                 //modified by Dong///////////////////
                 grad[i][j][k].dx = -grad[i][j][k].dx;
                 grad[i][j][k].dy = -grad[i][j][k].dy;
                 grad[i][j][k].dz = -grad[i][j][k].dz;
                 /////////////////////////////////////

                 grad[i][j][k].da=sqrt(pow(grad[i][j][k].dx,2)+
                                       pow(grad[i][j][k].dy,2)+
                                       pow(grad[i][j][k].dz,2));

                if (grad[i][j][k].da!=0)
                {
                 grad[i][j][k].dx/=grad[i][j][k].da;
                 grad[i][j][k].dy/=grad[i][j][k].da;
                 grad[i][j][k].dz/=grad[i][j][k].da;
                }

                if (globalmaxda<grad[i][j][k].da)
                    globalmaxda=grad[i][j][k].da;

                if (globalminda>grad[i][j][k].da && cube[i][j][k]!=0)
                    globalminda=grad[i][j][k].da;

          }

    cout<<"Done the gradient building!"<<endl;
    cout<<"global max gradient= "<<globalmaxda<<endl;
    cout<<"global min gradient= "<<globalminda<<endl<<endl;
}
////////////////////////////////////////////////////////////////////////////////////
// Build the structure tensor by using Jacobi method
//
void Map::buildTensor()
{
    cout<<"Building tensor..."<<endl;
    cout<<endl<<endl;

    //resize the tensor vector

    tens.resize(numRows());			//resize rows

	for(int i=0; i<numRows(); i++){
		tens[i].resize(numCols());		// resize cols

		for (int j=0; j<numCols(); j++)
			tens[i][j].resize(numSlcs());	//resize depth
	}

    /////////////////////////////build Hessian matrix////////////////////////////////////////
    for (int k=1; k<numSlcs()-1; k++)
       for (int j=1; j<numCols()-1; j++)
           for (int i=1; i<numRows()-1; i++)
           {
                   vector<float> row0, row1, row2;
                   float dx2=0.0, dy2=0.0, dz2=0.0, dxdy=0.0, dydz=0.0, dxdz=0.0;

                   dx2 = grad[i][j][k].dx * grad[i][j][k].dx;
                   dy2 = grad[i][j][k].dy * grad[i][j][k].dy;
                   dz2 = grad[i][j][k].dz * grad[i][j][k].dz;
                   dxdy = grad[i][j][k].dx * grad[i][j][k].dy;
                   dydz = grad[i][j][k].dy * grad[i][j][k].dz;
                   dxdz = grad[i][j][k].dx * grad[i][j][k].dz;

                   row0.push_back(dx2);
                   row0.push_back(dxdy);
                   row0.push_back(dxdz);
                   row1.push_back(dxdy);
                   row1.push_back(dy2);
                   row1.push_back(dydz);
                   row2.push_back(dxdz);
                   row2.push_back(dydz);
                   row2.push_back(dz2);


                   tens[i][j][k].Hmatrix.resize(3);
                   for (int n=0; n<3; n++)
                        tens[i][j][k].Hmatrix[n].resize(3);

                   tens[i][j][k].Hmatrix[0]=row0;
                   tens[i][j][k].Hmatrix[1]=row1;
                   tens[i][j][k].Hmatrix[2]=row2;
           }

    float wt[7]={0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006};   //gauss convolution mask window

    //Convolving along x direction
    for (int k=4; k<numSlcs()-4; k++)
       for (int j=4; j<numCols()-4; j++)
           for (int i=4; i<numRows()-4; i++)
           {
                for (int m=0; m<3; m++)
                    for (int n=0; n<3; n++)
                    {
                        float temp=0.0;

                        for (int t=-3; t<=3; t++)
                        {
                            temp += tens[i+t][j][k].Hmatrix[m][n]*wt[t+3];
                        }

                        tens[i][j][k].Hmatrix[m][n]=temp;
                    }
           }

    //Convolving along y direction
    for (int k=4; k<numSlcs()-4; k++)
       for (int j=4; j<numCols()-4; j++)
           for (int i=4; i<numRows()-4; i++)
           {
                for (int m=0; m<3; m++)
                    for (int n=0; n<3; n++)
                    {
                        float temp=0.0;

                        for (int t=-3; t<=3; t++)
                        {
                            temp += tens[i][j+t][k].Hmatrix[m][n]*wt[t+3];
                        }

                        tens[i][j][k].Hmatrix[m][n]=temp;
                    }
           }

    //Convolving along z direction
    for (int k=4; k<numSlcs()-4; k++)
       for (int j=4; j<numCols()-4; j++)
           for (int i=4; i<numRows()-4; i++)
           {
                for (int m=0; m<3; m++)
                    for (int n=0; n<3; n++)
                    {
                        float temp=0.0;

                        for (int t=-3; t<=3; t++)
                        {
                            temp += tens[i][j][k+t].Hmatrix[m][n]*wt[t+3];
                        }

                        //Store the new result in the Hmatrix entry
                        tens[i][j][k].Hmatrix[m][n] = temp;
                    }
           }


    for (int k=4; k<numSlcs()-4; k++)
       for (int j=4; j<numCols()-4; j++)
           for (int i=4; i<numRows()-4; i++)
           {
              if (cube[i][j][k]>0.0)
              {
                Jacobi J;

                J.matrix.resize(3);
                for (int n=0; n<3; n++)
                     J.matrix[n].resize(3);

                J.matrix = tens[i][j][k].Hmatrix;
                J.dimen = 3;
                J.eigenvalues.resize(J.dimen);
                J.eigenvectors.resize(J.dimen);
                J.e = 1e-8;
                J.jacobi();

                tens[i][j][k].Evalue.resize(3);
                tens[i][j][k].Evector.resize(3);
                for (int n=0; n<3; n++)
                     tens[i][j][k].Evector[n].resize(3);

                tens[i][j][k].Evalue = J.getEigenvalues();
                tens[i][j][k].Evector = J.getEigenvectors();
              }
           }


    cout<<"Done the tensor building!"<<endl;
    cout<<endl<<endl;
}
////////////////////////////////////////////////////////////////////////////////////
// Build the structure thickness by traveling along the eigen vector direction
//
void Map::buildThickness(float threshold)
{
    setApix();

    //resize the thickness vector
    thick.resize(numRows());			//resize rows

	for(int i=0; i<numRows(); i++){
		thick[i].resize(numCols());		// resize cols

		for (int j=0; j<numCols(); j++)
			thick[i][j].resize(numSlcs());	//resize depth
	}

    cout<<"Building thickness..."<<endl;
    cout<<endl<<endl;


    for (int k=4; k<numSlcs()-4; k++)
       for (int j=4; j<numCols()-4; j++)
           for (int i=4; i<numRows()-4; i++)
           {
              if (cube[i][j][k]>0.0)
              {
               //Evectors
               vector<float> v1(tens[i][j][k].Evector[0]);
               vector<float> v2(tens[i][j][k].Evector[1]);
               vector<float> v3(tens[i][j][k].Evector[2]);



               // t1, goes along Evector direction
               int n=0;
               float temp=0; // temp distance
               while (true)
               {
                    if (i+int(n*v1[0])>numRows()-4 || j+int(n*v1[1])>numCols()-4 || k+int(n*v1[2])>numSlcs()-4 \
                        || i+int(n*v1[0])<4 || j+int(n*v1[1])<4 || k+int(n*v1[2])<4)
                        {
                            temp = sqrt(pow(n*v1[0]*apixX,2)+pow(n*v1[1]*apixY,2)+pow(n*v1[2]*apixZ,2));
                            break;
                        }
                    else if (cube[i+int(n*v1[0])][j+int(n*v1[1])][k+int(n*v1[2])]<=threshold)
                    {
                        temp = sqrt(pow(n*v1[0]*apixX,2)+pow(n*v1[1]*apixY,2)+pow(n*v1[2]*apixZ,2));
                        break;
                    }
                    n++;
               }
               thick[i][j][k].t1 += temp;

               // goes along oppsite direction
               n=0;
               temp=0;
               while (true)
               {
                    if (i+int(n*-v1[0])>numRows()-4 || j+int(n*-v1[1])>numCols()-4 || k+int(n*-v1[2])>numSlcs()-4 \
                        || i+int(n*-v1[0])<4 || j+int(n*-v1[1])<4 || k+int(n*-v1[2])<4)
                        {
                            temp = sqrt(pow(n*-v1[0]*apixX,2)+pow(n*-v1[1]*apixY,2)+pow(n*-v1[2]*apixZ,2));
                            break;
                        }
                    else if (cube[i+int(n*-v1[0])][j+int(n*-v1[1])][k+int(n*-v1[2])]<=threshold)
                    {
                        temp = sqrt(pow(n*-v1[0]*apixX,2)+pow(n*-v1[1]*apixY,2)+pow(n*-v1[2]*apixZ,2));
                        break;
                    }
                    n++;
               }
               thick[i][j][k].t1 += temp;


               // t2
               n=0;
               temp=0;
               while (true)
               {
                    if (i+int(n*v2[0])>numRows()-4 || j+int(n*v2[1])>numCols()-4 || k+int(n*v2[2])>numSlcs()-4 \
                        || i+int(n*v2[0])<4 || j+int(n*v2[1])<4 || k+int(n*v2[2])<4)
                        {
                            temp = sqrt(pow(n*v2[0]*apixX,2)+pow(n*v2[1]*apixY,2)+pow(n*v2[2]*apixZ,2));
                            break;
                        }
                    else if (cube[i+int(n*v2[0])][j+int(n*v2[1])][k+int(n*v2[2])]<=threshold)
                    {
                        temp = sqrt(pow(n*v2[0]*apixX,2)+pow(n*v2[1]*apixY,2)+pow(n*v2[2]*apixZ,2));
                        break;
                    }
                    n++;
               }
               thick[i][j][k].t2 += temp;

               n=0;
               temp=0;
               while (true)
               {
                    if (i+int(n*-v2[0])>numRows()-4 || j+int(n*-v2[1])>numCols()-4 || k+int(n*-v2[2])>numSlcs()-4 \
                        || i+int(n*-v2[0])<4 || j+int(n*-v2[1])<4 || k+int(n*-v2[2])<4)
                        {
                            temp = sqrt(pow(n*-v2[0]*apixX,2)+pow(n*-v2[1]*apixY,2)+pow(n*-v2[2]*apixZ,2));
                            break;
                        }
                    else if (cube[i+int(n*-v2[0])][j+int(n*-v2[1])][k+int(n*-v2[2])]<=threshold)
                    {
                        temp = sqrt(pow(n*-v2[0]*apixX,2)+pow(n*-v2[1]*apixY,2)+pow(n*-v2[2]*apixZ,2));
                        break;
                    }
                    n++;
               }
               thick[i][j][k].t2 += temp;


               // t3
               n=0;
               temp=0;
               while (true)
               {
                    if (i+int(n*v3[0])>numRows()-4 || j+int(n*v3[1])>numCols()-4 || k+int(n*v3[2])>numSlcs()-4 \
                        || i+int(n*v3[0])<4 || j+int(n*v3[1])<4 || k+int(n*v3[2])<4)
                        {
                            temp = sqrt(pow(n*v3[0]*apixX,2)+pow(n*v3[1]*apixY,2)+pow(n*v3[2]*apixZ,2));
                            break;
                        }
                    else if (cube[i+int(n*v3[0])][j+int(n*v3[1])][k+int(n*v3[2])]<=threshold)
                    {
                        temp = sqrt(pow(n*v3[0]*apixX,2)+pow(n*v3[1]*apixY,2)+pow(n*v3[2]*apixZ,2));
                        break;
                    }
                    n++;
               }
               thick[i][j][k].t3 += temp;

               n=0;
               temp=0;
               while (true)
               {
                    if (i+int(n*-v3[0])>numRows()-4 || j+int(n*-v3[1])>numCols()-4 || k+int(n*-v3[2])>numSlcs()-4 \
                        || i+int(n*-v3[0])<4 || j+int(n*-v3[1])<4 || k+int(n*-v3[2])<4)
                        {
                            temp = sqrt(pow(n*-v3[0]*apixX,2)+pow(n*-v3[1]*apixY,2)+pow(n*-v3[2]*apixZ,2));
                            break;
                        }
                    else if (cube[i+int(n*-v3[0])][j+int(n*-v3[1])][k+int(n*-v3[2])]<=threshold)
                    {
                        temp = sqrt(pow(n*-v3[0]*apixX,2)+pow(n*-v3[1]*apixY,2)+pow(n*-v3[2]*apixZ,2));
                        break;
                    }
                    n++;
               }
               thick[i][j][k].t3 += temp;

               //cout<<thick[i][j][k].t1<<" "<<thick[i][j][k].t2<<" "<<thick[i][j][k].t3<<endl<<endl;
              }
           }


    cout<<"Done the thickness building!"<<endl;
    cout<<endl<<endl;
}
////////////////////////////////////////////////////////////////////////////////////
// Saito-Toriwaki algorithm for Euclidian Distance Transformation.
// Computing Local Thickness of 3D Structures with ImageJ - Robert P. Dougherty and Karl-Heinz Kunzelmann
//
void Map::EDT()
{
    cout<<"Euclidian Distance Transform..."<<endl;
    cout<<endl<<endl;

    //resize the DT vector
    dt.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		dt[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			dt[i][j].resize(numSlcs());	//resize depth
	}

	// temp pictures
	vector<vector<vector<int> > >   f;    // temp picture F
    //resize the f vector
    f.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		f[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			f[i][j].resize(numSlcs());	//resize depth
	}

	vector<vector<vector<int> > >   g;    // temp picture G
    //resize the g vector
    g.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		g[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			g[i][j].resize(numSlcs());	//resize depth
	}

	vector<vector<vector<int> > >   h;    // temp picture H
    //resize the h vector
    h.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		h[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			h[i][j].resize(numSlcs());	//resize depth
	}


	// make object voxels to 1, background 0
    for (int k=0; k<numSlcs(); k++)
       for (int j=0; j<numCols(); j++)
           for (int i=0; i<numRows(); i++)
           {
              if (cube[i][j][k]>0.0)
                  f[i][j][k] = 1;
              else
                  f[i][j][k] = 0;
           }

    int n = numRows();
    if(numCols() > n) n = numCols();
    if(numSlcs() > n) n = numSlcs();
    int noResult = 3*(n+1)*(n+1);

    // tansform 1, get G from F
    for (int k=0; k<numSlcs(); k++)
       for (int j=0; j<numCols(); j++)
          for (int i=0; i<numRows(); i++)
          {
              int min = noResult;
              for (int x = i; x < numRows(); x++)
              {
                  if (f[x][j][k] == 0) // reach the background
                  {
                      if (pow((double)i-x,2)<min)
                      {
                          min = pow((double)i-x,2);
                      }
                      break;
                  }
              }
              for (int x = i-1; x >=0 ; x--)
              {
                  if (f[x][j][k] == 0)
                  {
                      if (pow((double)i-x,2)<min)
                      {
                          min = pow((double)i-x,2);
                      }
                      break;
                  }
              }

              g[i][j][k] = min;
          }


    // tansform 2, get H from G
    for (int k=0; k<numSlcs(); k++)
       for (int i=0; i<numRows(); i++)
          for (int j=0; j<numCols(); j++)
          {
              bool nonempty = false;
              if (g[i][j][k] > 0)
                  nonempty = true;

              if (nonempty)
              {
                  int min = noResult;
                  for (int y=0; y<numCols(); y++)
                  {
                      if (g[i][y][k] + pow((double)j-y,2) < min)
                      {
                          min = g[i][y][k] + pow((double)j-y,2);
                      }
                  }

                  h[i][j][k] = min;
              }
          }


    // tansform 3, get S from H
    for (int j=0; j<numCols(); j++)
       for (int i=0; i<numRows(); i++)
          for (int k=0; k<numSlcs(); k++)
          {
              bool nonempty = false;
              if (h[i][j][k] > 0)
                  nonempty = true;

              if (nonempty)
              {
                  int min = noResult;
                  for (int z=0; z<numSlcs(); z++)
                  {
                      if (h[i][j][z] + pow((double)k-z,2) < min)
                      {
                          min = h[i][j][z] + pow((double)k-z,2);
                      }
                  }

                  dt[i][j][k] = sqrt(min);   // s = dt^2
              }
          }
}
////////////////////////////////////////////////////////////////////////////////////
// detect the distance ridge/medial axis from the distance map
// "Computing Local Thickness of 3D Structures with ImageJ" - Robert P. Dougherty and Karl-Heinz Kunzelmann
//
void Map::DR()
{
	cout<<"Find the Distance Ridge from distance map..."<<endl;
	cout<<endl<<endl;

    //resize the DR vector
    dr.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		dr[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			dr[i][j].resize(numSlcs());	//resize depth
	}

	//Find the largest distance in the data
	float distMax = 0;

    for (int k=0; k<numSlcs(); k++)
        for (int j=0; j<numCols(); j++)
            for (int i=0; i<numRows(); i++)
            {
                if (dt[i][j][k] > distMax)
                    distMax = dt[i][j][k];
            }

	int rSqMax = (int)(pow((double)distMax,2)+0.5) + 1; // maximum d^2

	vector<bool> occurs;  // if rSqMax occurs
	occurs.resize(rSqMax);

	for (int k=0; k<numSlcs(); k++)
        for (int j=0; j<numCols(); j++)
            for (int i=0; i<numRows(); i++)
            {
                occurs[(int)(pow((double)dt[i][j][k],2)+0.5)] = true;
            }

    int numRadii = 0;  // number of different r occurs from 0 to d

    for (int i=0; i<rSqMax; i++)
    {
         if (occurs[i] == true)
             numRadii++;
    }

    //Make an index of the distance-squared values if they occurs
	vector<int> distSqIndex;
	distSqIndex.resize(rSqMax);
    vector<int> distSqValues;
    distSqValues.resize(numRadii);
    int indDS = 0;

    for (int i=0; i<rSqMax; i++)
    {
        if (occurs[i] == true)
        {
            distSqIndex[i] = indDS;
            distSqValues[indDS++] = i;
        }
    }

    vector<vector<int> > rSqTemplate = createTemplate(distSqValues);

    int numCompZ,numCompY,numCompX,numComp;
    int k1,j1,i1,dz,dy,dx;
    bool notRidgePoint;
    int sk0Sq, sk0SqInd, sk1Sq;

    for (int k=0; k<numSlcs(); k++)
        for (int j=0; j<numCols(); j++)
            for (int i=0; i<numRows(); i++)
            {
                if (dt[i][j][k]>0)
                {
                    notRidgePoint = false;
                    sk0Sq = (int)(pow((double)dt[i][j][k],2)+0.5);
                    sk0SqInd = distSqIndex[sk0Sq];

                    for (dz=-1; dz<=1; dz++)
                    {
                        k1 = k + dz;
                        if ((k1>=0) && (k1<numSlcs()))
                        {
                            if (dz==0)
                                numCompZ = 0;
                            else
                                numCompZ = 1;

                            for (dy=-1; dy <= 1; dy++)
                            {
                                j1 = j + dy;
                                if ((j1>=0) && (j1<numCols()))
                                {
                                    if (dy==0)
                                        numCompY = 0;
                                    else
                                        numCompY = 1;

                                    for (dx=-1; dx<=1; dx++)
                                    {
                                        i1 = i + dx;
                                        if((i1>=0) && (i1<numRows()))
                                        {
                                            if (dx == 0)
                                                numCompX = 0;
                                            else
                                                numCompX = 1;

                                            numComp = numCompX + numCompY + numCompZ;

                                            if (numComp>0)
                                            {
                                                sk1Sq = (int)(pow((double)dt[i1][j1][k1],2)+0.5);

                                                if (sk1Sq>=rSqTemplate[numComp-1][sk0SqInd])
                                                    notRidgePoint = true;
                                            }
                                        }//if in grid for i1
                                        if(notRidgePoint)break;
                                    }//dx
                                }//if in grid for j1
                                if(notRidgePoint)break;
                            }//dy
                        }//if in grid for k1
                        if(notRidgePoint)break;
                    }//dz
                    if (!notRidgePoint)
                       dr[i][j][k] = dt[i][j][k];  // save the distance ridge as this voxel's dt
                }//if in the object
            }

    cout<<"Distance Ridge complete!"<<endl;
}
////////////////////////////////////////////////////////////////////////////////////
// Build template --- sub-function of DR();
//
// The first index of the template is the number of nonzero components
// in the offest from the test point to the remote point.  The second
// index is the radii index (of the test point).  The value of the template
// is the minimum square radius of the remote point required to cover the
// ball of the test point.
//
vector<vector<int > > Map::createTemplate(vector<int> distSqValues)
{
    int numRadii = distSqValues.size();

    vector<vector<int> > t;
    t.resize(3);
    for (int i=0; i<3; i++)
        t.resize(numRadii);

    t[0] = scanCube(1,0,0,distSqValues);
    t[1] = scanCube(1,1,0,distSqValues);
    t[2] = scanCube(1,1,1,distSqValues);
    return t;
}
////////////////////////////////////////////////////////////////////////////////////
// scan Cube --- sub-function of DR();
//
// For each offset from the origin, (dx,dy,dz), and each radius-squared,
// rSq, find the smallest radius-squared, r1Squared, such that a ball
// of radius r1 centered at (dx,dy,dz) includes a ball of radius
// rSq centered at the origin.  These balls refer to a 3D integer grid.
// The set of (dx,dy,dz) points considered is a cube center at the origin.
//
vector<int> Map::scanCube(int dx, int dy, int dz, vector<int> distSqValues)
{
    int numRadii = distSqValues.size();
    vector<int> r1Sq;
    r1Sq.resize(numRadii);

    if ((dx==0)&&(dy==0)&&(dz==0))
    {
        for (int rSq=0; rSq<numRadii; rSq++)
        {
             r1Sq[rSq] = INT_MAX;
        }
    }
    else
    {
        int dxAbs = -(int)abs(dx);
        int dyAbs = -(int)abs(dy);
        int dzAbs = -(int)abs(dz);

        for (int rSqInd=0; rSqInd<numRadii; rSqInd++)
        {
            int rSq = distSqValues[rSqInd];
            int max = 0;
            int r = 1 + (int)sqrt(rSq);
            int scank, scankj;
            int dk, dkji;
            int iBall;
            int iPlus;
            for (int k=0; k<=r; k++)
            {
                scank = k*k;
                dk = pow((double)(k-dzAbs),2);
                for (int j=0; j<=r; j++)
                {
                    scankj = scank + j*j;
                    if (scankj<=rSq)
                    {
                        iPlus = (int)sqrt(rSq-scankj) - dxAbs;
                        dkji = dk + pow((double)(j-dyAbs),2) + pow((double)iPlus,2);

                        if (dkji>max)
                            max = dkji;
                    }
                }
            }
            r1Sq[rSqInd] = max;
        }
    }
    return r1Sq;
}
////////////////////////////////////////////////////////////////////////////////////
// calculate local thickness derived from DT and DR
float Map::LocalThickness(int x, int y, int z)
{
    setApix();
/*
    cout<<"calculate local thickness based on DT and DR..."<<endl<<endl;

    //resize the localThick vector
    localThick.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		localThick[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			localThick[i][j].resize(numSlcs());	//resize depth
	}

    vector<vector<vector<float> > >   temp;  // temporary vector
    //resize the temp vector
    temp.resize(numRows());			//resize rows
	for(int i=0; i<numRows(); i++)
	{
		temp[i].resize(numCols());		// resize cols
		for (int j=0; j<numCols(); j++)
			temp[i][j].resize(numSlcs());	//resize depth
	}

    for (int w=0; w<numSlcs(); w++)
        for (int v=0; v<numCols(); v++)
            for (int u=0; u<numRows(); u++)
            {
                if (cube[u][v][w]>0)
                {
                   for (int k=0; k<numSlcs(); k++)
                       for (int j=0; j<numCols(); j++)
                           for (int i=0; i<numRows(); i++)
                           {
                               if (pow((double)(u-i),2)+pow((double)(v-j),2)+pow((double)(w-k),2)<pow((double)dr[i][j][k],2))
                               {
                                   if (dr[i][j][k]>temp[u][v][w])
                                   {
                                       temp[u][v][w] = dr[i][j][k];
                                   }
                               }

                           }
                }
            }

    for (int w=0; w<numSlcs(); w++)
        for (int v=0; v<numCols(); v++)
            for (int u=0; u<numRows(); u++)
            {
                localThick[u][v][w] = 2 * temp[u][v][w] * apixX;
            }

    cout<<"Done the Local thickness based on DT and DR!"<<endl;
*/
    float temp; //temp thickness

    for (int k=z-20; k<=z+20; k++)
        for (int j=y-20; j<=y+20; j++)
            for (int i=x-20; i<=x+20; i++)
            {
                if (i>=0 && i<numRows() && j>=0 && j<numCols() && k>=0 && k<numSlcs())
                   if (pow((double)(x-i),2)+pow((double)(y-j),2)+pow((double)(z-k),2)<pow((double)dr[i][j][k],2))
                   {
                       if (dr[i][j][k]>temp)
                       {
                        temp = dr[i][j][k];
                       }
                   }
            }

    return 2 * temp * apixX;
}
////////////////////////////////////////////////////////////////////////////////////////
/* local peak filter for selecting backbone voxels. For each voxel,
 the average density of all voxels contained within a sphere of 3 A˚ in radius
 is calculated and those voxels in the sphere with a density value
 greater than the average have their local-peak-count number increased by 1.
 The peak counting operation loops over all voxels and assigns each voxel a
 localpeak-count number. Upon completion of this process, all voxels are sorted
 according to their local-peak-count numbers. The top 50% of voxels with highest
 localpeak-count numbers are categorized as backbone voxels, whereas the lowest 50% are discarded. */
void Map::LocalPeakFilter(int divider)
{
    setApix();

    cout<<"filtering predicted map using LPF..."<<endl<<endl;

    vector<vector<vector<int> > > lpc;  //local-peak-count number for each voxel

    // initialization of the localPeakCount for all voxels
    lpc.resize(numRows());
	for(long i=0; i<numRows(); i++)
	{
		lpc[i].resize(numCols());
		for (long j=0; j<numCols(); j++)
			lpc[i][j].resize(numSlcs());
	}

    // working on each voxel
    for (long k=0; k<numSlcs(); k++)
        for (long j=0; j<numCols(); j++)
            for (long i=0; i<numRows(); i++)
            {
                if (cube[i][j][k]>0)
                {
                    Coordinate center; //center of the sphere - corrent voxel
                    center.x=i*apixX+hdr.xorigin;
                    center.y=j*apixY+hdr.yorigin;
                    center.z=k*apixZ+hdr.zorigin;

                    double totalDensity=0;  // total density in this sphere
                    double numOfpoints=0;   // total # of voxels in this sphere

                    // looking at the neighbors
                    for (int n=-10; n<=10; n++)
                        for (int p=-10; p<=10; p++)
                            for (int q=-10; q<=10; q++)
                            {
                                Coordinate point; //points in the sphere
                                point.x=(i+n)*apixX+hdr.xorigin;
                                point.y=(j+p)*apixY+hdr.yorigin;
                                point.z=(k+q)*apixZ+hdr.zorigin;

                                // if p is in the sphere of 3 A˚ in radius
                                if (i+n<numRows() && j+p<numCols() && k+q<numSlcs() \
                                    && i+n>=0 && j+p>=0 && k+q>=0 && getDistance(center,point)<=3 \
                                    && cube[i+n][j+p][k+q]>0)
                                    {
                                        totalDensity+=cube[i+n][j+p][k+q];
                                        numOfpoints++;
                                    }
                            }

                    // average density within this sphere
                    double averageDensity = (double)totalDensity/numOfpoints;

                    // working on the neighbors
                    for (int n=-10; n<=10; n++)
                        for (int p=-10; p<=10; p++)
                            for (int q=-10; q<=10; q++)
                            {
                                Coordinate point; //points in the sphere
                                point.x=(i+n)*apixX+hdr.xorigin;
                                point.y=(j+p)*apixY+hdr.yorigin;
                                point.z=(k+q)*apixZ+hdr.zorigin;

                                // if in the sphere of 3 A˚ in radius
                                if (i+n<numRows() && j+p<numCols() && k+q<numSlcs() \
                                    && i+n>=0 && j+p>=0 && k+q>=0 && getDistance(center,point)<=3 \
                                    && cube[i+n][j+p][k+q]>0)
                                    {
                                        if (cube[i+n][j+p][k+q]>averageDensity)
                                            lpc[i+n][j+p][k+q]++;
                                    }
                            }
                }

            }

    int maxCount = 0;

    for (long k=0; k<numSlcs(); k++)
        for (long j=0; j<numCols(); j++)
            for (long i=0; i<numRows(); i++)
            {
                if (lpc[i][j][k] > maxCount)
                    maxCount = lpc[i][j][k];
            }

    for (long k=0; k<numSlcs(); k++)
        for (long j=0; j<numCols(); j++)
            for (long i=0; i<numRows(); i++)
            {
                if (lpc[i][j][k] < maxCount/divider)  // filter voxels have lower local-peak-count
                    cube[i][j][k] = 0;
            }
}
////////////////////////////////////////////////////////////////////////////////////////
//delete small voxel group that has length smaller than minLength
void Map::deleteSmallVxlGroup(float minLength, float minSize)
{
    //cout<<"filtering the map with minimum voxel-group-length "<<minLength<<" & voxel space size "<<minSize<<" ..."<<endl<<endl;

    setApix();

    node.resize(numRows());			    //resize rows
	for(long i=0; i<numRows(); i++)
	{
		node[i].resize(numCols());		    //resize cols
		for (long j=0; j<numCols(); j++)
			node[i][j].resize(numSlcs());	//resize depth
	}

    // initialize the node structure
    for (long k=0; k<numSlcs(); k++)
        for (long j=0; j<numCols(); j++)
            for (long i=0; i<numRows(); i++)
            {
                if (cube[i][j][k]>0)
                {
                    node[i][j][k].pos.x = i;
                    node[i][j][k].pos.y = j;
                    node[i][j][k].pos.z = k;
                    node[i][j][k].density = cube[i][j][k];
                }
            }

    vector<vector<Node> > groups;

    //group the voxels
    for (long k=1; k<numSlcs()-1; k++)
        for (long j=1; j<numCols()-1; j++)
            for (long i=1; i<numRows()-1; i++)
            {
                if (node[i][j][k].density>0 && node[i][j][k].traveled == false)
                {
                    vector<Node> temp; //temp group
                    temp.push_back(node[i][j][k]);
                    node[i][j][k].traveled = true;
                    AddNearbyNodes(node[i][j][k],temp);
                    groups.push_back(temp);  //push a group into the vector
                }
            }

    //cout<<"num of voxel groups before = "<<groups.size()<<endl;

    // find the two ends for each group
    for (int i=0; i<groups.size(); i++)
    {
        //cout<<groups.size()<<endl;
        if(groups[i].size() >= 2)
        {
            double maxDistance = -999,
                   length; // length of the voxel group

            for (int j=0; j<groups[i].size()-1; j++)
                for (int k=j+1; k<groups[i].size(); k++)
                {
                     if (maxDistance <= dist(groups[i][j], groups[i][k]))
                         maxDistance = dist(groups[i][j], groups[i][k]);
                }

            bool flag = false;

            for (int j=0; j<groups[i].size()-1; j++)
            {
                for (int k=j+1; k<groups[i].size(); k++)
                     if (maxDistance == dist(groups[i][j], groups[i][k]))
                     {
                         length = maxDistance * apixX;

                         flag = true;
                         break;
                     }
                if (flag)
                    break;
            }

            float totalSpace=0; //calculate the total Space for this group
            totalSpace = groups[i].size() * (apixX*apixX*apixX);

            //cout<<totalSpace<<endl;

            // delete the small group that shorter than certain length or have few total Space
            if (length<minLength || totalSpace<minSize)
            {
                for (int j=0; j<groups[i].size(); j++)
                {
                     int x,y,z;
                     x = groups[i][j].pos.x;
                     y = groups[i][j].pos.y;
                     z = groups[i][j].pos.z;
                     cube[x][y][z] = 0;
                }
            }
        }
        else // delete the cluster that group only have one voxel
        {
            int x,y,z;
            x = groups[i][0].pos.x;
            y = groups[i][0].pos.y;
            z = groups[i][0].pos.z;
            cube[x][y][z] = 0;
        }
    }

    //clear the node structure
    node.clear();

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// add nearby voxel into group
void Map::AddNearbyNodes(Node d, vector<Node> & group)
{
    int i = d.pos.x, j = d.pos.y, k = d.pos.z;

    for (int n=-1; n<=1; n++)
        for (int p=-1; p<=1; p++)
            for (int q=-1; q<=1; q++)
            {
                int c=abs(n)+abs(p)+abs(q); // if c=1, consider 6 immediate neighbors; if c!=3, consider 18 neighbors

                if (c==1 && node[i+n][j+p][k+q].density>0 && node[i+n][j+p][k+q].traveled == false)
                {
                    group.push_back(node[i+n][j+p][k+q]);
                    node[i+n][j+p][k+q].traveled = true;
                    AddNearbyNodes(node[i+n][j+p][k+q], group);
                }
            }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// calculate the distance btw to nodes based on voxel
float Map::dist(Node p1, Node p2)
{
	return sqrt(((p1.pos.x - p2.pos.x)*(p1.pos.x - p2.pos.x)) + ((p1.pos.y - p2.pos.y)*(p1.pos.y - p2.pos.y)) + ((p1.pos.z - p2.pos.z)*(p1.pos.z - p2.pos.z)));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// cluster the helix points into different helix and output stick files
void Map::Output_HLX(string pdbID, vector<Coordinate> curve_pnts)
{
    cout<<"output helix stick files ..."<<endl<<endl;

    //calculate apix
    setApix();

    /////////////// cluster HLX points ///////////////////////
    cout<<"Clustering helix points..."<<endl<<endl;

    node.resize(numRows());			    //resize rows
	for(long i=0; i<numRows(); i++)
	{
		node[i].resize(numCols());		    //resize cols
		for (long j=0; j<numCols(); j++)
			node[i][j].resize(numSlcs());	//resize depth
	}

    // initialize the node structure
    for (long k=0; k<numSlcs(); k++)
        for (long j=0; j<numCols(); j++)
            for (long i=0; i<numRows(); i++)
            {
                if (cube[i][j][k]>0)
                {
                    node[i][j][k].pos.x = i;
                    node[i][j][k].pos.y = j;
                    node[i][j][k].pos.z = k;
                    node[i][j][k].density = cube[i][j][k];
                }
            }

    vector<vector<Node> > HLXclusters;

    //cluster the helix voxels
    for (long k=1; k<numSlcs()-1; k++)
        for (long j=1; j<numCols()-1; j++)
            for (long i=1; i<numRows()-1; i++)
            {
                if (node[i][j][k].density>0 && node[i][j][k].traveled == false)
                {
                    vector<Node> temp; //temp group
                    temp.push_back(node[i][j][k]);
                    node[i][j][k].traveled = true;
                    AddNearbyNodes(node[i][j][k],temp);
                    HLXclusters.push_back(temp);  //push a group into the vector
                }
            }

    int NumofHLX = HLXclusters.size();

    /////////////////////////// find the two ends for each cluster /////////////////////////
    cout<<"Find out two ends of each helix..."<<endl<<endl;

    for (int i=0; i<NumofHLX; i++)
    {
        double maxDistance = -999,
               length; // length of this hilex

        for (int j=0; j<HLXclusters[i].size()-1; j++)
            for (int k=j+1; k<HLXclusters[i].size(); k++)
            {
                if (maxDistance <= dist(HLXclusters[i][j], HLXclusters[i][k]))
                    maxDistance = dist(HLXclusters[i][j], HLXclusters[i][k]);
            }

        bool flag = false;

        for (int j=0; j<HLXclusters[i].size()-1; j++)
        {
            for (int k=j+1; k<HLXclusters[i].size(); k++)
                if (maxDistance == dist(HLXclusters[i][j], HLXclusters[i][k]))
                {
                    HLXclusters[i][j].isHlxEnd = true;
                    HLXclusters[i][k].isHlxEnd = true;

                    length = maxDistance * apixX;

                    flag = true;
                    break;
                }
            if (flag)
                break;
        }
    }

    cout<<"--- Number of detected helices = "<<NumofHLX<<endl<<endl;

    vector<vector<Node> > tempHLXs(NumofHLX); //temp helices represented by the skeleton centroids

    //convert all helix centroids to helix node structure
    for (int n=0; n<curve_pnts.size(); n++)
    {
        for (int i=0; i<NumofHLX; i++)
        {
            bool flag=false;
            for (int j=0; j<HLXclusters[i].size(); j++)
            {
                Coordinate temp;
                temp.x=HLXclusters[i][j].pos.x*apixX+hdr.xorigin;
                temp.y=HLXclusters[i][j].pos.y*apixY+hdr.yorigin;
                temp.z=HLXclusters[i][j].pos.z*apixZ+hdr.zorigin;

                if (getDistance(temp, curve_pnts[n])<=apixX)
                {
                    tempHLXs[i].push_back(HLXclusters[i][j]);
                    flag=true;
                    break;
                }
            }
            if (flag) break;
        }
    }
    for (int i=0; i<NumofHLX; i++)
        for (int j=0; j<HLXclusters[i].size(); j++)
        {
            if (HLXclusters[i][j].isHlxEnd == true)
               tempHLXs[i].push_back(HLXclusters[i][j]);
        }


    // re-order the psudo points of each helix
    cout<<"Re-order the helix nodes..."<<endl<<endl;
    vector<vector<Node> > AllHelices(NumofHLX); //helices represented by the re-orded skeleton centroids

    for (int i=0; i<tempHLXs.size(); i++)
        for (int j=0; j<tempHLXs[i].size(); j++)
        {
            if (tempHLXs[i][j].isHlxEnd == true)
            {
                Node end;
                end.pos.x=tempHLXs[i][j].pos.x;
                end.pos.y=tempHLXs[i][j].pos.y;
                end.pos.z=tempHLXs[i][j].pos.z;
                AllHelices[i].push_back(end); // push one end to the helix

                int tempSize = tempHLXs[i].size();

                for (int k=0; k<tempSize-1; k++)
                {
                    int index = findClosestNodeInHLX(tempHLXs[i], end);  //find the closest hlx centroids to that end
                    if (index != 999)
                    {
                        AllHelices[i].push_back(tempHLXs[i][index]); // push it to the helix
                        vector<Node>::iterator itr = tempHLXs[i].begin() + index;
                        tempHLXs[i].erase(itr);  //delete that node from tempHLX
                    }
                }
                break;
            }
        }


    // add more HLX psudo nodes along the axis
    //cout<<"Adding more helix psudo nodes along the axis..."<<endl<<endl;
    //AddHLXnodes(AllHelices);

    ///////////////////////////////// write to file ////////////////////////////////////

    //create directory for output files, output folder should be created firstly
	string outDirName = "Output/";
	outDirName += pdbID;
	outDirName += "_outFiles/";

    /*
    cout<<"Writting to Helix ends file..."<<endl<<endl;
    // write to pdb file
    string outFile2 = pdbID + "_hlxEnd.pdb";

    vector<Coordinate> Ends;   // points to write to pdb file
    Protein hlxEnds;

    for (int i=0; i<NumofHLX; i++)
        for (int j=0; j<AllHelices[i].size(); j++)
        {
             if (AllHelices[i][j].isHlxEnd == true)
             {
                 Coordinate p;
                 p.x=AllHelices[i][j].pos.x*apixX+mrcF.hdr.xorigin;
                 p.y=AllHelices[i][j].pos.y*apixY+mrcF.hdr.yorigin;
                 p.z=AllHelices[i][j].pos.z*apixZ+mrcF.hdr.zorigin;

                 Ends.push_back(p);

                 node[AllHelices[i][j].pos.x][AllHelices[i][j].pos.y][AllHelices[i][j].pos.z].isHlxEnd = true;
             }
        }

    hlxEnds = points2pdb(Ends);
    hlxEnds.writePDB(outFile2,1,hlxEnds.AAs.size());
    */

    // write each helix to a single file
    for (int i=0; i<NumofHLX; i++)
    {
        stringstream stream;
        string index;
        stream<<i+1;
        stream>>index;
        cout<<"--- output helix "<<index<<endl;

        string outfile3 = outDirName + pdbID + "_HLX" + index +".pdb";

        vector<Coordinate> pnts;   // points to write to pdb file
        Protein HLX;

        for (int j=0; j<AllHelices[i].size(); j++)
        {
            Coordinate p;
            p.x=AllHelices[i][j].pos.x*apixX+hdr.xorigin;
            p.y=AllHelices[i][j].pos.y*apixY+hdr.yorigin;
            p.z=AllHelices[i][j].pos.z*apixZ+hdr.zorigin;

            pnts.push_back(p);
        }

        HLX = points2stick(pnts);
        HLX.writePDB(outfile3,1,HLX.AAs.size());
    }


    // write to helix stick txt file
    string stickFile = outDirName + pdbID + "_helixSticks.txt";
    ofstream outfile1(stickFile.c_str());
    float x, y, z;

    for (int i=0; i<NumofHLX; i++)
    {
        for (int j=0; j<AllHelices[i].size(); j++)
        {
            x=AllHelices[i][j].pos.x*apixX+hdr.xorigin;
            y=AllHelices[i][j].pos.y*apixY+hdr.yorigin;
            z=AllHelices[i][j].pos.z*apixZ+hdr.zorigin;
            outfile1<<right<<setw(12)<<x<<'\t'<<right<<setw(13)<<y<<'\t'<<right<<setw(13)<<z<<'\t'<<" "<<i+1<<endl;
        }

        outfile1<<endl;
    }

    outfile1<<endl<<"--- Number of detected helices = "<<NumofHLX<<endl;
    outfile1.close();

    /*
    // write to helix stick pdb file
    string outFile4 = pdbID + "_helics.pdb";

    vector<Coordinate> atoms;   // points to write to pdb file
    Protein helics;

    for (int i=0; i<NumofHLX; i++)
        for (int j=0; j<AllHelices[i].size(); j++)
             {
                 Coordinate p;
                 p.x=AllHelices[i][j].pos.x*apixX+helices.hdr.xorigin;
                 p.y=AllHelices[i][j].pos.y*apixY+helices.hdr.yorigin;
                 p.z=AllHelices[i][j].pos.z*apixZ+helices.hdr.zorigin;

                 atoms.push_back(p);
             }

    helics = points2pdb(atoms);
    helics.writePDB(outFile4,1,helics.AAs.size());

    // write to helixEnd txt file
    string txt = pdbID + "_helixEnd.txt";
    ofstream outfile(txt.c_str());

    for (int i=0; i<NumofHLX; i++) {

        outfile<<"Helix   "<<setw(2)<<i+1;

        for (int j=0; j<AllHelices[i].size(); j++)
             if (AllHelices[i][j].isHlxEnd == true)
             {
                 x=AllHelices[i][j].pos.x*apixX+helices.hdr.xorigin;
                 y=AllHelices[i][j].pos.y*apixY+helices.hdr.yorigin;
                 z=AllHelices[i][j].pos.z*apixZ+helices.hdr.zorigin;
                 outfile<<setw(10)<<x<<setw(10)<<y<<setw(10)<<z<<"    ";
             }
        outfile<<endl;
    }

    outfile<<endl<<"--- Number of detected helices = "<<NumofHLX<<endl;
    outfile.close();
    */


    //clear the node structure
    node.clear();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// find closest point to the End node in the HLX cluster
int Map::findClosestNodeInHLX(vector<Node> Cluster, Node End)
{
    float minDistance = 999;

    for (int i=0; i<Cluster.size(); i++)
        if (dist(End,Cluster[i]) < minDistance && dist(End,Cluster[i]) > 0)
        {
             minDistance = dist(End,Cluster[i]);
        }

    //cout<<"minDistance = "<<minDistance<<endl;

    if (minDistance == 999)
       return 999;// can not find the closest - means there is no node left

    for (int i=0; i<Cluster.size(); i++) {
         if (dist(End,Cluster[i]) == minDistance)
         {
             return i;  // return the index of that node in this cluster
         }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// cluster the sheets points into different sheey and output stick files
void Map::Output_SHT(string pdbID, vector<Coordinate> & curve_pnts)
{
    //cout<<"output sheet stick files ..."<<endl<<endl;
    //calculate apix
    setApix();

    /////////////// cluster SHT points ///////////////////////

    node.resize(numRows());			    //resize rows
	for(long i=0; i<numRows(); i++)
	{
		node[i].resize(numCols());		    //resize cols
		for (long j=0; j<numCols(); j++)
			node[i][j].resize(numSlcs());	//resize depth
	}

    // initialize the node structure
    for (long k=0; k<numSlcs(); k++)
        for (long j=0; j<numCols(); j++)
            for (long i=0; i<numRows(); i++)
            {
                if (cube[i][j][k]>0)
                {
                    node[i][j][k].pos.x = i;
                    node[i][j][k].pos.y = j;
                    node[i][j][k].pos.z = k;
                    node[i][j][k].density = cube[i][j][k];
                }
            }

    vector<vector<Node> > SHTclusters;

    //cluster the sheet voxels
    for (long k=1; k<numSlcs()-1; k++)
        for (long j=1; j<numCols()-1; j++)
            for (long i=1; i<numRows()-1; i++)
            {
                if (node[i][j][k].density>0 && node[i][j][k].traveled == false)
                {
                    vector<Node> temp; //temp group
                    temp.push_back(node[i][j][k]);
                    node[i][j][k].traveled = true;
                    AddNearbyNodes(node[i][j][k],temp);
                    SHTclusters.push_back(temp);  //push a group into the vector
                }
            }

    int NumofSHT = SHTclusters.size();
    cout<<"--- Number of detected sheets = "<<NumofSHT<<endl<<endl;

    //create directory for output files, output folder should be created firstly
	string outDirName = "Output/";
	outDirName += pdbID;
	outDirName += "_outFiles/";

    //Mask all curve points that very close to sheet voxels
    for (int i=0; i<NumofSHT; i++)
        for (int j=0; j<SHTclusters[i].size(); j++)
        {
            Coordinate sht_pnt;
            sht_pnt.x = SHTclusters[i][j].pos.x*apixX+hdr.xorigin;
            sht_pnt.y = SHTclusters[i][j].pos.y*apixY+hdr.yorigin;
            sht_pnt.z = SHTclusters[i][j].pos.z*apixZ+hdr.zorigin;

            for (int n=0; n<curve_pnts.size(); n++)
            {
                if (getDistance(curve_pnts[n], sht_pnt)<=apixX)
                {
                    curve_pnts.erase(curve_pnts.begin()+n);
                    n--;
                }
            }
        }
/*
    //Find out outgoing curve points of each sheet
    for (int i=0; i<NumofSHT; i++)
    {
        string indx;
        stringstream out;
        out << i;
        indx = out.str();
        vector<Coordinate> outgo_pnts;

        for (int j=0; j<SHTclusters[i].size(); j++)
        {
            Coordinate sht_pnt;
            sht_pnt.x = SHTclusters[i][j].pos.x*apixX+hdr.xorigin;
            sht_pnt.y = SHTclusters[i][j].pos.y*apixY+hdr.yorigin;
            sht_pnt.z = SHTclusters[i][j].pos.z*apixZ+hdr.zorigin;

            for (int n=0; n<curve_pnts.size(); n++)
            {
                if (getDistance(curve_pnts[n], sht_pnt)<=2*apixX*1.732)
                {
                    outgo_pnts.push_back(curve_pnts[n]);
                }
            }
        }

        Protein outgo_atoms;		// a temp pdb file to write points into pdb files viewable by Chimera
	    outgo_atoms = points2pdb(outgo_pnts, "HOH", " O  ");
	    outgo_atoms.writePDB(outDirName + pdbID + "_sheet_" + indx + "_outgos.pdb", 1, outgo_atoms.numOfAA());
    }
*/

    //clear the node structure
    node.clear();
}


/*
 *		DENSITY MAP : End of CLASS Implementation
 */
#endif


