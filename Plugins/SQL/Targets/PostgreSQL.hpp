#pragma once

#if defined(NWNX_SQL_POSTGRESQL_SUPPORT)

#include <libpq-fe.h>
#include "Targets/ITarget.hpp"

namespace SQL {

class PostgreSQL final : public ITarget
{
public:
	PostgreSQL(NWNXLib::ViewPtr<NWNXLib::Services::LogProxy> log);
    ~PostgreSQL();

    virtual void Connect(NWNXLib::ViewPtr<NWNXLib::Services::ConfigProxy> config) override;
    virtual bool IsConnected() override;
    virtual bool PrepareQuery(const Query& query) override;
    virtual NWNXLib::Maybe<ResultSet> ExecuteQuery() override;
    virtual void PrepareInt(int32_t position, int32_t value) override;
    virtual void PrepareFloat(int32_t position, float value) override;
    virtual void PrepareString(int32_t position, const std::string& value) override;
    virtual int  GetAffectedRows() override;

private:
    NWNXLib::ViewPtr<NWNXLib::Services::LogProxy> m_log;
    int GetParamCount(std::string query);
    PGconn *m_conn;
    int affectedRows = -1;
    size_t paramCount = 0;
    std::vector<std::string> m_params;
};

}

#endif

