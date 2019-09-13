/*! \file
 \brief Holds the base for the Dimension class
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
 */

#include <tuple>
#include "core/GuideBase.hpp"
#include "io/Group.hpp"
#include "seqGenerator.hpp"
#include <boost/algorithm/string.hpp>
#include "core/Exceptions.hpp"
#include "core/GuideAppender.hpp"
#include "core/Logging.hpp"

#ifndef CORE_GUIDEPACK_HPP_
#define CORE_GUIDEPACK_HPP_

namespace frommle{

    namespace core{


///@brief the GuidePackBase provides runtime access to an underlying (templated) Guidepack 
class GuidePackBase: public virtual GauxPureVirt{
    public:
        GuidePackBase(){};
        ~GuidePackBase(){};
        virtual int nDim()const{return 0;}
        using Gvar=GuideRegistry::Gvar;
        using GauxPureVirt::append;
        using iterator=Gvar*;
        using const_iterator=const Gvar*;
        virtual iterator begin()=0;
        virtual iterator end()=0;
        virtual const_iterator begin()const=0;
        virtual const_iterator end()const=0;
        //dynamic access to the underlying guides should be implemented
        //virtual Gvar & operator[](const int i)=0;
        
        virtual Gvar & operator[](const int i)=0;
        virtual const  Gvar & operator[](const int i)const=0;
    
    private:

};

///@brief dynamic GuidePack (holds all possible types,dimension is fixed at compile time)
template<int n>
class GuidePackDyn: public virtual GuidePackBase,public GauxVirtImpl<n>{
    public:
        GuidePackDyn():GauxVirtImpl<n>(this){};
        using Gvar=GuideRegistry::Gvar;
        using GauxVirtImpl<n>::append;
        int nDim()const{return n;}
        Gvar & operator[](const int i)override{return gpar_[i];} 
        const Gvar & operator[](const int i)const override {return gpar_[i];} 
        iterator begin()override{return gpar_.begin();}
        iterator end()override{return gpar_.end();}
        const_iterator begin()const override{ return gpar_.begin();}
        const_iterator end()const override{ return gpar_.end();}
    private:
        std::array<Gvar,n> gpar_{};

};



/*!brief
 * Wraps several guides into a tuple and provide access functions
 * @tparam Guides: a variadic list of guides which spans the dimensions
 */
        template< class ... Guides >
        class GuidePack {
        public:
            static const int ndim=sizeof...(Guides);
            using guides_t=std::tuple<std::shared_ptr<Guides>...>;
            //using GApp=GuideRegisterimpl<GuidePack<Guides...>>;
            //using GApp::append;
            template<int n>
            using g_t=typename std::tuple_element<n,guides_t>::type;
//            using subguides=typename stripfirst<Guides...>::type;
            GuidePack(){}
            GuidePack(Guides&& ... Args){
                extent_={Args.size()...};
//                guides_ = std::make_tuple(std::forward<(std::make_shared<Guides>(std::forward<Guides>(Args)...)...);
                guides_ = std::make_tuple(std::make_shared<Guides>(std::move(Args))...);

            }
            
            //GuidePack(Guides & ... Args){
                //extent_={Args.size()...};
                //guides_ = std::make_tuple(std::make_shared<Guides>((Args))...);

            //}
            
            GuidePack(std::shared_ptr<Guides> & ... Args){
                extent_={Args->size()...};
                guides_ = std::make_tuple(Args ...);

            }
            //template<class PlusG>
            //using GPAppended=GuidePack<Guides...,PlusG>;
            ///@brief append additional Guide to the end of the Guidepack and return the new version
            //template<class PlusG>
            //GPAppended<PlusG> append_impl(std::shared_ptr<PlusG> extraG)const;
            //{
               ////auto GPout=std::make_shared<GuidePack< Guides ... , PlusG>>();
                ////////assign existing Guides
                //////for(int i=0;i<ndim;++i){
                    //////GPout->g(i)=g(i);
                //////}
                ////////append the new one
                //////GPout->g<ndim>=extraG;

                ////return GPout;
            //}

