/*
 * UccRequestHandler.h
 *
 *  Created on: Jan 13, 2015
 *      Author: root
 */

#ifndef UCCREQUESTHANDLER_H_
#define UCCREQUESTHANDLER_H_

#include <Wt/Http/Client>
#include <Wt/Http/Response>
#include <string>

using namespace std;

class UccRequestHandler {
	string locationType;
	const Wt::Http::Request& request;
	Wt::Http::Response& response;
	void handleRequestIP(const Wt::Http::Request& request,
			Wt::Http::Response& response);
	void handleRequestGeo(const Wt::Http::Request& request,
			Wt::Http::Response& response);
public:
	UccRequestHandler(const Wt::Http::Request& request,
			Wt::Http::Response& response);
	void handleRequest();
};

#endif /* UCCREQUESTHANDLER_H_ */
