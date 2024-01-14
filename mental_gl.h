/**
 * MentalGL (OpenGL debugging functions)
 * Published 2018 under the public domain
 *
 * Authors:
 *  - Lukas Hermanns (Creator)
 *
 * HISTORY:
 *  - v1.00 (03/26/2018): First release
 *  - v1.01 (01/26/2019): Refinements, MacOS portability
 *  - v1.02 (10/20/2023): Added mglQueryBindingPoints and mglPrintBindingPoints
 *  - v1.03 (14/01/2024): Added missing GL states: GL_POLYGON_MODE, GL_FRONT_FACE, GL_PATCH_*, GL_CLIP_*
 *
 * USAGE EXAMPLE:
 *  // Optionally enable glGetIntegeri_v and glGetInteger64i_v
 *  #define MENTAL_GL_GETINTEGERI_V
 *  #define MENTAL_GL_GETINTEGER64I_V
 *
 *  // Include and implement MentalGL in a single source file
 *  #define MENTAL_GL_IMPLEMENTATION
 *  #include "mental_gl.h"
 *
 *  // Do OpenGL stuff ...
 *
 *  // Query current OpenGL state
 *  MGLRenderState rs;
 *  mglQueryRenderState(&rs);
 *
 *  // Print queried OpenGL state (ignore optional formatting descriptor, otherwise see 'MGLFormattingOptions' structure)
 *  MGLString s = mglPrintRenderState(&rs, NULL);
 *
 *  // Print result
 *  puts(mglGetUTF8String(s));
 *
 *  // Free opaque string object
 *  mglFreeString(s);
 */

#ifndef MENTAL_GL_H
#define MENTAL_GL_H


#include <stddef.h>


// *****************************************************************
//      PUBLIC MACROS
// *****************************************************************

#define MGL_MAX_COMPRESSED_TEXTURE_FORMATS          ( 128 )
#define MGL_MAX_PROGRAM_BINARY_FORMATS              ( 16 )
#define MGL_MAX_SHADER_BINARY_FORMATS               ( 16 )
#define MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS      ( 32 )
#define MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS  ( 16 )
#define MGL_MAX_UNIFORM_BUFFER_BINDINGS             ( 32 )
#define MGL_MAX_VERTEX_BUFFER_BINDINGS              ( 32 )
#define MGL_MAX_TEXTURE_LAYERS                      ( 32 )


// *****************************************************************
//      PUBLIC ENUMERATIONS
// *****************************************************************

// Query formatting order.
enum MGLFormattingOrder
{
    MGLFormattingOrderDefault,
    MGLFormattingOrderSorted,
};


// *****************************************************************
//      PUBLIC STRUCTURES
// *****************************************************************

// Opaque string object used as result mglPrintRenderState.
typedef void* MGLString;

// Query formatting descriptor structure.
typedef struct MGLFormattingOptions
{
    char                    separator;      // Character to separate the attributes from their values. By default ' '.
    size_t                  distance;       // Distances (in number of characters) between the longest attribute name and the attribute values. By default 1.
    size_t                  array_limit;    // Limit of characters for an array. If an array exceeds this limit, it will be printed into multiple lines, one for each element. By default 200.
    enum MGLFormattingOrder order;          // Specifies the formatting order (if sorted for instance). By default MGLFormattingOrderDefault.
    int                     enable_hex;     // Specifies whether unknown enumerations shall be printed as hex codes (if != 0). By default 1.
    const char*             filter;         // Optional filter to only output parameters which contain this string. By default NULL.
}
MGLFormattingOptions;

typedef struct MGLRenderState
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
    GLint       iClipDepthMode;                                                                 // GL_CLIP_DEPTH_MODE
    GLint       iClipOrigin;                                                                    // GL_CLIP_ORIGIN
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
    GLint       iFrontFace;                                                                     // GL_FRONT_FACE
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
    GLint       iMaxTransformFeedbackBuffers;                                                   // GL_MAX_TRANSFORM_FEEDBACK_BUFFERS
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
    GLint       iPatchDefaultInnerLevel;                                                        // GL_PATCH_DEFAULT_INNER_LEVEL
    GLint       iPatchDefaultOuterLevel;                                                        // GL_PATCH_DEFAULT_OUTER_LEVEL
    GLint       iPatchVertices;                                                                 // GL_PATCH_VERTICES
    GLint       iPixelPackBufferBinding;                                                        // GL_PIXEL_PACK_BUFFER_BINDING
    GLint       iPixelUnpackBufferBinding;                                                      // GL_PIXEL_UNPACK_BUFFER_BINDING
    GLfloat     fPointFadeThresholdSize;                                                        // GL_POINT_FADE_THRESHOLD_SIZE
    GLint       iPolygonMode[2];                                                                // GL_POLYGON_MODE
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
    GLint       iShaderBinaryFormats[MGL_MAX_SHADER_BINARY_FORMATS];                            // GL_SHADER_BINARY_FORMATS
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
    GLint64     iTransformFeedbackBufferSize[MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS];       // GL_TRANSFORM_FEEDBACK_BUFFER_SIZE
    GLint64     iTransformFeedbackBufferStart[MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS];      // GL_TRANSFORM_FEEDBACK_BUFFER_START
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
MGLRenderState;

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
//      PUBLIC FUNCTIONS
// *****************************************************************

#ifdef __cplusplus
extern "C" {
#endif

// Queries the entire OpenGL render state and stores it in 'render_state'.
void mglQueryRenderState(MGLRenderState* render_state);

// Queries the entire OpenGL binding points and stores it in 'binding_points'.
void mglQueryBindingPoints(MGLBindingPoints* binding_points);

// Prints the entire OpenGL render states specified by 'render_state' and returns the formatted output string.
MGLString mglPrintRenderState(const MGLRenderState* render_state, const MGLFormattingOptions* formatting);

// Prints the entire OpenGL binding points specified by 'binding_points' and returns the formatted output string.
MGLString mglPrintBindingPoints(const MGLBindingPoints* binding_points, const MGLFormattingOptions* formatting);

// Returns the null-terminated string from the specified opaque object.
const char* mglGetUTF8String(MGLString s);

// Release all resources allocated by this library.
void mglFreeString(MGLString s);

#ifdef __cplusplus
} // /extern "C"
#endif


// *****************************************************************
//      IMPLEMENTATION
// *****************************************************************

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

#ifdef __APPLE__
#   include <OpenGL/OpenGL.h>
#else
#   include <GL/GL.h>
#   include <GL/glext.h>
#endif


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
#define MGL_STRING_NPOS                             (size_t)~0

#define MGL_MAX_NUM_RENDER_STATES                   264


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


// *****************************************************************
//      INTERNAL FUNCTIONS
// *****************************************************************

#ifdef __cplusplus
extern "C" {
#endif

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

#if 0 // UNUSED
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
#endif // /UNUSED

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
    if (s && appendix && appendix->len > off && len > 0)
    {
        // Resize string and copy appendix into new area
        size_t lhs_len = s->len;
        size_t rhs_len = MGL_MIN(len, appendix->len - off);
        if (rhs_len > 0)
        {
            mglStringInternalResize(s, lhs_len + rhs_len, 0);
            memcpy(s->buf + lhs_len, appendix->buf + off, rhs_len);
        }
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

static size_t mglStringInternalFindChar(const MGLStringInternal* s, char search_char, size_t off, size_t len)
{
    if (s)
    {
        len = MGL_MIN(len, s->len);
        for (; off < len; ++off)
        {
            if (s->buf[off] == search_char)
                return off;
        }
    }
    return MGL_STRING_NPOS;
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
    #ifdef MENTAL_GL_GETINTEGERI_V
    for (GLuint i = 0; i < count; ++i)
        glGetIntegeri_v(pname, i, &(data[i]));
    #endif
}

static void mglGetInteger64StaticArray(GLenum pname, GLint64* data, GLuint count)
{
    #ifdef MENTAL_GL_GETINTEGER64I_V
    for (GLuint i = 0; i < count; ++i)
        glGetInteger64i_v(pname, i, &(data[i]));
    #endif
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

static void mglNextParamEnumArray(MGLStringPairArray* str_array, const char* par, const GLint* val, size_t count, size_t limit, MGLEnumToStringProc proc)
{
    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    mglStringInternalAppendCStr(out_val, "{ ");

    size_t n = MGL_MIN(count, limit);

    for (size_t i = 0; i < n; ++i)
    {
        const char* s_val = proc(val[i]);
        if (!s_val)
        {
            char s_hex[11];
            mglEnumToHex(s_hex, (unsigned)val[i]);
            mglStringInternalAppendCStr(out_val, s_hex);
        }
        else
            mglStringInternalAppendCStr(out_val, s_val);

        if (i + 1 < n)
            mglStringInternalAppendCStr(out_val, ", ");
    }

    if (count > limit)
        mglStringInternalAppendCStr(out_val, ", ... ");

    mglStringInternalAppendCStr(out_val, " }");

    ++(str_array->index);
}

static void mglNextParamInteger64Array(MGLStringPairArray* str_array, const char* par, const GLint64* val, size_t count, size_t limit)
{
    char s_val[32];

    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    mglStringInternalAppendCStr(out_val, "{ ");

    size_t n = MGL_MIN(count, limit);

    for (size_t i = 0; i < n; ++i)
    {
        sprintf(s_val, "%lli", val[i]);

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

static void mglNextParamDoubleArray(MGLStringPairArray* str_array, const char* par, const GLdouble* val, size_t count)
{
    char s_val[64];

    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    mglStringInternalAppendCStr(out_val, "{ ");

    for (size_t i = 0; i < count; ++i)
    {
        sprintf(s_val, "%f", val[i]);
        mglStringInternalAppendCStr(out_val, s_val);

        if (i + 1 < count)
            mglStringInternalAppendCStr(out_val, ", ");
    }

    mglStringInternalAppendCStr(out_val, " }");

    ++(str_array->index);
}

static void mglNextParamBooleanArray(MGLStringPairArray* str_array, const char* par, const GLboolean* val, size_t count)
{
    MGLStringInternal* out_par = &(str_array->first[str_array->index]);
    MGLStringInternal* out_val = &(str_array->second[str_array->index]);

    mglStringInternalInitWith(out_par, par);
    mglStringInternalInit(out_val, 0);

    mglStringInternalAppendCStr(out_val, "{ ");

    for (size_t i = 0; i < count; ++i)
    {
        mglStringInternalAppendCStr(out_val, (val[i] != GL_FALSE ? "GL_TRUE" : "GL_FALSE"));
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

static const char* mglCullFaceModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_FRONT );
        MGL_CASE2STR( GL_BACK );
        MGL_CASE2STR( GL_FRONT_AND_BACK );
    }
    return NULL;
}

static const char* mglPolygonModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_POINT );
        MGL_CASE2STR( GL_LINE );
        MGL_CASE2STR( GL_FILL );
    }
    return NULL;
}

static const char* mglFrontFaceStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_CW );
        MGL_CASE2STR( GL_CCW );
    }
    return NULL;
}

