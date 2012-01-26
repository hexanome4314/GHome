#ifndef __IOS_DATA_MGR_H__
#define __IOS_DATA_MGR_H__

#include <pthread.h>

#include "drv_manager.h"
#include "drv_error.h"

char device_data_matrix[DRV_MAX_VALUE][DRV_MAX_COUNT];

int  ios_data_start_collect();
void ios_data_stop_collect();

void* ios_data_collector_callback( void* ptr );

#endif
