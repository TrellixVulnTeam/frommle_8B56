/*! \file GOperatorDiag.hpp
 \brief 
 \copyright Roelof Rietbroek 2019
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

 Created by Roelof Rietbroek,  2019

 */


#include "core/GOperatorBase.hpp"
#include "core/GArrayDiag.hpp"

#ifndef FROMMLE_DIAGGOPERATOR_HPP
#define FROMMLE_DIAGGOPERATOR_HPP

namespace frommle{
    namespace core{

        ///@brief Holds a diagonal matrix to be used as linear operator
        template<class T>
        class GOperatorDiag:public GOperatorDyn<T,1,1>{
        public:
            using GOp=GOperatorDyn<T,1,1>;
            using GOp::gpo_;
            using typename GOp::gpo_t;
            using eigd=typename Eigen::DiagonalMatrix<T,Eigen::Dynamic,Eigen::Dynamic>;
//            using garr=GArrayDiagDyn<T>;
//            using eigd=typename garr::eigd;
            GOperatorDiag(){}
            GOperatorDiag( guides::GuidePackDyn<1> gpo ):GOp(std::move(gpo)),diag_(gpo_->at(0)->size()){

            }
//            GOperatorDiag(guides::GuidePackDyn<1> gpo, eigd diag ):GOp(gpo),diag_(diag){}

//            virtual

//            ///@brief takes an array with extended dimension as input and applies the forward operator
//            GArrayDyn<T,2> operator()(const GArrayDyn<T,2> & gin){
//
//                //output will have the same gudies as the input
//                GArrayDyn<T,2> gout(gin.gp());
//                fwdOp(gin,gout);
//                return gout;
//            }

            virtual void fwdOp(const GArrayDyn<T,2> & gin, GArrayDyn<T,2> & gout) {
                    //let the eigen library do the math
                    gout.eig()=diag_*gout.eig();
            }




//            garr & diag(){return diag_;}
//            const garr & diag()const{return diag_;}

            eigd & eig(){return diag_;}
            const eigd & eig()const{return diag_;}
//            GOperatorDiag inverse()const{return GOperatorDiag(*gpo_,eig_.inverse());}
        protected:
            eigd diag_{};
        };


    }

}

#endif //FROMMLE_DIAGGOPERATOR_HPP