static const char* mglLogicOpModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_CLEAR );
        MGL_CASE2STR( GL_SET );
        MGL_CASE2STR( GL_COPY );
        MGL_CASE2STR( GL_COPY_INVERTED );
        MGL_CASE2STR( GL_NOOP );
        MGL_CASE2STR( GL_INVERT );
        MGL_CASE2STR( GL_AND );
        MGL_CASE2STR( GL_NAND );
        MGL_CASE2STR( GL_OR );
        MGL_CASE2STR( GL_NOR );
        MGL_CASE2STR( GL_XOR );
        MGL_CASE2STR( GL_EQUIV );
        MGL_CASE2STR( GL_AND_REVERSE );
        MGL_CASE2STR( GL_AND_INVERTED );
        MGL_CASE2STR( GL_OR_REVERSE );
        MGL_CASE2STR( GL_OR_INVERTED );
    }
    return NULL;
}

static const char* mglCompressedTextureInternalFormatStr(GLenum param)
{
    switch (param)
    {
        #if defined GL_VERSION_1_3
        MGL_CASE2STR( GL_COMPRESSED_RED );
        MGL_CASE2STR( GL_COMPRESSED_RG );
        MGL_CASE2STR( GL_COMPRESSED_RGB );
        MGL_CASE2STR( GL_COMPRESSED_RGBA );
        #elif defined GL_ARB_texture_compression
        MGL_CASE2STR( GL_COMPRESSED_ALPHA_ARB );
        MGL_CASE2STR( GL_COMPRESSED_LUMINANCE_ARB );
        MGL_CASE2STR( GL_COMPRESSED_LUMINANCE_ALPHA_ARB );
        MGL_CASE2STR( GL_COMPRESSED_INTENSITY_ARB );
        MGL_CASE2STR( GL_COMPRESSED_RGB_ARB );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ARB );
        #endif

        #if defined GL_VERSION_2_1
        MGL_CASE2STR( GL_COMPRESSED_SRGB );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA );
        #elif defined GL_EXT_texture_sRGB
        MGL_CASE2STR( GL_COMPRESSED_SRGB_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA_EXT );
        #endif

        #if defined GL_EXT_texture_sRGB
        MGL_CASE2STR( GL_COMPRESSED_SLUMINANCE_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SLUMINANCE_ALPHA_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_S3TC_DXT1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT );
        #endif

        #if defined GL_VERSION_3_0
        MGL_CASE2STR( GL_COMPRESSED_RED_RGTC1 );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_RED_RGTC1 );
        MGL_CASE2STR( GL_COMPRESSED_RG_RGTC2 );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_RG_RGTC2 );
        #elif defined GL_EXT_texture_compression_rgtc
        MGL_CASE2STR( GL_COMPRESSED_RED_RGTC1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_RED_RGTC1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_RED_GREEN_RGTC2_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT );
        #endif

        #if defined GL_VERSION_4_2
        MGL_CASE2STR( GL_COMPRESSED_RGBA_BPTC_UNORM );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM );
        MGL_CASE2STR( GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT );
        MGL_CASE2STR( GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT );
        #elif defined GL_ARB_texture_compression_bptc
        MGL_CASE2STR( GL_COMPRESSED_RGBA_BPTC_UNORM_ARB );
        MGL_CASE2STR( GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB );
        MGL_CASE2STR( GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB );
        MGL_CASE2STR( GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB );
        #endif

        #if defined GL_VERSION_4_3
        MGL_CASE2STR( GL_COMPRESSED_RGB8_ETC2 );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ETC2 );
        MGL_CASE2STR( GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 );
        MGL_CASE2STR( GL_COMPRESSED_RGBA8_ETC2_EAC );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC );
        MGL_CASE2STR( GL_COMPRESSED_R11_EAC );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_R11_EAC );
        MGL_CASE2STR( GL_COMPRESSED_RG11_EAC );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_RG11_EAC );
        #endif

        #if defined GL_KHR_texture_compression_astc_hdr
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_4x4_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_5x4_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_5x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_6x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_6x6_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_8x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_8x6_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_8x8_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_10x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_10x6_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_10x8_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_10x10_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_12x10_KHR );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_ASTC_12x12_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR );
        MGL_CASE2STR( GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR );
        #endif

        #if defined GL_3DFX_texture_compression_FXT1
        MGL_CASE2STR( GL_COMPRESSED_RGB_FXT1_3DFX );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_FXT1_3DFX );
        #endif

        #if defined GL_EXT_texture_compression_latc
        MGL_CASE2STR( GL_COMPRESSED_LUMINANCE_LATC1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT );
        MGL_CASE2STR( GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT );
        #endif

        #if defined GL_EXT_texture_compression_s3tc
        MGL_CASE2STR( GL_COMPRESSED_RGB_S3TC_DXT1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_S3TC_DXT1_EXT );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_S3TC_DXT3_EXT );
        MGL_CASE2STR( GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
        #endif

        #if defined GL_OES_compressed_paletted_texture
        MGL_CASE2STR( GL_PALETTE4_RGB8_OES );
        MGL_CASE2STR( GL_PALETTE4_RGBA8_OES );
        MGL_CASE2STR( GL_PALETTE4_R5_G6_B5_OES );
        MGL_CASE2STR( GL_PALETTE4_RGBA4_OES );
        MGL_CASE2STR( GL_PALETTE4_RGB5_A1_OES );
        MGL_CASE2STR( GL_PALETTE8_RGB8_OES );
        MGL_CASE2STR( GL_PALETTE8_RGBA8_OES );
        MGL_CASE2STR( GL_PALETTE8_R5_G6_B5_OES );
        MGL_CASE2STR( GL_PALETTE8_RGBA4_OES );
        MGL_CASE2STR( GL_PALETTE8_RGB5_A1_OES );
        #endif
    }
    return NULL;
}

#ifdef GL_VERSION_1_3

static const char* mglTextureStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_TEXTURE0 );
        MGL_CASE2STR( GL_TEXTURE1 );
        MGL_CASE2STR( GL_TEXTURE2 );
        MGL_CASE2STR( GL_TEXTURE3 );
        MGL_CASE2STR( GL_TEXTURE4 );
        MGL_CASE2STR( GL_TEXTURE5 );
        MGL_CASE2STR( GL_TEXTURE6 );
        MGL_CASE2STR( GL_TEXTURE7 );
        MGL_CASE2STR( GL_TEXTURE8 );
        MGL_CASE2STR( GL_TEXTURE9 );
        MGL_CASE2STR( GL_TEXTURE10 );
        MGL_CASE2STR( GL_TEXTURE11 );
        MGL_CASE2STR( GL_TEXTURE12 );
        MGL_CASE2STR( GL_TEXTURE13 );
        MGL_CASE2STR( GL_TEXTURE14 );
        MGL_CASE2STR( GL_TEXTURE15 );
        MGL_CASE2STR( GL_TEXTURE16 );
        MGL_CASE2STR( GL_TEXTURE17 );
        MGL_CASE2STR( GL_TEXTURE18 );
        MGL_CASE2STR( GL_TEXTURE19 );
        MGL_CASE2STR( GL_TEXTURE20 );
        MGL_CASE2STR( GL_TEXTURE21 );
        MGL_CASE2STR( GL_TEXTURE22 );
        MGL_CASE2STR( GL_TEXTURE23 );
        MGL_CASE2STR( GL_TEXTURE24 );
        MGL_CASE2STR( GL_TEXTURE25 );
        MGL_CASE2STR( GL_TEXTURE26 );
        MGL_CASE2STR( GL_TEXTURE27 );
        MGL_CASE2STR( GL_TEXTURE28 );
        MGL_CASE2STR( GL_TEXTURE29 );
        MGL_CASE2STR( GL_TEXTURE30 );
        MGL_CASE2STR( GL_TEXTURE31 );
    }
    return NULL;
}

