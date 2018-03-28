/**
 * MentalGL (OpenGL debugging functions)
 * Published 2018 under the public domain
 * 
 * Authors:
 *  - Lukas Hermanns (Creator)
 *
 * Version History:
 *  - v1.00a:
 *      First release
 *      Main functions: mglQueryRenderState, mglAnalyzeIssues
 */

#ifndef MENTAL_GL_H
#define MENTAL_GL_H


#include <stddef.h>


// Query formatting order.
enum MGLFormattingOrder
{
    MGLFormattingOrderDefault,
    MGLFormattingOrderSorted,
};

// Opaque string object used as result from all query and analyze functions.
typedef void* MGLString;

// Query formatting descriptor structure.
typedef struct MGLQueryFormatting
{
    char                    separator;      // Character to separate the attributes from their values. By default ' '.
    size_t                  distance;       // Distances (in number of characters) between the longest attribute name and the attribute values. By default 1.
    enum MGLFormattingOrder order;          // Specifies the formatting order (if sorted for instance). By default MGLFormattingOrderDefault.
    int                     enable_hex;     // Specifies whether unknown enumerations shall be printed as hex codes (if != 0). By default 1.
    const char*             filter;         // Optional filter to only output parameters which contain this string. By default NULL.
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
#include <stdlib.h>
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

#define MGL_MAX_NUM_RENDER_STATES                   256


// *****************************************************************
//      INTERNAL FUNCTION OBJECTS
// *****************************************************************

typedef const char* (*MGLEnumToStringProc)(GLenum);


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

typedef struct MGLStringPairArray
{
    MGLStringInternal*  first;
    MGLStringInternal*  second;
    size_t              index;
}
MGLStringPairArray;

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

static void mglStringInternalReset(MGLStringInternal* s)
{
    if (s)
    {
        s->cap = 0;
        s->len = 0;
        s->buf = NULL;
    }
}

// Allocates a new internal string object with the specified initial capacity (minimum is 'MGL_STRING_MIN_CAPACITY')
static void mglStringInternalInit(MGLStringInternal* s, size_t init_cap)
{
    if (s)
    {
        s->cap      = MGL_MAX(MGL_STRING_MIN_CAPACITY, init_cap);
        s->len      = 0;
        s->buf      = MGL_CALLOC(char, s->cap);
        s->buf[0]   = '\0';
    }
}

// Allocates a new internal string object and initializes the string buffer with the specified null terminated string
static void mglStringInternalInitWith(MGLStringInternal* s, const char* val)
{
    if (val)
    {
        size_t len = strlen(val);
        s->cap          = MGL_MAX(MGL_STRING_MIN_CAPACITY, len + 1);
        s->len          = len;
        s->buf          = MGL_CALLOC(char, s->cap);
        memcpy(s->buf, val, s->len);
        s->buf[s->len]  = '\0';
    }
    else
        mglStringInternalInit(s, 0);
}

// Releases the specified internal string object
static void mglStringInternalFree(MGLStringInternal* s)
{
    if (s && s->buf)
        MGL_FREE(s->buf);
}

// Allocates enough space for the specified capacity (if its larger than the current string capacity)
static void mglStringInternalReserve(MGLStringInternal* s, size_t cap)
{
    if (s && cap > s->cap)
    {
        // Allocate new string buffer and copy previous string into new buffer
        char* new_buf = MGL_CALLOC(char, cap);
        memcpy(new_buf, s->buf, s->len);
        new_buf[s->len] = '\0';

        // Replace previous string
        MGL_FREE(s->buf);
        s->cap = cap;
        s->buf = new_buf;
    }
}

// Copies the specified internal string object
static void mglStringInternalCopy(MGLStringInternal* dst, const MGLStringInternal* src)
{
    if (dst && src)
    {
        dst->cap = src->cap;
        dst->len = src->len;
        dst->buf = MGL_CALLOC(char, dst->cap);
        memcpy(dst->buf, src->buf, dst->len + 1);
    }
}

// Resizes the string to the specified length and fills the new characters with 'chr'
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

// Appends the specified appendix to the internal string object
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

// Appends the specified appendix sub-string (with offset and length) to the internal string object
static void mglStringInternalAppendSub(MGLStringInternal* s, const MGLStringInternal* appendix, size_t off, size_t len)
{
    if (s && appendix && appendix->len > off)
    {
        // Resize string and copy appendix into new area
        size_t lhs_len = s->len;
        size_t rhs_len = MGL_MIN(len, appendix->len - off);
        mglStringInternalResize(s, lhs_len + rhs_len, 0);
        memcpy(s->buf + lhs_len, appendix->buf + off, rhs_len);
    }
}

// Appends the specified appendix to the internal string object
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

static void mglGetIntegers(const GLenum* pnames, GLint* data, GLuint count)
{
    for (GLuint i = 0; i < count; ++i)
        glGetIntegerv(pnames[i], &(data[i]));
}

static void mglGetIntegerDynamicArray(GLenum pname, GLint* data, GLuint count, GLuint limit)
{
    if (count > limit)
    {
        // Allocate temporary container if the array is too small
        GLint* tmp_data = (GLint*)MGL_CALLOC(GLint, count);
        glGetIntegerv(pname, tmp_data);
        memcpy(data, tmp_data, sizeof(GLint)*limit);
        free(tmp_data);
    }
    else
        glGetIntegerv(pname, data);
}

static void mglGetIntegerStaticArray(GLenum pname, GLint* data, GLuint count)
{
    for (GLuint i = 0; i < count; ++i)
        glGetIntegeri_v(pname, i, &(data[i]));
}

static void mglGetInteger64StaticArray(GLenum pname, GLint64* data, GLuint count)
{
    for (GLuint i = 0; i < count; ++i)
        glGetInteger64i_v(pname, i, &(data[i]));
}

static void mglEnumToHex(char* s, unsigned val)
{
	sprintf(s, "0x%*X", 8, val);
    for (s += 2; *s == ' '; ++s)
        *s = '0';
}

static void mglNextHeadline(MGLStringPairArray* str_array, const char* headline)
{
    mglStringInternalInitWith(&(str_array->first[str_array->index]), headline);
    mglStringInternalReset(&(str_array->second[str_array->index]));
    ++(str_array->index);
}

static void mglNextParamString(MGLStringPairArray* str_array, const char* par, const char* val)
{
    mglStringInternalInitWith(&(str_array->first[str_array->index]), par);
    mglStringInternalInitWith(&(str_array->second[str_array->index]), val);
    ++(str_array->index);
}

static void mglNextParamInteger(MGLStringPairArray* str_array, const char* par, GLint val)
{
    char s_val[16];
    sprintf(s_val, "%i", val);
    mglNextParamString(str_array, par, s_val);
}

static void mglNextParamUInteger(MGLStringPairArray* str_array, const char* par, GLuint val)
{
    char s_val[16];
    sprintf(s_val, "%u", val);
    mglNextParamString(str_array, par, s_val);
}

static void mglNextParamIntegerHex(MGLStringPairArray* str_array, const char* par, GLint val)
{
    char s_hex[11];
    mglEnumToHex(s_hex, (unsigned)val);
    mglNextParamString(str_array, par, s_hex);
}

static void mglNextParamBoolean(MGLStringPairArray* str_array, const char* par, GLboolean val)
{
    mglNextParamString(str_array, par, (val ? "GL_TRUE" : "GL_FALSE"));
}

static void mglNextParamEnum(MGLStringPairArray* str_array, const char* par, GLint val, MGLEnumToStringProc proc)
{
    const char* s_val = proc(val);
    if (!s_val)
    {
        char s_hex[11];
        mglEnumToHex(s_hex, (unsigned)val);
        mglNextParamString(str_array, par, s_hex);
    }
    else
        mglNextParamString(str_array, par, s_val);
}

static void mglNextParamInteger64(MGLStringPairArray* str_array, const char* par, GLint64 val)
{
    long long int val_ll = (long long int)val;
    char s_val[32];
    sprintf(s_val, "%lli", val_ll);
    mglNextParamString(str_array, par, s_val);
}

static void mglNextParamFloat(MGLStringPairArray* str_array, const char* par, GLfloat val)
{
    double val_d = (double)val;
    char s_val[64];
    sprintf(s_val, "%f", val_d);
    mglNextParamString(str_array, par, s_val);
}

static void mglNextParamDouble(MGLStringPairArray* str_array, const char* par, GLdouble val)
{
    char s_val[64];
    sprintf(s_val, "%f", val);
    mglNextParamString(str_array, par, s_val);
}

static void mglNextParamIntegerArray(MGLStringPairArray* str_array, const char* par, const GLint* val, size_t count, size_t limit, int to_hex)
{
    char s_val[16];

    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    mglStringInternalAppendCStr(out_val, "{ ");

    size_t n = MGL_MIN(count, limit);

    for (size_t i = 0; i < n; ++i)
    {
        if (to_hex)
            mglEnumToHex(s_val, (unsigned)val[i]);
        else
            sprintf(s_val, "%i", val[i]);

        mglStringInternalAppendCStr(out_val, s_val);

        if (i + 1 < n)
            mglStringInternalAppendCStr(out_val, ", ");
    }

    if (count > limit)
        mglStringInternalAppendCStr(out_val, ", ... ");

    mglStringInternalAppendCStr(out_val, " }");

    ++(str_array->index);
}

static void mglNextParamBitfield(MGLStringPairArray* str_array, const char* par, GLbitfield val, size_t count, MGLEnumToStringProc proc)
{
    size_t num_fields = 0;

    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    for (size_t i = 0; i < count; ++i)
    {
        GLenum flag = (1 << i);
        if ((val & flag) != 0)
        {
            if (num_fields > 0)
                mglStringInternalAppendCStr(out_val, " | ");
            ++num_fields;

            const char* s_val = proc(flag);
            if (!s_val)
            {
                char s_hex[11];
                mglEnumToHex(s_hex, (unsigned)val);
                mglStringInternalAppendCStr(out_val, s_hex);
            }
            else
                mglStringInternalAppendCStr(out_val, s_val);
        }
    }

    if (num_fields == 0)
        mglStringInternalAppendCStr(out_val, "0");

    ++(str_array->index);
}

static void mglNextParamFloatArray(MGLStringPairArray* str_array, const char* par, const GLfloat* val, size_t count)
{
    double val_d;
    char s_val[64];

    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    mglStringInternalAppendCStr(out_val, "{ ");

    for (size_t i = 0; i < count; ++i)
    {
        val_d = (double)val[i];
        sprintf(s_val, "%f", val_d);
        mglStringInternalAppendCStr(out_val, s_val);

        if (i + 1 < count)
            mglStringInternalAppendCStr(out_val, ", ");
    }

    mglStringInternalAppendCStr(out_val, " }");

    ++(str_array->index);
}


// *****************************************************************
//      CONVERSION FUNCTIONS
// *****************************************************************

static const char* mglHintModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_FASTEST );
        MGL_CASE2STR( GL_NICEST );
        MGL_CASE2STR( GL_DONT_CARE );
    }
    return NULL;
}

