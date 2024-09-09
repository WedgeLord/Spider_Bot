#ifndef SPIDER_TYPES
#define SPIDER_TYPES

#define max( a, b ) ( (a > b) ? a : b )

#ifdef __CPLUSPLUS
extern "C" {
#endif

  // this is meh because the array address is passed around and could be changed mid-execution (EXTREMELY UNLIKELY)
  typedef struct { uint8_t p[4]; } tie_params; // maintains agreement between modules
  
  typedef struct { 
    uint8_t lower;
    uint8_t upper;
  } constraint;

  typedef struct {
    constraint leg1;
    constraint leg2;
  } tie_return;

  typedef struct {
      uint32_t leg;
      uint32_t height;
      uint32_t pivot;
  } spider_leg_t;

  const size_t MAX_TRANSMISSION = 
    max( max( 
      sizeof( tie_params ), 
      sizeof( tie_return ) ), 
      sizeof( spider_leg_t ) );

#ifdef __CPLUSPLUS
}
#endif

#endif