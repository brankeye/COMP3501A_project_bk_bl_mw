/*
Texture.h
---------

Created for: COMP3501A Project
Fall 2014, Carleton University

Authors:
Brandon Keyes, ID: 100897196
Bernard Llanos, ID: 100793648
Mark Wilkes, ID: 100884169

Created October 18, 2014

Primary basis: Chuck Walbourn's version of the MSDN Direct3D 11 Tutorial 7,
  available at: https://code.msdn.microsoft.com/Direct3D-Tutorial-Win32-829979ef

Description
  -A class encapsulating
    -Texture sampler state creation,
    -Binding textures and sampler states to the pipeline.
  -Initialization data is expected to come from a configuration file
  -Derived classes are responsible for the actual texture creation
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "ConfigUser.h"

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

// See http://msdn.microsoft.com/en-us/library/windows/desktop/ff476207%28v=vs.85%29.aspx
#define TEXTURE_SAMPLER_FILTER_DEFAULT 		D3D11_FILTER_MIN_MAG_MIP_LINEAR
#define TEXTURE_SAMPLER_FILTER_FIELD		L"filter"
#define TEXTURE_SAMPLER_ADDRESSU_DEFAULT 	D3D11_TEXTURE_ADDRESS_CLAMP
#define TEXTURE_SAMPLER_ADDRESSU_FIELD		L"addressU"
#define TEXTURE_SAMPLER_ADDRESSV_DEFAULT 	D3D11_TEXTURE_ADDRESS_CLAMP
#define TEXTURE_SAMPLER_ADDRESSV_FIELD		L"addressV"
#define TEXTURE_SAMPLER_ADDRESSW_DEFAULT 	D3D11_TEXTURE_ADDRESS_CLAMP
#define TEXTURE_SAMPLER_ADDRESSW_FIELD		L"addressW"
#define TEXTURE_SAMPLER_MAXANISOTROPY_DEFAULT 	1
#define TEXTURE_SAMPLER_MAXANISOTROPY_FIELD		L"maxAnisotropy"
#define TEXTURE_SAMPLER_COMPAREFUNC_DEFAULT 	D3D11_COMPARISON_NEVER
#define TEXTURE_SAMPLER_COMPAREFUNC_FIELD		L"comparisonFunc"
#define TEXTURE_SAMPLER_BORDERCOLOR_DEFAULT 	XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define TEXTURE_SAMPLER_BORDERCOLOR_FIELD		L"comparisonFunc"

/* LOD parameters are unlikely to be configured
   to non-default values
 */
#define TEXTURE_SAMPLER_MINLOD_DEFAULT			-FLT_MAX
#define TEXTURE_SAMPLER_MAXLOD_DEFAULT			FLT_MAX
#define TEXTURE_SAMPLER_MIPMAPLODBIAS_DEFAULT	0.0f

class Texture : public ConfigUser {

public:

	/* The possible pipeline shader stages
	   to which sampler states and textures
	   can be bound.
	*/
	enum class BindLocation : unsigned int {
		VS, // Vertex shader
		// HS, // Hull shader - Direct3D 11
		// DS, // Domain shader - Direct3D 11
		GS, // Geometry shader
		PS, // Pixel shader
		// CS // Compute shader - Direct3D 11
	};

public:

	template<typename ConfigIOClass> Texture(
		const bool enableLogging, const std::wstring& msgPrefix,
		ConfigIOClass* const optionalLoader,
		const std::wstring filename,
		const std::wstring path = L""
		);

	virtual ~Texture(void);

protected:
	/* Retrieves configuration data, using default values,
	   if possible, when configuration data is not found.
	   Calls the base class's configuration function
	   if there is a Config instance to use (which results
	   in a cascade of configuration function calls).

	   'scope' will override the scope used to find configuration data
	   for this class.

	   The 'configUserScope' and 'logUserScope' parameters
	   will override the scopes used to find configuration data for the
	   ConfigUser and LogUser base classes.
	   If 'logUserScope' is null, it will default to 'configUserScope'.
	   If 'configUserScope' is null, it will default to 'scope'.

	   This function must be called by derived classes;
	   it is not called by the Texture class.
	 */
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope = 0, const std::wstring* logUserScope = 0);

public:

	/* Creates texture sampler */
	virtual HRESULT initialize(ID3D11Device* device);

	/* Bind the texture and sampler to the pipeline
	   for use during rendering. The shader stage
	   to which the texture and sampler will be bound
	   is indicated by the 'bindLocation' parameter.
	*/
	virtual HRESULT bind(ID3D11DeviceContext* const context,
		const UINT textureSlot, const UINT samplerSlot,
		const BindLocation bindLocation);

	// Data members to be setup by derived classes
protected:
	ID3D11Resource* m_texture;
	ID3D11ShaderResourceView* m_textureView;

	// Data members set up by this class
private:
	D3D11_SAMPLER_DESC* m_samplerDesc; // Deleted when the sampler state is created
	ID3D11SamplerState* m_samplerState;

	// Currently not implemented - will cause linker errors if called
private:
	Texture(const Texture& other);
	Texture& operator=(const Texture& other);
};

template<typename ConfigIOClass> Texture::Texture(
	const bool enableLogging, const std::wstring& msgPrefix,
	ConfigIOClass* const optionalLoader,
	const std::wstring filename,
	const std::wstring path
	) :
	ConfigUser(
	enableLogging,
	msgPrefix,
	optionalLoader,
	filename,
	path
	),
	m_texture(0), m_textureView(0), m_samplerDesc(0), m_samplerState(0)
{}