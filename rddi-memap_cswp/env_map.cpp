// env_map.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*!
 *  \file env_map.cpp
 *  \brief EnvironmentMap implementation
 */
#include "env_map.h"

#include <limits>
#include <map>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/iterator/counting_iterator.hpp>
using namespace std;
using namespace boost;
using namespace EnvironmentMap;

#include "rddi_ex.h"

namespace
{
    typedef map<RDDIHandle, EnvPtr> EnvMap;
    EnvMap g_envs;
}

EnvironmentMap::EnvPtr EnvironmentMap::Lookup(RDDIHandle traceConn)
{
    EnvMap::const_iterator it(g_envs.find(traceConn));
    if (it == g_envs.end())
    {
        throw RddiEx(RDDI_INVHANDLE);
    }
    return it->second;
}

RDDIHandle EnvironmentMap::Store(EnvPtr newConn)
{
    // Find lowest handle number which does not have an entry in g_envs.
    RDDIHandle firstAvailable(*mismatch(g_envs.begin(),
                                        g_envs.end(), 

                                        // Iterate through integers from 0 in lock-step with map entries.
                                        counting_iterator<RDDIHandle>(0),

                                        // Binary predicate is true if key of current map entry equals current integer.
                                        boost::bind(equal_to<RDDIHandle>(), 
                                                boost::bind(&EnvMap::value_type::first, _1), 
                                                _2)
                                  ).second);

    if (firstAvailable == numeric_limits<RDDIHandle>::max())
    {
        throw RddiEx(RDDI_TOOMANYCONNECTIONS);
    }
    g_envs[firstAvailable] = newConn;
    return firstAvailable;
 }

void EnvironmentMap::Remove(RDDIHandle rddi)
{
    EnvMap::iterator it(g_envs.find(rddi));
    if (it == g_envs.end())
    {
        throw RddiEx(RDDI_INVHANDLE);
    }
    g_envs.erase(it);
}

