/*
 * AbstractCommEndPointAddress.h
 *
 * Copyright (C) 2010 by Christoph M�ller. Alle Rechte vorbehalten.
 * Copyright (C) 2006 - 2010 by Visualisierungsinstitut Universitaet Stuttgart. 
 * Alle Rechte vorbehalten.
 */

#ifndef VISLIB_ABSTRACTCOMMENDPOINTADDRESS_H_INCLUDED
#define VISLIB_ABSTRACTCOMMENDPOINTADDRESS_H_INCLUDED
#if (defined(_MSC_VER) && (_MSC_VER > 1000))
#pragma once
#endif /* (defined(_MSC_VER) && (_MSC_VER > 1000)) */
#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(push, off)
#endif /* defined(_WIN32) && defined(_MANAGED) */


#include "vislib/ReferenceCounted.h"
#include "vislib/String.h"


namespace vislib {
namespace net {


    /**
     * This class defines the interface for a network end point address in the
     * VISlib comm channel infrastructure. VISlib comm channels provide an 
     * abstraction of sockets and IB verbs, which is designed to be 
     * interchangeable.
     * 
     * Note for implementors: The AbstractCommEndPointAddress is designed to 
     * work with a reference counting mechanism like Direct 3D. It therefore
     * inherits from ReferenceCounted and has a protected ctor and dtor. 
     * Subclasses should provide static Create() methods which do the necessary
     * address parsing and return a pointer to an object on the heap, which
     * must have been created with C++ new. The Release() method of this class
     * assumes creation with C++ new and releases the object be calling
     * delete once the last reference was released.
     *
     * Rationale: AbstractCommEndPointAddress must support polymorphism for
     * supporting different types of comm channels and address types with the
     * same interface. Therefore, the use of pointers is mandatory. Using
     * reference counting in conjunction with SmartRefs allows for a reasonably
     * safe management of the object life time. The use of SmartRefs is not
     * mandatory as it might not be possible to use objects instead of pointers
     * in any case. Additionally, the pointer could be extracted from a SmartRef
     * anyways.
     */
    class AbstractCommEndPointAddress : public ReferenceCounted {

    public:

        /**
         * Parses a string as a end point address and sets the current
         * object to this address. An exception is thrown in case it was
         * not possible to parse the input string.
         *
         * @param str A string representation of an end point address.
         *
         * @throws vislib::Exception Or derived in case that 'str' could not
         *                           be parsed as an end point address.
         */
        virtual void Parse(const StringA& str) = 0;

        /**
         * Parses a string as a end point address and sets the current
         * object to this address. An exception is thrown in case it was
         * not possible to parse the input string.
         *
         * @param str A string representation of an end point address.
         *
         * @throws vislib::Exception Or derived in case that 'str' could not
         *                           be parsed as an end point address.
         */
        virtual void Parse(const StringW& str) = 0;

        /**
         * Answer a string representation of the address.
         *
         * Note for implementors: The string representation returned by this 
         * method must be compatible with the input strings for the Parse()
         * method, i. e. it must be possible to successfully parse a string
         * returned by this method.
         *
         * @return A string representation of the address.
         */
        virtual StringA ToStringA(void) const = 0;

        /**
         * Answer a string representation of the address.
         *
         * Note for implementors: The string representation returned by this 
         * method must be compatible with the input strings for the Parse()
         * method, i. e. it must be possible to successfully parse a string
         * returned by this method.
         *
         * @return A string representation of the address.
         */
        virtual StringW ToStringW(void) const = 0;

    protected:

        /** Superclass typedef. */
        typedef ReferenceCounted Super;

        /** Ctor. */
        AbstractCommEndPointAddress(void);

        /**
         * Clone 'rhs'.
         *
         * @param rhs The object to be cloned.
         */
        AbstractCommEndPointAddress(const AbstractCommEndPointAddress& rhs);

        /** Dtor. */
        virtual ~AbstractCommEndPointAddress(void);

    };
    
} /* end namespace net */
} /* end namespace vislib */

#if defined(_WIN32) && defined(_MANAGED)
#pragma managed(pop)
#endif /* defined(_WIN32) && defined(_MANAGED) */
#endif /* VISLIB_ABSTRACTCOMMENDPOINTADDRESS_H_INCLUDED */

