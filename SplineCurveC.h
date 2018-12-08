//-------------------------------------------------------------------------
//
// File:		SplineCurveC.h
// Desc:		spline curve class.
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_SPLINECURVEC_H__
#define __ESKIMO_SPLINECURVEC_H__


#include "PajaTypes.h"
#include "Vector3C.h"
#include <vector>


// simple spline class to smooth the fishing string

class SplineCurveC
{
public:
	SplineCurveC( PajaTypes::uint32 ui32PtCount );
	~SplineCurveC();

	void				set_point( PajaTypes::uint32 ui32Idx, const PajaTypes::Vector3C& rPt );
	PajaTypes::uint32	get_point_count() const;

	void				prepare();

	PajaTypes::Vector3C	get_value( PajaTypes::float32 f32T );		// T = [0, 1]

protected:

	void	compute_hermite_basis( PajaTypes::float32 f32U, PajaTypes::float32* pV ) const;
	void	comp_first_deriv( PajaTypes::uint32 ui32CurIndex, PajaTypes::uint32 ui32NextIndex );
	void	comp_last_deriv( PajaTypes::uint32 ui32CurIndex, PajaTypes::uint32 ui32NextIndex );
	void	comp_2key_deriv( PajaTypes::uint32 ui32CurIndex, PajaTypes::uint32 ui32NextIndex );
	void	comp_middle_deriv( PajaTypes::uint32 ui32PrevIndex, PajaTypes::uint32 ui32Index, PajaTypes::uint32 ui32NextIndex );

	struct ControlPtS {
		PajaTypes::float32		m_f32T;
		PajaTypes::Vector3C	m_rPt;
		PajaTypes::Vector3C	m_rInTan;
		PajaTypes::Vector3C	m_rOutTan;
	};
	std::vector<ControlPtS>	m_rPoints;
};


#endif