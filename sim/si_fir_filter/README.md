## Design of the Si TX Spectral Shaping Filter

The SI contains a 17-tap digital FIR filter for use in GFSK modes.

We use this in aprs mode to filter the gpio squarewave into a sensible
afsk signal.

There's some notes in a ipython notebook `si_fir_filter.ipynp` on
this. This notebook is also used to calculate the deviation values.

## Prerequisites

```
sudo apt-get install ipython libblas-dev liblapack-dev gfortran
pip install scipy matplotlib
```

Yes you need a fortran compiler!!!

## Python Usage

`ipython notebook`

## Calcuating new coefficients

There's an awesome calculator here
[here](http://t-filter.appspot.com/fir/index.html)

## So

The python script is from
[here](http://nbviewer.ipython.org/github/unpingco/Python-for-Signal-Processing/blob/master/Filtering.ipynb)
