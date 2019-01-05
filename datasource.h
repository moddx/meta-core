//
// Created by Matthias Ervens on 08/11/18.
// Copyright (c) 2018 Matthias Ervens. All rights reserved.
//

#ifndef META_DATASOURCE_H
#define META_DATASOURCE_H

#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <vector>

#include "dataset.h"

using namespace std;

namespace META { namespace Core {

    class DataSource {
    public:
        DataSource() = default;
        DataSource(DataSet data[], unsigned long size);
        DataSource(ComputeData* data[], unsigned long size);
        explicit DataSource(std::vector<ComputeData*> data);
        explicit DataSource(std::vector<DataSet> datasets);

        DataSource(const DataSource &that);
        DataSource(DataSource &&that) noexcept;

        DataSource &operator=(const DataSource that);

        bool hasNextElement() const;
        DataSet getNextElement();
        bool hasNextResult() const;
        DataSet getNextResult();
        void appendResult(const DataSet &result);
        void appendResult(vector<DataSet> results);

        // convenience
        bool hasAllResults();
        vector<DataSet> getSetOfElements(unsigned long size);
        vector<ComputeData*> getSortedResults();
        template <typename T>
        void getTypedResults(vector<T*> &resultsOut);
        vector<DataSet> getResults();

        void waitUntilFinished();
        void markFinished();
    private:
        vector<DataSet> data;
        vector<DataSet> results;
        std::mutex dataMutex;
        std::mutex resultsMutex;

        std::mutex finishedMutex;
        std::condition_variable finishedCond;
    };


    template <typename T>
    void DataSource::getTypedResults(vector<T*> &resultsOut){
        static_assert(std::is_base_of<ComputeData, T>::value, "type parameter of this function must derive from ComputeData");

        if(hasNextElement()) return;
        if(!hasAllResults()) return;

        std::lock_guard<std::mutex> lock(resultsMutex);
        sort(this->results.begin(), this->results.end());   // sorts by id

        // extract values - dismiss ids
        transform(this->results.begin(),
                  this->results.end(),
                  back_inserter(resultsOut),
                  [](DataSet set) -> T* {return dynamic_cast<T*>(set.value);}
        );
    }

}}

#endif // META_DATASOURCE_H