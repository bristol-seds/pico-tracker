## Design of the geofence

There's some notes in a ipython notebook `geofence.ipynp` on
this. This notebook is also used to calculate the deviation values.

## Prerequisites

```
sudo apt-get install ipython libblas-dev liblapack-dev gfortran python-pyproj
sudo pip install scipy matplotlib
sudo pip install --pre pyclipper
```

We use basemap for map plotting. This was a fun install

```
sudo apt-get install libgeos-dev
cd /usr/lib
sudo ln -s libgeos-3.4.2.so libgeos.so
sudo ldconfig
sudo pip install basemap --allow-external basemap --allow-unverified basemap
```

You can check `libgeos` is dandy with `ld -lgeos --verbose`

Yes you need a fortran compiler!!!

## Python Usage

`ipython notebook`
