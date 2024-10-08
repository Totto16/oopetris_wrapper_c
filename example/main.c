
#include <c_wrapper/wrapper.h>

#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_MSC_VER)
#include <io.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#if defined(WITH_READLINE)
#include <readline/history.h>
#include <readline/readline.h>
#endif

void print_bool(bool val) {
    printf("%s", val ? "true" : "false");
}


void print_u8(uint8_t x) {
    printf("%" PRIu8, x);
}

void print_i8(int8_t x) {
    printf("%" PRIi8, x);
}

void print_u32(uint32_t x) {
    printf("%" PRIu32, x);
}

void print_i32(int32_t x) {
    printf("%" PRIi32, x);
}

void print_u64(uint64_t x) {
    printf("%" PRIu64, x);
}

void print_i64(int64_t x) {
    printf("%" PRIi64, x);
}

void print_float(float x) {
    printf("%f", x);
}

void print_double(double x) {
    printf("%f", x);
}

void print_string(const char* x) {
    printf("%s", x);
}


void print_field(const OOPetrisAdditionalInformationField* const field);

void print_vector(const OOPetrisAdditionalInformationField* const* const field) {

    printf("\n");
    for (size_t i = 0; i < oopetris_array_len((void*) field); ++i) {
        print_field(field[i]);
        printf("\n");
    }
}

// usage of a "typeof c++ operator in C, it's not the same, but it can do some stuff", _Generic makes this possible :)

// clang-format off
#define PRINT_VALUE(val) \
	_Generic((val),\
        _Bool    : print_bool,\
        uint8_t  : print_u8, \
        int8_t   : print_i8,\
        uint32_t : print_u32,\
        int32_t  : print_i32,\
        uint64_t : print_u64,\
        int64_t  : print_i64,\
        float    : print_float,\
        double   : print_double,\
        const char* : print_string,\
        const OOPetrisAdditionalInformationField* const* : print_vector)(val)
// clang-format on


void print_field(const OOPetrisAdditionalInformationField* const field) {
    switch (oopetris_additional_information_field_get_type(field)) {
        case OOPetrisAdditionalInformationType_String:
            PRINT_VALUE(oopetris_additional_information_field_get_string(field));
            return;
        case OOPetrisAdditionalInformationType_Float:
            PRINT_VALUE(oopetris_additional_information_field_get_float(field));
            return;
        case OOPetrisAdditionalInformationType_Double:
            PRINT_VALUE(oopetris_additional_information_field_get_double(field));
            return;
        case OOPetrisAdditionalInformationType_Bool:
            PRINT_VALUE(oopetris_additional_information_field_get_bool(field));
            return;
        case OOPetrisAdditionalInformationType_U8:
            PRINT_VALUE(oopetris_additional_information_field_get_u8(field));
            return;
        case OOPetrisAdditionalInformationType_I8:
            PRINT_VALUE(oopetris_additional_information_field_get_i8(field));
            return;
        case OOPetrisAdditionalInformationType_U32:
            PRINT_VALUE(oopetris_additional_information_field_get_u32(field));
            return;
        case OOPetrisAdditionalInformationType_I32:
            PRINT_VALUE(oopetris_additional_information_field_get_i32(field));
            return;
        case OOPetrisAdditionalInformationType_U64:
            PRINT_VALUE(oopetris_additional_information_field_get_u64(field));
            return;
        case OOPetrisAdditionalInformationType_I64:
            PRINT_VALUE(oopetris_additional_information_field_get_i64(field));
            return;
        case OOPetrisAdditionalInformationType_Vector:
            PRINT_VALUE(oopetris_additional_information_field_get_vector(field));
            return;

        default:
            assert(false && "UNREACHABLE");
            return;
    }
}

void print_tetrion_header(const OOPetrisTetrionHeader* const header) {

    printf("seed: %" PRIu64 "\tstarting_level: %" PRIu32 "\n", header->seed, header->starting_level);
}

