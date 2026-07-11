#pragma once
#include <unordered_map>

namespace workload_weight
{
    inline double projectWeightByCount(int projectCount)
    {
        static const std::unordered_map<int, double> kWeights = {
            {1, 1.0}, {2, 1.3}, {3, 1.6}, {4, 1.9}, {5, 2.2},
            {6, 2.5}, {7, 2.8}, {8, 3.1}, {9, 3.4}, {10, 3.7}
        };

        if (projectCount <= 0)
        {
            return 0.0;
        }

        if (projectCount >= 10)
        {
            return 3.7;
        }

        auto it = kWeights.find(projectCount);
        return (it != kWeights.end()) ? it->second : 0.0;
    }

    inline double githubWeightByCount(int repositoryCount)
    {
        static const std::unordered_map<int, double> kWeights = {
            {1, 1.0}, {2, 1.3}, {3, 1.6}, {4, 1.9}, {5, 2.2},
            {6, 2.5}, {7, 2.8}, {8, 3.1}, {9, 3.4}, {10, 3.7}
        };

        if (repositoryCount <= 0)
        {
            return 0.0;
        }

        if (repositoryCount >= 10)
        {
            return 3.7;
        }

        auto it = kWeights.find(repositoryCount);
        return (it != kWeights.end()) ? it->second : 0.0;
    }
}