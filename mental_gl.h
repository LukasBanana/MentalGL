/**
 * MentalGL (OpenGL debugging functions)
 * Published 2018 under the public domain
 * 
 * Authors:
 *  - Lukas Hermanns (Creator)
 */

#ifndef MENTAL_GL_H
#define MENTAL_GL_H


#include <stddef.h>


// Opaque string object used as result from all query and analyze functions.
typedef void* MGLString;

typedef struct MGLQueryFormatting
{
    char    separator;  // Character to separate the attributes from their values. By default ' '.
    size_t  distance;   // Distances (in number of characters) between the longest attribute name and the attribute values. By default 1.
}
MGLQueryFormatting;

/**
Returns a descriptive string with information about the entire render state of OpenGL.
Usage example:

// Include and implement MentalGL in a single source file
#define MENTAL_GL_IMPLEMENTATION
#include "mental_gl.h"

// Do OpenGL stuff ...

// Query current OpenGL state (ignore optional formatting descriptor)
MGLString s = mglQueryRenderState(NULL);

// Print result
printf("%s\n", mglGetString(s));

// Free opaque string object
mglFreeString(s);
*/
MGLString mglQueryRenderState(const MGLQueryFormatting* formatting);

// Enumeration of a couple of issues with the OpenGL render state the function 'pglAnalayzeIssues' can analyze.
enum MGLIssue
{
	MGLIssueVisibleColor,				// Analyze why no color is visible on framebuffer (or screen).
	MGLIssueVisibleColoredPrimitive,	// Analyze why no colored primite (point, line, triangle) is visible on framebuffer (or screen).
};

// Returns a descriptive string with information why the specified issue may occur.
MGLString mglAnalyzeIssues(enum MGLIssue issue);

// Returns the null-terminated string from the specified opaque object.
const char* mglGetString(MGLString s);

// Release all resources allocated by this library.
void mglFreeString(MGLString s);


#ifdef MENTAL_GL_IMPLEMENTATION


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif // /_MSC_VER


#ifdef _WIN32
#include <Windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <GL/GL.h>
#include <GL/glext.h>


// *****************************************************************
//      INTERNAL MACROS
// *****************************************************************

#define MGL_CASE2STR(NAME)                          case NAME: return #NAME

#define MGL_MIN(A, B)                               ((A) < (B) ? (A) : (B))
#define MGL_MAX(A, B)                               ((A) > (B) ? (A) : (B))

#ifndef MGL_MALLOC
#define MGL_MALLOC(TYPE)                            ((TYPE*)malloc(sizeof(TYPE)))
#endif

#ifndef MGL_CALLOC
#define MGL_CALLOC(TYPE, COUNT)                     ((TYPE*)calloc(COUNT, sizeof(TYPE)))
#endif

#ifndef MGL_FREE
#define MGL_FREE(OBJ)                               free(OBJ)
#endif

#define MGL_STRING_MIN_CAPACITY                     16

#define MGL_MAX_COMPRESSED_TEXTURE_FORMATS          64
#define MGL_MAX_PROGRAM_BINARY_FORMATS              64
#define MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS      64
#define MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS  64
#define MGL_MAX_UNIFORM_BUFFER_BINDINGS             64
#define MGL_MAX_VERTEX_BUFFER_BINDINGS              64
#define MGL_MAX_TEXTURE_LAYERS                      32


// *****************************************************************
//      INTERNAL STRUCTURES
// *****************************************************************

typedef struct MGLStringInternal
{
    size_t  cap; // capacity
    size_t  len; // length
    char*   buf; // buffer (incl. NUL char)
}
MGLStringInternal;

