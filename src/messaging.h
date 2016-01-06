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
  /** Key for message type (mandatory); value is one of MessageType constants */
  KEY_TYPE = 1,
  /** Key for message method (mandatory); value is one of MessageMethod constants */
  KEY_METHOD = 2,

  /** Key for error message; mandatory when method=REPLY_ERROR*/
  KEY_ERROR = 3,
  
  /** Key for item count; mandatory when method=REPLY_COUNT */
  KEY_COUNT = 4,
  
  /** Key for item index; mandatory when method=REPLY_ITEM */
  KEY_INDEX = 5,
  /** Key for item name; mandatory when method=REPLY_ITEM */
  KEY_NAME = 6,
  /** Key for item index; used with method=REPLY_ITEM */
  KEY_ID = 7,
  
  /** Key for parking capacity; used with type=PARKING and method=REPLY_ITEM */
  KEY_CAPACITY = 8,
  /** Key for current parking occupancy; used with type=PARKING and method=REPLY_ITEM */
  KEY_FREE = 9,
  /** Key for current parking occupancy trend; used with type=PARKING and method=REPLY_ITEM */
  KEY_TREND = 10,
  /** Key for area name */
  KEY_AREA = 11,
  
} MessageKey;

typedef enum {
  /** Message relates to parkings */
  TYPE_PARKING = 1,
  /** Message relates to parking areas */
  TYPE_AREA = 2,
  /** Message is an error */
  TYPE_ERROR = 3,
} MessageType;

typedef enum {
  /** Message is a request to obtain a list of items as specified by message type.
  Additional criteria may be specified with extra tuples */
  METHOD_REQUEST_GET = 1, 
  
  /** Message is a reply containing the number of items returned by a REQUEST_GET. 
  Actual count is contained in tuple with key=COUNT.
  Always sent first, followed by as many REPLY_ITEM messages as required. */
  METHOD_REPLY_COUNT = 2,
  
  /** Message is reply containing a single item returned by a REQUEST_GET.
  Actual item properties are contained in tuples depending on item type; only INDEX and NAME are mandatory. */
  METHOD_REPLY_ITEM = 3,
  
  /** Message is a reply specifying that an error occured */
  METHOD_REPLY_ERROR = 4,
} MessageMethod;

