

#include "core/game/mino_stack.hpp"
#include "core/game/tetromino_type.hpp"
#include "core/helper/input_event.hpp"
#include "oopetris_wrapper.h"
#include "opaque_types.h"
#include "recordings/utility/additional_information.hpp"
#include "recordings/utility/recording_writer.hpp"
#include <filesystem>
#include <memory>
#include <utility>


#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

#define STB_DS_IMPLEMENTATION
#define STBDS_NO_SHORT_NAMES
#define STBDS_SIPHASH_2_4
#include "./thirdparty/stb_ds.h"


#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif


#include <core/core.hpp>
#include <cstring>
#include <ctime>
#include <recordings/recordings.hpp>


bool oopetris_is_recording_file(const char* file_path) {

    if (file_path == nullptr) {
        return false;
    }

    if (not std::filesystem::exists(file_path)) {
        return false;
    }

    auto parsed = recorder::RecordingReader::from_path(file_path);

    return parsed.has_value();
}


static OOPetrisRecordingReturnValue*
construct_error_from_cstr_impl(OOPetrisRecordingReturnValue* return_value, const char* value, size_t length) {

    auto* alloced_str = static_cast<char*>(malloc(length + 1));

    if (alloced_str == nullptr) {
        free(return_value);
        return nullptr;
    }

#if defined(_MSC_VER)
    std::memcpy(alloced_str, value, length + 1);
#else
    std::strcpy(alloced_str, value);
#endif


    return_value->is_error = true;
    return_value->value.error = alloced_str;

    return return_value;
}

static OOPetrisRecordingReturnValue*
construct_error_from_cstr(OOPetrisRecordingReturnValue* return_value, const char* value) {
    return construct_error_from_cstr_impl(return_value, value, strlen(value));
}

static OOPetrisRecordingReturnValue*
construct_error_from_string(OOPetrisRecordingReturnValue* return_value, const std::string& value) {
    return construct_error_from_cstr_impl(return_value, value.c_str(), value.size());
}

static void vector_of_value_free(OOPetrisAdditionalInformationField** vector, size_t until_index);

static OOPetrisAdditionalInformationField* information_value_to_c(const recorder::InformationValue& value) {


    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }

    return std::visit(
            helper::overloaded{ [return_value](const std::string& value) -> OOPetrisAdditionalInformationField* {
                                   auto* string = static_cast<char*>(malloc(value.size() + 1));

                                   if (string == nullptr) {
                                       free(return_value);
                                       return nullptr;
                                   }


#if defined(_MSC_VER)
                                   std::memcpy(string, value.c_str(), value.size() + 1);
#else
                                   std::strcpy(string, value.c_str());
#endif


                                   return_value->type = OOPetrisAdditionalInformationType_String;
                                   return_value->value.string = string;
                                   return return_value;
                               },
                                [return_value](const float& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Float;
                                    return_value->value.float_v = value;
                                    return return_value;
                                },
                                [return_value](const double& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Double;
                                    return_value->value.double_v = value;
                                    return return_value;
                                },
                                [return_value](const bool& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_Bool;
                                    return_value->value.bool_v = value;
                                    return return_value;
                                },
                                [return_value](const u8& value) {
                                    return_value->type = OOPetrisAdditionalInformationType_U8;
                                    return_value->value.u8 = value;
                                    return return_value;
                                },
                                [return_value](const i8& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_I8;
                                    return_value->value.i8 = value;
                                    return return_value;
                                },
                                [return_value](const u32& value) {
                                    return_value->type = OOPetrisAdditionalInformationType_U32;
                                    return_value->value.u32 = value;
                                    return return_value;
                                },
                                [return_value](const i32& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_I32;
                                    return_value->value.i32 = value;
                                    return return_value;
                                },
                                [return_value](const u64& value) {
                                    return_value->type = OOPetrisAdditionalInformationType_U64;
                                    return_value->value.u64 = value;
                                    return return_value;
                                },
                                [return_value](const i64& value) -> OOPetrisAdditionalInformationField* {
                                    return_value->type = OOPetrisAdditionalInformationType_I64;
                                    return_value->value.i64 = value;
                                    return return_value;
                                },
                                [return_value](const std::vector<recorder::InformationValue>& value
                                ) -> OOPetrisAdditionalInformationField* {
                                    OOPetrisAdditionalInformationField** fields = NULL;


                                    stbds_arrsetlen(fields, value.size());

                                    for (std::size_t i = 0; i < value.size(); ++i) {
                                        auto* result = information_value_to_c(value.at(i));
                                        if (result == nullptr) {
                                            vector_of_value_free(fields, i);
                                            free(return_value);
                                            return nullptr;
                                        }
                                        fields[i] = result;
                                    }

                                    return_value->type = OOPetrisAdditionalInformationType_Vector;
                                    return_value->value.vector = fields;

                                    return return_value;
                                } },
            value.underlying()
    );
}