typedef struct MGLRenderStates
{
    GLint       iActiveTexture;                                                                 // GL_ACTIVE_TEXTURE
    GLfloat     fAliasedLineWidthRange[2];                                                      // GL_ALIASED_LINE_WIDTH_RANGE
    GLint       iArrayBufferBinding;                                                            // GL_ARRAY_BUFFER_BINDING
    GLboolean   bBlend;                                                                         // GL_BLEND
    GLfloat     fBlendColor[4];                                                                 // GL_BLEND_COLOR
    GLint       iBlendDstAlpha;                                                                 // GL_BLEND_DST_ALPHA
    GLint       iBlendDstRGB;                                                                   // GL_BLEND_DST_RGB
    GLint       iBlendEquationAlpha;                                                            // GL_BLEND_EQUATION_ALPHA
    GLint       iBlendEquationRGB;                                                              // GL_BLEND_EQUATION_RGB
    GLint       iBlendSrcAlpha;                                                                 // GL_BLEND_SRC_ALPHA
    GLint       iBlendSrcRGB;                                                                   // GL_BLEND_SRC_RGB
    GLfloat     fColorClearValue[4];                                                            // GL_COLOR_CLEAR_VALUE
    GLboolean   bColorLogicOp;                                                                  // GL_COLOR_LOGIC_OP
    GLboolean   bColorWriteMask[4];                                                             // GL_COLOR_WRITEMASK
    GLint       iCompressedTextureFormats[MGL_MAX_COMPRESSED_TEXTURE_FORMATS];                  // GL_COMPRESSED_TEXTURE_FORMATS
    GLint       iContextFlags;                                                                  // GL_CONTEXT_FLAGS
    GLboolean   bCullFace;                                                                      // GL_CULL_FACE
    GLint       iCullFaceMode;                                                                  // GL_CULL_FACE_MODE
    GLint       iCurrentProgram;                                                                // GL_CURRENT_PROGRAM
    GLint       iDebugGroupStackDepth;                                                          // GL_DEBUG_GROUP_STACK_DEPTH
    GLdouble    dDepthClearValue;                                                               // GL_DEPTH_CLEAR_VALUE
    GLint       iDepthFunc;                                                                     // GL_DEPTH_FUNC
    GLdouble    dDepthRange[2];                                                                 // GL_DEPTH_RANGE
    GLboolean   bDepthTest;                                                                     // GL_DEPTH_TEST
    GLboolean   bDepthWriteMask;                                                                // GL_DEPTH_WRITEMASK
    GLint       iDispatchIndirectBufferBinding;                                                 // GL_DISPATCH_INDIRECT_BUFFER_BINDING
    GLboolean   bDither;                                                                        // GL_DITHER
    GLboolean   bDoubleBuffer;                                                                  // GL_DOUBLEBUFFER
    GLint       iDrawBuffer;                                                                    // GL_DRAW_BUFFER
    GLint       iDrawBuffer_i[16];                                                              // GL_DRAW_BUFFER<i>
    GLint       iDrawFramebufferBinding;                                                        // GL_DRAW_FRAMEBUFFER_BINDING
    GLint       iElementArrayBufferBinding;                                                     // GL_ELEMENT_ARRAY_BUFFER_BINDING
    GLint       iFragmentShaderDerivativeHint;                                                  // GL_FRAGMENT_SHADER_DERIVATIVE_HINT
    GLint       iImplementationColorReadFormat;                                                 // GL_IMPLEMENTATION_COLOR_READ_FORMAT
    GLint       iImplementationColorReadType;                                                   // GL_IMPLEMENTATION_COLOR_READ_TYPE
    GLboolean   bLineSmooth;                                                                    // GL_LINE_SMOOTH
    GLint       iLineSmoothHint;                                                                // GL_LINE_SMOOTH_HINT
    GLfloat     fLineWidth;                                                                     // GL_LINE_WIDTH
    GLint       iLayerProvokingVertex;                                                          // GL_LAYER_PROVOKING_VERTEX
    GLint       iLogicOpMode;                                                                   // GL_LOGIC_OP_MODE
    GLint       iMajorVersion;                                                                  // GL_MAJOR_VERSION
    GLint       iMax3DTextureSize;                                                              // GL_MAX_3D_TEXTURE_SIZE
    GLint       iMaxArrayTextureLayers;                                                         // GL_MAX_ARRAY_TEXTURE_LAYERS
    GLint       iMaxClipDistances;                                                              // GL_MAX_CLIP_DISTANCES
    GLint       iMaxColorTextureSamples;                                                        // GL_MAX_COLOR_TEXTURE_SAMPLES
    GLint       iMaxCombinedAtomicCounters;                                                     // GL_MAX_COMBINED_ATOMIC_COUNTERS
    GLint       iMaxCombinedComputeUniformComponents;                                           // GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS
    GLint       iMaxCombinedFragmentUniformComponents;                                          // GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS
    GLint       iMaxCombinedGeometryUniformComponents;                                          // GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS
    GLint       iMaxCombinedShaderStorageBlocks;                                                // GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS
    GLint       iMaxCombinedTextureImageUnits;                                                  // GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    GLint       iMaxCombinedUniformBlocks;                                                      // GL_MAX_COMBINED_UNIFORM_BLOCKS
    GLint       iMaxCombinedVertexUniformComponents;                                            // GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS
    GLint       iMaxComputeAtomicCounters;                                                      // GL_MAX_COMPUTE_ATOMIC_COUNTERS
    GLint       iMaxComputeAtomicCounterBuffers;                                                // GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS
    GLint       iMaxComputeShaderStorageBlocks;                                                 // GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS
    GLint       iMaxComputeTextureImageUnits;                                                   // GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS
    GLint       iMaxComputeUniformBlocks;                                                       // GL_MAX_COMPUTE_UNIFORM_BLOCKS
    GLint       iMaxComputeUniformComponents;                                                   // GL_MAX_COMPUTE_UNIFORM_COMPONENTS
    GLint       iMaxComputeWorkGroupCount[3];                                                   // GL_MAX_COMPUTE_WORK_GROUP_COUNT
    GLint       iMaxComputeWorkGroup;                                                           // GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS
    GLint       iMaxComputeWorkGroupSize[3];                                                    // GL_MAX_COMPUTE_WORK_GROUP_SIZE
    GLint       iMaxCubeMapTextureSize;                                                         // GL_MAX_CUBE_MAP_TEXTURE_SIZE
    GLint       iMaxDebugGroupStackDepth;                                                       // GL_MAX_DEBUG_GROUP_STACK_DEPTH
    GLint       iMaxDepthTextureSamples;                                                        // GL_MAX_DEPTH_TEXTURE_SAMPLES
    GLint       iMaxDrawBuffers;                                                                // GL_MAX_DRAW_BUFFERS
    GLint       iMaxDualSourceDrawBuffers;                                                      // GL_MAX_DUAL_SOURCE_DRAW_BUFFERS
    GLint       iMaxElementIndex;                                                               // GL_MAX_ELEMENT_INDEX
    GLint       iMaxElementsIndices;                                                            // GL_MAX_ELEMENTS_INDICES
    GLint       iMaxElementsVertices;                                                           // GL_MAX_ELEMENTS_VERTICES
    GLint       iMaxFragmentAtomicCounters;                                                     // GL_MAX_FRAGMENT_ATOMIC_COUNTERS
    GLint       iMaxFragmentShaderStorageBlocks;                                                // GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS
    GLint       iMaxFragmentInputComponents;                                                    // GL_MAX_FRAGMENT_INPUT_COMPONENTS
    GLint       iMaxFragmentUniformComponents;                                                  // GL_MAX_FRAGMENT_UNIFORM_COMPONENTS
    GLint       iMaxFragmentUniformVectors;                                                     // GL_MAX_FRAGMENT_UNIFORM_VECTORS
    GLint       iMaxFragmentUniformBlocks;                                                      // GL_MAX_FRAGMENT_UNIFORM_BLOCKS
    GLint       iMaxFramebufferWidth;                                                           // GL_MAX_FRAMEBUFFER_WIDTH
    GLint       iMaxFramebufferHeight;                                                          // GL_MAX_FRAMEBUFFER_HEIGHT
    GLint       iMaxFramebufferLayers;                                                          // GL_MAX_FRAMEBUFFER_LAYERS
    GLint       iMaxFramebufferSamples;                                                         // GL_MAX_FRAMEBUFFER_SAMPLES
    GLint       iMaxGeometryAtomicCounters;                                                     // GL_MAX_GEOMETRY_ATOMIC_COUNTERS
    GLint       iMaxGeometryShaderStorageBlocks;                                                // GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS
    GLint       iMaxGeometryInputComponents;                                                    // GL_MAX_GEOMETRY_INPUT_COMPONENTS
    GLint       iMaxGeometryOutputComponents;                                                   // GL_MAX_GEOMETRY_OUTPUT_COMPONENTS
    GLint       iMaxGeometryTextureImageUnits;                                                  // GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS
    GLint       iMaxGeometryUniformBlocks;                                                      // GL_MAX_GEOMETRY_UNIFORM_BLOCKS
    GLint       iMaxGeometryUniformComponents;                                                  // GL_MAX_GEOMETRY_UNIFORM_COMPONENTS
    GLint       iMaxIntegerSamples;                                                             // GL_MAX_INTEGER_SAMPLES
    GLint       iMaxLabelLength;                                                                // GL_MAX_LABEL_LENGTH
    GLint       iMaxProgramTexelOffest;                                                         // GL_MAX_PROGRAM_TEXEL_OFFSET
    GLint       iMaxRectangleTextureSize;                                                       // GL_MAX_RECTANGLE_TEXTURE_SIZE
    GLint       iMaxRenderbufferSize;                                                           // GL_MAX_RENDERBUFFER_SIZE
    GLint       iMaxSampleMaskWords;                                                            // GL_MAX_SAMPLE_MASK_WORDS
    GLint       iMaxServerWaitTimeout;                                                          // GL_MAX_SERVER_WAIT_TIMEOUT
    GLint       iMaxShaderStorageBufferBindings;                                                // GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
    GLint       iMaxTessControlAtomicCounters;                                                  // GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS
    GLint       iMaxTessControlShaderStorageBlocks;                                             // GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS
    GLint       iMaxTessEvaluationAtomicCounters;                                               // GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS
    GLint       iMaxTessEvaluationShaderStorageBlocks;                                          // GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS
    GLint       iMaxTextureBufferSize;                                                          // GL_MAX_TEXTURE_BUFFER_SIZE
    GLint       iMaxTextureImageUnits;                                                          // GL_MAX_TEXTURE_IMAGE_UNITS
    GLfloat     fMaxTextureLODBias;                                                             // GL_MAX_TEXTURE_LOD_BIAS
    GLint       iMaxTextureSize;                                                                // GL_MAX_TEXTURE_SIZE
    GLint       iMaxUniformBufferBindings;                                                      // GL_MAX_UNIFORM_BUFFER_BINDINGS
    GLint       iMaxUniformBlockSize;                                                           // GL_MAX_UNIFORM_BLOCK_SIZE
    GLint       iMaxUniformLocations;                                                           // GL_MAX_UNIFORM_LOCATIONS
    GLint       iMaxVaryingComponents;                                                          // GL_MAX_VARYING_COMPONENTS
    GLint       iMaxVaryingVectors;                                                             // GL_MAX_VARYING_VECTORS
    GLint       iMaxVaryingFloats;                                                              // GL_MAX_VARYING_FLOATS
    GLint       iMaxVertexAtomicCounters;                                                       // GL_MAX_VERTEX_ATOMIC_COUNTERS
    GLint       iMaxVertexAttribRelativeOffset;                                                 // GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET
    GLint       iMaxVertexAttribBindings;                                                       // GL_MAX_VERTEX_ATTRIB_BINDINGS
    GLint       iMaxVertexAttribs;                                                              // GL_MAX_VERTEX_ATTRIBS
    GLint       iMaxVertexOutputComponents;                                                     // GL_MAX_VERTEX_OUTPUT_COMPONENTS
    GLint       iMaxVertexShaderStorageBlocks;                                                  // GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS
    GLint       iMaxVertexTextureImageUnits;                                                    // GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS
    GLint       iMaxVertexUniformBlocks;                                                        // GL_MAX_VERTEX_UNIFORM_BLOCKS
    GLint       iMaxVertexUniformComponents;                                                    // GL_MAX_VERTEX_UNIFORM_COMPONENTS
    GLint       iMaxVertexUniformVectors;                                                       // GL_MAX_VERTEX_UNIFORM_VECTORS
    GLint       iMaxViewportDims[2];                                                            // GL_MAX_VIEWPORT_DIMS
    GLint       iMaxViewports;                                                                  // GL_MAX_VIEWPORTS
    GLint       iMinMapBufferAlignment;                                                         // GL_MIN_MAP_BUFFER_ALIGNMENT
    GLint       iMinProgramTexelOffest;                                                         // GL_MIN_PROGRAM_TEXEL_OFFSET
    GLint       iMinorVersion;                                                                  // GL_MINOR_VERSION
    GLint       iNumCompressedTextureFormats;                                                   // GL_NUM_COMPRESSED_TEXTURE_FORMATS
    GLint       iNumExtensions;                                                                 // GL_NUM_EXTENSIONS
    GLint       iNumProgramBinaryFormats;                                                       // GL_NUM_PROGRAM_BINARY_FORMATS
    GLint       iNumShaderBinaryFormats;                                                        // GL_NUM_SHADER_BINARY_FORMATS
    GLint       iPackAlignment;                                                                 // GL_PACK_ALIGNMENT
    GLint       iPackImageHeight;                                                               // GL_PACK_IMAGE_HEIGHT
    GLboolean   bPackLSBFirst;                                                                  // GL_PACK_LSB_FIRST
    GLint       iPackRowLength;                                                                 // GL_PACK_ROW_LENGTH
    GLint       iPackSkipImages;                                                                // GL_PACK_SKIP_IMAGES
    GLint       iPackSkipPixels;                                                                // GL_PACK_SKIP_PIXELS
    GLint       iPackSkipRows;                                                                  // GL_PACK_SKIP_ROWS
    GLboolean   bPackSwapBytes;                                                                 // GL_PACK_SWAP_BYTES
    GLint       iPixelPackBufferBinding;                                                        // GL_PIXEL_PACK_BUFFER_BINDING
    GLint       iPixelUnpackBufferBinding;                                                      // GL_PIXEL_UNPACK_BUFFER_BINDING
    GLfloat     fPointFadeThresholdSize;                                                        // GL_POINT_FADE_THRESHOLD_SIZE
    GLint       iPrimitiveRestartIndex;                                                         // GL_PRIMITIVE_RESTART_INDEX
    GLint       iProgramBinaryFormats[MGL_MAX_PROGRAM_BINARY_FORMATS];                          // GL_PROGRAM_BINARY_FORMATS
    GLint       iProgramPipelineBinding;                                                        // GL_PROGRAM_PIPELINE_BINDING
    GLboolean   bProgramPointSize;                                                              // GL_PROGRAM_POINT_SIZE
    GLint       iProvokingVertex;                                                               // GL_PROVOKING_VERTEX
    GLfloat     fPointSize;                                                                     // GL_POINT_SIZE
    GLfloat     fPointSizeGranularity;                                                          // GL_POINT_SIZE_GRANULARITY
    GLfloat     fPointSizeRange[2];                                                             // GL_POINT_SIZE_RANGE
    GLfloat     fPolygonOffsetFactor;                                                           // GL_POLYGON_OFFSET_FACTOR
    GLfloat     fPolygonOffsetUnits;                                                            // GL_POLYGON_OFFSET_UNITS
    GLboolean   bPolygonOffsetFill;                                                             // GL_POLYGON_OFFSET_FILL
    GLboolean   bPolygonOffsetLine;                                                             // GL_POLYGON_OFFSET_LINE
    GLboolean   bPolygonOffsetPoint;                                                            // GL_POLYGON_OFFSET_POINT
    GLboolean   bPolygonSmooth;                                                                 // GL_POLYGON_SMOOTH
    GLint       iPolygonSmoothHint;                                                             // GL_POLYGON_SMOOTH_HINT
    GLint       iReadBuffer;                                                                    // GL_READ_BUFFER
    GLint       iReadFramebufferBinding;                                                        // GL_READ_FRAMEBUFFER_BINDING
    GLint       iRenderbufferBinding;                                                           // GL_RENDERBUFFER_BINDING
    GLint       iSampleBuffers;                                                                 // GL_SAMPLE_BUFFERS
    GLfloat     fSampleCoverageValue;                                                           // GL_SAMPLE_COVERAGE_VALUE
    GLboolean   bSampleCoverageInvert;                                                          // GL_SAMPLE_COVERAGE_INVERT
    GLint       iSamplerBinding;                                                                // GL_SAMPLER_BINDING
    GLint       iSamples;                                                                       // GL_SAMPLES
    GLint       iScissorBox[4];                                                                 // GL_SCISSOR_BOX
    GLboolean   bScissorTest;                                                                   // GL_SCISSOR_TEST
    GLboolean   bShaderCompiler;                                                                // GL_SHADER_COMPILER
    GLint       iShaderStorageBufferBinding[MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS];            // GL_SHADER_STORAGE_BUFFER_BINDING
    GLint       iShaderStorageBufferOffsetAlignment;                                            // GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT
    GLint64     iShaderStorageBufferSize[MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS];               // GL_SHADER_STORAGE_BUFFER_SIZE
    GLint64     iShaderStorageBufferStart[MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS];              // GL_SHADER_STORAGE_BUFFER_START
    GLfloat     fSmoothLineWidthRange[2];                                                       // GL_SMOOTH_LINE_WIDTH_RANGE
    GLfloat     fSmoothLineWidthGranularity;                                                    // GL_SMOOTH_LINE_WIDTH_GRANULARITY
    GLint       iStencilBackFail;                                                               // GL_STENCIL_BACK_FAIL
    GLint       iStencilBackFunc;                                                               // GL_STENCIL_BACK_FUNC
    GLint       iStencilBackPassDepthFail;                                                      // GL_STENCIL_BACK_PASS_DEPTH_FAIL
    GLint       iStencilBackPassDepthPass;                                                      // GL_STENCIL_BACK_PASS_DEPTH_PASS
    GLint       iStencilBackRef;                                                                // GL_STENCIL_BACK_REF
    GLint       iStencilBackValueMask;                                                          // GL_STENCIL_BACK_VALUE_MASK
    GLint       iStencilBackWriteMask;                                                          // GL_STENCIL_BACK_WRITEMASK
    GLint       iStencilClearValue;                                                             // GL_STENCIL_CLEAR_VALUE
    GLint       iStencilFail;                                                                   // GL_STENCIL_FAIL
    GLint       iStencilFunc;                                                                   // GL_STENCIL_FUNC
    GLint       iStencilPassDepthFail;                                                          // GL_STENCIL_PASS_DEPTH_FAIL
    GLint       iStencilPassDepthPass;                                                          // GL_STENCIL_PASS_DEPTH_PASS
    GLint       iStencilRef;                                                                    // GL_STENCIL_REF
    GLboolean   bStencilTest;                                                                   // GL_STENCIL_TEST
    GLint       iStencilValueMask;                                                              // GL_STENCIL_VALUE_MASK
    GLint       iStencilWriteMask;                                                              // GL_STENCIL_WRITEMASK
    GLboolean   bStereo;                                                                        // GL_STEREO
    GLint       iSubPixelBits;                                                                  // GL_SUBPIXEL_BITS
    GLint       iTextureBinding1D;                                                              // GL_TEXTURE_BINDING_1D
    GLint       iTextureBinding1DArray;                                                         // GL_TEXTURE_BINDING_1D_ARRAY
    GLint       iTextureBinding2D;                                                              // GL_TEXTURE_BINDING_2D
    GLint       iTextureBinding2DArray;                                                         // GL_TEXTURE_BINDING_2D_ARRAY
    GLint       iTextureBinding2DMultisample;                                                   // GL_TEXTURE_BINDING_2D_MULTISAMPLE
    GLint       iTextureBinding2DMultisampleArray;                                              // GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
    GLint       iTextureBinding3D;                                                              // GL_TEXTURE_BINDING_3D
    GLint       iTextureBindingBuffer;                                                          // GL_TEXTURE_BINDING_BUFFER
    GLint       iTextureBindingCubeMap;                                                         // GL_TEXTURE_BINDING_CUBE_MAP
    GLint       iTextureBindingRectangle;                                                       // GL_TEXTURE_BINDING_RECTANGLE
    GLint       iTextureCompressionHint;                                                        // GL_TEXTURE_COMPRESSION_HINT
    GLint       iTextureBufferOffsetAlignment;                                                  // GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT
    GLint64     iTimestamp;                                                                     // GL_TIMESTAMP
    GLint       iTransformFeedbackBufferBinding[MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS];    // GL_TRANSFORM_FEEDBACK_BUFFER_BINDING
    GLint64     iTransformFeedbackBufferSize[MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS];           // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    GLint64     iTransformFeedbackBufferStart[MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS];          // GL_TRANSFORM_FEEDBACK_BUFFER_START
    GLint       iUniformBufferBinding[MGL_MAX_UNIFORM_BUFFER_BINDINGS];                         // GL_UNIFORM_BUFFER_BINDING
    GLint       iUniformBufferOffsetAlignment;                                                  // GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT
    GLint64     iUniformBufferSize[MGL_MAX_UNIFORM_BUFFER_BINDINGS];                            // GL_UNIFORM_BUFFER_SIZE
    GLint64     iUniformBufferStart[MGL_MAX_UNIFORM_BUFFER_BINDINGS];                           // GL_UNIFORM_BUFFER_START
    GLint       iUnpackAlignment;                                                               // GL_UNPACK_ALIGNMENT
    GLint       iUnpackImageHeight;                                                             // GL_UNPACK_IMAGE_HEIGHT
    GLboolean   bUnpackLSBFirst;                                                                // GL_UNPACK_LSB_FIRST
    GLint       iUnpackRowLength;                                                               // GL_UNPACK_ROW_LENGTH
    GLint       iUnpackSkipImages;                                                              // GL_UNPACK_SKIP_IMAGES
    GLint       iUnpackSkipPixels;                                                              // GL_UNPACK_SKIP_PIXELS
    GLint       iUnpackSkipRows;                                                                // GL_UNPACK_SKIP_ROWS
    GLboolean   bUnpackSwapBytes;                                                               // GL_UNPACK_SWAP_BYTES
    GLint       iVertexArrayBinding;                                                            // GL_VERTEX_ARRAY_BINDING
    GLint       iVertexBindingDivisor[MGL_MAX_VERTEX_BUFFER_BINDINGS];                          // GL_VERTEX_BINDING_DIVISOR
    GLint       iVertexBindingOffset[MGL_MAX_VERTEX_BUFFER_BINDINGS];                           // GL_VERTEX_BINDING_OFFSET
    GLint       iVertexBindingStride[MGL_MAX_VERTEX_BUFFER_BINDINGS];                           // GL_VERTEX_BINDING_STRIDE
    GLint       iViewport[4];                                                                   // GL_VIEWPORT
    GLint       iViewportBoundsRange[2];                                                        // GL_VIEWPORT_BOUNDS_RANGE
    GLint       iViewportIndexProvokingVertex;                                                  // GL_VIEWPORT_INDEX_PROVOKING_VERTEX
    GLint       iViewportSubPixelBits;                                                          // GL_VIEWPORT_SUBPIXEL_BITS
}
MGLRenderStates;

