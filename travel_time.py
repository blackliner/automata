import googlemaps
import numpy
import sys
import statistics
from pprint import pprint
from datetime import datetime, timezone


def nice_time(seconds):
    seconds = seconds % (24 * 3600)
    hour = seconds // 3600
    seconds %= 3600
    minutes = seconds // 60
    seconds %= 60

    return "%d:%02d:%02d" % (hour, minutes, seconds)


gmaps_client = googlemaps.Client(key="GOOGLE_API_KEY from env")

now = datetime.now()

checktime = datetime(2020, 9, 30, 8, 0, 0)
print(checktime)


def travel_time(source, dest):

    ret_val = []

    for s in source:
        # Request directions via public transit
        directions_result = gmaps_client.directions(
            origin=s, destination=dest, mode="transit", departure_time=checktime
        )

        if not directions_result:
            directions_result = gmaps_client.directions(origin=s, destination=dest, departure_time=checktime)
            if not directions_result:
                print(f"Error finding a result for {s}!")
                return 0

        changes = len(directions_result[0]["legs"][0]["steps"])

        if changes > 20:
            print(f"Too many changes for {s}!")
            return 0

        ret_val.append(directions_result[0]["legs"][0]["duration"]["value"])

    return ret_val


top_left = (48.297145, 11.537026)
bot_right = (48.144135, 11.696473)
n_long = 5
n_lat = 5

lng_range = numpy.linspace(bot_right[0], top_left[0], n_long)
# print(lng_range)
lat_range = numpy.linspace(top_left[1], bot_right[1], n_lat)
# print(lat_range)

employee_addresses = ["Munich"]


string_locations = ["Munich"]


def lon_lat(loc):
    tempval = gmaps_client.geocode(loc)[0]

    return (tempval["geometry"]["location"]["lat"], tempval["geometry"]["location"]["lng"])


locations = [lon_lat(loc) for loc in string_locations]


travel = [travel_time(source=employee_addresses, dest=loc) for loc in locations]

travel_min = [min(x) for x in travel]
travel_max = [max(x) for x in travel]
travel_mean = [statistics.mean(x) for x in travel]
travel_stddev = [statistics.stdev(x) for x in travel]

print(f"Time to get to place X for all ({len(employee_addresses)} people) employees:")

for x in range(len(string_locations)):
    print(f"To {string_locations[x]}:")
    print(f"    Min:    {nice_time(travel_min[x])}")
    print(f"    Max:    {nice_time(travel_max[x])}")
    print(f"    Mean:   {nice_time(travel_mean[x])}")
    print(f"    Stddev: {nice_time(travel_stddev[x])}")
    print()
