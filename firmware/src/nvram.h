
//*********************************************************************************************************************
//  
//
//*********************************************************************************************************************
//

#ifndef _NVRAM_H_
#define _NVRAM_H_ 
 
 
//*********************************************************************************************************************
// 
//
//*********************************************************************************************************************
//
  
#define KEEP   __attribute__ ((keep)) __attribute__((address(NVM_ADDRESS)))

/* The size of the media used for this demo. */
#define        APP_NVM_MEMORY_AREA_SIZE (DRV_NVM_MEDIA_SIZE * 128) 
  
  
  
  
#endif