typedef struct MGLBindingPoints
{
    GLint iTextureBinding1D[MGL_MAX_TEXTURE_LAYERS];                    // GL_TEXTURE_BINDING_1D
    GLint iTextureBinding1DArray[MGL_MAX_TEXTURE_LAYERS];               // GL_TEXTURE_BINDING_1D_ARRAY
    GLint iTextureBinding2D[MGL_MAX_TEXTURE_LAYERS];                    // GL_TEXTURE_BINDING_2D
    GLint iTextureBinding2DArray[MGL_MAX_TEXTURE_LAYERS];               // GL_TEXTURE_BINDING_2D_ARRAY
    GLint iTextureBinding2DMultisample[MGL_MAX_TEXTURE_LAYERS];         // GL_TEXTURE_BINDING_2D_MULTISAMPLE
    GLint iTextureBinding2DMultisampleArray[MGL_MAX_TEXTURE_LAYERS];    // GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
    GLint iTextureBinding3D[MGL_MAX_TEXTURE_LAYERS];                    // GL_TEXTURE_BINDING_3D
    GLint iTextureBindingBuffer[MGL_MAX_TEXTURE_LAYERS];                // GL_TEXTURE_BINDING_BUFFER
    GLint iTextureBindingCubeMap[MGL_MAX_TEXTURE_LAYERS];               // GL_TEXTURE_BINDING_CUBE_MAP
    GLint iTextureBindingRectangle[MGL_MAX_TEXTURE_LAYERS];             // GL_TEXTURE_BINDING_RECTANGLE
}
MGLBindingPoints;


