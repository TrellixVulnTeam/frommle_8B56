/*! \file
 \brief Definition file for the SHDimension class
 \copyright Roelof Rietbroek 2018
 \license
 This file is part of Frommle.
 frommle is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 Frommle is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with Frommle; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 Created by Roelof Rietbroek, 2018

 */

#include "core/DimensionBase.hpp"
#include <tuple>
#ifndef FROMMLE_SHDIMENSION_HPP
#define FROMMLE_SHDIMENSION_HPP

namespace frommle{
    namespace sh{


        /*! brief returns a vectorized index for order sorted spherical harmonics (no trigonometric variable)
         * @param n input degree
         * @param m input order
         * @param nmax maximum degree to accomodate for
         * @return zero based index of the corresponding entry
         */
        inline size_t i_from_mn(const int n,const int m, const int nmax){
            assert(m<=n);
            return m*(nmax+1)-(m*(m+1))/2+n;
        }

        /*!brief retrieve the degree and order from a index sorted by order and degree (no trig variable
        *
        * @param idx input index (zero based)
        * @param nmax maximum degree for which has been allocated
        * @return a tuple containing the degree and order in the first and second element respectively
        */
        inline std::tuple<int,int> mn_from_i(const size_t idx, const int nmax){
            int m=(3.0+2*nmax)/2-std::sqrt(std::pow(3+2*nmax,2)/4.0-2*idx);
            int n=idx-(((m+1)*(m+2))/2+m*(nmax-m))+m+1;
            assert(m<=n);
            return std::make_tuple(n,m);

        }

        inline int nmax_from_sz(const size_t sz){
            return (std::sqrt(1.0+8*(sz-1))-1.0)/2.0;
        }

        /*!brief
         * SHBasedimension groups all SH harmonic dimensions together
         */
        class SHBaseDimension:public DimensionBase{
        public:
            using frommle::DimensionBase::index;
            using trig=enum {C=0,S=1};
            using nmtpack=std::tuple<int,int,trig>;
            SHBaseDimension(const std::string & type, const index sz,const int nmax,const int nmin):DimensionBase(type,sz),nmax_(nmax),nmin_(nmin){}
            virtual index idx(const int n,const int m,const trig t)const=0;
            index idx(const nmtpack & in){
                int n,m;
                trig t;
                std::tie(n,m,t)=in;
                return idx(n,m,t);
            }
            virtual nmtpack nmt(const index idx)const=0;
        protected:
            int nmax_=0;
            int nmin_=0;
        };

        /*!brief
         * SHmnDim holds a SH sorting scheme which varies quickly in degree and than order
         * The scheme uses the sorting order for Cosines and Sine coefficients and thus
         * includes S_n0 terms (which should be zero)
         */
        class SHmnDim: public SHBaseDimension{
        public:
            using SHBaseDimension::trig;
            using SHBaseDimension::nmtpack;
            using SHBaseDimension::index;
            SHmnDim(const int nmax):SHBaseDimension("SHmnDim",2*i_from_mn(nmax,nmax,nmax),nmax,0){
            };
            index idx(const int n, const int m, const trig t)const{
                index shft=(t==trig::C)?0:size_/2;
                return i_from_mn(n,m,nmax_)+shft;
            }
            nmtpack nmt(const index idx)const{
                int n,m;
                trig t=(idx<size_/2)?trig::C:trig::S;
                std::tie(n,m)=mn_from_i(idx,nmax_);
                return std::make_tuple(n,m,t);
            }
        };

        /*! brief
         *
         *
         */
//        class SHDimension: public frommle::DimensionBase {
//        public:
//            using frommle::DimensionBase::index;
//            SHDimension(const int nmaxin){
//                nmax_=nmaxin;
//            }
//
//            /*
//             *
//             *
//             */
//            index operator[](const nmt & in)const{
//                index idx=(in.n*(in.n+1))/2+in.m+1+in.t*size();
//                return idx;
//            }
//
//            nmt invAt(const index in )const{
//                nmt idx={0,0,trig::C};
//                return idx;
//            }
//
//            index size()const{
//                index sz=nmax_*(nmax_+1);
//                return sz;
//            }
//
//            //Index function to get a specific function from n,m,trig
////            static index nmt(const int n, const int m, const trig cs ){
////                return
////            }
//            //Iterator over most cache localized sequence of degrees,order, trig
//
//        protected:
//        private:
//                int nmax_=0;
//        };

    }
}


#endif //FROMMLE_SHDIMENSION_HPP
