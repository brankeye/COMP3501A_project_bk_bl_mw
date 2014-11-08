/*
SSSE.h
------

Created for: COMP3501A Assignment 6
Fall 2014, Carleton University

Author:
Bernard Llanos, ID: 100793648

Created November 7, 2014

Primary basis: InvariantParticlesRenderer.h

Description
  -An abstract Screen-Space Special Effect class
  -Creates one or more textures to use as render targets (at least one) and input data
  -Executes pipeline to process images
  -Assumes that the effect is to be applied
     to the first render target, and creates a shader resource
     view for the first render target during initialization.
  -The screen-space quad vertex buffer used by this class
     is immutable (derived classes could override this behaviour
	 if necessary).
*/

#pragma once

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Texture2DFromBytes.h"
#include "ConfigUser.h"
#include "FlatAtomicConfigIO.h"
#include "Shader.h"
#include "vertexTypes.h"

#define SSSE_VERTEX_TYPE SSSEVertexType
#define SSSE_NVERTICES 4

/* The following definitions are:
   -Key parameters used to retrieve configuration data
   -Default values used in the absence of configuration data
    or constructor/function arguments (where necessary)
*/

/* Shader constructor and configuration parameters */

#define SSSE_SHADER_ENABLELOGGING_FLAG_DEFAULT true
#define SSSE_SHADER_ENABLELOGGING_FLAG_FIELD L"enableLogging"

#define SSSE_SHADER_MSGPREFIX_DEFAULT L"Shader (Shader class)"
#define SSSE_SHADER_MSGPREFIX_FIELD L"msgPrefix"

#define SSSE_SHADER_SCOPE_DEFAULT L"shader"
#define SSSE_SHADER_SCOPE_FIELD L"scope"

#define SSSE_SHADER_SCOPE_LOGUSER_DEFAULT L"shader_LogUser"
#define SSSE_SHADER_SCOPE_LOGUSER_FIELD L"scope_LogUser"

#define SSSE_SHADER_SCOPE_CONFIGUSER_DEFAULT L"shader_ConfigUser"
#define SSSE_SHADER_SCOPE_CONFIGUSER_FIELD L"scope_ConfigUser"

#define SSSE_SHADER_CONFIGFILE_NAME_FIELD L"inputConfigFileName"
#define SSSE_SHADER_CONFIGFILE_PATH_FIELD L"inputConfigFilePath"

/* Texture constructor and configuration parameters */
#define SSSE_TEXTURE_ENABLELOGGING_FLAG_DEFAULT true
#define SSSE_TEXTURE_ENABLELOGGING_FLAG_FIELD L"enableLogging"

#define SSSE_TEXTURE_MSGPREFIX_DEFAULT L"Texture (Texture2DFromBytes class)"
#define SSSE_TEXTURE_MSGPREFIX_FIELD L"msgPrefix"

#define SSSE_TEXTURE_SCOPE_DEFAULT L"texture"
#define SSSE_TEXTURE_SCOPE_FIELD L"scope"

#define SSSE_TEXTURE_SCOPE_LOGUSER_DEFAULT L"texture_LogUser"
#define SSSE_TEXTURE_SCOPE_LOGUSER_FIELD L"scope_LogUser"

#define SSSE_TEXTURE_SCOPE_CONFIGUSER_DEFAULT L"texture_ConfigUser"
#define SSSE_TEXTURE_SCOPE_CONFIGUSER_FIELD L"scope_ConfigUser"

#define SSSE_TEXTURE_CONFIGFILE_NAME_FIELD L"inputConfigFileName"
#define SSSE_TEXTURE_CONFIGFILE_PATH_FIELD L"inputConfigFilePath"

// Type of loader to use for configuration data when creating textures
#define SSSE_CONFIGIO_CLASS_TEXTURE FlatAtomicConfigIO

// Type of loader to use for configuration data when creating textures
#define SSSE_CONFIGIO_CLASS_SHADER FlatAtomicConfigIO

class SSSE : public ConfigUser {

	// Client-visible constant buffer data
public:
	struct Globals {
		DirectX::XMFLOAT4 rect; // Area to process (top left u, top left v, width, height) [pixels]
		DirectX::XMFLOAT2 focus; // Point of interest (e.g. cursor) (u, v) [pixels]
		DirectX::XMFLOAT2 time; // (current time, update time interval) [milliseconds]
	};

	// Constant buffer structures
private:
	struct GlobalBufferType {
		Globals globals;
		DirectX::XMFLOAT2 screenSize; // Dimensions of full render target and textures (width, height) [pixels]
		DirectX::XMFLOAT2 padding;
	};

protected:

