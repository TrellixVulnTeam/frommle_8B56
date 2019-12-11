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

# Author Roelof Rietbroek (roelof@geod.uni-bonn.de), 2018

import numpy as np
from frommle.sh import SHnmtGuide,trig
from frommle.io.shArchive import SHArchive,SHGVar
from frommle.io.numpyVariable import np_float64Var

# from frommle.sh.shxarray import newshxarray
# import operator
from frommle.core.time import decyear2datetime,datetime2decyear

class SHStandardArchive(SHArchive):
    def __init__(self,filen,mode='r',nmax=None):
        super().__init__(filen,mode,nmax)

    def fload_impl(self):
        """Loads the data into variables"""

        witherrors=False
        with self.fid() as fid:
            ln = fid.readline()
            lnspl = ln.split()

            if len(lnspl) == 6:
                witherrors=True

            self.attr["nmaxfile"]=int(lnspl[1])
            if not "nmax" in self.attr:
                self.attr["nmax"]=self.attr["nmaxfile"]
            tags=["tstart","tcent","tend"]
            for ky,val in zip(tags,lnspl[2:5]):
                decyr=float(val)
                if decyr > 0:
                    self.attr[ky]=decyear2datetime(decyr)

            nmax=self.attr["nmax"]
            shg=self.shg_c(nmax)
            sz=shg.size()

            cnm=np.zeros([sz])
            sigcnm=np.zeros([sz])

            for ln in fid:
                lnspl = ln.split()

                n = int(lnspl[0])
                if n > nmax:
                    continue
    
                m = int(lnspl[1])

                idxc=shg.idx((n, m,trig.c))
                idxs=shg.idx((n,m,trig.s))
    
                # import pdb;pdb.set_trace()
                cnm[idxc]=float(lnspl[2])
                if m!=0:
                    cnm[idxs]=float(lnspl[3])


                if witherrors:
                    sigcnm[idxc]=float(lnspl[2])
                    if m!=0:
                        sigcnm[idxs]=float(lnspl[3])

            #set variables and assign ndarray's to them

            self["cnm"]=np_float64Var(cnm)
            self["shg"]=SHGVar(shg)
            if witherrors:
                self["sigcnm"]=np_float64Var(sigcnm)

    def fsave_impl(self):

        cnm=self["cnm"][:]
        sigcnm=self["sigcnm"][:]
        shg=self["shg"][:]

        tstamps=[0.0,0.0,0.0]
        for i,tag in enumerate(["tstart","tcent","tend"]):
            if tag in self.attr:
                tstamps[i]=datetime2decyear(meta[tag])

        with self.fid() as fid:
            fid.write("META   %d %f %f %f\n"%(shg.nmax,tstamps[0],tstamps[1],tstamps[2]))


        for val in enumerate(shg):
            print(val)
        # idxsorted=sorted(enumerate(shg),key=lambda val:val[1])

        if sigcnm.isSet():
            lineval=[0.0,0.0,0.0,0.0]
            for i,(n,m,t) in idxsorted:
                lineval[t]=cnm[i]
                lineval[2+t]=sigcnm[i]
                if t == trig.s:
                    #print out line
                    fid.write("%d %d %e %e %e %e\n"%(n,m,*lineval))
        else:
            lineval=[0.0,0.0]
            for i,(n,m,t) in idxsorted:
                lineval[t]=cnm[i]
                if t == trig.s:
                    #print out line
                    fid.write("%d %d %e %e\n"%(n,m,*lineval[0:2]))


# def write_shstandard(file,idx, shcoef,sherr=None,meta=None):
#     """Write a dataset of spherical harmonic coeficients to a 'standard' sh file"""
#
#     #find the  maximum degree
#     nmax=max(idx,key=operator.itemgetter(0))[0]
#     # import pdb;pdb.set_trace()
#     #file order is  according to SHnmtGuide
#     shg=SHnmtGuide(nmax)
#     idxsorted=sorted(enumerate(idx),key=lambda val:shg.idx(val[1]))
#
#     tstamps=[0.0,0.0,0.0]
#     if meta:
#         for i,tag in enumerate(["tstart","tcent","tend"]):
#             if tag in meta:
#                 tstamps[i]=datetime2decyear(meta[tag])
#
#     #create an index vector which is used to fix the
#     with open(file,'wt') as fid:
#         fid.write("META   %d %f %f %f\n"%(nmax,tstamps[0],tstamps[1],tstamps[2]))
#
#         if sherr:
#             lineval=[0.0,0.0,0.0,0.0]
#             for i,(n,m,t) in idxsorted:
#                 lineval[t]=shcoef[i]
#                 lineval[2+t]=sherr[i]
#                 if t == trig.s:
#                     #print out line
#                     fid.write("%d %d %e %e %e %e\n"%(n,m,*lineval))
#         else:
#             lineval=[0.0,0.0]
#             for i,(n,m,t) in idxsorted:
#                 lineval[t]=shcoef[i]
#                 if t == trig.s:
#                     #print out line
#                     fid.write("%d %d %e %e\n"%(n,m,*lineval[0:2]))