#endif // /GL_VERSION_1_3

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
        #if defined GL_COLOR_ATTACHMENT16 && defined GL_COLOR_ATTACHMENT31
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
        #endif
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

#ifdef GL_VERSION_4_5

static const char* mglClipOriginStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_LOWER_LEFT );
        MGL_CASE2STR( GL_UPPER_LEFT );
    }
    return NULL;
}

static const char* mglClipDepthModeStr(GLenum param)
{
    switch (param)
    {
        MGL_CASE2STR( GL_NEGATIVE_ONE_TO_ONE );
        MGL_CASE2STR( GL_ZERO_TO_ONE );
    }
    return NULL;
}

#endif // /GL_VERSION_4_5

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
//      PUBLIC FUNCTION IMPLEMENTATIONS
// *****************************************************************

void mglQueryRenderState(MGLRenderState* rs)
{
    static const GLenum g_drawBufferPnames[16] = { GL_DRAW_BUFFER0,  GL_DRAW_BUFFER1,  GL_DRAW_BUFFER2,  GL_DRAW_BUFFER3,
                                                   GL_DRAW_BUFFER4,  GL_DRAW_BUFFER5,  GL_DRAW_BUFFER6,  GL_DRAW_BUFFER7,
                                                   GL_DRAW_BUFFER8,  GL_DRAW_BUFFER9,  GL_DRAW_BUFFER10, GL_DRAW_BUFFER11,
                                                   GL_DRAW_BUFFER12, GL_DRAW_BUFFER13, GL_DRAW_BUFFER14, GL_DRAW_BUFFER15 };

    // Get last GL error
    //GLenum last_err = glGetError();

    // Query all OpenGL states
    memset(rs, 0, sizeof(MGLRenderState));

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
    glGetIntegerv(GL_FRONT_FACE, &(rs->iFrontFace));
    glGetBooleanv(GL_LINE_SMOOTH, &(rs->bLineSmooth));
    glGetIntegerv(GL_LINE_SMOOTH_HINT, &(rs->iLineSmoothHint));
    glGetFloatv(GL_LINE_WIDTH, &(rs->fLineWidth));
    glGetIntegerv(GL_LOGIC_OP_MODE, &(rs->iLogicOpMode));
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(rs->iMaxTextureSize));
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &(rs->iMaxViewportDims[0]));
    glGetIntegerv(GL_PACK_ALIGNMENT, &(rs->iPackAlignment));
    glGetBooleanv(GL_PACK_LSB_FIRST, &(rs->bPackLSBFirst));
    glGetIntegerv(GL_PACK_ROW_LENGTH, &(rs->iPackRowLength));
    glGetIntegerv(GL_PACK_SKIP_PIXELS, &(rs->iPackSkipPixels));
    glGetIntegerv(GL_PACK_SKIP_ROWS, &(rs->iPackSkipRows));
    glGetBooleanv(GL_PACK_SWAP_BYTES, &(rs->bPackSwapBytes));
    glGetFloatv(GL_POINT_SIZE, &(rs->fPointSize));
    glGetFloatv(GL_POINT_SIZE_GRANULARITY, &(rs->fPointSizeGranularity));
    glGetFloatv(GL_POINT_SIZE_RANGE, &(rs->fPointSizeRange[0]));
    glGetIntegerv(GL_POLYGON_MODE, &(rs->iPolygonMode[0]));
    glGetBooleanv(GL_POLYGON_SMOOTH, &(rs->bPolygonSmooth));
    glGetIntegerv(GL_POLYGON_SMOOTH_HINT, &(rs->iPolygonSmoothHint));
    glGetIntegerv(GL_READ_BUFFER, &(rs->iReadBuffer));
    glGetIntegerv(GL_SCISSOR_BOX, &(rs->iScissorBox[0]));
    glGetBooleanv(GL_SCISSOR_TEST, &(rs->bScissorTest));
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
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &(rs->iTextureBinding2D));
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &(rs->iUnpackAlignment));
    glGetBooleanv(GL_UNPACK_LSB_FIRST, &(rs->bUnpackLSBFirst));
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &(rs->iUnpackRowLength));
    glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &(rs->iUnpackSkipPixels));
    glGetIntegerv(GL_UNPACK_SKIP_ROWS, &(rs->iUnpackSkipRows));
    glGetBooleanv(GL_UNPACK_SWAP_BYTES, &(rs->bUnpackSwapBytes));
    glGetIntegerv(GL_VIEWPORT, &(rs->iViewport[0]));

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
        glGetIntegerv(GL_SAMPLE_BUFFERS, &(rs->iSampleBuffers));
        glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &(rs->fSampleCoverageValue));
        glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT, &(rs->bSampleCoverageInvert));
        glGetIntegerv(GL_SAMPLES, &(rs->iSamples));
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
        glGetIntegerv(GL_NUM_EXTENSIONS, &(rs->iNumExtensions));

        glGetIntegerv(GL_CONTEXT_FLAGS, &(rs->iContextFlags));
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &(rs->iDrawFramebufferBinding));
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &(rs->iMaxArrayTextureLayers));
        glGetIntegerv(GL_MAX_CLIP_DISTANCES, &(rs->iMaxClipDistances));
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &(rs->iMaxRenderbufferSize));
        glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &(rs->iMaxVaryingComponents));
        glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &(rs->iMinProgramTexelOffest));
        glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &(rs->iMaxProgramTexelOffest));
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &(rs->iReadFramebufferBinding));
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &(rs->iRenderbufferBinding));
        glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &(rs->iTextureBinding1DArray));
        glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &(rs->iTextureBinding2DArray));
        mglGetIntegerStaticArray(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &(rs->iTransformFeedbackBufferBinding[0]), MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS);
        mglGetInteger64StaticArray(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, &(rs->iTransformFeedbackBufferSize[0]), MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS);
        mglGetInteger64StaticArray(GL_TRANSFORM_FEEDBACK_BUFFER_START, &(rs->iTransformFeedbackBufferStart[0]), MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &(rs->iVertexArrayBinding));
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
        glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &(rs->iMaxRectangleTextureSize));
        glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &(rs->iMaxTextureBufferSize));
        glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &(rs->iMaxUniformBufferBindings));
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &(rs->iMaxUniformBlockSize));
        glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, &(rs->iPrimitiveRestartIndex));
        glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &(rs->iTextureBindingBuffer));
        glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &(rs->iTextureBindingRectangle));
        mglGetIntegerStaticArray(GL_UNIFORM_BUFFER_BINDING, &(rs->iUniformBufferBinding[0]), MGL_MAX_UNIFORM_BUFFER_BINDINGS);
        mglGetInteger64StaticArray(GL_UNIFORM_BUFFER_SIZE, &(rs->iUniformBufferSize[0]), MGL_MAX_UNIFORM_BUFFER_BINDINGS);
        mglGetInteger64StaticArray(GL_UNIFORM_BUFFER_START, &(rs->iUniformBufferStart[0]), MGL_MAX_UNIFORM_BUFFER_BINDINGS);
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &(rs->iUniformBufferOffsetAlignment));
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
        glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &(rs->iMaxSampleMaskWords));
        glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, &(rs->iMaxServerWaitTimeout));
        glGetBooleanv(GL_PROGRAM_POINT_SIZE, &(rs->bProgramPointSize));
        glGetIntegerv(GL_PROVOKING_VERTEX, &(rs->iProvokingVertex));
        glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &(rs->iTextureBinding2DMultisample));
        glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &(rs->iTextureBinding2DMultisampleArray));
    }
    #endif // /GL_VERSION_3_2

    // *****************************************************************
    //      GL_VERSION_3_3
    // *****************************************************************

    #ifdef GL_VERSION_3_3
    if (MGL_VERSION_MIN(3, 3))
    {
        glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &(rs->iMaxDualSourceDrawBuffers));
        glGetIntegerv(GL_SAMPLER_BINDING, &(rs->iSamplerBinding));
        glGetInteger64v(GL_TIMESTAMP, &(rs->iTimestamp));
    }
    #endif // /GL_VERSION_3_3

    // *****************************************************************
    //      GL_VERSION_4_0
    // *****************************************************************

    #ifdef GL_VERSION_4_0
    if (MGL_VERSION_MIN(4, 0))
    {
        glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &(rs->iMaxTransformFeedbackBuffers));
        glGetIntegerv(GL_PATCH_DEFAULT_INNER_LEVEL, &(rs->iPatchDefaultInnerLevel));
        glGetIntegerv(GL_PATCH_DEFAULT_OUTER_LEVEL, &(rs->iPatchDefaultOuterLevel));
        glGetIntegerv(GL_PATCH_VERTICES, &(rs->iPatchVertices));
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
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &(rs->iMaxVertexUniformVectors));
        glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &(rs->iNumShaderBinaryFormats));
        mglGetIntegerDynamicArray(GL_SHADER_BINARY_FORMATS, &(rs->iShaderBinaryFormats[0]), rs->iNumShaderBinaryFormats, MGL_MAX_SHADER_BINARY_FORMATS);
        glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &(rs->iNumProgramBinaryFormats));
        mglGetIntegerDynamicArray(GL_PROGRAM_BINARY_FORMATS, &(rs->iProgramBinaryFormats[0]), rs->iNumProgramBinaryFormats, MGL_MAX_PROGRAM_BINARY_FORMATS);
        glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, &(rs->iProgramPipelineBinding));
        glGetBooleanv(GL_SHADER_COMPILER, &(rs->bShaderCompiler));
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
        mglGetIntegerStaticArray(GL_MAX_COMPUTE_WORK_GROUP_COUNT, &(rs->iMaxComputeWorkGroupCount[0]), 3);
        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &(rs->iMaxComputeWorkGroup));
        mglGetIntegerStaticArray(GL_MAX_COMPUTE_WORK_GROUP_SIZE, &(rs->iMaxComputeWorkGroupSize[0]), 3);
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
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &(rs->iMaxShaderStorageBufferBindings));
        mglGetIntegerStaticArray(GL_SHADER_STORAGE_BUFFER_BINDING, &(rs->iShaderStorageBufferBinding[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &(rs->iShaderStorageBufferOffsetAlignment));
        mglGetInteger64StaticArray(GL_SHADER_STORAGE_BUFFER_SIZE, &(rs->iShaderStorageBufferSize[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        mglGetInteger64StaticArray(GL_SHADER_STORAGE_BUFFER_START, &(rs->iShaderStorageBufferStart[0]), MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    }
    #endif // /GL_VERSION_4_3

    // *****************************************************************
    //      GL_VERSION_4_5
    // *****************************************************************

    #ifdef GL_VERSION_4_5
    if (MGL_VERSION_MIN(4, 5))
    {
        glGetIntegerv(GL_CLIP_DEPTH_MODE, &(rs->iClipDepthMode));
        glGetIntegerv(GL_CLIP_ORIGIN, &(rs->iClipOrigin));
    }
    #endif // /GL_VERSION_4_5

    #undef MGL_VERSION
    #undef MGL_VERSION_MIN
}

void mglQueryBindingPoints(MGLBindingPoints* bp)
{
    memset(bp, 0, sizeof(MGLBindingPoints));

    GLint iMajorVersion = 0, iMinorVersion = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &iMajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &iMinorVersion);

    #define MGL_VERSION(MAJOR, MINOR)       (((MAJOR) << 16) | (MINOR))
    #define MGL_VERSION_MIN(MAJOR, MINOR)   (MGL_VERSION(iMajorVersion, iMinorVersion) >= MGL_VERSION(MAJOR, MINOR))

    #ifdef GL_VERSION_1_3
    if (MGL_VERSION_MIN(1, 3))
    {
        // Store current active texture layer
        GLint iPrevActiveTexture = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &iPrevActiveTexture);

        // Query texture types for all layers [GL_TEXTURE0 .. GL_TEXTURE31]
        for (GLint layer = 0; layer < MGL_MAX_TEXTURE_LAYERS; ++layer)
        {
            glActiveTexture(GL_TEXTURE0 + layer);

            glGetIntegerv(GL_TEXTURE_BINDING_1D, &(bp->iTextureBinding1D[layer]));
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &(bp->iTextureBinding2D[layer]));
            glGetIntegerv(GL_TEXTURE_BINDING_3D, &(bp->iTextureBinding3D[layer]));
            glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &(bp->iTextureBindingCubeMap[layer]));

            #ifdef GL_VERSION_3_0
            if (MGL_VERSION_MIN(3, 0))
            {
                glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &(bp->iTextureBinding1DArray[layer]));
                glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &(bp->iTextureBinding2DArray[layer]));
            }
            #endif // /GL_VERSION_3_0

            #ifdef GL_VERSION_3_1
            if (MGL_VERSION_MIN(3, 1))
            {
                glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &(bp->iTextureBindingBuffer[layer]));
                glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &(bp->iTextureBindingRectangle[layer]));
            }
            #endif // /GL_VERSION_3_1

            #ifdef GL_VERSION_3_2
            if (MGL_VERSION_MIN(3, 2))
            {
                glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &(bp->iTextureBinding2DMultisample[layer]));
                glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &(bp->iTextureBinding2DMultisampleArray[layer]));
            }
            #endif // /GL_VERSION_3_2
        }

        // Restore previous active texture layer
        glActiveTexture(iPrevActiveTexture);
    }
    else
    #endif // /GL_VERSION_1_3
    {
        // Only query first layer for texture types supported up to GL 1.2
        glGetIntegerv(GL_TEXTURE_BINDING_1D, &(bp->iTextureBinding1D[0]));
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &(bp->iTextureBinding2D[0]));

        #ifdef GL_VERSION_1_2
        if (MGL_VERSION_MIN(1, 2))
        {
            glGetIntegerv(GL_TEXTURE_BINDING_3D, &(bp->iTextureBinding3D[0]));
        }
        #endif // /GL_VERSION_1_2
    }

    #undef MGL_VERSION
    #undef MGL_VERSION_MIN
}

