//
//  areas.h
//  LuxParking
//
//  Created by Olivier on 05/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#pragma once

#include <pebble.h>

typedef struct {
    uint8_t index;
    char name[32];
    uint16_t capacity;
    uint16_t occupancy;
} Area;

void areas_init();
void areas_finalize();