            //template<class PlusG>
            //std::shared_ptr<GuidePack> append_impl(std::shared_ptr<PlusG> extraG)const{
                //return std::make_shared<GuidePack>();
            //}
            template<class Gp>
            GuidePack & operator=(Gp & gpin){
                assert(ndim == gpin.ndim);
                assignGuides<0, Gp>(gpin);
                return *this;
            }


            std::array<size_t,ndim> getExtent()const{
                return extent_;
            };

            long long unsigned int num_elements()const{
                return std::accumulate(extent_.cbegin(),extent_.cend(),0);
            }

            void setNames(const std::string & coordinatenames){
                std::vector<std::string> splits;
                boost::split(splits,coordinatenames,boost::is_any_of(" "));
                for(int i=0;i<splits.size();++i){
                    g(i)->setName(splits[i]);
                }

            }
            size_t size()const{return std::accumulate(extent_.cbegin(),extent_.cend(),0);}

            /*!brief
             * Get the nth guide of the array
             * @tparam n
             * @return a reference to the Guide instance belonging to the Garray
             */
            template<int n>
            typename std::tuple_element<n,guides_t>::type & g() {
                assert(n<=ndim);
                return std::get<n>(guides_);
            }
            template<int n>
            const typename std::tuple_element<n,guides_t>::type & g()const{
                assert(n<=ndim);
                return std::get<n>(guides_);
            }

            /*!brief
             * Polymorphic version of the g() function
             * Recursively calls itself until the correct dimension is founc
             * @tparam n test for this dimension
             * @param i: dimension to retrieve
             * @return a GuideBase
             */
            template<int n=0>
            typename std::enable_if< n+1 != ndim,std::shared_ptr<const GuideBase>>::type  g(const int i)const{
                if (i == n) {
                    return g<n>();
                }else{
                    //call this function recursively
                    return g<n+1>(i);
                }
            }
            /*! brief the function below stops the recursion
             *
             * @tparam n
             */
            template<int n=0>
            typename std::enable_if< n+1 == ndim,std::shared_ptr<const GuideBase>>::type  g(const int i)const{
                assert(i+1==ndim);
                return g<n>();
            }

            //non-const versions

            template<int n=0>
            typename std::enable_if< n+1 != ndim, std::shared_ptr<GuideBase>>::type  g(const int i){
                if (i == n) {
                    return g<n>();
                }else{
                    //call this function recursively
                    return g<n+1>(i);
                }
            }
            /*! brief the function below stops the recursion
             *
             * @tparam n
             */
           template<int n=0>
            typename std::enable_if< n+1 == ndim,std::shared_ptr<GuideBase>>::type  g(const int i){
                assert(i+1==ndim);
                return g<n>();

            }

            template<int i>
            struct maskpack {
                    template<class Gother>
                    using mask_g=typename std::conditional<std::is_same<Gother,typename g_t<i>::element_type>::value,core::MaskedGuide<Gother>,Gother>::type;
                    using type=GuidePack< mask_g<Guides>...>;
            };

            ///@returns a guidepack which has one dimension masked
            template<int i>
            typename maskpack<i>::type mask(){
                typename maskpack<i>::type maskp{};
                //assign guides
                maskp.guides_=guides_;
                return maskp;
            }
        
//            //returns a tuple of boost::index_ranges to be used for creating arrayviews
//            using index_gen=arr::index_gen::gen::type<ndim,ndim>;
//            std::tuple< > indices(){
//                return get_indices<0>();
//            }
            //immplement virtual functions from GuidePackBase
            int  nDim()const{return ndim;}
            //std::shared_ptr<GuideBase> operator[](const int i){return g(i);}
            constGuideBasePtr operator[](const int i)const {return g(i);}
            //GuidePackPtr append(GuideBase & in)const{
                //return append<GuideBase>(in);
            //}
            
        //virtual GuidePackPtr append(Gappend & gpapp){
            //return gpapp(*this);
        //}
            //GPINJECTAPPEND;  

        private:

            //SFINEA templates to count the amount of dimensions which are class X at compiletime
            //template<int n,class X>
            //constexpr std::enable_if<std::is_same<g_t<n>,X>::value,
            

            template<class ...Gs>
            friend class GuidePack;

