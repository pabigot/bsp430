# This file is in the public domain

# Really stupid program that prints out the current time once per
# second, synchronized to the computer it's running on.  If your NTP
# configuration is correct this should be slightly ahead of the GPS
# program (which has to go through a couple low-speed serial links
# before the data hits the screen).

import datetime

last_us = 1000000
while True:
    now = datetime.datetime.now()
    # Print when we've observed a wrap-around in microseconds
    if now.microsecond < last_us:
        print now
    last_us = now.microsecond
        
    