// *****************************************************************
//      INTERNAL FUNCTIONS
// *****************************************************************

static MGLStringInternal* mglStringInternalAlloc()
{
    MGLStringInternal* s = MGL_MALLOC(MGLStringInternal);
    {
        s->cap      = MGL_STRING_MIN_CAPACITY;
        s->len      = 0;
        s->buf      = MGL_CALLOC(char, s->cap);
        s->buf[0]   = '\0';
    }
    return s;
}

static MGLStringInternal* mglStringInternalAllocFrom(const char* val)
{
    if (val)
    {
        MGLStringInternal* s = MGL_MALLOC(MGLStringInternal);
        {
            size_t len = strlen(val);
            s->cap          = MGL_MAX(len + 1, MGL_STRING_MIN_CAPACITY);
            s->len          = len;
            s->buf          = MGL_CALLOC(char, s->cap);
            memcpy(s->buf, val, s->len);
            s->buf[s->len]  = '\0';
        }
        return s;
    }
}

static void mglStringInternalFree(MGLStringInternal* s)
{
    if (s && s->buf)
    {
        MGL_FREE(s->buf);
        MGL_FREE(s);
    }
}

static MGLStringInternal* mglStringInternalCopy(const MGLStringInternal* src)
{
    if (src)
    {
        MGLStringInternal* s = MGL_MALLOC(MGLStringInternal);
        {
            s->cap = src->cap;
            s->len = src->len;
            s->buf = MGL_CALLOC(char, s->cap);
            memcpy(s->buf, src->buf, s->len + 1);
        }
        return s;
    }
    return NULL;
}

