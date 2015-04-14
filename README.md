bristol-seds.github.io
======================
Available at http://bristol-seds.github.io/.

## Running the server for local testing

You need Jekyll installed:

    $ sudo gem install jekyll RedCloth

Start Jekyll:

    $ jekyll serve --watch

Note that changes to `_config.yml` may require restarting the server.

## Adding photo carousels

First put all photos in the `asset_path` directory for a post. Then list all in the front-matter like so:

    photos:
      - url: photo1.jpg
      - url: photo2.jpg
      - url: photo3.jpg

Then simply add {% raw %}`{% include carousel.html %}`{% raw %} where you want the carousel.

## Adding Javascript to posts

Make sure your post has an `asset_path` key-value pair.

### Google Maps

Add a new key, `maps:`, with a list of `url:` key-value pairs as its value, each having
a script containing a Google Maps generator as its value, e.g.:

    maps:
      - url: map1.js
      - url: map2.js

The scripts should be placed in the `asset_path` directory.
For each map, insert a `<div>` tag into the post, with a unique ID referenced by the map generator script.
**TO-DO**: Google Maps KML files must be hosted publicly. Find a way to automatically insert the KML file's fully-qualified url into a map script. Currently I'm just hosting them from http://samhatfield.co.uk
