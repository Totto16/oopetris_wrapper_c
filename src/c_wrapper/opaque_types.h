

#pragma once

#include "oopetris_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif


struct OOPetrisRecordingReturnValueImpl {
    bool is_error;
    union {
        OOPetrisRecordingInformation* information;
        char* error;
    } value;
};


struct OOPetrisAdditionalInformationFieldImpl {
    OOPetrisAdditionalInformationType type;
    union {
        char* string;
        float float_v;
        double double_v;
        bool bool_v;
        uint8_t u8;
        int8_t i8;
        uint32_t u32;
        int32_t i32;
        uint64_t u64;
        int64_t i64;
        OOPetrisAdditionalInformationField** vector;
    } value;
};

typedef struct {
    char* key;
    OOPetrisAdditionalInformationField* value;
} OOPetrisAdditionalInformationValue;


struct OOPetrisAdditionalInformationImpl {
    OOPetrisAdditionalInformationValue* values;
};


#ifdef __cplusplus
}
#endif
