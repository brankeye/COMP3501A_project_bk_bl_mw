/*
ShipModel.h
--------------------

Created for: COMP3501A Assignment 5
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 31, 2014
Adapted for COMP3501A Project on October 31, 2014

Primary basis: GridQuadTextured.h
Secondary basis: A2Cylinder.h (COMP3501A Assignment 2 - Bernard Llanos)

Description
-A model of the player ship.
*/

#pragma once

#include <DirectXMath.h>
#include "Transformable.h"
#include "PropellerTransformable.h"
#include "CubeModel.h"
#include "SphereModel.h"
#include "LogUser.h"
#include "../../oct_tree.h"
#include "../../ObjectModel.h"
#include "IGeometry.h"
#include <string>
#include <vector>

// Default log message prefix used before more information is available
#define SHIPMODEL_START_MSG_PREFIX L"ShipModel "

/* The following definitions are:
-Key parameters used to retrieve configuration data
-Default values used in the absence of configuration data
or constructor/function arguments (where necessary)
*/

#define SHIPMODEL_SCOPE L"ShipModel"

/* LogUser and ConfigUser configuration parameters
Refer to LogUser.h and ConfigUser.h
*/
#define SHIPMODEL_LOGUSER_SCOPE		L"ShipModel_LogUser"
#define SHIPMODEL_CONFIGUSER_SCOPE	L"ShipModel_ConfigUser"

class ShipModel : public IGeometry, public LogUser
{
	// Initialization and destruction
public:

	ShipModel();
	virtual ~ShipModel(void);

	//HRESULT spawn(Octtree*);
	HRESULT initialize(ID3D11Device* d3dDevice, vector<Transformable*>* bones);
	vector<Transformable*>* m_bones;

	virtual HRESULT drawUsingAppropriateRenderer(
		ID3D11DeviceContext* const context,
		GeometryRendererManager& manager,
		const Camera* const camera
		) override;

	virtual HRESULT setTransformables(const std::vector<Transformable*>* const transforms) override;
	virtual XMFLOAT3 getPosition() override;
	virtual float getRadius() override;
private:
	CubeModel* body;
	CubeModel* leftWing;
	CubeModel* rightWing;
	SphereModel* capsule;
	CubeModel* leftPropellerA;
	CubeModel* leftPropellerB;
	CubeModel* rightPropellerA;
	CubeModel* rightPropellerB;

	ShipModel(const ShipModel& other);
	ShipModel& operator=(const ShipModel& other);
};