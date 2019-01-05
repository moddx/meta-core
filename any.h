//
// Created by Matthias Ervens on 16/11/19.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

/*
 * This header is no longer in use.
 *
 * Previously it provided an abstraction between std::any and boost::any.
 * Using this header file, both can be used interchangeably via any::any.
 */

#ifndef ANY
#if 0 //__cplusplus == 201703L

#include <any>
namespace any = std;

#else

#include <boost/any.hpp>
namespace any = boost;

#endif
#endif