static MGLString mglPrintStringPairs(MGLStringPairArray out, const MGLFormattingOptions* formatting)
{
    // Determine longest parameter name
    size_t max_par_len = 0;

    for (size_t i = 0; i < out.index; ++i)
    {
        if (formatting->filter == NULL || strstr(out.first[i].buf, formatting->filter) != NULL)
            max_par_len = MGL_MAX(max_par_len, out.first[i].len);
    }

    max_par_len += formatting->distance;

    // Determine output string capacity
    size_t out_cap = 0;

    for (size_t i = 0; i < out.index; ++i)
    {
        if (formatting->filter == NULL || strstr(out.first[i].buf, formatting->filter) != NULL)
        {
            out_cap += out.first[i].len;
            out_cap += (max_par_len - out.first[i].len);
            out_cap += out.second[i].len;
            out_cap += 1;
        }
    }

    // Allocate output string
    MGLStringInternal* s = MGL_MALLOC(MGLStringInternal);
    mglStringInternalInit(s, out_cap);

    // Change order of strings
    int out_permutations[MGL_MAX_NUM_RENDER_STATES];

    for (size_t i = 0; i < MGL_MAX_NUM_RENDER_STATES; ++i)
        out_permutations[i] = i;

    if (formatting->order == MGLFormattingOrderSorted)
    {
        g_MGLOutputParamas = out.first;
        qsort(out_permutations, MGL_MAX_NUM_RENDER_STATES, sizeof(int), mglCompareOutputPair);
    }

    // Merge all strings parts to the output string
    for (size_t i = 0; i < out.index; ++i)
    {
        int out_i = out_permutations[i];

        if (formatting->filter == NULL || strstr(out.first[out_i].buf, formatting->filter) != NULL)
        {
            // Append string parts to output string
            const MGLStringInternal* cur_par = &(out.first[out_i]);
            const MGLStringInternal* cur_val = &(out.second[out_i]);
            const size_t cur_spaces_len = max_par_len - out.first[out_i].len;

            mglStringInternalAppend(s, cur_par);
            mglStringInternalResize(s, s->len + cur_spaces_len, formatting->separator);

            if (cur_val->len > formatting->array_limit && cur_val->buf[cur_val->len - 1] == '}')
            {
                // Append string in multiple lines, one for each element
                size_t off = 0, next_off = 0;

                while (off < cur_val->len)
                {
                    // Find end of current element
                    next_off = mglStringInternalFindChar(cur_val, ',', off, MGL_STRING_NPOS);
                    if (next_off != MGL_STRING_NPOS)
                    {
                        mglStringInternalAppendSub(s, cur_val, off, next_off - off + 1);
                        mglStringInternalAppendCStr(s, "\n");
                        mglStringInternalResize(s, s->len + cur_par->len + cur_spaces_len + 1, formatting->separator);
                        off = next_off + 1;
                    }
                    else
                    {
                        mglStringInternalAppendSub(s, cur_val, off, MGL_STRING_NPOS);
                        break;
                    }
                }
            }
            else
                mglStringInternalAppend(s, cur_val);

            mglStringInternalAppendCStr(s, "\n");
        }

        // Free string parts
        mglStringInternalFree(&(out.first[out_i]));
        mglStringInternalFree(&(out.second[out_i]));
    }

    return (MGLString)s;
}

