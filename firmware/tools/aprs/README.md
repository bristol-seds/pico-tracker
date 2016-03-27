# APRS Tools

This directory contains scripts for processing backlog data.
Run `./get_python_deps.sh` to grab dependancies.

* `aprs_daemon.py` connects to APRS-IS and uploads backlog at it arrives.
* `raw_parser.py` uploads backlog from a text file.
* `equations.py` uploads equations for the telemetry values to APRS-IS

# Uploading #

This python script uploads APRSISCE telemetry equation packets for the
balloon to APRS-IS with a verified account.

For more information on APRS uploading and examples see

http://www.jmalsbury.com/techwiki/index.php?title=Uploading_ARPS_Frames_to_APRS-IS_-_Part_I
-and-
http://www.jmalsbury.com/techwiki/index.php?title=Uploading_ARPS_Frames_to_APRS-IS_-_Part_II
