#include "StatusReporter.hpp"
#ifdef ENABLE_STATUS_REPORTER
#include <json.hpp>
#include <string.h>
#include <plog/Log.h>

using json = nlohmann::json;

namespace eg
{
	StatusReporter::StatusReporter(int port)
	{
		daemon = MHD_start_daemon(
			MHD_USE_SELECT_INTERNALLY,
			port, 
			NULL, 
			NULL, 
			&StatusReporter::answerRequest, 
			this, 
			MHD_OPTION_END);
	}

	StatusReporter::~StatusReporter()
	{
		MHD_stop_daemon(daemon);
	}

	void StatusReporter::Update(eg::Object* scene)
	{
		this->scene = scene;
	}

	int StatusReporter::answerRequest(
		void *cls,
		struct MHD_Connection* connection,
		const char *url,
		const char *method,
		const char *version,
		const char *uploadData,
		size_t* uploadDataSize,
		void **conCls)
	{
		LOG_INFO << "http request: " << url;
		StatusReporter* s = (StatusReporter*)cls;
		if(strcmp(url, "/scene.json") == 0)
		{
			json j;
			to_json(j, *s->scene);
			std::string res = j.dump();
			const char* resStr = res.c_str();
			MHD_Response* response = MHD_create_response_from_buffer(
				strlen(resStr),
				(void*)resStr,
				MHD_RESPMEM_MUST_COPY);
			int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
			MHD_destroy_response(response);
			return ret;
		}

		return MHD_NO;
	}
};
#endif