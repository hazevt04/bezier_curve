
#include <iostream>
#include <string.h>
#include <stdio.h>

#include "Util.h"
#include "png.h"
#include "ImageData.h"
#include "PNGRenderer.h"

PNGRenderer::PNGRenderer( PNGImageData* image_data ) {
   this->image_data = image_data;
   
   this->png_ptr = NULL;
   this->info_ptr = NULL;
   this->row = NULL;
}

// Let's let val be 687. MAIN COLOR is val/768
// This takes the float value 'val', converts it to red, green & blue values,
// then sets those values into the image memory buffer location pointed to by
// 'ptr'
void PNGRenderer::set_rgb( png_byte* ptr, ulong val ) {
   memcpy( ptr, &val, 3 );
}


int PNGRenderer::write_png( char* title = NULL ) {
   int code            = 0;
   PNGImageData* image_data = this->image_data;
  
   if ( !image_data ) {
      fprintf( stderr, "%s(): ERROR: image_data is NULL!\n", __func__ );
      return 1;
   }
   std::string t_filename = image_data->get_filename( );
   char filename[100];
   strcpy( filename, t_filename.c_str( ) );

   int width = ( int )image_data->get_width( );
   int height = ( int )image_data->get_height( );
   //ulong* buffer = image_data->get_pixels( );

   // Open file for writing (binary mode)
   fp = fopen( filename, "wb" );
   if ( fp == NULL ) {
      fprintf( stderr, "Could not open file %s for writing\n", filename );
      return 1;
   }

   // Initialize write structure
   png_ptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, 
         NULL, NULL, NULL );
   if ( png_ptr == NULL ) {
      fprintf( stderr, "Could not allocate write struct\n" );
      return 1;
   }

   // Initialize info structure
   info_ptr = png_create_info_struct( png_ptr );
   if ( info_ptr == NULL ) {
      fprintf( stderr, "Could not allocate info struct\n" );
      return 1;
   }

   // Setup Exception handling
   if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
      fprintf( stderr, "Error during png creation\n" );
      return 1;
   }

   png_init_io( png_ptr, fp );

   // Write header (8 bit color depth)
   png_set_IHDR( png_ptr, info_ptr, width, height, 8, 
         PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, 
         PNG_COMPRESSION_TYPE_BASE,
         PNG_FILTER_TYPE_BASE );

   // Set title
   if ( title != NULL ) {
      png_text title_text;
      title_text.compression = PNG_TEXT_COMPRESSION_NONE;
      char* title_key = ( char* )"Title";
      title_text.key         = title_key;
      title_text.text        = title;
      png_set_text( png_ptr, info_ptr, &title_text, 1 );
   }

   png_write_info( png_ptr, info_ptr );

   // Write image data
   DEBUG_PRINTF( "%s() Total points: %d\n", __func__, ( width * height ) );
   DEBUG_PRINTF( "%s() Util::WHITE = %lx\n", __func__, Util::WHITE );
   DEBUG_PRINTF( "%s() Util::BLACK = %lx\n", __func__, Util::BLACK );
   
   png_byte** png_rows = image_data->get_png_rows();
   png_write_image( png_ptr, png_rows );
   
   // End write
   png_write_end( png_ptr, NULL );

   return code;
}


PNGRenderer::~PNGRenderer() {
   if ( info_ptr != NULL )
      png_free_data( png_ptr, info_ptr, PNG_FREE_ALL, -1 );
   if ( png_ptr != NULL )
      png_destroy_write_struct( &png_ptr, ( png_infopp )NULL );
   if ( row != NULL )
      free( row );
}


void PNGRenderer::render() {
   ImageData* image_data = this->image_data;
   ulong* pixels = image_data->get_pixels( );
   int width = (int)image_data->get_width( );
   int height = (int)image_data->get_height( );
   std::string filename = image_data->get_filename( );
   char fname[100];
   strcpy( fname, filename.c_str() );
   
   int result = write_png( fname ); 
   if ( result ) {
      fprintf( stderr, 
            "Exiting due to error in rendering PNG file." );
      exit(EXIT_FAILURE);
   }
   
}



