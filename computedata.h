//
// Created by Matthias Ervens on 14/12/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include <string>

namespace META { namespace Core {

    class ComputeData;
    typedef ComputeData* (*new_ComputeData_fp_t)();

    class ComputeData {
    public:
        virtual std::string toString() const { return nullptr; };

        virtual void fromString(std::string s) {};
        static new_ComputeData_fp_t new_ComputeData;
    };

}}