# This file is part of Frommle
# Frommle is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.

# Frommle is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.

# You should have received a copy of the GNU Lesser General Public
# License along with Frommle; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

# Author Roelof Rietbroek (roelof@geod.uni-bonn.de), 2019

import re
import gzip as gz
from frommle.io.shstandard import read_shstandard
from frommle.io.icgem import read_icgem
from frommle.io.GSM import readGSMv06

class formats:
    standard=1
    icgem=2
    GSMv6=3

def queryformat(filename):
    gzip=False

    ftest=filename
    
    if filename.endswith(".gz"):
        gzip=True
        ftest=filename.strip(".gz")

    if ftest.endswith("gfc"):
        return formats.icgem,gzip
    if gzip:
        fid=gz.open(filename,'rt')
    else:
        fid=open(filename,'rt')

    ln = fid.readline()
    fid.close()

    if re.search("META",ln):
        return formats.standard, gzip
    else:
        #fall back by means of exclusion
        return formats.GSMv6,gzip


def sh_read(filename,nmax=None,format=None,epoch=None,headerOnly=False,shg=None,error=False):

    if not format:
        format,gz=queryformat(filename)


    if format == formats.standard:
        return read_shstandard(filename,nmax=nmax,headerOnly=headerOnly,shg=shg,error=error)

    if format == formats.icgem:
        return read_icgem(filename,nmax=nmax,epoch=epoch,headerOnly=headerOnly,error=error)

    if format == formats.GSMv6:
        return readGSMv06(filename,nmax=nmax,headerOnly=headerOnly,error=error)
