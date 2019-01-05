//
// Created by Matthias Ervens on 08/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include "datasource.h"

namespace META { namespace Core {

    DataSource::DataSource(DataSet data[], unsigned long size) {
        this->data = vector<DataSet>(data, data + size);
        this->results = vector<DataSet>();
    }

    DataSource::DataSource(ComputeData* data[], unsigned long size) {
        this->data = vector<DataSet>(size);
        this->results = vector<DataSet>();

        for (int i = 0; i < size; i++) {
            this->data[i] = DataSet(i, data[i]);
        }
    }

    DataSource::DataSource(vector<ComputeData*> data) {
        this->data = vector<DataSet>(data.size());
        this->results = vector<DataSet>();

        for (int i = 0; i < data.size(); i++) {
            this->data[i] = DataSet(i, data[i]);
        }
    }

    DataSource::DataSource(std::vector<DataSet> datasets) {
        this->data = move(datasets);
        this->results = vector<DataSet>();
    }

    DataSource::DataSource(const DataSource &that) {
        this->data = that.data;
        this->results = that.results;
    }

    DataSource::DataSource(DataSource &&that) noexcept {
        this->data = move(that.data);
        this->results = move(that.results);
    }

    DataSource &DataSource::operator=(const DataSource that) {
        this->data = that.data;
        this->results = that.results;
        return *this;
    }



    bool DataSource::hasNextElement() const {
        return !data.empty();
    }

    DataSet DataSource::getNextElement() {
        std::lock_guard<std::mutex> lock(dataMutex);    // locks data for scope
        if (!this->hasNextElement())
            return No_DataSet;
        DataSet set = data.back();
        data.pop_back();
        return set;
    }

    bool DataSource::hasNextResult() const {
        return !results.empty();
    }

    DataSet DataSource::getNextResult() {
        std::lock_guard<std::mutex> lock(resultsMutex);    // locks results for scope
        if (!this->hasNextResult())
            return No_DataSet;
        DataSet result = results.back();
        results.pop_back();
        return result;
    }

    void DataSource::appendResult(const DataSet &result) {
        std::lock_guard<std::mutex> lock(resultsMutex);    // locks results for scope
        this->results.push_back(result);
    }

    void DataSource::appendResult(vector<DataSet> results) {
        std::lock_guard<std::mutex> lock(resultsMutex);
        this->results.insert(this->results.end(), make_move_iterator(results.begin()), make_move_iterator(results.end()));
        results.clear();
    }

    bool DataSource::hasAllResults() {
        return this->results.size() == this->data.capacity();
    }

    vector<DataSet> DataSource::getSetOfElements(unsigned long size) {
        std::lock_guard<std::mutex> lock(dataMutex);

        if(!hasNextElement())
            return vector<DataSet>(0);

        size = (data.size() < size) ? data.size() : size;
        vector<DataSet> elements;

        auto itend = this->data.begin() + size;
        elements.insert(elements.end(), make_move_iterator(this->data.begin()), make_move_iterator(itend));
        this->data.erase(this->data.begin(), itend);

        return elements;
    }

    vector<ComputeData*> DataSource::getSortedResults() {
        if(hasNextElement()) return vector<ComputeData*>(0);
        if(!hasAllResults()) return vector<ComputeData*>(0);

        std::lock_guard<std::mutex> lock(resultsMutex);
        sort(this->results.begin(), this->results.end());   // sorts by id

        // extract values - dismiss ids
        vector<ComputeData*> extractedResults;
        transform(this->results.begin(),
                  this->results.end(),
                  back_inserter(extractedResults),
                  [](DataSet set) -> ComputeData* {return set.value;}
        );

        return extractedResults;
    }

    vector<DataSet> DataSource::getResults() {
        return move(results);
    }

    void DataSource::markFinished() {
        std::unique_lock<std::mutex> lock(finishedMutex);

        std::cout << "Marking DataSource finished.\n";
        finishedCond.notify_all();
    }

    void DataSource::waitUntilFinished() {
        std::unique_lock<std::mutex> lock(finishedMutex);
        while(!hasAllResults()) {
            std::cout << "DataSource has not yet finished. Keeping on waiting.\n";
            finishedCond.wait(lock);
        }
    }

}}