	/* Proxy for a ConfigUser constructor */
	template<typename ConfigIOClass> SSSE(
		ConfigIOClass* const optionalLoader,
		const Config* locationSource,
		const std::wstring filenameScope,
		const std::wstring filenameField,
		const std::wstring directoryScope = L"",
		const std::wstring directoryField = L""
		);

	/* Retrieves configuration data, using default values,
	   if possible, when configuration data is not found.
	   Calls the base class's configuration function
	   if there is a Config instance to use (which results
	   in a cascade of configuration function calls).

	   'scope' determines the scope used to find configuration data
	   for this class.

	   The 'configUserScope' and 'logUserScope' parameters
	   determine the scopes used to find configuration data for the
	   ConfigUser and LogUser base classes.
	   If 'logUserScope' is null, it will default to 'configUserScope'.
	   If 'configUserScope' is null, it will default to 'scope'.

	   The 'textureFieldPrefixes' parameter sets the prefixes
	   added to the fields used to load texture configuration data.
	   This function will configure each texture in 'm_textures',
	   so there should be the same number of prefixes as there are
	   elements of 'm_textures'.

	   Derived classes are expected to have constructed
	   textures before this function is called,
	   and have stored them in 'm_textures'.
	*/
	virtual HRESULT configure(const std::wstring& scope, const std::wstring* configUserScope, const std::wstring* logUserScope,
		const std::wstring* textureFieldPrefixes);

public:
	virtual ~SSSE(void);

	/* Creates pipeline resources needed for rendering (e.g. shaders)
	   Returns a failure code if resource creation fails.

	   'width' and 'height' determine the dimensions
	   of all elements of 'm_textures'.

	   The first texture is assumed to be used as both a
	   shader resource and a render target.
	*/
	virtual HRESULT initialize(ID3D11Device* const device, UINT width, UINT height);

	/* Sets the first render target of the pipeline
	   to the texture referred to by the first element of 'm_textures'.

	   A handle to the render target which gets unbound is saved so that
	   apply() can put it back.
	 */
	virtual HRESULT setRenderTarget(ID3D11DeviceContext* const context);

	/* Applies the SSSE to the rectangular area of the current
	   render target (assumed to be the first render target)
	   specified by 'rect'.

	   rect = (top left u, top left v, width, height) [pixels]

	   If setRenderTarget() has been called since
	   the last call to this function, the render target last
	   displaced by setRenderTarget() will be put back on the
	   output merger stage.
	 */
	virtual HRESULT apply(ID3D11DeviceContext* const context);

	/* Sets global effect parameters
	 */
	HRESULT setGlobals(Globals* globals);

	// Helper functions
protected:

	/* Creates the pipeline shaders
	   Also responsible for calling createInputLayout()
	 */
	virtual HRESULT createShaders(ID3D11Device* const);

	/* Creates the vertex input layout */
	virtual HRESULT createInputLayout(ID3D11Device* const);

	/* Creates constant buffer(s) */
	virtual HRESULT createConstantBuffers(ID3D11Device* const);

	/* Prepares pipeline state for rendering */
	virtual HRESULT setShaderParameters(ID3D11DeviceContext* const);

	/* Pipeline execution */
	void renderShader(ID3D11DeviceContext* const);

	/* Texture initialization
	   Initializes each element of 'm_textures'
	 */
	virtual HRESULT initializeTextures(ID3D11Device* const device);

	/* Performs texture-related pipeline configuration
	   Textures are bound to the pixel shader stage
	   at slots corresponding to one less than their indices
	   in 'm_textures'.

	   The first element of 'm_textures' is not bound
	   by this function. See setRenderTarget()
	 */
	virtual HRESULT setTexturesOnContext(ID3D11DeviceContext* const context);

	/* Initializes the screen-space quad vertex buffer.
	   Four vertices are expected, with D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP topology
	 */
	virtual HRESULT initializeVertexBuffer(ID3D11Device* const device,
		const SSSE_VERTEX_TYPE* const vertices);

	/* Performs vertex buffer pipeline
	   configuration
	*/
	virtual HRESULT setVerticesOnContext(ID3D11DeviceContext* const context);

	// Data members
private:
	std::vector<Texture2DFromBytes*> m_textures;

	/* A handle to the render target originally on the pipeline,
	   used to re-bind it later.
	 */
	ID3D11RenderTargetView* m_renderTargetView;

	Shader* m_vertexShader;
	Shader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer *m_vertexBuffer; // Screen-space quad
	const size_t m_vertexCount; // SSSE_NVERTICES
	ID3D11Buffer* m_globalBuffer;

	Globals* m_globals;

	/* True if configuration was completed successfully. */
	bool m_configured;

	// Currently not implemented - will cause linker errors if called
private:
	SSSE(const SSSE& other);
	SSSE& operator=(const SSSE& other);
};