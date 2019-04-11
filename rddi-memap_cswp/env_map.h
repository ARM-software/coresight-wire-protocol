// env_map.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file env_map.h
 *  \brief Index dummy environments by RDDI handle.
 */
#ifndef INC_ENV_MAP_H
#define INC_ENV_MAP_H

class Env;
#include "rddi.h"

#include <boost/shared_ptr.hpp>

namespace EnvironmentMap
{
    typedef boost::shared_ptr<Env> EnvPtr;

    EnvPtr Lookup(RDDIHandle rddi);
    RDDIHandle Store(EnvPtr newEnv);
    void Remove(RDDIHandle rddi);
}

#endif // INC_ENV_MAP_H
