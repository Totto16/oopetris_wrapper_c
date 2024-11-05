

#pragma once

// assure we only include the header with correct defines
#ifndef STBDS_NO_SHORT_NAMES
#define STBDS_NO_SHORT_NAMES
#endif
#ifdef STB_DS_IMPLEMENTATION
#undef STB_DS_IMPLEMENTATION
#endif


#if defined(OOPETRIS_REALLOC) && !defined(OOPETRIS_FREE) || !defined(OOPETRIS_REALLOC) && defined(OOPETRIS_FREE)
#error "You must define both OOPETRIS_REALLOC and OOPETRIS_FREE, or neither."
#endif

#if defined(OOPETRIS_REALLOC) && defined(OOPETRIS_FREE)
#define STBDS_REALLOC(c, p, s) OOPETRIS_REALLOC(p, s)
#define STBDS_FREE(c, p) OOPETRIS_FREE(p)
#else

#include <stdlib.h>

#define OOPETRIS_REALLOC(p, s) realloc(p, s)
#define OOPETRIS_FREE(p) free(p)
#endif


#define OOPETRIS_MALLOC(s) OOPETRIS_REALLOC(NULL, s)

#include "./thirdparty/stb_ds.h"
#include "./windows.h"


#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief this determines if a file (may be nonexistent) is a recording file, it is not guaranteed, tat a consecutive call to oopetris_get_recording_information never fails, if this returns true, but it's highly unliekely, and this is faster, as it doesn't do as much work
 * 
 * @param file_path The FilePath of the potential recording file
 * @return bool
 */
OOPETRIS_C_WRAPPER_EXPORTED bool oopetris_is_recording_file(const char* file_path);

// opaque type
typedef struct OOPetrisAdditionalInformationImpl OOPetrisAdditionalInformation;

// opaque type
typedef struct OOPetrisAdditionalInformationFieldImpl OOPetrisAdditionalInformationField;

OOPETRIS_C_WRAPPER_EXPORTED const char** oopetris_additional_information_get_keys(
        OOPetrisAdditionalInformation* information
);


OOPETRIS_C_WRAPPER_EXPORTED void oopetris_additional_information_keys_free(const char** keys);

OOPETRIS_C_WRAPPER_EXPORTED const OOPetrisAdditionalInformationField*
oopetris_additional_information_get_field(OOPetrisAdditionalInformation* information, const char* key);


//TODO(): once we only support C23 (which also the compiled lib has to support ), add underlying enum type uint8_t
typedef enum {
    OOPetrisAdditionalInformationType_String = 0,
    OOPetrisAdditionalInformationType_Float,
    OOPetrisAdditionalInformationType_Double,
    OOPetrisAdditionalInformationType_Bool,
    OOPetrisAdditionalInformationType_U8,
    OOPetrisAdditionalInformationType_I8,
    OOPetrisAdditionalInformationType_U32,
    OOPetrisAdditionalInformationType_I32,
    OOPetrisAdditionalInformationType_U64,
    OOPetrisAdditionalInformationType_I64,
    OOPetrisAdditionalInformationType_Vector,
} OOPetrisAdditionalInformationType;


OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationType
oopetris_additional_information_field_get_type(const OOPetrisAdditionalInformationField* const field);

OOPETRIS_C_WRAPPER_EXPORTED const char* oopetris_additional_information_field_get_string(
        const OOPetrisAdditionalInformationField* const field
);

OOPETRIS_C_WRAPPER_EXPORTED float oopetris_additional_information_field_get_float(
        const OOPetrisAdditionalInformationField* const field
);

OOPETRIS_C_WRAPPER_EXPORTED double oopetris_additional_information_field_get_double(
        const OOPetrisAdditionalInformationField* const field
);

OOPETRIS_C_WRAPPER_EXPORTED bool oopetris_additional_information_field_get_bool(
        const OOPetrisAdditionalInformationField* const field
);

