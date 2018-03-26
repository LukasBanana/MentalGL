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


#include <string.h>
#include <stdio.h>
#include <GL/GL.h>
#include <GL/glext.h>


// *****************************************************************
//      INTERNAL MACROS
// *****************************************************************

#define MGL_CASE2STR(NAME)      case NAME: return #NAME

#define MGL_MIN(A, B)           ((A) < (B) ? (A) : (B))
#define MGL_MAX(A, B)           ((A) > (B) ? (A) : (B))

#ifndef MGL_MALLOC
#define MGL_MALLOC(TYPE)        ((TYPE*)malloc(sizeof(TYPE)))
#endif

#ifndef MGL_CALLOC
#define MGL_CALLOC(TYPE, COUNT) ((TYPE*)calloc(COUNT, sizeof(TYPE)))
#endif

#ifndef MGL_FREE
#define MGL_FREE(OBJ)           free(OBJ)
#endif

#define MGL_STRING_MIN_CAPACITY 16


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