static void free_additional_value_field(OOPetrisAdditionalInformationField* field) {

    if (field->type == OOPetrisAdditionalInformationType_String) {
        free(field->value.string);
    } else if (field->type == OOPetrisAdditionalInformationType_Vector) {
        auto* vector = field->value.vector;
        vector_of_value_free(vector, stbds_arrlenu(vector));
        field->value.vector = NULL;
    }


    free(field);
}


static void vector_of_value_free(OOPetrisAdditionalInformationField** vector, size_t until_index) {


    for (size_t i = 0; i < until_index; ++i) {
        free_additional_value_field(vector[i]);
    }


    stbds_arrfree(vector);
}


static OOPetrisAdditionalInformation* recording_reader_get_additional_information(
        const recorder::AdditionalInformation& information
) {

    stbds_rand_seed(time(NULL));

    OOPetrisAdditionalInformation* result =
            (OOPetrisAdditionalInformation*) malloc(sizeof(OOPetrisAdditionalInformation));

    if (result == nullptr) {
        return nullptr;
    }

    result->values = NULL;

    //NOTE: We use stbds_sh_new_strdup, so keys are automatically copied (malloced) and freed and we don't need to manage those
    stbds_sh_new_strdup(result->values);


    for (const auto& [key, value] : information) {

        auto* insert_value = information_value_to_c(value);

        stbds_shput(result->values, key.c_str(), insert_value);
    }

    return result;
}

const char** oopetris_additional_information_get_keys(OOPetrisAdditionalInformation* information) {

    const char** result_arr = NULL;

    const auto length = stbds_shlen(information->values);

    stbds_arrsetlen(result_arr, length);

    for (int i = 0; i < length; ++i) {

        result_arr[i] = information->values[i].key;
    }


    return result_arr;
}

void oopetris_additional_information_keys_free(const char** keys) {
    stbds_arrfree(keys);
}

const OOPetrisAdditionalInformationField*
oopetris_additional_information_get_field(OOPetrisAdditionalInformation* information, const char* key) {

    const auto index = stbds_shgeti(information->values, key);

    if (index < 0) {
        return nullptr;
    }

    return information->values[index].value;
}


OOPetrisAdditionalInformationType oopetris_additional_information_field_get_type(
        const OOPetrisAdditionalInformationField* const field
) {
    return field->type;
}

const char* oopetris_additional_information_field_get_string(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_String);
    return field->value.string;
}

float oopetris_additional_information_field_get_float(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Float);
    return field->value.float_v;
}

double oopetris_additional_information_field_get_double(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Double);
    return field->value.double_v;
}


bool oopetris_additional_information_field_get_bool(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Bool);
    return field->value.bool_v;
}


uint8_t oopetris_additional_information_field_get_u8(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_U8);
    return field->value.u8;
}


int8_t oopetris_additional_information_field_get_i8(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_I8);
    return field->value.i8;
}


uint32_t oopetris_additional_information_field_get_u32(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_U32);
    return field->value.u32;
}


