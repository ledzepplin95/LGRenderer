#!/usr/bin/python
# -*- coding: Latin-1 -*-

###############################################################################
#
#   cdServer - HTTP server designed for serving files off CD-ROMs,
#              with simple API for dynamic content generation
#
#   Copyright (C) 2004 - 2006 by Dr. Eckhard Licher, Frankfurt/Main, Germany.
#
#   This program is Free Software; see section COPYRIGHT below
#
###############################################################################


"""
CDSERVER(1)
=============
Dr. Eckhard Licher <ELicher@online.de>
v0.8, Jan15 2006


NAME
----
cdServer - HTTP server designed for serving files off CD-ROMs
with simple API for dynamic content generation


SYNOPSIS
--------
'cdServer.py' [-p number] [-r rootdir] [-s] [-t] [-a host] [-n net]
[-v] [-h] [-m] [-d] [--port=number] [--root=rootdir] [--server-only]
[--threaded-server] [--add-host=host] [--add-network=net] [--verbose]
[--help] [--manual] [--debug]


DESCRIPTION
-----------
'cdServer' is a simple HTTP server designed to serve HTML files off a CD-ROM.
'cdServer' also provides a simple interface for special functions implemented
in external application programs, e.g. search functions.  The current server
implementation only handles GET and HEAD requests.  POST requests are not
implemented.


OPTIONS
-------
'-p number', '--port=number'::
        sets the server's portnumber to number

'-r rootdir', '--root=rootdir'::
        sets the serverroot to directory rootdir

'-s', '--server-only'::
        server-only mode, no browser launched

'-t', '--threaded-server'::
        start separate thread for each request

'-v', '--verbose'::
        display additional messages at startup

'-a, --add-host'::
        add host to list of allowed hosts (ip address or host name)

'-n, --add-network'::
        add network to list of allowed networks;
        network is specified with CIDR notation, e.g. 192.168.0.0/24

'-h', '--help'::
        display help message

'-m', '--manual'::
        display this manual page

'-d', '--debug'::
        switch on debug mode

Options specified at the command line are evaluated from left to right.

When invoked without options, 'cdServer' serves HTTP requests through
port 8000. In case port 8000 is already in use the server searches for
an unused port by subsequently incrementing the port number and trying
to bind to the next port unless option '-p' is specified. If option '-p'
is specified the port auto-search function is turned off.

The serverroot is set to the parent directory of the 'cdServer'
executable unless the serverroot is explicitly specified by means of
option '-r'.  The default serverroot allows the start of the server off
the distribution CD-ROM without prior installation or customization.

Subsequently the default web browser is launched unless option '-s' is
given at the command line.

The verbosity of the server during startup can be increased slightly
with option '-v'.

If option '-t' is given the server starts a separate thread for each
request in order to enable the server to serve a large number of
possibly long-running parallel requests without blocking other clients.

For security reasons only requests originating from the local machine
are being served by default. Other hosts need to be included explicitly
in the access control list by means of option '-a'. Option '-a' may be
used more than once in order to allow access from several hosts.
Additionally, entire networks may be specified using option '-n';
network addresses shall be specified using CIDR format
(e.g. 192.168.0.0/24).

Incorrect invocation of the server as well as option '-h' display a
brief help message.  The full manual page is printed through option
'-m'.  In either case program execution terminates regardless of other
options possibly specified.

Upon startup 'cdServer' tries to import an application (a python module
named cdApp.py). If the import succeeds the application is supposed to
provide a request handler which is used instead of the built-in
SimpleHTTPRequesthandler from the Python standard library. The
application's request handler is in charge of intercepting special URLs
and providing additional functionality.

Option '-d' switches on debug mode.  cdApp is imported without catching
errors during import and variable cdApp.debug is set to True.


RUNTIME PROBLEMS
----------------
'cdServer' tries to do its best when being run in an unfriendly
environment such as Windows or on mis-configured machines and handles
common problems gracefully.

In case the server can not listen on the requested port the browser
is launched using the filesystem and 'cdServer' terminates itself.  On
a sensibly configured machine this should only happen if 'cdServer' is
being run by a regular user and the requested port is below 1024.

The server is started in a separate thread so that a local firewall
which may block intra-machine IP traffic can be detected.  After
successful startup of the server a test request is fired (from the main
thread) at the server.  If the request fails a firewall is blocking the
internal request.  In this case the browser is launched using the
filesystem and 'cdServer' terminates itself.  This is a common problem
on Windows machines where an unnecessary restrictive firewall is
installed.


AVAILABILITY
------------
'cdServer' is available on all platforms that support Python 2.2 or
later.


AUTHOR
------
Dr. Eckhard Licher, Frankfurt am Main, Germany

'cdServer' is available for download at
http://www.licher-ffm.de/cdServer.html


ACKNOWLEDGEMENTS
----------------
The code for access control lists for entire networks (option -n,
introduced in version 0.8) was contributed by Forest Bond
<forest@alittletooquiet.net> to moosicWebGUI and copied to 'cdServer'
with his permission.


FILES
-----
cdApp.py


BUGS
----
Please send bug reports to <ELicher@online.de>


COPYRIGHT
---------
Copyright (C) 2004-2006 by Dr. Eckhard Licher, Frankfurt/Main, Germany.

This program is Free Software; you can redistribute it and/or modify
it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Library General Public License (contained in file bin/COPYING.LIB)
for more details.

You should have received a copy of the GNU Library General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307,
USA.
"""

