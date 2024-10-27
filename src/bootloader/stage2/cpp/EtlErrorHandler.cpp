#include "EtlErrorHandler.hpp"
#include <etl/error_handler.h>
#include <core/Debug.hpp>

void EtlErrorHandler(const etl::exception& e)
{
    Debug::Error("ETL ERROR: %s in %s:%d", e.what(), e.file_name(), e.line_number());
}

void InstallEtlErrorHandler()
{
    etl::error_handler::set_callback<EtlErrorHandler>();
}