#ifdef GL_VERSION_1_4

static const char* mglBlendFuncStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_ZERO );
        MGL_CASE2STR( GL_ONE );
        MGL_CASE2STR( GL_SRC_COLOR );
        MGL_CASE2STR( GL_ONE_MINUS_SRC_COLOR );
        MGL_CASE2STR( GL_DST_COLOR );
        MGL_CASE2STR( GL_ONE_MINUS_DST_COLOR );
        MGL_CASE2STR( GL_SRC_ALPHA );
        MGL_CASE2STR( GL_ONE_MINUS_SRC_ALPHA );
        MGL_CASE2STR( GL_DST_ALPHA );
        MGL_CASE2STR( GL_ONE_MINUS_DST_ALPHA );
        MGL_CASE2STR( GL_CONSTANT_COLOR );
        MGL_CASE2STR( GL_ONE_MINUS_CONSTANT_COLOR );
        MGL_CASE2STR( GL_CONSTANT_ALPHA );
        MGL_CASE2STR( GL_ONE_MINUS_CONSTANT_ALPHA );
        MGL_CASE2STR( GL_SRC_ALPHA_SATURATE );
        MGL_CASE2STR( GL_SRC1_COLOR );
        MGL_CASE2STR( GL_ONE_MINUS_SRC1_COLOR );
        MGL_CASE2STR( GL_SRC1_ALPHA );
        MGL_CASE2STR( GL_ONE_MINUS_SRC1_ALPHA );
    }
    return NULL;
}

#endif // /GL_VERSION_1_4

#ifdef GL_VERSION_2_0

static const char* mglBlendEquationModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_FUNC_ADD );
        MGL_CASE2STR( GL_FUNC_SUBTRACT );
        MGL_CASE2STR( GL_FUNC_REVERSE_SUBTRACT );
        MGL_CASE2STR( GL_MIN );
        MGL_CASE2STR( GL_MAX );
    }
    return NULL;
}

static const char* mglDrawBufferModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_NONE );
        MGL_CASE2STR( GL_FRONT_LEFT );
        MGL_CASE2STR( GL_FRONT_RIGHT );
        MGL_CASE2STR( GL_BACK_LEFT );
        MGL_CASE2STR( GL_BACK_RIGHT );
        MGL_CASE2STR( GL_FRONT );
        MGL_CASE2STR( GL_BACK );
        MGL_CASE2STR( GL_LEFT );
        MGL_CASE2STR( GL_RIGHT );
        MGL_CASE2STR( GL_FRONT_AND_BACK );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT0 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT1 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT2 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT3 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT4 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT5 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT6 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT7 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT8 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT9 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT10 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT11 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT12 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT13 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT14 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT15 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT16 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT17 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT18 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT19 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT20 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT21 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT22 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT23 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT24 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT25 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT26 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT27 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT28 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT29 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT30 );
        MGL_CASE2STR( GL_COLOR_ATTACHMENT31 );
    }
    return NULL;
}

static const char* mglStencilOpStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_KEEP );
        MGL_CASE2STR( GL_ZERO );
        MGL_CASE2STR( GL_REPLACE );
        MGL_CASE2STR( GL_INCR );
        MGL_CASE2STR( GL_INCR_WRAP );
        MGL_CASE2STR( GL_DECR );
        MGL_CASE2STR( GL_DECR_WRAP );
        MGL_CASE2STR( GL_INVERT );
    }
    return NULL;
}

static const char* mglCompareFuncStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_NEVER );
        MGL_CASE2STR( GL_LESS );
        MGL_CASE2STR( GL_LEQUAL );
        MGL_CASE2STR( GL_GREATER );
        MGL_CASE2STR( GL_GEQUAL );
        MGL_CASE2STR( GL_EQUAL );
        MGL_CASE2STR( GL_NOTEQUAL );
        MGL_CASE2STR( GL_ALWAYS );
    }
    return NULL;
}

static const char* mglProvokingVertexModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_UNDEFINED_VERTEX );
        MGL_CASE2STR( GL_FIRST_VERTEX_CONVENTION );
        MGL_CASE2STR( GL_LAST_VERTEX_CONVENTION );
        MGL_CASE2STR( GL_PROVOKING_VERTEX );
    }
    return NULL;
}

#endif // /GL_VERSION_2_0

#ifdef GL_VERSION_3_0

static const char* mglContextFlagBitStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT );
        #ifdef GL_VERSION_4_3
        MGL_CASE2STR( GL_CONTEXT_FLAG_DEBUG_BIT );
        #endif
        #ifdef GL_VERSION_4_5
        MGL_CASE2STR( GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT );
        #endif
    }
    return NULL;
}

#endif

#ifdef GL_VERSION_4_1

static const char* mglImplementationColorReadFormatStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_STENCIL_INDEX );
        MGL_CASE2STR( GL_DEPTH_COMPONENT );
        MGL_CASE2STR( GL_DEPTH_STENCIL );
        MGL_CASE2STR( GL_RED );
        MGL_CASE2STR( GL_GREEN );
        MGL_CASE2STR( GL_BLUE );
        MGL_CASE2STR( GL_RGB );
        MGL_CASE2STR( GL_BGR );
        MGL_CASE2STR( GL_RGBA );
        MGL_CASE2STR( GL_BGRA );
    }
    return NULL;
}

static const char* mglImplementationColorReadTypeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_UNSIGNED_BYTE );
        MGL_CASE2STR( GL_BYTE );
        MGL_CASE2STR( GL_UNSIGNED_SHORT );
        MGL_CASE2STR( GL_SHORT );
        MGL_CASE2STR( GL_UNSIGNED_INT );
        MGL_CASE2STR( GL_INT );
        MGL_CASE2STR( GL_HALF_FLOAT );
        MGL_CASE2STR( GL_FLOAT );
        MGL_CASE2STR( GL_UNSIGNED_BYTE_3_3_2 );
        MGL_CASE2STR( GL_UNSIGNED_BYTE_2_3_3_REV );
        MGL_CASE2STR( GL_UNSIGNED_SHORT_5_6_5 );
        MGL_CASE2STR( GL_UNSIGNED_SHORT_5_6_5_REV );
        MGL_CASE2STR( GL_UNSIGNED_SHORT_4_4_4_4 );
        MGL_CASE2STR( GL_UNSIGNED_SHORT_4_4_4_4_REV );
        MGL_CASE2STR( GL_UNSIGNED_SHORT_5_5_5_1 );
        MGL_CASE2STR( GL_UNSIGNED_SHORT_1_5_5_5_REV );
        MGL_CASE2STR( GL_UNSIGNED_INT_8_8_8_8 );
        MGL_CASE2STR( GL_UNSIGNED_INT_8_8_8_8_REV );
        MGL_CASE2STR( GL_UNSIGNED_INT_10_10_10_2 );
        MGL_CASE2STR( GL_UNSIGNED_INT_2_10_10_10_REV );
        MGL_CASE2STR( GL_UNSIGNED_INT_24_8 );
        MGL_CASE2STR( GL_UNSIGNED_INT_10F_11F_11F_REV );
        MGL_CASE2STR( GL_UNSIGNED_INT_5_9_9_9_REV );
        MGL_CASE2STR( GL_FLOAT_32_UNSIGNED_INT_24_8_REV );
    }
    return NULL;
}