int32_t oopetris_additional_information_field_get_i32(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_I32);
    return field->value.i32;
}


uint64_t oopetris_additional_information_field_get_u64(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_U64);
    return field->value.u64;
}


int64_t oopetris_additional_information_field_get_i64(const OOPetrisAdditionalInformationField* const field) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_I64);
    return field->value.i64;
}


const OOPetrisAdditionalInformationField* const* oopetris_additional_information_field_get_vector(
        const OOPetrisAdditionalInformationField* const field
) {
    assert(oopetris_additional_information_field_get_type(field) == OOPetrisAdditionalInformationType_Vector);
    return field->value.vector;
}


static void oopetris_additional_information_free(OOPetrisAdditionalInformation* information) {


    for (int i = 0; i < stbds_shlen(information->values); ++i) {

        free_additional_value_field(information->values[i].value);
    }

    stbds_shfree(information->values);
    free(information);
}

static OOPetrisTetrionRecord record_to_c(const recorder::Record& record) {
    return OOPetrisTetrionRecord{ .simulation_step_index = record.simulation_step_index,
                                  .event = static_cast<OOPetrisInputEvent>(record.event),
                                  .tetrion_index = record.tetrion_index };
}

static OOPetrisMino mino_to_c(const Mino& mino) {
    auto orig_pos = mino.position();

    auto position = OOPetrisMinoPosition{ .x = orig_pos.x, .y = orig_pos.y };

    return OOPetrisMino{ .position = position, .type = static_cast<OOPetrisTetrominoType>(mino.type()) };
}

static OOPetrisTetrionSnapshot snapshot_to_c(const TetrionSnapshot& snapshot) {


    // convert mino_stack

    auto minos = snapshot.mino_stack().minos();

    OOPetrisMino* mino_stack = NULL;
    stbds_arrsetlen(mino_stack, minos.size());

    for (std::size_t i = 0; i < minos.size(); ++i) {
        mino_stack[i] = mino_to_c(minos.at(i));
    }


    return OOPetrisTetrionSnapshot{ .level = snapshot.level(),
                                    .mino_stack = mino_stack,
                                    .score = snapshot.score(),
                                    .simulation_step_index = snapshot.simulation_step_index(),
                                    .lines_cleared = snapshot.lines_cleared(),
                                    .tetrion_index = snapshot.tetrion_index() };
}

static OOPetrisTetrionHeader tetrion_header_to_c(const recorder::TetrionHeader& header) {


    return OOPetrisTetrionHeader{ .seed = header.seed, .starting_level = header.starting_level };
}


static OOPetrisRecordingInformation* recording_reader_to_c(const recorder::RecordingReader& value) {

    auto* return_value = static_cast<OOPetrisRecordingInformation*>(malloc(sizeof(OOPetrisRecordingInformation)));


    if (return_value == nullptr) {
        return nullptr;
    }

    return_value->version = recorder::Recording::current_supported_version_number;

    return_value->information = recording_reader_get_additional_information(value.information());
    if (return_value->information == nullptr) {
        free(return_value);
        return nullptr;
    }

    // convert records

    auto records = value.records();

    return_value->records = NULL;
    stbds_arrsetlen(return_value->records, records.size());

    for (std::size_t i = 0; i < records.size(); ++i) {
        return_value->records[i] = record_to_c(records.at(i));
    }

    // convert snapshots

    auto snapshots = value.snapshots();

    return_value->snapshots = NULL;
    stbds_arrsetlen(return_value->snapshots, snapshots.size());

    for (std::size_t i = 0; i < snapshots.size(); ++i) {
        return_value->snapshots[i] = snapshot_to_c(snapshots.at(i));
    }

    // convert tetrion_headers

    auto tetrion_headers = value.tetrion_headers();

    return_value->tetrion_headers = NULL;
    stbds_arrsetlen(return_value->tetrion_headers, tetrion_headers.size());

    for (std::size_t i = 0; i < tetrion_headers.size(); ++i) {
        return_value->tetrion_headers[i] = tetrion_header_to_c(tetrion_headers.at(i));
    }

    return return_value;
}