OOPETRIS_C_WRAPPER_EXPORTED uint8_t
oopetris_additional_information_field_get_u8(const OOPetrisAdditionalInformationField* const field);

OOPETRIS_C_WRAPPER_EXPORTED int8_t
oopetris_additional_information_field_get_i8(const OOPetrisAdditionalInformationField* const field);

OOPETRIS_C_WRAPPER_EXPORTED uint32_t
oopetris_additional_information_field_get_u32(const OOPetrisAdditionalInformationField* const field);


OOPETRIS_C_WRAPPER_EXPORTED int32_t
oopetris_additional_information_field_get_i32(const OOPetrisAdditionalInformationField* const field);

OOPETRIS_C_WRAPPER_EXPORTED uint64_t
oopetris_additional_information_field_get_u64(const OOPetrisAdditionalInformationField* const field);

OOPETRIS_C_WRAPPER_EXPORTED int64_t
oopetris_additional_information_field_get_i64(const OOPetrisAdditionalInformationField* const field);

OOPETRIS_C_WRAPPER_EXPORTED const OOPetrisAdditionalInformationField* const*
oopetris_additional_information_field_get_vector(const OOPetrisAdditionalInformationField* const field);


//TODO(): once we only support C23 (which also the compiled lib has to support ), add underlying enum type uint8_t
typedef enum {
    OOPetrisInputEvent_RotateLeftPressed = 0,
    OOPetrisInputEvent_RotateRightPressed,
    OOPetrisInputEvent_MoveLeftPressed,
    OOPetrisInputEvent_MoveRightPressed,
    OOPetrisInputEvent_MoveDownPressed,
    OOPetrisInputEvent_DropPressed,
    OOPetrisInputEvent_HoldPressed,
    OOPetrisInputEvent_RotateLeftReleased,
    OOPetrisInputEvent_RotateRightReleased,
    OOPetrisInputEvent_MoveLeftReleased,
    OOPetrisInputEvent_MoveRightReleased,
    OOPetrisInputEvent_MoveDownReleased,
    OOPetrisInputEvent_DropReleased,
    OOPetrisInputEvent_HoldReleased,
} OOPetrisInputEvent;

typedef struct {
    uint64_t simulation_step_index;
    OOPetrisInputEvent event;
    uint8_t tetrion_index;
} OOPetrisTetrionRecord;

typedef struct {
    int8_t x;
    int8_t y;
} OOPetrisMinoPosition;


//TODO(): once we only support C23 (which also the compiled lib has to support ), add underlying enum type uint8_t
typedef enum {
    OOPetrisTetrominoType_I = 0,
    OOPetrisTetrominoType_J,
    OOPetrisTetrominoType_L,
    OOPetrisTetrominoType_O,
    OOPetrisTetrominoType_S,
    OOPetrisTetrominoType_T,
    OOPetrisTetrominoType_Z,
} OOPetrisTetrominoType;

typedef struct {
    OOPetrisMinoPosition position;
    OOPetrisTetrominoType type;
} OOPetrisMino;

typedef struct {
    uint32_t level;
    OOPetrisMino* mino_stack;
    uint64_t score;
    uint64_t simulation_step_index;
    uint32_t lines_cleared;
    uint8_t tetrion_index;
} OOPetrisTetrionSnapshot;

typedef struct {
    uint64_t seed;
    uint32_t starting_level;
} OOPetrisTetrionHeader;


/**
 * @brief non opaque type, fields can be safely accessed, except they are opaque structs again
 * The values that are pointers are dynamic array, as defined by stb_ds.h, see tests for usage example, an exception is the opaque type OOPetrisAdditionalInformation, you have to use some helper functions, to get access to it, it is a object / dict with dynamic types
 * 
 */
typedef struct {
    OOPetrisAdditionalInformation* information; //NOT AN ARRAY
    OOPetrisTetrionRecord* records;
    OOPetrisTetrionSnapshot* snapshots;
    OOPetrisTetrionHeader* tetrion_headers;
    uint32_t version;
} OOPetrisRecordingInformation;