MGLString mglPrintRenderState(const MGLRenderState* rs, const MGLFormattingOptions* formatting)
{
    // Internal constant parameters
    static const MGLFormattingOptions   g_formattingDefault = { ' ', 1, 200, MGLFormattingOrderDefault, 1, NULL };
    static const char*                  g_valNA             = "n/a";
    //static const char*                  g_valNotYetImpl     = "< not yet implemented >";

    if (formatting == NULL)
        formatting = (&g_formattingDefault);

    // Provide array with all string parts
    MGLStringInternal out_par[MGL_MAX_NUM_RENDER_STATES], out_val[MGL_MAX_NUM_RENDER_STATES];

    memset(out_par, 0, sizeof(out_par));
    memset(out_val, 0, sizeof(out_val));

    MGLStringPairArray out = { out_par, out_val, 0 };

    #define MGL_VERSION(MAJOR, MINOR)       (((MAJOR) << 16) | (MINOR))
    #define MGL_VERSION_MIN(MAJOR, MINOR)   (MGL_VERSION(rs->iMajorVersion, rs->iMinorVersion) >= MGL_VERSION(MAJOR, MINOR))
    #define MGL_PARAM_UNAVAIL(NAME)         mglNextParamString(&out, #NAME, g_valNA)
    //#define MGL_PARAM_NOT_YET_IMPL(NAME)    mglNextParamString(&out, #NAME, g_valNotYetImpl)

    // *****************************************************************
    //      GL_VERSION_1_0
    // *****************************************************************

    mglNextParamInteger(&out, "GL_MAJOR_VERSION", rs->iMajorVersion);
    mglNextParamInteger(&out, "GL_MINOR_VERSION", rs->iMinorVersion);

    mglNextParamBoolean(&out, "GL_BLEND", rs->bBlend);
    mglNextParamFloatArray(&out, "GL_COLOR_CLEAR_VALUE", rs->fColorClearValue, 4);
    mglNextParamBooleanArray(&out, "GL_COLOR_WRITEMASK", rs->bColorWriteMask, 4);
    mglNextParamBoolean(&out, "GL_CULL_FACE", rs->bCullFace);
    mglNextParamEnum(&out, "GL_CULL_FACE_MODE", rs->iCullFaceMode, mglCullFaceModeStr);
    mglNextParamDouble(&out, "GL_DEPTH_CLEAR_VALUE", rs->dDepthClearValue);
    mglNextParamEnum(&out, "GL_DEPTH_FUNC", rs->iDepthFunc, mglCompareFuncStr);
    mglNextParamDoubleArray(&out, "GL_DEPTH_RANGE", rs->dDepthRange, 2);
    mglNextParamBoolean(&out, "GL_DEPTH_TEST", rs->bDepthTest);
    mglNextParamBoolean(&out, "GL_DEPTH_WRITEMASK", rs->bDepthWriteMask);
    mglNextParamBoolean(&out, "GL_DITHER", rs->bDither);
    mglNextParamBoolean(&out, "GL_DOUBLEBUFFER", rs->bDoubleBuffer);
    mglNextParamInteger(&out, "GL_DRAW_BUFFER", rs->iDrawBuffer);
    mglNextParamEnum(&out, "GL_FRONT_FACE", rs->iFrontFace, mglFrontFaceStr);
    mglNextParamBoolean(&out, "GL_LINE_SMOOTH", rs->bLineSmooth);
    mglNextParamEnum(&out, "GL_LINE_SMOOTH_HINT", rs->iLineSmoothHint, mglHintModeStr);
    mglNextParamFloat(&out, "GL_LINE_WIDTH", rs->fLineWidth);
    mglNextParamEnum(&out, "GL_LOGIC_OP_MODE", rs->iLogicOpMode, mglLogicOpModeStr);
    mglNextParamInteger(&out, "GL_MAX_TEXTURE_SIZE", rs->iMaxTextureSize);
    mglNextParamIntegerArray(&out, "GL_MAX_VIEWPORT_DIMS", rs->iMaxViewportDims, 2, 2, 0);
    mglNextParamInteger(&out, "GL_PACK_ALIGNMENT", rs->iPackAlignment);
    mglNextParamBoolean(&out, "GL_PACK_LSB_FIRST", rs->bPackLSBFirst);
    mglNextParamInteger(&out, "GL_PACK_ROW_LENGTH", rs->iPackRowLength);
    mglNextParamInteger(&out, "GL_PACK_SKIP_PIXELS", rs->iPackSkipPixels);
    mglNextParamInteger(&out, "GL_PACK_SKIP_ROWS", rs->iPackSkipRows);
    mglNextParamBoolean(&out, "GL_PACK_SWAP_BYTES", rs->bPackSwapBytes);
    mglNextParamFloat(&out, "GL_POINT_SIZE", rs->fPointSize);
    mglNextParamFloat(&out, "GL_POINT_SIZE_GRANULARITY", rs->fPointSizeGranularity);
    mglNextParamFloatArray(&out, "GL_POINT_SIZE_RANGE", rs->fPointSizeRange, 2);
    mglNextParamEnumArray(&out, "GL_POLYGON_MODE", rs->iPolygonMode, 2, 2, mglPolygonModeStr);
    mglNextParamBoolean(&out, "GL_POLYGON_SMOOTH", rs->bPolygonSmooth);
    mglNextParamEnum(&out, "GL_POLYGON_SMOOTH_HINT", rs->iPolygonSmoothHint, mglHintModeStr);
    mglNextParamInteger(&out, "GL_READ_BUFFER", rs->iReadBuffer);
    mglNextParamIntegerArray(&out, "GL_SCISSOR_BOX", rs->iScissorBox, 4, 4, 0);
    mglNextParamBoolean(&out, "GL_SCISSOR_TEST", rs->bScissorTest);
    mglNextParamInteger(&out, "GL_STENCIL_CLEAR_VALUE", rs->iStencilClearValue);
    mglNextParamEnum(&out, "GL_STENCIL_FAIL", rs->iStencilFail, mglStencilOpStr);
    mglNextParamEnum(&out, "GL_STENCIL_FUNC", rs->iStencilFunc, mglCompareFuncStr);
    mglNextParamEnum(&out, "GL_STENCIL_PASS_DEPTH_FAIL", rs->iStencilPassDepthFail, mglStencilOpStr);
    mglNextParamEnum(&out, "GL_STENCIL_PASS_DEPTH_PASS", rs->iStencilPassDepthPass, mglStencilOpStr);
    mglNextParamInteger(&out, "GL_STENCIL_REF", rs->iStencilRef);
    mglNextParamBoolean(&out, "GL_STENCIL_TEST", rs->bStencilTest);
    mglNextParamIntegerHex(&out, "GL_STENCIL_VALUE_MASK", rs->iStencilValueMask);
    mglNextParamIntegerHex(&out, "GL_STENCIL_WRITEMASK", rs->iStencilWriteMask);
    mglNextParamBoolean(&out, "GL_STEREO", rs->bStereo);
    mglNextParamInteger(&out, "GL_SUBPIXEL_BITS", rs->iSubPixelBits);
    mglNextParamInteger(&out, "GL_TEXTURE_BINDING_1D", rs->iTextureBinding1D);
    mglNextParamInteger(&out, "GL_TEXTURE_BINDING_2D", rs->iTextureBinding2D);
    mglNextParamInteger(&out, "GL_UNPACK_ALIGNMENT", rs->iUnpackAlignment);
    mglNextParamBoolean(&out, "GL_UNPACK_LSB_FIRST", rs->bUnpackLSBFirst);
    mglNextParamInteger(&out, "GL_UNPACK_ROW_LENGTH", rs->iUnpackRowLength);
    mglNextParamInteger(&out, "GL_UNPACK_SKIP_PIXELS", rs->iUnpackSkipPixels);
    mglNextParamInteger(&out, "GL_UNPACK_SKIP_ROWS", rs->iUnpackSkipRows);
    mglNextParamBoolean(&out, "GL_UNPACK_SWAP_BYTES", rs->bUnpackSwapBytes);
    mglNextParamIntegerArray(&out, "GL_VIEWPORT", rs->iViewport, 4, 4, 0);

    // *****************************************************************
    //      GL_VERSION_1_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_1");

    #ifdef GL_VERSION_1_1
    if (MGL_VERSION_MIN(1, 1))
    {
        mglNextParamBoolean(&out, "GL_COLOR_LOGIC_OP", rs->bColorLogicOp);
        mglNextParamFloat(&out, "GL_POLYGON_OFFSET_FACTOR", rs->fPolygonOffsetFactor);
        mglNextParamFloat(&out, "GL_POLYGON_OFFSET_UNITS", rs->fPolygonOffsetUnits);
        mglNextParamBoolean(&out, "GL_POLYGON_OFFSET_FILL", rs->bPolygonOffsetFill);
        mglNextParamBoolean(&out, "GL_POLYGON_OFFSET_LINE", rs->bPolygonOffsetLine);
        mglNextParamBoolean(&out, "GL_POLYGON_OFFSET_POINT", rs->bPolygonOffsetPoint);
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
        mglNextParamFloatArray(&out, "GL_ALIASED_LINE_WIDTH_RANGE", rs->fAliasedLineWidthRange, 2);
        mglNextParamFloatArray(&out, "GL_BLEND_COLOR", rs->fBlendColor, 4);
        mglNextParamInteger(&out, "GL_MAX_3D_TEXTURE_SIZE", rs->iMax3DTextureSize);
        mglNextParamInteger(&out, "GL_MAX_ELEMENTS_INDICES", rs->iMaxElementsIndices);
        mglNextParamInteger(&out, "GL_MAX_ELEMENTS_VERTICES", rs->iMaxElementsVertices);
        mglNextParamInteger(&out, "GL_PACK_IMAGE_HEIGHT", rs->iPackImageHeight);
        mglNextParamInteger(&out, "GL_PACK_SKIP_IMAGES", rs->iPackSkipImages);
        mglNextParamFloatArray(&out, "GL_SMOOTH_LINE_WIDTH_RANGE", rs->fSmoothLineWidthRange, 2);
        mglNextParamFloat(&out, "GL_SMOOTH_LINE_WIDTH_GRANULARITY", rs->fSmoothLineWidthGranularity);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_3D", rs->iTextureBinding3D);
        mglNextParamInteger(&out, "GL_UNPACK_IMAGE_HEIGHT", rs->iUnpackImageHeight);
        mglNextParamInteger(&out, "GL_UNPACK_SKIP_IMAGES", rs->iUnpackSkipImages);
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
        mglNextParamInteger(&out, "GL_NUM_COMPRESSED_TEXTURE_FORMATS", rs->iNumCompressedTextureFormats);
        mglNextParamEnumArray(&out, "GL_COMPRESSED_TEXTURE_FORMATS", rs->iCompressedTextureFormats, (size_t)rs->iNumCompressedTextureFormats, MGL_MAX_COMPRESSED_TEXTURE_FORMATS, mglCompressedTextureInternalFormatStr);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_CUBE_MAP", rs->iTextureBindingCubeMap);
        mglNextParamEnum(&out, "GL_TEXTURE_COMPRESSION_HINT", rs->iTextureCompressionHint, mglHintModeStr);
        mglNextParamEnum(&out, "GL_ACTIVE_TEXTURE", rs->iActiveTexture, mglTextureStr);
        mglNextParamInteger(&out, "GL_MAX_CUBE_MAP_TEXTURE_SIZE", rs->iMaxCubeMapTextureSize);
        mglNextParamInteger(&out, "GL_SAMPLE_BUFFERS", rs->iSampleBuffers);
        mglNextParamFloat(&out, "GL_SAMPLE_COVERAGE_VALUE", rs->fSampleCoverageValue);
        mglNextParamBoolean(&out, "GL_SAMPLE_COVERAGE_INVERT", rs->bSampleCoverageInvert);
        mglNextParamInteger(&out, "GL_SAMPLES", rs->iSamples);
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
        MGL_PARAM_UNAVAIL( GL_SAMPLE_BUFFERS );
        MGL_PARAM_UNAVAIL( GL_SAMPLE_COVERAGE_VALUE );
        MGL_PARAM_UNAVAIL( GL_SAMPLE_COVERAGE_INVERT );
        MGL_PARAM_UNAVAIL( GL_SAMPLES );
    }

    // *****************************************************************
    //      GL_VERSION_1_4
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_1_4");

    #ifdef GL_VERSION_1_4
    if (MGL_VERSION_MIN(1, 4))
    {
        mglNextParamEnum(&out, "GL_BLEND_DST_ALPHA", rs->iBlendDstAlpha, mglBlendFuncStr);
        mglNextParamEnum(&out, "GL_BLEND_DST_RGB", rs->iBlendDstRGB, mglBlendFuncStr);
        mglNextParamEnum(&out, "GL_BLEND_SRC_ALPHA", rs->iBlendSrcAlpha, mglBlendFuncStr);
        mglNextParamEnum(&out, "GL_BLEND_SRC_RGB", rs->iBlendSrcRGB, mglBlendFuncStr);
        mglNextParamFloat(&out, "GL_MAX_TEXTURE_LOD_BIAS", rs->fMaxTextureLODBias);
        mglNextParamFloat(&out, "GL_POINT_FADE_THRESHOLD_SIZE", rs->fPointFadeThresholdSize);
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
        mglNextParamInteger(&out, "GL_ARRAY_BUFFER_BINDING", rs->iArrayBufferBinding);
        mglNextParamInteger(&out, "GL_ELEMENT_ARRAY_BUFFER_BINDING", rs->iElementArrayBufferBinding);
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
        mglNextParamEnum(&out, "GL_BLEND_EQUATION_ALPHA", rs->iBlendEquationAlpha, mglBlendEquationModeStr);
        mglNextParamEnum(&out, "GL_BLEND_EQUATION_RGB", rs->iBlendEquationRGB, mglBlendEquationModeStr);
        mglNextParamInteger(&out, "GL_CURRENT_PROGRAM", rs->iCurrentProgram);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER0", rs->iDrawBuffer_i[0], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER1", rs->iDrawBuffer_i[1], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER2", rs->iDrawBuffer_i[2], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER3", rs->iDrawBuffer_i[3], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER4", rs->iDrawBuffer_i[4], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER5", rs->iDrawBuffer_i[5], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER6", rs->iDrawBuffer_i[6], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER7", rs->iDrawBuffer_i[7], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER8", rs->iDrawBuffer_i[8], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER9", rs->iDrawBuffer_i[9], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER10", rs->iDrawBuffer_i[10], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER11", rs->iDrawBuffer_i[11], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER12", rs->iDrawBuffer_i[12], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER13", rs->iDrawBuffer_i[13], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER14", rs->iDrawBuffer_i[14], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_DRAW_BUFFER15", rs->iDrawBuffer_i[15], mglDrawBufferModeStr);
        mglNextParamEnum(&out, "GL_FRAGMENT_SHADER_DERIVATIVE_HINT", rs->iFragmentShaderDerivativeHint, mglHintModeStr);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", rs->iMaxCombinedTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_DRAW_BUFFERS", rs->iMaxDrawBuffers);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS", rs->iMaxFragmentUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_TEXTURE_IMAGE_UNITS", rs->iMaxTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_VARYING_FLOATS", rs->iMaxVaryingFloats);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATTRIBS", rs->iMaxVertexAttribs);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", rs->iMaxVertexTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_UNIFORM_COMPONENTS", rs->iMaxVertexUniformComponents);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_FAIL", rs->iStencilBackFail, mglStencilOpStr);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_FUNC", rs->iStencilBackFunc, mglCompareFuncStr);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_PASS_DEPTH_FAIL", rs->iStencilBackPassDepthFail, mglStencilOpStr);
        mglNextParamEnum(&out, "GL_STENCIL_BACK_PASS_DEPTH_PASS", rs->iStencilBackPassDepthPass, mglStencilOpStr);
        mglNextParamInteger(&out, "GL_STENCIL_BACK_REF", rs->iStencilBackRef);
        mglNextParamIntegerHex(&out, "GL_STENCIL_BACK_VALUE_MASK", rs->iStencilBackValueMask);
        mglNextParamIntegerHex(&out, "GL_STENCIL_BACK_WRITEMASK", rs->iStencilBackWriteMask);
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
        mglNextParamInteger(&out, "GL_PIXEL_PACK_BUFFER_BINDING", rs->iPixelPackBufferBinding);
        mglNextParamInteger(&out, "GL_PIXEL_UNPACK_BUFFER_BINDING", rs->iPixelUnpackBufferBinding);
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
        mglNextParamBitfield(&out, "GL_CONTEXT_FLAGS", rs->iContextFlags, 32, mglContextFlagBitStr);
        mglNextParamInteger(&out, "GL_DRAW_FRAMEBUFFER_BINDING", rs->iDrawFramebufferBinding);
        mglNextParamInteger(&out, "GL_MAX_ARRAY_TEXTURE_LAYERS", rs->iMaxArrayTextureLayers);
        mglNextParamInteger(&out, "GL_MAX_CLIP_DISTANCES", rs->iMaxClipDistances);
        mglNextParamInteger(&out, "GL_MAX_RENDERBUFFER_SIZE", rs->iMaxRenderbufferSize);
        mglNextParamInteger(&out, "GL_MAX_VARYING_COMPONENTS", rs->iMaxVaryingComponents);
        mglNextParamInteger(&out, "GL_NUM_EXTENSIONS", rs->iNumExtensions);
        mglNextParamInteger(&out, "GL_MIN_PROGRAM_TEXEL_OFFSET", rs->iMinProgramTexelOffest);
        mglNextParamInteger(&out, "GL_MAX_PROGRAM_TEXEL_OFFSET", rs->iMaxProgramTexelOffest);
        mglNextParamInteger(&out, "GL_READ_FRAMEBUFFER_BINDING", rs->iReadFramebufferBinding);
        mglNextParamInteger(&out, "GL_RENDERBUFFER_BINDING", rs->iRenderbufferBinding);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_1D_ARRAY", rs->iTextureBinding1DArray);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_2D_ARRAY", rs->iTextureBinding2DArray);
        #ifdef MENTAL_GL_GETINTEGERI_V
        mglNextParamIntegerArray(&out, "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING", rs->iTransformFeedbackBufferBinding, MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, 0);
        #else
        MGL_PARAM_UNAVAIL( GL_TRANSFORM_FEEDBACK_BUFFER_BINDING );
        #endif
        #ifdef MENTAL_GL_GETINTEGER64I_V
        mglNextParamInteger64Array(&out, "GL_TRANSFORM_FEEDBACK_BUFFER_SIZE", rs->iTransformFeedbackBufferSize, MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS);
        mglNextParamInteger64Array(&out, "GL_TRANSFORM_FEEDBACK_BUFFER_START", rs->iTransformFeedbackBufferStart, MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS, MGL_MAX_TRANSFORM_FEEDBACK_BUFFER_BINDINGS);
        #else
        MGL_PARAM_UNAVAIL( GL_TRANSFORM_FEEDBACK_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_TRANSFORM_FEEDBACK_BUFFER_START );
        #endif
        mglNextParamInteger(&out, "GL_VERTEX_ARRAY_BINDING", rs->iVertexArrayBinding);
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
        MGL_PARAM_UNAVAIL( GL_NUM_EXTENSIONS );
        MGL_PARAM_UNAVAIL( GL_MIN_PROGRAM_TEXEL_OFFSET );
        MGL_PARAM_UNAVAIL( GL_MAX_PROGRAM_TEXEL_OFFSET );
        MGL_PARAM_UNAVAIL( GL_READ_FRAMEBUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_RENDERBUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_1D_ARRAY );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_2D_ARRAY );
        MGL_PARAM_UNAVAIL( GL_TRANSFORM_FEEDBACK_BUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_TRANSFORM_FEEDBACK_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_TRANSFORM_FEEDBACK_BUFFER_START );
        MGL_PARAM_UNAVAIL( GL_VERTEX_ARRAY_BINDING );
    }

    // *****************************************************************
    //      GL_VERSION_3_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_3_1");

    #ifdef GL_VERSION_3_1
    if (MGL_VERSION_MIN(3, 1))
    {
        mglNextParamInteger(&out, "GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS", rs->iMaxCombinedFragmentUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS", rs->iMaxCombinedGeometryUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS", rs->iMaxCombinedVertexUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_UNIFORM_BLOCKS", rs->iMaxCombinedUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_UNIFORM_BLOCKS", rs->iMaxFragmentUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_UNIFORM_BLOCKS", rs->iMaxGeometryUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_UNIFORM_BLOCKS", rs->iMaxVertexUniformBlocks);

        mglNextParamInteger(&out, "GL_MAX_RECTANGLE_TEXTURE_SIZE", rs->iMaxRectangleTextureSize);
        mglNextParamInteger(&out, "GL_MAX_TEXTURE_BUFFER_SIZE", rs->iMaxTextureBufferSize);
        mglNextParamInteger(&out, "GL_MAX_UNIFORM_BUFFER_BINDINGS", rs->iMaxUniformBufferBindings);
        mglNextParamInteger(&out, "GL_MAX_UNIFORM_BLOCK_SIZE", rs->iMaxUniformBlockSize);
        mglNextParamInteger(&out, "GL_PRIMITIVE_RESTART_INDEX", rs->iPrimitiveRestartIndex);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_BUFFER", rs->iTextureBindingBuffer);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_RECTANGLE", rs->iTextureBindingRectangle);
        #ifdef MENTAL_GL_GETINTEGERI_V
        mglNextParamIntegerArray(&out, "GL_UNIFORM_BUFFER_BINDING", rs->iUniformBufferBinding, MGL_MAX_UNIFORM_BUFFER_BINDINGS, MGL_MAX_UNIFORM_BUFFER_BINDINGS, 0);
        #else
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_BINDING );
        #endif
        #ifdef MENTAL_GL_GETINTEGER64I_V
        mglNextParamInteger64Array(&out, "GL_UNIFORM_BUFFER_SIZE", rs->iUniformBufferSize, MGL_MAX_UNIFORM_BUFFER_BINDINGS, MGL_MAX_UNIFORM_BUFFER_BINDINGS);
        mglNextParamInteger64Array(&out, "GL_UNIFORM_BUFFER_START", rs->iUniformBufferStart, MGL_MAX_UNIFORM_BUFFER_BINDINGS, MGL_MAX_UNIFORM_BUFFER_BINDINGS);
        #else
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_START );
        #endif
        mglNextParamInteger(&out, "GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT", rs->iUniformBufferOffsetAlignment);
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
        MGL_PARAM_UNAVAIL( GL_MAX_RECTANGLE_TEXTURE_SIZE );
        MGL_PARAM_UNAVAIL( GL_MAX_TEXTURE_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_MAX_UNIFORM_BUFFER_BINDINGS );
        MGL_PARAM_UNAVAIL( GL_MAX_UNIFORM_BLOCK_SIZE );
        MGL_PARAM_UNAVAIL( GL_PRIMITIVE_RESTART_INDEX );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_BUFFER );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_RECTANGLE );
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_START );
        MGL_PARAM_UNAVAIL( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT );
    }

    // *****************************************************************
    //      GL_VERSION_3_2
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_3_2");

    #ifdef GL_VERSION_3_2
    if (MGL_VERSION_MIN(3, 2))
    {
        mglNextParamInteger(&out, "GL_MAX_COLOR_TEXTURE_SAMPLES", rs->iMaxColorTextureSamples);
        mglNextParamInteger(&out, "GL_MAX_DEPTH_TEXTURE_SAMPLES", rs->iMaxDepthTextureSamples);
        mglNextParamInteger(&out, "GL_MAX_INTEGER_SAMPLES", rs->iMaxIntegerSamples);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_INPUT_COMPONENTS", rs->iMaxGeometryInputComponents);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_OUTPUT_COMPONENTS", rs->iMaxGeometryOutputComponents);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS", rs->iMaxGeometryTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_UNIFORM_COMPONENTS", rs->iMaxGeometryUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_INPUT_COMPONENTS", rs->iMaxFragmentInputComponents);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_OUTPUT_COMPONENTS", rs->iMaxVertexOutputComponents);
        mglNextParamInteger(&out, "GL_MAX_SAMPLE_MASK_WORDS", rs->iMaxSampleMaskWords);
        mglNextParamInteger(&out, "GL_MAX_SERVER_WAIT_TIMEOUT", rs->iMaxServerWaitTimeout);
        mglNextParamBoolean(&out, "GL_PROGRAM_POINT_SIZE", rs->bProgramPointSize);
        mglNextParamEnum(&out, "GL_PROVOKING_VERTEX", rs->iProvokingVertex, mglProvokingVertexModeStr);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_2D_MULTISAMPLE", rs->iTextureBinding2DMultisample);
        mglNextParamInteger(&out, "GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY", rs->iTextureBinding2DMultisampleArray);
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
        MGL_PARAM_UNAVAIL( GL_MAX_SAMPLE_MASK_WORDS );
        MGL_PARAM_UNAVAIL( GL_MAX_SERVER_WAIT_TIMEOUT );
        MGL_PARAM_UNAVAIL( GL_PROGRAM_POINT_SIZE );
        MGL_PARAM_UNAVAIL( GL_PROVOKING_VERTEX );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_2D_MULTISAMPLE );
        MGL_PARAM_UNAVAIL( GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY );
    }

    // *****************************************************************
    //      GL_VERSION_3_3
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_3_3");

    #ifdef GL_VERSION_3_3
    if (MGL_VERSION_MIN(3, 3))
    {
        mglNextParamInteger(&out, "GL_SAMPLER_BINDING", rs->iSamplerBinding);
        mglNextParamInteger64(&out, "GL_TIMESTAMP", rs->iTimestamp);
    }
    else
    #endif
    {
        MGL_PARAM_UNAVAIL( GL_SAMPLER_BINDING );
        MGL_PARAM_UNAVAIL( GL_TIMESTAMP );
    }

    // *****************************************************************
    //      GL_VERSION_4_0
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_0");

    #ifdef GL_VERSION_4_0
    if (MGL_VERSION_MIN(4, 0))
    {
        mglNextParamInteger(&out, "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS", rs->iMaxTransformFeedbackBuffers);
        mglNextParamInteger(&out, "GL_PATCH_DEFAULT_INNER_LEVEL", rs->iPatchDefaultInnerLevel);
        mglNextParamInteger(&out, "GL_PATCH_DEFAULT_OUTER_LEVEL", rs->iPatchDefaultOuterLevel);
        mglNextParamInteger(&out, "GL_PATCH_VERTICES", rs->iPatchVertices);
    }
    else
    #endif
    {
        MGL_PARAM_UNAVAIL( GL_MAX_TRANSFORM_FEEDBACK_BUFFERS );
    }

    // *****************************************************************
    //      GL_VERSION_4_1
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_1");

    #ifdef GL_VERSION_4_1
    if (MGL_VERSION_MIN(4, 1))
    {
        mglNextParamEnum(&out, "GL_IMPLEMENTATION_COLOR_READ_FORMAT", rs->iImplementationColorReadFormat, mglImplementationColorReadFormatStr);
        mglNextParamEnum(&out, "GL_IMPLEMENTATION_COLOR_READ_TYPE", rs->iImplementationColorReadType, mglImplementationColorReadTypeStr);
        mglNextParamEnum(&out, "GL_LAYER_PROVOKING_VERTEX", rs->iLayerProvokingVertex, mglProvokingVertexModeStr);
        mglNextParamInteger(&out, "GL_MAX_VARYING_VECTORS", rs->iMaxVaryingVectors);
        mglNextParamInteger(&out, "GL_MAX_VIEWPORTS", rs->iMaxViewports);
        mglNextParamIntegerArray(&out, "GL_VIEWPORT_BOUNDS_RANGE", rs->iViewportBoundsRange, 2, 2, 0);
        mglNextParamEnum(&out, "GL_VIEWPORT_INDEX_PROVOKING_VERTEX", rs->iViewportIndexProvokingVertex, mglProvokingVertexModeStr);
        mglNextParamInteger(&out, "GL_VIEWPORT_SUBPIXEL_BITS", rs->iViewportSubPixelBits);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_UNIFORM_VECTORS", rs->iMaxFragmentUniformVectors);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_UNIFORM_VECTORS", rs->iMaxVertexUniformVectors);
        mglNextParamInteger(&out, "GL_NUM_SHADER_BINARY_FORMATS", rs->iNumShaderBinaryFormats);
        mglNextParamIntegerArray(&out, "GL_SHADER_BINARY_FORMATS", rs->iShaderBinaryFormats, rs->iNumShaderBinaryFormats, MGL_MAX_SHADER_BINARY_FORMATS, formatting->enable_hex);
        mglNextParamInteger(&out, "GL_NUM_PROGRAM_BINARY_FORMATS", rs->iNumProgramBinaryFormats);
        mglNextParamIntegerArray(&out, "GL_PROGRAM_BINARY_FORMATS", rs->iProgramBinaryFormats, rs->iNumProgramBinaryFormats, MGL_MAX_PROGRAM_BINARY_FORMATS, formatting->enable_hex);
        mglNextParamInteger(&out, "GL_PROGRAM_PIPELINE_BINDING", rs->iProgramPipelineBinding);
        mglNextParamBoolean(&out, "GL_SHADER_COMPILER", rs->bShaderCompiler);
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
        MGL_PARAM_UNAVAIL( GL_MAX_VERTEX_UNIFORM_VECTORS );
        MGL_PARAM_UNAVAIL( GL_NUM_SHADER_BINARY_FORMATS );
        MGL_PARAM_UNAVAIL( GL_SHADER_BINARY_FORMATS );
        MGL_PARAM_UNAVAIL( GL_NUM_PROGRAM_BINARY_FORMATS );
        MGL_PARAM_UNAVAIL( GL_PROGRAM_BINARY_FORMATS );
        MGL_PARAM_UNAVAIL( GL_PROGRAM_PIPELINE_BINDING );
        MGL_PARAM_UNAVAIL( GL_SHADER_COMPILER );
    }

    // *****************************************************************
    //      GL_VERSION_4_2
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_2");

    #ifdef GL_VERSION_4_2
    if (MGL_VERSION_MIN(4, 2))
    {
        mglNextParamInteger(&out, "GL_MAX_COMBINED_ATOMIC_COUNTERS", rs->iMaxCombinedAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATOMIC_COUNTERS", rs->iMaxVertexAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS", rs->iMaxTessControlAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS", rs->iMaxTessEvaluationAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_ATOMIC_COUNTERS", rs->iMaxGeometryAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_ATOMIC_COUNTERS", rs->iMaxFragmentAtomicCounters);
        mglNextParamInteger(&out, "GL_MIN_MAP_BUFFER_ALIGNMENT", rs->iMinMapBufferAlignment);
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
        mglNextParamUInteger(&out, "GL_MAX_ELEMENT_INDEX", (GLuint)(rs->iMaxElementIndex));
        mglNextParamInteger(&out, "GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS", rs->iMaxCombinedComputeUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS", rs->iMaxCombinedShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_UNIFORM_BLOCKS", rs->iMaxComputeUniformBlocks);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS", rs->iMaxComputeTextureImageUnits);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_UNIFORM_COMPONENTS", rs->iMaxComputeUniformComponents);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_ATOMIC_COUNTERS", rs->iMaxComputeAtomicCounters);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS", rs->iMaxComputeAtomicCounterBuffers);
        #ifdef MENTAL_GL_GETINTEGERI_V
        mglNextParamIntegerArray(&out, "GL_MAX_COMPUTE_WORK_GROUP_COUNT", rs->iMaxComputeWorkGroupCount, 3, 3, 0);
        #else
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_WORK_GROUP_COUNT );
        #endif
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS", rs->iMaxComputeWorkGroup);
        #ifdef MENTAL_GL_GETINTEGERI_V
        mglNextParamIntegerArray(&out, "GL_MAX_COMPUTE_WORK_GROUP_SIZE", rs->iMaxComputeWorkGroupSize, 3, 3, 0);
        #else
        MGL_PARAM_UNAVAIL( GL_MAX_COMPUTE_WORK_GROUP_SIZE );
        #endif
        mglNextParamInteger(&out, "GL_DISPATCH_INDIRECT_BUFFER_BINDING", rs->iDispatchIndirectBufferBinding);
        mglNextParamInteger(&out, "GL_MAX_DEBUG_GROUP_STACK_DEPTH", rs->iMaxDebugGroupStackDepth);
        mglNextParamInteger(&out, "GL_DEBUG_GROUP_STACK_DEPTH", rs->iDebugGroupStackDepth);
        mglNextParamInteger(&out, "GL_MAX_LABEL_LENGTH", rs->iMaxLabelLength);
        mglNextParamInteger(&out, "GL_MAX_UNIFORM_LOCATIONS", rs->iMaxUniformLocations);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_WIDTH", rs->iMaxFramebufferWidth);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_HEIGHT", rs->iMaxFramebufferHeight);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_LAYERS", rs->iMaxFramebufferLayers);
        mglNextParamInteger(&out, "GL_MAX_FRAMEBUFFER_SAMPLES", rs->iMaxFramebufferSamples);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS", rs->iMaxVertexShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS", rs->iMaxTessControlShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", rs->iMaxTessEvaluationShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS", rs->iMaxGeometryShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS", rs->iMaxFragmentShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS", rs->iMaxComputeShaderStorageBlocks);
        mglNextParamInteger(&out, "GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT", rs->iTextureBufferOffsetAlignment);
        #ifdef MENTAL_GL_GETINTEGERI_V
        mglNextParamIntegerArray(&out, "GL_VERTEX_BINDING_DIVISOR", rs->iVertexBindingDivisor, MGL_MAX_VERTEX_BUFFER_BINDINGS, MGL_MAX_VERTEX_BUFFER_BINDINGS, 0);
        mglNextParamIntegerArray(&out, "GL_VERTEX_BINDING_OFFSET", rs->iVertexBindingOffset, MGL_MAX_VERTEX_BUFFER_BINDINGS, MGL_MAX_VERTEX_BUFFER_BINDINGS, 0);
        mglNextParamIntegerArray(&out, "GL_VERTEX_BINDING_STRIDE", rs->iVertexBindingStride, MGL_MAX_VERTEX_BUFFER_BINDINGS, MGL_MAX_VERTEX_BUFFER_BINDINGS, 0);
        #else
        MGL_PARAM_UNAVAIL( GL_VERTEX_BINDING_DIVISOR );
        MGL_PARAM_UNAVAIL( GL_VERTEX_BINDING_OFFSET );
        MGL_PARAM_UNAVAIL( GL_VERTEX_BINDING_STRIDE );
        #endif
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET", rs->iMaxVertexAttribRelativeOffset);
        mglNextParamInteger(&out, "GL_MAX_VERTEX_ATTRIB_BINDINGS", rs->iMaxVertexAttribBindings);
        mglNextParamInteger(&out, "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS", rs->iMaxShaderStorageBufferBindings);
        #ifdef MENTAL_GL_GETINTEGERI_V
        mglNextParamIntegerArray(&out, "GL_SHADER_STORAGE_BUFFER_BINDING", rs->iShaderStorageBufferBinding, MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, 0);
        #else
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_BINDING );
        #endif
        mglNextParamInteger(&out, "GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT", rs->iShaderStorageBufferOffsetAlignment);
        #ifdef MENTAL_GL_GETINTEGER64I_V
        mglNextParamInteger64Array(&out, "GL_SHADER_STORAGE_BUFFER_SIZE", rs->iShaderStorageBufferSize, MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        mglNextParamInteger64Array(&out, "GL_SHADER_STORAGE_BUFFER_START", rs->iShaderStorageBufferStart, MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, MGL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
        #else
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_START );
        #endif
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
        MGL_PARAM_UNAVAIL( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS );
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_BINDING );
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT );
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_SIZE );
        MGL_PARAM_UNAVAIL( GL_SHADER_STORAGE_BUFFER_START );
    }

    // *****************************************************************
    //      GL_VERSION_4_5
    // *****************************************************************

    if (formatting->order == MGLFormattingOrderDefault)
        mglNextHeadline(&out, "\nGL_VERSION_4_5");

    #ifdef GL_VERSION_4_5
    if (MGL_VERSION_MIN(4, 5))
    {
        mglNextParamEnum(&out, "GL_CLIP_DEPTH_MODE", rs->iClipDepthMode, mglClipDepthModeStr);
        mglNextParamEnum(&out, "GL_CLIP_ORIGIN", rs->iClipOrigin, mglClipOriginStr);
    }
    else
    #endif // /GL_VERSION_4_5
    {
        MGL_PARAM_UNAVAIL( GL_CLIP_DEPTH_MODE );
        MGL_PARAM_UNAVAIL( GL_CLIP_ORIGIN );
    }

    return mglPrintStringPairs(out, formatting);

    #undef MGL_VERSION
    #undef MGL_VERSION_MIN
    #undef MGL_PARAM_UNAVAIL
}