const char* input_event_string(OOPetrisInputEvent event) {
    switch (event) {
        case OOPetrisInputEvent_RotateLeftPressed:
            return "RotateLeftPressed";
        case OOPetrisInputEvent_RotateRightPressed:
            return "RotateRightPressed";
        case OOPetrisInputEvent_MoveLeftPressed:
            return "MoveLeftPressed";
        case OOPetrisInputEvent_MoveRightPressed:
            return "MoveRightPressed";
        case OOPetrisInputEvent_MoveDownPressed:
            return "MoveDownPressed";
        case OOPetrisInputEvent_DropPressed:
            return "DropPressed";
        case OOPetrisInputEvent_HoldPressed:
            return "HoldPressed";
        case OOPetrisInputEvent_RotateLeftReleased:
            return "RotateLeftReleased";
        case OOPetrisInputEvent_RotateRightReleased:
            return "RotateRightReleased";
        case OOPetrisInputEvent_MoveLeftReleased:
            return "MoveLeftReleased";
        case OOPetrisInputEvent_MoveRightReleased:
            return "MoveRightReleased";
        case OOPetrisInputEvent_MoveDownReleased:
            return "MoveDownReleased";
        case OOPetrisInputEvent_DropReleased:
            return "DropReleased";
        case OOPetrisInputEvent_HoldReleased:
            return "HoldReleased";
        default:
            assert(false && "UNREACHABLE");
            return "<ERROR>";
    }
}

void print_record(const OOPetrisTetrionRecord* const record) {
    printf("simulation_step_index: %" PRIu64 "\ttetrion_index: %" PRIu8 "\tevent: %s\n", record->simulation_step_index,
           record->tetrion_index, input_event_string(record->event));
}

void print_mino_stack(const OOPetrisMino* const stack) {

    OOPetrisGridProperties* properties = oopetris_get_grid_properties();

    if (properties == NULL) {
        return;
    }

    const size_t buffer_size = properties->height * properties->width;

    char* buffer = OOPETRIS_MALLOC(buffer_size);

    if (buffer == NULL) {
        return;
    }

    memset(buffer, '.', buffer_size);

    for (size_t i = 0; i < oopetris_array_len((void*) stack); ++i) {
        const OOPetrisMinoPosition position = stack[i].position;
        buffer[position.x + (position.y * properties->width)] = '#';
    }


    for (size_t y = 0; y < properties->height; ++y) {
#if defined(_MSC_VER)
        int result = _write(_fileno(stdout), buffer + (y * properties->width), properties->width);
#else
        int result = write(STDOUT_FILENO, buffer + (y * properties->width), properties->width);
#endif
        if (result < 0) {
            OOPETRIS_FREE(buffer);
            return;
        }
        printf("\n");
    }

    OOPETRIS_FREE(buffer);
    FREE_AND_SET_NULL(oopetris_free_grid_properties, properties);
}

void print_snapshot(const OOPetrisTetrionSnapshot* const snapshot) {
    printf("\tsimulation_step_index: %" PRIu64 "\ttetrion_index: %" PRIu8 "\n", snapshot->simulation_step_index,
           snapshot->tetrion_index);

    printf("\tlevel: %" PRIu32 "\tscore: %" PRIu64 "\tlines_cleared: %" PRIu32 "\n", snapshot->level, snapshot->score,
           snapshot->lines_cleared);

    print_mino_stack(snapshot->mino_stack);
}

typedef enum { ModeRead = 0, ModeWrite } Mode;


