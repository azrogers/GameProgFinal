#pragma once
#include "ECS/Object.hpp"
#ifdef ENABLE_STATUS_REPORTER
#include <microhttpd.h>

namespace eg
{
	/**
	 * Reports engine status via HTTP.
	 */
	class StatusReporter
	{
	public:
		StatusReporter(int port);
		~StatusReporter();
		void Update(eg::Object* scene);
	private:
		eg::Object* scene;
		struct MHD_Daemon* daemon;

		static int answerRequest(
			void *cls, 
			struct MHD_Connection* connection, 
			const char *url, 
			const char *method, 
			const char *version, 
			const char *uploadData, 
			size_t* uploadDataSize, 
			void **conCls);
	};
};
#endif