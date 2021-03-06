#include "testutil.h"
#include <npk.h>
#include <npk_dev.h>
#include <time.h>

int libnpk_durability( int argc, char * argv [] )
{
    srand((unsigned int)time(NULL));
    NPK_PACKAGE pack;
    NPK_ENTITY entity;

    for( int t = 0; t < 100; ++t )
    {
        int teakey[4] = {rand(),rand(),rand(),rand()};

        // create a pack
        CHECK( NPK_SUCCESS == npk_package_alloc( &pack, teakey ) );
        CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "sample.txt", &entity ) );
        CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zip.txt", &entity ) );
        CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS_ZLIB ) );
        CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "tea.txt", &entity ) );
        CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_ENCRYPT_TEA ) );
        CHECK( NPK_SUCCESS == npk_package_add_file( pack, "sample.txt", "zipntea.txt", &entity ) );
        CHECK( NPK_SUCCESS == npk_entity_set_flag( entity, NPK_ENTITY_COMPRESS_ZLIB | NPK_ENTITY_ENCRYPT_TEA | NPK_ENTITY_REVERSE ) );
        CHECK( NPK_SUCCESS == npk_package_save( pack, "foo.npk", true ) );
        npk_package_close( pack );

        // validation
        pack = npk_package_open( "foo.npk", teakey );
        entity = npk_package_get_first_entity( pack );

        while( entity )
        {
            CHECK( entity != NULL );

            NPK_SIZE size = npk_entity_get_size( entity );
            void* buf = malloc( size );

            CHECK( npk_entity_read( entity, buf ) );
            CHECK_EQUAL_STR_WITH_FILE( (const char*)buf, "sample.txt" );

            free( buf );
            entity = npk_entity_next( entity );
        }
        npk_package_close( pack );
    }

    return 0;
}