OOPetrisRecordingReturnValue* oopetris_get_recording_information(const char* file_path) {

    auto* return_value = static_cast<OOPetrisRecordingReturnValue*>(malloc(sizeof(OOPetrisRecordingReturnValue)));

    if (return_value == nullptr) {
        return nullptr;
    }

    if (file_path == nullptr) {
        return construct_error_from_cstr(return_value, "FilePath is NULL");
    }


    if (not std::filesystem::exists(file_path)) {
        std::string error = "File '";
        error += file_path;
        error += "' doesn't exist!";

        return construct_error_from_string(return_value, error);
    }

    auto parsed = recorder::RecordingReader::from_path(file_path);

    if (not parsed.has_value()) {
        std::string error = "An error occurred during parsing of the recording file '";
        error += file_path;
        error += "': ";
        error += parsed.error();

        return construct_error_from_string(return_value, error);
    }

    const auto recording_reader = std::move(parsed.value());

    OOPetrisRecordingInformation* information = recording_reader_to_c(recording_reader);

    if (information == nullptr) {
        return construct_error_from_cstr(return_value, "Failure in internal conversion");
    }

    return_value->is_error = false;
    return_value->value.information = information;
    return return_value;
}

bool oopetris_is_error(OOPetrisRecordingReturnValue* value) {
    return value->is_error;
}

const char* oopetris_get_error(OOPetrisRecordingReturnValue* value) {
    return value->value.error;
}

OOPetrisRecordingInformation* oopetris_get_information(OOPetrisRecordingReturnValue* value) {
    return value->value.information;
}

void oopetris_free_recording_information(OOPetrisRecordingInformation* information) {

    oopetris_additional_information_free(information->information);

    stbds_arrfree(information->records);


    for (std::size_t i = 0; i < stbds_arrlenu(information->snapshots); ++i) {
        const auto* mino_stack = information->snapshots[i].mino_stack;
        stbds_arrfree(mino_stack);
    }

    stbds_arrfree(information->snapshots);

    stbds_arrfree(information->tetrion_headers);

    free(information);
}

void oopetris_free_recording_value_only(OOPetrisRecordingReturnValue* information) {
    free(information);
}

void oopetris_free_recording_value_whole(OOPetrisRecordingReturnValue* information) {
    if (oopetris_is_error(information)) {
        free(information->value.error);
    } else {
        oopetris_free_recording_information(information->value.information);
    }

    oopetris_free_recording_value_only(information);
}

const char* oopetris_get_lib_version(void) {
    return utils::version();
}

OOPetrisGridProperties* oopetris_get_grid_properties(void) {
    auto* properties = static_cast<OOPetrisGridProperties*>(malloc(sizeof(OOPetrisGridProperties)));
    if (properties == nullptr) {
        return nullptr;
    }

    properties->height = grid::height_in_tiles;
    properties->width = grid::width_in_tiles;

    return properties;
}


void oopetris_free_grid_properties(OOPetrisGridProperties* properties) {
    free(properties);
}


size_t oopetris_array_len(void* const array) {
    return stbds_arrlenu(array);
}

// CREATION functions

OOPetrisRecordingInformation* oopetris_create_recording_information(void) {

    auto* return_value = static_cast<OOPetrisRecordingInformation*>(malloc(sizeof(OOPetrisRecordingInformation)));


    if (return_value == nullptr) {
        return nullptr;
    }

    return_value->version = recorder::Recording::current_supported_version_number;

    stbds_rand_seed(time(NULL));
    return_value->information = NULL;


    return_value->information = (OOPetrisAdditionalInformation*) malloc(sizeof(OOPetrisAdditionalInformation));

    if (return_value->information == nullptr) {
        return nullptr;
    }

    return_value->information->values = NULL;

    stbds_sh_new_strdup(return_value->information->values);

    return_value->records = NULL;

    return_value->snapshots = NULL;

    return_value->tetrion_headers = NULL;

    return return_value;
}