###############################################################################
# NOTE
###############################################################################
# the text contained in the preceding docstring contains the program's man
# page in asciidoc format. The asciidoc source can be converted to HTML
# and/or docbook-sgml (for docbook2man post-processing).
#
# Extract the docstring with the following command and post-process with
# asciidoc (and possibly docbook2man) as desired:
# $ python cdServer.py -m > cdServer.asc
###############################################################################


###############################################################################
# Revision History
###############################################################################
#
# Version 0.4 (2004-05-02) -- E.L.
# - intial version created, based on dhbmServer 1.4.1 by deleting all
#   application specific code.
#
# Version 0.4.1 (2004-05-29) -- E.L.
# - some minor bugfixes
#
# Version 0.5 (2004-07-10) -- E.L.
# - auto search function for a free port included
#
# Version 0.6 (2004-07-31) -- E.L.
# - license changed to LGPL
# - new option -t added: main request handling loop starts a separate thread
#   for each request in order to enable the server to serve a large number of
#   possibly long-running parallel requests without blocking other clients.
#
# Version 0.7 (2005-04-02) -- E.L.
# - prepared for publication
# - end_flag (set in cdApp) signals program termination
#
# Version 0.8 (2006-01-15) -- E.L.
# - added support for ACLs (code contributed to moosicWebGUI by Forest Bond)
# - added debug mode
#
###############################################################################


###############################################################################
# Import needed python modules
###############################################################################

import getopt, os, string, sys, time, httplib, threading, re
import socket, SocketServer, BaseHTTPServer, SimpleHTTPServer, webbrowser


###############################################################################
# Separate thread for request handler
###############################################################################

class handler_thread(threading.Thread):
    """
    Run request handler in separate thread so that any cdApp-request_handler
    may use pretty much time to complete without blocking other connections.
    In connetion with a request_queue_size of 48 the server should be able
    to handle a lot of connections simultaneously (if your CPU is fast enough)
    """

    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        global httpd
        httpd.handle_request()


###############################################################################
# Separate thread for cdServer
###############################################################################

class server_thread(threading.Thread):
    """
    Run cdServer in separate thread. This is done so that
    an exisiting firewall (which might block local traffic)
    can be detected.
    """
    def __init__(self):
        threading.Thread.__init__(self)

    def run(self):
        """start server and handle requests"""
        global server_flag, thread_flag, port, httpd

        t_print("\n*** Starting server ...")
        # scan all ports above initial port number
        for i in range(65536 - port):
            if verbose:
                t_print("Trying to connect to port %d ..." % port)
            try:
                httpd = CDHTTPServer(('', port), request_handler)
            except socket.error, exc:
                # error code 98: Address already in use
                if exc.args[0] == 98 and auto_inc:
                    port += 1
                else:
                    if verbose:
                        t_print("Can not connect to port %s -- %s." % (port, exc.args[1]))
                    server_flag = True
                    return
            else:
                if verbose:
                    t_print("Server ready on port %d." % port)
                server_flag = False
                break

        # serve forever...
        end_flag = False
        while not end_flag:
            # start handler in separate thread if option -t was given else just start handler
            try:
                if thread_flag:
                    h_thread = handler_thread()
                    h_thread.start()
                    h_thread.join()
                else:
                    httpd.handle_request()
            except:
                t_print('Caught a weird error.')
                httpd.server_close()
                t_print('Trying to start a new server instance ...')
                try:
                    httpd = CDHTTPServer(('', port), request_handler)
                except:
                    t_print('Can not start new server, giving up. Sorry.')
                    return
            # check if cdApp likes to termiante
            try:
                end_flag = cdApp.end_flag
            except:
                pass

        # we only come here if cdApp signals termination via cdApp.end_flag
        t_print('\n*** cdServer terminating ...\n')


