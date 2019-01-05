//
// Created by Matthias Ervens on 01/01/19.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#include <iostream>
#include "arraydatasource.h"

namespace META { namespace Core {

    ArrayDataSource::ArrayDataSource() : size(0) {
        this->data = new DataSet[this->size];
        this->results = new DataSet[this->size];
    }

    ArrayDataSource::ArrayDataSource(DataSet data[], int size) : size(size) {
        this->data = data;
        this->results = new DataSet[size];
    }

    ArrayDataSource::ArrayDataSource(ComputeData* data[], int size) : size(size) {
        this->data = new DataSet[size];
        for (int i = 0; i < size; i++) {
            this->data[i] = DataSet(i, data[i]);
        }
        allocatedDataArray = true;

        this->results = new DataSet[size];
    }

    ArrayDataSource::ArrayDataSource(std::vector<DataSet> datasets)
            : size(static_cast<const int>(datasets.size())) {
        this->data = new DataSet[this->size];
        this->results = new DataSet[this->size];

        std::move(datasets.begin(), datasets.end(), data);
    }

    ArrayDataSource::~ArrayDataSource() {
        if(allocatedDataArray)
            delete[] this->data;
        delete[] this->results;
    }

    bool ArrayDataSource::hasNextElement() const {
        return this->curDataIdx < this->size;
    }

    // passes the element by reference
    // returns true on success
    bool ArrayDataSource::getNextElement(DataSet &pOutDataSet) {
        std::lock_guard<std::mutex> lock(dataMutex);    // locks data for scope
        if (!this->hasNextElement())
            return false;
        pOutDataSet = this->data[curDataIdx++];
        return true;
    }

    bool ArrayDataSource::hasNextResult() const {
        return curResultsIdx < availResults;
    }

    // passes the result by reference
    // returns true on success
    bool ArrayDataSource::getNextResult(DataSet &pOutResult) {
        std::lock_guard<std::mutex> lock(resultsMutex);    // locks results for scope
        if (!this->hasNextResult())
            return false;
        pOutResult = this->results[curResultsIdx++];
        return true;
    }

    bool ArrayDataSource::storeNextResult(DataSet result) {
        std::lock_guard<std::mutex> lock(resultsMutex);    // locks results for scope
        if (availResults >= this->size)
            return false;
        this->results[availResults++] = move(result);
        return true;
    }

    bool ArrayDataSource::hasAllResults() {
        return availResults == this->size;
    }

    bool ArrayDataSource::getSetOfElements(DataSet *pOutData, int size) {
        std::lock_guard<std::mutex> lock(dataMutex);

        if(!hasNextElement())
            return false;

        int num_taken = 0;
        while(num_taken < size) {
            pOutData[num_taken++] = hasNextElement()
                    ? this->data[curDataIdx++] : DataSet();
        }

        return true;
    }

    bool ArrayDataSource::appendSetOfResults(DataSet *pInResults, int size) {
        std::lock_guard<std::mutex> lock(resultsMutex);

        if(curResultsIdx + size > this->size)
            return false;

        for(int i = 0; i < size; i++) {
            this->results[curResultsIdx++] = pInResults[i];
        }

        return true;
    }

    void ArrayDataSource::getResults(META::Core::DataSet *pOutResults) {
        std::move(this->results, this->results + size, pOutResults);
    }

    void ArrayDataSource::markFinished() {
        std::unique_lock<std::mutex> lock(finishedMutex);

        std::cout << "Marking DataSource finished.\n";
        finishedCond.notify_all();
    }

    void ArrayDataSource::waitUntilFinished() {
        std::unique_lock<std::mutex> lock(finishedMutex);
        while(!hasAllResults()) {
            std::cout << "DataSource has not yet finished. Keeping on waiting.\n";
            finishedCond.wait(lock);
        }
    }

}}