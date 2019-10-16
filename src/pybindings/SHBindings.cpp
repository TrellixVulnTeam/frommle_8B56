#include <iostream>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include "pybindings/coreBindings.hpp"
#include "core/GuideBase.hpp"

namespace p = boost::python;
namespace np = boost::python::numpy;

#include "sh/SHGuide.hpp"
#include "sh/Legendre_nm.hpp"
#include "../sh/SHGuide.hpp"
#include "../core/GuideBase.hpp"
#include "coreBindings.hpp"
#include "tupleconversion.hpp"


namespace frommle{
    namespace guides{

//Wrapper class is needed to cope with pure virtual functions of SHGuideBase
        struct SHGuideBaseWrapper : SHGuideBase, p::wrapper<SHGuideBase> {
        public:
            using SHGuideBase::idx;
            using Element=SHGuideBase::Element;
            using trig=SHGuideBase::trig;

//            frommle::core::index   (SHGuideBaseWrapper::*idxfromEl)(const Element) const = &SHGuideBaseWrapper::idx;
//            frommle::core::index   (sh::SHGuideBase::*idxfromnmt)(const int,const int, const trig ) const = &sh::SHGuideBase::idx;
//            inline frommle::core::index   idxfromEl(const sh::SHGuideBase::Element El)const{ return this->idx(El);}
            inline size_t idxfromnmt(const int n, const int m, const SHGuideBase::trig t) const { return this->idx(n, m, t); }

            inline size_t idx(const int n, const int m, const SHGuideBase::trig t) const {
                return this->get_override("idx")(n, m, t);
            }

            inline size_t idx(const Element el) const {
                return this->SHGuideBase::idx(el);
            }

            Element operator[](const size_t idx) const {
                return this->get_override("operator[]")(idx);
            }
        };
    }
}


using namespace frommle::guides;

void pyexport_sh()
{


    p::enum_<SHGuideBase::trig>("trig")
            .value("c",SHGuideBase::trig::C)
            .value("s",SHGuideBase::trig::S);

    frommle::py::register_tuple<SHGuideBase::Element >();

    //the SHGuideBase
    p::class_<SHGuideBaseWrapper,p::bases<GuideBase>,boost::noncopyable>("SHGuideBase")
            .add_property("nmax",&SHGuideBase::nmax)
            .add_property("nmin",&SHGuideBase::nmin)
            .def("idx",&SHGuideBase::idxfromEl)
            .def("__getitem__",p::pure_virtual(&SHGuideBase::operator[]))
            .def("__iter__",p::iterator<const SHGuideBase>());

    //export subclass SHtmnGuide
    p::class_<SHtmnGuide,p::bases<SHGuideBase> >("SHtmnGuide",p::init<int,p::optional<std::string>>());
            //.def("i_from_mn",&sh::SHtmnGuide::i_from_mn).staticmethod("i_from_mn")
            //.def("mn_from_i",&sh::SHtmnGuide::mn_from_i).staticmethod("mn_from_i");

    //export subclass SHnmtGuide
    p::class_<SHnmtGuide,p::bases<SHGuideBase> >("SHnmtGuide",p::init<int,p::optional<std::string>>())
            .def("i_from_nmt",&SHnmtGuide::i_from_nmt).staticmethod("i_from_nmt");


    p::def("nmax_from_sz",&nmax_from_sz);

//    p::class_<sh::Legendre_nm_d>("Legendre_nm",p::init<int>())
//            .def("__call__",&sh::Legendre_nm_d::operator())
//            .def("indxnm", &sh::Legendre_nm_d::indxnm);

}