###############################################################################
# CDHTTPServer class -- a BaseHTTPServer with a larger request_queue_size
###############################################################################

class CDHTTPServer(BaseHTTPServer.HTTPServer):
    """BaseHTTPServer with a larger request_queue_size"""
    request_queue_size = 48

    def server_bind(self):
        SocketServer.TCPServer.server_bind(self)
        host, port = self.socket.getsockname()
        self.server_name = "localhost"
        self.server_port = port

    #def handle_error(self, request, client_address):
    #    pass


###############################################################################
# SimpleHTTPRequestHandlerWithACL class --
# a SimpleHTTPServer.SimpleHTTPRequestHandler with access control
###############################################################################

class SimpleHTTPRequestHandlerWithACL(SimpleHTTPServer.SimpleHTTPRequestHandler):
    """Complete HTTP server with access control"""

    def do_HEAD(self):
        """check acl and forward request to SimpleHTTPServer if authorized"""
        if not checkACL(self.client_address[0]):
            self.send_response(403)
        else:
            SimpleHTTPServer.SimpleHTTPRequestHandler.do_HEAD(self)

    def do_GET(self):
        """check acl and forward request to SimpleHTTPServer if authorized"""
        if not checkACL(self.client_address[0]):
            self.send_response(403)
            self.wfile.write('Content-type: text/html\n\n')
            self.wfile.write('''<html><body>
                                <h1>Not authorized</h1>
                                <p>You are not authorized to connect to this server.</p>
                                </body></html>''')
        else:
            SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)


###############################################################################
# some error classes
###############################################################################

class Error(Exception):
    pass

class SubnetError(Error):
    def __init__(self, subnet):
        self.subnet = subnet
    def __str__(self):
        return "Invalid subnet: " + str(self.subnet)

class SubnetIPError(Error):
    def __init__(self, ip):
        self.ip = ip
    def __str__(self):
        return "Invalid IP: " + str(self.ip)


###############################################################################
# subnet class -- handling of network IP addresses
###############################################################################

class subnet:
    def __init__(self, subnet_cidr):
        if not re.compile(r'^[0-9]{1,3}(\.[0-9]{1,3}){0,3}/[0-3]*[0-9]?$').match(subnet_cidr):
            raise SubnetError, subnet_cidr
        [cidr_subnet_ip, cidr_mask] = subnet_cidr.split('/')

        cidr_mask = int(cidr_mask)
        if cidr_mask < 0 or cidr_mask > 32:
            raise SubnetError, subnet_cidr
        subnet_mask_int = 0
        for bit_num in range(32 - cidr_mask, 33):
            subnet_mask_int += 2**bit_num

        cidr_subnet_ip_nums = cidr_subnet_ip.split('.')
        cidr_subnet_int = 0
        for number in cidr_subnet_ip_nums:
            cidr_subnet_int = cidr_subnet_int * 256 + int(number)
        for index in range(0, 4 - len(cidr_subnet_ip_nums)):
            cidr_subnet_int = cidr_subnet_int * 256

        self.subnet_int = cidr_subnet_int
        self.subnet_ip = cidr_subnet_ip
        self.subnet_mask_int = subnet_mask_int
        self.address_mask_int = 4294967295 ^ subnet_mask_int
        self.cidr_mask = cidr_mask

        if(self.subnet_int & self.subnet_mask_int != self.subnet_int):
            raise SubnetError, subnet_cidr
    def get_cidr(self):
        return self.subnet_ip, self.cidr_mask
    def get_int(self):
        return self.subnet_int, self.subnet_mask_int
    def ip_is_member(self, ip_address):
        if not re.compile(r'^[0-9]{1,3}(\.[0-9]{1,3}){3,3}$').match(ip_address):
            raise SubnetIPError, ip_address
        ip_address = ip_address.split('.')
        ip_address_int = 0
        for number in ip_address:
            ip_address_int = ip_address_int * 256 + int(number)
        ip_subnet_int = ip_address_int & self.subnet_mask_int
        if ip_subnet_int != self.subnet_int:
            return False
        return True
    def __str__(self):
        return self.subnet_ip + '/' + str(self.cidr_mask)
    def __repr__(self):
        return self.__str__()


###############################################################################
# Subroutines
###############################################################################

