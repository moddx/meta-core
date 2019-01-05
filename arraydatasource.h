//
// Created by Matthias Ervens on 01/01/19.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#ifndef META_CORE_ARRAYDATASOURCE_H
#define META_CORE_ARRAYDATASOURCE_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include "dataset.h"

/*
 * Deprecated DataSource class using arrays for DataSet/Result - Storage.
 * Dropped in favor of the more versatile new implementation using vectors.
 *
 * The AbstractDataSource abstract class is not fit to accommodate both a
 * vector and an array implementation. Doing so would eliminate the strengths
 * of the particular implementations.
 */

using namespace std;

namespace META { namespace Core {

class AbstractDataSourceDeprecated {
public:
    virtual bool hasNextElement() const = 0;
    virtual bool getNextElement(DataSet &pOutDataSet) = 0;
    virtual bool hasNextResult() const = 0;
    virtual bool getNextResult(DataSet &pOutResult) = 0;
    virtual bool storeNextResult(DataSet result) = 0;
};

class ArrayDataSource : public AbstractDataSourceDeprecated {
    public:
        // produces an empty DataSource without data or results; size = 0.
        ArrayDataSource();
        ArrayDataSource(DataSet data[], int size);
        ArrayDataSource(ComputeData* data[], int size);
        explicit ArrayDataSource(std::vector<DataSet> datasets);
        ~ArrayDataSource();

        ArrayDataSource(ArrayDataSource &&that) = default;
        ArrayDataSource(const ArrayDataSource &that) = delete;
        ArrayDataSource &operator=(const ArrayDataSource &that) = delete;

        bool hasNextElement() const override;
        bool getNextElement(DataSet &pOutDataSet) override;
        bool hasNextResult() const override;
        bool getNextResult(DataSet &pOutResult) override;
        bool storeNextResult(DataSet result) override;
        // convenience
        bool hasAllResults();
        bool getSetOfElements(DataSet *pOutData, int size);
        bool appendSetOfResults(DataSet *pInResults, int size);
        void getResults(DataSet *pOutResults);

        void waitUntilFinished();
        void markFinished();
        const int size;     // number of elements
    private:
        DataSet *data;      // array
        DataSet *results;   // array
        std::mutex dataMutex;
        std::mutex resultsMutex;
        int curDataIdx = 0;
        int curResultsIdx = 0;
        int availResults = 0;
        bool allocatedDataArray = false;

        std::mutex finishedMutex;
        std::condition_variable finishedCond;
    };

}}

#endif //META_CORE_ARRAYDATASOURCE_H
