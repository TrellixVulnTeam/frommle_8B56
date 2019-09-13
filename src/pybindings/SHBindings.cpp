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


using namespace frommle;
namespace frommle{
    namespace sh {

//Wrapper class is needed to cope with pure virtual functions of SHGuideBase
        struct SHGuideBaseWrapper : sh::SHGuideBase, p::wrapper<sh::SHGuideBase> {
        public:
            using SHGuideBase::idx;
            using Element=SHGuideBase::Element;
            using trig=SHGuideBase::trig;

//            frommle::core::index   (SHGuideBaseWrapper::*idxfromEl)(const Element) const = &SHGuideBaseWrapper::idx;
//            frommle::core::index   (sh::SHGuideBase::*idxfromnmt)(const int,const int, const trig ) const = &sh::SHGuideBase::idx;
//            inline frommle::core::index   idxfromEl(const sh::SHGuideBase::Element El)const{ return this->idx(El);}
            inline frommle::core::index idxfromnmt(const int n, const int m, const SHGuideBase::trig t) const { return this->idx(n, m, t); }

            inline frommle::core::index idx(const int n, const int m, const SHGuideBase::trig t) const {
                return this->get_override("idx")(n, m, t);
            }

            inline frommle::core::index idx(const Element el) const {
                return this->SHGuideBase::idx(el);
            }

            Element operator[](const frommle::core::index idx) const {
                return this->get_override("operator[]")(idx);
            }
        };
    }
}
void pyexport_sh()
{


    p::enum_<sh::SHGuideBase::trig>("trig")
            .value("c",sh::SHGuideBase::trig::C)
            .value("s",sh::SHGuideBase::trig::S);

    py::register_tuple<sh::SHGuideBase::Element >();

    //the SHGuideBase
    p::class_<sh::SHGuideBaseWrapper,p::bases<core::GuideBase>,boost::noncopyable>("SHGuideBase")
            .add_property("nmax",&sh::SHGuideBase::nmax)
            .add_property("nmin",&sh::SHGuideBase::nmin)
            .def("idx",&sh::SHGuideBase::idxfromEl)
            .def("__getitem__",p::pure_virtual(&sh::SHGuideBase::operator[]))
            .def("__iter__",p::iterator<const sh::SHGuideBase>());

    //export subclass SHtmnGuide
    p::class_<sh::SHtmnGuide,p::bases<sh::SHGuideBase> >("SHtmnGuide",p::init<int,p::optional<std::string>>())
            .def("i_from_mn",&sh::SHtmnGuide::i_from_mn).staticmethod("i_from_mn")
            .def("mn_from_i",&sh::SHtmnGuide::mn_from_i).staticmethod("mn_from_i");

    //export subclass SHnmtGuide
    p::class_<sh::SHnmtGuide,p::bases<sh::SHGuideBase> >("SHnmtGuide",p::init<int,p::optional<std::string>>())
            .def("i_from_nmt",&sh::SHnmtGuide::i_from_nmt).staticmethod("i_from_nmt");


    p::def("nmax_from_sz",&sh::nmax_from_sz);

//    p::class_<sh::Legendre_nm_d>("Legendre_nm",p::init<int>())
//            .def("__call__",&sh::Legendre_nm_d::operator())
//            .def("indxnm", &sh::Legendre_nm_d::indxnm);

}