            friend class io::serialize;
            template<class Archive>
            void load(Archive & Ar){
                loadGuides<Archive,0>(Ar);
            }

            template<class Archive>
            void save(Archive & Ar)const{
                saveGuides<Archive,0>(Ar);            
            
            }
            template<class Archive,int n>
            typename std::enable_if< n+1 != ndim>::type saveGuides(Archive & Ar)const{
                Ar << *(std::get<n>(guides_));
                //also recursively save the remaining guides
                saveGuides<Archive,n+1>(Ar);
            }
            
            template<class Archive,int n>
            typename std::enable_if< n+1 == ndim>::type saveGuides(Archive & Ar)const{
                Ar << *(std::get<n>(guides_));
            }

            template<class Archive,int n>
            typename std::enable_if< n+1 != ndim>::type loadGuides(Archive & Ar){
                Ar >> *(std::get<n>(guides_));
                //also recursively save the remaining guides
                loadGuides<Archive,n+1>(Ar);
            }

            template<class Archive,int n>
            typename std::enable_if< n+1 == ndim>::type loadGuides(Archive & Ar){
                Ar >> *(std::get<n>(guides_));
            }

            template<class G,class Gin>
                static typename std::enable_if<std::is_same< typename G::element_type,typename Gin::element_type>::value>::type assign(G & out, Gin & in){
                    //we don't convert but just add an owner to the shared_ptr
                    out=in;
                }
            
            template<class G,class Gin>
                static typename std::enable_if<!std::is_same<typename G::element_type,typename Gin::element_type>::value>::type assign(G & out, Gin & in){
                    //we convert the value here but just add an owner to the shared_ptr
                    //convert

                    out=std::shared_ptr<typename G::element_type>(new typename G::element_type(*in));
                }

            template<int n,class Gpackother>
            typename std::enable_if< n+1 != ndim >::type assignGuides(Gpackother &gpother){
                    assign(g<n>(),gpother.template g<n>());
                    //also recursively assign the other guides
                assignGuides<n + 1, Gpackother>(gpother);
            }

            template<int n,class Gpackother>
            typename std::enable_if< n+1 == ndim>::type assignGuides(Gpackother &gpother){
                assign(g<n>(),gpother.template g<n>());

            }

            
//            template<int n>
//            typename std::enable_if< n+1 != ndim >::type get_indices()const{
//                //also recursively call
//                return std::get<n>(guides_).index_range() get_indices<n+1>();
//            };
//
//            template<int n>
//            typename std::enable_if< n+1 == ndim>::type get_indices()const{
//                //ending call
//                return std::get<n>(guides_).index_range();
//            };
            

            guides_t guides_{};
            std::array<size_t,ndim> extent_={0};
        };



        
        ////specialization for an empty Guidepack
        //template<>
        //class GuidePack<>:public GuidePackBase,public GuideRegisterimpl<GuidePack<>>{
            //public:
            //int ndim=0;
            //GuidePack(){};
            
            ////template<class PlusG>
            ////using GPAppended=GuidePack<PlusG>;
            /////@brief append additional Guide to the end of the Guidepack and return the new version
            ////template<class PlusG>
            ////GPAppended<PlusG> append_impl(std::shared_ptr<PlusG> extraG)const
            ////{
                //////return std::make_shared<GuidePack<PlusG>>(extraG);
               ////return std::make_shared<GuidePack<PlusG>>(extraG);
            ////}
            ////template<class PlusG>
            ////std::shared_ptr<GuidePack> append_impl(std::shared_ptr<PlusG> extraG)const{
                ////return std::make_shared<GuidePack>();
            ////}
            //private:

        //};

        ////template<class ... Guides>
        ////template<class PlusG>
        ////typename GuidePack<Guides...>::template GPAppended<PlusG> GuidePack<Guides...>::append_impl(std::shared_ptr<PlusG> extraG )const{
                
               ////return std::make_shared<GuidePack<Guides...,PlusG>>();
        ////}

    ////GuidePackPtr makeGuidePack(GuideBase & in);
    

    ////GuidePackPtr makeGuidePack(GuideBase & in,GuideBase & in2);


    }
}

#endif /* _CORE_GUIDEPACK_HPP_ */
