//-------------------------------------------------------------------------
//
// File:		DecomposeAffineC.h
// Desc:		Helper class to decompose a matrix into a affine parts.
//				Based on: "Polar Matrix Decomposition"
//				by Ken Shoemake, shoemake@graphics.cis.upenn.edu
//				in "Graphics Gems IV", Academic Press, 1994
// Author:		memon <memon@inside.org>
//
//-------------------------------------------------------------------------
//	Copyright (c) 2002 Moppi Productions. All Rights Reserved.
//  This file is part of Eskimo Fishing Trip game. For conditions of 
//  distribution and use, see the accompanying license.txt file.
//  http://moppi.inside.org/
//-------------------------------------------------------------------------

#ifndef __ESKIMO_DECOMPOSEAFFINEC_H__
#define __ESKIMO_DECOMPOSEAFFINEC_H__

#include "PajaTypes.h"
#include "Vector3C.h"
#include "QuatC.h"
#include "Matrix3C.h"

namespace PajaTypes {

	//! Decomposes a matrix into affine parts.
	/*!	Decomposes matrix into translation, rotation, scale and scael rotation
		parts.
		The class is based on the article:
		"Polar Matrix Decomposition"
		by Ken Shoemake, shoemake@graphics.cis.upenn.edu
		in "Graphics Gems IV", Academic Press, 1994

		To reassemble the matrix use following:
		\code
		Matrix3C	rScaleMat, rScaleRotMat, rInvScaleRotMat;
		Matrix3C	rPosMat, rRotMat, rTM;
		QuatC		rInvScaleRot;
		// scale
		rScaleMat.set_scale( rScale );
		// scale rot
		rScaleRot.from_axis_angle( rScaleAxis, f32ScaleAngle );
		rScaleRotMat.set_rot( rScaleRot );
		// inv scale rot
		rInvScaleRot = rScaleRot.unit_inverse();
		rInvScaleRotMat.set_rot( rInvScaleRot );
		// rot
		rRot.from_axis_angle( rRotAxis, f32RotAngle );
		rRotMat.set_rot( rRot );
		// pos
		rPosMat.set_trans( rPos );
		// Compose matrix
		rTM = rInvScaleRotMat * rScaleMat * rScaleRotMat * rRotMat * rPosMat;
		\endcode

		Or using OpenGL:

		\code
		float32		f32X, f32Y, f32Z, f32A;
		QuatC		rInvScaleRot = rScaleRot.unit_inverse();

		glTranslatef( rPos[0], rPos[1], rPos[2] );

		rRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
		glRotatef( f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );

		rScaleRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
		glRotatef( f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );

		glScalef( rScale[0], rScale[1], rScale[2] );

		rInvScaleRot.to_axis_angle( f32X, f32Y, f32Z, f32A );
		glRotatef( f32A / (float32)M_PI * 180.0f, f32X, f32Y, f32Z );
		\endcode
	*/
	class DecomposeAffineC
	{
	public:
		//! Constructor.
		DecomposeAffineC( const PajaTypes::Matrix3C& rMat );
		//! Desctructor.
		~DecomposeAffineC();

		//! Returns the translation part of the matrix.
		PajaTypes::Vector3C	get_translation();
		//! Return essential rotation part of the matrix.
		PajaTypes::QuatC	get_rotation();
		//! Return scale part of the matrix.
		PajaTypes::Vector3C	get_scale();
		//! Return scale rotation part of the matrix.
		PajaTypes::QuatC	get_scale_rotation();
		//! Returns sign of the determinant.
		PajaTypes::float32	get_sign();

	private:
		PajaTypes::Vector3C	m_rTrans;
		PajaTypes::QuatC	m_rRotation;
		PajaTypes::Vector3C	m_rScale;
		PajaTypes::QuatC	m_rScaleRot;
		PajaTypes::float32	m_f32Sign;
	};

};	// namespace

#endif	// __ESKIMO_DECOMPOSEAFFINEC_H__