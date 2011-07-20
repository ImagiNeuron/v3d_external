/*
 * Copyright (c)2006-2010  Hanchuan Peng (Janelia Farm, Howard Hughes Medical Institute).
 * All rights reserved.
 */


/************
                                            ********* LICENSE NOTICE ************

This folder contains all source codes for the V3D project, which is subject to the following conditions if you want to use it.

You will ***have to agree*** the following terms, *before* downloading/using/running/editing/changing any portion of codes in this package.

1. This package is free for non-profit research, but needs a special license for any commercial purpose. Please contact Hanchuan Peng for details.

2. You agree to appropriately cite this work in your related studies and publications.

Peng, H., Ruan, Z., Long, F., Simpson, J.H., and Myers, E.W. (2010) “V3D enables real-time 3D visualization and quantitative analysis of large-scale biological image data sets,” Nature Biotechnology, Vol. 28, No. 4, pp. 348-353, DOI: 10.1038/nbt.1612. ( http://penglab.janelia.org/papersall/docpdf/2010_NBT_V3D.pdf )

Peng, H, Ruan, Z., Atasoy, D., and Sternson, S. (2010) “Automatic reconstruction of 3D neuron structures using a graph-augmented deformable model,” Bioinformatics, Vol. 26, pp. i38-i46, 2010. ( http://penglab.janelia.org/papersall/docpdf/2010_Bioinfo_GD_ISMB2010.pdf )

3. This software is provided by the copyright holders (Hanchuan Peng), Howard Hughes Medical Institute, Janelia Farm Research Campus, and contributors "as is" and any express or implied warranties, including, but not limited to, any implied warranties of merchantability, non-infringement, or fitness for a particular purpose are disclaimed. In no event shall the copyright owner, Howard Hughes Medical Institute, Janelia Farm Research Campus, or contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement of substitute goods or services; loss of use, data, or profits; reasonable royalties; or business interruption) however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out of the use of this software, even if advised of the possibility of such damage.

4. Neither the name of the Howard Hughes Medical Institute, Janelia Farm Research Campus, nor Hanchuan Peng, may be used to endorse or promote products derived from this software without specific prior written permission.

*************/



/*
 * channelTable.cpp
 *
 *  Created on: Jul 18, 2011
 *      Author: ruanz
 */

#include "ChannelTable.h"


void make_linear_lut_one(RGBA8 color, vector<RGBA8>& lut)
{
	assert(lut.size()==256); //////// must be
	for (int j=0; j<256; j++)
	{
		float f = j/256.0;
		lut[j].r = color.r *f;
		lut[j].g = color.g *f;
		lut[j].b = color.b *f;
		lut[j].a = color.a;   //only alpha is constant
	}
}

void make_linear_lut(vector<RGBA8>& colors, vector< vector<RGBA8> >& luts)
{
	int N = colors.size();
	for (int k=0; k<N; k++)
	{
		make_linear_lut_one(colors[k], luts[k]);
	}
}

RGB8 lookup_mix(vector<unsigned char>& mC, vector< vector<RGBA8> >& mLut, int op)
{
	#define R(k) (mLut[k][ mC[k] ].r /255.0)
	#define G(k) (mLut[k][ mC[k] ].g /255.0)
	#define B(k) (mLut[k][ mC[k] ].b /255.0)
	#define A(k) (mLut[k][ mC[k] ].a /255.0)

	#define AR(k) (A(k)*R(k))
	#define AG(k) (A(k)*G(k))
	#define AB(k) (A(k)*B(k))

	int N = mC.size();
	assert(N <= mLut.size());

	float o1,o2,o3;
	o1=o2=o3=0; //must be

	if (op==OP_MAX)
	{
		for (int k=0; k<N; k++)
		{
			o1 = MAX(o1, AR(k));
			o2 = MAX(o2, AG(k));
			o3 = MAX(o3, AB(k));
		}
	}
	else if (op==OP_SUM)
	{
		for (int k=0; k<N; k++)
		{
			o1 += AR(k);
			o2 += AG(k);
			o3 += AB(k);
		}
	}
	else if (op==OP_MEAN)
	{
		for (int k=0; k<N; k++)
		{
			o1 += AR(k);
			o2 += AG(k);
			o3 += AB(k);
		}
		o1 /= N;
		o2 /= N;
		o3 /= N;
	}

	RGB8 oC;
	oC.r = o1*255;
	oC.g = o2*255;
	oC.b = o3*255;
	return oC;
}