static void mglStringInternalResize(MGLStringInternal* s, size_t len, char chr)
{
    if (s)
    {
        if (len + 1 > s->cap)
        {
            // Allocate new string buffer and copy previous string into new buffer
            size_t new_cap  = MGL_MAX(len + 1, s->cap * 2);
            char* new_buf   = MGL_CALLOC(char, new_cap);
            memcpy(new_buf, s->buf, s->len);

            // Initialize new characters
            if (chr != 0)
            {
                for (size_t i = s->len; i < len; ++i)
                    new_buf[i] = chr;
            }
            new_buf[len] = '\0';

            // Replace previous string
            MGL_FREE(s->buf);
            s->cap = new_cap;
            s->len = len;
            s->buf = new_buf;
        }
        else if (len < s->len)
        {
            if (s->cap > MGL_STRING_MIN_CAPACITY && len < s->cap / 2)
            {
                // Allocate new string buffer and copy previous partial string into new buffer
                size_t new_cap  = MGL_MAX(MGL_STRING_MIN_CAPACITY, s->cap / 2);
                char* new_buf   = MGL_CALLOC(char, new_cap);
                memcpy(new_buf, s->buf, len);
                new_buf[len] = '\0';

                // Replace previous string
                MGL_FREE(s->buf);
                s->cap = new_cap;
                s->len = len;
                s->buf = new_buf;
            }
            else
            {
                // Set new null terminator
                s->buf[len] = '\0';
            }
        }
        else if (len > s->len)
        {
            // Initialize new characters
            if (chr != 0)
            {
                for (size_t i = s->len; i < len; ++i)
                    s->buf[i] = chr;
            }
            s->buf[len] = '\0';
            s->len = len;
        }
    }
}

