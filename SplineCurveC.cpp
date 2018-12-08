//-------------------------------------------------------------------------
//
// File:		SplineCurveC.cpp
// Desc:		spline curve class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#include "PajaTypes.h"
#include "Vector3C.h"
#include "SplineCurveC.h"


using namespace PajaTypes;


SplineCurveC::SplineCurveC( uint32 ui32PtCount )
{
	m_rPoints.resize( ui32PtCount );
	for( uint32 i = 0; i < m_rPoints.size(); i++ ) {
		m_rPoints[i].m_f32T = (float32)i / (float32)((int32)m_rPoints.size() - 1);
	}
}

SplineCurveC::~SplineCurveC()
{
	// empty
}

void
SplineCurveC::set_point( uint32 ui32Idx, const Vector3C& rPt )
{
	m_rPoints[ui32Idx].m_rPt = rPt;
}

uint32
SplineCurveC::get_point_count() const
{
	return m_rPoints.size();
}


Vector3C
SplineCurveC::get_value( float32 f32T )
{
	if( m_rPoints.size() == 0 )
		return Vector3C( 0, 0, 0 );

	if( m_rPoints.size() == 1 )
		return m_rPoints[0].m_rPt;

	float32	f32StartTime = 0.0f;
	float32	f32EndTime = 1.0f;

	// Check if the value is out of range

	// Is the requested time before the first key?
	if( f32T < f32StartTime )
		return m_rPoints[0].m_rPt;

	// Is the requested time after the last key?
	if( f32T >= f32EndTime )
		return m_rPoints[m_rPoints.size() - 1].m_rPt;

	// interpolate
	for( uint32 i = 1; i < m_rPoints.size(); i++ ) {
		if( m_rPoints[i].m_f32T > f32T ) {

			float32	f32U = (f32T - m_rPoints[i - 1].m_f32T) / (m_rPoints[i].m_f32T - m_rPoints[i - 1].m_f32T);

			Vector3C	rCurVal = m_rPoints[i].m_rPt;
			Vector3C	rPrevVal = m_rPoints[i - 1].m_rPt;

			// A Smooth segment
			Vector3C	rCurInTan = m_rPoints[i].m_rInTan;
			Vector3C	rPrevOutTan = m_rPoints[i - 1].m_rOutTan;

			float32	f32V[4];
			compute_hermite_basis( f32U, f32V );

			return rPrevVal * f32V[0] + rCurVal * f32V[1] + rPrevOutTan * f32V[2] + rCurInTan * f32V[3];
		}
	}

	return Vector3C( 0, 0, 0 );
}


void
SplineCurveC::compute_hermite_basis( float32 f32U, float32* pV ) const
{
	float32	f32U2, f32U3, f32A;
	
	f32U2 = f32U * f32U;
	f32U3 = f32U2 * f32U;
	f32A  = 2.0f * f32U3 - 3.0f * f32U2;
	pV[0] = 1.0f + f32A;
	pV[1] = -f32A;
	pV[2] = f32U - 2.0f * f32U2 + f32U3;
	pV[3] = -f32U2 + f32U3;
}

void
SplineCurveC::comp_first_deriv( uint32 ui32CurIndex, uint32 ui32NextIndex )
{
	Vector3C	rValNext = m_rPoints[ui32NextIndex].m_rPt;
	Vector3C	rValCur = m_rPoints[ui32CurIndex].m_rPt;
	Vector3C	rInTanNext = m_rPoints[ui32NextIndex].m_rInTan;

	m_rPoints[ui32CurIndex].m_rOutTan =  0.5f * (3.0f * (rValNext - rValCur) - rInTanNext);
}

void
SplineCurveC::comp_last_deriv( uint32 ui32CurIndex, uint32 ui32NextIndex )
{
	Vector3C	rValNext = m_rPoints[ui32NextIndex].m_rPt;
	Vector3C	rValCur = m_rPoints[ui32CurIndex].m_rPt;
	Vector3C	rOutTanCur = m_rPoints[ui32CurIndex].m_rOutTan;

	m_rPoints[ui32NextIndex].m_rOutTan = -0.5f * (3.0f * (rValCur - rValNext) + rOutTanCur);
}

void
SplineCurveC::comp_2key_deriv( uint32 ui32CurIndex, uint32 ui32NextIndex )
{
	m_rPoints[ui32CurIndex].m_rOutTan = (m_rPoints[ui32NextIndex].m_rPt - m_rPoints[ui32CurIndex].m_rPt);
	m_rPoints[ui32NextIndex].m_rInTan = (m_rPoints[ui32NextIndex].m_rPt - m_rPoints[ui32CurIndex].m_rPt);
}

void
SplineCurveC::comp_middle_deriv( uint32 ui32PrevIndex, uint32 ui32Index, uint32 ui32NextIndex )
{
	float32	dt, fp, fn;

	float32	f32PrevTime = m_rPoints[ui32PrevIndex].m_f32T;
	float32	f32Time = m_rPoints[ui32Index].m_f32T;
	float32	f32NextTime = m_rPoints[ui32NextIndex].m_f32T;

	// fp,fn apply speed correction when continuity is 0.0
	dt = 0.5f * (f32NextTime - f32PrevTime);
	fp = (f32Time - f32PrevTime) / dt;
	fn = (f32NextTime - f32Time) / dt;
	
	Vector3C	delm = m_rPoints[ui32Index].m_rPt - m_rPoints[ui32PrevIndex].m_rPt;
	Vector3C	delp = m_rPoints[ui32NextIndex].m_rPt - m_rPoints[ui32Index].m_rPt;
	m_rPoints[ui32Index].m_rInTan = (delm + delp) * 0.5f * fp;
	m_rPoints[ui32Index].m_rOutTan = (delm + delp) * 0.5f * fn;
}


void
SplineCurveC::prepare()
{
	if( m_rPoints.size() < 2 )
		return;

	if( m_rPoints.size() == 2 ) {
		comp_2key_deriv( 0, 1 );
	}
	else {
		// comp middle derivates
		for( uint32 i = 1; i < m_rPoints.size() - 1; i++ )
			comp_middle_deriv( i - 1, i, i + 1 );
		comp_first_deriv( 0, 1 );
		comp_last_deriv( m_rPoints.size() - 2, m_rPoints.size() - 1 );
	}
}