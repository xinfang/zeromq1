#!/bin/sh
# Script to generate all required files from fresh svn checkout.

autoreconf -fvi -I config

echo
echo "Run './configure ; make'"
echo