template <class T> QPixmap copyRaw2QPixmap_Slice(
		ImagePlaneDisplayType cplane,
		V3DLONG cpos,
		const T **** p4d,
		V3DLONG sz0,
		V3DLONG sz1,
		V3DLONG sz2,
		V3DLONG sz3,
		ImageDisplayColorType Ctype,
		bool bIntensityRescale,
		double *p_vmax,
		double *p_vmin)
{

	V3DLONG x,y,z,pp;

	int N = MIN(sz3, 4); ////////////////
	vector<double> vscale(N);
	vector<double> vmin(N);
	for (int k=0; k<N; k++)
	{
		vmin[k] = p_vmin[k];
		vscale[k] = p_vmax[k]-p_vmin[k];
		vscale[k] = (vscale[k]==0)? 255.0 : (255.0/vscale[k]);
	}

	//qDebug()<<"copyRaw2QPixmap_Slice switch (Ctype)"<<Ctype;

	//set lookup-table
	vector< vector<RGBA8> > luts(4);
	vector<RGBA8> lut(256);
	RGBA8 _Red		= XYZW(255,0,0,255);
	RGBA8 _Green	= XYZW(0,255,0,255);
	RGBA8 _Blue		= XYZW(0,0,255,255);
	RGBA8 _Gray		= XYZW(255,255,255,255);
	RGBA8 _Blank	= XYZW(0,0,0,0);
	switch (Ctype)
	{
		case colorGray: //070716
			make_linear_lut_one( _Gray, lut );
			luts[0] = lut;
			luts[1] = lut;
			luts[2] = lut;
			luts[3] = lut;
			break;

		case colorRedOnly:
			make_linear_lut_one( _Red, lut );
			luts[0] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[1] = lut;
			luts[2] = lut;
			luts[3] = lut;
			break;

		case colorRed2Gray:
			make_linear_lut_one( _Gray, lut );
			luts[0] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[1] = lut;
			luts[2] = lut;
			luts[3] = lut;
			break;

		case colorGreenOnly:
			make_linear_lut_one( _Green, lut );
			luts[1] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[0] = lut;
			luts[2] = lut;
			luts[3] = lut;
			break;

		case colorGreen2Gray:
			make_linear_lut_one( _Gray, lut );
			luts[1] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[0] = lut;
			luts[2] = lut;
			luts[3] = lut;
			break;

		case colorBlueOnly:
			make_linear_lut_one( _Blue, lut );
			luts[2] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[0] = lut;
			luts[1] = lut;
			luts[3] = lut;
			break;

		case colorBlue2Gray:
			make_linear_lut_one( _Gray, lut );
			luts[2] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[0] = lut;
			luts[1] = lut;
			luts[3] = lut;
			break;

		case colorRGB:
			make_linear_lut_one( _Red, lut );
			luts[0] = lut;
			make_linear_lut_one( _Green, lut );
			luts[1] = lut;
			make_linear_lut_one( _Blue, lut );
			luts[2] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[3] = lut;
			break;

		case colorRG:
			make_linear_lut_one( _Red, lut );
			luts[0] = lut;
			make_linear_lut_one( _Green, lut );
			luts[1] = lut;
			make_linear_lut_one( _Blank, lut );
			luts[2] = lut;
			luts[3] = lut;
			break;

		case colorUnknown:
		default:
			break;
	}

	// transfer N channel's pixels
	int op =  (Ctype == colorGray)? OP_MEAN : OP_MAX;
	vector<unsigned char> mC(N);
	QImage tmpimg;

	//qDebug()<<"copyRaw2QPixmap_Slice switch (cplane)"<<cplane;

	switch (cplane) //QImage(w,h)
	{
	case imgPlaneX: //(Z,Y)
		pp = (cpos>sz0)? sz0-1:cpos-1;   pp = (pp<0)? 0:pp;
		tmpimg = QImage(sz2, sz1, QImage::Format_RGB32);

		for (y=0; y<sz1; y++)
		for (z=0; z<sz2; z++)
			{
				for (int k=0; k<N; k++)
				{
					float c = p4d[k][z][y][pp];
					mC[k] =  (bIntensityRescale==false) ? c : floor((c-vmin[k])*vscale[k]);
				}
				//qDebug("(x) y z = (%d/%d) %d/%d %d/%d", pp,sz0, y,sz1, z,sz2);
				RGB8 o = lookup_mix(mC, luts, op);
				tmpimg.setPixel(z, y, qRgb(o.r, o.g, o.b));
			}
		break;

	case imgPlaneY: //(X,Z)
		pp = (cpos>sz1)? sz1-1:cpos-1;   pp = (pp<0)? 0:pp;
		tmpimg = QImage(sz0, sz2, QImage::Format_RGB32);

		for (z=0; z<sz2; z++)
		for (x=0; x<sz0; x++)
			{
				for (int k=0; k<N; k++)
				{
					float c = p4d[k][z][pp][x];
					mC[k] =  (bIntensityRescale==false) ? c : floor((c-vmin[k])*vscale[k]);
				}
				//qDebug("x (y) z = %d/%d (%d/%d) %d/%d", x,sz0, pp,sz1, z,sz2);
				RGB8 o = lookup_mix(mC, luts, op);
				tmpimg.setPixel(x, z, qRgb(o.r, o.g, o.b));
			}
		break;

	case imgPlaneZ: //(X,Y)
		pp = (cpos>sz2)? sz2-1:cpos-1;   pp = (pp<0)? 0:pp;
		tmpimg = QImage(sz0, sz1, QImage::Format_RGB32);

		for (y=0; y<sz1; y++)
		for (x=0; x<sz0; x++)
			{
				for (int k=0; k<N; k++)
				{
					float c = p4d[k][pp][y][x];
					mC[k] =  (bIntensityRescale==false) ? c : floor((c-vmin[k])*vscale[k]);
				}
				//qDebug("x y (z) = %d/%d %d/%d (%d/%d)", x,sz0, y,sz1, pp,sz2);
				RGB8 o = lookup_mix(mC, luts, op);
				tmpimg.setPixel(x, y, qRgb(o.r, o.g, o.b));
			}
		break;
	}

	//qDebug()<<"copyRaw2QPixmap_Slice fromImage(tmpimg)"<<tmpimg.size();

	return QPixmap::fromImage(tmpimg);
}