void print_recording_information(const OOPetrisRecordingInformation* const information) {

    printf("Version: %d\n\n", information->version);
    printf("Additional Information:\n");

    const char** keys = oopetris_additional_information_get_keys(information->information);

    for (size_t i = 0; i < oopetris_array_len(keys); ++i) {
        printf("\t%s: ", keys[i]);
        print_field(oopetris_additional_information_get_field(information->information, keys[i]));
        printf("\n");
    }
    printf("\n");

    FREE_AND_SET_NULL(oopetris_additional_information_keys_free, keys);

    printf("Headers:\n");

    for (size_t i = 0; i < oopetris_array_len(information->tetrion_headers); ++i) {
        printf("\t");
        print_tetrion_header(&(information->tetrion_headers[i]));
    }
    printf("\n");

    printf("Records:\n");

    for (size_t i = 0; i < oopetris_array_len(information->records); ++i) {
        printf("\t");
        print_record(&(information->records[i]));
    }
    printf("\n");

    printf("Snapshots:\n");

    for (size_t i = 0; i < oopetris_array_len(information->snapshots); ++i) {
        print_snapshot(&(information->snapshots[i]));
    }
    printf("\n");
}

void print_usage(const char* name) {
    fprintf(stderr, "usage: %s <mode> <file>\n", name);
    fprintf(stderr, "\tmode: read, r\nwrite, w\n");
}

int read_file(const char* file) {

    const bool is_recordings_file = oopetris_is_recording_file(file);

    if (!is_recordings_file) {
        fprintf(stderr, "Input file is no recordings file!\n");
        return EXIT_FAILURE;
    }

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information(file);

    const bool is_error = oopetris_is_error(return_value);

    if (is_error) {
        const char* error = oopetris_get_error(return_value);
        fprintf(stderr, "An error occurred: %s\n", error);
        FREE_AND_SET_NULL(oopetris_free_recording_value_whole, return_value);
        return EXIT_FAILURE;
    }

    OOPetrisRecordingInformation* information = oopetris_get_information(return_value);

    FREE_AND_SET_NULL(oopetris_free_recording_value_only, return_value);

    print_recording_information(information);

    FREE_AND_SET_NULL(oopetris_free_recording_information, information);

    return EXIT_SUCCESS;
}


#if defined(WITH_READLINE)


bool file_exists(const char* file) {
#if defined(_MSC_VER)
    return OpenFile(path, NULL, OF_EXIST) != HFILE_ERROR;
#else

    struct stat buffer;
    return (stat(file, &buffer) == 0);

#endif
}

typedef enum {
    StateRoot = 0, //
    StateAdditionalInformation,
    StateRecords,
    StateSnapshots,
    StateHeaders
} State;

typedef enum {
    CommandPrintHelp = 0,
    CommandNoOp,
    CommandError,
    //
    CommandInsertAddInfo,
    //
    //TODO
} Command;

typedef enum { TempDataEnumAddInfoKey, TempDataEnumVector } TempDataEnum;

typedef struct {
    TempDataEnum type;
    union {
        struct {
            char* key;
        } add_info_key;
        struct {
            char* key;
            OOPetrisAdditionalInformationField*** vectors;
        } vector;
        //
    } value;

} TempData;

typedef struct {
    char* key;
    OOPetrisAdditionalInformationField* value;
} AdditionalInformationData;

static char* copy_str(const char* input) {
    size_t size = strlen(input);
    char* string = (char*) OOPETRIS_MALLOC(size + 1);

    if (string == NULL) {
        return NULL;
    }

    strcpy(string, input);

    return string;
}


static float* get_float(const char* input) {


    char* end;

    float res = strtof(input, &end);

    if (end == input) {
        return NULL;
    }

    if (*end != '\0') {
        return NULL;
    }

    float* return_value = OOPETRIS_MALLOC(sizeof(float));

    if (return_value == NULL) {
        return NULL;
    }

    *return_value = res;

    return return_value;
}


static double* get_double(const char* input) {


    char* end;

    double res = strtod(input, &end);

    if (end == input) {
        return NULL;
    }

    double* return_value = OOPETRIS_MALLOC(sizeof(double));

    if (return_value == NULL) {
        return NULL;
    }

    if (*end != '\0') {
        return NULL;
    }

    *return_value = res;

    return return_value;
}

static long long* get_long(const char* input) {
    char* end;

    long long res = strtoll(input, &end, 0);

    if (end == input) {
        return NULL;
    }

    if (*end != '\0') {
        return NULL;
    }

    long long* return_value = OOPETRIS_MALLOC(sizeof(long long));

    if (return_value == NULL) {
        return NULL;
    }

    *return_value = res;

    return return_value;
}