def usage(file=sys.stdout):
    """print help message"""
    print >> file, '\nusage: %s.py [-p number] [-r rootdir] [-s] [-t] [-a host] [-n net]' % progname
    print >> file, '                   [-v] [-h] [-m] [-d] [--port=number] [--root=rootdir]'
    print >> file, '                   [--server-only] [--threaded-server] [--add-host=host]'
    print >> file, '                   [--add-network=net] [--verbose ] [--help] [--manual]'
    print >> file, '                   [--debug]'
    print >> file, '\noptions:'
    print >> file, '     -p number, --port=number     sets the server\'s portnumber to number'
    print >> file, '     -r rootdir, --root=rootdir   sets the serverroot to directory rootdir'
    print >> file, '     -s, --server-only            server-only mode, no browser launched'
    print >> file, '     -t, --threaded-server        start separate thread for each request'
    print >> file, '     -a host, --add-host=host     add host to list of allowed hosts'
    print >> file, '                                  (ip address or host name)'
    print >> file, '     -n net, --add-network=net    add network to list of allowed networks'
    print >> file, '                                  (use CIDR notation, e.g.192.168.0.2/24)'
    print >> file, '     -v, --verbose                display additional messages at startup'
    print >> file, '     -h, --help                   display help message'
    print >> file, '     -m, --manual                 display manual page'
    print >> file, '     -d, --debug                  switch on debug mode\n'


def t_print(s, nonl=False):
    """print to standard output (thread safe)"""
    mutex.acquire()
    if nonl: print s,
    else: print s
    mutex.release()


def checkACL(ip_address):
    ''' determine if a client is allowed to connect to webserver '''
    return True
    if ip_address in allowed_hosts:
        return True
    for subnet in allowed_networks:
        if subnet.ip_is_member(ip_address):
            return True
    return False


###############################################################################
# MAIN PROGRAM
###############################################################################

########################################
# we start here defining some variables
# and set system behaviour / defaults
########################################

# program version and copyright stuff
progname    = 'cdServer'
pversion    = '0.8 (2006-01-15)'
version     = '%s version %s' % (progname, pversion)

port              = 8000      # default server port
openbrowser       = False      # open browser after server start
auto_inc          = True      # search for free port to connect to
thread_flag       = False     # threading is disabled by default
verbose           = False     # don't be verbose
debug             = False     # no debug in default mode
allowed_hosts = ['127.0.0.1'] # allow only these hosts ...
allowed_networks  = []        # ... or these networks to connect to server

notice      = \
"""%s is Free Software.  You are welcome to redistribute it
under certain conditions; see file COPYING.LIB for details. """ % progname

# default serverroot
serverroot  = os.path.dirname(os.path.abspath(sys.argv[0]))
serverroot, bin = os.path.split(serverroot)


########################################
# scan command line options and print
# welcome message
########################################

try:
    optlist, args = getopt.getopt(sys.argv[1:], 'a:n:p:r:sthvmd',
        ["add-host=", "add-network=", "port=", "root=", "help",
         "server-only", "threaded-server", "verbose", "manual", "debug"])
except:
    usage(sys.stderr)
    print >>sys.stderr,notice + "\n"
    sys.exit(2)
for i, j in optlist:
    if i in ('-p', "--port"):
        port = string.atoi(j)
        # if port is explicitly specified do not search for unused port
        auto_inc = False
    elif i in ('-r', "--root"):
        serverroot = os.path.abspath(j)
    elif i in ('-v', "--verbose"):
        verbose = True
    elif i in ('-s', "--server-only"):
        openbrowser = False
    elif i in ('-t', "--threaded-server"):
        thread_flag = True
    elif i in ('-a', "--add-host"):
        try:
            h = socket.gethostbyname(j)
        except socket.gaierror:
            print >> sys.stderr, "\n*** DNS lookup for host '%s' failed.\n" % j
            sys.exit(1)
        else:
            allowed_hosts.append(h)
    elif i in ('-n', "--add-network"):
        try:
            sn = subnet(j)
        except SubnetError:
            print >> sys.stderr, "\n*** '%s' is not a valid network specification.\n" % j
            sys.exit(1)
        else:
            allowed_networks.append(sn)
    elif i in ('-h', "--help"):
        usage()
        print notice + "\n"
        sys.exit(0)
    elif i in ('-m', "--manual"):
        print __doc__
        sys.exit(0)
    elif i in ('-d', "--debug"):
        debug = True
if args:
    usage(sys.stderr)
    print >>sys.stderr,notice + "\n"
    sys.exit(2)

