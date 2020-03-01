/*
 * ======================== dyParams.h ==========================
 *                          -- tpr --
 *                                        CREATE -- 2019.10.05
 *                                        MODIFY --
 * ----------------------------------------------------------
 */
#ifndef TPR_DY_PARAMS_H
#define TPR_DY_PARAMS_H

//-------------------- CPP --------------------//
#include <vector>
#include <set>
#include <cstdint> // uint8_t


//-------------------- Engine --------------------//
#include "DyParam.h"
#include "MapAltitude.h"
#include "Density.h"
#include "GoDataForCreate.h"

#include "Job_Field.h"



struct DyParams_GoDataForCreate{    
    const GoDataForCreate   *goDataPtr {};
};


#endif 