MGLString mglPrintBindingPoints(const MGLBindingPoints* bp, const MGLFormattingOptions* formatting)
{
    // Internal constant parameters
    static const MGLFormattingOptions   g_formattingDefault = { ' ', 1, 200, MGLFormattingOrderDefault, 1, NULL };

    if (formatting == NULL)
        formatting = (&g_formattingDefault);

    // Provide array with all string parts
    MGLStringInternal out_par[MGL_MAX_NUM_RENDER_STATES], out_val[MGL_MAX_NUM_RENDER_STATES];

    memset(out_par, 0, sizeof(out_par));
    memset(out_val, 0, sizeof(out_val));

    MGLStringPairArray out = { out_par, out_val, 0 };

    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_1D", bp->iTextureBinding1D, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_1D_ARRAY", bp->iTextureBinding1DArray, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_2D", bp->iTextureBinding2D, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_2D_ARRAY", bp->iTextureBinding2DArray, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_2D_MULTISAMPLE", bp->iTextureBinding2DMultisample, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY", bp->iTextureBinding2DMultisampleArray, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_3D", bp->iTextureBinding3D, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_BUFFER", bp->iTextureBindingBuffer, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_CUBE_MAP", bp->iTextureBindingCubeMap, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);
    mglNextParamIntegerArray(&out, "GL_TEXTURE_BINDING_RECTANGLE", bp->iTextureBindingRectangle, MGL_MAX_TEXTURE_LAYERS, MGL_MAX_TEXTURE_LAYERS, 0);

    return mglPrintStringPairs(out, formatting);
}

const char* mglGetUTF8String(MGLString s)
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

#ifdef __cplusplus
} // /extern "C"
#endif


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
#undef MGL_MAX_SHADER_BINARY_FORMATS
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
