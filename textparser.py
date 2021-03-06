"""This file contains code for use with "Python for Software Design",
available from Cambridge University Press, and "Think Python,"
available from greenteapress.com

Copyright 2010 Allen B. Downey
License: GNU GPLv3 http://www.gnu.org/licenses/gpl.html

Modified by: Ben Kahle and Evan Dorsky
"""

import os
import re
import redis

def open_gunzip(filename):
    """open the filename, using a pipe and gunzip to read a
    compressed file without uncompressing the whole thing."""
    cmd = 'gunzip -c ' + filename
    fp = os.popen(cmd)
    return fp

def process_file(filename, f):
    """read the given filename and extract information;
    for each film, call f() with string arguments:
    actor, date, title, role """
    fp = open_gunzip(filename)
    i = 0

    # skip over the header until you get to the following magic line
    magic = re.compile('----\s+?------')
    for line in fp:
        if magic.match(line.strip()):
            break

    # regexp to recognize actor, tabs, movie
    split1 = re.compile('([^\t]*)\t*(.*)', re.UNICODE)

    # regexp to recognize title, date, role
    split2 = re.compile('([^\(]*)\s*(\([^\)]*\))[^\[]*(\[[^\]]*\])?', 
                        re.UNICODE)

    # regexp to recognize television (TV), video (V), video game (VG)
    video = re.compile('\(T?V|VG\)', re.U)

    actor = ''
    for line in fp:
        line = line.rstrip()
        if line == '': continue
        if line[0] == '-----': break

        # split the line into actor info and movie info;
        # keep track of the current actor
        ro = split1.match(line)
        if ro:
            new_actor, info = ro.groups()
            if not info:
                stat = fp.close()
                return
            if new_actor:
                actor = new_actor
        else:
            print 'BAD1', line
            continue

        # skip television shows (titles in quotation marks)
        if info[0] == '"':
            continue

        # skip made for TV and straight to video
        if video.search(info):
            continue

        # split the info into title, date and role
        ro = split2.match(info)
        if ro:
            title, date, role = ro.groups()
            if date == None:
                print 'BAD2', line
                continue

            f(actor, date, title.rstrip(), role)
        else:
            print 'BAD3', line
            continue

    stat = fp.close()


if __name__ == '__main__':
    r0 = redis.StrictRedis(host="localhost",port=6379, db=0)
    r1 = redis.StrictRedis(host="localhost",port=6379, db=1)

    def print_info(actor, date, title, role):
        print actor, date, title, role

    def by_movie(actor, date, title, role):
        # if title in title_dict.keys():
        #     title_dict[title].add(actor)
        # else:
        #     title_dict[title] = set(actor)
        r0.sadd(title, actor)

    process_file('actors.list.gz', by_movie)
    print "Done with actors, doing actresses now"
    process_file('actresses.list.gz', by_movie)
    print "Done with actresses"

    print "Starting redis import"
    for movie in r0.keys():
        for actor in r0.smembers(movie):
            for conn_actor in r0.smembers(movie):
                if actor != conn_actor:
                    r1.sadd(actor, conn_actor)
    print "Done!"