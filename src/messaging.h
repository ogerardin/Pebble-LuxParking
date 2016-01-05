//
//  messaging.h
//  LuxParking
//
//  Created by Olivier on 03/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#pragma once

#include <pebble.h>

typedef enum {
    KEY_MESSAGE_TYPE = 1,
    KEY_AREA = 2,
    KEY_CAPACITY = 3,
    KEY_OCCUPANCY = 4,
    KEY_PARKING_ID = 5,
    KEY_NAME = 6,
} MessageKey;

typedef enum {
  MESSAGE_TYPE_PARKING_INFO = 1,
  MESSAGE_TYPE_QUERY_AREA = 2,
  MESSAGE_TYPE_GET_AREAS = 3,
  MESSAGE_TYPE_AREA_INFO = 4,
} MessageType;
