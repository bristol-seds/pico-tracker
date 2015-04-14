#!/usr/bin/python
import math
import csv
import time
from datetime import datetime

####################################################################################################

# Assume the earth is a nice smooth sphere
earth_radius = 6400000

def angle_between(vec1, vec2):
    dot_product = dot_product_spherical(vec1, vec2)

    vec1mag = math.sqrt(dot_product_spherical(vec1, vec1))
    vec2mag = math.sqrt(dot_product_spherical(vec2, vec2))

    arg = round(dot_product / (vec1mag * vec2mag), 13)
    alpha = math.acos(arg)
    return alpha

def dot_product_spherical(vec1, vec2):
    abcosalpha = ((vec1[2]*vec2[2]*math.cos(vec1[0])*math.cos(vec2[0])*math.cos(vec1[1])*math.cos(vec2[1]))
            + (vec1[2]*vec2[2]*math.cos(vec1[0])*math.cos(vec2[0])*math.sin(vec1[1])*math.sin(vec2[1]))
            + (vec1[2]*vec2[2]*math.sin(vec1[0])*math.sin(vec2[0])))

    return abcosalpha

# Finds the ground distance between two spherical vectors
# Works well only for vectors with a small angular difference
def ground_distance_covered(vec1, vec2):
    distance = math.sqrt(2) * vec1[2] * math.sqrt(1 - math.cos(angle_between(vec1, vec2)))

    return distance

#####################################################################################################

file_path = "balloon_coords.csv"

data = []

with open(file_path, 'rb') as csvfile:
    file_reader = csv.reader(csvfile)
    for row in file_reader:
        launch_date = '2014-06-15 '
        thisDateTime = datetime.strptime(launch_date + row[0], '%Y-%m-%d %H:%M:%S')
        formattedRow = []
        formattedRow.append(time.mktime(thisDateTime.timetuple()))# Time
        formattedRow.append(math.radians(float(row[1])))# Latitude
        formattedRow.append(math.radians(float(row[2])))# Longitude
        formattedRow.append(float(row[3]))# Altitude
        data.append(formattedRow)


final_data = []

out_file = open('speed.csv', 'w')
file_writer = csv.writer(out_file)

for i in range(0, len(data)-1):
    thisTime = data[i][0]
    nextTime = data[i+1][0]
    timeDiff = nextTime - thisTime

    start = [data[i][1],   data[i][2],   data[i][3]   + earth_radius]
    end   = [data[i+1][1], data[i+1][2], data[i+1][3] + earth_radius]
    dist = ground_distance_covered(start, end)
    ground_speed = dist / timeDiff

    alt_speed = (data[i+1][3] - data[i][3]) / timeDiff
    final_data.append([thisTime, ground_speed, alt_speed])

for i in range(0, len(final_data)):
  print final_data[i][0], final_data[i][1]


with open('speed.csv', 'w') as file_pointer:
    file_writer = csv.writer(file_pointer, delimiter=',')
    file_writer.writerows(final_data)


