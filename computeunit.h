//
// Created by Matthias Ervens on 06/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include "dataset.h"

namespace META { namespace Core {

    class ComputeUnit {
    public:
        virtual ComputeData* compute(ComputeData *data) const = 0;
    };

}}