//////////////////////////////////////////////////////////////////////

void ChannelTable::create()
{
//  /////////////////////////////////////////////////////////
//  tabOptions = new QTabWidget(this); //tabOptions = new AutoTabWidget(this);//090117: commented by PHC
//	/////////////////////////////////////////////////////////
//
//	QGroupBox* tabAndBtnGroup = new QGroupBox();
//    QHBoxLayout *tabAndBtnLayout = new QHBoxLayout(tabAndBtnGroup);
//    tabAndBtnLayout->addWidget(tabOptions);
//    tabAndBtnLayout->addWidget(buttonGroup);
//
//	//overall layout
//    QVBoxLayout *allLayout = new QVBoxLayout(this);
//	allLayout->addWidget(tabAndBtnGroup);
//	allLayout->addWidget(searchGroup);
}

void ChannelTable::createTable()
{
//	table[stImageMarker]     = createTableMarker();
//	table[stLabelSurface]    = createTableSurf();
//	table[stNeuronStructure] = createTableSWC();
//	table[stPointCloud]      = createTableAPO();
//	table[stPointSet]     = createTableAPO_Set();
//
//	//==========================================================================
//	// for easy accessing tabs, addTab using the same order of V3dr_SurfaceType
//	//==========================================================================
//	if (tabOptions)
//	{  // insertTab according to the order of enum v3dr_SurfaceType
//		int i;
//		QString qs;
//		i= tabOptions->addTab(table[stImageMarker],		qs =QString("Marker (%1)").arg(table[stImageMarker]->rowCount()));
//		tabOptions->setTabToolTip(i, qs);
//		i= tabOptions->addTab(table[stLabelSurface],	qs =QString("Label Surface (%1)").arg(table[stLabelSurface]->rowCount()));
//		tabOptions->setTabToolTip(i, qs);
//		i= tabOptions->addTab(table[stNeuronStructure], qs =QString("Neuron/line Structure (%1)").arg(table[stNeuronStructure]->rowCount()));
//		tabOptions->setTabToolTip(i, qs);
//		i= tabOptions->addTab(table[stPointCloud],      qs =QString("Point Cloud (%1)").arg(table[stPointCloud]->rowCount()));
//		tabOptions->setTabToolTip(i, qs);
//		i= tabOptions->addTab(table[stPointSet],     qs =QString("Point Cloud Set (%1)").arg(table[stPointSet]->rowCount()));
//		tabOptions->setTabToolTip(i, qs);
//	}
//
//
//	//if (renderer)	connect(renderer, SIGNAL)
//	if (table[stImageMarker])      connect(table[stImageMarker], SIGNAL(cellChanged(int,int)), this, SLOT(pickMarker(int,int)));
//	if (table[stLabelSurface])     connect(table[stLabelSurface], SIGNAL(cellChanged(int,int)), this, SLOT(pickSurf(int,int)));
//	if (table[stNeuronStructure])  connect(table[stNeuronStructure], SIGNAL(cellChanged(int,int)), this, SLOT(pickSWC(int,int)));
//	if (table[stPointCloud])       connect(table[stPointCloud], SIGNAL(cellChanged(int,int)), this, SLOT(pickAPO(int,int)));
//	if (table[stPointSet])      connect(table[stPointSet], SIGNAL(cellChanged(int,int)), this, SLOT(pickAPO_Set(int,int)));
//
//	for (int i=1; i<=5; i++) if (table[i])
//	{
//		table[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
//	//		table[i]->setEditTriggers(//QAbstractItemView::CurrentChanged |
//	//				QAbstractItemView::DoubleClicked |
//	//				QAbstractItemView::SelectedClicked);                       //use doubleClickHandler() to override delay of popping dialog by the setEditTriggers
//
//		connect(table[i], SIGNAL(cellDoubleClicked(int,int)), this, SLOT(doubleClickHandler(int,int))); //to override delay of popping dialog by the setEditTriggers
//		connect(table[i], SIGNAL(cellPressed(int,int)), this, SLOT(pressedClickHandler(int,int)));      //to pop context menu
//
//	}
}

void ChannelTable::connectSignals(XFormWidget* form)
{

}

void ChannelTable::setItemEditor()
{
	//::setItemEditor();

	// turn off item editor
	QItemEditorFactory::setDefaultFactory( new QItemEditorFactory(*QItemEditorFactory::defaultFactory()) );
}