static void mglStringInternalAppend(MGLStringInternal* s, const MGLStringInternal* appendix)
{
    if (s && appendix && appendix->len > 0)
    {
        // Resize string and copy appendix into new area
        size_t lhs_len = s->len;
        mglStringInternalResize(s, lhs_len + appendix->len, 0);
        memcpy(s->buf + lhs_len, appendix->buf, appendix->len);
    }
}

static void mglStringInternalAppendCStr(MGLStringInternal* s, const char* appendix)
{
    if (s && appendix && *appendix != '\0')
    {
        // Resize string and copy appendix into new area
        size_t lhs_len = s->len;
        size_t rhs_len = strlen(appendix);
        mglStringInternalResize(s, lhs_len + rhs_len, 0);
        memcpy(s->buf + lhs_len, appendix, rhs_len);
    }
}

static void mglEnumToHex(char* s, unsigned val)
{
	sprintf(s, "0x%*X", 8, val);
    for (s += 2; *s == ' '; ++s)
        *s = '0';
}

static const char* mglTextureTargetStr(GLenum target)
{
	switch (target)
	{
        #ifdef GL_TEXTURE_1D
        MGL_CASE2STR( GL_TEXTURE_1D );
        #endif // /GL_TEXTURE_1D
        
        #ifdef GL_TEXTURE_2D
        MGL_CASE2STR( GL_TEXTURE_2D );
        #endif // /GL_TEXTURE_2D
        
        #ifdef GL_TEXTURE_3D
        MGL_CASE2STR( GL_TEXTURE_3D );
        #endif // /GL_TEXTURE_3D
        
        #ifdef GL_TEXTURE_1D_ARRAY
        MGL_CASE2STR( GL_TEXTURE_1D_ARRAY );
        #endif // /GL_TEXTURE_1D_ARRAY
        
        #ifdef GL_TEXTURE_2D_ARRAY
        MGL_CASE2STR( GL_TEXTURE_2D_ARRAY );
        #endif // /GL_TEXTURE_2D_ARRAY
        
        #ifdef GL_TEXTURE_RECTANGLE
        MGL_CASE2STR( GL_TEXTURE_RECTANGLE );
        #endif // /GL_TEXTURE_RECTANGLE
        
        #ifdef GL_TEXTURE_CUBE_MAP
        MGL_CASE2STR( GL_TEXTURE_CUBE_MAP );
        #endif // /GL_TEXTURE_CUBE_MAP
        
        #ifdef GL_TEXTURE_CUBE_MAP_ARRAY
        MGL_CASE2STR( GL_TEXTURE_CUBE_MAP_ARRAY );
        #endif // /GL_TEXTURE_CUBE_MAP_ARRAY
        
        #ifdef GL_TEXTURE_BUFFER
        MGL_CASE2STR( GL_TEXTURE_BUFFER );
        #endif // /GL_TEXTURE_BUFFER
        
        #ifdef GL_TEXTURE_2D_MULTISAMPLE
        MGL_CASE2STR( GL_TEXTURE_2D_MULTISAMPLE );
        #endif // /GL_TEXTURE_2D_MULTISAMPLE

        #ifdef GL_TEXTURE_2D_MULTISAMPLE_ARRAY
        MGL_CASE2STR( GL_TEXTURE_2D_MULTISAMPLE_ARRAY );
        #endif // /GL_TEXTURE_2D_MULTISAMPLE_ARRAY
	}
	return NULL;
}