# print welcome message
print '-' * 79
print """This is %s

(C) 2004-2006 by Dr. Eckhard Licher, Frankfurt/Main, Germany.
%s comes with ABSOLUTELY NO WARRANTY; for details use option "-m".
""" % (version, progname)
print notice
print '-' * 79 + '\n'


########################################
# Set new serverroot / cwd() and try to
# initialize application
########################################

try:
    os.chdir(serverroot)
except:
    print '\n%s: could not set serverroot to "%s".\n' % (progname, serverroot)
    sys.exit(1)
if verbose:
    print 'Serverroot set to "%s" ...' % serverroot

print '\n*** Trying to initialize CD application ...'

# import application (if any) so it can initialize itself
# and set the app's request handler as new request handler
# NOTE: the name of the RH shall be SimpleHTTPRequestHandler
if debug:
    import cdApp
    request_handler = cdApp.SimpleHTTPRequestHandler
    cdApp.checkACL = checkACL
    cdApp.debug = True
else:
    try:
        import cdApp
        request_handler = cdApp.SimpleHTTPRequestHandler
        cdApp.checkACL = checkACL
        cdApp.debug = False
        print "\n*** %s successfully initialized." % cdApp.version
    except ImportError:
        print '\n*** could not initialize application, using SimpleHTTPServer instead.'
        #request_handler = SimpleHTTPServer.SimpleHTTPRequestHandler
        request_handler = SimpleHTTPRequestHandlerWithACL


########################################
# start server in a separate thread
########################################

if verbose:
    print '\n*** %s is starting up ...\n' % progname
server_flag = None
mutex = threading.Lock()

# start server in separate thread
end_flag = False
s_thread = server_thread()
s_thread.start()

# wait for server to start up before we fire a test request at it...
while server_flag is None:
    # server_flag set to True or False during its startup
    time.sleep(0.05)


########################################
# set URL according to the server
# and firewall status (to be tested)
# - browse filesystem if server could
#   not be started or a firewall is
#   blocking intra-machine traffic
# - serve through cdServer otherwise
########################################

# set start URL / fallback URL (read from filesystem)
url  = "http://localhost:%s/" % port
url2 = os.path.join(serverroot, "index.html")

fwabort = False
if server_flag:
    # server could not be launched
    mutex.acquire()
    print "\n" + "=" * 79
    print """The integrated webserver could not be started.

Therefore %s has to resort to the static HTML files on the CD.
Special functions will not be available. Sorry for the inconvenience.""" % (progname)
    print "=" * 79
    mutex.release()
    url = url2

else:
    # open connection to cdServer which is running in separate thread.
    if verbose:
        t_print("Checking if local firewall is active ...")
    conn = httplib.HTTPConnection("localhost:%s" % port)
    try:
        # request the /index.html document
        conn.request("GET", "/")
        resp = conn.getresponse()
        data = resp.read()
        conn.close()
        if verbose:
            t_print("Firewall OK, traffic to port %s is not blocked." % port)
            t_print("Hosts allowed to connect: %s" % allowed_hosts)
            t_print("Networks allowed to connect: %s" % allowed_networks)
    except:
        mutex.acquire()
        print "\n" + "=" * 79
        print """Firewall test failed.

This machine seems to be running a local firewall which is configured to block
IP connections to local servers even when client requests originate from the
same machine.

Therefore %s  has to resort to the static HTML files on the CD.
Special functions will not be available. Sorry for the inconvenience.""" % (progname)
        print "=" * 79
        mutex.release()
        url = url2
        fwabort = True


########################################
# open URL in webbrowser
########################################

if openbrowser:
    t_print('\n*** Please wait, the web browser is being launched ...\n')
    try:
        webbrowser.open_new(url)
    except:
        t_print('\n*** Can not launch web browser, please open URL "%s" manually.\n' % url)
        openbrowser = False
else:
    t_print('\n*** Please open URL "%s" with your browser.\n' % url)

# abort program if firewall is blocking or server doesn't run --
# we browse the filesystem instead.
if server_flag or fwabort:
    # give the user a chance to see the messages before the program
    # terminates (and its window disappears on a Windows machine)
    if not openbrowser:
        try:
            t_print("")
        except:
            pass
        t_print('*** Hit <enter> key to terminate program ...')
        line = raw_input()
    os._exit(1)


########################################
# wait for server to shut down
########################################

try:
    s_thread.join()
except:
    pass
sys.exit(0)


###############################################################################
# END
###############################################################################