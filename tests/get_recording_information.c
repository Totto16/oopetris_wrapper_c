#include <criterion/criterion.h>
#include <criterion/new/assert.h>

#include <c_wrapper/wrapper.h>

Test(GetRecordingInformation, NullptrAsArgument) {
    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information(NULL);
    cr_assert_not_null(return_value, "NULL is supported and returns an error");


    const bool is_error = oopetris_is_error(return_value);

    cr_assert(is_error, "Non existent recordings file is error");

    const char* error = oopetris_get_error(return_value);

    char* dynamic_error = malloc(strlen(error) + 1);
    strcpy(dynamic_error, error);

    cr_assert(eq(str, dynamic_error, "FilePath is NULL"));
    free(dynamic_error);
}


Test(GetRecordingInformation, NonExistentFile) {
    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./NON_EXISTENT_FILE");
    cr_assert_not_null(return_value, "Non existent recordings file is detected correctly");

    const bool is_error = oopetris_is_error(return_value);

    cr_assert(is_error, "Non existent recordings file is error");

    const char* error = oopetris_get_error(return_value);

    char* dynamic_error = malloc(strlen(error) + 1);
    strcpy(dynamic_error, error);

    cr_assert(eq(str, dynamic_error, "File './NON_EXISTENT_FILE' doesn't exist!"));
    free(dynamic_error);
}

Test(GetRecordingInformation, IncorrectFile) {

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./incorrect.rec");
    cr_assert(return_value != NULL, "Incorrect recordings file is detected correctly");

    const bool is_error = oopetris_is_error(return_value);

    cr_assert(is_error, "Incorrect recordings file is error");

    const char* error = oopetris_get_error(return_value);

    char* dynamic_error = malloc(strlen(error) + 1);
    strcpy(dynamic_error, error);

    cr_assert(
            eq(str, dynamic_error,
               "An error occurred during parsing of the recording file './incorrect.rec': magic file bytes are not "
               "correct, this is either an old format or no recording at all")
    );
    free(dynamic_error);
}


Test(GetRecordingInformation, CorrectFile) {

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./correct.rec");
    cr_assert(return_value != NULL, "Correct recordings file is detected correctly");

    const bool is_error = oopetris_is_error(return_value);

    const char* maybe_error = is_error ? oopetris_get_error(return_value) : "<NO ERROR>";

    cr_assert(not(is_error), "Correct recordings file isn't an error: %s", maybe_error);

    oopetris_free_recording_value_whole(return_value);
}

Test(GetRecordingInformation, CorrectFileEntries) {

    OOPetrisRecordingReturnValue* return_value = oopetris_get_recording_information("./example.rec");
    cr_assert(return_value != NULL, "Correct recordings file is detected correctly");

    const bool is_not_error = !oopetris_is_error(return_value);

    const char* maybe_error = is_not_error ? "<NO ERROR>" : oopetris_get_error(return_value);

    cr_assert(is_not_error, "Correct recordings file isn't an error: %s", maybe_error);


    OOPetrisRecordingInformation* information = oopetris_get_information(return_value);

    oopetris_free_recording_value_only(return_value);

    cr_assert(eq(u32, information->version, 1, "Version is as expected"));

    const char** keys = oopetris_additional_information_get_keys(information->information);

    uint32_t visited_bit_flags = 0;

    for (size_t i = 0; i < oopetris_array_len(keys); ++i) {
        const OOPetrisAdditionalInformationField* field =
                oopetris_additional_information_get_field(information->information, keys[i]);

        cr_assert_not_null(field, "Field is not null");

        const OOPetrisAdditionalInformationType type = oopetris_additional_information_field_get_type(field);

        cr_assert(ge(int, type, 0, "is a valid enum entry"));
        cr_assert(le(int, type, OOPetrisAdditionalInformationType_Vector, "is a valid enum entry"));

        visited_bit_flags |= (1 << (int) type);

        switch (type) {
            case OOPetrisAdditionalInformationType_String:
                oopetris_additional_information_field_get_string(field);
                break;
            case OOPetrisAdditionalInformationType_Float:
                oopetris_additional_information_field_get_float(field);
                break;
            case OOPetrisAdditionalInformationType_Double:
                oopetris_additional_information_field_get_double(field);
                break;
            case OOPetrisAdditionalInformationType_Bool:
                oopetris_additional_information_field_get_bool(field);
                break;
            case OOPetrisAdditionalInformationType_U8:
                oopetris_additional_information_field_get_u8(field);
                break;
            case OOPetrisAdditionalInformationType_I8:
                oopetris_additional_information_field_get_i8(field);
                break;
            case OOPetrisAdditionalInformationType_U32:
                oopetris_additional_information_field_get_u32(field);
                break;
            case OOPetrisAdditionalInformationType_I32:
                oopetris_additional_information_field_get_i32(field);
                break;
            case OOPetrisAdditionalInformationType_U64:
                oopetris_additional_information_field_get_u64(field);
                break;
            case OOPetrisAdditionalInformationType_I64:
                oopetris_additional_information_field_get_i64(field);
                break;
            case OOPetrisAdditionalInformationType_Vector:
                oopetris_additional_information_field_get_vector(field);
                break;

            default:
                cr_fatal("UNREACHABLE");
        }
    }

    for (uint32_t i = OOPetrisAdditionalInformationType_String; i <= OOPetrisAdditionalInformationType_Vector; ++i) {
        const bool bitflag = ((visited_bit_flags >> i) & 1) == 1;
        cr_assert(bitflag, "Visited type: %ld", i);
    }


    oopetris_additional_information_keys_free(keys);


    oopetris_free_recording_information(information);
}
