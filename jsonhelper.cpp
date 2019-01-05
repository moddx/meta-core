//
// Created by Matthias Ervens on 03/12/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include "jsonhelper.h"

namespace META { namespace Core {


    string JsonHelper::toJson(const DataSet *sets, const int size, const string &apikey) {
        json j;
        j["Id"] = apikey;
        json jdata;
        for(int i = 0; i < size; i++) {
            const DataSet &set = sets[i];
            json item;
            item["id"] = set.id;
            item["value"] = set.value->toString();
            jdata.push_back(item);
        }

        j["data"] = jdata;

        return j.dump();
    }

    string JsonHelper::toJson(const vector<DataSet> sets, const string &apikey) {
        json j;
        j["Id"] = apikey;
        json jdata = json();
        for(const DataSet &set : sets) {
            json item;
            item["id"] = set.id;
            item["value"] = std::string(set.value->toString());
            jdata.push_back(item);
        }

        j["data"] = jdata;

        return j.dump();
    }

    string JsonHelper::fromJson(const string jsonIn, vector<DataSet> &setsOut) {
        string apikey;
        fromJson(jsonIn, setsOut, apikey);
        return apikey;
    }

    void JsonHelper::fromJson(const string jsonIn, vector<DataSet> &setsOut, string &apikeyOut) {
        json j = json::parse(jsonIn);

        apikeyOut = j["Id"].get<string>();

        json jvals = j["data"];
        for (auto &item : jvals) {
            ComputeData *value = ComputeData::new_ComputeData();
            value->fromString(item["value"].get<string>());
            setsOut.emplace_back(item["id"], value);
        }
    }

}}