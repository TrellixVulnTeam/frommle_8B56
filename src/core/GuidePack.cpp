/*! \file
 \brief some explicitly instantiated templated functions
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
#include "core/GuidePack.hpp"

///free function to allow streaming the typehas to a stream

namespace frommle{

	namespace guides{
	//class GuidePackBase;
	 //explicitly instantiate a few dynamic guidepacks
	template class GuidePackDyn<0>;
	template class GuidePackDyn<1>;
	template class GuidePackDyn<2>;
	template class GuidePackDyn<3>;
	template class GuidePackDyn<4>;
	template class GuidePackDyn<5>;



	}
}