// *****************************************************************
//      UNDEF INTERNAL MACROS
// *****************************************************************

#undef PGL_CASE2STR
#undef MGL_MIN
#undef MGL_MAX
#undef MGL_MALLOC
#undef MGL_CALLOC
#undef MGL_FREE
#undef MGL_STRING_MIN_CAPACITY
#undef MGL_MAX_COMPRESSED_TEXTURE_FORMATS
#undef MGL_MAX_PROGRAM_BINARY_FORMATS
#undef MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS
#undef MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS
#undef MGL_MAX_UNIFORM_BUFFER_BINDINGS
#undef MGL_MAX_VERTEX_BUFFER_BINDINGS
#undef MGL_MAX_TEXTURE_LAYERS

#ifdef _MSC_VER
#pragma warning(pop)
#endif // /_MSC_VER


// *****************************************************************
//      GLOBAL FUNCTIONS
// *****************************************************************

MGLString mglQueryRenderState(const MGLQueryFormatting* formatting)
{
    static MGLQueryFormatting formattingDefault = { ' ', 1 };

    MGLStringInternal* s = mglStringInternalAlloc();

    if (formatting == NULL)
        formatting = (&formattingDefault);

    glActiveTexture(GL_TEXTURE0);
    //glGetIntegerv(GL_TEXTURE_BINDING_2D, 

    char h[100] = { 0 };
    mglEnumToHex(h, GL_TEXTURE_2D);
    printf("%s\n", h);


    return (MGLString)s;
}

MGLString mglAnalyzeIssues(enum MGLIssue issue)
{
    MGLStringInternal* s = mglStringInternalAlloc();


    return (MGLString)s;
}

const char* mglGetString(MGLString s)
{
    return ((MGLStringInternal*)s)->buf;
}

void mglFreeString(MGLString s)
{
    mglStringInternalFree((MGLStringInternal*)s);
}


#endif


#endif // /MENTAL_GL_H
