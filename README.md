# LuxParking
A Pebble watchapp for displaying parking occupancy in Luxembourg city using the RSS feed provided by Ville de Luxembourg: http://service.vdl.lu/rss/circulation_guidageparking.php

## Usage
The first screen shows a list of city areas. If you select an area, you will see the list of parkings for that area, with additional information:
* number of available spaces / total spaces
* an icon indicating occupancy trend
* on color watches, the text will be green if the parking has available spaces, red if it's full

## Yet to do
* Speed improvements. Refresh is much slower on the actual device than on the emulator.
* Add a screen to display full detail for a selected parking 
* Avoid redrawing all items each time an item is added
* Auto-refresh parking data 
* Sort parkings (by what?)
* Add some configuration options
* Store last selected area
* i18n
* Alternative solution for XML to JSON 

