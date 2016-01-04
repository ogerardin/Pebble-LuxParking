//
//  messaging.h
//  LuxParking
//
//  Created by Olivier on 03/01/2016.
//  Copyright © 2016 Olivier Gérardin. All rights reserved.
//

#pragma once

typedef enum {
    KEY_MESSAGE_TYPE = 1,
    KEY_AREA = 2,
    KEY_PARKING_CAPACITY = 3,
    KEY_PARKING_OCCUPANCY = 4
} MessageKey;

typedef enum {
  MESSAGE_TYPE_PARKING_STATUS = 1,
  MESSAGE_TYPE_QUERY_AREA = 2,
  MESSAGE_TYPE_GET_AREAS = 3,
} MessageType;