void oopetris_add_information_field(
        OOPetrisAdditionalInformation* additional_information,
        const char* key,
        OOPetrisAdditionalInformationField* value
) {

    stbds_shput(additional_information->values, key, value);
}


OOPetrisAdditionalInformationField* oopetris_additional_information_create_string(const char* value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    const size_t length = strlen(value);

    auto* string = static_cast<char*>(malloc(length + 1));

    if (string == nullptr) {
        free(return_value);
        return nullptr;
    }

#if defined(_MSC_VER)
    std::memcpy(string, value, length + 1);
#else
    std::strcpy(string, value);
#endif

    return_value->type = OOPetrisAdditionalInformationType_String;
    return_value->value.string = string;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_float(float value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_Float;
    return_value->value.float_v = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_double(double value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_Double;
    return_value->value.double_v = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_bool(bool value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_Bool;
    return_value->value.bool_v = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_u8(uint8_t value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_U8;
    return_value->value.u8 = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_i8(int8_t value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_I8;
    return_value->value.i8 = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_u32(uint32_t value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_U32;
    return_value->value.u32 = value;
    return return_value;
}


OOPetrisAdditionalInformationField* oopetris_additional_information_create_i32(int32_t value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_I32;
    return_value->value.i32 = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_u64(uint64_t value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_U64;
    return_value->value.u64 = value;
    return return_value;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_i64(int64_t value) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_I64;
    return_value->value.i64 = value;
    return return_value;
}

OOPetrisAdditionalInformationField** oopetris_additional_information_create_empty_vector(void) {
    OOPetrisAdditionalInformationField** result = NULL;
    return result;
}

OOPetrisAdditionalInformationField** oopetris_additional_information_create_vector_with_size(size_t size) {
    OOPetrisAdditionalInformationField** result = NULL;
    stbds_arrsetlen(result, size);
    return result;
}

OOPetrisAdditionalInformationField* oopetris_additional_information_create_vector(
        OOPetrisAdditionalInformationField** value
) {
    auto* return_value =
            static_cast<OOPetrisAdditionalInformationField*>(malloc(sizeof(OOPetrisAdditionalInformationField)));


    if (return_value == nullptr) {
        return nullptr;
    }


    return_value->type = OOPetrisAdditionalInformationType_Vector;
    return_value->value.vector = value;
    return return_value;
}

void oopetris_add_record(OOPetrisRecordingInformation* information, OOPetrisTetrionRecord record) {
    stbds_arrput(information->records, record);
}

void oopetris_add_snapshot(OOPetrisRecordingInformation* information, OOPetrisTetrionSnapshot snapshot) {
    stbds_arrput(information->snapshots, snapshot);
}

void oopetris_add_header(OOPetrisRecordingInformation* information, OOPetrisTetrionHeader tetrion_header) {
    stbds_arrput(information->tetrion_headers, tetrion_header);
}

static char* str_to_error(const std::string& error) {
    auto* alloced_str = static_cast<char*>(malloc(error.size() + 1));

    if (alloced_str == nullptr) {
        return nullptr;
    }

#if defined(_MSC_VER)
    std::memcpy(alloced_str, error.c_str(), error.size() + 1);
#else
    std::strcpy(alloced_str, error.c_str());
#endif

    return alloced_str;
}

static recorder::InformationValue additonal_information_field_to_cpp(const OOPetrisAdditionalInformationField* field) {
    switch (oopetris_additional_information_field_get_type(field)) {
        case OOPetrisAdditionalInformationType_String:
            return recorder::InformationValue{ std::string{ oopetris_additional_information_field_get_string(field) } };
        case OOPetrisAdditionalInformationType_Float:
            return recorder::InformationValue{ oopetris_additional_information_field_get_float(field) };
        case OOPetrisAdditionalInformationType_Double:
            return recorder::InformationValue{ oopetris_additional_information_field_get_double(field) };
        case OOPetrisAdditionalInformationType_Bool:
            return recorder::InformationValue{ oopetris_additional_information_field_get_bool(field) };
        case OOPetrisAdditionalInformationType_U8:
            return recorder::InformationValue{ oopetris_additional_information_field_get_u8(field) };
        case OOPetrisAdditionalInformationType_I8:
            return recorder::InformationValue{ oopetris_additional_information_field_get_i8(field) };
        case OOPetrisAdditionalInformationType_U32:
            return recorder::InformationValue{ oopetris_additional_information_field_get_u32(field) };
        case OOPetrisAdditionalInformationType_I32:
            return recorder::InformationValue{ oopetris_additional_information_field_get_i32(field) };
        case OOPetrisAdditionalInformationType_U64:
            return recorder::InformationValue{ oopetris_additional_information_field_get_u64(field) };
        case OOPetrisAdditionalInformationType_I64:
            return recorder::InformationValue{ oopetris_additional_information_field_get_i64(field) };
        case OOPetrisAdditionalInformationType_Vector: {

            const auto raw_vector = oopetris_additional_information_field_get_vector(field);

            const auto length = stbds_arrlenu(raw_vector);

            std::vector<recorder::InformationValue> final_vector{};
            final_vector.reserve(length);

            for (std::size_t i = 0; i < length; ++i) {
                const auto value = raw_vector[i];
                final_vector.push_back(additonal_information_field_to_cpp(value));
            }

            return recorder::InformationValue{ final_vector };
        }

        default:
            assert(false && "UNREACHABLE");
            std::unreachable();
    };
}

char* oopetris_write_to_file(OOPetrisRecordingInformation* information, const char* file_path, bool overwrite) {

    std::vector<recorder::TetrionHeader> headers{};

    for (std::size_t i = 0; i < stbds_arrlenu(information->tetrion_headers); ++i) {
        const auto header = information->tetrion_headers[i];
        headers.emplace_back(header.seed, header.starting_level);
    }

    recorder::AdditionalInformation additional_information{};

    const auto keys = oopetris_additional_information_get_keys(information->information);


    for (std::size_t i = 0; i < stbds_arrlenu(keys); ++i) {
        const auto key = keys[i];
        const auto value = additonal_information_field_to_cpp(
                oopetris_additional_information_get_field(information->information, key)
        );
        additional_information.add_value(key, value);
    }

    oopetris_additional_information_keys_free(keys);

    auto writer_value = recorder::RecordingWriter::get_writer(
            std::filesystem::path{ file_path }, std::move(headers), std::move(additional_information), overwrite
    );

    if (not writer_value.has_value()) {
        return str_to_error(writer_value.error());
    }

    auto writer = std::move(writer_value.value());

    for (std::size_t i = 0; i < stbds_arrlenu(information->records); ++i) {

        auto record = information->records[i];

        auto res = writer.add_record(
                record.tetrion_index, record.simulation_step_index, static_cast<InputEvent>(record.event)
        );

        if (not res.has_value()) {
            return str_to_error(res.error());
        }
    }


    for (std::size_t i = 0; i < stbds_arrlenu(information->snapshots); ++i) {

        auto snapshot = information->snapshots[i];

        MinoStack mino_stack{};

        for (std::size_t j = 0; j < stbds_arrlenu(snapshot.mino_stack); ++j) {
            const auto mino = snapshot.mino_stack[j];
            mino_stack.set({ mino.position.x, mino.position.y }, static_cast<helper::TetrominoType>(mino.type));
        }

        auto core_information = std::make_unique<TetrionCoreInformation>(
                snapshot.tetrion_index, snapshot.level, snapshot.score, snapshot.lines_cleared, std::move(mino_stack)
        );

        auto res = writer.add_snapshot(snapshot.simulation_step_index, std::move(core_information));
        if (not res.has_value()) {
            return str_to_error(res.error());
        }
    }

    //TODO:
    return NULL;
}
