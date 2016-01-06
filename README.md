# LuxParking
A Pebble watchapp for displaying parking occupancy in Luxembourg city using the RSS feed provided by Ville de Luxembourg: http://service.vdl.lu/rss/circulation_guidageparking.php

## Usage
The first screen shows a list of city areas. If you select an area, you will see the list of parkings for that area, with additional information:
* number of available spaces / total spaces
* an icon indicating occupancy trend
* on color watches, the text will be green if the parking has available spaces, red if it's full

## Yet to do
* Add a screen to display full detail for a selected parking 
* Avoid redrawing all items each time an item is added
* Display "loading" message before updating parking list (instead of the previous results currently)
* Sort parkings? (by what?)
* Add some configuration options
* Alternative solution for XML to JSON ?

