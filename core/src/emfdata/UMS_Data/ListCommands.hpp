// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * UMS_Data/ListCommands.hpp
 * Copyright (C) Cátedra SAES-UMU 2010 <andres.senac@um.es>
 *
 * EMF4CPP is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EMF4CPP is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \file ListCommands.hpp
 * \brief The ListCommands class
 * \author Generated file
 * \date 31/03/2011
 */

#ifndef UMS_DATA_LISTCOMMANDS_HPP
#define UMS_DATA_LISTCOMMANDS_HPP

#include <UMS_Data_forward.hpp>
#include <ecorecpp/mapping_forward.hpp>

#include <ecore/EObject.hpp>

/*PROTECTED REGION ID(ListCommands_pre) START*/
// Please, enable the protected region if you add manually written code.
// To do this, add the keyword ENABLED before START.
/*PROTECTED REGION END*/

namespace UMS_Data
{

    /**
     * \class ListCommands
     * \brief Implementation of the ListCommands class
     */
    class ListCommands: public virtual ::ecore::EObject

    {
    public:
        /**
         * \brief The default constructor for ListCommands
         */
        ListCommands();
        /**
         * \brief The destructor for ListCommands
         */
        virtual ~ListCommands();

        /**
         * \brief Internal method
         */
        virtual void _initialize();

        // Operations


        // Attributes

        // References
        /**
         * \brief To get the list of Commands
         * \return A reference to a list of Commands
         **/
        ::ecorecpp::mapping::EList< ::UMS_Data::Command >& getCommands();

        /*PROTECTED REGION ID(ListCommands) START*/
        // Please, enable the protected region if you add manually written code.
        // To do this, add the keyword ENABLED before START.
        /*PROTECTED REGION END*/

        // EObjectImpl
        virtual ::ecore::EJavaObject eGet(::ecore::EInt _featureID,
                ::ecore::EBoolean _resolve);
        virtual void eSet(::ecore::EInt _featureID,
                ::ecore::EJavaObject const& _newValue);
        virtual ::ecore::EBoolean eIsSet(::ecore::EInt _featureID);
        virtual void eUnset(::ecore::EInt _featureID);
        virtual ::ecore::EClass_ptr _eClass();

        /*PROTECTED REGION ID(ListCommandsImpl) START*/
        // Please, enable the protected region if you add manually written code.
        // To do this, add the keyword ENABLED before START.
        /*PROTECTED REGION END*/

    protected:
        // Attributes


        // References

        ::ecorecpp::mapping::out_ptr< ::ecorecpp::mapping::EList<
                ::UMS_Data::Command > > m_Commands;

    };

} // UMS_Data

#endif // UMS_DATA_LISTCOMMANDS_HPP
