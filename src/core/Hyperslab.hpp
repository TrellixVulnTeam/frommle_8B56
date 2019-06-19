/*! \file
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
 */


#include <boost/multi_array.hpp>
#ifndef FROMMLE_HYPERSLAB_HPP
#define FROMMLE_HYPERSLAB_HPP
namespace frommle{
    namespace core {

        template<class T>
        class Hyperslab {
        public:
            using ivec=std::vector<size_t>;
            Hyperslab()=default;
            template<size_t ndim>
            Hyperslab(const boost::multi_array_ref<T,ndim> &arr){
                ndim_=ndim;
                cdata_= arr.origin();
                offset_=ivec(ndim_,0);
                stride_=ivec(ndim_,1);
                auto shape=arr.shape();
                count_=ivec(shape,shape+ndim_);

            }
            template<size_t len>
            Hyperslab(const std::array<T,len> &ar){
                ndim_=1;
                cdata_=ar.data();
                offset_=ivec(1,0);
                stride_=ivec(1,1);
                count_=ivec(1,len);

            }

            Hyperslab(const std::vector<size_t> &extents){
                ndim_=extents.size();
                offset_=ivec(ndim_,0);
                stride_=ivec(ndim_,1);
                count_=ivec(&extents[0],&extents[0]+ndim_);
                size_t nvals=std::accumulate(count_.cbegin(),count_.cend(),0);

                //allocate data
                data_=std::shared_ptr<T[]>(new T[nvals]);
            }
        const T* data()const{return cdata_;}
        T* data(){return data_.get();}
        const ivec & extents()const{return count_;}
        private:
            size_t ndim_=0;
//            T *data_ = nullptr;
            const T* cdata_=nullptr;
            std::shared_ptr<T[]> data_{};
            ivec offset_{};
            ivec count_{};
            ivec stride_{};
//            std::vector <size_t> map_{};
        };
    }
}
#endif //FROMMLE_HYPERSLAB_HPP