static unsigned long long* get_ulong(const char* input) {


    char* end;

    unsigned long long res = strtoull(input, &end, 0);

    if (end == input) {
        return NULL;
    }

    if (*end != '\0') {
        return NULL;
    }

    unsigned long long* return_value = OOPETRIS_MALLOC(sizeof(unsigned long long));

    if (return_value == NULL) {
        return NULL;
    }

    *return_value = res;

    return return_value;
}


static bool* get_bool(const char* input) {

    bool res = false;

    if (strcmp(input, "true") == 0 || strcmp(input, "t") == 0 || strcmp(input, "1") == 0) {
        res = true;
    } else if (strcmp(input, "false") == 0 || strcmp(input, "f") == 0 || strcmp(input, "0") == 0) {
        res = false;
    } else {
        return NULL;
    }


    bool* return_value = OOPETRIS_MALLOC(sizeof(bool));

    if (return_value == NULL) {
        return NULL;
    }

    *return_value = res;

    return return_value;
}


Command parse_command(State* state, void** data, const char* input) {

#define ERROR_BUF_SIZE 0x200
#define RETURN_ERROR(...)                                       \
    do {                                                        \
        *data = OOPETRIS_MALLOC(ERROR_BUF_SIZE);                \
        if (snprintf(*data, ERROR_BUF_SIZE, __VA_ARGS__) < 0) { \
            OOPETRIS_FREE(*data);                               \
            *data = NULL;                                       \
        }                                                       \
        return CommandError;                                    \
    } while (false)


#define ASSERT_OR_ERROR(check, ...) \
    if (!(check)) {                 \
        RETURN_ERROR(__VA_ARGS__);  \
    }

#define VERIFY_MALLOC(var) ASSERT_OR_ERROR((var) != NULL, "Malloc failed")

    if (strcmp(input, "h") == 0 || strcmp(input, "help") == 0 || strcmp(input, "?") == 0) {
        return CommandPrintHelp;
    }

    size_t length = strlen(input);
    if (length == 0) {
        RETURN_ERROR("To short command");
    }


    if (input[0] == 's') {

        switch (*state) {
            case StateRoot:
                printf("Root\n");
                return CommandNoOp;
            case StateAdditionalInformation:
                printf("AdditionalInformation\n");
                return CommandNoOp;
            case StateRecords:
                printf("Records\n");
                return CommandNoOp;
            case StateSnapshots:
                printf("Snapshots\n");
                return CommandNoOp;
            case StateHeaders:
                printf("Headers\n");
                return CommandNoOp;
            default:
                RETURN_ERROR("Unknown state");
        }
    } else if (input[0] == '!') {

        if (length < 2) {
            RETURN_ERROR("To short '!' command: missing second argument");
        }

        size_t index = 1;
        while (isspace(input[index])) {
            if (index + 1 >= length) {
                RETURN_ERROR("To short '!' command: missing second argument");
            }

            ++index;
        }

        char type = input[index];

        switch (type) {
            case 'a':
                *state = StateAdditionalInformation;
                return CommandNoOp;
            case 'r':
                *state = StateRecords;
                return CommandNoOp;
            case 's':
                *state = StateSnapshots;
                return CommandNoOp;
            case 'h':
                *state = StateHeaders;
                return CommandNoOp;
            case '?':
                printf("Possible insertion types:\n"
                       "\ta: additional information\n"
                       "\tr: records\n"
                       "\ts: snapshots\n"
                       "\th: headers\n");
                return CommandNoOp;
            default:
                RETURN_ERROR("Unknown Insertion type '%c'", type);
        }
    }


    switch (*state) {
        case StateRoot:
            RETURN_ERROR("Unknown command: %s", input);

        case StateAdditionalInformation: {

            if (input[0] == ':') {
                if (length < 2) {
                    RETURN_ERROR("To short ':' command: missing second argument");
                }

                size_t index = 1;
                while (isspace(input[index])) {
                    if (index + 1 >= length) {
                        RETURN_ERROR("To short ':' command: missing second argument");
                    }

                    ++index;
                }

                char type = input[index];

                OOPetrisAdditionalInformationField* field = NULL;

                const char* adding = "<unknown>";

                switch (type) {
                    case 'k': { // key

                        // note on error here, no free is done, since data can be anything, just freeing the pointer want really help
                        ASSERT_OR_ERROR((*data == NULL), "Unexpected state in additional info: add key")

                        if (index + 2 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        ASSERT_OR_ERROR(isspace(input[index + 1]), "Expected space as delimiter for the third argument")

                        const char* key = input + index + 2;
                        char* malloced_str = copy_str(key);
                        VERIFY_MALLOC(malloced_str);

                        TempData* return_value = (TempData*) OOPETRIS_MALLOC(sizeof(TempData));
                        VERIFY_MALLOC(return_value);

                        return_value->type = TempDataEnumAddInfoKey;
                        return_value->value.add_info_key.key = malloced_str;

                        *data = return_value;
                        return CommandNoOp;
                    }
                    case 'f': {

                        if (index + 2 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        ASSERT_OR_ERROR(isspace(input[index + 1]), "Expected space as delimiter for the third argument")

                        const char* float_v = input + index + 2;

                        float* value = get_float(float_v);
                        ASSERT_OR_ERROR(value != NULL, "Not a float: %s", float_v);

                        field = oopetris_additional_information_create_float(*value);
                        OOPETRIS_FREE(value);
                        adding = "float";
                        goto return_field_value;
                    }

                    case 'd': {

                        if (index + 2 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        ASSERT_OR_ERROR(isspace(input[index + 1]), "Expected space as delimiter for the third argument")

                        const char* double_v = input + index + 2;

                        double* value = get_double(double_v);
                        ASSERT_OR_ERROR(value != NULL, "Not a double: %s", double_v);

                        field = oopetris_additional_information_create_double(*value);
                        OOPETRIS_FREE(value);
                        adding = "double";
                        goto return_field_value;
                    }

                    case 'b': {

                        if (index + 2 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        ASSERT_OR_ERROR(isspace(input[index + 1]), "Expected space as delimiter for the third argument")

                        const char* bool_v = input + index + 2;

                        bool* value = get_bool(bool_v);
                        ASSERT_OR_ERROR(value != NULL, "Not a bool: %s", bool_v);

                        field = oopetris_additional_information_create_bool(*value);
                        OOPETRIS_FREE(value);
                        adding = "bool";
                        goto return_field_value;
                    }


                    case 's': {

                        if (index + 1 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        ASSERT_OR_ERROR(isspace(input[index + 1]), "Expected space as delimiter for the third argument")

                        const char* value = input + index + 2;
                        field = oopetris_additional_information_create_string(value);
                        adding = "string";
                        goto return_field_value;
                    }

                    case 'u': {
                        if (index + 3 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        unsigned long long limit_max = 0;
                        const char* start_char = input + index;
                        uint8_t type = 0;

                        if (input[index + 2] == ' ') {
                            if (input[index + 1] == '8') {
                                limit_max = UCHAR_MAX;
                                type = 1;
                            } else {
                                RETURN_ERROR("Unrecognized number command: u%c", input[index + 1]);
                            }
                            start_char = input + index + 3;
                        } else if (input[index + 3] == ' ') {
                            if (input[index + 1] == '3' && input[index + 2] == '2') {
                                limit_max = UINT_MAX;
                                type = 2;
                            } else if (input[index + 1] == '6' && input[index + 2] == '4') {
                                limit_max = ULLONG_MAX;
                                type = 3;
                            } else {
                                RETURN_ERROR("Unrecognized number command: u%c%c", input[index + 1], input[index + 2]);
                            }

                            if (index + 4 >= length) {
                                RETURN_ERROR("To short ':' command: missing third argument");
                            }

                            start_char = input + index + 4;

                        } else {
                            RETURN_ERROR("Unrecognized number command: u%s", (input + index + 1));
                        }


                        unsigned long long* value = get_ulong(start_char);
                        ASSERT_OR_ERROR(value != NULL, "Not an unsigned int: '%s'", start_char);

                        if (*value > limit_max) {
                            RETURN_ERROR("Maximum Limit reached for unsigned int: %llu > %llu", *value, limit_max);
                        }

                        if (type == 1) {
                            field = oopetris_additional_information_create_u8(*value);
                        } else if (type == 2) {
                            field = oopetris_additional_information_create_u32(*value);
                        } else if (type == 3) {
                            field = oopetris_additional_information_create_u64(*value);
                        } else {
                            RETURN_ERROR("PROGRAMMING ERROR: UNREACHABLE");
                        }

                        OOPETRIS_FREE(value);
                        adding = "unsigned int";
                        goto return_field_value;
                    }

                    case 'i': {
                        if (index + 3 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        long long limit_min = 0;
                        long long limit_max = 0;
                        const char* start_char = input + index;
                        uint8_t type = 0;

                        if (input[index + 2] == ' ') {
                            if (input[index + 1] == '8') {
                                limit_min = SCHAR_MIN;
                                limit_max = SCHAR_MAX;
                                type = 1;
                            } else {
                                RETURN_ERROR("Unrecognized number command: i%c", input[index + 1]);
                            }
                            start_char = input + index + 3;
                        } else if (input[index + 3] == ' ') {
                            if (input[index + 1] == '3' && input[index + 2] == '2') {
                                limit_min = INT_MIN;
                                limit_max = INT_MAX;
                                type = 2;
                            } else if (input[index + 1] == '6' && input[index + 2] == '4') {
                                limit_min = LLONG_MIN;
                                limit_max = LLONG_MAX;
                                type = 3;
                            } else {
                                RETURN_ERROR("Unrecognized number command: i%c%c", input[index + 1], input[index + 2]);
                            }

                            if (index + 4 >= length) {
                                RETURN_ERROR("To short ':' command: missing third argument");
                            }

                            start_char = input + index + 4;

                        } else {
                            RETURN_ERROR("Unrecognized number command: i%s", (input + index + 1));
                        }


                        long long* value = get_long(start_char);
                        ASSERT_OR_ERROR(value != NULL, "Not an int: '%s'", start_char);

                        if (*value > limit_max) {
                            RETURN_ERROR("Maximum Limit reached for int: %lld > %lld", *value, limit_max);
                        }


                        if (*value < limit_min) {
                            RETURN_ERROR("Minimum Limit reached for int: %lld < %lld", *value, limit_min);
                        }

                        if (type == 1) {
                            field = oopetris_additional_information_create_i8(*value);
                        } else if (type == 2) {
                            field = oopetris_additional_information_create_i32(*value);
                        } else if (type == 3) {
                            field = oopetris_additional_information_create_i64(*value);
                        } else {
                            RETURN_ERROR("PROGRAMMING ERROR: UNREACHABLE");
                        }

                        OOPETRIS_FREE(value);
                        adding = "int";
                        goto return_field_value;
                    }

                    case 'v': {

                        if (index + 1 >= length) {
                            RETURN_ERROR("To short ':' command: missing third argument");
                        }

                        if (input[index + 1] == 'b') {

                            if (*data == NULL) {
                                RETURN_ERROR("Unexpected state in additional info, data NULL: add vector begin");
                            }

                            TempData* temp = (TempData*) *data;

                            if (temp->type == TempDataEnumAddInfoKey) {


                                char* temp_key = temp->value.add_info_key.key;

                                memset(temp, '\0', sizeof(TempData));

                                temp->type = TempDataEnumVector;
                                temp->value.vector.key = temp_key;
                                temp->value.vector.vectors = NULL;


                                OOPetrisAdditionalInformationField** first_elem =
                                        oopetris_additional_information_create_empty_vector();

                                stbds_arrput(temp->value.vector.vectors, first_elem);

                                return CommandNoOp;

                            } else if (temp->type == TempDataEnumVector) {

                                OOPetrisAdditionalInformationField** first_elem =
                                        oopetris_additional_information_create_empty_vector();

                                stbds_arrput(temp->value.vector.vectors, first_elem);
                                return CommandNoOp;

                            } else {
                                RETURN_ERROR("Unexpected state in additional info: add vector begin");
                            }


                        } else if (input[index + 1] == 'e') {


                            if (*data == NULL) {
                                RETURN_ERROR("Unexpected state in additional info, data NULL: add vector end");
                            }

                            TempData* temp = (TempData*) *data;


                            if (temp->type == TempDataEnumAddInfoKey) {

                                RETURN_ERROR(
                                        "Unexpected state in additional info: add vector end, can't be proceeded by a "
                                        "key!"
                                );

                            } else if (temp->type == TempDataEnumVector) {

                                size_t length = stbds_arrlenu(temp->value.vector.vectors);

                                if (length == 0) {

                                    RETURN_ERROR("UNREACHABLE, programming error");
                                } else if (length == 1) {


                                    OOPetrisAdditionalInformationField** final_vec =
                                            stbds_arrpop(temp->value.vector.vectors);

                                    OOPetrisAdditionalInformationField* field =
                                            oopetris_additional_information_create_vector(final_vec);


                                    AdditionalInformationData* return_value = (AdditionalInformationData*)
                                            OOPETRIS_MALLOC(sizeof(AdditionalInformationData));
                                    VERIFY_MALLOC(return_value);

                                    return_value->key = temp->value.vector.key;
                                    return_value->value = field;

                                    stbds_arrfree(temp->value.vector.vectors);
                                    OOPETRIS_FREE(temp);

                                    *data = return_value;
                                    return CommandInsertAddInfo;

                                } else {

                                    OOPetrisAdditionalInformationField** last_vec =
                                            stbds_arrpop(temp->value.vector.vectors);

                                    OOPetrisAdditionalInformationField* field =
                                            oopetris_additional_information_create_vector(last_vec);

                                    stbds_arrput(temp->value.vector.vectors[length - 2], field);
                                    return CommandNoOp;
                                }


                            } else {
                                RETURN_ERROR("Unexpected state in additional info: add vector end");
                            }


                        } else {
                            RETURN_ERROR("Invalid command: %s", input + index);
                        }
                    }


                    return_field_value: {

                        if (*data == NULL) {
                            RETURN_ERROR("Unexpected state in additional info, data NULL: add %s", adding);
                        }

                        TempData* temp = (TempData*) *data;

                        if (temp->type == TempDataEnumAddInfoKey) {
                            // construct a normal CommandInsertAddInfo

                            AdditionalInformationData* return_value =
                                    (AdditionalInformationData*) OOPETRIS_MALLOC(sizeof(AdditionalInformationData));
                            VERIFY_MALLOC(return_value);

                            return_value->key = temp->value.add_info_key.key;
                            return_value->value = field;
                            OOPETRIS_FREE(temp);

                            *data = return_value;
                            return CommandInsertAddInfo;

                        } else if (temp->type == TempDataEnumVector) {
                            // add the field to the vector

                            size_t length = stbds_arrlenu(temp->value.vector.vectors);

                            if (length == 0) {

                                RETURN_ERROR("UNREACHABLE, programming error");
                            }

                            stbds_arrput(temp->value.vector.vectors[length - 1], field);

                            return CommandNoOp;

                        } else {
                            RETURN_ERROR("Unexpected state in additional info: add %s", adding);
                        }


                        //TODO; constcuct a vector
                    }
                    default:
                        RETURN_ERROR("Unknown type '%c'", type);
                }
            }

            RETURN_ERROR("Unknown command: %s", input);
        }

        case StateRecords:
            RETURN_ERROR("NOT IMPLEMENTED YET");
        case StateSnapshots:
            RETURN_ERROR("NOT IMPLEMENTED YET");
        case StateHeaders:
            RETURN_ERROR("NOT IMPLEMENTED YET");
        default:
            RETURN_ERROR("Unknown state");
    }
}

#define STRINGIFY(a) STRINGIFY_HELPER_(a)
#define STRINGIFY_HELPER_(a) #a


int write_to_file(const char* file, bool failOnREPLError) {

    if (file_exists(file)) {
        fprintf(stderr, "File already exists, can't overwrite it!\n");
        return EXIT_FAILURE;
    }

    OOPetrisRecordingInformation* information = oopetris_create_recording_information();

    if (information == NULL) {
        fprintf(stderr, "Couldn't allocate new RecordingInformation\n");
        return EXIT_FAILURE;
    }

    int is_tty = isatty(STDIN_FILENO);

    //TODO:maybe pipe stdout to /dev/null in no tty case

    const char* prompt = is_tty ? "> " : NULL;

    State state = StateRoot;
    void* data = NULL;

    if (is_tty) {
        printf("OOPetris Recordings REPL v%s\nType 'help' for help\n", STRINGIFY(_REPL_VERSION));
    }

    while (true) {
        char* input = readline(prompt);

        if (input == NULL) { /* ctrl-d */
            printf("\n");
            break;
        }

        if (input[0] == '\0') {
            continue;
        }

        const Command command = parse_command(&state, &data, input);

        if (command == CommandNoOp) {
            continue;
        }

        add_history(input);

        switch (command) {
            case CommandPrintHelp:
                printf("Help:\n"
                       "h, help, ?: display helper menu\n"
                       "!: change insertion type, possible values:\n"
                       "\ta: additional information\n"
                       "\tr: records\n"
                       "\ts: snapshots\n"
                       "\th: headers\n"
                       "s: Show current state\n"
                       "<TODO>\n");
                break;
            case CommandNoOp:
                break;

            case CommandError:
                if (data == NULL) {
                    fprintf(stderr, "Error in displaying error, aborting!\n");
                    return EXIT_FAILURE;
                }

                printf("Error: %s\n", (const char*) data);
                OOPETRIS_FREE(data);
                data = NULL;
                if (failOnREPLError) {
                    return EXIT_FAILURE;
                }
                break;
            case CommandInsertAddInfo: {
                AdditionalInformationData* ptr = (AdditionalInformationData*) data;
                oopetris_add_information_field(information->information, ptr->key, ptr->value);
                OOPETRIS_FREE(ptr->key);
                OOPETRIS_FREE(ptr);
                data = NULL;
                break;
            }
            default:
                break;
        }

        OOPETRIS_FREE(input);
    }

    char* write_error = oopetris_write_to_file(information, file, false);

    if (write_error != NULL) {
        fprintf(stderr, "An error occured, while trying to write information to file '%s': %s\n", file, write_error);
        OOPETRIS_FREE(write_error);
        oopetris_free_recording_information(information);
        return EXIT_FAILURE;
    }

    oopetris_free_recording_information(information);


    return EXIT_SUCCESS;
}
#else
int write_to_file(const char* file, bool failOnREPLError) {
    fprintf(stderr, "NOT SUPPORTED\n");
    (void) file;
    (void) failOnREPLError;
    return EXIT_FAILURE;
}
#endif

int main(int argc, char** argv) {

    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }


    const char* mode_str = argv[1];

    Mode mode = ModeRead;

    if (strcmp(mode_str, "r") == 0 || strcmp(mode_str, "read") == 0) {
        mode = ModeRead;
    } else if (strcmp(mode_str, "w") == 0 || strcmp(mode_str, "write") == 0) {
        mode = ModeWrite;
    } else {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* file = argv[2];

    if (mode == ModeRead) {
        return read_file(file);
    } else if (mode == ModeWrite) {
        return write_to_file(file, true);
    } else {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
}
