//
// Created by Matthias Ervens on 08/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include "computedata.h"

namespace META { namespace Core {

    struct DataSet {
        DataSet() { id = -1; };

        DataSet(int id, ComputeData *value) : id(id), value(value) {};
        int id;
        ComputeData *value;

        bool operator<(const DataSet &other) const {
            return (id < other.id);
        }

        bool isUninitialized() {
            return id == -1;
        }
    };

    const DataSet No_DataSet;
}}