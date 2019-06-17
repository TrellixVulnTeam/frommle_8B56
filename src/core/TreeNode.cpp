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

#include "core/TreeNode.hpp"
#include "TreeNode.hpp"

namespace frommle {
    namespace core {


        TreeNodeRef::cvec::const_iterator
        TreeNodeRef::cbegin() const {
            if (!ptr_->isCollection()) {
                throw MethodException("Can only iterate over a TreeNodeItem");

            }

            return static_cast<const TreeNodeCollection *>(ptr_.get())->cbegin();
        }

        TreeNodeRef::cvec::const_iterator
        TreeNodeRef::cend() const {
            if (!ptr_->isCollection()) {
                throw MethodException("Can only iterate over a TreeNodeItem");

            }
            return static_cast<const TreeNodeCollection *>(ptr_.get())->cend();
        }

        TreeNodeRef::cvec::iterator
        TreeNodeRef::begin() {
            if (!ptr_->isCollection()) {
                throw MethodException("Can only iterate over a TreeNodeCollection");

            }
            return static_cast<TreeNodeCollection *>(ptr_.get())->begin();
        }

        TreeNodeRef::cvec::iterator
        TreeNodeRef::end() {
            if (!ptr_->isCollection()) {
                throw MethodException("Can only iterate over an TreeNodeCollection");

            }
            return static_cast<TreeNodeCollection *>(ptr_.get())->end();

        }

        const TreeNodeRef &TreeNodeRef::operator=(TreeNodeRef &&in) {
            if (!ptr_) {
                //quick shortcut if current TreeNode is informationless
                ptr_=std::move(in.ptr_);
                return *this;
            }


            std::string iname = ptr_->getName();
            auto currentParent = ptr_->getParent();

            if (currentParent) {
                ptr_ = currentParent->convertChild(std::move(in)).ptr_;
                //also inherit the parent from the current TreeNodeRef
                ptr_->setParent(currentParent);
            }

            if (!iname.empty()) {
                //Note existing name takes precedence so here we put it back in
                ptr_->setName(iname);
            }

            return *this;
        }

        const TreeNodeRef &TreeNodeRef::operator=(TreeNodeRef &in) {
            if (!ptr_) {
                //quick shortcut if current TreeNode is informationless
                ptr_=std::move(in.ptr_);
                return *this;
            }
            std::string iname=ptr_->getName();
            auto currentParent=ptr_->getParent();

            if(currentParent) {
                //possibly modify the underlying type so it can work together with the parent
                ptr_=currentParent->convertChild(TreeNodeRef(in)).ptr_;
                //also let the input.ptr point to the same ptr_
                in.ptr_=ptr_;
                //also inherit the parent from the current TreeNodeRef
                ptr_->setParent(currentParent);
            }else{
                ptr_=in.ptr_;
            }

            if (!iname.empty()){
                //Note existing name takes precedence so here we put it back in
                ptr_->setName(iname);
            }

            return *this;
        }


        const TreeNodeRef &TreeNodeCollection::operator[](const std::string &name) const {
            auto idx = findidx(name);
            if (idx == -1) {
                throw IndexingException("Cannot access unregistered element by name");
            }
            return collection_[idx];
        }

        const TreeNodeRef &TreeNodeCollection::operator[](const size_t &idx) const {
            if (idx >= collection_.size()) {
                throw IndexingException("Requested index of requested Treenode is larger than collection");
            } else if (!collection_[idx]) {
                throw IndexingException("Requested Treenode has not been properly initialized");
            }
            return collection_[idx];
        }


        ptrdiff_t TreeNodeCollection::findidx(const std::string name) const {
            for (ptrdiff_t i = 0; i < collection_.size(); ++i) {
                if (collection_[i]) {
                    if (collection_[i]->getName() == name) {
                        return i;
                    }
                }
            }
            return -1;
        }

        TreeNodeRef &TreeNodeCollection::operator[](const std::string &name) {
            loadCollection(name);
            auto idx = findidx(name);
            if (idx == -1) {
                return upsertChild(name, TreeNodeItem());
            }
            return collection_[idx];
        }

        TreeNodeRef &TreeNodeCollection::operator[](const size_t &idx) {
            loadCollection(idx);
            if (idx >= collection_.size()) {
                return upsertChild(idx, TreeNodeItem());
            } else if (!collection_[idx]) {
                return upsertChild(idx, TreeNodeItem());
            }
            return collection_[idx];

        }

        void TreeNodeBase::throwMethExcept() const {
            throw MethodException("Cannot call operator [] on this TreeNodeBase");
        }

        ///@brief default just passes the input
        TreeNodeRef TreeNodeBase::convertChild(TreeNodeRef &&in) {
            return std::move(in);
        }


        TreeNodeBase::TreeNodeBase(TreeNodeRef &&in) {
            name_ = in->name_;
            attrib_ = in->attrib_;
            parent_ = in->parent_;
        }

    }
}