#endif // /GL_VERSION_4_1

static void mglGetGLRenderStates(MGLRenderStates* rs)
{
    static const GLenum g_drawBufferPnames[16] = { GL_DRAW_BUFFER0,  GL_DRAW_BUFFER1,  GL_DRAW_BUFFER2,  GL_DRAW_BUFFER3,
                                                   GL_DRAW_BUFFER4,  GL_DRAW_BUFFER5,  GL_DRAW_BUFFER6,  GL_DRAW_BUFFER7,
                                                   GL_DRAW_BUFFER8,  GL_DRAW_BUFFER9,  GL_DRAW_BUFFER10, GL_DRAW_BUFFER11,
                                                   GL_DRAW_BUFFER12, GL_DRAW_BUFFER13, GL_DRAW_BUFFER14, GL_DRAW_BUFFER15 };

    // Get last GL error
    GLenum last_err = glGetError();

    // Query all OpenGL states
    memset(rs, 0, sizeof(MGLRenderStates));

    // *****************************************************************
    //      GL_VERSION_1_0
    // *****************************************************************

    glGetIntegerv(GL_MAJOR_VERSION, &(rs->iMajorVersion));
    glGetIntegerv(GL_MINOR_VERSION, &(rs->iMinorVersion));

    glGetBooleanv(GL_BLEND, &(rs->bBlend));
    glGetFloatv(GL_COLOR_CLEAR_VALUE, &(rs->fColorClearValue[0]));
    glGetBooleanv(GL_COLOR_WRITEMASK, &(rs->bColorWriteMask[0]));
    glGetBooleanv(GL_CULL_FACE, &(rs->bCullFace));
    glGetIntegerv(GL_CULL_FACE_MODE, &(rs->iCullFaceMode));
    glGetDoublev(GL_DEPTH_CLEAR_VALUE, &(rs->dDepthClearValue));
    glGetIntegerv(GL_DEPTH_FUNC, &(rs->iDepthFunc));
    glGetDoublev(GL_DEPTH_RANGE, &(rs->dDepthRange[0]));
    glGetBooleanv(GL_DEPTH_TEST, &(rs->bDepthTest));
    glGetBooleanv(GL_DEPTH_WRITEMASK, &(rs->bDepthWriteMask));
    glGetBooleanv(GL_DITHER, &(rs->bDither));
    glGetBooleanv(GL_DOUBLEBUFFER, &(rs->bDoubleBuffer));
    glGetIntegerv(GL_DRAW_BUFFER, &(rs->iDrawBuffer));
    glGetBooleanv(GL_LINE_SMOOTH, &(rs->bLineSmooth));
    glGetIntegerv(GL_LINE_SMOOTH_HINT, &(rs->iLineSmoothHint));
    glGetFloatv(GL_LINE_WIDTH, &(rs->fLineWidth));
    glGetIntegerv(GL_LOGIC_OP_MODE, &(rs->iLogicOpMode));

    #define MGL_VERSION(MAJOR, MINOR)       (((MAJOR) << 16) | (MINOR))
    #define MGL_VERSION_MIN(MAJOR, MINOR)   (MGL_VERSION(rs->iMajorVersion, rs->iMinorVersion) >= MGL_VERSION(MAJOR, MINOR))

    // *****************************************************************
    //      GL_VERSION_1_1
    // *****************************************************************

    #ifdef GL_VERSION_1_1
    if (MGL_VERSION_MIN(1, 1))
    {
        glGetBooleanv(GL_COLOR_LOGIC_OP, &(rs->bColorLogicOp));
        glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &(rs->fPolygonOffsetFactor));
        glGetFloatv(GL_POLYGON_OFFSET_UNITS, &(rs->fPolygonOffsetUnits));
        glGetBooleanv(GL_POLYGON_OFFSET_FILL, &(rs->bPolygonOffsetFill));
        glGetBooleanv(GL_POLYGON_OFFSET_LINE, &(rs->bPolygonOffsetLine));
        glGetBooleanv(GL_POLYGON_OFFSET_POINT, &(rs->bPolygonOffsetPoint));
    }
    #endif // /GL_VERSION_1_1

    // *****************************************************************
    //      GL_VERSION_1_2
    // *****************************************************************

    #ifdef GL_VERSION_1_2
    if (MGL_VERSION_MIN(1, 2))
    {
        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &(rs->fAliasedLineWidthRange[0]));
        glGetFloatv(GL_BLEND_COLOR, &(rs->fBlendColor[0]));
        glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &(rs->iMax3DTextureSize));
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &(rs->iMaxElementsIndices));
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &(rs->iMaxElementsVertices));
        glGetIntegerv(GL_PACK_IMAGE_HEIGHT, &(rs->iPackImageHeight));
        glGetIntegerv(GL_PACK_SKIP_IMAGES, &(rs->iPackSkipImages));
        glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, &(rs->fSmoothLineWidthRange[0]));
        glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &(rs->fSmoothLineWidthGranularity));
        glGetIntegerv(GL_TEXTURE_BINDING_3D, &(rs->iTextureBinding3D));
        glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT, &(rs->iUnpackImageHeight));
        glGetIntegerv(GL_UNPACK_SKIP_IMAGES, &(rs->iUnpackSkipImages));
    }
    #endif // /GL_VERSION_1_2

    // *****************************************************************
    //      GL_VERSION_1_3
    // *****************************************************************

    #ifdef GL_VERSION_1_3
    if (MGL_VERSION_MIN(1, 3))
    {
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &(rs->iNumCompressedTextureFormats));
        mglGetIntegerDynamicArray(GL_COMPRESSED_TEXTURE_FORMATS, &(rs->iCompressedTextureFormats[0]), rs->iNumCompressedTextureFormats, MGL_MAX_COMPRESSED_TEXTURE_FORMATS);
        glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &(rs->iTextureBindingCubeMap));
        glGetIntegerv(GL_TEXTURE_COMPRESSION_HINT, &(rs->iTextureCompressionHint));
        glGetIntegerv(GL_ACTIVE_TEXTURE, &(rs->iActiveTexture));
        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &(rs->iMaxCubeMapTextureSize));
    }
    #endif // /GL_VERSION_1_3

    // *****************************************************************
    //      GL_VERSION_1_4
    // *****************************************************************

    #ifdef GL_VERSION_1_4
    if (MGL_VERSION_MIN(1, 4))
    {
        glGetIntegerv(GL_BLEND_DST_ALPHA, &(rs->iBlendDstAlpha));
        glGetIntegerv(GL_BLEND_DST_RGB, &(rs->iBlendDstRGB));
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &(rs->iBlendSrcAlpha));
        glGetIntegerv(GL_BLEND_SRC_RGB, &(rs->iBlendSrcRGB));
        glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &(rs->fMaxTextureLODBias));
        glGetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, &(rs->fPointFadeThresholdSize));
    }
    #endif // /GL_VERSION_1_4

    // *****************************************************************
    //      GL_VERSION_1_5
    // *****************************************************************

    #ifdef GL_VERSION_1_5
    if (MGL_VERSION_MIN(1, 5))
    {
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &(rs->iArrayBufferBinding));
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &(rs->iElementArrayBufferBinding));
    }
    #endif // /GL_VERSION_1_5

    // *****************************************************************
    //      GL_VERSION_1_5
    // *****************************************************************

    #ifdef GL_VERSION_2_0
    if (MGL_VERSION_MIN(2, 0))
    {
        glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &(rs->iBlendEquationAlpha));
        glGetIntegerv(GL_BLEND_EQUATION_RGB, &(rs->iBlendEquationRGB));
        glGetIntegerv(GL_CURRENT_PROGRAM, &(rs->iCurrentProgram));
        mglGetIntegers(g_drawBufferPnames, &(rs->iDrawBuffer_i[0]), 16);
        glGetIntegerv(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, &(rs->iFragmentShaderDerivativeHint));
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &(rs->iMaxCombinedTextureImageUnits));
        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &(rs->iMaxDrawBuffers));
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &(rs->iMaxFragmentUniformComponents));
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &(rs->iMaxTextureImageUnits));
        glGetIntegerv(GL_MAX_VARYING_FLOATS, &(rs->iMaxVaryingFloats));
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &(rs->iMaxVertexAttribs));
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &(rs->iMaxVertexTextureImageUnits));
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &(rs->iMaxVertexUniformComponents));
        glGetIntegerv(GL_STENCIL_BACK_FAIL, &(rs->iStencilBackFail));
        glGetIntegerv(GL_STENCIL_BACK_FUNC, &(rs->iStencilBackFunc));
        glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &(rs->iStencilBackPassDepthFail));
        glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &(rs->iStencilBackPassDepthPass));
        glGetIntegerv(GL_STENCIL_BACK_REF, &(rs->iStencilBackRef));
        glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &(rs->iStencilBackValueMask));
        glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &(rs->iStencilBackWriteMask));
    }
    #endif // /GL_VERSION_2_0

    // *****************************************************************
    //      GL_VERSION_2_1
    // *****************************************************************

    #ifdef GL_VERSION_2_1
    if (MGL_VERSION_MIN(2, 1))
    {
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &(rs->iPixelPackBufferBinding));
        glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &(rs->iPixelUnpackBufferBinding));
    }
    #endif // /GL_VERSION_2_1

    // *****************************************************************
    //      GL_VERSION_3_0
    // *****************************************************************

    #ifdef GL_VERSION_3_0
    if (MGL_VERSION_MIN(3, 0))
    {
        glGetIntegerv(GL_CONTEXT_FLAGS, &(rs->iContextFlags));
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &(rs->iDrawFramebufferBinding));
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &(rs->iMaxArrayTextureLayers));
        glGetIntegerv(GL_MAX_CLIP_DISTANCES, &(rs->iMaxClipDistances));
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &(rs->iMaxRenderbufferSize));
        glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &(rs->iMaxVaryingComponents));
    }
    #endif // /GL_VERSION_3_0

    // *****************************************************************
    //      GL_VERSION_3_1
    // *****************************************************************

    #ifdef GL_VERSION_3_1
    if (MGL_VERSION_MIN(3, 1))
    {
        glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &(rs->iMaxCombinedFragmentUniformComponents));
        glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, &(rs->iMaxCombinedGeometryUniformComponents));
        glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &(rs->iMaxCombinedVertexUniformComponents));
        glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &(rs->iMaxCombinedUniformBlocks));
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &(rs->iMaxFragmentUniformBlocks));
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &(rs->iMaxGeometryUniformBlocks));
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &(rs->iMaxVertexUniformBlocks));
    }
    #endif // /GL_VERSION_3_1

    // *****************************************************************
    //      GL_VERSION_3_2
    // *****************************************************************

    #ifdef GL_VERSION_3_2
    if (MGL_VERSION_MIN(3, 2))
    {
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &(rs->iMaxColorTextureSamples));
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &(rs->iMaxDepthTextureSamples));
        glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &(rs->iMaxIntegerSamples));
        glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &(rs->iMaxGeometryInputComponents));
        glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &(rs->iMaxGeometryOutputComponents));
        glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &(rs->iMaxGeometryTextureImageUnits));
        glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &(rs->iMaxGeometryUniformComponents));
        glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &(rs->iMaxFragmentInputComponents));
        glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &(rs->iMaxVertexOutputComponents));
    }
    #endif // /GL_VERSION_3_2

    // *****************************************************************
    //      GL_VERSION_3_3
    // *****************************************************************

    #ifdef GL_VERSION_3_3
    if (MGL_VERSION_MIN(3, 3))
    {
        glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &(rs->iMaxDualSourceDrawBuffers));
    }
    #endif // /GL_VERSION_3_3

    // *****************************************************************
    //      GL_VERSION_4_0
    // *****************************************************************

    #ifdef GL_VERSION_4_0
    if (MGL_VERSION_MIN(4, 0))
    {
        //TODO...
    }
    #endif // /GL_VERSION_4_0

    // *****************************************************************
    //      GL_VERSION_4_1
    // *****************************************************************

    #ifdef GL_VERSION_4_1
    if (MGL_VERSION_MIN(4, 1))
    {
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &(rs->iImplementationColorReadFormat));
        glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &(rs->iImplementationColorReadType));
        glGetIntegerv(GL_LAYER_PROVOKING_VERTEX, &(rs->iLayerProvokingVertex));
        glGetIntegerv(GL_MAX_VARYING_VECTORS, &(rs->iMaxVaryingVectors));
        glGetIntegerv(GL_MAX_VIEWPORTS, &(rs->iMaxViewports));
        glGetIntegerv(GL_VIEWPORT_BOUNDS_RANGE, &(rs->iViewportBoundsRange[0]));
        glGetIntegerv(GL_VIEWPORT_INDEX_PROVOKING_VERTEX, &(rs->iViewportIndexProvokingVertex));
        glGetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS, &(rs->iViewportSubPixelBits));
        glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &(rs->iMaxFragmentUniformVectors));
    }
    #endif // /GL_VERSION_4_1

    // *****************************************************************
    //      GL_VERSION_4_2
    // *****************************************************************

    #ifdef GL_VERSION_4_2
    if (MGL_VERSION_MIN(4, 2))
    {
        glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &(rs->iMaxCombinedAtomicCounters));
        glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &(rs->iMaxVertexAtomicCounters));
        glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, &(rs->iMaxTessControlAtomicCounters));
        glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, &(rs->iMaxTessEvaluationAtomicCounters));
        glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, &(rs->iMaxGeometryAtomicCounters));
        glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &(rs->iMaxFragmentAtomicCounters));
        glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &(rs->iMinMapBufferAlignment));
    }
    #endif // /GL_VERSION_4_2

    // *****************************************************************
    //      GL_VERSION_4_3
    // *****************************************************************

    #ifdef GL_VERSION_4_3
    if (MGL_VERSION_MIN(4, 3))
    {
        glGetIntegerv(GL_MAX_ELEMENT_INDEX, &(rs->iMaxElementIndex));
        glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &(rs->iMaxCombinedComputeUniformComponents));
        glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &(rs->iMaxCombinedShaderStorageBlocks));
        glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &(rs->iMaxComputeUniformBlocks));
        glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &(rs->iMaxComputeTextureImageUnits));
        glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &(rs->iMaxComputeUniformComponents));
        glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &(rs->iMaxComputeAtomicCounters));
        glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &(rs->iMaxComputeAtomicCounterBuffers));
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, &(rs->iMaxComputeWorkGroupCount[0]));
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &(rs->iMaxComputeWorkGroup));
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, &(rs->iMaxComputeWorkGroupSize[0]));
        glGetIntegerv(GL_DISPATCH_INDIRECT_BUFFER_BINDING, &(rs->iDispatchIndirectBufferBinding));
        glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH, &(rs->iMaxDebugGroupStackDepth));
        glGetIntegerv(GL_DEBUG_GROUP_STACK_DEPTH, &(rs->iDebugGroupStackDepth));
        glGetIntegerv(GL_MAX_LABEL_LENGTH, &(rs->iMaxLabelLength));
        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &(rs->iMaxUniformLocations));
        glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &(rs->iMaxFramebufferWidth));
        glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &(rs->iMaxFramebufferHeight));
        glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &(rs->iMaxFramebufferLayers));
        glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &(rs->iMaxFramebufferSamples));
        glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &(rs->iMaxVertexShaderStorageBlocks));
        glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &(rs->iMaxTessControlShaderStorageBlocks));
        glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &(rs->iMaxTessEvaluationShaderStorageBlocks));
        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &(rs->iMaxGeometryShaderStorageBlocks));
        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &(rs->iMaxFragmentShaderStorageBlocks));
        glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &(rs->iMaxComputeShaderStorageBlocks));
        glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &(rs->iTextureBufferOffsetAlignment));
        mglGetIntegerStaticArray(GL_VERTEX_BINDING_DIVISOR, &(rs->iVertexBindingDivisor[0]), MGL_MAX_VERTEX_BUFFER_BINDINGS);
        mglGetIntegerStaticArray(GL_VERTEX_BINDING_OFFSET, &(rs->iVertexBindingOffset[0]), MGL_MAX_VERTEX_BUFFER_BINDINGS);
        mglGetIntegerStaticArray(GL_VERTEX_BINDING_STRIDE, &(rs->iVertexBindingStride[0]), MGL_MAX_VERTEX_BUFFER_BINDINGS);
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &(rs->iMaxVertexAttribRelativeOffset));
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &(rs->iMaxVertexAttribBindings));
    }
    #endif // /GL_VERSION_4_3

    // Query number of container items first
    glGetIntegerv(GL_NUM_EXTENSIONS, &(rs->iNumExtensions));
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &(rs->iNumShaderBinaryFormats));

