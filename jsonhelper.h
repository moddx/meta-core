//
// Created by Matthias Ervens on 03/12/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#pragma once

#include <json.hpp>

#include <dataset.h>

#include <string>
#include <vector>

using namespace std;
using namespace nlohmann;

namespace META { namespace Core {

class JsonHelper {
public:
    static string toJson(const DataSet *sets, const int size, const string &apikey);
    static string toJson(vector<DataSet> sets, const string &apikey);
    static string fromJson(const string jsonIn, vector<DataSet> &setsOut);
    static void fromJson(const string jsonIn, vector<DataSet> &setsOut, string &apikeyOut);
};

}}

