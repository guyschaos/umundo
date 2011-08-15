#! /usr/bin/env python
# encoding: utf-8
#
# file:	wscript
# desc:	zMundo waf build file
# auth: fheinrichs
# 
# To build zmundo using this build file use a make like process with waf:
#
#  * first configure the project: waf configure [options]
#  * then build the project (repeatedly): waf [build]
#  * clean it if needed: waf clean build
#  * you can wrap up everything needed for shipping: waf dist
#  * you can clean the configuration: waf distclean
#  * finally you can install it: waf install
#
# See the waf documentation for a full list of options with waf.
#
# EXAMPLE: 
#  
#  * waf configure build -- configure and build zmundo 
#
# Note that this is only the top level build script used for global
# operations and options (e.g. packaging etc.). Check the individual
# build scripts for java and cpp to see the special configurations for
# these builds.

import os, sys

# SETTINGS

# global project settings
APPNAME = 'zmundo'
VERSION = '0.0.1-prealpha'

zm_platform = sys.platform

# zm directory settings
zm_basedir = os.getcwd()
zm_dir = {
	'BASE' : zm_basedir,
	'TLS'  : zm_basedir + '/tls',
	'LIB'  : zm_basedir + '/lib',
	'SRC'  : zm_basedir + '/src',
	'BIN'  : zm_basedir + '/bin',
	'CNT'  : zm_basedir + '/cnt'
	}

# zm dist settings
zm_archive = 'zip'	# options are: 'zip', 'tar.gz', 'tar.bz2'
zm_ignore = '**/*~ **/*.swp **/.waf* **/.lock-w* **/*.pyc **/c4*'

# zm target
zm_default_target = 'java'  # NOTE change to java,cpp in the end

# zm suffixes
zm_suffix = {
	'JAVA'  : '/java',  # suffix for java sources
	'CPP'   : '/cpp',   # suffix for cpp sources
	'PROTO' : '/proto', # suffix for protocol buffer
	'GEN'   : '/gen',   # suffix for generated codezm_proto  = '/proto'	# suffix for protocol buffer sources
	'TEST'  : '/test/'  # suffix for unit tests
	}

# waf settings
top = zm_dir['BASE']
out = zm_dir['BIN']


# BUILD PROCESS

def options(opt):
    
    opt.add_option(
	    '--target', 
	    action='store', 
	    default=zm_default_target,
	    help='Define the target (java, cpp)')

    opt.recurse(zm_dir['SRC'] + zm_suffix['JAVA'])
    opt.recurse(zm_dir['SRC'] + zm_suffix['CPP'])


def configure(cnf):

    print('\nCONFIGURING ' 
	    + APPNAME + ' (for ' + zm_platform + ')') 

    cnf.env.ZM_PLATFORM = zm_platform
    cnf.env.ZM_DIR = zm_dir
    cnf.env.ZM_SUFFIX = zm_suffix

    cnf.env.TARGET = cnf.options.target.split(',')

    if 'java' in cnf.env.TARGET:
        cnf.recurse(zm_dir['SRC'] + zm_suffix['JAVA'])

    if 'cpp' in cnf.env.TARGET:
    	cnf.recurse(zm_dir['SRC'] + zm_suffix['CPP'])


def build(bld):
    if 'java' in bld.env.TARGET:
        bld.recurse(zm_dir['SRC'] + zm_suffix['JAVA'])

    if 'cpp' in bld.env.TARGET:
    	bld.recurse(zm_dir['SRC'] + zm_suffix['CPP'])


def dist(dst):

    print('\nDISTRIBUTING ' + APPNAME + ' (for ' + zm_platform + ')\n')
    dst.algo = zm_archive
    dst.excl = zm_ignore
