#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <string.h>

#include "mocks/malloc_mock.h"

#define OOPETRIS_REALLOC(p, s) mock_realloc(p, s)
#define OOPETRIS_FREE(p) mock_free(p)

#include <c_wrapper/wrapper.h>

// this tests likely fail, if we update the lib, but this failure on version bump serves as remainder,to check, if other things need changing too, since wrapper may become outdated otherwise

Test(MallocTests, LibVersionNoMallocIsUsed) {
    MockMallocStats* stats = mock_malloc_create_stats();
    const char* lib_version = oopetris_get_lib_version();
    char* dynamic_lib_version = malloc(strlen(lib_version) + 1);

    strcpy(dynamic_lib_version, lib_version);

    cr_assert(eq(str, dynamic_lib_version, "0.5.6"));
    free(dynamic_lib_version);

    cr_assert(mock_malloc_count_realloc(stats) == 0);
    cr_assert(mock_malloc_count_free(stats) == 0);

    mock_malloc_free_stats(stats);
}

/* 
Test(Information, GridProperties) {
    OOPetrisGridProperties* properties = oopetris_get_grid_properties();
    cr_assert_not_null(properties, "returned properties are non-NULL");
    cr_assert(eq(u32, properties->height, 20));
    cr_assert(eq(u32, properties->width, 10));

    FREE_AND_SET_NULL(oopetris_free_grid_properties, properties);
    cr_assert_null(properties, "properties are freed correctly");
} */