#if 1 //TODO: move these into a respective "if (MGL_VERSION(X, Y)) { ... }" block

    // 1.0
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(rs->iMaxTextureSize));
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &(rs->iMaxViewportDims[0]));
    glGetIntegerv(GL_PACK_ALIGNMENT, &(rs->iPackAlignment));
    glGetBooleanv(GL_PACK_LSB_FIRST, &(rs->bPackLSBFirst));
    glGetIntegerv(GL_PACK_ROW_LENGTH, &(rs->iPackRowLength));
    glGetIntegerv(GL_PACK_SKIP_PIXELS, &(rs->iPackSkipPixels));
    glGetIntegerv(GL_PACK_SKIP_ROWS, &(rs->iPackSkipRows));
    glGetBooleanv(GL_PACK_SWAP_BYTES, &(rs->bPackSwapBytes));

    // 3.0
    glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &(rs->iMinProgramTexelOffest));
    glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &(rs->iMaxProgramTexelOffest));

    // 3.1
    glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &(rs->iMaxRectangleTextureSize));
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &(rs->iMaxTextureBufferSize));
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &(rs->iMaxUniformBufferBindings));
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &(rs->iMaxUniformBlockSize));
    glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, &(rs->iPrimitiveRestartIndex));

    // 3.2
    glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &(rs->iMaxSampleMaskWords));
    glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, &(rs->iMaxServerWaitTimeout));
    glGetBooleanv(GL_PROGRAM_POINT_SIZE, &(rs->bProgramPointSize));
    glGetIntegerv(GL_PROVOKING_VERTEX, &(rs->iProvokingVertex));

    // 4.1
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &(rs->iMaxVertexUniformVectors));
    glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &(rs->iNumProgramBinaryFormats));
    mglGetIntegerDynamicArray(GL_PROGRAM_BINARY_FORMATS, &(rs->iProgramBinaryFormats[0]), rs->iNumProgramBinaryFormats, MGL_MAX_PROGRAM_BINARY_FORMATS);
    glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &(rs->iProgramPipelineBinding));

    // 4.3
    glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &(rs->iMaxShaderStorageBufferBindings));


    

    glGetFloatv(GL_POINT_SIZE, &(rs->fPointSize));
    glGetFloatv(GL_POINT_SIZE_GRANULARITY, &(rs->fPointSizeGranularity));
    glGetFloatv(GL_POINT_SIZE_RANGE, &(rs->fPointSizeRange[0]));
    glGetBooleanv(GL_POLYGON_SMOOTH, &(rs->bPolygonSmooth));
    glGetIntegerv(GL_POLYGON_SMOOTH_HINT, &(rs->iPolygonSmoothHint));
    glGetIntegerv(GL_READ_BUFFER, &(rs->iReadBuffer));
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &(rs->iReadFramebufferBinding));
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &(rs->iRenderbufferBinding));
    glGetIntegerv(GL_SAMPLE_BUFFERS, &(rs->iSampleBuffers));
    glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &(rs->fSampleCoverageValue));
    glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT, &(rs->bSampleCoverageInvert));
    glGetIntegerv(GL_SAMPLER_BINDING, &(rs->iSamplerBinding));
    glGetIntegerv(GL_SAMPLES, &(rs->iSamples));
    glGetIntegerv(GL_SCISSOR_BOX, &(rs->iScissorBox[0]));
    glGetBooleanv(GL_SCISSOR_TEST, &(rs->bScissorTest));
    glGetBooleanv(GL_SHADER_COMPILER, &(rs->bShaderCompiler));
    mglGetIntegerStaticArray(GL_SHADER_STORAGE_BUFFER_BINDING, &(rs->iShaderStorageBufferBinding[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &(rs->iShaderStorageBufferOffsetAlignment));
    mglGetInteger64StaticArray(GL_SHADER_STORAGE_BUFFER_SIZE, &(rs->iShaderStorageBufferSize[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    mglGetInteger64StaticArray(GL_SHADER_STORAGE_BUFFER_START, &(rs->iShaderStorageBufferStart[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &(rs->iStencilClearValue));
    glGetIntegerv(GL_STENCIL_FAIL, &(rs->iStencilFail));
    glGetIntegerv(GL_STENCIL_FUNC, &(rs->iStencilFunc));
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &(rs->iStencilPassDepthFail));
    glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &(rs->iStencilPassDepthPass));
    glGetIntegerv(GL_STENCIL_REF, &(rs->iStencilRef));
    glGetBooleanv(GL_STENCIL_TEST, &(rs->bStencilTest));
    glGetIntegerv(GL_STENCIL_VALUE_MASK, &(rs->iStencilValueMask));
    glGetIntegerv(GL_STENCIL_WRITEMASK, &(rs->iStencilWriteMask));
    glGetBooleanv(GL_STEREO, &(rs->bStereo));
    glGetIntegerv(GL_SUBPIXEL_BITS, &(rs->iSubPixelBits));
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &(rs->iTextureBinding1D));
    glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &(rs->iTextureBinding1DArray));
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &(rs->iTextureBinding2D));
    glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &(rs->iTextureBinding2DArray));
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &(rs->iTextureBinding2DMultisample));
    glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &(rs->iTextureBinding2DMultisampleArray));
    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &(rs->iTextureBindingBuffer));
    glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &(rs->iTextureBindingRectangle));
    glGetInteger64i_v(GL_TIMESTAMP, 0, &(rs->iTimestamp));
    mglGetIntegerStaticArray(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &(rs->iTransformFeedbackBufferBinding[0]), MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS);
    mglGetInteger64StaticArray(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, &(rs->iTransformFeedbackBufferSize[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    mglGetInteger64StaticArray(GL_TRANSFORM_FEEDBACK_BUFFER_START, &(rs->iTransformFeedbackBufferStart[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    mglGetIntegerStaticArray(GL_UNIFORM_BUFFER_BINDING, &(rs->iUniformBufferBinding[0]), MGL_MAX_UNIFORM_BUFFER_BINDINGS);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &(rs->iUniformBufferOffsetAlignment));
    mglGetInteger64StaticArray(GL_UNIFORM_BUFFER_SIZE, &(rs->iUniformBufferSize[0]), MGL_MAX_UNIFORM_BUFFER_BINDINGS);
    mglGetInteger64StaticArray(GL_UNIFORM_BUFFER_START, &(rs->iUniformBufferStart[0]), MGL_MAX_UNIFORM_BUFFER_BINDINGS);
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &(rs->iUnpackAlignment));
    glGetBooleanv(GL_UNPACK_LSB_FIRST, &(rs->bUnpackLSBFirst));
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &(rs->iUnpackRowLength));
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &(rs->iUnpackSkipPixels));
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &(rs->iUnpackSkipRows));
    glGetBooleanv(GL_UNPACK_SWAP_BYTES, &(rs->bUnpackSwapBytes));
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &(rs->iVertexArrayBinding));
    glGetIntegerv(GL_VIEWPORT, &(rs->iViewport[0]));