// opaque type
typedef struct OOPetrisRecordingReturnValueImpl OOPetrisRecordingReturnValue;


/**
 * @brief Retrieve the Recording information of a file, returns an opaque struct pointer, that may be null and you have to use oopetris_is_error and similar functions to retrieve the internal value
 * 
 * @param file_path The FilePath of the recording file
 * @return OOPetrisRecordingReturnValue* 
 */
OOPETRIS_C_WRAPPER_EXPORTED OOPetrisRecordingReturnValue* oopetris_get_recording_information(const char* file_path);

/**
 * @brief Determine if the given OOPetrisRecordingReturnValue* is an error or not, after calling this you can call either oopetris_get_error or oopetris_get_information. This has to be freed later, but pay attention, which free function you use, read more in the description of oopetris_free_recording_value
 * 
 * @param value The return value of oopetris_get_recording_information 
 * @return bool 
 */
OOPETRIS_C_WRAPPER_EXPORTED bool oopetris_is_error(OOPetrisRecordingReturnValue* value);

OOPETRIS_C_WRAPPER_EXPORTED const char* oopetris_get_error(OOPetrisRecordingReturnValue* value);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisRecordingInformation* oopetris_get_information(OOPetrisRecordingReturnValue* value);

OOPETRIS_C_WRAPPER_EXPORTED void oopetris_free_recording_information(OOPetrisRecordingInformation* information);

OOPETRIS_C_WRAPPER_EXPORTED void oopetris_free_recording_value_only(OOPetrisRecordingReturnValue* information);

OOPETRIS_C_WRAPPER_EXPORTED void oopetris_free_recording_value_whole(OOPetrisRecordingReturnValue* information);

OOPETRIS_C_WRAPPER_EXPORTED const char* oopetris_get_lib_version(void);

typedef struct {
    uint32_t height;
    uint32_t width;
} OOPetrisGridProperties;

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisGridProperties* oopetris_get_grid_properties(void);

OOPETRIS_C_WRAPPER_EXPORTED void oopetris_free_grid_properties(OOPetrisGridProperties* properties);


OOPETRIS_C_WRAPPER_EXPORTED size_t oopetris_array_len(void* const array);

#define FREE_AND_SET_NULL(fn, obj) \
    do {                           \
        fn(obj);                   \
        obj = NULL;                \
    } while (false)


// CREATION functions

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisRecordingInformation* oopetris_create_recording_information(void);

void oopetris_add_information_field(
        OOPetrisAdditionalInformation* additional_information,
        const char* key,
        OOPetrisAdditionalInformationField* value
);


OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_string(
        const char* value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_float(float value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_double(
        double value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_bool(bool value);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_u8(uint8_t value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_i8(int8_t value);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_u32(
        uint32_t value
);


OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_i32(int32_t value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_u64(
        uint64_t value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_i64(int64_t value
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField** oopetris_additional_information_create_empty_vector(
        void
);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField**
oopetris_additional_information_create_vector_with_size(size_t size);

OOPETRIS_C_WRAPPER_EXPORTED OOPetrisAdditionalInformationField* oopetris_additional_information_create_vector(
        OOPetrisAdditionalInformationField** value
);

OOPETRIS_C_WRAPPER_EXPORTED void
oopetris_add_record(OOPetrisRecordingInformation* information, OOPetrisTetrionRecord record);

OOPETRIS_C_WRAPPER_EXPORTED void
oopetris_add_snapshot(OOPetrisRecordingInformation* information, OOPetrisTetrionSnapshot snapshot);

OOPETRIS_C_WRAPPER_EXPORTED void
oopetris_add_header(OOPetrisRecordingInformation* information, OOPetrisTetrionHeader tetrion_header);

OOPETRIS_C_WRAPPER_EXPORTED char*
oopetris_write_to_file(OOPetrisRecordingInformation* information, const char* file_path, bool overwrite);


#ifdef __cplusplus
}
#endif
