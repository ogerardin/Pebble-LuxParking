
#Quick explanation of the protocol between LuxParking watchapp and Pebble JS


App keys are defined in appinfo.json as required, and are automatically exported to appinfo.h during build to be usable from C.
Enumerated values for some keys are defined in keys.json and are automatically exported to keys.h and keys.js during build.

##Description of app keys used

###Generic keys
- type:  message type (mandatory); value is PARKING, AREA or ERROR
- method: method (mandatory); value is REQUEST_GET, REPLY_COUNT, REPLY_ITEM
- error: error message; mandatory when method=REPLY_ERROR
- count: item count; mandatory when method=REPLY_COUNT
- index: item index; mandatory when method=REPLY_ITEM
- name: item name; mandatory when method=REPLY_ITEM
- id: item index; used with method=REPLY_ITEM

###Additional keys used with type=PARKING and method=REPLY_ITEM
- capacity: parking capacity
- free: free parking spaces
- trend: parking occupancy trend (-1, 0 or 1)
- area: area name
- open: parking open ? (1 or 0)

###Additional keys used with type=AREA and method=REPLY_ITEM
None.

##Enumerated values

###Values for key type
- PARKING: message relates to parkings
- AREA: message relate to city areas
- ERROR: message is an error

###Values for key "method"
- REQUEST_GET: Message is a request to obtain a list of items as specified by message type.
  Additional criteria may be specified with extra tuples */
- REPLY_COUNT: Message is a reply containing the number of items returned by a REQUEST_GET.
  Actual count is contained in tuple with key=COUNT.
  Always sent first, followed by as many REPLY_ITEM messages as required. */
- REPLY_ITEM: Message is reply containing a single item returned by a REQUEST_GET.
  Actual item properties are contained in tuples depending on item type; only INDEX and NAME are mandatory. */
- REPLY_ERROR: Message is a reply specifying that an error occured */