#endif // /TODO

    #undef MGL_VERSION
    #undef MGL_VERSION_MIN
}

static const MGLStringInternal* g_MGLOutputParamas = NULL;

static int mglCompareOutputPair(const void* lhs, const void* rhs)
{
    int lhs_i = *(const int*)lhs;
    int rhs_i = *(const int*)rhs;

    const char* lhs_s = g_MGLOutputParamas[lhs_i].buf;
    const char* rhs_s = g_MGLOutputParamas[rhs_i].buf;

    if (lhs_s != NULL)
    {
        if (rhs_s != NULL)
            return strcmp(lhs_s, rhs_s);
        else
            return -1;
    }
    return 1;
}


// *****************************************************************
//      GLOBAL FUNCTIONS
// *****************************************************************

MGLString mglQueryRenderState(const MGLQueryFormatting* formatting)
{
    // Internal constnat parameters
    static const MGLQueryFormatting g_formattingDefault     = { ' ', 1, MGLFormattingOrderDefault, 1, NULL };
    static const char*              g_valNA                 = "n/a";
    static const GLenum             g_drawBufferPnames[16]  = { GL_DRAW_BUFFER0, GL_DRAW_BUFFER1, GL_DRAW_BUFFER2, GL_DRAW_BUFFER3,
                                                                GL_DRAW_BUFFER4, GL_DRAW_BUFFER5, GL_DRAW_BUFFER6, GL_DRAW_BUFFER7,
                                                                GL_DRAW_BUFFER8, GL_DRAW_BUFFER9, GL_DRAW_BUFFER10, GL_DRAW_BUFFER11,
                                                                GL_DRAW_BUFFER12, GL_DRAW_BUFFER13, GL_DRAW_BUFFER14, GL_DRAW_BUFFER15 };

    if (formatting == NULL)
        formatting = (&g_formattingDefault);

    // Provide array with all string parts
    MGLStringInternal out_par[MGL_MAX_NUM_RENDER_STATES], out_val[MGL_MAX_NUM_RENDER_STATES];

    memset(out_par, 0, sizeof(out_par));
    memset(out_val, 0, sizeof(out_val));

    MGLStringPairArray out = { out_par, out_val, 0 };

    // Get last GL error
    GLenum last_err = glGetError();

    // Query all OpenGL states
    MGLRenderStates rs;
    mglGetGLRenderStates(&rs);

    #define MGL_VERSION(MAJOR, MINOR)       (((MAJOR) << 16) | (MINOR))
    #define MGL_VERSION_MIN(MAJOR, MINOR)   (MGL_VERSION(rs.iMajorVersion, rs.iMinorVersion) >= MGL_VERSION(MAJOR, MINOR))
    #define MGL_PARAM_UNAVAIL(NAME)         mglNextParamString(&out, #NAME, g_valNA)

    // *****************************************************************
    //      GL_VERSION_1_0
    // *****************************************************************

    mglNextParamInteger(&out, "GL_MAJOR_VERSION", rs.iMajorVersion);
    mglNextParamInteger(&out, "GL_MINOR_VERSION", rs.iMinorVersion);

    // *****************************************************************
    //      GL_VERSION_1_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_1");

    #ifdef GL_VERSION_1_1
    if (MGL_VERSION_MIN(1, 1))
    {
        mglNextParamBoolean(&out, "GL_COLOR_LOGIC_OP", rs.bColorLogicOp);
        mglNextParamFloat(&out, "GL_POLYGON_OFFSET_FACTOR", rs.fPolygonOffsetFactor);
        mglNextParamFloat(&out, "GL_POLYGON_OFFSET_UNITS", rs.fPolygonOffsetUnits);
        mglNextParamBoolean(&out, "GL_POLYGON_OFFSET_FILL", rs.bPolygonOffsetFill);
        mglNextParamBoolean(&out, "GL_POLYGON_OFFSET_LINE", rs.bPolygonOffsetLine);
        mglNextParamBoolean(&out, "GL_POLYGON_OFFSET_POINT", rs.bPolygonOffsetPoint);
    }
    else
    #endif // /GL_VERSION_1_1
    {
        MGL_PARAM_UNAVAIL( GL_COLOR_LOGIC_OP );
        MGL_PARAM_UNAVAIL( GL_POLYGON_OFFSET_FACTOR );
        MGL_PARAM_UNAVAIL( GL_POLYGON_OFFSET_UNITS );
        MGL_PARAM_UNAVAIL( GL_POLYGON_OFFSET_FILL );
        MGL_PARAM_UNAVAIL( GL_POLYGON_OFFSET_LINE );
        MGL_PARAM_UNAVAIL( GL_POLYGON_OFFSET_POINT );
    }

    // *****************************************************************
    //      GL_VERSION_1_2
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_2");

    #ifdef GL_VERSION_1_2
    if (MGL_VERSION_MIN(1, 2))
    {
        mglNextParamFloatArray(&out, "GL_ALIASED_LINE_WIDTH_RANGE", rs.fAliasedLineWidthRange, 2);
        mglNextParamFloatArray(&out, "GL_BLEND_COLOR", rs.fBlendColor, 4);
        mglNextParamInteger(&out, "GL_MAX_3D_TEXTURE_SIZE", rs.iMax3DTextureSize);
        mglNextParamInteger(&out, "GL_MAX_ELEMENTS_INDICES", rs.iMaxElementsIndices);
        mglNextParamInteger(&out, "GL_MAX_ELEMENTS_VERTICES", rs.iMaxElementsVertices);
        mglNextParamInteger(&out, "GL_PACK_IMAGE_HEIGHT", rs.iPackImageHeight);
        mglNextParamInteger(&out, "GL_PACK_SKIP_IMAGES", rs.iPackSkipImages);
        mglNextParamFloatArray(&out, "GL_SMOOTH_LINE_WIDTH_RANGE", rs.fSmoothLineWidthRange, 2);
        mglNextParamFloat(&out, "GL_SMOOTH_LINE_WIDTH_GRANULARITY", rs.fSmoothLineWidthGranularity);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_3D", rs.iTextureBinding3D);
        mglNextParamInteger(&out, "GL_UNPACK_IMAGE_HEIGHT", rs.iUnpackImageHeight);
        mglNextParamInteger(&out, "GL_UNPACK_SKIP_IMAGES", rs.iUnpackSkipImages);
    }
    else
    #endif // /GL_VERSION_1_2
    {
        MGL_PARAM_UNAVAIL( GL_ALIASED_LINE_WIDTH_RANGE );
        MGL_PARAM_UNAVAIL( GL_BLEND_COLOR );
        MGL_PARAM_UNAVAIL( GL_MAX_3D_TEXTURE_SIZE );
        MGL_PARAM_UNAVAIL( GL_MAX_ELEMENTS_INDICES );
        MGL_PARAM_UNAVAIL( GL_MAX_ELEMENTS_VERTICES );
        MGL_PARAM_UNAVAIL( GL_PACK_IMAGE_HEIGHT );
        MGL_PARAM_UNAVAIL( GL_PACK_SKIP_IMAGES );
        MGL_PARAM_UNAVAIL( GL_SMOOTH_LINE_WIDTH_RANGE );
        MGL_PARAM_UNAVAIL( GL_SMOOTH_LINE_WIDTH_GRANULARITY );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_3D );
        MGL_PARAM_UNAVAIL( GL_UNPACK_IMAGE_HEIGHT );
        MGL_PARAM_UNAVAIL( GL_UNPACK_SKIP_IMAGES );
    }

    // *****************************************************************
    //      GL_VERSION_1_3
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_3");

    #ifdef GL_VERSION_1_3
    if (MGL_VERSION_MIN(1, 3))
    {
        mglNextParamInteger(&out, "GL_NUM_COMPRESSED_TEXTURE_FORMATS", rs.iNumCompressedTextureFormats);
        mglNextParamIntegerArray(&out, "GL_COMPRESSED_TEXTURE_FORMATS", rs.iCompressedTextureFormats, (size_t)rs.iNumCompressedTextureFormats, MGL_MAX_COMPRESSED_TEXTURE_FORMATS, formatting->enable_hex);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_CUBE_MAP", rs.iTextureBindingCubeMap);
        mglNextParamEnum(&out, "GL_TEXTURE_COMPRESSION_HINT", rs.iTextureCompressionHint, mglHintModeStr);
        mglNextParamInteger(&out, "GL_ACTIVE_TEXTURE", rs.iActiveTexture);
        mglNextParamInteger(&out, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", rs.iMaxCubeMapTextureSize);
    }
    else
    #endif // /GL_VERSION_1_3
    {
        MGL_PARAM_UNAVAIL( GL_NUM_COMPRESSED_TEXTURE_FORMATS );
        MGL_PARAM_UNAVAIL( GL_COMPRESSED_TEXTURE_FORMATS );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_CUBE_MAP );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_COMPRESSION_HINT );
        MGL_PARAM_UNAVAIL( GL_ACTIVE_TEXTURE );
        MGL_PARAM_UNAVAIL( GL_MAX_CUBE_MAP_TEXTURE_SIZE );
    }

    // *****************************************************************
    //      GL_VERSION_1_4
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_4");

    #ifdef GL_VERSION_1_4
    if (MGL_VERSION_MIN(1, 4))
    {
        mglNextParamEnum(&out, "GL_BLEND_DST_ALPHA", rs.iBlendDstAlpha, mglBlendFuncStr);
        mglNextParamEnum(&out, "GL_BLEND_DST_RGB", rs.iBlendDstRGB, mglBlendFuncStr);
        mglNextParamEnum(&out, "GL_BLEND_SRC_ALPHA", rs.iBlendSrcAlpha, mglBlendFuncStr);
        mglNextParamEnum(&out, "GL_BLEND_SRC_RGB", rs.iBlendSrcRGB, mglBlendFuncStr);
        mglNextParamFloat(&out, "GL_MAX_TEXTURE_LOD_BIAS", rs.fMaxTextureLODBias);
        mglNextParamFloat(&out, "GL_POINT_FADE_THRESHOLD_SIZE", rs.fPointFadeThresholdSize);
    }
    else
    #endif // /GL_VERSION_1_4
    {
        MGL_PARAM_UNAVAIL( GL_BLEND_DST_ALPHA );
        MGL_PARAM_UNAVAIL( GL_BLEND_DST_RGB );
        MGL_PARAM_UNAVAIL( GL_BLEND_SRC_ALPHA );
        MGL_PARAM_UNAVAIL( GL_BLEND_SRC_RGB );
        MGL_PARAM_UNAVAIL( GL_MAX_TEXTURE_LOD_BIAS );
        MGL_PARAM_UNAVAIL( GL_POINT_FADE_THRESHOLD_SIZE );
    }

    // *****************************************************************
    //      GL_VERSION_1_5
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_5");

    #ifdef GL_VERSION_1_5
    if (MGL_VERSION_MIN(1, 5))
    {
        mglNextParamInteger(&out, "GL_ARRAY_BUFFER_BINDING", rs.iArrayBufferBinding);
        mglNextParamInteger(&out, "GL_ELEMENT_ARRAY_BUFFER_BINDING", rs.iElementArrayBufferBinding);
    }
    else
    #endif // /GL_VERSION_1_5
    {
        MGL_PARAM_UNAVAIL( GL_ARRAY_BUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_ELEMENT_ARRAY_BUFFER_BINDING );
    }

    // *****************************************************************
    //      GL_VERSION_2_0
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_2_0");

    #ifdef GL_VERSION_2_0
    if (MGL_VERSION_MIN(2, 0))
    {
        mglNextParamEnum(&out, "GL_BLEND_EQUATION_ALPHA", rs.iBlendEquationAlpha, mglBlendEquationModeStr);
        mglNextParamEnum(&out, "GL_BLEND_EQUATION_RGB", rs.iBlendEquationRGB, mglBlendEquationModeStr);
        mglNextParamInteger(&out, "GL_CURRENT_PROGRAM", rs.iCurrentProgram);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER0", rs.iDrawBuffer_i[0], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER1", rs.iDrawBuffer_i[1], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER2", rs.iDrawBuffer_i[2], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER3", rs.iDrawBuffer_i[3], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER4", rs.iDrawBuffer_i[4], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER5", rs.iDrawBuffer_i[5], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER6", rs.iDrawBuffer_i[6], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER7", rs.iDrawBuffer_i[7], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER8", rs.iDrawBuffer_i[8], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER9", rs.iDrawBuffer_i[9], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER10", rs.iDrawBuffer_i[10], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER11", rs.iDrawBuffer_i[11], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER12", rs.iDrawBuffer_i[12], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER13", rs.iDrawBuffer_i[13], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER14", rs.iDrawBuffer_i[14], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER15", rs.iDrawBuffer_i[15], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_FRAGMENT_SHADER_DERIVATIVE_HINT", rs.iFragmentShaderDerivativeHint, mglHintModeStr);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", rs.iMaxCombinedTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_DRAW_BUFFERS", rs.iMaxDrawBuffers);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", rs.iMaxFragmentUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_TEXTURE_IMAGE_UNITS", rs.iMaxTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_VARYING_FLOATS", rs.iMaxVaryingFloats);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATTRIBS", rs.iMaxVertexAttribs);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", rs.iMaxVertexTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_UNIFORM_COMPONENTS", rs.iMaxVertexUniformComponents);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_FAIL", rs.iStencilBackFail, mglStencilOpStr);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_FUNC", rs.iStencilBackFunc, mglCompareFuncStr);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_PASS_DEPTH_FAIL", rs.iStencilBackPassDepthFail, mglStencilOpStr);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_PASS_DEPTH_PASS", rs.iStencilBackPassDepthPass, mglStencilOpStr);
        mglNextParamInteger(&out, "GL_STENCIL_BACK_REF", rs.iStencilBackRef);
        mglNextParamIntegerHex(&out, "GL_STENCIL_BACK_VALUE_MASK", rs.iStencilBackValueMask);
        mglNextParamIntegerHex(&out, "GL_STENCIL_BACK_WRITEMASK", rs.iStencilBackWriteMask);
    }
    else
    #endif // /GL_VERSION_2_0
    {
        MGL_PARAM_UNAVAIL( GL_BLEND_EQUATION_ALPHA );
        MGL_PARAM_UNAVAIL( GL_BLEND_EQUATION_RGB );
        MGL_PARAM_UNAVAIL( GL_CURRENT_PROGRAM );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER0 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER1 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER2 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER3 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER4 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER5 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER6 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER7 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER8 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER9 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER10 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER11 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER12 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER13 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER14 );
        MGL_PARAM_UNAVAIL( GL_DRAW_BUFFER15 );
        MGL_PARAM_UNAVAIL( GL_FRAGMENT_SHADER_DERIVATIVE_HINT );
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS );
        MGL_PARAM_UNAVAIL( GL_MAX_DRAW_BUFFERS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAGMENT_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_TEXTURE_IMAGE_UNITS );
        MGL_PARAM_UNAVAIL( GL_MAX_VARYING_FLOATS );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_ATTRIBS );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_FAIL );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_FUNC );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_PASS_DEPTH_FAIL );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_PASS_DEPTH_PASS );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_REF );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_VALUE_MASK );
        MGL_PARAM_UNAVAIL( GL_STENCIL_BACK_WRITEMASK );
    }

    // *****************************************************************
    //      GL_VERSION_2_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_2_1");

    #ifdef GL_VERSION_2_1
    if (MGL_VERSION_MIN(2, 1))
    {
        mglNextParamInteger(&out, "GL_PIXEL_PACK_BUFFER_BINDING", rs.iPixelPackBufferBinding);
        mglNextParamInteger(&out, "GL_PIXEL_UNPACK_BUFFER_BINDING", rs.iPixelUnpackBufferBinding);
    }
    else
    #endif // /GL_VERSION_2_1
    {
        MGL_PARAM_UNAVAIL( GL_PIXEL_PACK_BUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_PIXEL_UNPACK_BUFFER_BINDING );
    }

    // *****************************************************************
    //      GL_VERSION_3_0
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_3_0");

    #ifdef GL_VERSION_3_0
    if (MGL_VERSION_MIN(3, 0))
    {
        mglNextParamBitfield(&out, "GL_CONTEXT_FLAGS", rs.iContextFlags, 32, mglContextFlagBitStr);
        mglNextParamInteger(&out, "GL_DRAW_FRAMEBUFFER_BINDING", rs.iDrawFramebufferBinding);
        mglNextParamInteger(&out, "GL_MAX_ARRAY_TEXTURE_LAYERS", rs.iMaxArrayTextureLayers);
        mglNextParamInteger(&out, "GL_MAX_CLIP_DISTANCES", rs.iMaxClipDistances);
        mglNextParamInteger(&out, "GL_MAX_RENDERBUFFER_SIZE", rs.iMaxRenderbufferSize);
        mglNextParamInteger(&out, "GL_MAX_VARYING_COMPONENTS", rs.iMaxVaryingComponents);
    }
    else
    #endif // /GL_VERSION_3_0
    {
        MGL_PARAM_UNAVAIL( GL_CONTEXT_FLAGS );
        MGL_PARAM_UNAVAIL( GL_DRAW_FRAMEBUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_MAX_ARRAY_TEXTURE_LAYERS );
        MGL_PARAM_UNAVAIL( GL_MAX_CLIP_DISTANCES );
        MGL_PARAM_UNAVAIL( GL_MAX_RENDERBUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_MAX_VARYING_COMPONENTS );
    }

    // *****************************************************************
    //      GL_VERSION_3_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_3_1");

    #ifdef GL_VERSION_3_1
    if (MGL_VERSION_MIN(3, 1))
    {
        mglNextParamInteger(&out, "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", rs.iMaxCombinedFragmentUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", rs.iMaxCombinedGeometryUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", rs.iMaxCombinedVertexUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_UNIFORM_BLOCKS", rs.iMaxCombinedUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_UNIFORM_BLOCKS", rs.iMaxFragmentUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_UNIFORM_BLOCKS", rs.iMaxGeometryUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_UNIFORM_BLOCKS", rs.iMaxVertexUniformBlocks);
    }
    else
    #endif // /GL_VERSION_3_1
    {
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_UNIFORM_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAGMENT_UNIFORM_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_UNIFORM_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_UNIFORM_BLOCKS );
    }

    // *****************************************************************
    //      GL_VERSION_3_2
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_3_2");

    #ifdef GL_VERSION_3_2
    if (MGL_VERSION_MIN(3, 2))
    {
        mglNextParamInteger(&out, "GL_MAX_COLOR_TEXTURE_SAMPLES", rs.iMaxColorTextureSamples);
        mglNextParamInteger(&out, "GL_MAX_DEPTH_TEXTURE_SAMPLES", rs.iMaxDepthTextureSamples);
        mglNextParamInteger(&out, "GL_MAX_INTEGER_SAMPLES", rs.iMaxIntegerSamples);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_INPUT_COMPONENTS", rs.iMaxGeometryInputComponents);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS", rs.iMaxGeometryOutputComponents);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", rs.iMaxGeometryTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS", rs.iMaxGeometryUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_INPUT_COMPONENTS", rs.iMaxFragmentInputComponents);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_OUTPUT_COMPONENTS", rs.iMaxVertexOutputComponents);
    }
    else
    #endif
    {
        MGL_PARAM_UNAVAIL( GL_MAX_COLOR_TEXTURE_SAMPLES );
        MGL_PARAM_UNAVAIL( GL_MAX_DEPTH_TEXTURE_SAMPLES );
        MGL_PARAM_UNAVAIL( GL_MAX_INTEGER_SAMPLES );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_INPUT_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_OUTPUT_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAGMENT_INPUT_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_OUTPUT_COMPONENTS );
    }

    // *****************************************************************
    //      GL_VERSION_3_3
    // *****************************************************************

    #ifdef GL_VERSION_3_3
    if (MGL_VERSION_MIN(3, 3))
    {
        //TODO...
    }
    else
    #endif
    {
        //TODO...
    }

    // *****************************************************************
    //      GL_VERSION_4_0
    // *****************************************************************

    #ifdef GL_VERSION_4_0
    if (MGL_VERSION_MIN(4, 0))
    {
        //TODO...
    }
    else
    #endif
    {
        //TODO...
    }

    // *****************************************************************
    //      GL_VERSION_4_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_1");

    #ifdef GL_VERSION_4_1
    if (MGL_VERSION_MIN(4, 1))
    {
        mglNextParamEnum(&out, "GL_IMPLEMENTATION_COLOR_READ_FORMAT", rs.iImplementationColorReadFormat, mglImplementationColorReadFormatStr);
        mglNextParamEnum(&out, "GL_IMPLEMENTATION_COLOR_READ_TYPE", rs.iImplementationColorReadType, mglImplementationColorReadTypeStr);
        mglNextParamEnum(&out, "GL_LAYER_PROVOKING_VERTEX", rs.iLayerProvokingVertex, mglProvokingVertexModeStr);
        mglNextParamInteger(&out, "GL_MAX_VARYING_VECTORS", rs.iMaxVaryingVectors);
        mglNextParamInteger(&out, "GL_MAX_VIEWPORTS", rs.iMaxViewports);
        mglNextParamIntegerArray(&out, "GL_VIEWPORT_BOUNDS_RANGE", rs.iViewportBoundsRange, 2, 2, 0);
        mglNextParamEnum(&out, "GL_VIEWPORT_INDEX_PROVOKING_VERTEX", rs.iViewportIndexProvokingVertex, mglProvokingVertexModeStr);
        mglNextParamInteger(&out, "GL_VIEWPORT_SUBPIXEL_BITS", rs.iViewportSubPixelBits);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", rs.iMaxFragmentUniformVectors);
    }
    else
    #endif // /GL_VERSION_4_1
    {
        MGL_PARAM_UNAVAIL( GL_IMPLEMENTATION_COLOR_READ_FORMAT );
        MGL_PARAM_UNAVAIL( GL_IMPLEMENTATION_COLOR_READ_TYPE );
        MGL_PARAM_UNAVAIL( GL_LAYER_PROVOKING_VERTEX );
        MGL_PARAM_UNAVAIL( GL_MAX_VARYING_VECTORS );
        MGL_PARAM_UNAVAIL( GL_MAX_VIEWPORTS );
        MGL_PARAM_UNAVAIL( GL_VIEWPORT_BOUNDS_RANGE );
        MGL_PARAM_UNAVAIL( GL_VIEWPORT_INDEX_PROVOKING_VERTEX );
        MGL_PARAM_UNAVAIL( GL_VIEWPORT_SUBPIXEL_BITS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAGMENT_UNIFORM_VECTORS );
    }

    // *****************************************************************
    //      GL_VERSION_4_2
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_2");

    #ifdef GL_VERSION_4_2
    if (MGL_VERSION_MIN(4, 2))
    {
        mglNextParamInteger(&out, "GL_MAX_COMBINED_ATOMIC_COUNTERS", rs.iMaxCombinedAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATOMIC_COUNTERS", rs.iMaxVertexAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS", rs.iMaxTessControlAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS", rs.iMaxTessEvaluationAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_ATOMIC_COUNTERS", rs.iMaxGeometryAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_ATOMIC_COUNTERS", rs.iMaxFragmentAtomicCounters);
        mglNextParamInteger(&out, "GL_MIN_MAP_BUFFER_ALIGNMENT", rs.iMinMapBufferAlignment);
    }
    else
    #endif // /GL_VERSION_4_2
    {
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAGMENT_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MIN_MAP_BUFFER_ALIGNMENT );
    }

    // *****************************************************************
    //      GL_VERSION_4_3
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_3");

    #ifdef GL_VERSION_4_3
    if (MGL_VERSION_MIN(4, 3))
    {
        mglNextParamUInteger(&out, "GL_MAX_ELEMENT_INDEX", (GLuint)(rs.iMaxElementIndex));
        mglNextParamInteger(&out, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", rs.iMaxCombinedComputeUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS", rs.iMaxCombinedShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_UNIFORM_BLOCKS", rs.iMaxComputeUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS", rs.iMaxComputeTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS", rs.iMaxComputeUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_ATOMIC_COUNTERS", rs.iMaxComputeAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", rs.iMaxComputeAtomicCounterBuffers);
        mglNextParamIntegerArray(&out, "GL_MAX_COMPUTE_WORK_GROUP_COUNT", rs.iMaxComputeWorkGroupCount, 3, 3, 0);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS", rs.iMaxComputeWorkGroup);
        mglNextParamIntegerArray(&out, "GL_MAX_COMPUTE_WORK_GROUP_SIZE", rs.iMaxComputeWorkGroupSize, 3, 3, 0);
        mglNextParamInteger(&out, "GL_DISPATCH_INDIRECT_BUFFER_BINDING", rs.iDispatchIndirectBufferBinding);
        mglNextParamInteger(&out, "GL_MAX_DEBUG_GROUP_STACK_DEPTH", rs.iMaxDebugGroupStackDepth);
        mglNextParamInteger(&out, "GL_DEBUG_GROUP_STACK_DEPTH", rs.iDebugGroupStackDepth);
        mglNextParamInteger(&out, "GL_MAX_LABEL_LENGTH", rs.iMaxLabelLength);
        mglNextParamInteger(&out, "GL_MAX_UNIFORM_LOCATIONS", rs.iMaxUniformLocations);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_WIDTH", rs.iMaxFramebufferWidth);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_HEIGHT", rs.iMaxFramebufferHeight);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_LAYERS", rs.iMaxFramebufferLayers);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_SAMPLES", rs.iMaxFramebufferSamples);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS", rs.iMaxVertexShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS", rs.iMaxTessControlShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", rs.iMaxTessEvaluationShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS", rs.iMaxGeometryShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS", rs.iMaxFragmentShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS", rs.iMaxComputeShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT", rs.iTextureBufferOffsetAlignment);
        /*mglNextParamInteger(&out, "GL_VERTEX_BINDING_DIVISOR", rs.iVertexBindingDivisor);
        mglNextParamInteger(&out, "GL_VERTEX_BINDING_OFFSET", rs.iVertexBindingDivisor);
        mglNextParamInteger(&out, "GL_VERTEX_BINDING_STRIDE", rs.iVertexBindingDivisor);*/
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", rs.iMaxVertexAttribRelativeOffset);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATTRIB_BINDINGS", rs.iMaxVertexAttribBindings);
    }
    else
    #endif // /GL_VERSION_4_3
    {
        MGL_PARAM_UNAVAIL( GL_MAX_ELEMENT_INDEX );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_UNIFORM_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_ATOMIC_COUNTERS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_WORK_GROUP_COUNT );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_WORK_GROUP_SIZE );
        MGL_PARAM_UNAVAIL( GL_DISPATCH_INDIRECT_BUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_MAX_DEBUG_GROUP_STACK_DEPTH );
        MGL_PARAM_UNAVAIL( GL_DEBUG_GROUP_STACK_DEPTH );
        MGL_PARAM_UNAVAIL( GL_MAX_LABEL_LENGTH );
        MGL_PARAM_UNAVAIL( GL_MAX_UNIFORM_LOCATIONS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAMEBUFFER_WIDTH );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAMEBUFFER_HEIGHT );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAMEBUFFER_LAYERS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAMEBUFFER_SAMPLES );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT );
        MGL_PARAM_UNAVAIL( GL_VERTEX_BINDING_DIVISOR );
        MGL_PARAM_UNAVAIL( GL_VERTEX_BINDING_OFFSET );
        MGL_PARAM_UNAVAIL( GL_VERTEX_BINDING_STRIDE );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET );
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_ATTRIB_BINDINGS );
    }

    // Determine longest parameter name
    size_t max_par_len = 0;

    for (size_t i = 0; i < out.index; ++i)
        max_par_len = MGL_MAX(max_par_len, out.first[i].len);

    max_par_len += formatting->distance;

    // Determine output string capacity
    size_t out_cap = 0;

    for (size_t i = 0; i < out.index; ++i)
    {
        out_cap += out.first[i].len;
        out_cap += (max_par_len - out.first[i].len);
        out_cap += out.second[i].len;
        out_cap += 1;
    }

    // Allocate string with spaces to separate parameters and values nicely
    MGLStringInternal s_spaces;
    mglStringInternalInit(&s_spaces, max_par_len + 1);
    mglStringInternalResize(&s_spaces, max_par_len, formatting->separator);

    // Allocate output string
    MGLStringInternal* s = MGL_MALLOC(MGLStringInternal);
    mglStringInternalInit(s, out_cap);

    // Change order of strings
    int out_permutations[MGL_MAX_NUM_RENDER_STATES];

    for (size_t i = 0; i < MGL_MAX_NUM_RENDER_STATES; ++i)
        out_permutations[i] = i;

    if (formatting->order == MGLFormattingOrderSorted)
    {
        g_MGLOutputParamas = out_par;
        qsort(out_permutations, MGL_MAX_NUM_RENDER_STATES, sizeof(int), mglCompareOutputPair);
    }

    // Merge all strings parts to the output string
    for (size_t i = 0; i < out.index; ++i)
    {
        int out_i = out_permutations[i];

        if (formatting->filter == NULL || strstr(out.first[out_i].buf, formatting->filter) != NULL)
        {
            // Append string parts to output string
            mglStringInternalAppend(s, &(out.first[out_i]));
            mglStringInternalAppendSub(s, &s_spaces, 0, max_par_len - out.first[out_i].len);
            mglStringInternalAppend(s, &(out.second[out_i]));
            mglStringInternalAppendCStr(s, "\n");
        }

        // Free string parts
        mglStringInternalFree(&(out.first[out_i]));
        mglStringInternalFree(&(out.second[out_i]));
    }

    mglStringInternalFree(&s_spaces);

    return (MGLString)s;

    #undef MGL_VERSION
    #undef MGL_VERSION_MIN
    #undef MGL_PARAM_UNAVAIL
}

MGLString mglAnalyzeIssues(enum MGLIssue issue)
{
    MGLStringInternal* s = MGL_MALLOC(MGLStringInternal);
    mglStringInternalInit(s, 0);


    return (MGLString)s;
}

const char* mglGetString(MGLString s)
{
    return ((MGLStringInternal*)s)->buf;
}

void mglFreeString(MGLString s)
{
    if (s)
    {
        mglStringInternalFree((MGLStringInternal*)s);
        free(s);
    }
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


#endif


#endif // /MENTAL_GL_H
