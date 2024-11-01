#pragma once

#include <hal/fs/File.hpp>
#include <util/ErrorChain.hpp>
#include <etl/expected.h>
#include <etl/vector.h>
#include "Partition.hpp"

namespace hal::disk
{

class PartitioningScheme
{
public:
    virtual bool Probe(hal::fs::File* device, ErrorChain& err) = 0;
    virtual void GetPartitions(hal::fs::File* device, etl::ivector<Partition>& outPartitions, ErrorChain& err) = 0;
};

}